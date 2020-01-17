#pragma once
#include <Arduino.h>
#include "constant.h"
#include "web.h"


void status(const char *txt, ...);

// Delay for checking input sensors, call set it before smartgarden_setup()
extern uint8 SERIAL_REG[];
extern unsigned long smartgarden_delay;

// Current analogs signal state
extern uint8 ANALOG_SENSOR[];
extern uint8 HUMIDITY;
// in celcius
extern int8 TEMPERATURE;

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
    char displayText[21];
};

// Default config will set on smartgarden_setup()
extern SmartGardenConfig *smartgarden_config;

// Setup pin and start ticker
void smartgarden_setup();
void smartgarden_loop();

// Apply REG to Serial Pararel Chained IC 595
void smartgarden_apply();

enum STATUS_t
{
    OFF = LOW,
    ON = HIGH
};

/*********************************
 *  sensorsuhu.cpp
 *********************************/
enum DHT_MODEL_t
{
    DHT11,
    DHT22
};

enum DHT_ERROR_t
{
    ERROR_NONE = 0,
    ERROR_TIMEOUT,
    ERROR_CHECKSUM
};

extern DHT_MODEL_t sensorsuhu_model;
extern DHT_ERROR_t sensorsuhu_error;
void sensorsuhu_read();

/*********************************
 *  ir_remote.cpp
 *********************************/
void ir_remote_setup();
extern int8_t currentButton;
bool ir_remote_read();