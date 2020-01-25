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
