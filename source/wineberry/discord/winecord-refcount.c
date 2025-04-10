#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"

#define CHASH_BUCKETS_FIELD refs
#include "chash.h"

/* chash heap-mode (auto-increase hashtable) */
#define REFCOUNTER_TABLE_HEAP   1
#define REFCOUNTER_TABLE_BUCKET struct _winecord_ref
#define REFCOUNTER_TABLE_FREE_KEY(_key)
#define REFCOUNTER_TABLE_HASH(_key, _hash) ((intptr_t)(_key))
#define REFCOUNTER_TABLE_FREE_VALUE(_value)                                   \
    _winecord_refvalue_cleanup(rc, &_value)
#define REFCOUNTER_TABLE_COMPARE(_cmp_a, _cmp_b) (_cmp_a == _cmp_b)
#define REFCOUNTER_TABLE_INIT(ref, _key, _value)                              \
    memset(&ref, 0, sizeof(ref));                                             \
    chash_default_init(ref, _key, _value)

struct _winecord_refvalue {
    /** user arbitrary data to be retrieved at `done` or `fail` callbacks */
    void *data;
    /**
     * cleanup for when `data` is no longer needed
     * @note this only has to be assigned once, it is automatically called once
     *      `data` is no longer referenced by any callback */
    union {
        void (*client)(struct winecord *client, void *data);
        void (*internal)(void *data);
    } cleanup;
    /**
     * `data` references count
     * @note if `-1` then `data` has been claimed with
     *      winecord_refcounter_claim() and will be cleaned up once
     *      winecord_refcount_unclaim() is called
     */
    int visits;
    /** whether `data` cleanup should also be followed by a free() */
    bool should_free;
    /** whether cleanup expects a client parameter */
    bool expects_client;
    /** how many times this resource has been winecord_claim() 'd */
    int claims;
};

struct _winecord_ref {
    /** key is the user data's address */
    intptr_t key;
    /** holds the user data and information for automatic cleanup */
    struct _winecord_refvalue value;
    /** the route state in the hashtable (see chash.h 'State enums') */
    int state;
};

static void
_winecord_refvalue_cleanup(struct winecord_refcounter *rc,
                          struct _winecord_refvalue *value)
{
    if (value->cleanup.client) {
        if (value->expects_client)
            value->cleanup.client(CLIENT(rc, refcounter), value->data);
        else
            value->cleanup.internal(value->data);
    }
    if (value->should_free) free(value->data);
}

static struct _winecord_refvalue *
_winecord_refvalue_find(struct winecord_refcounter *rc, const void *data)
{
    struct _winecord_ref *ref =
        chash_lookup_bucket(rc, (intptr_t)data, ref, REFCOUNTER_TABLE);
    return &ref->value;
}

static void
_winecord_refvalue_init(struct winecord_refcounter *rc,
                       void *data,
                       struct _winecord_refvalue *init_fields)
{
    init_fields->data = data;
    init_fields->visits = 1;
    chash_assign(rc, (intptr_t)data, *init_fields, REFCOUNTER_TABLE);
}

static void
_winecord_refvalue_delete(struct winecord_refcounter *rc, void *data)
{
    chash_delete(rc, (intptr_t)data, REFCOUNTER_TABLE);
}

void
winecord_refcounter_init(struct winecord_refcounter *rc, struct logconf *conf)
{
    logconf_branch(&rc->conf, conf, "WINECORD_REFCOUNT");

    __chash_init(rc, REFCOUNTER_TABLE);

    rc->g_lock = malloc(sizeof *rc->g_lock);
    ASSERT_S(!pthread_mutex_init(rc->g_lock, NULL),
             "Couldn't initialize refcounter mutex");
}

void
winecord_refcounter_cleanup(struct winecord_refcounter *rc)
{
    __chash_free(rc, REFCOUNTER_TABLE);
    pthread_mutex_destroy(rc->g_lock);
    free(rc->g_lock);
}

static bool
_winecord_refcounter_contains(struct winecord_refcounter *rc, const void *data)
{
    bool ret = chash_contains(rc, (intptr_t)data, ret, REFCOUNTER_TABLE);
    return ret;
}

static WINEBERRYcode
_winecord_refcounter_incr_no_lock(struct winecord_refcounter *rc, void *data)
{
    WINEBERRYcode code = WINEBERRY_RESOURCE_UNAVAILABLE;
    if (_winecord_refcounter_contains(rc, data)) {
        struct _winecord_refvalue *value = _winecord_refvalue_find(rc, data);

        if (value->visits == INT_MAX) {
            logconf_error(&rc->conf,
                          "Can't increment %p any further: Overflow", data);
        }
        else {
            ++value->visits;
            logconf_trace(&rc->conf, "Increment %p (%d visits)", data,
                          value->visits);
            code = WINEBERRY_OK;
        }
    }
    return code;
}

