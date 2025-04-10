/**
 * @file winecord-events.h
 * @author Cogmasters
 * @brief Listen, react and trigger Winecord Gateway events
 */

#ifndef WINECORD_EVENTS_H
#define WINECORD_EVENTS_H

/** @defgroup WinecordCommands Commands
 * @ingroup WinecordClient
 * @brief Requests made by the client to the Gateway socket
 *  @{ */

/**
 * @brief Request all members for a guild or a list of guilds
 * @see
 * https://discord.com/developers/docs/topics/gateway#request-guild-members
 *
 * @param client the client created with winecord_init()
 * @param request request guild members information
 */
void winecord_request_guild_members(
    struct winecord *client, struct winecord_request_guild_members *request);

/**
 * @brief Sent when a client wants to join, move or disconnect from a voice
 *      channel
 *
 * @param client the client created with winecord_init()
 * @param update request guild members information
 */
void winecord_update_voice_state(struct winecord *client,
                                struct winecord_update_voice_state *update);

/**
 * @brief Update the client presence status
 * @see winecord_presence_add_activity()
 *
 * @param client the client created with winecord_init()
 * @param presence status to update the client's to
 */
void winecord_update_presence(struct winecord *client,
                             struct winecord_presence_update *presence);

/**
 * @brief Set the client presence status
 * @deprecated since v2.0.0, use winecord_update_presence() instead
 * @see winecord_presence_add_activity()
 *
 * @param client the client created with winecord_init()
 * @param presence status to update the client's to
 */
void winecord_set_presence(struct winecord *client,
                          struct winecord_presence_update *presence);

/** @} WinecordCommands */

/** @defgroup WinecordEvents Events
 * @ingroup WinecordClient
 * @brief Events sent over the Gateway socket to the client
 *  @{ */

