#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

WINEBERRY
winecord_get_global_application_commands(
    struct WINECORD *client,
    u64snowflake application_id,
    struct winecord_ret_application_commands *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_application_commands, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/applications/%" PRIu64 "/commands",
                            application_id);
}

WINEBERRY
winecord_create_global_application_command(
    struct WINECORD *client,
    u64snowflake application_id,
    struct winecord_create_global_application_command *params,
    struct winecord_ret_application_command *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(params->name), WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(params->description),
                 WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_application_command, ret, NULL);

    body.size = winecord_create_global_application_command_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/applications/%" PRIu64 "/commands",
                            application_id);
}

WINEBERRY
winecord_get_global_application_command(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake command_id,
    struct winecord_ret_application_command *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, command_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_application_command, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/applications/%" PRIu64 "/commands/%" PRIu64,
                            application_id, command_id);
}

WINEBERRY
winecord_edit_global_application_command(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake command_id,
    struct winecord_edit_global_application_command *params,
    struct winecord_ret_application_command *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, command_id != 0, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_edit_global_application_command_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_application_command, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/applications/%" PRIu64 "/commands/%" PRIu64,
                            application_id, command_id);
}

WINEBERRY
winecord_delete_global_application_command(struct WINECORD *client,
                                          u64snowflake application_id,
                                          u64snowflake command_id,
                                          struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, command_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/applications/%" PRIu64 "/commands/%" PRIu64,
                            application_id, command_id);
}

WINEBERRY
winecord_bulk_overwrite_global_application_commands(
    struct WINECORD *client,
    u64snowflake application_id,
    struct winecord_application_commands *params,
    struct winecord_ret_application_commands *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[8192];

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_application_commands_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_LIST_INIT(attr, winecord_application_commands, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PUT,
                            "/applications/%" PRIu64 "/commands",
                            application_id);
}

WINEBERRY
winecord_get_guild_application_commands(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    struct winecord_ret_application_commands *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_application_commands, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/applications/%" PRIu64 "/guilds/%" PRIu64
                            "/commands",
                            application_id, guild_id);
}

WINEBERRY
winecord_create_guild_application_command(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    struct winecord_create_guild_application_command *params,
    struct winecord_ret_application_command *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(params->name), WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(params->description),
                 WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_create_guild_application_command_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_application_command, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/applications/%" PRIu64 "/guilds/%" PRIu64
                            "/commands",
                            application_id, guild_id);
}

WINEBERRY
winecord_get_guild_application_command(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    u64snowflake command_id,
    struct winecord_ret_application_command *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, command_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_application_command, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/applications/%" PRIu64 "/guilds/%" PRIu64
                            "/commands/%" PRIu64,
                            application_id, guild_id, command_id);
}

WINEBERRY
winecord_edit_guild_application_command(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    u64snowflake command_id,
    struct winecord_edit_guild_application_command *params,
    struct winecord_ret_application_command *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, command_id != 0, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_edit_guild_application_command_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_application_command, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/applications/%" PRIu64 "/guilds/%" PRIu64
                            "/commands/%" PRIu64,
                            application_id, guild_id, command_id);
}

WINEBERRY
winecord_delete_guild_application_command(struct WINECORD *client,
                                         u64snowflake application_id,
                                         u64snowflake guild_id,
                                         u64snowflake command_id,
                                         struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, command_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/applications/%" PRIu64 "/guilds/%" PRIu64
                            "/commands/%" PRIu64,
                            application_id, guild_id, command_id);
}

WINEBERRY
winecord_bulk_overwrite_guild_application_commands(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    struct winecord_bulk_overwrite_guild_application_commands *params,
    struct winecord_ret_application_commands *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[8192];

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_bulk_overwrite_guild_application_commands_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_LIST_INIT(attr, winecord_application_commands, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PUT,
                            "/applications/%" PRIu64 "/guilds/%" PRIu64
                            "/commands",
                            application_id, guild_id);
}

WINEBERRY
winecord_get_guild_application_command_permissions(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    struct winecord_ret_guild_application_command_permissions *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_application_command_permissions, ret,
                           NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/applications/%" PRIu64 "/guilds/%" PRIu64
                            "/commands/permissions",
                            application_id, guild_id);
}

WINEBERRY
winecord_get_application_command_permissions(
    struct WINECORD *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    u64snowflake command_id,
    struct winecord_ret_application_command_permission *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, application_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, guild_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, command_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_application_command_permission, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/applications/%" PRIu64 "/guilds/%" PRIu64
                            "/commands/%" PRIu64 "/permissions",
                            application_id, guild_id, command_id);
}
