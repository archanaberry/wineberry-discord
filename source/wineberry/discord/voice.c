#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winecord.h"
#include "winecord-internal.h"
#include "winecord-request.h"

WINEBERRYcode
winecord_list_voice_regions(struct winecord *client,
                           struct winecord_ret_voice_regions *ret)
{
    struct winecord_attributes attr = { 0 };

    WINECORD_ATTR_LIST_INIT(attr, winecord_voice_regions, ret, NULL);

    return winecord_rest_run(&client->rest, &attr, NULL, HTTP_GET,
                            "/voice/regions");
}
