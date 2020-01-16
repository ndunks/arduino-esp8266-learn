#pragma once
#include <Arduino.h>
#include <user_interface.h>
#include "constant.h"
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

void displayStatus(const char *txt);
void clearStatus();
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
    // minimal kelembaban utk men trigger valve
    uint8 humidity_minimal[VALVE_COUNT];
    // Default valve delay in percent
    uint8 humidity_minimal_default;
    // max temperatur to trigger sprayer in celcius
    uint8 temperature_max;
    // Maksimal pompa menyala (detik)
    uint8 maksimal_pompa_hidup;
    // Maksimal pompa mati/istirahat (detik)
    uint8 maksimal_pompa_mati;
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

// Activate analog selector
void smartgarden_set_analog(int no);
int smartgarden_read_analog(int no);

#define COLOR_BLACK 30
#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_BLUE 34
#define COLOR_MAGENTA 35
#define COLOR_CYAN 36

#ifdef SMARTGARDEN_DEBUG
#define P(fmt, ...) ::printf(fmt, ##__VA_ARGS__)

#define ONOFF(val) (String("\x1b[") +                       \
                    (val ? COLOR_GREEN : COLOR_RED) +       \
                    "m" + (val ? "ON" : "OFF") + "\x1b[0m") \
                       .c_str()
#define BLACK(str) (String("\x1b[") + COLOR_BLACK + "m" + str + "\x1b[0m").c_str()
#define RED(str) (String("\x1b[") + COLOR_RED + "m" + str + "\x1b[0m").c_str()
#define GREEN(str) (String("\x1b[") + COLOR_GREEN + "m" + str + "\x1b[0m").c_str()
#define YELLOW(str) (String("\x1b[") + COLOR_YELLOW + "m" + str + "\x1b[0m").c_str()
#define BLUE(str) (String("\x1b[") + COLOR_BLUE + "m" + str + "\x1b[0m").c_str()
#define MAGENTA(str) (String("\x1b[") + COLOR_MAGENTA + "m" + str + "\x1b[0m").c_str()
#define CYAN(str) (String("\x1b[") + COLOR_CYAN + "m" + str + "\x1b[0m").c_str()

#endif
#ifndef SMARTGARDEN_DEBUG
#define P(...)   \
    do           \
    {            \
        (void)0; \
    } while (0)

#define ONOFF(...) P()
#define BLACK(str) P()
#define RED(str) P()
#define GREEN(str) P()
#define YELLOW(str) P()
#define BLUE(str) P()
#define MAGENTA(str) P()
#define CYAN(str) P()
#endif

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
