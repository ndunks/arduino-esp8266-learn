#include "web.h"

void handle_valve_patch(String &response, HTTPMethod method)
{
    int no = server.arg("no").toInt();
    int delay = server.arg("delay").toInt();
    int humidity = server.arg("humidity").toInt();
    if (server.arg("manual").equals("true"))
    {
        config->valve_manual |= (1 << no);
    }
    else
    {
        config->valve_manual &= ~(1 << no);
    }

    config->humidity_minimal[no] = static_cast<uint8>(humidity);
    config->valve_delay[no] = static_cast<uint8>(delay);

    if (no == SPRAYER_NO)
    {
        config->temperature_max = static_cast<uint8>(server.arg("temp").toInt());
    }

    if (config_save())
    {
        //send the current settings
        handle_settings(response, HTTPMethod::HTTP_GET);
    }
    else
    {
        server.send(500);
    }
}
void handle_pump_patch(String &response, HTTPMethod method)
{
    P("HANDLE PUMP PATCH\n");
    config->maksimal_pompa_hidup = strtoul(server.arg("maxon").c_str(), NULL, 0);
    config->maksimal_pompa_mati = strtoul(server.arg("maxoff").c_str(), NULL, 0);
    config->delay_valve_off = strtoul(server.arg("delay_valve_off").c_str(), NULL, 0);

    if (config_save())
    {
        //send the current settings
        handle_settings(response, HTTPMethod::HTTP_GET);
    }
    else
    {
        server.send(500);
    }
}
void handle_valve(String &response, HTTPMethod method)
{
    if (method == HTTPMethod::HTTP_POST)
    {
        handle_valve_patch(response, method);
    }
    else if (server.args())
    {
        int no = server.arg("no").toInt();
        if (no < VALVE_COUNT)
        {
            P("WEB ON %d\n", no);
            valveOn(no);
            // Send current sensor state
            handle_sensor(response, method);
            if (no < 0 && server.hasArg("off"))
            {
                pompa_mati_sampai = DETIK + server.arg("off").toInt();
            }
        }
        else
        {
            server.send(401);
        }
    }
}
