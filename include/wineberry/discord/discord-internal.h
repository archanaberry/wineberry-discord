/**
 * @file winecord-internal.h
 * @ingroup WinecordInternal
 * @author Cogmasters
 * @brief Internal functions and datatypes
 */

#ifndef WINECORD_INTERNAL_H
#define WINECORD_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <pthread.h>

#define JSONB_HEADER
#include "json-build.h"
#define JSMN_STRICT
#define JSMN_HEADER
#include "jsmn.h"
#include "jsmn-find.h"

#include "logconf.h"
#include "user-agent.h"
#include "websockets.h"
#include "cog-utils.h"
#include "io_poller.h"
#include "queue.h"
#include "priority_queue.h"
#include "attributes.h"

/** @brief Return 1 if string isn't considered empty */
#define NOT_EMPTY_STR(str) ((str) && *(str))
/**
 * @brief Get container `type` from a field `ptr`
 *
 * @param[in] ptr the field contained in `type`
 * @param[in] type the container datatype
 * @param[in] path the path to the field from the container POV
 */
#define CONTAINEROF(ptr, type, path)                                          \
    ((type *)((char *)(ptr)-offsetof(type, path)))

/** @defgroup WinecordInternal Internal implementation details
 * @brief Documentation useful when developing or debugging Winecord itself
 *  @{ */

/** @brief dup shutdown fd to listen for winecord_shutdown_async() */
int winecord_dup_shutdown_fd(void);

/** @brief Get client from its nested field */
#define CLIENT(ptr, path) CONTAINEROF(ptr, struct winecord, path)

/**
 * @brief log and return `code` if `expect` condition is false
 *
 * @param[in] client the Winecord client
 * @param[in] expect the expected outcome
 * @param[in] code return WINEBERRY error code
 * @param[in] reason for return
 * @return the provided @ref WINEBERRY `code` parameter
 */
