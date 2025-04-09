#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"

#define INIT(type)                                                            \
    {                                                                         \
        sizeof(struct type),                                                  \
            (size_t(*)(jsmnf_pair *, const char *, void *))type##_from_jsmnf, \
            (void (*)(void *))type##_cleanup                                  \
    }

/** @brief Information for deserializing a winecord event */
static const struct {
    /** size of event's datatype */
    size_t size;
    /** event's payload deserializer */
    size_t (*from_jsmnf)(jsmnf_pair *, const char *, void *);
    /** event's cleanup */
    void (*cleanup)(void *);
} dispatch[] = {
    [WINEBERRY_EV_READY] = INIT(winecord_ready),
    [WINEBERRY_EV_APPLICATION_COMMAND_PERMISSIONS_UPDATE] =
        INIT(winecord_application_command_permissions),
    [WINEBERRY_EV_AUTO_MODERATION_RULE_CREATE] =
        INIT(winecord_auto_moderation_rule),
    [WINEBERRY_EV_AUTO_MODERATION_RULE_UPDATE] =
        INIT(winecord_auto_moderation_rule),
    [WINEBERRY_EV_AUTO_MODERATION_RULE_DELETE] =
        INIT(winecord_auto_moderation_rule),
    [WINEBERRY_EV_AUTO_MODERATION_ACTION_EXECUTION] =
        INIT(winecord_auto_moderation_action_execution),
    [WINEBERRY_EV_CHANNEL_CREATE] = INIT(winecord_channel),
    [WINEBERRY_EV_CHANNEL_UPDATE] = INIT(winecord_channel),
    [WINEBERRY_EV_CHANNEL_DELETE] = INIT(winecord_channel),
    [WINEBERRY_EV_CHANNEL_PINS_UPDATE] = INIT(winecord_channel_pins_update),
    [WINEBERRY_EV_THREAD_CREATE] = INIT(winecord_channel),
    [WINEBERRY_EV_THREAD_UPDATE] = INIT(winecord_channel),
    [WINEBERRY_EV_THREAD_DELETE] = INIT(winecord_channel),
    [WINEBERRY_EV_THREAD_LIST_SYNC] = INIT(winecord_thread_list_sync),
    [WINEBERRY_EV_THREAD_MEMBER_UPDATE] = INIT(winecord_thread_member),
    [WINEBERRY_EV_THREAD_MEMBERS_UPDATE] = INIT(winecord_thread_members_update),
    [WINEBERRY_EV_GUILD_CREATE] = INIT(winecord_guild),
    [WINEBERRY_EV_GUILD_UPDATE] = INIT(winecord_guild),
    [WINEBERRY_EV_GUILD_DELETE] = INIT(winecord_guild),
    [WINEBERRY_EV_GUILD_BAN_ADD] = INIT(winecord_guild_ban_add),
    [WINEBERRY_EV_GUILD_BAN_REMOVE] = INIT(winecord_guild_ban_remove),
    [WINEBERRY_EV_GUILD_EMOJIS_UPDATE] = INIT(winecord_guild_emojis_update),
    [WINEBERRY_EV_GUILD_STICKERS_UPDATE] = INIT(winecord_guild_stickers_update),
    [WINEBERRY_EV_GUILD_INTEGRATIONS_UPDATE] =
        INIT(winecord_guild_integrations_update),
    [WINEBERRY_EV_GUILD_MEMBER_ADD] = INIT(winecord_guild_member),
    [WINEBERRY_EV_GUILD_MEMBER_UPDATE] = INIT(winecord_guild_member_update),
    [WINEBERRY_EV_GUILD_MEMBER_REMOVE] = INIT(winecord_guild_member_remove),
    [WINEBERRY_EV_GUILD_MEMBERS_CHUNK] = INIT(winecord_guild_members_chunk),
    [WINEBERRY_EV_GUILD_ROLE_CREATE] = INIT(winecord_guild_role_create),
    [WINEBERRY_EV_GUILD_ROLE_UPDATE] = INIT(winecord_guild_role_update),
    [WINEBERRY_EV_GUILD_ROLE_DELETE] = INIT(winecord_guild_role_delete),
    [WINEBERRY_EV_GUILD_SCHEDULED_EVENT_CREATE] =
        INIT(winecord_guild_scheduled_event),
    [WINEBERRY_EV_GUILD_SCHEDULED_EVENT_UPDATE] =
        INIT(winecord_guild_scheduled_event),
    [WINEBERRY_EV_GUILD_SCHEDULED_EVENT_DELETE] =
        INIT(winecord_guild_scheduled_event),
    [WINEBERRY_EV_GUILD_SCHEDULED_EVENT_USER_ADD] =
        INIT(winecord_guild_scheduled_event_user_add),
    [WINEBERRY_EV_GUILD_SCHEDULED_EVENT_USER_REMOVE] =
        INIT(winecord_guild_scheduled_event_user_remove),
    [WINEBERRY_EV_INTEGRATION_CREATE] = INIT(winecord_integration),
    [WINEBERRY_EV_INTEGRATION_UPDATE] = INIT(winecord_integration),
    [WINEBERRY_EV_INTEGRATION_DELETE] = INIT(winecord_integration_delete),
    [WINEBERRY_EV_INTERACTION_CREATE] = INIT(winecord_interaction),
    [WINEBERRY_EV_INVITE_CREATE] = INIT(winecord_invite_create),
    [WINEBERRY_EV_INVITE_DELETE] = INIT(winecord_invite_delete),
    [WINEBERRY_EV_MESSAGE_CREATE] = INIT(winecord_message),
    [WINEBERRY_EV_MESSAGE_UPDATE] = INIT(winecord_message),
    [WINEBERRY_EV_MESSAGE_DELETE] = INIT(winecord_message_delete),
    [WINEBERRY_EV_MESSAGE_DELETE_BULK] = INIT(winecord_message_delete_bulk),
    [WINEBERRY_EV_MESSAGE_REACTION_ADD] = INIT(winecord_message_reaction_add),
    [WINEBERRY_EV_MESSAGE_REACTION_REMOVE] =
        INIT(winecord_message_reaction_remove),
    [WINEBERRY_EV_MESSAGE_REACTION_REMOVE_ALL] =
        INIT(winecord_message_reaction_remove_all),
    [WINEBERRY_EV_MESSAGE_REACTION_REMOVE_EMOJI] =
        INIT(winecord_message_reaction_remove_emoji),
    [WINEBERRY_EV_PRESENCE_UPDATE] = INIT(winecord_presence_update),
    [WINEBERRY_EV_STAGE_INSTANCE_CREATE] = INIT(winecord_stage_instance),
    [WINEBERRY_EV_STAGE_INSTANCE_UPDATE] = INIT(winecord_stage_instance),
    [WINEBERRY_EV_STAGE_INSTANCE_DELETE] = INIT(winecord_stage_instance),
    [WINEBERRY_EV_TYPING_START] = INIT(winecord_typing_start),
    [WINEBERRY_EV_USER_UPDATE] = INIT(winecord_user),
    [WINEBERRY_EV_VOICE_STATE_UPDATE] = INIT(winecord_voice_state),
    [WINEBERRY_EV_VOICE_SERVER_UPDATE] = INIT(winecord_voice_server_update),
    [WINEBERRY_EV_WEBHOOKS_UPDATE] = INIT(winecord_webhooks_update),
};

