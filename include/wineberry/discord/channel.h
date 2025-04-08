/**
 * @file channel.h
 * @author Cogmasters
 * @brief Channel public functions and datatypes
 */

#ifndef WINECORD_CHANNEL_H
#define WINECORD_CHANNEL_H

/* forward declaration */
struct winecord_ret_users;
/**/

/** @defgroup WineberryAPIChannel Channel
 * @ingroup WineberryAPI
 * @brief Channel's public API supported by Concord
 *  @{ */

/**
 * @brief Get channel from given id
 * @note If the channel is a thread, a thread member object is included in the
 * returned result
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to be retrieved
 * @WINEBERRY_ret_obj{ret,channel}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_channel(struct winecord *client,
                              u64snowflake channel_id,
                              struct winecord_ret_channel *ret);

/**
 * @brief Update a channel's settings
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to be modified
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,channel}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_modify_channel(struct winecord *client,
                                 u64snowflake channel_id,
                                 struct winecord_modify_channel *params,
                                 struct winecord_ret_channel *ret);

/**
 * @brief Delete a channel, or close a private message
 * @note Requires the MANAGE_CHANNELS permission for the guild, or
 *        MANAGE_THREADS if the channel is a thread
 * @note Deleting a category does not delete its child channels; they will have
 *        their parent_id removed and a `Channel Update Gateway` event will
 * fire for each of them
 * @note Fires a `Channel Delete` event (or `Thread Delete` if the channel
 *       was a thread)
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to be deleted
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,channel}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_channel(struct winecord *client,
                                 u64snowflake channel_id,
                                 struct winecord_delete_channel *params,
                                 struct winecord_ret_channel *ret);

/**
 * @brief Get messages for a given channel
 * @note If operating on a guild channel, this endpoint requires the
 *        VIEW_CHANNEL permission to be present on the current user
 * @note If the current user is missing the READ_MESSAGE_HISTORY permission
 *       in the channel then this will return no messages (since they cannot
 *        read the message history)
 * @note The before, after, and around keys are mutually exclusive, only one
 *       may be passed at a time
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to get messages from
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,messages}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_channel_messages(
    struct winecord *client,
    u64snowflake channel_id,
    struct winecord_get_channel_messages *params,
    struct winecord_ret_messages *ret);

/**
 * @brief Get a specific message in the channel
 * @note If operating on a guild channel, this endpoint requires the
 * 'READ_MESSAGE_HISTORY' permission to be present on the current user
 * @param client the client created with winecord_init()
 * @param channel_id the channel where the message resides
 * @param message_id the message itself
 * @WINEBERRY_ret_obj{ret,message}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_channel_message(struct winecord *client,
                                      u64snowflake channel_id,
                                      u64snowflake message_id,
                                      struct winecord_ret_message *ret);

/**
 * @brief Post a message to a guild text or DM channel
 * @note Fires a `Message Create` event
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to send the message at
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,message}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_create_message(struct winecord *client,
                                 u64snowflake channel_id,
                                 struct winecord_create_message *params,
                                 struct winecord_ret_message *ret);

/**
 * @brief Crosspost a message in a News Channel to following channels
 * @note This endpoint requires the 'SEND_MESSAGES' permission, if the current
 *        user sent the message, or additionally the 'MANAGE_MESSAGES'
 *        permission, for all other messages, to be present for the current
 *        user
 *
 * @param client the client created with winecord_init()
 * @param channel_id the news channel that will crosspost
 * @param message_id the message that will crospost
 * @WINEBERRY_ret_obj{ret,message}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_crosspost_message(struct winecord *client,
                                    u64snowflake channel_id,
                                    u64snowflake message_id,
                                    struct winecord_ret_message *ret);

/**
 * @brief Create a reaction for the message
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param message_id the message to receive a reaction
 * @param emoji_id the emoji id (leave as 0 if not a custom emoji)
 * @param emoji_name the emoji name
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_create_reaction(struct winecord *client,
                                  u64snowflake channel_id,
                                  u64snowflake message_id,
                                  u64snowflake emoji_id,
                                  const char emoji_name[],
                                  struct winecord_ret *ret);

/**
 * @brief Delete a reaction the current user has made for the message
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param message_id the message to have a reaction deleted
 * @param emoji_id the emoji id (leave as 0 if not a custom emoji)
 * @param emoji_name the emoji name
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_own_reaction(struct winecord *client,
                                      u64snowflake channel_id,
                                      u64snowflake message_id,
                                      u64snowflake emoji_id,
                                      const char emoji_name[],
                                      struct winecord_ret *ret);

/**
 * @brief Deletes another user's reaction
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param message_id the message to have a reaction deleted
 * @param user_id the user the reaction belongs to
 * @param emoji_id the emoji id (leave as 0 if not a custom emoji)
 * @param emoji_name the emoji name
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_user_reaction(struct winecord *client,
                                       u64snowflake channel_id,
                                       u64snowflake message_id,
                                       u64snowflake user_id,
                                       u64snowflake emoji_id,
                                       const char emoji_name[],
                                       struct winecord_ret *ret);

/**
 * @brief Get a list of users that reacted with given emoji
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param message_id the message reacted to
 * @param emoji_id the emoji id (leave as 0 if not a custom emoji)
 * @param emoji_name the emoji name
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,users}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_reactions(struct winecord *client,
                                u64snowflake channel_id,
                                u64snowflake message_id,
                                u64snowflake emoji_id,
                                const char emoji_name[],
                                struct winecord_get_reactions *params,
                                struct winecord_ret_users *ret);

/**
 * @brief Deletes all reactions from message
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param message_id the message that will be purged of reactions
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_all_reactions(struct winecord *client,
                                       u64snowflake channel_id,
                                       u64snowflake message_id,
                                       struct winecord_ret *ret);

/**
 * @brief Deletes all the reactions for a given emoji on message
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param message_id the message that will be purged of reactions from
 *       particular emoji
 * @param emoji_id the emoji id (leave as 0 if not a custom emoji)
 * @param emoji_name the emoji name
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_all_reactions_for_emoji(struct winecord *client,
                                                 u64snowflake channel_id,
                                                 u64snowflake message_id,
                                                 u64snowflake emoji_id,
                                                 const char emoji_name[],
                                                 struct winecord_ret *ret);

/**
 * @brief Edit a previously sent message
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param message_id the message that will be purged of reactions from
 *       particular emoji
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,message}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_edit_message(struct winecord *client,
                               u64snowflake channel_id,
                               u64snowflake message_id,
                               struct winecord_edit_message *params,
                               struct winecord_ret_message *ret);

/**
 * @brief Delete a message
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param message_id the message that will be purged of reactions from
 *       particular emoji
 * @param params request parameters
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_message(struct winecord *client,
                                 u64snowflake channel_id,
                                 u64snowflake message_id,
                                 struct winecord_delete_message *params,
                                 struct winecord_ret *ret);

/**
 * @brief Delete multiple messages in a single request
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param params request parameters
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_bulk_delete_messages(
    struct winecord *client,
    u64snowflake channel_id,
    struct winecord_bulk_delete_messages *params,
    struct winecord_ret *ret);

/**
 * @brief Edit the channel permission overwrites for a user or role in a
 *        channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param overwrite_id
 * @param params request parameters
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_edit_channel_permissions(
    struct winecord *client,
    u64snowflake channel_id,
    u64snowflake overwrite_id,
    struct winecord_edit_channel_permissions *params,
    struct winecord_ret *ret);

/**
 * @brief Get invites (with invite metadata) for the channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @WINEBERRY_ret_obj{ret,invites}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_channel_invites(struct winecord *client,
                                      u64snowflake channel_id,
                                      struct winecord_ret_invites *ret);

/**
 * @brief Create a new invite for the channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel that the message belongs to
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,invite}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_create_channel_invite(
    struct winecord *client,
    u64snowflake channel_id,
    struct winecord_create_channel_invite *params,
    struct winecord_ret_invite *ret);

/**
 * @brief Delete a channel permission overwrite for a user or role in a
 *        channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to the permission deleted
 * @param overwrite_id the id of the overwritten permission
 * @param params request parameters
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_delete_channel_permission(
    struct winecord *client,
    u64snowflake channel_id,
    u64snowflake overwrite_id,
    struct winecord_delete_channel_permission *params,
    struct winecord_ret *ret);

/**
 * @brief Post a typing indicator for the specified channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to post the typing indicator to
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_trigger_typing_indicator(struct winecord *client,
                                           u64snowflake channel_id,
                                           struct winecord_ret *ret);

/**
 * @brief Follow a News Channel to send messages to a target channel
 * @note Requires MANAGE_WEBHOOKS permission in the target channel
 *        MANAGE_WEBHOOKS permission in the target channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to be followed
 * @WINEBERRY_ret_obj{ret,followed_channel}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_follow_news_channel(
    struct winecord *client,
    u64snowflake channel_id,
    struct winecord_follow_news_channel *params,
    struct winecord_ret_followed_channel *ret);

/**
 * @brief Get all pinned messages in the channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel where the get pinned messages from
 * @WINEBERRY_ret_obj{ret,messages}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_pinned_messages(struct winecord *client,
                                      u64snowflake channel_id,
                                      struct winecord_ret_messages *ret);

/**
 * @brief Pin a message to a channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id channel to pin the message on
 * @param message_id message to be pinned
 * @param params request parameters
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_pin_message(struct winecord *client,
                              u64snowflake channel_id,
                              u64snowflake message_id,
                              struct winecord_pin_message *params,
                              struct winecord_ret *ret);

/**
 * @brief Unpin a message from a channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id channel for the message to be unpinned
 * @param message_id message to be unpinned
 * @param params request parameters
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_unpin_message(struct winecord *client,
                                u64snowflake channel_id,
                                u64snowflake message_id,
                                struct winecord_unpin_message *params,
                                struct winecord_ret *ret);

/**
 * @brief Adds a recipient to a Group DM using their access token
 *
 * @param client the client created with winecord_init()
 * @param channel_id group to add the user in
 * @param user_id user to be added
 * @param params request parameters
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_group_dm_add_recipient(
    struct winecord *client,
    u64snowflake channel_id,
    u64snowflake user_id,
    struct winecord_group_dm_add_recipient *params,
    struct winecord_ret *ret);

/**
 * @brief Removes a recipient from a Group DM
 *
 * @param client the client created with winecord_init()
 * @param channel_id channel for the user to be removed from
 * @param user_id user to be removed
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_group_dm_remove_recipient(struct winecord *client,
                                            u64snowflake channel_id,
                                            u64snowflake user_id,
                                            struct winecord_ret *ret);

/**
 * @brief Creates a new thread from an existing message
 * @note Fires a `Thread Create` event
 *
 * @param client the client created with winecord_init()
 * @param channel_id channel to start a thread on
 * @param message_id message to start a thread from
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,channel}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_start_thread_with_message(
    struct winecord *client,
    u64snowflake channel_id,
    u64snowflake message_id,
    struct winecord_start_thread_with_message *params,
    struct winecord_ret_channel *ret);

/**
 * @brief Creates a new thread that is not connected to an existing message
 * @note Fires a `Thread Create` event
 *
 * @param client the client created with winecord_init()
 * @param channel_id channel to start a thread on
 * @param params request parameters
 * @WINEBERRY_ret_obj{ret,channel}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_start_thread_without_message(
    struct winecord *client,
    u64snowflake channel_id,
    struct winecord_start_thread_without_message *params,
    struct winecord_ret_channel *ret);

/**
 * @brief Adds the current user to an un-archived thread
 * @note Fires a `Thread Members Update` event
 *
 * @param client the client created with winecord_init()
 * @param channel_id the thread to be joined
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_join_thread(struct winecord *client,
                              u64snowflake channel_id,
                              struct winecord_ret *ret);

/**
 * @brief Adds another member to an un-archived thread
 * @note Fires a `Thread Members Update` event
 *
 * @param client the client created with winecord_init()
 * @param channel_id the thread to be joined
 * @param user_id user to be added to thread
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_add_thread_member(struct winecord *client,
                                    u64snowflake channel_id,
                                    u64snowflake user_id,
                                    struct winecord_ret *ret);

/**
 * @brief Removes the current user from a un-archived thread
 * @note Fires a `Thread Members Update` event
 *
 * @param client the client created with winecord_init()
 * @param channel_id the thread to be removed from
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_leave_thread(struct winecord *client,
                               u64snowflake channel_id,
                               struct winecord_ret *ret);

/**
 * @brief Removes another member from a un-archived thread
 * @note Fires a `Thread Members Update` event
 * @note Requires `MANAGE_THREADS` permission
 *
 * @param client the client created with winecord_init()
 * @param channel_id the thread to be removed from
 * @param user_id user to be removed
 * @WINEBERRY_ret{ret}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_remove_thread_member(struct winecord *client,
                                       u64snowflake channel_id,
                                       u64snowflake user_id,
                                       struct winecord_ret *ret);

/**
 * @brief Get members from a given thread channel
 * @note Fires a `Thread Members Update` event
 * @note Requires `MANAGE_THREADS` permission
 *
 * @param client the client created with winecord_init()
 * @param channel_id the thread to be joined
 * @WINEBERRY_ret_obj{ret,thread_members}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_list_thread_members(struct winecord *client,
                                      u64snowflake channel_id,
                                      struct winecord_ret_thread_members *ret);

/**
 * @todo replace with
 * https://discord.com/developers/docs/resources/guild#list-active-threads
 * @deprecated Discord will deprecate this in V10
 * @brief Get all active threads in a given channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to be searched for threads
 * @WINEBERRY_ret_obj{ret,thread_response_body}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_list_active_threads(
    struct winecord *client,
    u64snowflake channel_id,
    struct winecord_ret_thread_response_body *ret);

/**
 * @brief Get public archived threads in a given channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to be searched for threads
 * @param before return threads before this timestamp
 * @param limit maximum number of threads to return
 * @WINEBERRY_ret_obj{ret,thread_response_body}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_list_public_archived_threads(
    struct winecord *client,
    u64snowflake channel_id,
    u64unix_ms before,
    int limit,
    struct winecord_ret_thread_response_body *ret);

/**
 * @brief Get private archived threads in a given channel
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to be searched for threads
 * @param before return threads before this timestamp
 * @param limit maximum number of threads to return
 * @WINEBERRY_ret_obj{ret,thread_response_body}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_list_private_archived_threads(
    struct winecord *client,
    u64snowflake channel_id,
    u64unix_ms before,
    int limit,
    struct winecord_ret_thread_response_body *ret);

/**
 * @brief Get private archived threads that current user has joined
 *
 * @param client the client created with winecord_init()
 * @param channel_id the channel to be searched for threads
 * @param before return threads before this timestamp
 * @param limit maximum number of threads to return
 * @WINEBERRY_ret_obj{ret,thread_response_body}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_list_joined_private_archived_threads(
    struct winecord *client,
    u64snowflake channel_id,
    u64unix_ms before,
    int limit,
    struct winecord_ret_thread_response_body *ret);

/** @defgroup WineberryAPIChannelEmbed Embed builder
 * @brief Dynamic embed builder functions
 *  @{ */

