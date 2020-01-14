#pragma once
#include <Arduino.h>
#include <user_interface.h>

// Serial to Pararel
#define SERIAL_DATA D7  //D7 --> DS Outputs the byte to transfer
#define SERIAL_LOAD D8  //D8 --> RCLK Controls the internal transference of data in SN74HC595 internal registers
#define SERIAL_CLOCK D5 //D5 --> SRCLK Generates the clock signal to control the transference of data

// SENSOR SUHU
#define SENSOR_SUHU_PIN D2 // D2

const struct _Pins595
{
    // Pin of first IC 74HC595
    uint8_t IC4051_SA = 0;
    uint8_t IC4051_SB = 1;
    uint8_t IC4051_SC = 2;
    uint8_t Pompa = 3;
    //uint8_t Unused_S04 = 4;
    //uint8_t Unused_S05 = 5;
    //uint8_t Unused_S06 = 6;
    //uint8_t Unused_S07 = 7;

    // Pin of second IC 74HC595, valve harus urutan
    uint8_t Valve_0 = 8;
    uint8_t Valve_1 = 9;
    uint8_t Valve_2 = 10;
    uint8_t Valve_3 = 11;
    uint8_t Valve_4 = 12;
    uint8_t Valve_5 = 13;
    uint8_t Sprayer = 14;
    //uint8_t Unused_S15 = 15;
} PinSerial;

// Analog/Digital (de)multiplexer CD4051
const struct _Pins4051
{
    uint8_t SensorTanah_0 = 0;
    uint8_t SensorTanah_1 = 1;
    uint8_t SensorTanah_2 = 2;
    uint8_t SensorTanah_3 = 3;
    uint8_t SensorTanah_4 = 4;
    uint8_t SensorTanah_5 = 5;
    // uint8_t Unused_6 = 6;
    // uint8_t Unused_7 = 7;
} PinAnalog;

// OUTUT/LED PIN
static const uint8_t VALVE_START = PinSerial.Valve_0;
static const uint8_t VALVE_COUNT = 7; // 6 penyiraman, 1 pengembunan
static const uint8_t VALVE_STACK_MAX = 20;

#define SPRAYER_NO 6 //  harus dibawah VALVE_COUNT

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
