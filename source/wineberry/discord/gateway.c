#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

/******************************************************************************
 * Custom functions
 ******************************************************************************/

WINEBERRYcode
winecord_disconnect_guild_member(struct winecord *client,
                                u64snowflake guild_id,
                                u64snowflake user_id,
                                struct winecord_modify_guild_member *params,
                                struct winecord_ret_guild_member *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[128];
    jsonb b;

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, user_id != 0, WINEBERRY_BAD_PARAMETER, "");

    jsonb_init(&b);
    jsonb_object(&b, buf, sizeof(buf));
    {
        jsonb_key(&b, buf, sizeof(buf), "channel_id",
                  sizeof("channel_id") - 1);
        jsonb_null(&b, buf, sizeof(buf));
        jsonb_object_pop(&b, buf, sizeof(buf));
    }

    body.start = buf;
    body.size = b.pos;

    WINECORD_ATTR_INIT(attr, winecord_guild_member, ret,
                      params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/members/%" PRIu64, guild_id,
                            user_id);
}

/******************************************************************************
 * REST functions
 ******************************************************************************/

static size_t
_wineberry_szbuf_from_json(const char str[], size_t len, void *p_buf)
{
    struct wineberry_szbuf *buf = p_buf;
    return buf->size = cog_strndup(str, len, &buf->start);
}

WINEBERRYcode
winecord_get_gateway(struct winecord *client, struct wineberry_szbuf *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, ret != NULL, WINEBERRY_BAD_PARAMETER, "");

    attr.response.from_json = &_wineberry_szbuf_from_json;
    attr.dispatch.has_type = true;
    attr.dispatch.sync = ret;

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET, "/gateway");
}

WINEBERRYcode
winecord_get_gateway_bot(struct winecord *client, struct wineberry_szbuf *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, ret != NULL, WINEBERRY_BAD_PARAMETER, "");

    attr.response.from_json = &_wineberry_szbuf_from_json;
    attr.dispatch.has_type = true;
    attr.dispatch.sync = ret;

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/gateway/bot");
}
