#include "web.h"
#include "controllers.h"
#include "smartgarden.h"
#include "config.h"
#include <base64.h>

void handle_valve_patch(String &response, HTTPMethod method)
{
    int no = server.arg("no").toInt();
    int delay = server.arg("delay").toInt();
    int humidity = server.arg("humidity").toInt();
    config->humidity_minimal[no] = static_cast<uint8>(humidity);
    config->valve_delay[no] = static_cast<uint8>(delay);
    if (no == SPRAYER_NO)
    {
        config->temperature_max = static_cast<uint8>(server.arg("temp").toInt());
    }

    if (config_save())
    {
        //send the current settings
        handle_settings(response, method);
    }
    else
    {
        server.send(500);
    }
}
void handle_pump_patch(String &response, HTTPMethod method)
{
    config->maksimal_pompa_hidup = strtoul(server.arg("maxon").c_str(), NULL, 0);
    config->maksimal_pompa_mati = strtoul(server.arg("maxoff").c_str(), NULL, 0);

    if (config_save())
    {
        //send the current settings
        handle_settings(response, method);
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
        int no = server.arg(0).toInt();
        if (no >= 0 && no < VALVE_COUNT)
        {
            P("WEB ON %d\n", no);
            valveForceOn(no);
            // Send current sensor state
            handle_sensor(response, method);
        }
        else
        {
            server.send(401);
        }
    }
}
void handle_settings(String &response, HTTPMethod method)
{
    response = base64::encode((uint8_t *)config, sizeof(SmartGardenConfig), false);
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
    // 8 bit state
    // 0 - 5 : valve
    /* tmp = 0;
    tmp |= SERIAL_REG[PinSerial::Valve_0] << 0;
    tmp |= SERIAL_REG[PinSerial::Valve_1] << 1;
    tmp |= SERIAL_REG[PinSerial::Valve_2] << 2;
    tmp |= SERIAL_REG[PinSerial::Valve_3] << 3;
    tmp |= SERIAL_REG[PinSerial::Valve_4] << 4;
    tmp |= SERIAL_REG[PinSerial::Valve_5] << 5;
    // 6     : sprayer
    tmp |= SERIAL_REG[PinSerial::Sprayer] << 6;
    // 7     : pump
    tmp |= SERIAL_REG[PinSerial::Pompa] << 7;
    response += "out\t";
    response += tmp;
    response += '\n'; */
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
        response += (millis() - pompa_nyala_sejak) / 1000;
        response += '\n';
    }
    else
    {
        response += "pompa_off\t";
        response += (pompa_mati_sampai - millis()) / 1000;
        response += '\n';
    }
}
void handle_index(String &response, HTTPMethod method)
{
    //digitalWrite(ledPin, LOW);
    File f = SPIFFS.open("/index.html.gz", "r");
    server.streamFile(f, "text/html");
    f.close();
    //digitalWrite(ledPin, HIGH);
}

void handle_gpio(String &response, HTTPMethod method)
{
    response += String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    response += "|" + String(analogRead(A0));
}

void handle_ping(String &response, HTTPMethod method)
{
    response = server_guard();
}
// Status will periodically called by frontend, make it cheap
void handle_status(String &response, HTTPMethod method)
{
    uint32_t hfree = 0;
    uint16_t hmax = 0;
    uint8_t hfrag = 0;
    WiFiMode_t mode = WiFi.getMode();
    ESP.getHeapStats(&hfree, &hmax, &hfrag);
    response += "heap\t";
    response += String(hfree) + " " + String(hmax) + " " + String(hfrag);
    response += "\nmode\t";
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
    // show sensors too
    response += "\n";
    handle_sensor(response, method);
}

// A supertset of Status with more detail, loaded onetime from frontend at first page load
// Also used on config dialog to display initial/current value
void handle_config_get(String &response, HTTPMethod method)
{
    uint32_t hfree = 0;
    uint16_t hmax = 0;
    uint8_t hfrag = 0;
    WiFiMode_t mode = WiFi.getMode();
    ESP.getHeapStats(&hfree, &hmax, &hfrag);
    response += "heap\t";
    response += String(hfree) + " " + String(hmax) + " " + String(hfrag);
    response += "\nmode\t";
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
    response += "\n";
    // show sensors too
    handle_sensor(response, method);
}

void handle_config_set(String &response, HTTPMethod method)
{
    //Todo: will you contribute?
}

void handle_config(String &response, HTTPMethod method)
{
    if (method == HTTP_GET)
    {
        handle_config_get(response, method);
    }
    else if (server_guard())
    {
        handle_config_set(response, method);
    }
    else
    {
        response = "Disallowed";
    }
}

void handle_reboot(String &response, HTTPMethod method)
{
    ESP.restart();
}

void handle_wifi(String &response, HTTPMethod method)
{

    if (server.hasArg("connect"))
    {
        int id = server.arg("connect").toInt();
        String pass = server.arg("pass");
        WiFi.begin(WiFi.SSID(id).c_str(), pass.c_str(), WiFi.channel(id));
        WiFi.setAutoConnect(true);
        ESP.restart();
    }
    else if (server.hasArg("disconnect"))
    {
        WiFi.persistent(false);
        response = WiFi.disconnect(false);
        WiFi.persistent(true);
    }
    else if (server.hasArg("ap"))
    {
        response = WiFi.enableAP(server.arg("ap").equals("true"));
    }
    else if (server.hasArg("sta"))
    {
        if (WiFi.disconnect(true))
        {
            response = WiFi.enableSTA(server.arg("sta").equals("true"));
        }
        else
        {
            response = "Failed";
        }
    }
}

void handle_set_password(String &response, HTTPMethod method)
{

    if (server.hasArg("password"))
    {
        String password = server.arg("password");
        if (password.length() > 8)
        {
            response += "Password terlalu panjang, maks 8 huruf";
        }
        else
        {
            strncpy(config->password, password.c_str(), password.length());
            if (config_save())
            {
                response += "OK, Berhasil disimpan";
            }
            else
            {
                response += "Gagal menyimpan";
            }
        }
    }
    else
    {
        response += "Password tidak boleh kosong";
    }
}

void handle_scan(String &response, HTTPMethod method)
{
    int found = WiFi.scanNetworks();

    if (found == WIFI_SCAN_FAILED)
    {
        response += "FAILED";
    }
    else
    {
        for (int i = 0; i < found; i++)
        {
            response += String(WiFi.RSSI(i)) + "\t";
            switch (WiFi.encryptionType(i))
            {
            case ENC_TYPE_NONE:
                response += "none";
                break;
            case ENC_TYPE_WEP:
                response += "wep";
                break;
            case ENC_TYPE_TKIP:
                response += "tkip";
                break;
            case ENC_TYPE_CCMP:
                response += "ccmp";
                break;
            case ENC_TYPE_AUTO:
                response += "auto";
                break;
            };
            response += "\t" + WiFi.SSID(i);
            response += "\n";
        }
    };
}