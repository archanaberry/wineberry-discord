#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

WINEBERRYcode
winecord_get_current_user(struct winecord *client, struct winecord_ret_user *ret)
{
    struct winecord_attributes attr = { 0 };

    WINECORD_ATTR_INIT(attr, winecord_user, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/users/@me");
}

WINEBERRYcode
winecord_get_user(struct winecord *client,
                 u64snowflake user_id,
                 struct winecord_ret_user *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, user_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_user, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/users/%" PRIu64, user_id);
}

WINEBERRYcode
winecord_modify_current_user(struct winecord *client,
                            struct winecord_modify_current_user *params,
                            struct winecord_ret_user *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_modify_current_user_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_user, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/users/@me");
}

WINEBERRYcode
winecord_get_current_user_guilds(struct winecord *client,
                                struct winecord_ret_guilds *ret)
{
    struct winecord_attributes attr = { 0 };

    WINECORD_ATTR_LIST_INIT(attr, winecord_guilds, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/users/@me/guilds");
}

WINEBERRYcode
winecord_leave_guild(struct winecord *client,
                    u64snowflake guild_id,
                    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body = { "{}", 2 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_DELETE,
                            "/users/@me/guilds/%" PRIu64, guild_id);
}

WINEBERRYcode
winecord_create_dm(struct winecord *client,
                  struct winecord_create_dm *params,
                  struct winecord_ret_channel *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[128];

    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_create_dm_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_channel, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/users/@me/channels");
}

WINEBERRYcode
winecord_create_group_dm(struct winecord *client,
                        struct winecord_create_group_dm *params,
                        struct winecord_ret_channel *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->access_tokens != NULL, WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, params->nicks != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_create_group_dm_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_channel, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/users/@me/channels");
}

WINEBERRYcode
winecord_get_user_connections(struct winecord *client,
                             struct winecord_ret_connections *ret)
{
    struct winecord_attributes attr = { 0 };

    WINECORD_ATTR_LIST_INIT(attr, winecord_connections, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/users/@me/connections");
}