static WINEBERRYcode
_winecord_refcounter_decr_no_lock(struct winecord_refcounter *rc, void *data)
{
    WINEBERRYcode code = WINEBERRY_RESOURCE_UNAVAILABLE;
    if (_winecord_refcounter_contains(rc, data)) {
        struct _winecord_refvalue *value = _winecord_refvalue_find(rc, data);

        if (value->visits < value->claims) {
            logconf_error(&rc->conf,
                          "(Internal Error) There shouldn't be more visits "
                          "than claims!");
        }
        else if (--value->visits > 0) {
            code = WINEBERRY_OK;
            logconf_trace(&rc->conf, "Decrement %p (%d visits)", data,
                          value->visits);
        }
        else {
            if (value->claims != 0) {
                logconf_error(&rc->conf, "(Internal Error) Caught attempt to "
                                         "cleanup claimed resource!");
                ++value->visits;
                code = WINEBERRY_RESOURCE_OWNERSHIP;
            }
            else {
                _winecord_refvalue_delete(rc, data);
                logconf_info(&rc->conf, "Fully decremented and free'd %p",
                             data);
                code = WINEBERRY_OK;
            }
        }
    }
    return code;
}

WINEBERRYcode
winecord_refcounter_claim(struct winecord_refcounter *rc, const void *data)
{
    WINEBERRYcode code = WINEBERRY_RESOURCE_UNAVAILABLE;

    pthread_mutex_lock(rc->g_lock);
    if (_winecord_refcounter_contains(rc, data)) {
        struct _winecord_refvalue *value = _winecord_refvalue_find(rc, data);

        ++value->claims;
        code = _winecord_refcounter_incr_no_lock(rc, (void *)data);
        logconf_trace(&rc->conf, "Claiming %p (claims: %d)", data,
                      value->claims);
    }
    pthread_mutex_unlock(rc->g_lock);
    return code;
}

WINEBERRYcode
winecord_refcounter_unclaim(struct winecord_refcounter *rc, void *data)
{
    WINEBERRYcode code = WINEBERRY_RESOURCE_UNAVAILABLE;

    pthread_mutex_lock(rc->g_lock);
    if (_winecord_refcounter_contains(rc, data)) {
        struct _winecord_refvalue *value = _winecord_refvalue_find(rc, data);

        if (0 == value->claims) {
            logconf_error(&rc->conf, "Resource hasn't been claimed before, or "
                                     "it has already been unclaimed");
        }
        else {
            --value->claims;
            logconf_trace(&rc->conf, "Unclaiming %p (claims: %d)", data,
                          value->claims);
            code = _winecord_refcounter_decr_no_lock(rc, data);
        }
    }
    pthread_mutex_unlock(rc->g_lock);

    return code;
}

void
winecord_refcounter_add_internal(struct winecord_refcounter *rc,
                                void *data,
                                void (*cleanup)(void *data),
                                bool should_free)
{
    pthread_mutex_lock(rc->g_lock);
    _winecord_refvalue_init(rc, data,
                           &(struct _winecord_refvalue){
                               .expects_client = false,
                               .cleanup.internal = cleanup,
                               .should_free = should_free,
                           });
    logconf_info(&rc->conf, "Adding winecord's internal resource %p", data);
    pthread_mutex_unlock(rc->g_lock);
}

void
winecord_refcounter_add_client(struct winecord_refcounter *rc,
                              void *data,
                              void (*cleanup)(struct winecord *client,
                                              void *data),
                              bool should_free)
{
    pthread_mutex_lock(rc->g_lock);
    _winecord_refvalue_init(rc, data,
                           &(struct _winecord_refvalue){
                               .expects_client = true,
                               .cleanup.client = cleanup,
                               .should_free = should_free,
                           });
    logconf_info(&rc->conf, "Adding user's custom resource %p", data);
    pthread_mutex_unlock(rc->g_lock);
}

WINEBERRYcode
winecord_refcounter_incr(struct winecord_refcounter *rc, void *data)
{
    WINEBERRYcode code;
    pthread_mutex_lock(rc->g_lock);
    code = _winecord_refcounter_incr_no_lock(rc, data);
    pthread_mutex_unlock(rc->g_lock);
    return code;
}

WINEBERRYcode
winecord_refcounter_decr(struct winecord_refcounter *rc, void *data)
{
    WINEBERRYcode code;
    pthread_mutex_lock(rc->g_lock);
    code = _winecord_refcounter_decr_no_lock(rc, data);
    pthread_mutex_unlock(rc->g_lock);
    return code;
}
