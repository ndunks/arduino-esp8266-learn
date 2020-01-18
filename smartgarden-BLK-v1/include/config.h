#pragma once
#include <Arduino.h>
#include "constant.h"
#include <ESP8266WiFi.h>

extern IPAddress local_IP;
extern char device_id[7];
// Config that stored on EEPROM
struct SmartGardenConfig
{
    // Delay valve in second
    uint8 valve_delay[VALVE_COUNT];
    // Default valve delay in second
    uint8 valve_delay_default;
    // Delay membaca sensor analog
    uint8 sensor_delay;
    // minimal kelembaban utk men trigger valve
    uint8 humidity_minimal[VALVE_COUNT];
    // Default valve delay in percent
    uint8 humidity_minimal_default;
    // max temperatur to trigger sprayer in celcius
    uint8 temperature_max;
    // Maksimal pompa menyala (detik)
    unsigned long maksimal_pompa_hidup;
    // Maksimal pompa mati/istirahat (detik)
    unsigned long maksimal_pompa_mati;
    // LCD Text when iddle
    char displayText[21];
    // Boot status flag
    uint8 flag;
    // Web Password
    char password[9];
};

// Default config will set on smartgarden_setup()
extern SmartGardenConfig *config;
void config_setup();
bool config_save();
void cofig_reset();
