#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"
#include "queriec.h"

/******************************************************************************
 * Custom functions
 ******************************************************************************/

struct _winecord_get_channel_at_pos {
    enum winecord_channel_types type;
    int position;
    struct winecord_ret_channel ret;
};

/* XXX: placeholder until channel is obtained via cache at
 *      winecord_get_channel_at_pos() */
static void
_done_get_channels(struct winecord *client,
                   struct winecord_response *resp,
                   const struct winecord_channels *chs)
{
    struct _winecord_get_channel_at_pos *cxt = resp->data;
    const struct winecord_channel *found_ch = NULL;

    for (int i = 0, pos = 0; i < chs->size; ++i) {
        if (cxt->type == chs->array[i].type && pos++ == cxt->position) {
            found_ch = &chs->array[i];
            break;
        }
    }

    resp->data = cxt->ret.data;
    resp->keep = cxt->ret.keep;

    if (found_ch) {
        if (cxt->ret.done) cxt->ret.done(client, resp, found_ch);
    }
    else if (cxt->ret.fail) {
        resp->code = WINEBERRY_BAD_PARAMETER;
        cxt->ret.fail(client, resp);
    }

    if (cxt->ret.keep)
        winecord_refcounter_decr(&client->refcounter, (void *)cxt->ret.keep);
    if (cxt->ret.data)
        winecord_refcounter_decr(&client->refcounter, cxt->ret.data);
}

WINEBERRYcode
winecord_get_channel_at_pos(struct winecord *client,
                           u64snowflake guild_id,
                           enum winecord_channel_types type,
                           int position,
                           struct winecord_ret_channel *ret)
{
    struct _winecord_get_channel_at_pos *cxt;
    struct winecord_ret_channels channels_ret = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, ret != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, ret->done != NULL, WINEBERRY_BAD_PARAMETER, "");

    cxt = malloc(sizeof *cxt);
    *cxt = (struct _winecord_get_channel_at_pos){ .type = type,
                                                 .position = position,
                                                 .ret = *ret };

    channels_ret.done = &_done_get_channels;
    channels_ret.fail = ret->fail;
    channels_ret.data = cxt;

    if (ret->keep) {
        WINEBERRYcode code =
            winecord_refcounter_incr(&client->refcounter, (void *)ret->keep);
        ASSERT_S(code == WINEBERRY_OK,
                 "'.keep' data must be a Winecord callback parameter");
    }
    if (ret->data
        && WINEBERRY_RESOURCE_UNAVAILABLE
               == winecord_refcounter_incr(&client->refcounter, ret->data))
    {
        winecord_refcounter_add_client(&client->refcounter, ret->data,
                                      ret->cleanup, false);
    }

    /* TODO: fetch channel via caching, and return if results are non-existent
     */
    return winecord_get_guild_channels(client, guild_id, &channels_ret);
}

/******************************************************************************
 * REST functions
 ******************************************************************************/

WINEBERRYcode
winecord_get_channel(struct winecord *client,
                    u64snowflake channel_id,
                    struct winecord_ret_channel *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_channel, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64, channel_id);
}

WINEBERRYcode
winecord_modify_channel(struct winecord *client,
                       u64snowflake channel_id,
                       struct winecord_modify_channel *params,
                       struct winecord_ret_channel *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_modify_channel_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_channel, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/channels/%" PRIu64, channel_id);
}

WINEBERRYcode
winecord_delete_channel(struct winecord *client,
                       u64snowflake channel_id,
                       struct winecord_delete_channel *params,
                       struct winecord_ret_channel *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_channel, ret,
                      params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64, channel_id);
}

