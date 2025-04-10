#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

WINEBERRY
winecord_list_auto_moderation_rules_for_guild(
    struct winecord *client,
    u64snowflake guild_id,
    struct winecord_ret_auto_moderation_rules *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_auto_moderation_rules, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/auto-moderation/rules",
                            guild_id);
}

WINEBERRY
winecord_get_auto_moderation_rule(struct winecord *client,
                                 u64snowflake guild_id,
                                 u64snowflake auto_moderation_rule_id,
                                 struct winecord_ret_auto_moderation_rule *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, auto_moderation_rule_id != 0, WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_INIT(attr, winecord_auto_moderation_rule, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64
                            "/auto-moderation/rules/%" PRIu64,
                            guild_id, auto_moderation_rule_id);
}

WINEBERRY
winecord_create_auto_moderation_rule(
    struct winecord *client,
    u64snowflake guild_id,
    struct winecord_create_auto_moderation_rule *params,
    struct winecord_ret_auto_moderation_rule *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(params->name), WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->event_type != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->trigger_type != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->actions != NULL, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_auto_moderation_rule, ret, params->reason);

    body.size =
        winecord_create_auto_moderation_rule_to_json(buf, sizeof(buf), params);
    body.start = buf;

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/guilds/%" PRIu64 "/auto-moderation/rules",
                            guild_id);
}

WINEBERRY
winecord_modify_auto_moderation_rule(
    struct winecord *client,
    u64snowflake guild_id,
    u64snowflake auto_moderation_rule_id,
    struct winecord_modify_auto_moderation_rule *params,
    struct winecord_ret_auto_moderation_rule *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, auto_moderation_rule_id != 0, WINEBERRY_BAD_PARAMETER,
                 "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(params->name), WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->event_type != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->actions != NULL, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_auto_moderation_rule, ret, params->reason);

    body.size =
        winecord_modify_auto_moderation_rule_to_json(buf, sizeof(buf), params);
    body.start = buf;

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64
                            "/auto-moderation/rules/%" PRIu64,
                            guild_id, auto_moderation_rule_id);
}

WINEBERRY
winecord_delete_auto_moderation_rule(
    struct winecord *client,
    u64snowflake guild_id,
    u64snowflake auto_moderation_rule_id,
    struct winecord_delete_auto_moderation_rule *params,
    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, auto_moderation_rule_id != 0, WINEBERRY_BAD_PARAMETER,
                 "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64
                            "/auto-moderation/rules/%" PRIu64,
                            guild_id, auto_moderation_rule_id);
}