/**
 * @brief Add title to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 * longer being used
 *
 * @param embed the embed being modified
 * @param format printf-like formatting string
 * @param ... variadic arguments to be matched to format
 */
void winecord_embed_set_title(struct winecord_embed *embed, char format[], ...);

/**
 * @brief Add description to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 * longer being used
 *
 * @param embed the embed being modified
 * @param format printf-like formatting string
 * @param ... variadic arguments to be matched to format
 */
void winecord_embed_set_description(struct winecord_embed *embed,
                                   char format[],
                                   ...);

/**
 * @brief Add URL to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 * longer being used
 *
 * @param embed the embed being modified
 * @param format printf-like formatting string
 * @param ... variadic arguments to be matched to format
 */
void winecord_embed_set_url(struct winecord_embed *embed, char format[], ...);

/**
 * @brief Add thumbnail to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 * longer being used
 *
 * @param embed the embed being modified
 * @param url source url of thumbnail
 * @param proxy_url a proxied url of the thumbnail
 * @param height height of thumbnail
 * @param width width of thumbnail
 */
void winecord_embed_set_thumbnail(struct winecord_embed *embed,
                                 char url[],
                                 char proxy_url[],
                                 int height,
                                 int width);
/**
 * @brief Add image to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 * longer being used
 *
 * @param embed the embed being modified
 * @param url source url of image
 * @param proxy_url a proxied url of the image
 * @param height height of image
 * @param width width of image
 */