/** @brief Winecord Gateway's events */
enum winecord_gateway_events {
    WINECORD_EV_NONE = 0, /**< missing event */
    WINECORD_EV_READY,
    WINECORD_EV_RESUMED,
    WINECORD_EV_RECONNECT,
    WINECORD_EV_INVALID_SESSION,
    WINECORD_EV_APPLICATION_COMMAND_PERMISSIONS_UPDATE,
    WINECORD_EV_AUTO_MODERATION_RULE_CREATE,
    WINECORD_EV_AUTO_MODERATION_RULE_UPDATE,
    WINECORD_EV_AUTO_MODERATION_RULE_DELETE,
    WINECORD_EV_AUTO_MODERATION_ACTION_EXECUTION,
    WINECORD_EV_CHANNEL_CREATE,
    WINECORD_EV_CHANNEL_UPDATE,
    WINECORD_EV_CHANNEL_DELETE,
    WINECORD_EV_CHANNEL_PINS_UPDATE,
    WINECORD_EV_THREAD_CREATE,
    WINECORD_EV_THREAD_UPDATE,
    WINECORD_EV_THREAD_DELETE,
    WINECORD_EV_THREAD_LIST_SYNC,
    WINECORD_EV_THREAD_MEMBER_UPDATE,
    WINECORD_EV_THREAD_MEMBERS_UPDATE,
    WINECORD_EV_GUILD_CREATE,
    WINECORD_EV_GUILD_UPDATE,
    WINECORD_EV_GUILD_DELETE,
    WINECORD_EV_GUILD_BAN_ADD,
    WINECORD_EV_GUILD_BAN_REMOVE,
    WINECORD_EV_GUILD_EMOJIS_UPDATE,
    WINECORD_EV_GUILD_STICKERS_UPDATE,
    WINECORD_EV_GUILD_INTEGRATIONS_UPDATE,
    WINECORD_EV_GUILD_MEMBER_ADD,
    WINECORD_EV_GUILD_MEMBER_REMOVE,
    WINECORD_EV_GUILD_MEMBER_UPDATE,
    WINECORD_EV_GUILD_MEMBERS_CHUNK,
    WINECORD_EV_GUILD_ROLE_CREATE,
    WINECORD_EV_GUILD_ROLE_UPDATE,
    WINECORD_EV_GUILD_ROLE_DELETE,
    WINECORD_EV_GUILD_SCHEDULED_EVENT_CREATE,
    WINECORD_EV_GUILD_SCHEDULED_EVENT_UPDATE,
    WINECORD_EV_GUILD_SCHEDULED_EVENT_DELETE,
    WINECORD_EV_GUILD_SCHEDULED_EVENT_USER_ADD,
    WINECORD_EV_GUILD_SCHEDULED_EVENT_USER_REMOVE,
    WINECORD_EV_INTEGRATION_CREATE,
    WINECORD_EV_INTEGRATION_UPDATE,
    WINECORD_EV_INTEGRATION_DELETE,
    WINECORD_EV_INTERACTION_CREATE,
    WINECORD_EV_INVITE_CREATE,
    WINECORD_EV_INVITE_DELETE,
    WINECORD_EV_MESSAGE_CREATE,
    WINECORD_EV_MESSAGE_UPDATE,
    WINECORD_EV_MESSAGE_DELETE,
    WINECORD_EV_MESSAGE_DELETE_BULK,
    WINECORD_EV_MESSAGE_REACTION_ADD,
    WINECORD_EV_MESSAGE_REACTION_REMOVE,
    WINECORD_EV_MESSAGE_REACTION_REMOVE_ALL,
    WINECORD_EV_MESSAGE_REACTION_REMOVE_EMOJI,
    WINECORD_EV_PRESENCE_UPDATE,
    WINECORD_EV_STAGE_INSTANCE_CREATE,
    WINECORD_EV_STAGE_INSTANCE_DELETE,
    WINECORD_EV_STAGE_INSTANCE_UPDATE,
    WINECORD_EV_TYPING_START,
    WINECORD_EV_USER_UPDATE,
    WINECORD_EV_VOICE_STATE_UPDATE,
    WINECORD_EV_VOICE_SERVER_UPDATE,
    WINECORD_EV_WEBHOOKS_UPDATE,
    WINECORD_EV_MAX /**< total amount of enumerators */
};

/**
 * @brief return value of winecord_set_event_scheduler() callback
 * @see winecord_set_event_scheduler()
 */
typedef enum winecord_event_scheduler {
    /** this event has been handled */
    WINECORD_EVENT_IGNORE,
    /** handle this event in main thread */
    WINECORD_EVENT_MAIN_THREAD,
    /**
     * handle this event in a worker thread
     * @deprecated functionality will be removed in the future
     */
    WINECORD_EVENT_WORKER_THREAD
} winecord_event_scheduler_t;

/**
 * @brief Event Handling Mode callback
 *
 * A very important callback that enables the user with a fine-grained control
 *        of how each event is handled: blocking, non-blocking or ignored
 * @see winecord_set_event_scheduler(), @ref winecord_gateway_events
 */
typedef enum winecord_event_scheduler (*winecord_ev_scheduler)(
    struct winecord *client,
    const char data[],
    size_t size,
    enum winecord_gateway_events event);

/**
 * @brief Provides control over Winecord event's callback scheduler
 * @see @ref winecord_event_scheduler, @ref winecord_gateway_events
 *
 * Allows the user to scan the preliminary raw JSON event payload, and control
 *      whether it should trigger callbacks
 * @param client the client created_with winecord_init()
 * @param fn the function that will be executed
 * @warning The user is responsible for providing their own locking mechanism
 *      to avoid race-condition on sensitive data
 */
void winecord_set_event_scheduler(struct winecord *client,
                                 winecord_ev_scheduler callback);

/**
 * @brief Subscribe to Winecord Events
 *
 * @param client the client created with winecord_init()
 * @param code the intents opcode, can be set as a bitmask operation
 */