WINEBERRYcode
winecord_get_channel_messages(struct winecord *client,
                             u64snowflake channel_id,
                             struct winecord_get_channel_messages *params,
                             struct winecord_ret_messages *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (params) {
        int res;

        char buf[32];
        if (params->limit) {
            res =
                queriec_snprintf_add(&queriec, query, "limit", sizeof("limit"),
                                     buf, sizeof(buf), "%d", params->limit);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM,
                     "Out of bounds write attempt");
        }
        if (params->around) {
            res = queriec_snprintf_add(&queriec, query, "around",
                                       sizeof("around"), buf, sizeof(buf),
                                       "%" PRIu64, params->around);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM,
                     "Out of bounds write attempt");
        }
        if (params->before) {
            res = queriec_snprintf_add(&queriec, query, "before",
                                       sizeof("before"), buf, sizeof(buf),
                                       "%" PRIu64, params->before);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM,
                     "Out of bounds write attempt");
        }
        if (params->after) {
            res = queriec_snprintf_add(&queriec, query, "after",
                                       sizeof("after"), buf, sizeof(buf),
                                       "%" PRIu64, params->after);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM,
                     "Out of bounds write attempt");
        }
    }

    WINECORD_ATTR_LIST_INIT(attr, winecord_messages, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/messages%s", channel_id,
                            query);
}

WINEBERRYcode
winecord_get_channel_message(struct winecord *client,
                            u64snowflake channel_id,
                            u64snowflake message_id,
                            struct winecord_ret_message *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_message, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/messages/%" PRIu64,
                            channel_id, message_id);
}

WINEBERRYcode
winecord_create_message(struct winecord *client,
                       u64snowflake channel_id,
                       struct winecord_create_message *params,
                       struct winecord_ret_message *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    enum http_method method;
    char buf[16384]; /**< @todo dynamic buffer */

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    if (params->attachments) {
        method = HTTP_MIMEPOST;
        WINECORD_ATTACHMENTS_IDS_INIT(params->attachments);
        attr.attachments = *params->attachments;
    }
    else {
        method = HTTP_POST;
    }

    body.size = winecord_create_message_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_message, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, method,
                            "/channels/%" PRIu64 "/messages", channel_id);
}

WINEBERRYcode
winecord_crosspost_message(struct winecord *client,
                          u64snowflake channel_id,
                          u64snowflake message_id,
                          struct winecord_ret_message *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_message, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_POST,
                            "/channels/%" PRIu64 "/messages/%" PRIu64
                            "/crosspost",
                            channel_id, message_id);
}

WINEBERRYcode
winecord_create_reaction(struct winecord *client,
                        u64snowflake channel_id,
                        u64snowflake message_id,
                        u64snowflake emoji_id,
                        const char emoji_name[],
                        struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    char *pct_emoji_name;
    char emoji_endpoint[256];
    WINEBERRYcode code;

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    pct_emoji_name =
        emoji_name ? curl_escape(emoji_name, (int)strlen(emoji_name)) : NULL;

    if (emoji_id)
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s:%" PRIu64,
                 pct_emoji_name, emoji_id);
    else
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s", pct_emoji_name);

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    code = winecord_rest_run(&client->rest, &attr, NULL, HTTP_PUT,
                            "/channels/%" PRIu64 "/messages/%" PRIu64
                            "/reactions/%s/@me",
                            channel_id, message_id, emoji_endpoint);

    curl_free(pct_emoji_name);

    return code;
}

WINEBERRYcode
winecord_delete_own_reaction(struct winecord *client,
                            u64snowflake channel_id,
                            u64snowflake message_id,
                            u64snowflake emoji_id,
                            const char emoji_name[],
                            struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    char *pct_emoji_name;
    char emoji_endpoint[256];
    WINEBERRYcode code;

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    pct_emoji_name =
        emoji_name ? curl_escape(emoji_name, (int)strlen(emoji_name)) : NULL;

    if (emoji_id)
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s:%" PRIu64,
                 pct_emoji_name, emoji_id);
    else
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s", pct_emoji_name);

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    code = winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/messages/%" PRIu64
                            "/reactions/%s/@me",
                            channel_id, message_id, emoji_endpoint);

    curl_free(pct_emoji_name);

    return code;
}

WINEBERRYcode
winecord_delete_user_reaction(struct winecord *client,
                             u64snowflake channel_id,
                             u64snowflake message_id,
                             u64snowflake user_id,
                             u64snowflake emoji_id,
                             const char emoji_name[],
                             struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    char *pct_emoji_name;
    char emoji_endpoint[256];
    WINEBERRYcode code;

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, user_id != 0, WINEBERRY_BAD_PARAMETER, "");

    pct_emoji_name =
        emoji_name ? curl_escape(emoji_name, (int)strlen(emoji_name)) : NULL;

    if (emoji_id)
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s:%" PRIu64,
                 pct_emoji_name, emoji_id);
    else
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s", pct_emoji_name);

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    code = winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/messages/%" PRIu64
                            "/reactions/%s/%" PRIu64,
                            channel_id, message_id, emoji_endpoint, user_id);

    curl_free(pct_emoji_name);

    return code;
}

