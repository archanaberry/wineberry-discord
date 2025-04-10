#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"

#define WINEBERRY_TIMER_ALLOWED_FLAGS                                           \
    (WINEBERRY_TIMER_MILLISECONDS | WINEBERRY_TIMER_MICROSECONDS                  \
     | WINEBERRY_TIMER_DELETE | WINEBERRY_TIMER_DELETE_AUTO                       \
     | WINEBERRY_TIMER_INTERVAL_FIXED)

#define WINEBERRY_STATUS_FLAGS (WINEBERRY_TIMER_CANCELED | WINEBERRY_TIMER_DELETE)

#define WINEBERRY_TIMER_CANCELED_AND_DELETE_AUTO                                \
    (WINEBERRY_TIMER_CANCELED | WINEBERRY_TIMER_DELETE_AUTO)

#define HAS_FLAGS(var, flags) (((flags) == ((var) & (flags))))

static int
cmp_timers(const void *a, const void *b)
{
    const int64_t l = *(int64_t *)a;
    const int64_t r = *(int64_t *)b;
    if (l == r || (l < 0 && r < 0)) return 0;
    if (l < 0) return 1;
    if (r < 0) return -1;
    return l > r ? 1 : -1;
}

void
winecord_timers_init(struct winecord_timers *timers, struct io_poller *io)
{
    timers->q = priority_queue_create(
        sizeof(int64_t), sizeof(struct winecord_timer), cmp_timers, 0);
    timers->io = io;
    pthread_mutex_init(&timers->lock, NULL);
    pthread_cond_init(&timers->cond, NULL);
}

static void
winecord_timers_cancel_all(struct winecord *client,
                          struct winecord_timers *timers)
{
    struct winecord_timer timer;
    while ((timer.id = priority_queue_pop(timers->q, NULL, &timer))) {
        timer.flags |= WINEBERRY_TIMER_CANCELED | WINEBERRY_TIMER_DELETE;
        if (timer.on_status_changed) timer.on_status_changed(client, &timer);
    }
}

void
winecord_timers_cleanup(struct winecord *client, struct winecord_timers *timers)
{
    priority_queue_set_max_capacity(timers->q, 0);
    winecord_timers_cancel_all(client, timers);
    pthread_cond_destroy(&timers->cond);
    pthread_mutex_destroy(&timers->lock);
    priority_queue_destroy(timers->q);
    memset(timers, 0, sizeof *timers);
}

int64_t
winecord_timers_get_next_trigger(struct winecord_timers *const timers[],
                                size_t n,
                                int64_t now,
                                int64_t max_time)
{
    if (max_time == 0) return 0;

    for (unsigned i = 0; i < n; i++) {
        int64_t trigger;
        if (0 != pthread_mutex_trylock(&timers[i]->lock)) return 0;

        if (priority_queue_peek(timers[i]->q, &trigger, NULL)) {
            if (trigger < 0) goto unlock;

            if (trigger <= now)
                max_time = 0;
            else if (max_time > trigger - now)
                max_time = trigger - now;
        }
    unlock:
        pthread_mutex_unlock(&timers[i]->lock);
    }
    return max_time;
}

static unsigned
_winecord_timer_ctl_no_lock(struct winecord *client,
                           struct winecord_timers *timers,
                           struct winecord_timer *timer_ret)
{
    struct winecord_timer timer;
    memcpy(&timer, timer_ret, sizeof timer);

    int64_t key = -1;
    if (timer.id) {
        if (!priority_queue_get(timers->q, timer.id, &key, NULL)) return 0;

        if (timer.flags & WINEBERRY_TIMER_GET) {
            timer_ret->id =
                priority_queue_get(timers->q, timer.id, NULL, timer_ret);
            if (timer.flags == WINEBERRY_TIMER_GET) return timer_ret->id;
        }
    }

    int64_t now = -1;
    if (timer.delay >= 0) {
        now = (int64_t)winecord_timestamp_us(client)
              + ((timer.flags & WINEBERRY_TIMER_MICROSECONDS)
                     ? timer.delay
                     : timer.delay * 1000);
    }
    if (timer.flags & (WINEBERRY_TIMER_DELETE | WINEBERRY_TIMER_CANCELED)) now = 0;

    timer.flags &= (WINEBERRY_TIMER_ALLOWED_FLAGS | WINEBERRY_TIMER_CANCELED);

    if (!timer.id) {
        return priority_queue_push(timers->q, &now, &timer);
    }
    else {
        if (timers->active.timer && timers->active.timer->id == timer.id)
            timers->active.skip_update_phase = true;
        if (priority_queue_update(timers->q, timer.id, &now, &timer))
            return timer.id;
        return 0;
    }
}

#define LOCK_TIMERS(timers)                                                   \
    do {                                                                      \
        pthread_mutex_lock(&timers.lock);                                     \
        if (timers.active.is_active                                           \
            && !pthread_equal(pthread_self(), timers.active.thread))          \
            pthread_cond_wait(&timers.cond, &timers.lock);                    \
    } while (0);