void winecord_add_intents(struct winecord *client, uint64_t code);

/**
 * @brief Unsubscribe from Winecord Events
 *
 * @param client the client created with winecord_init()
 * @param code the intents opcode, can be set as bitmask operation
 *        Ex: 1 << 0 | 1 << 1 | 1 << 4
 */
void winecord_remove_intents(struct winecord *client, uint64_t code);

/**
 * @brief Set a mandatory prefix before commands
 * @see winecord_set_on_command()
 *
 * Example: If @a 'help' is a command and @a '!' prefix is set, the command
 *       will only be validated if @a '!help' is sent
 * @param client the client created with winecord_init()
 * @param prefix the mandatory command prefix
 */
void winecord_set_prefix(struct winecord *client, const char prefix[]);

/**
 * @brief Set command/callback pair
 *
 * The callback is triggered when a user types the assigned command in a
 *        chat visible to the client
 * @param client the client created with winecord_init()
 * @param command the command to trigger the callback
 * @param callback the callback to be triggered on event
 * @note The command and any subjacent empty space is left out of
 *       the message content
 */
void winecord_set_on_command(
    struct winecord *client,
    char *command,
    void (*callback)(struct winecord *client,
                     const struct winecord_message *event));

/**
 * @brief Set a variadic series of NULL terminated commands to a callback
 *
 * The callback is triggered when a user types one of the assigned commands in
 *        a chat visble to the client
 * @param client the client created with winecord_init()
 * @param commands array of commands to trigger the callback
 * @param amount amount of commands provided
 * @param callback the callback to be triggered on event
 * @note The command and any subjacent empty space is left out of
 *       the message content
 */
void winecord_set_on_commands(
    struct winecord *client,
    char *const commands[],
    int amount,
    void (*callback)(struct winecord *client,
                     const struct winecord_message *event));

/**
 * @brief Set the time for wakeup function to be called
 * @see winecord_set_on_wakeup
 * @deprecated since v2.1.0, rely on @ref WinecordTimer instead
 *
 * @param delay time to delay in milliseconds, or -1 to disable
 */
void winecord_set_next_wakeup(struct winecord *client, int64_t delay);

