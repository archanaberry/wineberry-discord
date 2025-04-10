#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"
#include "queriec.h"

WINEBERRYcode
winecord_list_guild_scheduled_events(
    struct winecord *client,
    u64snowflake guild_id,
    struct winecord_list_guild_scheduled_events *params,
    struct winecord_ret_guild_scheduled_events *ret)
{
    struct winecord_attributes attr = { 0 };
    const char *query =
        (params && params->with_user_count) ? "?with_user_count=1" : "";

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_guild_scheduled_events, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/scheduled-events%s", guild_id,
                            query);
}

WINEBERRYcode
winecord_create_guild_scheduled_event(
    struct winecord *client,
    u64snowflake guild_id,
    struct winecord_create_guild_scheduled_event *params,
    struct winecord_ret_guild_scheduled_event *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[4096];

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(params->name), WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->privacy_level != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->scheduled_start_time != 0,
                 WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->entity_type != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_guild_scheduled_event, ret,
                      params->reason);

    body.size =
        winecord_create_guild_scheduled_event_to_json(buf, sizeof(buf), params);
    body.start = buf;

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/guilds/%" PRIu64 "/scheduled-events", guild_id);
}

WINEBERRYcode
winecord_get_guild_scheduled_event(
    struct winecord *client,
    u64snowflake guild_id,
    u64snowflake guild_scheduled_event_id,
    struct winecord_get_guild_scheduled_event *params,
    struct winecord_ret_guild_scheduled_event *ret)
{
    struct winecord_attributes attr = { 0 };
    const char *query =
        (params && params->with_user_count) ? "?with_user_count=1" : "";

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_scheduled_event_id != 0, WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_INIT(attr, winecord_guild_scheduled_event, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/scheduled-events/%" PRIu64
                            "/%s",
                            guild_id, guild_scheduled_event_id, query);
}

WINEBERRYcode
winecord_modify_guild_scheduled_event(
    struct winecord *client,
    u64snowflake guild_id,
    u64snowflake guild_scheduled_event_id,
    struct winecord_modify_guild_scheduled_event *params,
    struct winecord_ret_guild_scheduled_event *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[4096];

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_scheduled_event_id != 0, WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_INIT(attr, winecord_guild_scheduled_event, ret,
                      params ? params->reason : NULL);

    body.size =
        winecord_modify_guild_scheduled_event_to_json(buf, sizeof(buf), params);
    body.start = buf;

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/scheduled-events/%" PRIu64,
                            guild_id, guild_scheduled_event_id);
}

WINEBERRYcode
winecord_delete_guild_scheduled_event(struct winecord *client,
                                     u64snowflake guild_id,
                                     u64snowflake guild_scheduled_event_id,
                                     struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_scheduled_event_id != 0, WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/scheduled-events/%" PRIu64,
                            guild_id, guild_scheduled_event_id);
}

WINEBERRYcode
winecord_get_guild_scheduled_event_users(
    struct winecord *client,
    u64snowflake guild_id,
    u64snowflake guild_scheduled_event_id,
    struct winecord_get_guild_scheduled_event_users *params,
    struct winecord_ret_guild_scheduled_event_users *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_scheduled_event_id != 0, WINEBERRY_BAD_PARAMETER,
                 "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (params) {
        int res;

        char buf[32];
        if (params->limit) {
            res = queriec_snprintf_add(&queriec, query, "limit", sizeof("limit"),
                                       buf, sizeof(buf), "%d", params->limit);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
        if (params->with_member) {
            res = queriec_snprintf_add(&queriec, query, "with_member", sizeof("with_member"),
                                       buf, sizeof(buf), "%d", params->with_member);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
        if (params->before) {
            res = queriec_snprintf_add(&queriec, query, "before", sizeof("before"),
                                       buf, sizeof(buf), "%" PRIu64, params->before);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
        if (params->after) {
            res = queriec_snprintf_add(&queriec, query, "after", sizeof("after"),
                                       buf, sizeof(buf), "%" PRIu64, params->after);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
    }

    WINECORD_ATTR_LIST_INIT(attr, winecord_guild_scheduled_event_users, ret,
                           NULL);

    return winecord_rest_run(
        &client->rest, &attr, NULL, HTTP_GET,
        "/guilds/%" PRIu64 "/scheduled-events/%" PRIu64 "/users%s", guild_id,
        guild_scheduled_event_id, query);
}
