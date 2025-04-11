#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

/******************************************************************************
 * REST functions
 ******************************************************************************/

WINEBERRYcode
winecord_create_stage_instance(struct winecord *client,
                              struct winecord_create_stage_instance *params,
                              struct winecord_ret_stage_instance *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params->channel_id != 0, WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, IS_NOT_EMPTY_STRING(params->topic),
                 WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_create_stage_instance(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_stage_instance, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_POST,
                            "/stage-instances");
}

WINEBERRYcode
winecord_get_stage_instance(struct winecord *client,
                           u64snowflake channel_id,
                           struct winecord_ret_stage_instance *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_stage_instance, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/stage-instances/%" PRIu64, channel_id);
}

WINEBERRYcode
winecord_modify_stage_instance(struct winecord *client,
                              u64snowflake channel_id,
                              struct winecord_modify_stage_instance *params,
                              struct winecord_ret_stage_instance *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_modify_stage_instance(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_stage_instance, ret, params->reason);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_PATCH,
                            "/stage-instances/%" PRIu64, channel_id);
}

WINEBERRYcode
winecord_delete_stage_instance(struct winecord *client,
                              u64snowflake channel_id,
                              struct winecord_delete_stage_instance *params,
                              struct winecord_ret *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, channel_id != 0, WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_BLANK_INIT(attr, ret, params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/stage-instances/%" PRIu64, channel_id);
}
