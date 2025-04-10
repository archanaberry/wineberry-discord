#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"
#include "queriec.h"

WINEBERRY
winecord_get_guild_audit_log(struct winecord *client,
                            u64snowflake guild_id,
                            struct winecord_get_guild_audit_log *params,
                            struct winecord_ret_audit_log *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (params) {
        int res;

        char buf[32];
        if (params->user_id) {
            res = queriec_snprintf_add(&queriec, query, "user_id", sizeof("user_id"),
                                       buf, sizeof(buf), "%" PRIu64, params->user_id);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
        if (params->action_type) {
            res = queriec_snprintf_add(&queriec, query, "action_type", sizeof("action_type"),
                                       buf, sizeof(buf), "%d", params->action_type);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
        if (params->before) {
            res = queriec_snprintf_add(&queriec, query, "before", sizeof("before"),
                                       buf, sizeof(buf), "%" PRIu64, params->before);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
        if (params->limit) {
            res = queriec_snprintf_add(&queriec, query, "limit", sizeof("limit"),
                                       buf, sizeof(buf), "%d", params->limit);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
    }

    WINECORD_ATTR_INIT(attr, winecord_audit_log, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/audit-logs%s", guild_id, query);
}
