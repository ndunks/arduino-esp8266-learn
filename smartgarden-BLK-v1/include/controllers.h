#pragma once
#include <ESP8266WebServer.h>

void handle_index(String &response, HTTPMethod method);
void handle_status(String &response, HTTPMethod method);
void handle_reboot(String &response, HTTPMethod method);
void handle_wifi(String &response, HTTPMethod method);
void handle_set_password(String &response, HTTPMethod method);
void handle_scan(String &response, HTTPMethod method);
void handle_config(String &response, HTTPMethod method);
void handle_settings(String &response, HTTPMethod method);
void handle_valve(String &response, HTTPMethod method);
void handle_sensor(String &response, HTTPMethod method);
void handle_pump_patch(String &response, HTTPMethod method);