void
winecord_gateway_dispatch(struct winecord_gateway *gw)
{
    const enum winecord_gateway_events event = gw->payload.event;
    struct winecord *client = CLIENT(gw, gw);

    switch (event) {
    case WINEBERRY_EV_MESSAGE_CREATE:
        if (winecord_message_commands_try_perform(&client->commands,
                                                 &gw->payload)) {
            return;
        }
    /* fall-through */
    default:
        if (gw->cbs[0][event] || gw->cbs[1][event]) {
            void *event_data = calloc(1, dispatch[event].size);

            dispatch[event].from_jsmnf(gw->payload.data,
                                       gw->payload.json.start, event_data);

            if (WINEBERRY_RESOURCE_UNAVAILABLE
                == winecord_refcounter_incr(&client->refcounter, event_data))
            {
                winecord_refcounter_add_internal(&client->refcounter,
                                                event_data,
                                                dispatch[event].cleanup, true);
            }
            if (gw->cbs[0][event]) gw->cbs[0][event](client, event_data);
            if (gw->cbs[1][event]) gw->cbs[1][event](client, event_data);
            winecord_refcounter_decr(&client->refcounter, event_data);
        }
        break;
    case WINEBERRY_EV_NONE:
        logconf_warn(
            &gw->conf,
            "Expected unimplemented GATEWAY_DISPATCH event (code: %d)", event);
        break;
    }
}

void
winecord_gateway_send_identify(struct winecord_gateway *gw,
                              struct winecord_identify *identify)
{
    struct ws_info info = { 0 };
    char buf[2056];
    jsonb b;

    /* Ratelimit check */
    if (gw->timer->now - gw->timer->identify_last < 5) {
        ++gw->session->concurrent;
        VASSERT_S(gw->session->concurrent
                      < gw->session->start_limit.max_concurrency,
                  "Reach identify request threshold (%d every 5 seconds)",
                  gw->session->start_limit.max_concurrency);
    }
    else {
        gw->session->concurrent = 0;
    }

