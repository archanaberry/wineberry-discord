#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"
#include "queriec.h"

WINEBERRY
winecord_create_guild(struct WINECORD *client,
                     struct winecord_create_guild *params,
                     struct winecord_ret_guild *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");

    body.size = winecord_create_guild_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST, "/guilds");
}

WINEBERRY
winecord_get_guild(struct WINECORD *client,
                  u64snowflake guild_id,
                  struct winecord_ret_guild *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_guild, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64, guild_id);
}

WINEBERRY
winecord_get_guild_preview(struct WINECORD *client,
                          u64snowflake guild_id,
                          struct winecord_ret_guild_preview *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_guild_preview, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/preview", guild_id);
}

WINEBERRY
winecord_modify_guild(struct WINECORD *client,
                     u64snowflake guild_id,
                     struct winecord_modify_guild *params,
                     struct winecord_ret_guild *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");

    body.size = winecord_modify_guild_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64, guild_id);
}

WINEBERRY
winecord_delete_guild(struct WINECORD *client,
                     u64snowflake guild_id,
                     struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64, guild_id);
}

WINEBERRY
winecord_get_guild_channels(struct WINECORD *client,
                           u64snowflake guild_id,
                           struct winecord_ret_channels *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_channels, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/channels", guild_id);
}

WINEBERRY
winecord_create_guild_channel(struct WINECORD *client,
                             u64snowflake guild_id,
                             struct winecord_create_guild_channel *params,
                             struct winecord_ret_channel *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[2048];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");

    body.size = winecord_create_guild_channel_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_channel, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/guilds/%" PRIu64 "/channels", guild_id);
}

WINEBERRY
winecord_modify_guild_channel_positions(
    struct WINECORD *client,
    u64snowflake guild_id,
    struct winecord_modify_guild_channel_positions *params,
    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");

    body.size = winecord_modify_guild_channel_positions_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/channels", guild_id);
}

WINEBERRY
winecord_get_guild_member(struct WINECORD *client,
                         u64snowflake guild_id,
                         u64snowflake user_id,
                         struct winecord_ret_guild_member *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_guild_member, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/members/%" PRIu64, guild_id,
                            user_id);
}

WINEBERRY
winecord_list_guild_members(struct WINECORD *client,
                           u64snowflake guild_id,
                           struct winecord_list_guild_members *params,
                           struct winecord_ret_guild_members *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

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
        if (params->after) {
            res = queriec_snprintf_add(&queriec, query, "after", sizeof("after"),
                                       buf, sizeof(buf), "%" PRIu64,
                                       params->after);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
    }

    WINECORD_ATTR_LIST_INIT(attr, winecord_guild_members, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/members%s", guild_id,
                            query);
}

WINEBERRY
winecord_search_guild_members(struct WINECORD *client,
                             u64snowflake guild_id,
                             struct winecord_search_guild_members *params,
                             struct winecord_ret_guild_members *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (params) {
        int res;

        char buf[32];
        if (params->query) {
            char *pe_query =
                curl_escape(params->query, (int)strlen(params->query));

            res = queriec_snprintf_add(&queriec, query, "query",
                                       sizeof("query"), buf, sizeof(buf), "%s",
                                       pe_query);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");

            curl_free(pe_query);
        }
        if (params->limit) {
            res = queriec_snprintf_add(&queriec, query, "limit", sizeof("limit"),
                                       buf, sizeof(buf), "%d", params->limit);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
    }

    WINECORD_ATTR_LIST_INIT(attr, winecord_guild_members, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/members/search%s", guild_id,
                            query);
}

WINEBERRY
winecord_add_guild_member(struct WINECORD *client,
                         u64snowflake guild_id,
                         u64snowflake user_id,
                         struct winecord_add_guild_member *params,
                         struct winecord_ret_guild_member *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params->access_token != NULL, CCORD_BAD_PARAMETER,
                 "");

    body.size = winecord_add_guild_member_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild_member, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PUT,
                            "/guilds/%" PRIu64 "/members/%" PRIu64, guild_id,
                            user_id);
}

WINEBERRY
winecord_modify_guild_member(struct WINECORD *client,
                            u64snowflake guild_id,
                            u64snowflake user_id,
                            struct winecord_modify_guild_member *params,
                            struct winecord_ret_guild_member *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[2048];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");

    body.size = winecord_modify_guild_member_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild_member, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/members/%" PRIu64, guild_id,
                            user_id);
}

WINEBERRY
winecord_modify_current_member(struct WINECORD *client,
                              u64snowflake guild_id,
                              struct winecord_modify_current_member *params,
                              struct winecord_ret_guild_member *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[512];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params->nick != NULL, CCORD_BAD_PARAMETER, "");

    body.size =
        winecord_modify_current_member_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild_member, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/members/@me", guild_id);
}