void winecord_embed_set_image(struct winecord_embed *embed,
                             char url[],
                             char proxy_url[],
                             int height,
                             int width);

/**
 * @brief Add video to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 * longer being used
 *
 * @param embed the embed being modified
 * @param url source url of video
 * @param proxy_url a proxied url of the video
 * @param height height of video
 * @param width width of video
 */
void winecord_embed_set_video(struct winecord_embed *embed,
                             char url[],
                             char proxy_url[],
                             int height,
                             int width);

/**
 * @brief Add footer to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 * longer being used
 *
 * @param embed the embed being modified
 * @param text footer text
 * @param icon_url url of footer icon
 * @param proxy_icon_url a proxied url of footer icon
 */
void winecord_embed_set_footer(struct winecord_embed *embed,
                              char text[],
                              char icon_url[],
                              char proxy_icon_url[]);

/**
 * @brief Add provider to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 * longer being used
 *
 * @param embed the embed being modified
 * @param name name of provider
 * @param url url of provider
 */
void winecord_embed_set_provider(struct winecord_embed *embed,
                                char name[],
                                char url[]);

/**
 * @brief Add author to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 *        longer being used
 *
 * @param embed the embed being modified
 * @param name name of author
 * @param url url of author
 * @param icon_url url of author icon
 * @param proxy_icon_url a proxied url of author icon
 */
