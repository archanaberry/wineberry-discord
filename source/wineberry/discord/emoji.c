#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

WINEBERRYcode
winecord_list_guild_emojis(struct winecord *client,
                          u64snowflake guild_id,
                          struct winecord_ret_emojis *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_emojis, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/emojis", guild_id);
}

WINEBERRYcode
winecord_get_guild_emoji(struct winecord *client,
                        u64snowflake guild_id,
                        u64snowflake emoji_id,
                        struct winecord_ret_emoji *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, emoji_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_emoji, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/emojis/%" PRIu64, guild_id,
                            emoji_id);
}

WINEBERRYcode
winecord_create_guild_emoji(struct winecord *client,
                           u64snowflake guild_id,
                           struct winecord_create_guild_emoji *params,
                           struct winecord_ret_emoji *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[2048];

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_create_guild_emoji_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_emoji, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/guilds/%" PRIu64 "/emojis", guild_id);
}

WINEBERRYcode
winecord_modify_guild_emoji(struct winecord *client,
                           u64snowflake guild_id,
                           u64snowflake emoji_id,
                           struct winecord_modify_guild_emoji *params,
                           struct winecord_ret_emoji *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[2048];

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, emoji_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_modify_guild_emoji_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_emoji, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/emojis/%" PRIu64, guild_id,
                            emoji_id);
}

WINEBERRYcode
winecord_delete_guild_emoji(struct winecord *client,
                           u64snowflake guild_id,
                           u64snowflake emoji_id,
                           struct winecord_delete_guild_emoji *params,
                           struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, emoji_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/emojis/%" PRIu64, guild_id,
                            emoji_id);
}
