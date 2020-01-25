#include "web.h"
#include "controllers.h"
#include "smartgarden.h"
#include "config.h"
#include "detik.h"
#include <base64.h>

void handle_valve_patch(String &response, HTTPMethod method)
{
    int no = server.arg("no").toInt();
    int delay = server.arg("delay").toInt();
    int humidity = server.arg("humidity").toInt();
    if (server.arg("manual").equals("true"))
    {
        config->valve_manual |= (1 << no);
        P("SET MANUAL\n");
    }
    else
    {
        config->valve_manual &= ~(1 << no);
        P("CLR MANUAL\n");
    }
    P("MANUAL %d\n", config->valve_manual);

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
    response += '\n';
}

void handle_index(String &response, HTTPMethod method)
{
    //digitalWrite(ledPin, LOW);
    File f = SPIFFS.open("/index.html.gz", "r");
    server.streamFile(f, "text/html");
    f.close();
    //digitalWrite(ledPin, HIGH);
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
    //uptime second
    response += "\nuptime\t";
    response += DETIK;
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
    //uptime second
    response += "\nuptime\t";
    response += DETIK;
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
        response = F("Disallowed");
    }
}

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
void handle_wifi(String &response, HTTPMethod method)
{
    const String F_CONNECT("connect");
    const String F_DISCONNECT("disconnect");
    P("Handle Wifi\n");
    if (server.hasArg(F_CONNECT))
    {
        int id = server.arg(F_CONNECT).toInt();
        String pass = server.arg(F("pass"));
        WiFi.begin(WiFi.SSID(id).c_str(), pass.c_str(), WiFi.channel(id));
        WiFi.setAutoConnect(true);
        ESP.restart();
    }
    else if (server.hasArg(F_DISCONNECT))
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
        P("SET STA MODE %s\n", server.arg("sta").c_str());
        delay(100);
        if (WiFi.disconnect(true))
        {
            response = "OK";
        }
        else
        {
            response = "Failed";
        }
    }
}

void handle_set_password(String &response, HTTPMethod method)
{

    if (server.hasArg(F("password")))
    {
        String password = server.arg(F("password"));
        if (password.length() > 8)
        {
            response += F("Password terlalu panjang, maks 8 huruf");
        }
        else
        {
            strncpy(config->password, password.c_str(), password.length());
            // make sure null terminated
            config->password[password.length()] = 0;
            if (config_save())
            {
                response += F("OK, Berhasil disimpan");
            }
            else
            {
                response += F("Gagal menyimpan");
            }
        }
    }
    else
    {
        response += F("Password tidak boleh kosong");
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