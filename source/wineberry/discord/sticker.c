#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

/******************************************************************************
 * REST functions
 ******************************************************************************/

WINEBERRY
winecord_get_sticker(struct winecord *client,
                    u64snowflake sticker_id,
                    struct winecord_ret_sticker *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, sticker_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_sticker, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/stickers/%" PRIu64, sticker_id);
}

WINEBERRY
winecord_list_nitro_sticker_packs(
    struct winecord *client, struct winecord_ret_list_nitro_sticker_packs *ret)
{
    struct winecord_attributes attr = { 0 };

    WINECORD_ATTR_INIT(attr, winecord_list_nitro_sticker_packs, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/sticker-packs");
}

WINEBERRY
winecord_list_guild_stickers(struct winecord *client,
                            u64snowflake guild_id,
                            struct winecord_ret_stickers *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_stickers, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/stickers", guild_id);
}

WINEBERRY
winecord_get_guild_sticker(struct winecord *client,
                          u64snowflake guild_id,
                          u64snowflake sticker_id,
                          struct winecord_ret_sticker *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, sticker_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_sticker, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/stickers/%" PRIu64, guild_id,
                            sticker_id);
}

WINEBERRY
winecord_modify_guild_sticker(struct winecord *client,
                             u64snowflake guild_id,
                             u64snowflake sticker_id,
                             struct winecord_modify_guild_sticker *params,
                             struct winecord_ret_sticker *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, sticker_id != 0, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_modify_guild_sticker_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_sticker, ret,
                      params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/stickers/%" PRIu64, guild_id,
                            sticker_id);
}

WINEBERRY
winecord_delete_guild_sticker(struct winecord *client,
                             u64snowflake guild_id,
                             u64snowflake sticker_id,
                             struct winecord_delete_guild_sticker *params,
                             struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, sticker_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/stickers/%" PRIu64, guild_id,
                            sticker_id);
}
