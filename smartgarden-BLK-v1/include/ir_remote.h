#pragma once

/*********************************
 *  ir_remote.cpp
 *********************************/
#include "IRremoteESP8266.h"
#include "IRrecv.h"
#include "IRutils.h"
#include "constant.h"

enum RemoteButton : int8_t {
    BTN_1, // This must be 0, because linked with valve index!
    BTN_2,
    BTN_3,
    BTN_4,
    BTN_5,
    BTN_6,
    BTN_7,
    BTN_8,
    BTN_9,
    BTN_0,
    BTN_WILDCARD,
    BTN_HASH,
    BTN_UP,
    BTN_LEFT,
    BTN_OK,
    BTN_RIGHT,
    BTN_DOWN,
    BTN_UNKNOWN,
};

static const PROGMEM char *const RemoteButtonName[] = {
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "*",
    "#",
    "UP",
    "LEFT",
    "OK",
    "RIGHT",
    "DOWN",
    "UNKNOWN"};

struct Button
{
    RemoteButton remoteButton;
    // NEX Code ignored first byte (always 0xff on universal mini remote)
    uint16 code;
};

// Jika mau nambah code dari remote merk lain, mappingkan disini

extern Button *currentButton;
void ir_remote_setup();
void ir_remote_loop();