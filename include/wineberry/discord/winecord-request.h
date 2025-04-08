/**
 * @file winecord-request.h
 * @ingroup WinecordInternalREST
 * @author Cogmasters
 * @brief Generic macros for initializing a @ref winecord_attributes
 */

#ifndef WINECORD_REQUEST_H
#define WINECORD_REQUEST_H

/* helper typedefs for casting */
typedef void (*cast_done_typed)(struct winecord *,
                                struct winecord_response *,
                                const void *);
typedef void (*cast_init)(void *);
typedef void (*cast_cleanup)(void *);
typedef size_t (*cast_from_json)(const char *, size_t, void *);

/* helper typedef for getting sizeof of `struct winecord_ret` common fields */
typedef struct {
    WINEBERRY_RET_DEFAULT_FIELDS;
} winecord_ret_default_fields;

#define _RET_COPY_TYPED(dest, src)                                            \
    do {                                                                      \
        memcpy(&(dest), &(src), sizeof(winecord_ret_default_fields));         \
        (dest).has_type = true;                                               \
        (dest).done.typed = (cast_done_typed)(src).done;                      \
        (dest).sync = (src).sync;                                             \
    } while (0)

#define _RET_COPY_TYPELESS(dest, src)                                         \
    do {                                                                      \
        memcpy(&(dest), &(src), sizeof(winecord_ret_default_fields));         \
        (dest).has_type = false;                                              \
        (dest).done.typeless = (src).done;                                    \
        (dest).sync = (void *)(src).sync;                                     \
    } while (0)

/**
 * @brief Helper for setting attributes for a specs-generated return struct
 *
 * @param[out] attr @ref winecord_attributes handler to be initialized
 * @param[in] type datatype of the struct
 * @param[in] ret dispatch attributes
 * @param[in] _reason reason for request (if available)
 */
#define WINECORD_ATTR_INIT(attr, type, ret, _reason)                          \
    do {                                                                      \
        (attr).response.size = sizeof(struct type);                           \
        (attr).response.init = (cast_init)type##_init;                        \
        (attr).response.from_json = (cast_from_json)type##_from_json;         \
        (attr).response.cleanup = (cast_cleanup)type##_cleanup;               \
        (attr).reason = _reason;                                              \
        if (ret) _RET_COPY_TYPED(attr.dispatch, *ret);                        \
    } while (0)

/**
 * @brief Helper for setting attributes for a specs-generated list
 *
 * @param[out] attr @ref winecord_attributes handler to be initialized
 * @param[in] type datatype of the list
 * @param[in] ret dispatch attributes
 * @param[in] _reason reason for request (if available)
 */
#define WINECORD_ATTR_LIST_INIT(attr, type, ret, _reason)                     \
    do {                                                                      \
        (attr).response.size = sizeof(struct type);                           \
        (attr).response.from_json = (cast_from_json)type##_from_json;         \
        (attr).response.cleanup = (cast_cleanup)type##_cleanup;               \
        (attr).reason = _reason;                                              \
        if (ret) _RET_COPY_TYPED(attr.dispatch, *ret);                        \
    } while (0)

/**
 * @brief Helper for setting attributes for requests that doesn't expect a
 *      response object
 *
 * @param[out] attr @ref winecord_attributes handler to be initialized
 * @param[in] ret dispatch attributes
 * @param[in] _reason reason for request (if available)
 */
#define WINECORD_ATTR_BLANK_INIT(attr, ret, _reason)                          \
    do {                                                                      \
        (attr).reason = _reason;                                              \
        if (ret) _RET_COPY_TYPELESS(attr.dispatch, *ret);                     \
    } while (0)

/**
 * @brief Helper for initializing attachments ids
 *
 * @param[in,out] attchs a @ref winecord_attachments to have its IDs initialized
 */
#define WINECORD_ATTACHMENTS_IDS_INIT(attchs)                                 \
    do {                                                                      \
        for (int i = 0; i < attchs->size; ++i) {                              \
            attchs->array[i].id = (u64snowflake)i;                            \
        }                                                                     \
    } while (0)

#endif /* WINECORD_REQUEST_H */
