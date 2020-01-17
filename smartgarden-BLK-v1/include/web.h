#pragma once
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <FS.h>
#include "controllers.h"
#include "constant.h"

// #define ledPin D4
// //#define btnPin D8 // RX GPIO 03
// #define btnPin 0 // GPIO 0 / FLASH BUTTON

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
extern Controller *routeEnd;
