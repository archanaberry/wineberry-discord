#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "cog-utils.h"
#include "carray.h"

void
winecord_embed_set_footer(struct winecord_embed *embed,
                         char text[],
                         char icon_url[],
                         char proxy_icon_url[])
{
    if (!text || !*text) {
        log_error("Missing 'text'");
        return;
    }

    if (embed->footer)
        winecord_embed_footer_cleanup(embed->footer);
    else
        embed->footer = malloc(sizeof *embed->footer);
    winecord_embed_footer_init(embed->footer);

    if (text) cog_strndup(text, strlen(text), &embed->footer->text);
    if (icon_url)
        cog_strndup(icon_url, strlen(icon_url), &embed->footer->icon_url);
    if (proxy_icon_url)
        cog_strndup(proxy_icon_url, strlen(proxy_icon_url),
                    &embed->footer->proxy_icon_url);
}

void
winecord_embed_set_title(struct winecord_embed *embed, char format[], ...)
{
    char buf[WINECORD_EMBED_TITLE_LEN];
    va_list args;
    int len;

    va_start(args, format);

    len = vsnprintf(buf, sizeof(buf), format, args);
    ASSERT_NOT_OOB(len, sizeof(buf));

    if (embed->title) free(embed->title);
    cog_strndup(buf, (size_t)len, &embed->title);

    va_end(args);
}

void
winecord_embed_set_description(struct winecord_embed *embed, char format[], ...)
{
    char buf[WINECORD_EMBED_DESCRIPTION_LEN];
    va_list args;
    int len;

    va_start(args, format);

    len = vsnprintf(buf, sizeof(buf), format, args);
    ASSERT_NOT_OOB(len, sizeof(buf));

    if (embed->description) free(embed->description);
    cog_strndup(buf, (size_t)len, &embed->description);

    va_end(args);
}

void
winecord_embed_set_url(struct winecord_embed *embed, char format[], ...)
{
    char buf[2048];
    va_list args;
    int len;

    va_start(args, format);

    len = vsnprintf(buf, sizeof(buf), format, args);
    ASSERT_NOT_OOB(len, sizeof(buf));

    if (embed->url) free(embed->url);
    cog_strndup(buf, (size_t)len, &embed->url);

    va_end(args);
}

void
winecord_embed_set_thumbnail(struct winecord_embed *embed,
                            char url[],
                            char proxy_url[],
                            int height,
                            int width)
{
    if (embed->thumbnail)
        winecord_embed_thumbnail_cleanup(embed->thumbnail);
    else
        embed->thumbnail = malloc(sizeof *embed->thumbnail);
    winecord_embed_thumbnail_init(embed->thumbnail);

    if (url) cog_strndup(url, strlen(url), &embed->thumbnail->url);
    if (proxy_url)
        cog_strndup(proxy_url, strlen(proxy_url),
                    &embed->thumbnail->proxy_url);
    if (height) embed->thumbnail->height = height;
    if (width) embed->thumbnail->width = width;
}

void
winecord_embed_set_image(struct winecord_embed *embed,
                        char url[],
                        char proxy_url[],
                        int height,
                        int width)
{
    if (embed->image)
        winecord_embed_image_cleanup(embed->image);
    else
        embed->image = malloc(sizeof *embed->image);
    winecord_embed_image_init(embed->image);

    if (url) cog_strndup(url, strlen(url), &embed->image->url);
    if (proxy_url)
        cog_strndup(proxy_url, strlen(proxy_url), &embed->image->proxy_url);
    if (height) embed->image->height = height;
    if (width) embed->image->width = width;
}

void
winecord_embed_set_video(struct winecord_embed *embed,
                        char url[],
                        char proxy_url[],
                        int height,
                        int width)
{
    if (embed->video)
        winecord_embed_video_cleanup(embed->video);
    else
        embed->video = malloc(sizeof *embed->video);
    winecord_embed_video_init(embed->video);

    if (url) cog_strndup(url, strlen(url), &embed->video->url);
    if (proxy_url)
        cog_strndup(proxy_url, strlen(proxy_url), &embed->video->proxy_url);
    if (height) embed->video->height = height;
    if (width) embed->video->width = width;
}

void
winecord_embed_set_provider(struct winecord_embed *embed,
                           char name[],
                           char url[])
{
    if (embed->provider)
        winecord_embed_provider_cleanup(embed->provider);
    else
        embed->provider = malloc(sizeof *embed->provider);
    winecord_embed_provider_init(embed->provider);

    if (name) cog_strndup(name, strlen(name), &embed->provider->name);
    if (url) cog_strndup(url, strlen(url), &embed->provider->url);
}

void
winecord_embed_set_author(struct winecord_embed *embed,
                         char name[],
                         char url[],
                         char icon_url[],
                         char proxy_icon_url[])
{
    if (embed->author)
        winecord_embed_author_cleanup(embed->author);
    else
        embed->author = malloc(sizeof *embed->author);
    winecord_embed_author_init(embed->author);

    if (name) cog_strndup(name, strlen(name), &embed->author->name);
    if (url) cog_strndup(url, strlen(url), &embed->author->url);
    if (icon_url)
        cog_strndup(icon_url, strlen(icon_url), &embed->author->icon_url);
    if (proxy_icon_url)
        cog_strndup(proxy_icon_url, strlen(proxy_icon_url),
                    &embed->author->proxy_icon_url);
}

void
winecord_embed_add_field(struct winecord_embed *embed,
                        char name[],
                        char value[],
                        bool Inline)
{
    struct winecord_embed_field field = { 0 };

    field.Inline = Inline;

    if (name) cog_strndup(name, strlen(name), &field.name);
    if (value) cog_strndup(value, strlen(value), &field.value);

    if (!embed->fields)
        embed->fields = calloc(1, sizeof *embed->fields);
    carray_append(embed->fields, field);
}

void
winecord_overwrite_append(struct winecord_overwrites *permission_overwrites,
                         u64snowflake id,
                         int type,
                         u64bitmask allow,
                         u64bitmask deny)
{
    struct winecord_overwrite new_overwrite = { 0 };

    new_overwrite.id = id;
    new_overwrite.type = type;
    new_overwrite.allow = allow;
    new_overwrite.deny = deny;

    carray_append(permission_overwrites, new_overwrite);
}

void
winecord_presence_add_activity(struct winecord_presence_update *presence,
                              struct winecord_activity *activity)
{
    if (!presence->activities)
        presence->activities = calloc(1, sizeof *presence->activities);
    carray_append(presence->activities, *activity);
}
