#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* isspace() */

#include "winecord.h"
#include "winecord-internal.h"

#define CHASH_KEY_FIELD     command
#define CHASH_VALUE_FIELD   callback
#define CHASH_BUCKETS_FIELD entries
#include "chash.h"

#define _key_hash(key, hash)                                                  \
    5031;                                                                     \
    do {                                                                      \
        unsigned __CHASH_HINDEX;                                              \
        for (__CHASH_HINDEX = 0; __CHASH_HINDEX < (key).size;                 \
             ++__CHASH_HINDEX) {                                              \
            (hash) = (((hash) << 1) + (hash)) + (key).start[__CHASH_HINDEX];  \
        }                                                                     \
    } while (0)

/* compare jsmnf keys */
#define _key_compare(cmp_a, cmp_b)                                            \
    ((cmp_a).size == (cmp_b).size                                             \
     && !strncmp((cmp_a).start, (cmp_b).start, (cmp_a).size))

/* chash heap-mode (auto-increase hashtable) */
#define COMMANDS_TABLE_HEAP              1
#define COMMANDS_TABLE_BUCKET            struct _winecord_message_commands_entry
#define COMMANDS_TABLE_FREE_KEY(_key)    free((_key).start)
#define COMMANDS_TABLE_HASH(_key, _hash) _key_hash(_key, _hash)
#define COMMANDS_TABLE_FREE_VALUE(_value)
#define COMMANDS_TABLE_COMPARE(_cmp_a, _cmp_b) _key_compare(_cmp_a, _cmp_b)
#define COMMANDS_TABLE_INIT(entry, _key, _value)                              \
    chash_default_init(entry, _key, _value)

struct _winecord_message_commands_entry {
    /** message command */
    struct ccord_szbuf command;
    /** the callback assigned to the command */
    winecord_ev_message callback;
    /** the route state in the hashtable (see chash.h 'State enums') */
    int state;
};

void
winecord_message_commands_init(struct winecord_message_commands *cmds,
                              struct logconf *conf)
{
    __chash_init(cmds, COMMANDS_TABLE);

    logconf_branch(&cmds->conf, conf, "WINECORD_MESSAGE_COMMANDS");

    cmds->fallback = NULL;
    memset(&cmds->prefix, 0, sizeof(cmds->prefix));
}

void
winecord_message_commands_cleanup(struct winecord_message_commands *cmds)
{
    if (cmds->prefix.start) free(cmds->prefix.start);
    __chash_free(cmds, COMMANDS_TABLE);
}

winecord_ev_message
winecord_message_commands_find(struct winecord_message_commands *cmds,
                              const char command[],
                              size_t length)
{
    struct ccord_szbuf key = { (char *)command, length };
    winecord_ev_message callback = NULL;
    int ret;

    ret = chash_contains(cmds, key, ret, COMMANDS_TABLE);
    if (ret) {
        callback = chash_lookup(cmds, key, callback, COMMANDS_TABLE);
    }

    return callback;
}

void
winecord_message_commands_append(struct winecord_message_commands *cmds,
                                const char command[],
                                size_t length,
                                winecord_ev_message callback)
{
    /* define callback as a fallback callback if prefix is detected, but
     *      command isn't specified */
    if (cmds->prefix.size && !length) {
        cmds->fallback = callback;
    }
    else {
        struct ccord_szbuf key;

        key.size = cog_strndup(command, length, &key.start);
        chash_assign(cmds, key, callback, COMMANDS_TABLE);
    }
}

void
winecord_message_commands_set_prefix(struct winecord_message_commands *cmds,
                                    const char prefix[],
                                    size_t length)
{
    if (cmds->prefix.start) free(cmds->prefix.start);

    cmds->prefix.size = cog_strndup(prefix, length, &cmds->prefix.start);
}

static void
_winecord_message_cleanup_v(void *p_message)
{
    winecord_message_cleanup(p_message);
    free(p_message);
}

/** return true in case user command has been triggered */
bool
winecord_message_commands_try_perform(struct winecord_message_commands *cmds,
                                     struct winecord_gateway_payload *payload)
{
    jsmnf_pair *f;

    if (!(f = jsmnf_find(payload->data, payload->json.start, "content", 7)))
        return false;

    if (cmds->length
        && !strncmp(cmds->prefix.start, payload->json.start + f->v.pos,
                    cmds->prefix.size))
    {
        struct winecord *client = CLIENT(cmds, commands);
        struct winecord_message *event_data = calloc(1, sizeof *event_data);
        winecord_ev_message callback = NULL;
        struct ccord_szbuf command;
        char *tmp;

        winecord_message_from_jsmnf(payload->data, payload->json.start,
                                   event_data);

        command.start = event_data->content + cmds->prefix.size;
        command.size = strcspn(command.start, " \n\t\r");

        tmp = event_data->content;

        /* match command to its callback */
        if (!(callback = winecord_message_commands_find(cmds, command.start,
                                                       command.size)))
        {
            /* couldn't match command to callback, get fallback if available */
            if (!cmds->prefix.size || !cmds->fallback) {
                _winecord_message_cleanup_v(event_data);
                return false;
            }
            command.size = 0;
            callback = cmds->fallback;
        }

        /* skip blank characters after command */
        if (event_data->content) {
            event_data->content = command.start + command.size;
            while (*event_data->content
                   && isspace((int)event_data->content[0]))
                ++event_data->content;
        }

        if (WINEBERRY_RESOURCE_UNAVAILABLE
            == winecord_refcounter_incr(&client->refcounter, event_data))
        {
            winecord_refcounter_add_internal(&client->refcounter, event_data,
                                            _winecord_message_cleanup_v, false);
        }
        callback(client, event_data);
        event_data->content = tmp; /* retrieve original ptr */
        winecord_refcounter_decr(&client->refcounter, event_data);

        return true;
    }

    return false;
}
