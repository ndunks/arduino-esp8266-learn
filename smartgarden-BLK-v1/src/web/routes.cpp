#include "web.h"

struct Controller routes[] = {
    {"status", &handle_status, false},
    {"gpio", &handle_gpio, false},
    {"ping", &handle_ping, false},
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
Controller *routeEnd = routes + sizeof(routes) / sizeof(routes[0]);
