#pragma once
#include <Arduino.h>
#include "constant.h"

// OUTUT/LED PIN
const uint8_t VALVE_START = PinSerial::Valve_0;

const uint8_t SPRAYER_NO = PinSerial::Sprayer - VALVE_START;

// Delay for checking input sensors, call set it before smartgarden_setup()
extern uint8_t SERIAL_REG[];
extern uint32_t smartgarden_delay;

// Current analogs signal state
extern uint8_t ANALOG_SENSOR[];
extern uint8_t HUMIDITY;
// in celcius
extern int8_t TEMPERATURE;
extern int8_t VALVE_CURRENT;
// extern uint32_t pompa_nyala_sejak;
// extern uint32_t pompa_mati_sampai;

// Setup pin and start ticker
void smartgarden_setup();
void smartgarden_loop();
void valveOn(int8 no);
// Apply REG to Serial Pararel Chained IC 595
void smartgarden_apply();