#define WINEBERRY_EXPECT(client, expect, code, reason)                            \
    do {                                                                      \
        if (!(expect)) {                                                      \
            logconf_error(&(client)->conf, "Expected: " #expect ": " reason); \
            return code;                                                      \
        }                                                                     \
    } while (0)

/**
 * @brief Shortcut for checking OOB-write attempts
 * @note unsigned values are expected
 *
 * @param[in] nbytes amount of bytes to be written
 * @param[in] destsz size of dest in bytes
 */
#define ASSERT_NOT_OOB(nbytes, destsz)                                        \
    ASSERT_S((size_t)nbytes < (size_t)destsz, "Out of bounds write attempt");

/** URL endpoint threshold length */
#define WINECORD_ENDPT_LEN 512
/** Route's unique key threshold length */
#define WINECORD_ROUTE_LEN 256

/** @defgroup WinecordInternalTimer Timer API
 * @brief Callback scheduling API
 *  @{ */

struct winecord_timers {
    priority_queue *q;
    struct io_poller *io;
    struct {
        bool is_active;
        pthread_t thread;
        struct winecord_timer *timer;
        bool skip_update_phase;
    } active;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

/**
 * @brief Prepare timers for usage
 *
 * @param timers the 'struct winecord_timers' to init
 */
void winecord_timers_init(struct winecord_timers *timers, struct io_poller *io);

/**
 * @brief Cleanup timers and call cancel any running ones
 *
 * @param client the client created with winecord_init()
 * @param timers the 'struct winecord_timers' to cleanup
 */
void winecord_timers_cleanup(struct winecord *client,
                            struct winecord_timers *timers);

/**
 * @brief Get earliest trigger time from a group of timers
 *
 * @param timers array of timers
 * @param n number of timers in array
 * @param now current time
 * @param max_time max time to allowed
 * @return time in microseconds until next timer, or max
 */
int64_t winecord_timers_get_next_trigger(struct winecord_timers *const timers[],
                                        size_t n,
                                        int64_t now,
                                        int64_t max_time);

/**
 * @brief Run all timers that are due
 *
 * @param client the client created with winecord_init()
 * @param timers the timers to run
 */
void winecord_timers_run(struct winecord *client, struct winecord_timers *timers);

/**
 * @brief Modifies or creates a timer
 *
 * @param client the client created with winecord_init()
 * @param timers the timer group to perform this operation on
 * @param timer the timer that should be modified
 * @return the id of the timer
 */
unsigned _winecord_timer_ctl(struct winecord *client,
                            struct winecord_timers *timers,
                            struct winecord_timer *timer);

/**
 * @brief Modifies or creates a timer
 *
 * @param client the client created with winecord_init()
 * @param timer the timer that should be modified
 * @return the id of the timer
 */
unsigned winecord_internal_timer_ctl(struct winecord *client,
                                    struct winecord_timer *timer);

/**
 * @brief Creates a one shot timer that automatically deletes itself upon
 *      completion
 *
 * @param client the client created with winecord_init()
 * @param on_tick_cb (nullable) the callback that should be called when timer
 * triggers
 * @param on_status_changed_cb (nullable) the callback for status updates
 * timer->flags will have: WINECORD_TIMER_CANCELED, and WINECORD_TIMER_DELETE
 * @param data user data
 * @param delay delay before timer should start in milliseconds
 * @return the id of the timer
 */
unsigned winecord_internal_timer(struct winecord *client,
                                winecord_ev_timer on_tick_cb,
                                winecord_ev_timer on_status_changed_cb,
                                void *data,
                                int64_t delay);

/** @} WinecordInternalTimer */

/** @defgroup WinecordInternalREST REST API
 * @brief Wrapper to the Winecord REST API
 *  @{ */

/** @defgroup WinecordInternalRESTRequest Request's handling
 * @brief Store, manage and dispatch individual requests
 *  @{ */

/** @defgroup WinecordInternalRESTRequestRatelimit Ratelimiting
 * @brief Enforce ratelimiting per the official Winecord Documentation
 *  @{ */

/**
 * @brief Value assigned to @ref winecord_bucket `busy_req` field in case
 *      it's being timed-out
 */
#define WINECORD_BUCKET_TIMEOUT (void *)(0xf)

/**
 * @brief The ratelimiter struct for handling ratelimiting
 * @note this struct **SHOULD** only be handled from the `REST` manager thread
 */
struct winecord_ratelimiter {
    /** `WINECORD_RATELIMIT` logging module */
    struct logconf conf;
    /** amount of bucket's routes discovered */
    int length;
    /** route's cap before increase */
    int capacity;
    /**
     * routes matched to individual buckets
     * @note datatype declared at winecord-rest_ratelimit.c
     */
    struct _winecord_route *routes;
    /** singleton bucket for requests that haven't been matched to a
     *      known or new bucket (i.e first time running the request) */
    struct winecord_bucket *null;
    /** singleton bucket for requests that are not part of any known
     *      ratelimiting group */
    struct winecord_bucket *miss;

    /* client-wide global ratelimiting */
    u64unix_ms *global_wait_tstamp;

    /** bucket queues */
    struct {
        /** buckets that are currently pending (have pending requests) */
        QUEUE(struct winecord_bucket) pending;
    } queues;
};

/**
 * @brief Initialize ratelimiter handle
 *
 * A hashtable shall be used for storage and retrieval of discovered buckets
 * @param rl the ratelimiter handle to be initialized
 * @param conf pointer to @ref winecord_rest logging module
 */
void winecord_ratelimiter_init(struct winecord_ratelimiter *rl,
                              struct logconf *conf);

/**
 * @brief Cleanup all buckets that have been discovered
 *
 * @note pending requests will be moved to `rest.queues->recycling`
 * @param rl the handle initialized with winecord_ratelimiter_init()
 */
void winecord_ratelimiter_cleanup(struct winecord_ratelimiter *rl);

/**
 * @brief Build unique key formed from the HTTP method and endpoint
 * @see https://winecord.com/developers/docs/topics/rate-limits
 *
 * @param[in] method the request method
 * @param[out] key unique key for matching to buckets
 * @param[in] endpoint_fmt the printf-like endpoint formatting string
 * @param[in] args variadic arguments matched to `endpoint_fmt`
 */
void winecord_ratelimiter_build_key(enum http_method method,
                                   char key[WINECORD_ROUTE_LEN],
                                   const char endpoint_fmt[],
                                   va_list args);

/**
 * @brief Update the bucket with response header data
 *
 * @param rl the handle initialized with winecord_ratelimiter_init()
 * @param bucket NULL when bucket is first discovered
 * @param key obtained from winecord_ratelimiter_build_key()
 * @param info informational struct containing details on the current transfer
 * @note If the bucket was just discovered it will be created here.
 */
void winecord_ratelimiter_build(struct winecord_ratelimiter *rl,
                               struct winecord_bucket *bucket,
                               const char key[],
                               struct ua_info *info);

/**
 * @brief Update global ratelimiting value
 * @todo check if all pending buckets must be unset
 *
 * @param rl the handle initialized with winecord_ratelimiter_init()
 * @param bucket bucket that received the global ratelimiting notice
 * @param wait_ms the amount of time that all buckets should wait for
 */
void winecord_ratelimiter_set_global_timeout(struct winecord_ratelimiter *rl,
                                            struct winecord_bucket *bucket,
                                            u64unix_ms wait_ms);

/** @brief The Winecord bucket for handling per-group ratelimits */
struct winecord_bucket {
    /** the hash associated with the bucket's ratelimiting group */
    char hash[64];
    /** maximum connections this bucket can handle before ratelimit */
    long limit;
    /** connections this bucket can do before pending for cooldown */
    long remaining;
    /** timestamp of when cooldown timer resets */
    u64unix_ms reset_tstamp;

    /**
     * pointer to this bucket's currently busy request
     * @note @ref WINECORD_BUCKET_TIMEOUT if bucket is being ratelimited
     */
    struct winecord_request *busy_req;

    /** request queues */
    struct {
        /** next requests queue */
        QUEUE(struct winecord_request) next;
    } queues;
    /** entry for @ref winecord_ratelimiter pending buckets queue */
    QUEUE entry;
};

/**
 * @brief Set bucket timeout
 *
 * @param bucket the bucket to be checked for time out
 * @param wait_ms how long the bucket should wait for
 */
void winecord_bucket_set_timeout(struct winecord_bucket *bucket,
                                u64unix_ms wait_ms);

/**
 * @brief Get a `struct winecord_bucket` assigned to `key`
 *
 * @param rl the handle initialized with winecord_ratelimiter_init()
 * @param key obtained from winecord_ratelimiter_build_key()
 * @return bucket matched to `key`
 */
struct winecord_bucket *winecord_bucket_get(struct winecord_ratelimiter *rl,
                                          const char key[]);

/**
 * @brief Insert into bucket's next requests queue
 *
 * @param rl the handle initialized with winecord_ratelimiter_init()
 * @param b the bucket to insert the request to
 * @param req the request to be inserted to bucket
 * @param high_priority if high priority then request shall be prioritized over
 *      already enqueued requests
 */
void winecord_bucket_insert(struct winecord_ratelimiter *rl,
                           struct winecord_bucket *b,
                           struct winecord_request *req,
                           bool high_priority);

/**
 * @brief Iterate and select next requests
 * @note winecord_bucket_unselect() must be called once bucket's current request
 *      is done and its next one should be selected
 *
 * @param rl the handle initialized with winecord_ratelimiter_init()
 * @param data user arbitrary data
 * @param iter the user callback to be called per bucket
 */
void winecord_bucket_request_selector(
    struct winecord_ratelimiter *rl,
    void *data,
    void (*iter)(void *data, struct winecord_request *req));

/**
 * @brief Unselect a request provided at winecord_ratelimiter_request_selector()
 * @note counterpart to winecord_ratelimiter_request_selector()
 *
 * @param rl the handle initialized with winecord_ratelimiter_init()
 * @param b the request's bucket
 * @param req the request to unslect
 */
void winecord_bucket_request_unselect(struct winecord_ratelimiter *rl,
                                     struct winecord_bucket *b,
                                     struct winecord_request *req);

/** @} WinecordInternalRESTRequestRatelimit */

/** @brief Generic request dispatcher */
struct winecord_ret_dispatch {
    WINEBERRY_RET_DEFAULT_FIELDS;
    /** `true` if may receive a datatype from response */
    bool has_type;

    /**
     * optional callback to be executed on a successful request
     * @todo should be cast to the original callback signature before calling,
     *      otherwise its UB
     */
    union {
        void (*typed)(struct winecord *client,
                      struct winecord_response *resp,
                      const void *ret);
        void (*typeless)(struct winecord *client,
                         struct winecord_response *resp);
    } done;

    /** if an address is provided, then request will block the thread and
     * perform on-spot. On success the response object will be written to
     * the address. */
    void *sync;
};

/** @brief Attributes of response datatype */
struct winecord_ret_response {
    /** pointer to datatype */
    void *data;
    /** size of datatype in bytes */
    size_t size;
    /** initializer function for datatype fields */
    void (*init)(void *data);
    /** populate datatype with JSON values */
    size_t (*from_json)(const char *json, size_t len, void *data);
    /** cleanup function for datatype */
    void (*cleanup)(void *data);
};

/**
 * @brief Macro containing @ref winecord_attributes fields
 * @note this exists for @ref winecord_request alignment purposes
 */
#define WINEBERRY_ATTRIBUTES_FIELDS                                             \
    /** attributes set by client for request dispatch behavior */             \
    struct winecord_ret_dispatch dispatch;                                     \
    /** information for parsing response into a datatype (if possible) */     \
    struct winecord_ret_response response;                                     \
    /** if @ref HTTP_MIMEPOST provide attachments for file transfer */        \
    struct winecord_attachments attachments;                                   \
    /** indicated reason to why the action was taken @note when used at       \
     *      @ref winecord_request buffer is kept and reused */                 \
    char *reason

/** @brief Request to be performed */
struct winecord_attributes {
    WINEBERRY_ATTRIBUTES_FIELDS;
};

/**
 * @brief Individual requests that are scheduled to run asynchronously
 * @note this struct **SHOULD NOT** be handled from the `REST` manager thread
 * @note its fields are aligned with @ref winecord_attributes
 *      (see @ref WINEBERRY_ATTRIBUTES_FIELDS)
 */
struct winecord_request {
    WINEBERRY_ATTRIBUTES_FIELDS;

    /** the request's bucket */
    struct winecord_bucket *b;
    /** request body handle @note buffer is kept and reused */
    struct ccord_szbuf_reusable body;
    /** the request's http method */
    enum http_method method;
    /** the request's endpoint */
    char endpoint[WINECORD_ENDPT_LEN];
    /** the request bucket's key */
    char key[WINECORD_ROUTE_LEN];
    /** the connection handler assigned */
    struct ua_conn *conn;
    /** request's status code */
    WINEBERRYcode code;
    /** current retry attempt (stop at rest->retry_limit) */
    int retry_attempt;
    /** synchronize synchronous requests */
    pthread_cond_t *cond;
    /** entry for @ref winecord_ratelimiter and @ref winecord_bucket queues */
    QUEUE entry;
};

/** @brief The handle used for handling asynchronous requests */
struct winecord_requestor {
    /** `WINECORD_REQUEST` logging module */
    struct logconf conf;
    /** the user agent handle for performing requests */
    struct user_agent *ua;
    /** curl_multi handle for performing asynchronous requests */
    CURLM *mhandle;
    /** enforce Winecord's ratelimiting for requests */
    struct winecord_ratelimiter ratelimiter;

    /** max amount of retries before a failed request gives up */
    int retry_limit;

    /** request queues */
    struct {
        /** requests for recycling */
        QUEUE(struct winecord_request) recycling;
        /** pending requests waiting to be assigned to a bucket */
        QUEUE(struct winecord_request) pending;
        /**
         * finished requests that are done performing and waiting for
         *      their callbacks to be called from the main thread
         */
        QUEUE(struct winecord_request) finished;
    } * queues;

    /** queue locks */
    struct {
        /** recycling queue lock */
        pthread_mutex_t recycling;
        /** pending queue lock */
        pthread_mutex_t pending;
        /** finished queue lock */
        pthread_mutex_t finished;
    } * qlocks;
};

/**
 * @brief Initialize the request handler
 *
 * This shall initialize a `CURLM` multi handle for performing requests
 *      asynchronously, and a queue for storing individual requests
 * @param rqtor the requestor handle to be initialized
 * @param conf pointer to @ref winecord_rest logging module
 * @param token the bot token
 */
void winecord_requestor_init(struct winecord_requestor *rqtor,
                            struct logconf *conf,
                            const char token[]);

/**
 * @brief Free the request handler
 *
 * @param rqtor the handle initialized with winecord_requestor_init()
 */
void winecord_requestor_cleanup(struct winecord_requestor *rqtor);

/**
 * @brief Check for and start pending bucket's requests
 *
 * @param rqtor the handle initialized with winecord_requestor_init()
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_requestor_start_pending(struct winecord_requestor *rqtor);

/**
 * @brief Poll for request's completion
 *
 * @param rqtor the handle initialized with winecord_requestor_init()
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_requestor_info_read(struct winecord_requestor *rqtor);

/**
 * @brief Run pending callbacks from completed requests
 *
 * @param req the request containing preliminary information for its dispatch
 */
void winecord_requestor_dispatch_responses(struct winecord_requestor *rqtor);

/**
 * @brief Mark request as canceled and move it to the recycling queue
 *
 * @param rqtor the requestor handle initialized with winecord_requestor_init()
 * @param req the on-going request to be canceled
 */
void winecord_request_cancel(struct winecord_requestor *rqtor,
                            struct winecord_request *req);

/**
 * @brief Begin a new request
 *
 * The returned request automatically be performed from the `REST` thread
 * @param rqtor the requestor handle initialized with winecord_requestor_init()
 * @param req the request containing preliminary information for its dispatch
 * and response's parsing
 * @param body the request's body
 * @param method the request's HTTP method
 * @param endpoint the request's endpoint
 * @param key the request bucket's group for ratelimiting
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_request_begin(struct winecord_requestor *rqtor,
                                struct winecord_attributes *req,
                                struct ccord_szbuf *body,
                                enum http_method method,
                                char endpoint[WINECORD_ENDPT_LEN],
                                char key[WINECORD_ROUTE_LEN]);

/** @} WinecordInternalRESTRequest */

/**
 * @brief The handle used for interfacing with Winecord's REST API
 *
 * This handle will manage the special REST thread where requests are performed
 *      in
 */
struct winecord_rest {
    /** `WINECORD_HTTP` or `WINECORD_WEBHOOK` logging module */
    struct logconf conf;
    /** the requests handler */
    struct winecord_requestor requestor;
    /** the timer queue for the rest thread */
    struct winecord_timers timers;
    /** poller for REST requests */
    struct io_poller *io_poller;
    /** threadpool for managing the REST thread */
    struct threadpool_t *tpool;
};

/**
 * @brief Initialize an REST handle
 *
 * Structure used for interfacing with the Winecord's REST API
 * @param rest the REST handle to be initialized
 * @param conf pointer to @ref winecord logging module
 * @param token the bot token
 */
void winecord_rest_init(struct winecord_rest *rest,
                       struct logconf *conf,
                       const char token[]);

/**
 * @brief Free an REST handle
 *
 * @param rest the handle initialized with winecord_rest_init()
 */
void winecord_rest_cleanup(struct winecord_rest *rest);

/**
 * @brief Perform a request to Winecord
 *
 * This functions is a selector over winecord_rest_run() or
 *        winecord_rest_run_requestor()
 * @param rest the handle initialized with winecord_rest_init()
 * @param req return object of request
 * @param body the body sent for methods that require (ex: post), leave as
 *        null if unecessary
 * @param method the method in opcode format of the request being sent
 * @param endpoint_fmt the printf-like endpoint formatting string
 * @WINEBERRY_return
 * @note if sync is set then this function will block the thread and perform it
 *              immediately
 */
WINEBERRYcode winecord_rest_run(struct winecord_rest *rest,
                           struct winecord_attributes *req,
                           struct ccord_szbuf *body,
                           enum http_method method,
                           char endpoint_fmt[],
                           ...) PRINTF_LIKE(5, 6);

/**
 * @brief Stop all bucket's on-going, pending and timed-out requests
 *
 * The requests will be moved over to client's 'queues->recycling' queue
 * @param rest the handle initialized with winecord_rest_init()
 */
void winecord_rest_stop_buckets(struct winecord_rest *rest);

/** @} WinecordInternalREST */

/** @defgroup WinecordInternalGateway WebSockets API
 * @brief Wrapper to the Winecord Gateway API
 *  @{ */

/** @defgroup WinecordInternalGatewaySessionStatus Client's session status
 * @brief Client's session status
 *  @{ */
/** client is currently offline */
#define WINECORD_SESSION_OFFLINE 0u
/** client will attempt to resume session after reconnect */
#define WINECORD_SESSION_RESUMABLE 1u << 0
/** client in the process of being shutdown */
#define WINECORD_SESSION_SHUTDOWN 1u << 1
/** @} WinecordInternalGatewaySessionStatus */

/** @brief The handle for storing the Winecord Gateway session */
struct winecord_gateway_session {
    /** whether client is ready to start sending/receiving events */
    bool is_ready;
    /** session id for resuming lost connections */
    char id[64];
    /** amount of shards being used by this session */
    int shards;
    /** the session base url */
    char base_url[256];
    /** the base url for resuming */
    char resume_url[256];
    /** session limits */
    struct winecord_session_start_limit start_limit;
    /** active concurrent sessions */
    int concurrent;
    /** event counter to avoid reaching limit of 120 events per 60 sec */
    int event_count;
    /** @ref WinecordInternalGatewaySessionStatus */
    unsigned status;

    /** retry connection structure */
    struct {
        /** will attempt reconnecting if true */
        bool enable;
        /** current retry attempt (resets to 0 when succesful) */
        int attempt;
        /** max amount of retries before giving up */
        int limit;
    } retry;
};

/** @brief The handle for storing the Winecord response payload */
struct winecord_gateway_payload {
    /** current iteration JSON */
    struct {
        /** the JSON text */
        char *start;
        /** the text length */
        size_t size;
        /** jsmn tokens */
        jsmntok_t *tokens;
        /** amount of jsmn tokens */
        unsigned ntokens;
        /** jsmn-find key/value pairs */
        jsmnf_pair *pairs;
        /** amount of jsmn-find key/value pairs */
        unsigned npairs;
    } json;

    /** field 'op' */
    enum winecord_gateway_opcodes opcode;
    /** field 's' */
    int seq;
    /** field 't' */
    char name[32];
    /** field 't' enumerator value */
    enum winecord_gateway_events event;
    /** field 'd' */
    jsmnf_pair *data;
};

/** A generic event callback for casting */
typedef void (*winecord_ev_event)(struct winecord *client, const void *event);
/** An event callback for @ref WINECORD_EV_MESSAGE_CREATE */
typedef void (*winecord_ev_message)(struct winecord *client,
                                   const struct winecord_message *event);

/** @brief The handle used for interfacing with Winecord's Gateway API */
struct winecord_gateway {
    /** `WINECORD_GATEWAY` logging module */
    struct logconf conf;
    /** the websockets handle that connects to Winecord */
    struct websockets *ws;
    /** curl_multi handle for non-blocking transfer over websockets */
    CURLM *mhandle;

    /** timers kept for synchronization */
    struct {
        /**
         * fixed milliseconds interval between heartbeats
         * @note obtained at `HELLO`
         */
        int64_t hbeat_interval;
        /**
         * boolean that indicates if the last heartbeat was answered
         * @note used to detect zombie connections
        */
        bool hbeat_acknowledged;
        /**
         * Gateway's concept of "now"
         * @note updated at winecord_gateway_perform()
         */
        u64unix_ms now;
        /**
         * last heartbeat pulse timestamp
         * @note first sent at `READY` and `RESUME`, then updated every
         *      `hbeat_interval`
         */
        u64unix_ms hbeat_last;
        /**
         * timestamp of last succesful identify request
         * @note updated at winecord_gateway_send_identify()
         */
        u64unix_ms identify_last;
        /**
         * timestamp of last succesful event
         * @note resets every 60s
         */
        u64unix_ms event;
        /** timer id for heartbeat timer */
        unsigned hbeat_timer;

        /**
         * latency obtained from `HEARTBEAT` and `HEARTBEAT_ACK` response
         *      interval
         */
        int ping_ms;
        /** ping rwlock  */
        pthread_rwlock_t rwlock;
    } * timer;

    /** the identify structure for client authentication */
    struct winecord_identify id;

    /** on-going session structure */
    struct winecord_gateway_session *session;

    /** response-payload structure */
    struct winecord_gateway_payload payload;
    /**
     * the user's callbacks for Winecord events
     * @note index 0 for cache callbacks, index 1 for user callbacks
     * @todo should be cast to the original callback signature before calling,
     *      otherwise its UB
     */
    winecord_ev_event cbs[2][WINECORD_EV_MAX];
    /** the event scheduler callback */
    winecord_ev_scheduler scheduler;
};

/**
 * @brief Initialize a Gateway handle
 *
 * Structure used for interfacing with the Winecord's Gateway API
 * @param gw the gateway handle to be initialized
 * @param conf pointer to @ref winecord logging module
 * @param token the bot token
 */
void winecord_gateway_init(struct winecord_gateway *gw,
                          struct logconf *conf,
                          const char token[]);

/**
 * @brief Free a Gateway handle
 *
 * @param gw the handle initialized with winecord_gateway_init()
 */
void winecord_gateway_cleanup(struct winecord_gateway *gw);

/**
 * @brief Initialize handle with the new session primitives
 *
 * @param gw the handle initialized with winecord_gateway_init()
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_gateway_start(struct winecord_gateway *gw);

/**
 * @brief Cleanup and reset `gw` session primitives
 *
 * @param ws the WebSockets handle created with ws_init()
 * @return `true` if session is over, `false` if session can be retried for
 *      reconnection
 */
bool winecord_gateway_end(struct winecord_gateway *gw);

/**
 * @brief Check and manage on-going Gateway session
 *
 * @param req the request handler
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_gateway_perform(struct winecord_gateway *gw);

/**
 * @brief Gracefully shutdown a ongoing Winecord connection over WebSockets
 *
 * @param gw the handle initialized with winecord_gateway_init()
 */
void winecord_gateway_shutdown(struct winecord_gateway *gw);

/**
 * @brief Gracefully reconnect a ongoing Winecord connection over WebSockets
 *
 * @param gw the handle initialized with winecord_gateway_init()
 * @param resume true to attempt to resume to previous session,
 *        false restart a fresh session
 */
void winecord_gateway_reconnect(struct winecord_gateway *gw, bool resume);

/**
 * @brief Trigger the initial handshake with the gateway
 *
 * @param gw the handle initialized with winecord_gateway_init()
 * @param event provide client identification information
 */
void winecord_gateway_send_identify(struct winecord_gateway *gw,
                                   struct winecord_identify *event);

/**
 * @brief Replay missed events when a disconnected client resumes
 *
 * @param gw the handle initialized with winecord_gateway_init()
 * @param event session resume information
 */
void winecord_gateway_send_resume(struct winecord_gateway *gw,
                                 struct winecord_resume *event);

/**
 * @brief Maintain an active gateway connection
 *
 * @param gw the handle initialized with winecord_gateway_init()
 * @param seq the last session sequence number
 */
void winecord_gateway_send_heartbeat(struct winecord_gateway *gw, int seq);

/**
 * @brief Request all members for a guild or a list of guilds.
 *
 * @param gw the handle initialized with winecord_gateway_init()
 * @param event request guild members information
 */
void winecord_gateway_send_request_guild_members(
    struct winecord_gateway *gw, struct winecord_request_guild_members *event);

/**
 * @brief Sent when a client wants to join, move or disconnect from a voice
 *      channel
 *
 * @param gw the handle initialized with winecord_gateway_init()
 * @param event request guild members information
 */
void winecord_gateway_send_update_voice_state(
    struct winecord_gateway *gw, struct winecord_update_voice_state *event);

/**
 * @brief Send client's presence status update payload
 *
 * @param gw the handle initialized with winecord_gateway_init()
 * @param event the presence to be set
 */
void winecord_gateway_send_presence_update(
    struct winecord_gateway *gw, struct winecord_presence_update *event);

/**
 * @brief Dispatch user callback matched to event
 *
 * @param gw the handle initialized with winecord_gateway_init()
 */
void winecord_gateway_dispatch(struct winecord_gateway *gw);

/** @} WinecordInternalGateway */

/** @defgroup WinecordInternalRefcount Reference counter
 * @brief Handle automatic cleanup of user's data
 *  @{ */

/**
 * @brief Automatically cleanup user data
 *
 * Automatically cleanup user data that is passed around Winecord event's
 *      callbacks once its reference counter reaches 0, meaning there are no
 *      more callbacks expecting the data
 */
struct winecord_refcounter {
    /** `WINECORD_REFCOUNT` logging module */
    struct logconf conf;
    /** amount of individual user's data held for automatic cleanup */
    int length;
    /** cap before increase */
    int capacity;
    /**
     * individual user's data held for automatic cleanup
     * @note datatype declared at winecord-refcount.c
     */
    struct _winecord_ref *refs;
    /** global lock */
    pthread_mutex_t *g_lock;
};

/**
 * @brief Initialize reference counter handle
 *
 * A hashtable shall be used for storage and retrieval of user data
 * @param rc the reference counter handle to be initialized
 * @param conf pointer to @ref winecord logging module
 */
void winecord_refcounter_init(struct winecord_refcounter *rc,
                             struct logconf *conf);

/**
 * @brief Add a new internal reference to the reference counter
 *
 * @param rc the handle initialized with winecord_refcounter_init()
 * @param data the data address to be referenced
 * @param cleanup function for cleaning `data` resources once its
 *      no longer referenced
 * @param should_free whether `data` cleanup should be followed by a free()
 */
void winecord_refcounter_add_internal(struct winecord_refcounter *rc,
                                     void *data,
                                     void (*cleanup)(void *data),
                                     bool should_free);

/**
 * @brief Add a new client reference to the reference counter
 *
 * @param rc the handle initialized with winecord_refcounter_init()
 * @param data the data address to be referenced
 * @param cleanup function for cleaning `data` resources once its
 *      no longer referenced
 * @param should_free whether `data` cleanup should be followed by a free()
 */
void winecord_refcounter_add_client(struct winecord_refcounter *rc,
                                   void *data,
                                   void (*cleanup)(struct winecord *client,
                                                   void *data),
                                   bool should_free);

/**
 * @brief Cleanup refcounter and all user data currently held
 *
 * @param rc the handle initialized with winecord_refcounter_init()
 */
void winecord_refcounter_cleanup(struct winecord_refcounter *rc);

/**
 * @brief Claim ownership of `data`
 * @see winecord_refcounter_unclaim()
 *
 * After ownership is claimed `data` will no longer be cleaned automatically,
 *      instead shall be cleaned only when the same amount of
 *      winecord_refcounter_unclaim() have been called
 * @param rc the handle initialized with winecord_refcounter_init()
 * @param data the data to have its ownership claimed
 * @retval WINEBERRY_OK counter for `data` has been incremented
 * @retval WINEBERRY_RESOURCE_UNAVAILABLE couldn't find a match to `data`
 */
WINEBERRYcode winecord_refcounter_claim(struct winecord_refcounter *rc,
                                   const void *data);

/**
 * @brief Unclaim ownership of `data`
 * @see winecord_refcounter_claim()
 *
 * This will make the resource eligible for cleanup, so this should only be
 *      called when you no longer plan to use it
 * @param rc the handle initialized with winecord_refcounter_init()
 * @param data the data to have its ownership unclaimed
 * @retval WINEBERRY_OK counter for `data` has been decremented
 * @retval WINEBERRY_RESOURCE_UNAVAILABLE couldn't find a match to `data`
 * @retval WINEBERRY_RESOURCE_OWNERSHIP `data` has never been winecord_claim() 'd
 */
WINEBERRYcode winecord_refcounter_unclaim(struct winecord_refcounter *rc,
                                     void *data);

/**
 * @brief Increment the reference counter for `ret->data`
 * @see winecord_refcounter_decr()
 *
 * @param rc the handle initialized with winecord_refcounter_init()
 * @param data the data to have its reference counter incremented
 * @retval WINEBERRY_OK counter for `data` has been incremented
 * @retval WINEBERRY_RESOURCE_UNAVAILABLE couldn't find a match to `data`
 */
WINEBERRYcode winecord_refcounter_incr(struct winecord_refcounter *rc, void *data);

/**
 * @brief Decrement the reference counter for `data`
 * @see winecord_refcounter_incr()
 *
 * If the count reaches zero then `data` shall be cleanup up with its
 *      user-defined cleanup function
 * @param rc the handle initialized with winecord_refcounter_init()
 * @param data the data to have its reference counter decremented
 * @retval WINEBERRY_OK counter for `data` has been decremented
 * @retval WINEBERRY_RESOURCE_UNAVAILABLE couldn't find a match to `data`
 * @retval WINEBERRY_RESOURCE_OWNERSHIP caught attempt to cleanup a claimed
 * resource
 */
WINEBERRYcode winecord_refcounter_decr(struct winecord_refcounter *rc, void *data);

/** @} WinecordInternalRefcount */

/** @defgroup WinecordInternalMessageCommands Message Commands API
 * @brief The Message Commands API for registering and parsing user commands
 *  @{ */

/**
 * @brief The handle for storing user's message commands
 * @see winecord_set_on_command()
 */
struct winecord_message_commands {
    /** `WINECORD_MESSAGE_COMMANDS` logging module */
    struct logconf conf;
    /** the prefix expected for every command */
    struct ccord_szbuf prefix;
    /** fallback message command @see winecord_set_on_command() */
    winecord_ev_message fallback;
    /** amount of message commands created */
    int length;
    /** message commands cap before increase */
    int capacity;
    /**
     * message command entries
     * @note datatype declared at winecord-messagecommands.c
     */
    struct _winecord_message_commands_entry *entries;
};

/**
 * @brief Initialize a Message Commands handle
 *
 * @param cmds the message commands handle to be initialized
 * @param conf pointer to @ref winecord logging module
 */
void winecord_message_commands_init(struct winecord_message_commands *cmds,
                                   struct logconf *conf);

/**
 * @brief Free a Message Commands handle
 *
 * @param cmds the handle initialized with winecord_message_commands_init()
 */
void winecord_message_commands_cleanup(struct winecord_message_commands *cmds);

/**
 * @brief Search for a callback matching the command
 *
 * @param cmds the handle initialized with winecord_message_commands_init()
 * @param command the command to be searched for
 * @param length the command length
 * @return the callback match, `NULL` in case there wasn't a match
 */
winecord_ev_message winecord_message_commands_find(
    struct winecord_message_commands *cmds,
    const char command[],
    size_t length);

/**
 * @brief Add a new command/callback pair, or update an existing command
 *
 * @param cmds the handle initialized with winecord_message_commands_init()
 * @param command the message command to be matched with callback
 * @param length the command length
 * @param callback the callback to be triggered when the command is sent
 */
void winecord_message_commands_append(struct winecord_message_commands *cmds,
                                     const char command[],
                                     size_t length,
                                     winecord_ev_message callback);

/**
 * @brief Set a mandatory prefix before commands
 * @see winecord_set_on_command()
 *
 * Example: If @a 'help' is a command and @a '!' prefix is set, the command
 *       will only be validated if @a '!help' is sent
 * @param cmds the handle initialized with winecord_message_commands_init()
 * @param prefix the mandatory command prefix
 * @param length the prefix length
 */
void winecord_message_commands_set_prefix(struct winecord_message_commands *cmds,
                                         const char prefix[],
                                         size_t length);

/**
 * @brief Read the current @ref WINECORD_EV_MESSAGE_CREATE payload and attempt
 *      to perform its matching callback
 *
 * @param cmds the handle initialized with winecord_message_commands_init()
 * @param payload the event payload to read from
 *      (assumes its from `MESSAGE_CREATE`)
 * @return `true` if the callback has been performed
 */
bool winecord_message_commands_try_perform(
    struct winecord_message_commands *cmds,
    struct winecord_gateway_payload *payload);

/** @} WinecordInternalMessageCommands */

/** @defgroup WinecordInternalCache Cache API
 * @brief The Cache API for storage and retrieval of Winecord data
 *  @{ */

/**
 * @brief The Winecord Cache control handler
 */
struct winecord_cache {
    struct _winecord_cache_data *data;
    void (*cleanup)(struct winecord *client);

    /** gateway should call this when a shard has lost connection */
    void (*on_shard_disconnected)(struct winecord *client,
                                  const struct winecord_identify *ident,
                                  bool resumable);
    /** gateway should call this when a shard has resumed */
    void (*on_shard_resumed)(struct winecord *client,
                             const struct winecord_identify *ident);
    /** gateway should call this when a shard has reconnected */
    void (*on_shard_reconnected)(struct winecord *client,
                                 const struct winecord_identify *ident);
};

/** @} WinecordInternalCache */

/**
 * @brief The Winecord client handler
 *
 * Used to access/perform public functions from winecord.h
 * @see winecord_init(), winecord_config_init(), winecord_cleanup()
 */
struct winecord {
    /** `WINECORD` logging module */
    struct logconf conf;
    /** whether this is the original client or a clone */
    bool is_original;
    /** the bot token */
    char *token;
    /** the io poller for listening to file descriptors */
    struct io_poller *io_poller;

    /** the user's message commands @see winecord_set_on_command() */
    struct winecord_message_commands commands;
    /** user's data reference counter for automatic cleanup */
    struct winecord_refcounter refcounter;

    /** the handle for interfacing with Winecord's REST API */
    struct winecord_rest rest;
    /** the handle for interfacing with Winecord's Gateway API */
    struct winecord_gateway gw;
    /** the client's user structure */
    struct winecord_user self;
    /** the handle for registering and retrieving Winecord data */
    struct winecord_cache cache;

    /** fd that gets triggered when wineberry_shutdown_async is called */
    int shutdown_fd;

    struct {
        struct winecord_timers internal;
        struct winecord_timers user;
    } timers;

    /** wakeup timer handle */
    struct {
        /** callback to be triggered on timer's timeout */
        void (*cb)(struct winecord *client);
        /** the id of the wake timer */
        unsigned id;
    } wakeup_timer;

    /** triggers when idle */
    void (*on_idle)(struct winecord *client);
    /** triggers once per loop cycle */
    void (*on_cycle)(struct winecord *client);

    /** user arbitrary data @see winecord_set_data() */
    void *data;

    /** keep tab of amount of worker threads being used by client */
    struct {
        /** amount of worker-threads currently being used by client */
        int count;
        /** synchronize `count` between workers */
        pthread_mutex_t lock;
        /** notify of `count` decrement */
        pthread_cond_t cond;
    } * workers;

#ifdef WINEBERRY_VOICE
    struct winecord_voice *vcs;
    struct winecord_voice_evcallbacks voice_cbs;
#endif /* WINEBERRY_VOICE */
};

/** @} WinecordInternal */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* WINECORD_INTERNAL_H */