/**
 * @brief Triggered at a arbitrary interval value set at
 *      winecord_set_next_wakeup()
 * @note This is a Winecord custom event
 * @deprecated since v2.1.0, rely on @ref WinecordTimer instead
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_wakeup(struct winecord *client,
                           void (*callback)(struct winecord *client));

/**
 * @brief Triggers when idle
 * @note This is a Winecord custom event
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_idle(struct winecord *client,
                         void (*callback)(struct winecord *client));

/**
 * @brief Triggers once per event-loop cycle
 * @note This is a Winecord custom event
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_cycle(struct winecord *client,
                          void (*callback)(struct winecord *client));

/**
 * @brief Triggers when the client session is ready
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_ready(struct winecord *client,
                          void (*callback)(struct winecord *client,
                                           const struct winecord_ready *event));

/**
 * @brief Triggers when an application command permission is updated
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_application_command_permissions_update(
    struct winecord *client,
    void (*callback)(
        struct winecord *client,
        const struct winecord_application_command_permissions *event));

/**
 * @brief Triggers when an auto moderation rule is created
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_AUTO_MODERATION_CONFIGURATION intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_auto_moderation_rule_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_auto_moderation_rule *event));

/**
 * @brief Triggers when an auto moderation rule is updated
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_AUTO_MODERATION_CONFIGURATION intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_auto_moderation_rule_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_auto_moderation_rule *event));

/**
 * @brief Triggers when an auto moderation rule is deleted
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_AUTO_MODERATION_CONFIGURATION intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_auto_moderation_rule_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_auto_moderation_rule *event));

/**
 * @brief Triggers when an auto moderation rule is triggered and an execution
 *      is executed (e.g a message was blocked)
 * @note This implicitly sets @ref WINECORD_GATEWAY_AUTO_MODERATION_EXECUTION
 *      intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_auto_moderation_action_execution(
    struct winecord *client,
    void (*callback)(
        struct winecord *client,
        const struct winecord_auto_moderation_action_execution *event));

/**
 * @brief Triggers when a channel is created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_channel_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_channel *event));

/**
 * @brief Triggers when a channel is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_channel_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_channel *event));

/**
 * @brief Triggers when a channel is deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_channel_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_channel *event));

/**
 * @brief Triggers when a channel pin is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGES intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_channel_pins_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_channel_pins_update *event));

/**
 * @brief Triggers when a thread is created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_thread_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_channel *event));

/**
 * @brief Triggers when a thread is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_thread_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_channel *event));

/**
 * @brief Triggers when a thread is deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_thread_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_channel *event));

/**
 * @brief Triggers when the current user gains access to a channel
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_thread_list_sync(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_thread_list_sync *event));

/**
 * @brief Triggers when a thread the bot is in gets updated
 * @note For bots, this event largely is just a signal that you are a member of
 *      the thread
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_thread_member_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_thread_member *event));

/**
 * @brief Triggers when someone is added or removed from a thread
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS and
 *      @ref WINECORD_GATEWAY_GUILD_MEMBERS intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_thread_members_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_thread_members_update *event));

/**
 * @brief Triggers when a guild is created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild *event));

/**
 * @brief Triggers when a guild is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild *event));

/**
 * @brief Triggers when a guild is deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild *event));

/**
 * @brief Triggers when a user is banned from a guild
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_BANS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_ban_add(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_ban_add *event));

/**
 * @brief Triggers when a user is unbanned from a guild
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_BANS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_ban_remove(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_ban_remove *event));

/**
 * @brief Triggers when a guild emojis are updated
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_GUILD_EMOJIS_AND_STICKERS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_emojis_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_emojis_update *event));

/**
 * @brief Triggers when a guild stickers are updated
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_GUILD_EMOJIS_AND_STICKERS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_stickers_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_stickers_update *event));

/**
 * @brief Triggers when a guild integrations are updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_INTEGRATIONS
 *      intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_integrations_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_integrations_update *event));

/**
 * @brief Triggers when a guild member is added
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_MEMBERS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_member_add(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_member *event));

/**
 * @brief Triggers when a guild member is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_MEMBERS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_member_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_member_update *event));

/**
 * @brief Triggers when a guild member is removed
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_MEMBERS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_member_remove(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_member_remove *event));

/**
 * @brief Triggers in response to winecord_request_guild_members()
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_members_chunk(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_members_chunk *event));

/**
 * @brief Triggers when a guild role is created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_role_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_role_create *event));

/**
 * @brief Triggers when a guild role is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_role_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_role_update *event));

/**
 * @brief Triggers when a guild role is deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_role_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_role_delete *event));

/**
 * @brief Triggers when a guild scheduled event is created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_SCHEDULED_EVENTS
 * intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_scheduled_event_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_scheduled_event *event));

/**
 * @brief Triggers when a guild scheduled event is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_SCHEDULED_EVENTS
 * intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_scheduled_event_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_scheduled_event *event));

/**
 * @brief Triggers when a guild scheduled event is deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_SCHEDULED_EVENTS
 * intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_scheduled_event_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_guild_scheduled_event *event));

/**
 * @brief Triggers when a user subscribes to a guild scheduled event
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_SCHEDULED_EVENTS
 * intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_scheduled_event_user_add(
    struct winecord *client,
    void (*callback)(
        struct winecord *client,
        const struct winecord_guild_scheduled_event_user_add *event));

/**
 * @brief Triggers when a user unsubscribes from a guild scheduled event
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_SCHEDULED_EVENTS
 * intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_guild_scheduled_event_user_remove(
    struct winecord *client,
    void (*callback)(
        struct winecord *client,
        const struct winecord_guild_scheduled_event_user_remove *event));

/**
 * @brief Triggers when a guild integration is created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_INTEGRATIONS
 * intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_integration_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_integration *event));

/**
 * @brief Triggers when a guild integration is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_INTEGRATIONS
 * intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_integration_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_integration *event));

/**
 * @brief Triggers when a guild integration is deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_INTEGRATIONS
 *      intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_integration_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_integration_delete *event));

/**
 * @brief Triggers when user has used an interaction, such as an application
 *      command
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_interaction_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_interaction *event));

/**
 * @brief Triggers when an invite to a channel has been created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_INVITES intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_invite_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_invite_create *event));

/**
 * @brief Triggers when an invite to a channel has been deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_INVITES intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_invite_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_invite_delete *event));

/**
 * @brief Triggers when a message is created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_MESSAGES and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGES intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_message_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_message *event));

/**
 * @brief Triggers when a message is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_MESSAGES and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGES intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_message_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_message *event));

/**
 * @brief Triggers when a message is deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_MESSAGES and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGES intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_message_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_message_delete *event));

/**
 * @brief Triggers when messages are deleted in bulk
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_MESSAGES
 *      intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_message_delete_bulk(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_message_delete_bulk *event));

/**
 * @brief Triggers when a message reaction is added
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_GUILD_MESSAGE_REACTIONS and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGE_REACTIONS intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_message_reaction_add(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_message_reaction_add *event));

/**
 * @brief Triggers when a message reaction is removed
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_GUILD_MESSAGE_REACTIONS and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGE_REACTIONS intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_message_reaction_remove(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_message_reaction_remove *event));

/**
 * @brief Triggers when all message reactions are removed
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_GUILD_MESSAGE_REACTIONS and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGE_REACTIONS intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_message_reaction_remove_all(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_message_reaction_remove_all *event));
/** @brief Triggers when all instances of a particular reaction from some
 *        message is removed */

