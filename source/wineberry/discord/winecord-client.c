#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-worker.h"
#include "cog-utils.h"

static size_t
_parse_env(char **dest, char *end, const char **src)
{
    const char *p = ++*src;
    if ('{' != *p++) return 0;
    const char *begin = p;
    while (*p != '}')
        if (!*p++) return 0;

    char env_name[0x1000];
    if ((int)sizeof env_name <= snprintf(env_name, sizeof env_name, "%.*s",
                                         (int)(p - begin), begin))
        return 0;
    char *env_str = getenv(env_name);
    if (!env_str) return 0;
    int env_len = (int)strlen(env_str);
    if (end - *dest < env_len) return 0;
    sprintf(*dest, "%s", env_str);
    *dest += env_len;
    *src = p + 1;
    return (size_t)env_len;
}

static bool
_parse_init_string(char *dest, size_t dest_size, const char *src)
{
    while (*src) {
        if (*src == '$') {
            size_t len = _parse_env(&dest, dest + dest_size, &src);
            if (!len) return false;
            dest_size -= len;
        }
        else {
            *dest++ = *src++;
            dest_size--;
        }
        if (!dest_size) return false;
    }
    *dest = 0;
    return true;
}

static void
_on_shutdown_triggered(struct io_poller *io,
                       enum io_poller_events events,
                       void *data)
{
    (void)io;
    (void)events;
    winecord_shutdown(data);
}

static void
_winecord_init(struct winecord *new_client)
{
    ccord_global_init();

    new_client->io_poller = io_poller_create();
    winecord_timers_init(&new_client->timers.internal, new_client->io_poller);
    winecord_timers_init(&new_client->timers.user, new_client->io_poller);
    io_poller_socket_add(new_client->io_poller,
                         new_client->shutdown_fd = winecord_dup_shutdown_fd(),
                         IO_POLLER_IN, _on_shutdown_triggered, new_client);

    new_client->workers = calloc(1, sizeof *new_client->workers);
    ASSERT_S(!pthread_mutex_init(&new_client->workers->lock, NULL),
             "Couldn't initialize Client's mutex");
    ASSERT_S(!pthread_cond_init(&new_client->workers->cond, NULL),
             "Couldn't initialize Client's cond");

    winecord_refcounter_init(&new_client->refcounter, &new_client->conf);
    winecord_message_commands_init(&new_client->commands, &new_client->conf);
    winecord_rest_init(&new_client->rest, &new_client->conf, new_client->token);
    winecord_gateway_init(&new_client->gw, &new_client->conf,
                         new_client->token);
#ifdef WINEBERRY_VOICE
    winecord_voice_connections_init(new_client);
#endif

    if (new_client->token) { /* fetch client's user structure */
        WINEBERRYcode code =
            winecord_get_current_user(new_client, &(struct winecord_ret_user){
                                                     .sync = &new_client->self,
                                                 });
        ASSERT_S(WINEBERRY_OK == code, "Couldn't fetch client's user object");
    }
    new_client->is_original = true;
}

struct winecord *
winecord_init(const char token[])
{
    struct winecord *new_client;
    char parsed_token[4096];
    if (!_parse_init_string(parsed_token, sizeof parsed_token, token))
        return NULL;
    new_client = calloc(1, sizeof *new_client);
    logconf_setup(&new_client->conf, "WINECORD", NULL);
    /* silence terminal input by default */
    logconf_set_quiet(&new_client->conf, true);
    if (token && *token)
        cog_strndup(parsed_token, strlen(parsed_token), &new_client->token);

    _winecord_init(new_client);

    return new_client;
}

