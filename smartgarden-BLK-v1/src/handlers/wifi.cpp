#include "web.h"

void handle_scan(String &response, HTTPMethod method)
{
    int found = WiFi.scanNetworks();
    switch (found)
    {
    case WIFI_SCAN_FAILED:
        /* code */
        response = "FAILED";
        break;
    case 0:
        response = "\n";
        break;
    default:
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
        break;
    }
}

const char PROGMEM F_CONNECT[] = "connect";
const char PROGMEM F_DISCONNECT[] = "disconnect";

void handle_wifi(String &response, HTTPMethod method)
{
    if (server.hasArg(FPSTR(F_CONNECT)))
    {
        if (server.hasArg(F("pass")))
        {
            WiFi.begin(server.arg(FPSTR(F_CONNECT)).c_str(), server.arg(F("pass")).c_str());
        }
        else
        {
            WiFi.enableInsecureWEP(true);
            WiFi.begin(server.arg(FPSTR(F_CONNECT)).c_str());
        }
        response = "OK";
    }
    else if (server.hasArg(FPSTR(F_DISCONNECT)))
    {
        response = WiFi.disconnect(false) ? "OK" : "FAIL";
    }
    else if (server.hasArg("ap"))
    {
        response = WiFi.enableAP(server.arg("ap").equals("true")) ? "OK" : "FAIL";
    }
    else if (server.hasArg("sta"))
    {
        bool enable = server.arg("sta").equals("true");
        response = WiFi.enableSTA(enable) ? "OK" : "FAIL";
    }
}
