#pragma once

#include "header.h"
#include "handlers.h"
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <FS.h>

typedef void (*HandlerFunction)(String &response, HTTPMethod method);
struct Controller
{
    const char *path;
    HandlerFunction function;
    bool mustLogin;
};


//  server.cpp
bool server_guard();
void web_setup();
void web_loop();
extern ESP8266WebServer server;

// handler.cpp
// Api Handler
class Handler : public RequestHandler
{
public:
    Handler(const char *uri = "/api/")
        : prefix(uri)
    {
    }
    bool canHandle(HTTPMethod method, String uri);
    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, String requestUri);

protected:
    String prefix;
};

// routes.cpp
extern struct Controller routes[];