WINEBERRYcode
winecord_get_reactions(struct winecord *client,
                      u64snowflake channel_id,
                      u64snowflake message_id,
                      u64snowflake emoji_id,
                      const char emoji_name[],
                      struct winecord_get_reactions *params,
                      struct winecord_ret_users *ret)
{
    struct winecord_attributes attr = { 0 };
    char emoji_endpoint[256];
    char query[1024] = "";
    char *pct_emoji_name;
    WINEBERRYcode code;

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (params) {
        int res;

        char buf[32];
        if (params->after) {
            WINEBERRY_EXPECT(client, params->after != 0, WINEBERRY_BAD_PARAMETER, "");

            res = queriec_snprintf_add(&queriec, query, "after",
                                       sizeof("after"), buf, sizeof(buf),
                                       "%" PRIu64, params->after);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM,
                     "Out of bounds write attempt");
        }
        if (params->limit) {
            WINEBERRY_EXPECT(client, params->limit > 0 && params->limit <= 100,
                         WINEBERRY_BAD_PARAMETER, "");

            res =
                queriec_snprintf_add(&queriec, query, "limit", sizeof("limit"),
                                     buf, sizeof(buf), "%d", params->limit);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM,
                     "Out of bounds write attempt");
        }
    }

    pct_emoji_name =
        emoji_name ? curl_escape(emoji_name, (int)strlen(emoji_name)) : NULL;

    if (emoji_id)
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s:%" PRIu64,
                 pct_emoji_name, emoji_id);
    else
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s", pct_emoji_name);

    WINECORD_ATTR_LIST_INIT(attr, winecord_users, ret, NULL);

    code = winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/messages/%" PRIu64
                            "/reactions/%s%s",
                            channel_id, message_id, emoji_endpoint, query);

    curl_free(pct_emoji_name);

    return code;
}

WINEBERRYcode
winecord_delete_all_reactions(struct winecord *client,
                             u64snowflake channel_id,
                             u64snowflake message_id,
                             struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/messages/%" PRIu64
                            "/reactions",
                            channel_id, message_id);
}

WINEBERRYcode
winecord_delete_all_reactions_for_emoji(struct winecord *client,
                                       u64snowflake channel_id,
                                       u64snowflake message_id,
                                       u64snowflake emoji_id,
                                       const char emoji_name[],
                                       struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    char *pct_emoji_name;
    char emoji_endpoint[256];
    WINEBERRYcode code;

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    pct_emoji_name =
        emoji_name ? curl_escape(emoji_name, (int)strlen(emoji_name)) : NULL;

    if (emoji_id)
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s:%" PRIu64,
                 pct_emoji_name, emoji_id);
    else
        snprintf(emoji_endpoint, sizeof(emoji_endpoint), "%s", pct_emoji_name);

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    code = winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/messages/%" PRIu64
                            "/reactions/%s",
                            channel_id, message_id, emoji_endpoint);

    curl_free(pct_emoji_name);

    return code;
}

WINEBERRYcode
winecord_edit_message(struct winecord *client,
                     u64snowflake channel_id,
                     u64snowflake message_id,
                     struct winecord_edit_message *params,
                     struct winecord_ret_message *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[16384]; /**< @todo dynamic buffer */

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_edit_message_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_message, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/channels/%" PRIu64 "/messages/%" PRIu64,
                            channel_id, message_id);
}

WINEBERRYcode
winecord_delete_message(struct winecord *client,
                       u64snowflake channel_id,
                       u64snowflake message_id,
                       struct winecord_delete_message *params,
                       struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/messages/%" PRIu64,
                            channel_id, message_id);
}

