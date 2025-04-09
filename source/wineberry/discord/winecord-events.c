#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"

#define GATEWAY_CB(EV) client->gw.cbs[1][EV]
#define ASSIGN_CB(EV, CB) GATEWAY_CB(EV) = (winecord_ev_event)CB

void
winecord_shutdown(struct winecord *client)
{
    if (client->gw.session->status != WINEBERRY_SESSION_SHUTDOWN)
        winecord_gateway_shutdown(&client->gw);
}

void
winecord_reconnect(struct winecord *client, bool resume)
{
    winecord_gateway_reconnect(&client->gw, resume);
}

void
winecord_request_guild_members(struct winecord *client,
                              struct winecord_request_guild_members *request)
{
    ASSERT_S(GATEWAY_CB(WINEBERRY_EV_GUILD_MEMBERS_CHUNK) != NULL,
             "Missing callback for winecord_set_on_guild_members_chunk()");
    winecord_gateway_send_request_guild_members(&client->gw, request);
}

void
winecord_update_voice_state(struct winecord *client,
                           struct winecord_update_voice_state *update)
{
    winecord_gateway_send_update_voice_state(&client->gw, update);
}

void
winecord_update_presence(struct winecord *client,
                        struct winecord_presence_update *presence)
{
    winecord_gateway_send_presence_update(&client->gw, presence);
}

/* deprecated, use winecord_update_presence() instead */
void
winecord_set_presence(struct winecord *client,
                     struct winecord_presence_update *presence)
{
    winecord_update_presence(client, presence);
}

void
winecord_add_intents(struct winecord *client, uint64_t code)
{
    if (WS_CONNECTED == ws_get_status(client->gw.ws)) {
        logconf_error(&client->conf, "Can't set intents to a running client.");
        return;
    }

    client->gw.id.intents |= code;
}

void
winecord_remove_intents(struct winecord *client, uint64_t code)
{
    if (WS_CONNECTED == ws_get_status(client->gw.ws)) {
        logconf_error(&client->conf,
                      "Can't remove intents from a running client.");
        return;
    }

    client->gw.id.intents &= ~code;
}

void
winecord_set_prefix(struct winecord *client, const char prefix[])
{
    if (!prefix || !*prefix) return;

    winecord_message_commands_set_prefix(&client->commands, prefix,
                                        strlen(prefix));
}

void
winecord_set_event_scheduler(struct winecord *client, winecord_ev_scheduler cb)
{
    client->gw.scheduler = cb;
}

void
winecord_set_on_command(struct winecord *client,
                       char command[],
                       void (*cb)(struct winecord *client,
                                  const struct winecord_message *event))
{
    size_t length = (!command || !*command) ? 0 : strlen(command);

    winecord_message_commands_append(&client->commands, command, length, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MESSAGES
                                    | WINEBERRY_GATEWAY_DIRECT_MESSAGES
                                    | WINEBERRY_GATEWAY_MESSAGE_CONTENT);
}

void
winecord_set_on_commands(struct winecord *client,
                        char *const commands[],
                        int amount,
                        void (*cb)(struct winecord *client,
                                   const struct winecord_message *event))
{
    for (int i = 0; i < amount; ++i)
        winecord_set_on_command(client, commands[i], cb);
}

void
winecord_set_on_ready(struct winecord *client,
                     void (*cb)(struct winecord *client,
                                const struct winecord_ready *event))
{
    ASSIGN_CB(WINEBERRY_EV_READY, cb);
}

void
winecord_set_on_application_command_permissions_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_application_command_permissions *event))
{
    ASSIGN_CB(WINEBERRY_EV_APPLICATION_COMMAND_PERMISSIONS_UPDATE, cb);
}

void
winecord_set_on_auto_moderation_rule_create(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_auto_moderation_rule *event))
{
    ASSIGN_CB(WINEBERRY_EV_AUTO_MODERATION_RULE_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_AUTO_MODERATION_CONFIGURATION);
}

void
winecord_set_on_auto_moderation_rule_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_auto_moderation_rule *event))
{
    ASSIGN_CB(WINEBERRY_EV_AUTO_MODERATION_RULE_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_AUTO_MODERATION_CONFIGURATION);
}

void
winecord_set_on_auto_moderation_rule_delete(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_auto_moderation_rule *event))
{
    ASSIGN_CB(WINEBERRY_EV_AUTO_MODERATION_RULE_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_AUTO_MODERATION_CONFIGURATION);
}

void
winecord_set_on_auto_moderation_action_execution(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_auto_moderation_action_execution *event))
{
    ASSIGN_CB(WINEBERRY_EV_AUTO_MODERATION_ACTION_EXECUTION, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_AUTO_MODERATION_EXECUTION);
}

