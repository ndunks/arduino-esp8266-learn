#include "header.h"
#include "web.h"

ESP8266WebServer server;
WiFiClient client;
char method[8] = {0};
char path[100] = {0};
DNSServer dnsServer;
Handler handler;

bool server_guard()
{
    if (
        (server.hasHeader("authorization") && server.header("authorization").equals(config->password)) ||
        (server.hasArg("auth") && server.arg("auth").equals(config->password)))
    {
        return true;
    }
    Serial.println("Blocked by GUARD");
    server.send(403, "text/plain", "Unauthorized");
    return false;
}

void web_setup()
{
    P("Server web_setup");
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", local_IP);
    if (!SPIFFS.begin())
    {
        status("[!] FileSystem ERROR");
        return;
    }
    server.addHandler(&handler);
    server.serveStatic("/", SPIFFS, "/", "public, max-age=86400");

    server.onNotFound([]() {
        String host = server.hostHeader();
        P("Not found: %s %s\n", host.c_str(), server.uri().c_str());
        if (host.length() && (host.equalsIgnoreCase(WiFi.hostname()) ||
                              host.equals(WiFi.localIP().toString()) ||
                              host.equals(WiFi.softAPIP().toString()) ||
                              server.uri().endsWith("generate_204")))
        {
            Serial.println("SPA");
            handle_index(*((String *)&emptyString), server.method());
        }
        else
        {
            server.sendHeader("Location", "http://" + WiFi.hostname() + "/");
            server.send(302);
            P("Handled Captive Portal");
        }
    });
    server.begin();
}

void web_loop()
{
    dnsServer.processNextRequest();
    server.handleClient();
}