#define UNLOCK_TIMERS(timers)                                                 \
    do {                                                                      \
        bool should_wakeup = !timers.active.is_active;                        \
        pthread_mutex_unlock(&timers.lock);                                   \
        if (should_wakeup) io_poller_wakeup(timers.io);                       \
    } while (0)

unsigned
_winecord_timer_ctl(struct winecord *client,
                   struct winecord_timers *timers,
                   struct winecord_timer *timer_ret)

{
    LOCK_TIMERS((*timers));
    unsigned id = _winecord_timer_ctl_no_lock(client, timers, timer_ret);
    UNLOCK_TIMERS((*timers));
    return id;
}

#define TIMER_TRY_DELETE                                                      \
    if (timer.flags & WINEBERRY_TIMER_DELETE) {                                 \
        priority_queue_del(timers->q, timer.id);                              \
        if (timer.on_status_changed) {                                        \
            pthread_mutex_unlock(&timers->lock);                              \
            timer.on_status_changed(client, &timer);                          \
            pthread_mutex_lock(&timers->lock);                                \
        }                                                                     \
        timers->active.skip_update_phase = false;                             \
        continue;                                                             \
    }

void
winecord_timers_run(struct winecord *client, struct winecord_timers *timers)
{
    int64_t now = (int64_t)winecord_timestamp_us(client);
    const int64_t start_time = now;

    pthread_mutex_lock(&timers->lock);
    timers->active.is_active = true;
    timers->active.thread = pthread_self();
    struct winecord_timer timer;
    timers->active.timer = &timer;

    timers->active.skip_update_phase = false;
    for (int64_t trigger, max_iterations = 100000;
         (timer.id = priority_queue_peek(timers->q, &trigger, &timer))
         && max_iterations > 0;
         max_iterations--)
    {
        wineberry_ev_timer cb;
        // update now timestamp every so often
        if ((max_iterations & 0x1F) == 0) {
            now = (int64_t)winecord_timestamp_us(client);
            // break if we've spent too much time running timers
            if (now - start_time > 10000) break;
        }

        // no timers to run
        if (trigger > now || trigger == -1) break;
    restart:
        if (timer.flags & WINEBERRY_STATUS_FLAGS) {
            cb = timer.on_status_changed;
            TIMER_TRY_DELETE;
        }
        else {
            if (timer.repeat > 0) timer.repeat--;
            cb = timer.on_tick;
            timer.flags |= WINEBERRY_TIMER_TICK;
        }

        enum wineberry_timer_flags prev_flags = timer.flags;
        if (cb) {
            pthread_mutex_unlock(&timers->lock);
            cb(client, &timer);
            pthread_mutex_lock(&timers->lock);
        }
        timer.flags &= ~(enum wineberry_timer_flags)WINEBERRY_TIMER_TICK;

        if (timers->active.skip_update_phase) {
            timers->active.skip_update_phase = false;
            continue;
        }

        if ((timer.flags & WINEBERRY_STATUS_FLAGS)
            != (prev_flags & WINEBERRY_STATUS_FLAGS))
        {
            if (!(prev_flags & WINEBERRY_TIMER_CANCELED)
                && timer.flags & WINEBERRY_TIMER_CANCELED)
                goto restart;
        }

        // time has expired, delete if WINEBERRY_TIMER_DELETE_AUTO is set
        if ((timer.flags & WINEBERRY_TIMER_CANCELED || timer.repeat == 0)
            && timer.flags & WINEBERRY_TIMER_DELETE_AUTO)
            timer.flags |= WINEBERRY_TIMER_DELETE;

        // we just called cancel, only call delete
        if ((timer.flags & WINEBERRY_TIMER_DELETE)
            && (prev_flags & WINEBERRY_TIMER_CANCELED))
            timer.flags &= ~(enum wineberry_timer_flags)WINEBERRY_TIMER_CANCELED;
        TIMER_TRY_DELETE;

        int64_t next = -1;
        if (timer.delay != -1 && timer.interval >= 0 && timer.repeat != 0
            && ~timer.flags & WINEBERRY_TIMER_CANCELED)
        {
            next =
                ((timer.flags & WINEBERRY_TIMER_INTERVAL_FIXED) ? trigger : now)
                + ((timer.flags & WINEBERRY_TIMER_MICROSECONDS)
                       ? timer.interval
                       : timer.interval * 1000);
        }
        timer.flags &= WINEBERRY_TIMER_ALLOWED_FLAGS;
        priority_queue_update(timers->q, timer.id, &next, &timer);
    }

    timers->active.is_active = false;
    timers->active.timer = NULL;
    pthread_cond_broadcast(&timers->cond);
    pthread_mutex_unlock(&timers->lock);
}

unsigned
winecord_timer_ctl(struct winecord *client, struct winecord_timer *timer)
{
    return _winecord_timer_ctl(client, &client->timers.user, timer);
}