    jsonb_init(&b);
    jsonb_object(&b, buf, sizeof(buf));
    {
        jsonb_key(&b, buf, sizeof(buf), "op", 2);
        jsonb_number(&b, buf, sizeof(buf), 2);
        jsonb_key(&b, buf, sizeof(buf), "d", 1);
        winecord_identify_to_jsonb(&b, buf, sizeof(buf), identify);
        jsonb_object_pop(&b, buf, sizeof(buf));
    }

    if (ws_send_text(gw->ws, &info, buf, b.pos)) {
        io_poller_curlm_enable_perform(CLIENT(gw, gw)->io_poller, gw->mhandle);
        logconf_info(
            &gw->conf,
            ANSICOLOR(
                "SEND",
                ANSI_FG_BRIGHT_GREEN) " IDENTIFY (%d bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);

        /* get timestamp for this identify */
        gw->timer->identify_last = gw->timer->now;
    }
    else {
        logconf_info(
            &gw->conf,
            ANSICOLOR("FAIL SEND",
                      ANSI_FG_RED) " IDENTIFY (%d bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);
    }
}

void
winecord_gateway_send_resume(struct winecord_gateway *gw,
                            struct winecord_resume *event)
{
    struct ws_info info = { 0 };
    char buf[1024];
    jsonb b;

    /* reset */
    gw->session->status ^= WINEBERRY_SESSION_RESUMABLE;

    jsonb_init(&b);
    jsonb_object(&b, buf, sizeof(buf));
    {
        jsonb_key(&b, buf, sizeof(buf), "op", 2);
        jsonb_number(&b, buf, sizeof(buf), 6);
        jsonb_key(&b, buf, sizeof(buf), "d", 1);
        winecord_resume_to_jsonb(&b, buf, sizeof(buf), event);
        jsonb_object_pop(&b, buf, sizeof(buf));
    }

    if (ws_send_text(gw->ws, &info, buf, b.pos)) {
        io_poller_curlm_enable_perform(CLIENT(gw, gw)->io_poller, gw->mhandle);
        logconf_info(
            &gw->conf,
            ANSICOLOR("SEND",
                      ANSI_FG_BRIGHT_GREEN) " RESUME (%d bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);
    }
    else {
        logconf_info(&gw->conf,
                     ANSICOLOR("FAIL SEND",
                               ANSI_FG_RED) " RESUME (%d bytes) [@@@_%zu_@@@]",
                     b.pos, info.loginfo.counter + 1);
    }
}

static void
_winecord_on_heartbeat_timeout(struct winecord *client,
                              struct winecord_timer *timer)
{
    (void)client;
    struct winecord_gateway *gw = timer->data;

    if (WINEBERRY_OK == winecord_gateway_perform(gw)
        && ~gw->session->status & WINEBERRY_SESSION_SHUTDOWN
        && gw->session->is_ready)
    {
        winecord_gateway_send_heartbeat(gw, gw->payload.seq);
    }
    const u64unix_ms next_hb =
        gw->timer->hbeat_last + (u64unix_ms)gw->timer->hbeat_interval;

    timer->interval = (int64_t)(next_hb) - (int64_t)winecord_timestamp(client);
    if (timer->interval < 1) timer->interval = 1;
    timer->repeat = 1;
}

/* send heartbeat pulse to websockets server in order
 *  to maintain connection alive */
void
winecord_gateway_send_heartbeat(struct winecord_gateway *gw, int seq)
{
    struct ws_info info = { 0 };
    char buf[64];
    jsonb b;

    jsonb_init(&b);
    jsonb_object(&b, buf, sizeof(buf));
    {
        jsonb_key(&b, buf, sizeof(buf), "op", 2);
        jsonb_number(&b, buf, sizeof(buf), 1);
        jsonb_key(&b, buf, sizeof(buf), "d", 1);
        jsonb_number(&b, buf, sizeof(buf), seq);
        jsonb_object_pop(&b, buf, sizeof(buf));
    }

    if (!gw->timer->hbeat_acknowledged) {
        logconf_warn(&gw->conf, "Heartbeat ACK not received, marked as zombie");

        gw->timer->hbeat_acknowledged = true;

        winecord_gateway_reconnect(gw, false);

        return;
    }

    if (ws_send_text(gw->ws, &info, buf, b.pos)) {
        io_poller_curlm_enable_perform(CLIENT(gw, gw)->io_poller, gw->mhandle);
        logconf_info(
            &gw->conf,
            ANSICOLOR(
                "SEND",
                ANSI_FG_BRIGHT_GREEN) " HEARTBEAT (%d bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);

        gw->timer->hbeat_acknowledged = false;

        /* update heartbeat timestamp */
        gw->timer->hbeat_last = gw->timer->now;
        if (!gw->timer->hbeat_timer)
            gw->timer->hbeat_timer = winecord_internal_timer(
                CLIENT(gw, gw), _winecord_on_heartbeat_timeout, NULL, gw,
                gw->timer->hbeat_interval);
    }
    else {
        logconf_info(
            &gw->conf,
            ANSICOLOR("FAIL SEND",
                      ANSI_FG_RED) " HEARTBEAT (%d bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);
    }
}

void
winecord_gateway_send_request_guild_members(
    struct winecord_gateway *gw, struct winecord_request_guild_members *event)
{
    struct ws_info info = { 0 };
    char buf[4096];
    jsonb b;

    jsonb_init(&b);
    jsonb_object(&b, buf, sizeof(buf));
    {
        jsonb_key(&b, buf, sizeof(buf), "op", 2);
        jsonb_number(&b, buf, sizeof(buf), 8);
        jsonb_key(&b, buf, sizeof(buf), "d", 1);
        winecord_request_guild_members_to_jsonb(&b, buf, sizeof(buf), event);
        jsonb_object_pop(&b, buf, sizeof(buf));
    }

    if (ws_send_text(gw->ws, &info, buf, b.pos)) {
        io_poller_curlm_enable_perform(CLIENT(gw, gw)->io_poller, gw->mhandle);
        logconf_info(
            &gw->conf,
            ANSICOLOR("SEND", ANSI_FG_BRIGHT_GREEN) " REQUEST_GUILD_MEMBERS "
                                                    "(%d bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);
    }
    else {
        logconf_info(
            &gw->conf,
            ANSICOLOR(
                "FAIL SEND",
                ANSI_FG_RED) " REQUEST_GUILD_MEMBERS (%d bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);
    }
}

void
winecord_gateway_send_update_voice_state(
    struct winecord_gateway *gw, struct winecord_update_voice_state *event)
{
    struct ws_info info = { 0 };
    char buf[256];
    jsonb b;

    jsonb_init(&b);
    jsonb_object(&b, buf, sizeof(buf));
    {
        jsonb_key(&b, buf, sizeof(buf), "op", 2);
        jsonb_number(&b, buf, sizeof(buf), 4);
        jsonb_key(&b, buf, sizeof(buf), "d", 1);
        winecord_update_voice_state_to_jsonb(&b, buf, sizeof(buf), event);
        jsonb_object_pop(&b, buf, sizeof(buf));
    }

    if (ws_send_text(gw->ws, &info, buf, b.pos)) {
        io_poller_curlm_enable_perform(CLIENT(gw, gw)->io_poller, gw->mhandle);
        logconf_info(
            &gw->conf,
            ANSICOLOR(
                "SEND",
                ANSI_FG_BRIGHT_GREEN) " UPDATE_VOICE_STATE "
                                      "(%d bytes): %s channels [@@@_%zu_@@@]",
            b.pos, event->channel_id ? "join" : "leave",
            info.loginfo.counter + 1);
    }
    else {
        logconf_info(
            &gw->conf,
            ANSICOLOR(
                "FAIL SEND",
                ANSI_FG_RED) " UPDATE_VOICE_STATE (%d bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);
    }
}

void
winecord_gateway_send_presence_update(struct winecord_gateway *gw,
                                     struct winecord_presence_update *presence)
{
    struct ws_info info = { 0 };
    char buf[2048];
    jsonb b;

    if (!gw->session->is_ready) return;

    jsonb_init(&b);
    jsonb_object(&b, buf, sizeof(buf));
    {
        jsonb_key(&b, buf, sizeof(buf), "op", 2);
        jsonb_number(&b, buf, sizeof(buf), 3);
        jsonb_key(&b, buf, sizeof(buf), "d", 1);
        winecord_presence_update_to_jsonb(&b, buf, sizeof(buf), presence);
        jsonb_object_pop(&b, buf, sizeof(buf));
    }

    if (ws_send_text(gw->ws, &info, buf, b.pos)) {
        io_poller_curlm_enable_perform(CLIENT(gw, gw)->io_poller, gw->mhandle);
        logconf_info(
            &gw->conf,
            ANSICOLOR("SEND", ANSI_FG_BRIGHT_GREEN) " PRESENCE UPDATE (%d "
                                                    "bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);
    }
    else {
        logconf_error(
            &gw->conf,
            ANSICOLOR("FAIL SEND", ANSI_FG_RED) " PRESENCE UPDATE (%d "
                                                "bytes) [@@@_%zu_@@@]",
            b.pos, info.loginfo.counter + 1);
    }
}
