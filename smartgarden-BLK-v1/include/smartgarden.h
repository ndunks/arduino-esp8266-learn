#pragma once
#include <Arduino.h>
#include "constant.h"

#define setValve(no, state) SERIAL_REG[VALVE_START + no] = state
#define getValve(no) SERIAL_REG[VALVE_START + no]
#define setPompa(state) SERIAL_REG[PinSerial::Pompa] = state
#define getPompa() SERIAL_REG[PinSerial::Pompa]
#define valveName(no) PinSerialNames[VALVE_START + no]
#define hasValveCurrent VALVE_CURRENT >= 0

// OUTUT/LED PIN
const uint8_t VALVE_START = PinSerial::Valve_0;

const uint8_t SPRAYER_NO = PinSerial::Sprayer - VALVE_START;

// Delay for checking input sensors, call set it before smartgarden_setup()
extern uint8_t SERIAL_REG[];

// Current analogs signal state
extern uint8_t ANALOG_SENSOR[];
extern uint8_t HUMIDITY;
// in celcius
extern int8_t TEMPERATURE;
extern int8_t VALVE_CURRENT;

/**
 * store all valve conditions, if bit set 1 mean need water
 * based on defined minimum on config
 */
extern uint8_t VALVE_STATE;
extern uint32_t VALVE_LAST_ON[VALVE_COUNT];
extern uint32_t pompa_nyala_sejak;
extern uint32_t pompa_mati_sampai;

// Setup pin and start ticker
void smartgarden_setup();
void smartgarden_loop();
void valveOn(int8 no);
