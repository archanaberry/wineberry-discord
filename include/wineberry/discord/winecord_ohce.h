/**
 * @file wineberry-once.h
 * @author Cogmasters
 * @brief Initialized once
 */

#ifndef WINECORD_ONCE_H
#define WINECORD_ONCE_H

/** @brief Asynchronously shutdown all client(s) from their on-going sessions */
void wineberry_shutdown_async();

/**
 * @brief Whether or not winecord is currently shutting down
 *
 * If true, clients will then attempt to perform a clean disconnect, rather than
 *    just letting the program end abruptly (e.g. in the case of a SIGINT).
 * @note client shall only attempt to disconnect if there aren't any active
 *    events waiting to be listened or reacted to
 */
int wineberry_shutting_down();

/**
 * @brief Initialize global shared-resources not API-specific
 *
 * @return WINEBERRY_OK on success, WINEBERRY_GLOBAL_INIT on error
 */
WINEBERRY wineberry_global_init();

/** @brief Cleanup global shared-resources */
void wineberry_global_cleanup();

#endif /* WINECORD_ONCE_H */