/** @todo add duplicated ID verification */
WINEBERRYcode
winecord_bulk_delete_messages(struct winecord *client,
                             u64snowflake channel_id,
                             struct winecord_bulk_delete_messages *params,
                             struct winecord_ret *ret)
{
    const u64unix_ms now = winecord_timestamp(client);
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096] = "";

    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->messages != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client,
                 params->messages->size >= 2 && params->messages->size <= 100,
                 WINEBERRY_BAD_PARAMETER, "");

    for (int i = 0; i < params->messages->size; ++i) {
        u64unix_ms tstamp = (params->messages->array[i] >> 22) + 1420070400000;

        WINEBERRY_EXPECT(client, now <= tstamp || now - tstamp <= 1209600000,
                     WINEBERRY_BAD_PARAMETER,
                     "Messages should not be older than 2 weeks.");
    }

    body.size = winecord_bulk_delete_messages_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/channels/%" PRIu64 "/messages/bulk-delete",
                            channel_id);
}

WINEBERRYcode
winecord_edit_channel_permissions(
    struct winecord *client,
    u64snowflake channel_id,
    u64snowflake overwrite_id,
    struct winecord_edit_channel_permissions *params,
    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, overwrite_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size =
        winecord_edit_channel_permissions_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PUT,
                            "/channels/%" PRIu64 "/permissions/%" PRIu64,
                            channel_id, overwrite_id);
}

WINEBERRYcode
winecord_get_channel_invites(struct winecord *client,
                            u64snowflake channel_id,
                            struct winecord_ret_invites *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_invites, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/invites", channel_id);
}

WINEBERRYcode
winecord_create_channel_invite(struct winecord *client,
                              u64snowflake channel_id,
                              struct winecord_create_channel_invite *params,
                              struct winecord_ret_invite *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    body.size =
        winecord_create_channel_invite_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_invite, ret,
                      params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/channels/%" PRIu64 "/invites", channel_id);
}

WINEBERRYcode
winecord_delete_channel_permission(
    struct winecord *client,
    u64snowflake channel_id,
    u64snowflake overwrite_id,
    struct winecord_delete_channel_permission *params,
    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, overwrite_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/permissions/%" PRIu64,
                            channel_id, overwrite_id);
}

WINEBERRYcode
winecord_follow_news_channel(struct winecord *client,
                            u64snowflake channel_id,
                            struct winecord_follow_news_channel *params,
                            struct winecord_ret_followed_channel *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[256]; /* should be more than enough for this */

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->webhook_channel_id != 0, WINEBERRY_BAD_PARAMETER,
                 "");

    body.size = winecord_follow_news_channel_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_channel, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/channels/%" PRIu64 "/followers", channel_id);
}

WINEBERRYcode
winecord_trigger_typing_indicator(struct winecord *client,
                                 u64snowflake channel_id,
                                 struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_POST,
                            "/channels/%" PRIu64 "/typing", channel_id);
}

WINEBERRYcode
winecord_get_pinned_messages(struct winecord *client,
                            u64snowflake channel_id,
                            struct winecord_ret_messages *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_messages, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/pins", channel_id);
}

WINEBERRYcode
winecord_pin_message(struct winecord *client,
                    u64snowflake channel_id,
                    u64snowflake message_id,
                    struct winecord_pin_message *params,
                    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_PUT,
                            "/channels/%" PRIu64 "/pins/%" PRIu64, channel_id,
                            message_id);
}

WINEBERRYcode
winecord_unpin_message(struct winecord *client,
                      u64snowflake channel_id,
                      u64snowflake message_id,
                      struct winecord_unpin_message *params,
                      struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/pins/%" PRIu64, channel_id,
                            message_id);
}

WINEBERRYcode
winecord_group_dm_add_recipient(struct winecord *client,
                               u64snowflake channel_id,
                               u64snowflake user_id,
                               struct winecord_group_dm_add_recipient *params,
                               struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, user_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size =
        winecord_group_dm_add_recipient_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PUT,
                            "/channels/%" PRIu64 "/recipients/%" PRIu64,
                            channel_id, user_id);
}

WINEBERRYcode
winecord_group_dm_remove_recipient(struct winecord *client,
                                  u64snowflake channel_id,
                                  u64snowflake user_id,
                                  struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, user_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/recipients/%" PRIu64,
                            channel_id, user_id);
}

