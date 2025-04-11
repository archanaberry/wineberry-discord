#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

WINEBERRYcode
winecord_get_current_bot_application_information(
    struct winecord *client, struct winecord_ret_application *ret)
{
    struct winecord_attributes attr = { 0 };

    WINECORD_ATTR_INIT(attr, winecord_application, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/oauth2/applications/@me");
}

WINEBERRYcode
winecord_get_current_authorization_information(
    struct winecord *client, struct winecord_ret_auth_response *ret)
{
    struct winecord_attributes attr = { 0 };

    WINECORD_ATTR_INIT(attr, winecord_auth_response, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/oauth2/@me");
}