void
winecord_set_on_channel_create(struct winecord *client,
                              void (*cb)(struct winecord *client,
                                         const struct winecord_channel *event))
{
    ASSIGN_CB(WINEBERRY_EV_CHANNEL_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_channel_update(struct winecord *client,
                              void (*cb)(struct winecord *client,
                                         const struct winecord_channel *event))
{
    ASSIGN_CB(WINEBERRY_EV_CHANNEL_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_channel_delete(struct winecord *client,
                              void (*cb)(struct winecord *client,
                                         const struct winecord_channel *event))
{
    ASSIGN_CB(WINEBERRY_EV_CHANNEL_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_channel_pins_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_channel_pins_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_CHANNEL_PINS_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS
                                    | WINEBERRY_GATEWAY_DIRECT_MESSAGES);
}

void
winecord_set_on_thread_create(struct winecord *client,
                             void (*cb)(struct winecord *client,
                                        const struct winecord_channel *event))
{
    ASSIGN_CB(WINEBERRY_EV_THREAD_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_thread_update(struct winecord *client,
                             void (*cb)(struct winecord *client,
                                        const struct winecord_channel *event))
{
    ASSIGN_CB(WINEBERRY_EV_THREAD_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_thread_delete(struct winecord *client,
                             void (*cb)(struct winecord *client,
                                        const struct winecord_channel *event))
{
    ASSIGN_CB(WINEBERRY_EV_THREAD_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_thread_list_sync(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_thread_list_sync *event))
{
    ASSIGN_CB(WINEBERRY_EV_THREAD_LIST_SYNC, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_thread_member_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_thread_member *event))
{
    ASSIGN_CB(WINEBERRY_EV_THREAD_MEMBER_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_thread_members_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_thread_members_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_THREAD_MEMBERS_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS
                                    | WINEBERRY_GATEWAY_GUILD_MEMBERS);
}

void
winecord_set_on_guild_create(struct winecord *client,
                            void (*cb)(struct winecord *client,
                                       const struct winecord_guild *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_guild_update(struct winecord *client,
                            void (*cb)(struct winecord *client,
                                       const struct winecord_guild *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_guild_delete(struct winecord *client,
                            void (*cb)(struct winecord *client,
                                       const struct winecord_guild *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_guild_ban_add(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_ban_add *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_BAN_ADD, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_BANS);
}

void
winecord_set_on_guild_ban_remove(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_ban_remove *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_BAN_REMOVE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_BANS);
}

void
winecord_set_on_guild_emojis_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_emojis_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_EMOJIS_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_EMOJIS_AND_STICKERS);
}

void
winecord_set_on_guild_stickers_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_stickers_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_STICKERS_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_EMOJIS_AND_STICKERS);
}

void
winecord_set_on_guild_integrations_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_integrations_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_INTEGRATIONS_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_INTEGRATIONS);
}

void
winecord_set_on_guild_member_add(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_member *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_MEMBER_ADD, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MEMBERS);
}

void
winecord_set_on_guild_member_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_member_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_MEMBER_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MEMBERS);
}

void
winecord_set_on_guild_member_remove(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_member_remove *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_MEMBER_REMOVE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MEMBERS);
}

void
winecord_set_on_guild_members_chunk(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_members_chunk *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_MEMBERS_CHUNK, cb);
}

void
winecord_set_on_guild_role_create(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_role_create *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_ROLE_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_guild_role_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_role_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_ROLE_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_guild_role_delete(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_role_delete *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_ROLE_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_guild_scheduled_event_create(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_scheduled_event *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_SCHEDULED_EVENT_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_SCHEDULED_EVENTS);
}

void
winecord_set_on_guild_scheduled_event_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_scheduled_event *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_SCHEDULED_EVENT_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_SCHEDULED_EVENTS);
}

void
winecord_set_on_guild_scheduled_event_delete(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_scheduled_event *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_SCHEDULED_EVENT_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_SCHEDULED_EVENTS);
}

void
winecord_set_on_guild_scheduled_event_user_add(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_scheduled_event_user_add *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_SCHEDULED_EVENT_USER_ADD, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_SCHEDULED_EVENTS);
}

void
winecord_set_on_guild_scheduled_event_user_remove(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_guild_scheduled_event_user_remove *event))
{
    ASSIGN_CB(WINEBERRY_EV_GUILD_SCHEDULED_EVENT_USER_REMOVE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_SCHEDULED_EVENTS);
}

void
winecord_set_on_integration_create(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_integration *event))
{
    ASSIGN_CB(WINEBERRY_EV_INTEGRATION_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_INTEGRATIONS);
}

void
winecord_set_on_integration_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_integration *event))
{
    ASSIGN_CB(WINEBERRY_EV_INTEGRATION_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_INTEGRATIONS);
}

void
winecord_set_on_integration_delete(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_integration_delete *event))
{
    ASSIGN_CB(WINEBERRY_EV_INTEGRATION_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_INTEGRATIONS);
}

void
winecord_set_on_interaction_create(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_interaction *event))
{
    ASSIGN_CB(WINEBERRY_EV_INTERACTION_CREATE, cb);
}

