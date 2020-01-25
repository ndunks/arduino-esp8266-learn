#include "web.h"

struct Controller routes[] = {
    {"status", &handle_status, false},
    {"reboot", &handle_reboot, true},
    {"wifi", &handle_wifi, true},
    {"set-password", &handle_set_password, true},
    {"scan", &handle_scan, false},
    // Auth Will checked on controller function on edit only
    {"config", &handle_config, false},
    {"settings", &handle_settings, true},
    {"valve", &handle_valve, true},
    {"pump", &handle_pump_patch, true},
};

// End pointer
const Controller *routeEnd = routes + sizeof(routes) / sizeof(routes[0]);

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
                                  host.equals(WiFi.hostname()) ||
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


bool Handler::canHandle(HTTPMethod method, String path)
{
    if (method == HTTP_OPTIONS)
    {
        return true;
    }
    // no handle without subPath, eg: /api/
    if (path.length() <= prefix.length())
    {
        return false;
    }
    return path.startsWith(prefix);
}

bool Handler::handle(ESP8266WebServer &server, HTTPMethod method, String path)
{
    //P("Handler: %s\n", path.c_str());
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.sendHeader(F("Cache-Control"), F("no-cache"));
    if (method == HTTP_OPTIONS)
    {
        server.sendHeader(F("Access-Control-Allow-Methods"), F("GET,POST"));
        server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
        // 60 * 60 * 60 * 30 = 6480000
        server.sendHeader(F("Access-Control-Max-Age"), F("6480000"));
        server.send(200, "text/plain");
        return true;
    }
    String response;
    String subPath = path.substring(prefix.length());
    //P("Api Check: %s\n", subPath.c_str());
    Controller *matched = routes;

    while (matched < routeEnd)
    {
        if (subPath.equals(matched->path))
        {
            if (matched->mustLogin && !server_guard())
            {
                // blocked
                return true;
            }
            matched->function(response, method);
            if (response.length())
            {
                server.send(200, "text/plain", response);
            } // no response mean handled directly
            response.clear();
            return true;
        }
        matched++;
    }
    return false;
}