void winecord_embed_set_author(struct winecord_embed *embed,
                              char name[],
                              char url[],
                              char icon_url[],
                              char proxy_icon_url[]);

/**
 * @brief Add field to embed
 * @note the embed must be freed with `winecord_embed_cleanup()` after its no
 *        longer being used
 *
 * @param embed the embed being modified
 * @param name name of the field
 * @param value value of the field
 * @param Inline whether or not this field should display inline
 */
void winecord_embed_add_field(struct winecord_embed *embed,
                             char name[],
                             char value[],
                             bool Inline);

/** @} WineberryAPIChannelEmbed */

/** @defgroup WineberryAPIChannelHelper Helper functions
 * @brief Custom helper functions
 *  @{ */

/**
 * @brief Get a guild's channel from its given numerical position
 *
 * @param client the client created with winecord_init()
 * @param guild_id guild the channel belongs to
 * @param type the channel type where to take position reference from
 * @WINEBERRY_ret_obj{ret,channel}
 * @WINEBERRY_return
 */
WINEBERRYcode winecord_get_channel_at_pos(struct winecord *client,
                                     u64snowflake guild_id,
                                     enum winecord_channel_types type,
                                     int position,
                                     struct winecord_ret_channel *ret);

/**
 * @brief Append to an overwrite list
 * @note the list should be freed with `winecord_overwrite_list_free()` after
 *        its no longer being used
 *
 * @param permission_overwrites list to be appended to
 * @param id role or user id
 * @param type either 0 (role) or 1 (member)
 * @param allow permission bit set
 * @param deny permission bit set
 */
void winecord_overwrite_append(struct winecord_overwrites *permission_overwrites,
                              u64snowflake id,
                              int type,
                              u64bitmask allow,
                              u64bitmask deny);

/** @} WineberryAPIChannelHelper */

/** @example channel.c
 * Demonstrates a couple use cases of the Channel API */
/** @example embed.c
 * Demonstrates embed manipulation */
/** @example fetch-messages.c
 * Demonstrates fetching user messages */
/** @example manual-dm.c
 * Demonstrates sending DMs with your client */
/** @example pin.c
 * Demonstrates pinning messages */
/** @example reaction.c
 * Demonstrates a couple use cases of the Channel reactions API */

/** @} WineberryAPIChannel */

#endif /* WINECORD_CHANNEL_H */
