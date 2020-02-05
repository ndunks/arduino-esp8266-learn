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
    server.send(403, "text/plain", "Unauthorized");
    return false;
}
void web_on_notfound()
{
    String host = server.hostHeader();
    bool isIpAddressLike = false;
    for (uint i = 0; i < host.length(); i++)
    {
        isIpAddressLike = host.charAt(i) >= '.' && host.charAt(i) <= '9';
        if (!isIpAddressLike)
            break;
    }

#ifdef SMARTGARDEN_DEBUG
    P("404: %s %s%s\n",
      isIpAddressLike ? GREEN("IP") : BLUE("DOMAIN"),
      host.c_str(),
      server.uri().c_str());
#endif

    if (isIpAddressLike ||
        host.equalsIgnoreCase(config->name) ||
        host.equalsIgnoreCase(WiFi.hostname()))
    {
        P("SPA\n");
        File f = SPIFFS.open("/index.html.gz", "r");
        server.streamFile(f, mime::mimeTable[mime::html].mimeType);
        f.close();
    }
    else
    {
        String redirect("http://");
        redirect += config->name;
        server.sendHeader("Location", redirect.c_str());
        server.send(302);
        P("%s\n", redirect.c_str());
    }
    host.clear();
}
void web_setup()
{

    if (SPIFFS.begin())
    {
        server.addHandler(&handler);
        server.serveStatic("/", SPIFFS, "/", "public, max-age=86400");

        server.onNotFound(&web_on_notfound);
    }
    else
    {
        status(String(F("[!] FileSystem ERROR")).c_str());
        server.onNotFound([]() {
            server.send(200, FPSTR(mime::mimeTable[mime::html].mimeType), String(F("<html><body><h1>FILE SYSTEM ERROR</h1></body></html>")));
        });
    }
    server.begin();
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
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
        server.send(200, mime::mimeTable[mime::txt].mimeType);
        return true;
    }
    String response;
    String subPath = path.substring(prefix.length());
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
                server.send(200, mime::mimeTable[mime::txt].mimeType, response);
            }
            else
            {
                // no response mean handled directly
                P("WARN: Handler without response on %s\n", matched->path);
            }
            response.clear();
            return true;
        }
        matched++;
    }
    return false;
}
