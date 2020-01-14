#pragma once
#include <Arduino.h>
#include <user_interface.h>
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

/**
 * 8 Input Analog/Digital
 * 16 Output digital 
 * 
 * OUT IC 595 (2 chain) Serial output mapping:
 *   ------------- FIRST IC 595
 * 0 - 2    To control input multiplexer IC 4051 (SA - SC)
 * 3        Pompa DC
 * 4 - 6    UNUSED
 *   ------------- SECOND IC 595
 * 8 - 14   Control Solenoid Valve/Led (see: OUTSTART)
 * 15       Solenoid pengembunan
 * 
 * IN IC 4051 Analog/Digital mapping:
 * 0 - 5    Sensor tanah (analog)
 * 6        UNUSED
 * 7        DHT22 (digital)
 */

// Serial to Pararel
#define SERIAL_DATA 13  //D7 --> DS Outputs the byte to transfer
#define SERIAL_LOAD 15  //D8 --> RCLK Controls the internal transference of data in SN74HC595 internal registers
#define SERIAL_CLOCK 14 //D5 --> SRCLK Generates the clock signal to control the transference of data

// SENSOR SUHU
#define SENSOR_SUHU_PIN 4 // D2

// OUTUT/LED PIN
#define VALVE_START 8
#define VALVE_COUNT 7 // 6 penyiraman, 1 pengembunan
#define VALVE_STACK_MAX 20
#define POMPA_NO 3
#define SPRAYER_NO 6 //  harus dibawah VALVE_COUNT

// IC595 -> IC4051
#define SA 0
#define SB 1
#define SC 2

// Delay for checking input sensors, call set it before smartgarden_setup()
extern uint8 REG[];
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

/*  sensorsuhu.h */
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
