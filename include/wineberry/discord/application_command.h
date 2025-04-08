/**
 * @file application_command.h
 * @author Cogmasters
 * @brief Application Command public functions and datatypes
 * @todo application_id should be cached and used when its input value is `0`
 */

#ifndef WINECORD_APPLICATION_COMMAND_H
#define WINECORD_APPLICATION_COMMAND_H

/** @defgroup WinecordAPIInteractionsApplicationCommand Slash commands
 * @ingroup WinecordAPIInteractions
 * @brief Receiving and registering slash commands
 *  @{ */

/**
 * @brief Fetch all of the global commands for your application
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @WINEBERRY_ret_obj{ret,application_commands}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_global_application_commands(
    struct winecord *client,
    u64snowflake application_id,
    struct winecord_ret_application_commands *ret);

/**
 * @brief Create a new global command
 * @note New global commands will be available in all guilds after 1 hour
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,application_command}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_create_global_application_command(
    struct winecord *client,
    u64snowflake application_id,
    struct winecord_create_global_application_command *params,
    struct winecord_ret_application_command *ret);

/**
 * @brief Fetch a global command for your application
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param command_id the registered command id
 * @WINEBERRY_ret_obj{ret,application_command}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_global_application_command(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake command_id,
    struct winecord_ret_application_command *ret);

/**
 * @brief Edit a global command
 * @note Updates will be available in all guilds after 1 hour
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param command_id the registered command id
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,application_command}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_edit_global_application_command(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake command_id,
    struct winecord_edit_global_application_command *params,
    struct winecord_ret_application_command *ret);

/**
 * @brief Deletes a global command
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param command_id the registered command id
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_global_application_command(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake command_id,
    struct winecord_ret *ret);

/**
 * @brief Overwrite existing global application commands
 * @note Updates will be available in all guilds after 1 hour
 * @warning Will overwrite all types of application commands: slash
 *        commands, user commands, and message commands
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param params the request parameters, a list of application commands
 * @WINEBERRY_ret_obj{ret,application_commands}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_bulk_overwrite_global_application_commands(
    struct winecord *client,
    u64snowflake application_id,
    struct winecord_application_commands *params,
    struct winecord_ret_application_commands *ret);

/**
 * @brief Fetch all of the guild commands of a given guild
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param guild_id the guild where the commands are located
 * @WINEBERRY_ret_obj{ret,application_commands}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_guild_application_commands(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    struct winecord_ret_application_commands *ret);

/**
 * @brief Create a new guild command
 * @note Commands will be available in the guild immediately
 * @note Will overwrite any existing guild command with the same name
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param guild_id the guild where the command is located
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,application_command}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_create_guild_application_command(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    struct winecord_create_guild_application_command *params,
    struct winecord_ret_application_command *ret);

/**
 * @brief Fetch a guild command for your application
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param guild_id the guild where the command is located
 * @param command_id the registered command id
 * @WINEBERRY_ret_obj{ret,application_command}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_guild_application_command(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    u64snowflake command_id,
    struct winecord_ret_application_command *ret);

/**
 * @brief Edit a guild command
 * @note Updates for guild commands will be available immediately
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param guild_id the guild where the command is located
 * @param command_id the registered command id
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,application_command}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_edit_guild_application_command(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    u64snowflake command_id,
    struct winecord_edit_guild_application_command *params,
    struct winecord_ret_application_command *ret);

/**
 * @brief Deletes a guild command
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param guild_id the guild where the command is located
 * @param command_id the registered command id
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_guild_application_command(struct winecord *client,
                                                   u64snowflake application_id,
                                                   u64snowflake guild_id,
                                                   u64snowflake command_id,
                                                   struct winecord_ret *ret);

/**
 * @brief Overwrite existing guild application commands
 * @warning This will overwrite all types of application commands: slash
 *        commands, user commands, and message commands
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param guild_id the guild where the commands are located
 * @param params the request parameters, a list of application commands
 * @WINEBERRY_ret_obj{ret,application_commands}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_bulk_overwrite_guild_application_commands(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    struct winecord_bulk_overwrite_guild_application_commands *params,
    struct winecord_ret_application_commands *ret);

/**
 * @brief Fetches command permissions for all commands in a given guild
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param guild_id the guild where the commands are located
 * @WINEBERRY_ret_obj{ret,guild_application_command_permissions}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_guild_application_command_permissions(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    struct winecord_ret_guild_application_command_permissions *ret);

/**
 * @brief Fetches command permissions for a specific command in a given guild
 *
 * @param client the client created with winecord_init()
 * @param application_id the unique id of the parent application
 * @param guild_id the guild where the command is located
 * @param command_id the registered command id
 * @WINEBERRY_ret_obj{ret,application_command_permissions}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_application_command_permissions(
    struct winecord *client,
    u64snowflake application_id,
    u64snowflake guild_id,
    u64snowflake command_id,
    struct winecord_ret_application_command_permission *ret);

/** @example slash-commands.c
 * Demonstrates registering and reacting to slash commands */
/** @example slash-commands2.c
 * Demonstrates registering and reacting to slash commands from the console */

/** @} WinecordAPIInteractionsApplicationCommand */

#endif /* WINECORD_APPLICATION_COMMAND_H */