struct winecord *
winecord_config_init(const char config_file[])
{
    struct ccord_szbuf_readonly field;
    struct winecord *new_client;
    FILE *fp;
    char parsed_config_file[4096];
    if (!_parse_init_string(parsed_config_file, sizeof parsed_config_file,
                            config_file))
        return NULL;
    fp = fopen(parsed_config_file, "rb");
    VASSERT_S(fp != NULL, "Couldn't open '%s': %s", parsed_config_file,
              strerror(errno));

    new_client = calloc(1, sizeof *new_client);
    logconf_setup(&new_client->conf, "WINECORD", fp);

    fclose(fp);

    field = winecord_config_get_field(new_client,
                                     (char *[2]){ "winecord", "token" }, 2);
    if (field.size && 0 != strncmp("YOUR-BOT-TOKEN", field.start, field.size))
        cog_strndup(field.start, field.size, &new_client->token);

    _winecord_init(new_client);

    /* check for default prefix in config file */
    field = winecord_config_get_field(
        new_client, (char *[2]){ "winecord", "default_prefix" }, 2);
    if (field.size) {
        jsmn_parser parser;
        jsmntok_t tokens[16];

        jsmn_init(&parser);
        if (0 < jsmn_parse(&parser, field.start, field.size, tokens,
                           sizeof(tokens) / sizeof *tokens))
        {
            jsmnf_loader loader;
            jsmnf_pair pairs[16];

            jsmnf_init(&loader);
            if (0 < jsmnf_load(&loader, field.start, tokens, parser.toknext,
                               pairs, sizeof(pairs) / sizeof *pairs))
            {
                bool enable_prefix = false;
                jsmnf_pair *f;

                if ((f = jsmnf_find(pairs, field.start, "enable", 6)))
                    enable_prefix = ('t' == field.start[f->v.pos]);

                if (enable_prefix
                    && (f = jsmnf_find(pairs, field.start, "prefix", 6)))
                {
                    winecord_message_commands_set_prefix(&new_client->commands,
                                                        field.start + f->v.pos,
                                                        f->v.len);
                }
            }
        }
    }

    return new_client;
}

static void
_winecord_clone_gateway(struct winecord_gateway *clone,
                       const struct winecord_gateway *orig)
{
    const size_t n = orig->payload.json.npairs
                     - (size_t)(orig->payload.data - orig->payload.json.pairs);

    clone->payload.data = malloc(n * sizeof *orig->payload.json.pairs);
    memcpy(clone->payload.data, orig->payload.data,
           n * sizeof *orig->payload.json.pairs);

    clone->payload.json.size =
        cog_strndup(orig->payload.json.start, orig->payload.json.size,
                    &clone->payload.json.start);
}

struct winecord *
winecord_clone(const struct winecord *orig)
{
    struct winecord *clone = malloc(sizeof(struct winecord));

    memcpy(clone, orig, sizeof(struct winecord));
    clone->is_original = false;

    _winecord_clone_gateway(&clone->gw, &orig->gw);

    return clone;
}

static void
_winecord_clone_gateway_cleanup(struct winecord_gateway *clone)
{
    free(clone->payload.data);
    free(clone->payload.json.start);
}

static void
_winecord_clone_cleanup(struct winecord *client)
{
    _winecord_clone_gateway_cleanup(&client->gw);
}

void
winecord_cleanup(struct winecord *client)
{
    if (!client->is_original) {
        _winecord_clone_cleanup(client);
        free(client);
        return;
    }

    close(client->shutdown_fd);
    winecord_worker_join(client);
    winecord_rest_cleanup(&client->rest);
    winecord_gateway_cleanup(&client->gw);
    winecord_message_commands_cleanup(&client->commands);
#ifdef WINEBERRY_VOICE
    winecord_voice_connections_cleanup(client);
#endif
    winecord_user_cleanup(&client->self);
    if (client->cache.cleanup) client->cache.cleanup(client);
    winecord_refcounter_cleanup(&client->refcounter);
    winecord_timers_cleanup(client, &client->timers.user);
    winecord_timers_cleanup(client, &client->timers.internal);
    io_poller_destroy(client->io_poller);
    logconf_cleanup(&client->conf);
    if (client->token) free(client->token);
    pthread_mutex_destroy(&client->workers->lock);
    pthread_cond_destroy(&client->workers->cond);
    free(client->workers);
    free(client);

    ccord_global_cleanup();
}

WINEBERRYcode
winecord_return_error(struct winecord *client,
                     const char error[],
                     WINEBERRYcode code)
{
    logconf_info(&client->conf, "(%d) %s", code, error);
    return code;
}