unsigned
winecord_internal_timer_ctl(struct winecord *client, struct winecord_timer *timer)
{
    return _winecord_timer_ctl(client, &client->timers.internal, timer);
}

static unsigned
_winecord_timer(struct winecord *client,
               struct winecord_timers *timers,
               wineberry_ev_timer on_tick_cb,
               wineberry_ev_timer on_status_changed_cb,
               void *data,
               int64_t delay)
{
    struct winecord_timer timer = {
        .on_tick = on_tick_cb,
        .on_status_changed = on_status_changed_cb,
        .data = data,
        .delay = delay,
        .flags = WINEBERRY_TIMER_DELETE_AUTO,
    };
    return _winecord_timer_ctl(client, timers, &timer);
}

unsigned
winecord_timer_interval(struct winecord *client,
                       wineberry_ev_timer on_tick_cb,
                       wineberry_ev_timer on_status_changed_cb,
                       void *data,
                       int64_t delay,
                       int64_t interval,
                       int64_t repeat)
{
    struct winecord_timer timer = {
        .on_tick = on_tick_cb,
        .on_status_changed = on_status_changed_cb,
        .data = data,
        .delay = delay,
        .interval = interval,
        .repeat = repeat,
        .flags = WINEBERRY_TIMER_DELETE_AUTO,
    };
    return winecord_timer_ctl(client, &timer);
}

unsigned
winecord_timer(struct winecord *client,
              wineberry_ev_timer on_tick_cb,
              wineberry_ev_timer on_status_changed_cb,
              void *data,
              int64_t delay)
{
    return _winecord_timer(client, &client->timers.user, on_tick_cb,
                          on_status_changed_cb, data, delay);
}

unsigned
winecord_internal_timer(struct winecord *client,
                       wineberry_ev_timer on_tick_cb,
                       wineberry_ev_timer on_status_changed_cb,
                       void *data,
                       int64_t delay)
{
    return _winecord_timer(client, &client->timers.internal, on_tick_cb,
                          on_status_changed_cb, data, delay);
}

bool
winecord_timer_get(struct winecord *client,
                  unsigned id,
                  struct winecord_timer *timer)
{
    if (!id) return 0;
    LOCK_TIMERS(client->timers.user);
    timer->id = priority_queue_get(client->timers.user.q, id, NULL, timer);
    UNLOCK_TIMERS(client->timers.user);
    return timer->id;
}

static void
winecord_timer_disable_update_if_active(struct winecord_timers *timers,
                                       unsigned id)
{
    if (!timers->active.timer) return;
    if (timers->active.timer->id == id)
        timers->active.skip_update_phase = true;
}

bool
winecord_timer_start(struct winecord *client, unsigned id)
{
    bool result = 0;
    struct winecord_timer timer;
    LOCK_TIMERS(client->timers.user);
    winecord_timer_disable_update_if_active(&client->timers.user, id);
    if (priority_queue_get(client->timers.user.q, id, NULL, &timer)) {
        if (timer.delay < 0) timer.delay = 0;
        result =
            _winecord_timer_ctl_no_lock(client, &client->timers.user, &timer);
    }
    UNLOCK_TIMERS(client->timers.user);
    return result;
}

bool
winecord_timer_stop(struct winecord *client, unsigned id)
{
    bool result = 0;
    struct winecord_timer timer;
    LOCK_TIMERS(client->timers.user);
    winecord_timer_disable_update_if_active(&client->timers.user, id);
    if (priority_queue_get(client->timers.user.q, id, NULL, &timer)) {
        int64_t disabled = -1;
        result = priority_queue_update(client->timers.user.q, id, &disabled,
                                       &timer);
    }
    UNLOCK_TIMERS(client->timers.user);
    return result;
}

static bool
winecord_timer_add_flags(struct winecord *client,
                        unsigned id,
                        enum wineberry_timer_flags flags)
{
    bool result = 0;
    struct winecord_timer timer;
    LOCK_TIMERS(client->timers.user);
    winecord_timer_disable_update_if_active(&client->timers.user, id);
    if (priority_queue_get(client->timers.user.q, id, NULL, &timer)) {
        timer.flags |= flags;
        int64_t run_now = 0;
        result =
            priority_queue_update(client->timers.user.q, id, &run_now, &timer);
    }
    UNLOCK_TIMERS(client->timers.user);
    return result;
}

bool
winecord_timer_cancel(struct winecord *client, unsigned id)
{
    return winecord_timer_add_flags(client, id, WINEBERRY_TIMER_CANCELED);
}

bool
winecord_timer_delete(struct winecord *client, unsigned id)
{
    return winecord_timer_add_flags(client, id, WINEBERRY_TIMER_DELETE);
}

bool
winecord_timer_cancel_and_delete(struct winecord *client, unsigned id)
{
    return winecord_timer_add_flags(
        client, id, WINEBERRY_TIMER_DELETE | WINEBERRY_TIMER_CANCELED);
}
