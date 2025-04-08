/**
 * @file winecord-cache.h
 * @author Cogmasters
 * @brief Caching of Winecord resources
 */

#ifndef WINECORD_CACHE_H
#define WINECORD_CACHE_H

/** @defgroup WinecordClientCache Caching
 * @ingroup WinecordClient
 * @brief Caching API supported by Winecord
 *  @{ */

enum winecord_cache_options {
    WINECORD_CACHE_MESSAGES = 1 << 0,
    WINECORD_CACHE_GUILDS = 1 << 1,
};

void winecord_cache_enable(struct winecord *client,
                          enum winecord_cache_options options);

/**
 * @brief Get a message from cache, only if locally available in RAM
 * @note When done, winecord_unclaim() must be called on the message resource
 *
 * @param client the client initialized with winecord_init()
 * @param channel_id the channel id the message is in
 * @param message_id the id of the message
 * @return `NULL` if not found, or a cache'd message
 */
const struct winecord_message *winecord_cache_get_channel_message(
    struct winecord *client, u64snowflake channel_id, u64snowflake message_id);

/**
 * @brief Get a guild from cache, only if locally available in RAM
 * @note When done, winecord_unclaim() must be called on the guild resource
 *
 * @param client the client initialized with winecord_init()
 * @param guild_id the id of the guild
 * @return `NULL` if not found, or a cache'd guild
 */
const struct winecord_guild *winecord_cache_get_guild(struct winecord *client,
                                                    u64snowflake guild_id);

/** @example cache.c
 * Demonstrates cache usage */

/** @} WinecordClientCache */

#endif /* WINECORD_CACHE_H */