WINEBERRY
winecord_modify_current_user_nick(
    struct WINECORD *client,
    u64snowflake guild_id,
    struct winecord_modify_current_user_nick *params,
    struct winecord_ret_guild_member *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[512];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params->nick != NULL, CCORD_BAD_PARAMETER, "");

    logconf_warn(&client->conf,
                 "This endpoint is now deprecated by Discord. Please use "
                 "winecord_modify_current_member instead");

    body.size =
        winecord_modify_current_user_nick_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild_member, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/members/@me/nick", guild_id);
}

WINEBERRY
winecord_add_guild_member_role(struct WINECORD *client,
                              u64snowflake guild_id,
                              u64snowflake user_id,
                              u64snowflake role_id,
                              struct winecord_add_guild_member_role *params,
                              struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, role_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_PUT,
                            "/guilds/%" PRIu64 "/members/%" PRIu64
                            "/roles/%" PRIu64,
                            guild_id, user_id, role_id);
}

WINEBERRY
winecord_remove_guild_member_role(
    struct WINECORD *client,
    u64snowflake guild_id,
    u64snowflake user_id,
    u64snowflake role_id,
    struct winecord_remove_guild_member_role *params,
    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, role_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/members/%" PRIu64
                            "/roles/%" PRIu64,
                            guild_id, user_id, role_id);
}

WINEBERRY
winecord_remove_guild_member(struct WINECORD *client,
                            u64snowflake guild_id,
                            u64snowflake user_id,
                            struct winecord_remove_guild_member *params,
                            struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/members/%" PRIu64, guild_id,
                            user_id);
}

WINEBERRY
winecord_get_guild_bans(struct WINECORD *client,
                       u64snowflake guild_id,
                       struct winecord_ret_bans *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_bans, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/bans", guild_id);
}

WINEBERRY
winecord_get_guild_ban(struct WINECORD *client,
                      u64snowflake guild_id,
                      u64snowflake user_id,
                      struct winecord_ret_ban *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_ban, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/bans/%" PRIu64, guild_id,
                            user_id);
}

WINEBERRY
winecord_create_guild_ban(struct WINECORD *client,
                         u64snowflake guild_id,
                         u64snowflake user_id,
                         struct winecord_create_guild_ban *params,
                         struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[256];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client,
                 params->delete_message_days >= 0
                     && params->delete_message_days <= 7,
                 CCORD_BAD_PARAMETER, "");

    body.size = winecord_create_guild_ban_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PUT,
                            "/guilds/%" PRIu64 "/bans/%" PRIu64, guild_id,
                            user_id);
}

WINEBERRY
winecord_remove_guild_ban(struct WINECORD *client,
                         u64snowflake guild_id,
                         u64snowflake user_id,
                         struct winecord_remove_guild_ban *params,
                         struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, user_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/bans/%" PRIu64, guild_id,
                            user_id);
}

WINEBERRY
winecord_get_guild_roles(struct WINECORD *client,
                        u64snowflake guild_id,
                        struct winecord_ret_roles *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_roles, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/roles", guild_id);
}

WINEBERRY
winecord_create_guild_role(struct WINECORD *client,
                          u64snowflake guild_id,
                          struct winecord_create_guild_role *params,
                          struct winecord_ret_role *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[1024];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    body.size = winecord_create_guild_role_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_role, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/guilds/%" PRIu64 "/roles", guild_id);
}

WINEBERRY
winecord_modify_guild_role_positions(
    struct WINECORD *client,
    u64snowflake guild_id,
    struct winecord_modify_guild_role_positions *params,
    struct winecord_ret_roles *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");

    body.size =
        winecord_modify_guild_role_positions_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_LIST_INIT(attr, winecord_roles, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/roles", guild_id);
}

WINEBERRY
winecord_modify_guild_role(struct WINECORD *client,
                          u64snowflake guild_id,
                          u64snowflake role_id,
                          struct winecord_modify_guild_role *params,
                          struct winecord_ret_role *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[2048];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, role_id != 0, CCORD_BAD_PARAMETER, "");

    body.size = winecord_modify_guild_role_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_role, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/roles/%" PRIu64, guild_id,
                            role_id);
}

WINEBERRY
winecord_delete_guild_role(struct WINECORD *client,
                          u64snowflake guild_id,
                          u64snowflake role_id,
                          struct winecord_delete_guild_role *params,
                          struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, role_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/roles/%" PRIu64, guild_id,
                            role_id);
}

WINEBERRY
winecord_get_guild_prune_count(struct WINECORD *client,
                              u64snowflake guild_id,
                              struct winecord_get_guild_prune_count *params,
                              struct winecord_ret_prune_count *ret)
{
    struct winecord_attributes attr = { 0 };
    char query[1024] = "";

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    struct queriec queriec;
    queriec_init(&queriec, sizeof(query));