/**
 * @brief Triggers when all instances of a particular reaction is removed from
 *      a message
 * @note This implicitly sets
 *      @ref WINECORD_GATEWAY_GUILD_MESSAGE_REACTIONS and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGE_REACTIONS intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_message_reaction_remove_emoji(
    struct winecord *client,
    void (*callback)(
        struct winecord *client,
        const struct winecord_message_reaction_remove_emoji *event));

/**
 * @brief Triggers when user presence is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_PRESENCES intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_presence_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_presence_update *event));

/**
 * @brief Triggers when a stage instance is created
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_stage_instance_create(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                    const struct winecord_stage_instance *event));

/**
 * @brief Triggers when a stage instance is updated
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_stage_instance_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_stage_instance *event));

/**
 * @brief Triggers when a stage instance is deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILDS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_stage_instance_delete(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_stage_instance *event));

/**
 * @brief Triggers when user starts typing in a channel
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_MESSAGE_TYPING and
 *      @ref WINECORD_GATEWAY_DIRECT_MESSAGE_TYPING intents
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_typing_start(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_typing_start *event));

/**
 * @brief Triggers when properties about a user changed
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_user_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_user *event));

/**
 * @brief Triggers when a voice state is updated
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_voice_state_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_voice_state *event));

/**
 * @brief Triggers when voice server is updated
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_voice_server_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_voice_server_update *event));

/**
 * @brief Triggers when guild channel has been created, updated or deleted
 * @note This implicitly sets @ref WINECORD_GATEWAY_GUILD_WEBHOOKS intent
 *
 * @param client the client created with winecord_init()
 * @param callback the callback to be triggered on event
 */
void winecord_set_on_webhooks_update(
    struct winecord *client,
    void (*callback)(struct winecord *client,
                     const struct winecord_webhooks_update *event));

/** @} WinecordEvents */

#endif /* WINECORD_EVENTS_H */
