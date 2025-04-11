#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

WINEBERRYcode
winecord_get_guild_template(struct winecord *client,
                           const char template_code[],
                           struct winecord_ret_guild_template *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(template_code), WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_INIT(attr, winecord_guild_template, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/templates/%s", template_code);
}

WINEBERRYcode
winecord_create_guild_from_guild_template(
    struct winecord *client,
    const char template_code[],
    struct winecord_create_guild_from_guild_template *params,
    struct winecord_ret_guild *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[256] = { 0 };

    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(template_code), WINEBERRY_BAD_PARAMETER,
                 "");

    body.size = winecord_create_guild_from_guild_template_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/guilds/templates/%s", template_code);
}

WINEBERRYcode
winecord_get_guild_templates(struct winecord *client,
                            u64snowflake guild_id,
                            struct winecord_ret_guild_templates *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_guild_templates, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/templates", guild_id);
}

WINEBERRYcode
winecord_create_guild_template(struct winecord *client,
                              u64snowflake guild_id,
                              struct winecord_create_guild_template *params,
                              struct winecord_ret_guild_template *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[256];

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    body.size =
        winecord_create_guild_template_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild_template, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/guilds/%" PRIu64 "/templates", guild_id);
}

WINEBERRYcode
winecord_sync_guild_template(struct winecord *client,
                            u64snowflake guild_id,
                            const char template_code[],
                            struct winecord_ret_guild_template *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(template_code), WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_INIT(attr, winecord_guild_template, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_PUT,
                            "/guilds/%" PRIu64 "/templates/%s", guild_id,
                            template_code);
}

WINEBERRYcode
winecord_modify_guild_template(struct winecord *client,
                              u64snowflake guild_id,
                              const char template_code[],
                              struct winecord_modify_guild_template *params,
                              struct winecord_ret_guild_template *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[1024] = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(template_code), WINEBERRY_BAD_PARAMETER,
                 "");

    body.size =
        winecord_modify_guild_template_from_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild_template, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/templates/%s", guild_id,
                            template_code);
}

WINEBERRYcode
winecord_delete_guild_template(struct winecord *client,
                              u64snowflake guild_id,
                              const char template_code[],
                              struct winecord_ret_guild_template *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(template_code), WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_INIT(attr, winecord_guild_template, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/templates/%s", guild_id,
                            template_code);
}
