#include "web.h"

void handle_reboot(String &response, HTTPMethod method)
{
    if (server.hasArg(F("factory")))
    {
        server.send(200);
        yield();
        config_reset();
    }
    else
    {
        ESP.restart();
    }
}