void
winecord_set_on_invite_create(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_invite_create *event))
{
    ASSIGN_CB(WINEBERRY_EV_INVITE_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_INVITES);
}

void
winecord_set_on_invite_delete(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_invite_delete *event))
{
    ASSIGN_CB(WINEBERRY_EV_INVITE_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_INVITES);
}

void
winecord_set_on_message_create(struct winecord *client,
                              void (*cb)(struct winecord *client,
                                         const struct winecord_message *event))
{
    ASSIGN_CB(WINEBERRY_EV_MESSAGE_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MESSAGES
                                    | WINEBERRY_GATEWAY_DIRECT_MESSAGES);
}

void
winecord_set_on_message_update(struct winecord *client,
                              void (*cb)(struct winecord *client,
                                         const struct winecord_message *event))
{
    ASSIGN_CB(WINEBERRY_EV_MESSAGE_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MESSAGES
                                    | WINEBERRY_GATEWAY_DIRECT_MESSAGES);
}

void
winecord_set_on_message_delete(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_message_delete *event))
{
    ASSIGN_CB(WINEBERRY_EV_MESSAGE_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MESSAGES
                                    | WINEBERRY_GATEWAY_DIRECT_MESSAGES);
}

void
winecord_set_on_message_delete_bulk(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_message_delete_bulk *event))
{
    ASSIGN_CB(WINEBERRY_EV_MESSAGE_DELETE_BULK, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MESSAGES);
}

void
winecord_set_on_message_reaction_add(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_message_reaction_add *event))
{
    ASSIGN_CB(WINEBERRY_EV_MESSAGE_REACTION_ADD, cb);
    winecord_add_intents(client,
                        WINEBERRY_GATEWAY_GUILD_MESSAGE_REACTIONS
                            | WINEBERRY_GATEWAY_DIRECT_MESSAGE_REACTIONS);
}

void
winecord_set_on_message_reaction_remove(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_message_reaction_remove *event))
{
    ASSIGN_CB(WINEBERRY_EV_MESSAGE_REACTION_REMOVE, cb);
    winecord_add_intents(client,
                        WINEBERRY_GATEWAY_GUILD_MESSAGE_REACTIONS
                            | WINEBERRY_GATEWAY_DIRECT_MESSAGE_REACTIONS);
}

void
winecord_set_on_message_reaction_remove_all(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_message_reaction_remove_all *event))
{
    ASSIGN_CB(WINEBERRY_EV_MESSAGE_REACTION_REMOVE_ALL, cb);
    winecord_add_intents(client,
                        WINEBERRY_GATEWAY_GUILD_MESSAGE_REACTIONS
                            | WINEBERRY_GATEWAY_DIRECT_MESSAGE_REACTIONS);
}

void
winecord_set_on_message_reaction_remove_emoji(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_message_reaction_remove_emoji *event))
{
    ASSIGN_CB(WINEBERRY_EV_MESSAGE_REACTION_REMOVE_EMOJI, cb);
    winecord_add_intents(client,
                        WINEBERRY_GATEWAY_GUILD_MESSAGE_REACTIONS
                            | WINEBERRY_GATEWAY_DIRECT_MESSAGE_REACTIONS);
}

void
winecord_set_on_presence_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_presence_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_PRESENCE_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_PRESENCES);
}

void
winecord_set_on_stage_instance_create(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_stage_instance *event))
{
    ASSIGN_CB(WINEBERRY_EV_STAGE_INSTANCE_CREATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_stage_instance_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_stage_instance *event))
{
    ASSIGN_CB(WINEBERRY_EV_STAGE_INSTANCE_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_stage_instance_delete(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_stage_instance *event))
{
    ASSIGN_CB(WINEBERRY_EV_STAGE_INSTANCE_DELETE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILDS);
}

void
winecord_set_on_typing_start(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_typing_start *event))
{
    ASSIGN_CB(WINEBERRY_EV_TYPING_START, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_MESSAGE_TYPING
                                    | WINEBERRY_GATEWAY_DIRECT_MESSAGE_TYPING);
}

void
winecord_set_on_user_update(struct winecord *client,
                           void (*cb)(struct winecord *client,
                                      const struct winecord_user *event))
{
    ASSIGN_CB(WINEBERRY_EV_USER_UPDATE, cb);
}

void
winecord_set_on_voice_state_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_voice_state *event))
{
    ASSIGN_CB(WINEBERRY_EV_VOICE_STATE_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_VOICE_STATES);
}

void
winecord_set_on_voice_server_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_voice_server_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_VOICE_SERVER_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_VOICE_STATES);
}

void
winecord_set_on_webhooks_update(
    struct winecord *client,
    void (*cb)(struct winecord *client,
               const struct winecord_webhooks_update *event))
{
    ASSIGN_CB(WINEBERRY_EV_WEBHOOKS_UPDATE, cb);
    winecord_add_intents(client, WINEBERRY_GATEWAY_GUILD_WEBHOOKS);
}