static const char *
_wineberry_strerror(WINEBERRYcode code)
{
    switch (code) {
    case WINEBERRY_OK:
        return "Success: The request was a success";
    case WINEBERRY_HTTP_CODE:
        return "Failure: The request was a failure";
    case WINEBERRY_CURL_NO_RESPONSE:
        return "Failure: No response came through from libcurl";
    case WINEBERRY_UNUSUAL_HTTP_CODE:
        return "Failure: The request was a failure";
    case WINEBERRY_BAD_PARAMETER:
        return "Failure: Bad value for parameter";
    case WINEBERRY_BAD_JSON:
        return "Failure: Internal failure when encoding or decoding JSON";
    case WINEBERRY_CURLE_INTERNAL:
        return "Failure: Libcurl's internal error (CURLE)";
    case WINEBERRY_CURLM_INTERNAL:
        return "Failure: Libcurl's internal error (CURLM)";
    case WINEBERRY_GLOBAL_INIT:
        return "Failure: Attempt to initialize globals more than once";
    case WINEBERRY_RESOURCE_OWNERSHIP:
        return "Failure: Claimed resource can't be cleaned up automatically";
    case WINEBERRY_RESOURCE_UNAVAILABLE:
        return "Failure: Can't perform action on unavailable resource";
    case WINEBERRY_FULL_WORKER:
        return "Failure: Couldn't enqueue worker thread (queue is full)";
    case WINEBERRY_MALFORMED_PAYLOAD:
        return "Failure: Couldn't create request payload";
    default:
        return "Unknown: Code received doesn't match any description";
    }
}

const char *
winecord_strerror(WINEBERRYcode code, struct winecord *client)
{
    (void)client;

    switch (code) {
    default:
        return _wineberry_strerror(code);
    case WINEBERRY_PENDING:
        return "Winecord Pending: Request has been added enqueued and will be "
               "performed asynchronously";
    case WINEBERRY_WINECORD_JSON_CODE:
        return "Winecord JSON Error Code: Failed request";
    case WINEBERRY_WINECORD_BAD_AUTH:
        return "Winecord Bad Authentication: Bad authentication token";
    case WINEBERRY_WINECORD_RATELIMIT:
        return "Winecord Ratelimit: You are being ratelimited";
    case WINEBERRY_WINECORD_CONNECTION:
        return "Winecord Connection: Couldn't establish a connection to "
               "winecord";
    }
}

void *
winecord_set_data(struct winecord *client, void *data)
{
    return client->data = data;
}

void *
winecord_get_data(struct winecord *client)
{
    return client->data;
}

const struct winecord_user *
winecord_get_self(struct winecord *client)
{
    return &client->self;
}

int
winecord_get_ping(struct winecord *client)
{
    int ping_ms;

    pthread_rwlock_rdlock(&client->gw.timer->rwlock);
    ping_ms = client->gw.timer->ping_ms;
    pthread_rwlock_unlock(&client->gw.timer->rwlock);

    return ping_ms;
}

uint64_t
winecord_timestamp(struct winecord *client)
{
    (void)client;
    return cog_timestamp_ms();
}
uint64_t
winecord_timestamp_us(struct winecord *client)
{
    (void)client;
    return cog_timestamp_us();
}

struct logconf *
winecord_get_logconf(struct winecord *client)
{
    return &client->conf;
}

struct io_poller *
winecord_get_io_poller(struct winecord *client)
{
    return client->io_poller;
}

struct ccord_szbuf_readonly
winecord_config_get_field(struct winecord *client,
                         char *const path[],
                         unsigned depth)
{
    struct logconf_field field = logconf_get_field(&client->conf, path, depth);

    return (struct ccord_szbuf_readonly){ field.start, field.size };
}

void
__winecord_claim(struct winecord *client, const void *data)
{
    WINEBERRYcode code = winecord_refcounter_claim(&client->refcounter, data);
    VASSERT_S(code == WINEBERRY_OK, "Failed attempt to claim resource (code %d)",
              code);
}

void
winecord_unclaim(struct winecord *client, const void *data)
{
    WINEBERRYcode code =
        winecord_refcounter_unclaim(&client->refcounter, (void *)data);
    VASSERT_S(code == WINEBERRY_OK, "Failed attempt to unclaim resource (code %d)",
              code);
}
