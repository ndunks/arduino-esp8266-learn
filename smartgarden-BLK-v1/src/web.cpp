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
    P("Blocked by GUARD\n");
    server.send(403, "text/plain", "Unauthorized");
    return false;
}

void web_setup()
{

    if (SPIFFS.begin())
    {
        server.addHandler(&handler);
        server.serveStatic("/", SPIFFS, "/", "public, max-age=86400");

        server.onNotFound([]() {
            String host = server.hostHeader();
            P("Not found: %s %s\n", host.c_str(), server.uri().c_str());
            if (host.length() && (host.equalsIgnoreCase(config->name) ||
                                  host.equals(WiFi.localIP().toString()) ||
                                  host.equals(WiFi.softAPIP().toString()) ||
                                  server.uri().endsWith("generate_204")))
            {
                P("SPA\n");
                handle_index(*((String *)&emptyString), server.method());
            }
            else
            {
                String redirect("http://");
                redirect += config->name;
                server.sendHeader("Location", redirect.c_str());
                server.send(302);
                P("Handled Captive Portal %s\n", redirect.c_str());
            }
        });
    }
    else
    {
        status("[!] FileSystem ERROR");
        server.onNotFound([]() {
            server.send(200, "text/html", "<html><body><h1>FILE SYSTEM ERROR</h1></body></html>");
        });
    }
    server.begin();
    dnsServer.setErrorReplyCode(DNSReplyCode::Refused);
    dnsServer.start(53, "*", local_IP);
}

void web_loop()
{
    dnsServer.processNextRequest();
    server.handleClient();
}
