#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"
#include "queriec.h"

WINEBERRY
winecord_create_webhook(struct WINECORD *client,
                       u64snowflake channel_id,
                       struct winecord_create_webhook *params,
                       struct winecord_ret_webhook *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(params->name), WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_create_webhook_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_webhook, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/channels/%" PRIu64 "/webhooks", channel_id);
}

WINEBERRY
winecord_get_channel_webhooks(struct WINECORD *client,
                             u64snowflake channel_id,
                             struct winecord_ret_webhooks *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_webhooks, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/channels/%" PRIu64 "/webhooks", channel_id);
}

WINEBERRY
winecord_get_guild_webhooks(struct WINECORD *client,
                           u64snowflake guild_id,
                           struct winecord_ret_webhooks *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_webhooks, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/webhooks", guild_id);
}

WINEBERRY
winecord_get_webhook(struct WINECORD *client,
                    u64snowflake webhook_id,
                    struct winecord_ret_webhook *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_webhook, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/webhooks/%" PRIu64, webhook_id);
}

WINEBERRY
winecord_get_webhook_with_token(struct WINECORD *client,
                               u64snowflake webhook_id,
                               const char webhook_token[],
                               struct winecord_ret_webhook *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(webhook_token), WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_INIT(attr, winecord_webhook, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/webhooks/%" PRIu64 "/%s", webhook_id,
                            webhook_token);
}

WINEBERRY
winecord_modify_webhook(struct WINECORD *client,
                       u64snowflake webhook_id,
                       struct winecord_modify_webhook *params,
                       struct winecord_ret_webhook *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_modify_webhook_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_webhook, ret,
                      params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/webhooks/%" PRIu64, webhook_id);
}

WINEBERRY
winecord_modify_webhook_with_token(
    struct WINECORD *client,
    u64snowflake webhook_id,
    const char webhook_token[],
    struct winecord_modify_webhook_with_token *params,
    struct winecord_ret_webhook *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(webhook_token), WINEBERRY_BAD_PARAMETER,
                 "");

    body.size =
        winecord_modify_webhook_with_token_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_webhook, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/webhooks/%" PRIu64 "/%s", webhook_id,
                            webhook_token);
}

WINEBERRY
winecord_delete_webhook(struct WINECORD *client,
                       u64snowflake webhook_id,
                       struct winecord_delete_webhook *params,
                       struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/webhooks/%" PRIu64, webhook_id);
}

WINEBERRY
winecord_delete_webhook_with_token(struct WINECORD *client,
                                  u64snowflake webhook_id,
                                  const char webhook_token[],
                                  struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(webhook_token), WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/webhooks/%" PRIu64 "/%s", webhook_id,
                            webhook_token);
}

WINEBERRY
winecord_execute_webhook(struct WINECORD *client,
                        u64snowflake webhook_id,
                        const char webhook_token[],
                        struct winecord_execute_webhook *params,
                        struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    enum http_method method;
    char buf[16384]; /**< @todo dynamic buffer */
    char query[4096] = "";
    char qbuf[32];
    int res;

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(webhook_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (params->wait) {
        res = queriec_add(&queriec, query, "wait", sizeof("wait"), "1", 1);
        ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
    }
    if (params->thread_id) {
        res = queriec_snprintf_add(&queriec, query, "thread_id", sizeof("thread_id"),
                                       qbuf, sizeof(qbuf), "%" PRIu64, params->thread_id);
        ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
    }

    if (params->attachments) {
        method = HTTP_MIMEPOST;
        WINECORD_ATTACHMENTS_IDS_INIT(params->attachments);
        attr.attachments = *params->attachments;
    }
    else {
        method = HTTP_POST;
    }

    body.size = winecord_execute_webhook_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, method,
                            "/webhooks/%" PRIu64 "/%s%s", webhook_id,
                            webhook_token, query);
}

WINEBERRY
winecord_get_webhook_message(struct WINECORD *client,
                            u64snowflake webhook_id,
                            const char webhook_token[],
                            u64snowflake message_id,
                            struct winecord_ret_message *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(webhook_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_message, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/webhooks/%" PRIu64 "/%s/%" PRIu64, webhook_id,
                            webhook_token, message_id);
}

WINEBERRY
winecord_edit_webhook_message(struct WINECORD *client,
                             u64snowflake webhook_id,
                             const char webhook_token[],
                             u64snowflake message_id,
                             struct winecord_edit_webhook_message *params,
                             struct winecord_ret_message *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    enum http_method method;
    char buf[16384]; /**< @todo dynamic buffer */

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(webhook_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    if (params->attachments) {
        method = HTTP_MIMEPOST;
        WINECORD_ATTACHMENTS_IDS_INIT(params->attachments);
        attr.attachments = *params->attachments;
    }
    else {
        method = HTTP_PATCH;
    }

    body.size = winecord_edit_webhook_message_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_message, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, method,
                            "/webhooks/%" PRIu64 "/%s/messages/%" PRIu64,
                            webhook_id, webhook_token, message_id);
}

WINEBERRY
winecord_delete_webhook_message(struct WINECORD *client,
                               u64snowflake webhook_id,
                               const char webhook_token[],
                               u64snowflake message_id,
                               struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, webhook_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(webhook_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/webhooks/%" PRIu64 "/%s/messages/%" PRIu64,
                            webhook_id, webhook_token, message_id);
}
