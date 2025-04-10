#include <stdlib.h>
#include <errno.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "cog-utils.h"

static void
winecord_wake_timer_cb(struct winecord *client, struct winecord_timer *timer)
{
    (void)timer;
    if (client->wakeup_timer.cb) client->wakeup_timer.cb(client);
}

void
winecord_set_next_wakeup(struct winecord *client, int64_t delay)
{
    unsigned id = winecord_internal_timer_ctl(
        client, &(struct winecord_timer){
                    .id = client->wakeup_timer.id,
                    .on_tick = winecord_wake_timer_cb,
                    .delay = delay,
                });
    client->wakeup_timer.id = id;
}

void
winecord_set_on_wakeup(struct winecord *client,
                      void (*callback)(struct winecord *client))
{
    client->wakeup_timer.cb = callback;
    if (client->wakeup_timer.id) {
        winecord_internal_timer_ctl(client,
                                   &(struct winecord_timer){
                                       .id = client->wakeup_timer.id,
                                       .on_tick = winecord_wake_timer_cb,
                                       .delay = -1,
                                   });
    }
}

void
winecord_set_on_idle(struct winecord *client,
                    void (*callback)(struct winecord *client))
{
    client->on_idle = callback;
}

void
winecord_set_on_cycle(struct winecord *client,
                     void (*callback)(struct winecord *client))
{
    client->on_cycle = callback;
}

#define BREAK_ON_FAIL(code, function)                                         \
    if (WINEBERRY_OK != (code = function)) break

#define CALL_IO_POLLER_POLL(poll_errno, poll_result, io_poller, delay)        \
    do {                                                                      \
        if (-1 == (poll_result = io_poller_poll(io_poller, (int)(delay))))    \
            poll_errno = errno;                                               \
    } while (0)

WINEBERRY
winecord_run(struct winecord *client)
{
    struct winecord_timers *const timers[] = { &client->timers.internal,
                                              &client->timers.user };
    int64_t now;
    WINEBERRY code;

    while (1) {
        BREAK_ON_FAIL(code, winecord_gateway_start(&client->gw));

        while (1) {
            int poll_result, poll_errno = 0;
            int64_t poll_time = 0;

            now = (int64_t)winecord_timestamp_us(client);

            if (!client->on_idle) {
                poll_time = winecord_timers_get_next_trigger(
                    timers, sizeof timers / sizeof *timers, now, 60000000);
            }

            CALL_IO_POLLER_POLL(poll_errno, poll_result, client->io_poller,
                                poll_time / 1000);

            now = (int64_t)winecord_timestamp_us(client);

            if (0 == poll_result) {
                if (client->on_idle) {
                    client->on_idle(client);
                }
                else {
                    int64_t sleep_time = winecord_timers_get_next_trigger(
                        timers, sizeof timers / sizeof *timers, now, 1000);
                    if (sleep_time > 0 && sleep_time < 1000)
                        cog_sleep_us(sleep_time);
                }
            }

            if (client->on_cycle) client->on_cycle(client);

            for (unsigned i = 0; i < sizeof timers / sizeof *timers; i++)
                winecord_timers_run(client, timers[i]);

            if (poll_result >= 0 && !client->on_idle)
                CALL_IO_POLLER_POLL(poll_errno, poll_result, client->io_poller,
                                    0);

            if (-1 == poll_result) {
                /* TODO: handle poll error here */
                /* use poll_errno instead of errno */
                (void)poll_errno;
            }

            if (client->gw.session->status & WINECORD_SESSION_SHUTDOWN) break;

            BREAK_ON_FAIL(code, io_poller_perform(client->io_poller));

            winecord_requestor_dispatch_responses(&client->rest.requestor);
        }

        logconf_info(&client->conf,
                     "Exits main gateway loop (code: %d, reason: %s)", code,
                     winecord_strerror(code, client));

        /* stop all pending requests in case of connection shutdown */
        if (true == winecord_gateway_end(&client->gw)) break;
    }

    return code;
}

#undef BREAK_ON_FAIL
#undef CALL_IO_POLLER_POLL
