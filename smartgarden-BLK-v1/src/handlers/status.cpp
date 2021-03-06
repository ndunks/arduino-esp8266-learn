#include "web.h"
#include <base64.h>

// Status will periodically called by frontend, make it cheap
void handle_status(String &response, HTTPMethod method)
{
    WiFiMode_t mode = WiFi.getMode();
    response += "mode\t";
    response += mode;
    if ((mode & WIFI_STA))
    {
        wl_status_t status = WiFi.status();
        response += "\nstatus\t";
        response += status;
        if (status == WL_CONNECTED)
        {
            response += "\nrssi\t";
            response += WiFi.RSSI();
        }
    }
    if ((mode & WIFI_AP))
    {
        response += "\nap_clients\t";
        response += WiFi.softAPgetStationNum();
    }
    //uptime second
    response += "\nuptime\t";
    response += DETIK;
    // show sensors too
    response += "\n";
    handle_sensor(response, method);
    response += "\nfree\t";
    response += String(ESP.getFreeHeap());
}

// A supertset of Status with more detail, loaded onetime from frontend at first page load
// Also used on config dialog to display initial/current value
void handle_config(String &response, HTTPMethod method)
{
    WiFiMode_t mode = WiFi.getMode();
    response += "mode\t";
    response += mode;
    response += "\nhostname\t";
    response += WiFi.hostname();
    response += "\nautoconnect\t";
    response += WiFi.getAutoConnect();
    if ((mode & WIFI_STA))
    {
        wl_status_t status = WiFi.status();
        response += "\nstatus\t";
        response += status;
        response += "\nssid\t";
        response += WiFi.SSID();
        if (status == WL_CONNECTED)
        {
            response += "\nrssi\t";
            response += WiFi.RSSI();
            response += "\nip\t";
            response += WiFi.localIP().toString();
            response += "\ngateway\t";
            response += WiFi.gatewayIP().toString();
        }
    }
    if ((mode & WIFI_AP))
    {
        response += "\nap_ssid\t";
        response += WiFi.softAPSSID();
        response += "\nap_ip\t";
        response += WiFi.softAPIP().toString();
        response += "\nap_psk\t";
        response += WiFi.softAPPSK();
        response += "\nap_clients\t";
        response += WiFi.softAPgetStationNum();
    }
    //uptime second
    response += "\nuptime\t";
    response += DETIK;
    response += "\n";
    // show sensors too
    handle_sensor(response, method);
    response += "\nfree\t";
    response += String(ESP.getFreeHeap());
}

void handle_settings(String &response, HTTPMethod method)
{
    if (method == HTTP_POST)
    {
        String name = server.arg("name");
        String display = server.arg("display");
        if (name.length() == 0)
        {
            response = "Nama harus diisi";
            return;
        }
        if (name.length() > 20)
        {
            response = "Nama terlalu panjang";
            return;
        }
        if (display.length() == 0)
        {
            response = "Display harus diisi";
            return;
        }
        if (display.length() > 20)
        {
            response = "Display terlalu panjang";
            return;
        }
        strncpy(config->name, name.c_str(), name.length());
        config->name[name.length()] = 0;
        strncpy(config->displayText, display.c_str(), display.length());
        config->displayText[display.length()] = 0;
        if (!config_save())
        {
            response = "Gagal menyimpan data!";
            return;
        }
        response = "OK, Restart untuk memberikan efek.";
    }
    else
    {
        response = base64::encode((uint8_t *)config, sizeof(SmartGardenConfig), false);
    }
}

void handle_sensor(String &response, HTTPMethod method)
{
    uint8_t tmp = 0U;
    response += "in\t";
    for (tmp = 0; tmp < (VALVE_COUNT - 1); tmp++)
    {
        response += ANALOG_SENSOR[tmp];
        response += ' ';
    }
    response += '\n';
    // Suhu & kelemebaban
    response += "temp\t";
    response += TEMPERATURE;
    response += '\n';
    response += "hum\t";
    response += HUMIDITY;
    response += '\n';
    response += "cur\t";
    response += VALVE_CURRENT;
    response += '\n';
    response += "pompa\t";
    response += SERIAL_REG[PinSerial::Pompa];
    response += '\n';
    if (SERIAL_REG[PinSerial::Pompa])
    {
        response += "pompa_on\t";
        response += DETIK - pompa_nyala_sejak;
        response += '\n';
    }
    else
    {
        response += "pompa_off\t";
        if (pompa_mati_sampai > 0)
        {
            response += pompa_mati_sampai - DETIK;
        }
        else
        {
            response += 0;
        }
        response += '\n';
    }
    response += "state\t";
    response += VALVE_STATE;
    response += '\n';
    response += "laston\t";
    for (tmp = 0; tmp < VALVE_COUNT; tmp++)
    {
        response += VALVE_LAST_ON[tmp];
        response += ' ';
    }
}
