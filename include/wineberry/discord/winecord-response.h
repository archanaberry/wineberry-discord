/**
 * @file winecord-response.h
 * @author Cogmasters
 * @brief Generic macros for initializing a @ref winecord_response and return
 *      handles
 */

#ifndef WINECORD_RESPONSE_H
#define WINECORD_RESPONSE_H

/** @brief The response for the completed request */
struct winecord_response {
    /** user arbitrary data provided at @ref winecord_ret */
    void *data;
    /** kept concord's parameter provided at @ref winecord_ret */
    const void *keep;
    /** request completion status @see @ref WinecordError */
    WINEBERRYcode code;
};

/******************************************************************************
 * Templates for generating type-safe return handles for async requests
 ******************************************************************************/

/**
 * @brief Macro containing common fields for `struct winecord_ret*` datatypes
 * @note this exists for alignment purposes
 */
#define WINEBERRY_RET_DEFAULT_FIELDS                                          \
    /** user arbitrary data to be passed to `done` or `fail` callbacks */     \
    void *data;                                                               \
    /** cleanup method to be called for `data`, once its no longer            \
    being referenced */                                                       \
    void (*cleanup)(struct winecord * client, void *data);                    \
    /** Winecord callback parameter the client wish to keep reference */      \
    const void *keep;                                                         \
    /** if `true` then request will be prioritized over already enqueued      \
        requests */                                                           \
    bool high_priority;                                                       \
    /** optional callback to be executed on a failed request */               \
    void (*fail)(struct winecord * client, struct winecord_response * resp)

#define WINECORD_RETURN(_type)                                                \
    /** @brief Request's return context */                                    \
    struct winecord_ret_##_type {                                             \
        WINEBERRY_RET_DEFAULT_FIELDS;                                         \
        /** optional callback to be executed on a successful request */       \
        void (*done)(struct winecord * client,                                \
                     struct winecord_response *resp,                          \
                     const struct winecord_##_type *ret);                     \
        /** if an address is provided, then request will block the thread and \
           perform on-spot.                                                   \
           On success the response object will be written to the address,     \
           unless enabled with @ref WINECORD_SYNC_FLAG */                     \
        struct winecord_##_type *sync;                                        \
    }

/** @brief Request's return context */
struct winecord_ret {
    WINEBERRY_RET_DEFAULT_FIELDS;
    /** optional callback to be executed on a successful request */
    void (*done)(struct winecord *client, struct winecord_response *resp);
    /** if `true`, request will block the thread and perform on-spot */
    bool sync;
};

/** @brief flag for enabling `sync` mode without expecting a datatype return */
#define WINECORD_SYNC_FLAG ((void *)-1)

/** @addtogroup WinecordAPIOAuth2
 *  @{ */
WINECORD_RETURN(application);
WINECORD_RETURN(auth_response);
/** @} WinecordAPIOAuth2 */

/** @addtogroup WinecordAPIAuditLog
 *  @{ */
WINECORD_RETURN(audit_log);
/** @} WinecordAPIAuditLog */

/** @addtogroup WinecordAPIAutoModeration
 *  @{ */
WINECORD_RETURN(auto_moderation_rule);
WINECORD_RETURN(auto_moderation_rules);
/** @} WinecordAPIAutoModeration */

/** @addtogroup WinecordAPIChannel
 *  @{ */
WINECORD_RETURN(channel);
WINECORD_RETURN(channels);
WINECORD_RETURN(message);
WINECORD_RETURN(messages);
WINECORD_RETURN(followed_channel);
WINECORD_RETURN(thread_members);
WINECORD_RETURN(thread_response_body);
/** @} WinecordAPIChannel */

/** @addtogroup WinecordAPIEmoji
 *  @{ */
WINECORD_RETURN(emoji);
WINECORD_RETURN(emojis);
/** @} WinecordAPIEmoji */

/** @addtogroup WinecordAPIGuild
 *  @{ */
WINECORD_RETURN(guild);
WINECORD_RETURN(guilds);
WINECORD_RETURN(guild_preview);
WINECORD_RETURN(guild_member);
WINECORD_RETURN(guild_members);
WINECORD_RETURN(guild_widget);
WINECORD_RETURN(guild_widget_settings);
WINECORD_RETURN(ban);
WINECORD_RETURN(bans);
WINECORD_RETURN(role);
WINECORD_RETURN(roles);
WINECORD_RETURN(welcome_screen);
WINECORD_RETURN(integrations);
WINECORD_RETURN(prune_count);
/** @} WinecordAPIGuild */

/** @addtogroup WinecordAPIGuildScheduledEvent
 *  @{ */
WINECORD_RETURN(guild_scheduled_event);
WINECORD_RETURN(guild_scheduled_events);
WINECORD_RETURN(guild_scheduled_event_users);
/** @} WinecordAPIGuildScheduledEvent */

/** @addtogroup WinecordAPIGuildTemplate
 *  @{ */
WINECORD_RETURN(guild_template);
WINECORD_RETURN(guild_templates);
/** @} WinecordAPIGuildTemplate */

/** @addtogroup WinecordAPIInvite
 *  @{ */
WINECORD_RETURN(invite);
WINECORD_RETURN(invites);
/** @} WinecordAPIInvite */

/** @addtogroup WinecordAPIStageInstance
 *  @{ */
WINECORD_RETURN(stage_instance);
/** @} WinecordAPIStageInstance */

/** @addtogroup WinecordAPISticker
 *  @{ */
WINECORD_RETURN(sticker);
WINECORD_RETURN(stickers);
WINECORD_RETURN(list_nitro_sticker_packs);
/** @} WinecordAPISticker */

/** @addtogroup WinecordAPIUser
 *  @{ */
WINECORD_RETURN(user);
WINECORD_RETURN(users);
WINECORD_RETURN(connections);
/** @} WinecordAPIUser */

/** @addtogroup WinecordAPIVoice
 *  @{ */
WINECORD_RETURN(voice_regions);
/** @} WinecordAPIVoice */

/** @addtogroup WinecordAPIWebhook
 *  @{ */
WINECORD_RETURN(webhook);
WINECORD_RETURN(webhooks);
/** @} WinecordAPIWebhook */

/** @addtogroup WinecordAPIInteractionsApplicationCommand
 * @ingroup WinecordAPIInteractions
 *  @{ */
WINECORD_RETURN(application_command);
WINECORD_RETURN(application_commands);
WINECORD_RETURN(application_command_permission);
WINECORD_RETURN(application_command_permissions);
WINECORD_RETURN(guild_application_command_permissions);
/** @} WinecordAPIInteractionsApplicationCommand */

/** @addtogroup WinecordAPIInteractionsReact
 * @ingroup WinecordAPIInteractions
 *  @{ */
WINECORD_RETURN(interaction_response);
/** @} WinecordAPIInteractionsReact */

#endif /* WINECORD_RESPONSE_H */
