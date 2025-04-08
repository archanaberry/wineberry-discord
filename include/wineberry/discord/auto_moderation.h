/**
 * @file auto_moderation.h
 * @author Cogmasters
 * @brief Auto Moderation public functions and datatypes
 */

#ifndef WINECORD_AUTO_MODERATION_H
#define WINECORD_AUTO_MODERATION_H

/** @defgroup WinecordAPIAutoModeration Auto Moderation
 * @ingroup WinecordAPI
 * @brief Auto Moderation public API supported by Winecord
 *  @{ */

/**
 * @brief Get a list of all rules currently configured for the guild
 * @note Requires the `MANAGE_GUILD` permission
 *
 * @param client the client created with winecord_init()
 * @param guild_id the guild to fetch the rules from
 * @WINEBERRY_ret_obj{ret,auto_moderation_rules}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_list_auto_moderation_rules_for_guild(
    struct winecord *client,
    u64snowflake guild_id,
    struct winecord_ret_auto_moderation_rules *ret);

/**
 * @brief Get a single rule
 * @note Requires the `MANAGE_GUILD` permission
 *
 * @param client the client created with winecord_init()
 * @param guild_id the guild to fetch the rule from
 * @param auto_moderation_rule_id the rule to be fetched
 * @WINEBERRY_ret_obj{ret,auto_moderation_rule}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_auto_moderation_rule(
    struct winecord *client,
    u64snowflake guild_id,
    u64snowflake auto_moderation_rule_id,
    struct winecord_ret_auto_moderation_rule *ret);

/**
 * @brief Create a new rule
 * @note Requires the `MANAGE_GUILD` permission
 *
 * @param client the client created with winecord_init()
 * @param guild_id the guild to create the rule in
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,auto_moderation_rule}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_create_auto_moderation_rule(
    struct winecord *client,
    u64snowflake guild_id,
    struct winecord_create_auto_moderation_rule *params,
    struct winecord_ret_auto_moderation_rule *ret);

/**
 * @brief Modify an existing rule
 * @note Requires the `MANAGE_GUILD` permission
 *
 * @param client the client created with winecord_init()
 * @param guild_id the guild where the rule to be modified is at
 * @param auto_moderation_rule_id the rule to be modified
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,auto_moderation_rule}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_modify_auto_moderation_rule(
    struct winecord *client,
    u64snowflake guild_id,
    u64snowflake auto_moderation_rule_id,
    struct winecord_modify_auto_moderation_rule *params,
    struct winecord_ret_auto_moderation_rule *ret);

/**
 * @brief Delete a rule
 * @note Requires the `MANAGE_GUILD` permission
 *
 * @param client the client created with winecord_init()
 * @param guild_id the guild where the rule to be deleted is at
 * @param auto_moderation_rule_id the rule to be deleted
 * @param params request parameters
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_auto_moderation_rule(
    struct winecord *client,
    u64snowflake guild_id,
    u64snowflake auto_moderation_rule_id,
    struct winecord_delete_auto_moderation_rule *params,
    struct winecord_ret *ret);

/** @} WinecordAPIAutoModeration */

#endif /* WINECORD_AUTO_MODERATION_H */