    if (params) {
        int res;

        char buf[1024];
        if (params->days) {
            res = queriec_snprintf_add(&queriec, query, "days", sizeof("days"),
                                       buf, sizeof(buf), "%d", params->days);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
        if (params->include_roles && params->include_roles->size) {
            char roles[1024];
            int i = 0, offset = 0;

            for (; i < params->include_roles->size - 1; ++i) {
                offset += snprintf(roles + offset, sizeof(roles) - (size_t)offset,
                                   "%" PRIu64 ",", params->include_roles->array[i]);
                ASSERT_NOT_OOB(offset, sizeof(roles));
            }
            offset += snprintf(roles + offset, sizeof(roles) - (size_t)offset,
                               "%" PRIu64 ",", params->include_roles->array[i]);
            ASSERT_NOT_OOB(offset, sizeof(roles));

            res = queriec_snprintf_add(&queriec, query, "include_roles",
                                       sizeof("include_roles"), roles,
                                       sizeof(roles), "%s", roles);
            ASSERT_S(res != QUERIEC_ERROR_NOMEM, "Out of bounds write attempt");
        }
    }

    WINECORD_ATTR_INIT(attr, winecord_prune_count, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/prune%s", guild_id,
                            query);
}

WINEBERRY
winecord_begin_guild_prune(struct WINECORD *client,
                          u64snowflake guild_id,
                          struct winecord_begin_guild_prune *params,
                          struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    body.size = winecord_begin_guild_prune_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/guilds/%" PRIu64 "/prune", guild_id);
}

WINEBERRY
winecord_get_guild_voice_regions(struct WINECORD *client,
                                u64snowflake guild_id,
                                struct winecord_ret_voice_regions *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_voice_regions, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/regions", guild_id);
}

WINEBERRY
winecord_get_guild_invites(struct WINECORD *client,
                          u64snowflake guild_id,
                          struct winecord_ret_invites *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_invites, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/invites", guild_id);
}

WINEBERRY
winecord_get_guild_integrations(struct WINECORD *client,
                               u64snowflake guild_id,
                               struct winecord_ret_integrations *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_LIST_INIT(attr, winecord_integrations, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/integrations", guild_id);
}

WINEBERRY
winecord_delete_guild_integrations(
    struct WINECORD *client,
    u64snowflake guild_id,
    u64snowflake integration_id,
    struct winecord_delete_guild_integrations *params,
    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, integration_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/guilds/%" PRIu64 "/integrations/%" PRIu64,
                            guild_id, integration_id);
}

WINEBERRY
winecord_get_guild_widget_settings(
    struct WINECORD *client,
    u64snowflake guild_id,
    struct winecord_ret_guild_widget_settings *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_guild_widget_settings, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/widget", guild_id);
}

WINEBERRY
winecord_modify_guild_widget(struct WINECORD *client,
                            u64snowflake guild_id,
                            struct winecord_guild_widget_settings *params,
                            struct winecord_ret_guild_widget_settings *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[512];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");

    body.size =
        winecord_guild_widget_settings_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_guild_widget_settings, ret,
                      params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/widget", guild_id);
}

WINEBERRY
winecord_get_guild_widget(struct WINECORD *client,
                         u64snowflake guild_id,
                         struct winecord_ret_guild_widget *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_guild_widget_settings, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/widget.json", guild_id);
}

WINEBERRY
winecord_get_guild_vanity_url(struct WINECORD *client,
                             u64snowflake guild_id,
                             struct winecord_ret_invite *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_invite, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/vanity-url", guild_id);
}

WINEBERRY
winecord_get_guild_welcome_screen(struct WINECORD *client,
                                 u64snowflake guild_id,
                                 struct winecord_ret_welcome_screen *ret)
{
    struct winecord_attributes attr = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_welcome_screen, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/guilds/%" PRIu64 "/welcome-screen", guild_id);
}

WINEBERRY
winecord_modify_guild_welcome_screen(
    struct WINECORD *client,
    u64snowflake guild_id,
    struct winecord_modify_guild_welcome_screen *params,
    struct winecord_ret_welcome_screen *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[4096];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    body.size =
        winecord_modify_guild_welcome_screen_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_welcome_screen, ret,
                      params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/welcome-screen", guild_id);
}

WINEBERRY
winecord_modify_current_user_voice_state(
    struct WINECORD *client,
    u64snowflake guild_id,
    struct winecord_modify_current_user_voice_state *params,
    struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[512];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    body.size = winecord_modify_current_user_voice_state_to_json(
        buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/voice-states/@me", guild_id);
}

WINEBERRY
winecord_modify_user_voice_state(struct WINECORD *client,
                                u64snowflake guild_id,
                                u64snowflake user_id,
                                struct winecord_modify_user_voice_state *params,
                                struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };
    struct ccord_szbuf body;
    char buf[512];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    body.size =
        winecord_modify_user_voice_state_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_BLANK_INIT(attr, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/guilds/%" PRIu64 "/voice-states/%" PRIu64,
                            guild_id, user_id);
}