WINEBERRYcode
winecord_start_thread_with_message(
    struct winecord *client,
    u64snowflake channel_id,
    u64snowflake message_id,
    struct winecord_start_thread_with_message *params,
    struct winecord_ret_channel *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size =
        winecord_start_thread_with_message_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_channel, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/channels/%" PRIu64 "/messages/%" PRIu64
                            "/threads",
                            channel_id, message_id);
}

WINEBERRYcode
winecord_start_thread_without_message(
    struct winecord *client,
    u64snowflake channel_id,
    struct winecord_start_thread_without_message *params,
    struct winecord_ret_channel *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size =
        winecord_start_thread_without_message_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_channel, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/channels/%" PRIu64 "/threads", channel_id);
}

WINEBERRYcode
winecord_join_thread(struct winecord *client,
                    u64snowflake channel_id,
                    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_PUT,
                            "/channels/%" PRIu64 "/thread-members/@me",
                            channel_id);
}

WINEBERRYcode
winecord_add_thread_member(struct winecord *client,
                          u64snowflake channel_id,
                          u64snowflake user_id,
                          struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, user_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_PUT,
                            "/channels/%" PRIu64 "/thread-members/%" PRIu64,
                            channel_id, user_id);
}

WINEBERRYcode
winecord_leave_thread(struct winecord *client,
                     u64snowflake channel_id,
                     struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/thread-members/@me",
                            channel_id);
}

WINEBERRYcode
winecord_remove_thread_member(struct winecord *client,
                             u64snowflake channel_id,
                             u64snowflake user_id,
                             struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, user_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/channels/%" PRIu64 "/thread-members/%" PRIu64,
                            channel_id, user_id);
}

WINEBERRYcode
winecord_list_thread_members(struct winecord *client,
                            u64snowflake channel_id,
                            struct winecord_ret_thread_members *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_thread_members, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/thread-members",
                            channel_id);
}

WINEBERRYcode
winecord_list_active_threads(struct winecord *client,
                            u64snowflake channel_id,
                            struct winecord_ret_thread_response_body *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_thread_response_body, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/threads/active",
                            channel_id);
}

WINEBERRYcode
winecord_list_public_archived_threads(
    struct winecord *client,
    u64snowflake channel_id,
    u64unix_ms before,
    int limit,
    struct winecord_ret_thread_response_body *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";
    char buf[32];
    int res;

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (before) {
        res = queriec_snprintf_add(&queriec, query, "before", sizeof("before"),
                                   buf, sizeof(buf), "%" PRIu64, before);
        ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
    }
    if (limit) {
        res = queriec_snprintf_add(&queriec, query, "limit", sizeof("limit"),
                                   buf, sizeof(buf), "%d", limit);
        ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
    }

    WINECORD_ATTR_INIT(attr, winecord_thread_response_body, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/threads/archived/public%s",
                            channel_id, query);
}

WINEBERRYcode
winecord_list_private_archived_threads(
    struct winecord *client,
    u64snowflake channel_id,
    u64unix_ms before,
    int limit,
    struct winecord_ret_thread_response_body *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";
    char buf[32];
    int res;

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (before) {
        res = queriec_snprintf_add(&queriec, query, "before", sizeof("before"),
                                   buf, sizeof(buf), "%" PRIu64, before);
        ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
    }
    if (limit) {
        res = queriec_snprintf_add(&queriec, query, "limit", sizeof("limit"),
                                   buf, sizeof(buf), "%d", limit);
        ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
    }

    WINECORD_ATTR_INIT(attr, winecord_thread_response_body, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/threads/archived/private%s",
                            channel_id, query);
}

WINEBERRYcode
winecord_list_joined_private_archived_threads(
    struct winecord *client,
    u64snowflake channel_id,
    u64unix_ms before,
    int limit,
    struct winecord_ret_thread_response_body *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";
    char buf[32];
    int res;

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (before) {
        res = queriec_snprintf_add(&queriec, query, "before", sizeof("before"),
                                   buf, sizeof(buf), "%" PRIu64, before);
        ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
    }
    if (limit) {
        res = queriec_snprintf_add(&queriec, query, "limit", sizeof("limit"),
                                   buf, sizeof(buf), "%d", limit);
        ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
    }

    WINECORD_ATTR_INIT(attr, winecord_thread_response_body, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64
                            "/users/@me/threads/archived/private%s",
                            channel_id, query);
}
