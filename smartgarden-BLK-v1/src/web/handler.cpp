#include "web.h"

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
