#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

WINEBERRYcode
winecord_get_invite(struct winecord *client,
                   char *invite_code,
                   struct winecord_get_invite *params,
                   struct winecord_ret_invite *ret)
{
    struct winecord_attributes attr = { 0 };
    struct wineberry_szbuf body;
    char buf[1024];

    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(invite_code), WINEBERRY_BAD_PARAMETER, "");
    WINEBERRY_EXPECT(client, params != NULL, WINEBERRY_BAD_PARAMETER, "");

    body.size = winecord_get_invite_to_json(buf, sizeof(buf), params);
    body.start = buf;

    WINECORD_ATTR_INIT(attr, winecord_invite, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, &body, HTTP_GET,
                            "/invites/%s", invite_code);
}

WINEBERRYcode
winecord_delete_invite(struct winecord *client,
                      char *invite_code,
                      struct winecord_delete_invite *params,
                      struct winecord_ret_invite *ret)
{
    struct winecord_attributes attr = { 0 };

    WINEBERRY_EXPECT(client, NOT_EMPTY_STR(invite_code), WINEBERRY_BAD_PARAMETER, "");

    WINECORD_ATTR_INIT(attr, winecord_invite, ret,
                      params ? params->reason : NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_DELETE,
                            "/invites/%s", invite_code);
}
