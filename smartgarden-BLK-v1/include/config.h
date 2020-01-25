#pragma once
#include <Arduino.h>
#include "constant.h"
#include <ESP8266WiFi.h>

extern IPAddress local_IP;
// Config that stored on EEPROM
struct SmartGardenConfig
{
    // Boot status flag
    uint8_t flag;
    // Delay membaca sensor analog dalam detik
    uint8_t sensor_delay;
    // max temperatur to trigger sprayer in celcius
    uint8_t temperature_max;
    // Delay valve in second
    uint8_t valve_delay[VALVE_COUNT];
    // minimal kelembaban utk men trigger valve
    uint8_t humidity_minimal[VALVE_COUNT];
    // Delay space before next check in second
    uint8_t valve_gap[VALVE_COUNT];
    // Maksimal pompa menyala (detik)
    uint32_t maksimal_pompa_hidup;
    // Maksimal pompa mati/istirahat (detik)
    uint32_t maksimal_pompa_mati;
    // Device name / hostname
    char name[21];
    // LCD Text when iddle
    char displayText[21];
    // Web Password
    char password[9];
    // 8 bit manual mode flag valve
    uint8_t valve_manual;
};

// Default config will set on smartgarden_setup()
extern SmartGardenConfig *config;
void config_setup();
bool config_save();
// hapus semua data
void config_reset();
