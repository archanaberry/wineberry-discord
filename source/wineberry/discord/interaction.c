#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"
#include "queriec.h"

WINEBERRYcode
winecord_create_interaction_response(
    struct winecord *client,
    u64snowflake interaction_id,
    const char interaction_token[],
    struct winecord_interaction_response *params,
    struct winecord_ret_interaction_response *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    enum http_method method;
    char buf[16384];

    WINEBERRY_EXPECT(client, interaction_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(interaction_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    if (params->data && params->data->attachments) {
        method = HTTP_MIMEPOST;
        WINECORD_ATTACHMENTS_IDS_INIT(params->data->attachments);
        attr.attachments = *params->data->attachments;
    }
    else {
        method = HTTP_POST;
    }

    body.size = winecord_interaction_response_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_interaction_response, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, method,
                            "/interactions/%" PRIu64 "/%s/callback",
                            interaction_id, interaction_token);
}

WINEBERRYcode
winecord_get_original_interaction_response(
    struct winecord *client,
    u64snowflake application_id,
    const char interaction_token[],
    struct winecord_ret_interaction_response *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(interaction_token), WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_INIT(attr, winecord_interaction_response, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/webhooks/%" PRIu64 "/%s/messages/@original",
                            application_id, interaction_token);
}

WINEBERRYcode
winecord_edit_original_interaction_response(
    struct winecord *client,
    u64snowflake application_id,
    const char interaction_token[],
    struct winecord_edit_original_interaction_response *params,
    struct winecord_ret_interaction_response *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    enum http_method method;
    char buf[16384]; /**< @todo dynamic buffer */

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(interaction_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    if (params->attachments) {
        method = HTTP_MIMEPOST;
        WINECORD_ATTACHMENTS_IDS_INIT(params->attachments);
        attr.attachments = *params->attachments;
    }
    else {
        method = HTTP_PATCH;
    }

    body.size = winecord_edit_original_interaction_response_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_interaction_response, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, method,
                            "/webhooks/%" PRIu64 "/%s/messages/@original",
                            application_id, interaction_token);
}

WINEBERRYcode
winecord_delete_original_interaction_response(struct winecord *client,
                                             u64snowflake application_id,
                                             const char interaction_token[],
                                             struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(interaction_token), WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/webhooks/%" PRIu64 "/%s/messages/@original",
                            application_id, interaction_token);
}

WINEBERRYcode
winecord_create_followup_message(struct winecord *client,
                                u64snowflake application_id,
                                const char interaction_token[],
                                struct winecord_create_followup_message *params,
                                struct winecord_ret_webhook *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    enum http_method method;
    char buf[16384]; /**< @todo dynamic buffer */
    char query[4096] = "";
    char qbuf[32];

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(interaction_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (params->thread_id) {
        int res = queriec_snprintf_add(&queriec, query, "thread_id", sizeof("thread_id"),
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

    body.size =
        winecord_create_followup_message_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_webhook, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, method,
                            "/webhooks/%" PRIu64 "/%s%s", application_id,
                            interaction_token, query);
}

WINEBERRYcode
winecord_get_followup_message(struct winecord *client,
                             u64snowflake application_id,
                             const char interaction_token[],
                             u64snowflake message_id,
                             struct winecord_ret_message *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(interaction_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_message, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/webhooks/%" PRIu64 "/%s/%" PRIu64,
                            application_id, interaction_token, message_id);
}

WINEBERRYcode
winecord_edit_followup_message(struct winecord *client,
                              u64snowflake application_id,
                              const char interaction_token[],
                              u64snowflake message_id,
                              struct winecord_edit_followup_message *params,
                              struct winecord_ret_message *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    enum http_method method;
    char buf[16384]; /**< @todo dynamic buffer */

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(interaction_token), WINEBERRY_BAD_PARAMETER,
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

    body.size =
        winecord_edit_followup_message_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_message, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, method,
                            "/webhooks/%" PRIu64 "/%s/messages/%" PRIu64,
                            application_id, interaction_token, message_id);
}

WINEBERRYcode
winecord_delete_followup_message(struct winecord *client,
                                u64snowflake application_id,
                                const char interaction_token[],
                                u64snowflake message_id,
                                struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(interaction_token), WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, message_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/webhooks/%" PRIu64 "/%s/messages/%" PRIu64,
                            application_id, interaction_token, message_id);
}
