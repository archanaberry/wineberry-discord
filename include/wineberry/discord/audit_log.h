/**
 * @file audit_log.h
 * @author Cogmasters
 * @brief Audit Log public functions and datatypes
 */

#ifndef WINEBERRY_AUDIT_LOG
#define WINEBERRY_AUDIT_LOG

#include "winecord.h"

/** @defgroup WineberryAPIAuditLog Audit Log
 * @ingroup WineberryAPI
 * @brief Audit Log's public API supported by Concord
 *  @{ */

struct winecord_get_guild_audit_log {
    /** the user ID to filter audit log */
    u64snowflake user_id;
    /** the type of audit log event */
    int action_type;
    /** the number of entries before a certain entry */
    int before;
    /** the maximum number of entries to return (1-100) */
    int limit;
};

struct winecord_audit_log_entry {
    u64snowflake id;
    u64snowflake user_id;
    int action_type;
    struct winecord_audit_log_change **changes;
    size_t changes_size;
    struct winecord_optional_audit_entry_info options;
    char *reason;
};

struct winecord_audit_log {
    struct winecord_audit_log_entry **entries;
    size_t entries_size;
    struct winecord_webhook *integrations;
    size_t integrations_size;
    struct winecord_channel *threads;
    size_t threads_size;
    struct winecord_user *users;
    size_t users_size;
};

/**
 * @brief Get audit log for a given guild
 *
 * @note Requires the 'VIEW_AUDIT_LOG' permission
 * @param client the client created with winecord_init()
 * @param guild_id the guild to retrieve the audit log from
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,audit_log}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_guild_audit_log(
    struct winecord *client,
    u64snowflake guild_id,
    struct winecord_get_guild_audit_log *params,
    struct winecord_ret_audit_log *ret);

/** @example audit-log.c
 * Demonstrates listening to audit-log events and fetching a specific audit-log
 */

/** @} WineberryAPIAuditLog */

#endif /* WINEBERRY_AUDIT_LOG */
