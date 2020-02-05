#include <Arduino.h>
#include "smartgarden.h"
#include "constant.h"
#include "ir_remote.h"
#include "sensorsuhu.h"
#include "config.h"
#include "display.h"
#define isValveManual(i) ((config->valve_manual >> i) & 0x1)

// Serial registers
uint8_t SERIAL_REG[16] = {};

// Analog 0 - 5    Sensor Kelembaban Tanah, tanpa DHT22
uint8_t ANALOG_SENSOR[VALVE_COUNT - 1] = {};

// Analog 6
uint8_t HUMIDITY = 0;
int8_t TEMPERATURE = 0;

// Valve yg saat ini sedang on
int8_t VALVE_CURRENT = -1;
int8_t VALVE_OFF_AFTER_DELAY = -1;

uint8_t VALVE_STATE = 0;
// In seconds
uint8_t DELAY_CLOSE_ALL_VALVE = 2;

// Last valve turned on, in second
uint32_t VALVE_LAST_ON[VALVE_COUNT] = {};
uint32_t LAST_LOOP = 0;

uint32_t pompa_nyala_sejak = 0;
uint32_t pompa_mati_sampai = 0;
// time sejak pompa mati
uint32_t timer_valve_delay_off = 0;
// Capacitive Soil Moisture Sensor v.12

// Jika kurang dari ini, berarti tidak ada sensor dipasang
uint16_t SOIL_CALIBRATE_LOWEST = 170u;
// KERING = voltase tinggi = max 745
uint16_t SOIL_CALIBRATE_MAX = 745u;
// BASAH  = voltase rendah = min 270
uint16_t SOIL_CALIBRATE_MIN = 270u;
float_t SOIL_PERCENTAGE = 100.0f / static_cast<float_t>(SOIL_CALIBRATE_MAX - SOIL_CALIBRATE_MIN);

/** Apply REG value to 2 chained IC 595 */
void serialApply()
{
    digitalWrite(SERIAL_LOAD, LOW);
    for (int i = 15; i >= 0; i--)
    {
        digitalWrite(SERIAL_DATA, SERIAL_REG[i] & B1);
        digitalWrite(SERIAL_CLOCK, HIGH);
        digitalWrite(SERIAL_CLOCK, LOW);
    }
    digitalWrite(SERIAL_LOAD, HIGH);
}

uint8_t soil_persentase(int value)
{
    if (value > SOIL_CALIBRATE_MAX)
    {
        value = SOIL_CALIBRATE_MAX;
    }
    else if (value <= SOIL_CALIBRATE_LOWEST)
    {
        // tidak ada sensor dipasang
        value = SOIL_CALIBRATE_MAX;
    }
    else if (value < SOIL_CALIBRATE_MIN)
    {
        value = SOIL_CALIBRATE_MIN;
    }
    return static_cast<uint8_t>(100 - ((value - SOIL_CALIBRATE_MIN) * SOIL_PERCENTAGE));
}

void sensorUpdate()
{
    // Sensor Suhu
    sensorsuhu_read();

    // readAllAnalog
    for (int i = 0; i < (VALVE_COUNT - 1); i++)
    {
        // Set bit switches on IC 4051
        SERIAL_REG[PinSerial::IC4051_SC] = (i >> 2) & 0x1;
        SERIAL_REG[PinSerial::IC4051_SB] = (i >> 1) & 0x1;
        SERIAL_REG[PinSerial::IC4051_SA] = (i >> 0) & 0x1;
        serialApply();
        ANALOG_SENSOR[i] = soil_persentase(system_adc_read());
    }
}
void smartgarden_setup()
{
    pinMode(SERIAL_DATA, OUTPUT);
    pinMode(SERIAL_LOAD, OUTPUT);
    pinMode(SERIAL_CLOCK, OUTPUT);
    pinMode(SENSOR_SUHU_PIN, INPUT);
    // turn off all
    serialApply();
    // update initial sensors
    //sensorUpdate();
}

void dumpSerial(int start = 0, int end = 15)
{
    P(MAGENTA("--- dumpSerial %d - %d ---\n"), start, end);
    for (int i = start; i <= end; i++)
    {
        P(" > %11s : %s\n", PinSerialNames[i], ONOFF(SERIAL_REG[i] & B1));
    }
}

// no -1 mean turn off all
void valveOn(int8 no)
{
    // Turn off all
    for (int i = 0; i < VALVE_COUNT; i++)
    {
        SERIAL_REG[VALVE_START + i] = LOW;
    }

    // Check if turn on valve
    if (no >= 0 && no <= SPRAYER_NO)
    {
        SERIAL_REG[VALVE_START + no] = HIGH;
        VALVE_LAST_ON[no] = DETIK;
        // if pompa off, save state off high now
        if (getPompa() == LOW)
        {
            pompa_nyala_sejak = DETIK;
            pompa_mati_sampai = 0;
            timer_valve_delay_off = 0;
            setPompa(HIGH);
        }
        status("%s ON %d dtk*", valveName(no), config->valve_delay[no]);
    }
    else if (getPompa() == HIGH) // Turn off all
    {
        setPompa(LOW);
        pompa_nyala_sejak = 0;
        status(config->displayText);
    }
    if (VALVE_CURRENT >= 0 && VALVE_OFF_AFTER_DELAY != VALVE_CURRENT)
    {
        // Don't turn of now
        VALVE_OFF_AFTER_DELAY = VALVE_CURRENT;
        timer_valve_delay_off = DETIK;
        SERIAL_REG[VALVE_START + VALVE_OFF_AFTER_DELAY] = HIGH;
        P("%s OFF DELAYED\n", valveName(VALVE_OFF_AFTER_DELAY));
    }
    else
    {
        P("Turn off but no recent valve on!\n");
    }
    VALVE_CURRENT = no;
    serialApply();
    dumpSerial(PinSerial::Valve_0, PinSerial::Sprayer);
}

void dumpValveState()
{
    P(RED("VALVE STATE:\n"));
    for (int i = 0; i < VALVE_COUNT; i++)
    {
        P("%s\t%d\n",
          valveName(i),
          (VALVE_STATE >> i) & 1);
    }
}

// Check valve that need to turn on
int8_t findValveThatNeedWater()
{
    // reset state
    VALVE_STATE = 0;

    // All Analog sensors, except DHT22
    int8_t needWater = -1;
    for (int8_t i = 0; i < VALVE_COUNT - 1; i++)
    {
        // check if is not manual mode
        if (!isValveManual(i))
        {
            VALVE_STATE |= (ANALOG_SENSOR[i] <= config->humidity_minimal[i]) << i;
        }
    }
    if (!isValveManual(SPRAYER_NO))
    {
        if (TEMPERATURE >= config->temperature_max)
        {
            VALVE_STATE |= 1 << SPRAYER_NO;
        }
        else if (HUMIDITY < config->humidity_minimal[SPRAYER_NO])
        {
            VALVE_STATE |= 1 << SPRAYER_NO;
        }
    }
    if (VALVE_STATE > 0)
    {
        // Find that oldest start
        for (int8_t i = 0; i < VALVE_COUNT; i++)
        {
            if (VALVE_STATE & 1 << i)
            {
                if (needWater < 0 ||
                    // whois oldest
                    VALVE_LAST_ON[i] < VALVE_LAST_ON[needWater])
                {
                    needWater = i;
                }
            }
        }
        //dumpValveState();
    }

    return needWater;
}

void forceTempOff(const char *reason)
{
    //SERIAL_REG[VALVE_START + VALVE_CURRENT] = LOW;
    if (VALVE_CURRENT >= 0)
    {
        if (VALVE_OFF_AFTER_DELAY >= 0 && VALVE_CURRENT != VALVE_OFF_AFTER_DELAY)
        {
            SERIAL_REG[VALVE_START + VALVE_OFF_AFTER_DELAY] = LOW;
        }
        VALVE_OFF_AFTER_DELAY = VALVE_CURRENT;
        SERIAL_REG[VALVE_START + VALVE_OFF_AFTER_DELAY] = HIGH;
        timer_valve_delay_off = DETIK;
    }
    SERIAL_REG[PinSerial::Pompa] = LOW;
    serialApply();
    //P(RED("forceTempOff: %s\n"), reason);
}

void smartgarden_loop()
{
    int remain;
    // Check delayed valve off
    if (VALVE_OFF_AFTER_DELAY >= 0 && DETIK - timer_valve_delay_off >= DELAY_CLOSE_ALL_VALVE)
    {
        P("%s OFF\n", valveName(VALVE_OFF_AFTER_DELAY));
        SERIAL_REG[VALVE_START + VALVE_OFF_AFTER_DELAY] = LOW;
        serialApply();
        VALVE_OFF_AFTER_DELAY = -1;
        timer_valve_delay_off = 0;
        dumpSerial(PinSerial::Valve_0, PinSerial::Sprayer);
        return;
    }

    // Masih di detik yg sama, abaikan
    if (DETIK - LAST_LOOP <= 0)
    {
        return;
    }

    // Update last loop
    LAST_LOOP = DETIK;

    if ((DETIK % config->sensor_delay) == 0)
    {
        sensorUpdate();
    }

    if (pompa_mati_sampai > 0)
    {
        remain = pompa_mati_sampai - DETIK;
        if (remain > 0)
        {
            status("Pompa off %d detik", remain);
            return;
        }
        if (hasValveCurrent)
        {
            VALVE_LAST_ON[VALVE_CURRENT] += config->maksimal_pompa_mati;
        }

        pompa_nyala_sejak = 0;
        pompa_mati_sampai = 0;
    }

    // have valve that still on
    if (hasValveCurrent && getPompa() == HIGH)
    {
        // Berapa lama lagi pompa boleh nyala
        remain = pompa_nyala_sejak + config->maksimal_pompa_hidup - DETIK;
        if (remain < 0)
        {
            pompa_mati_sampai = DETIK + config->maksimal_pompa_mati;
            forceTempOff("POMPA AUTO OFF");
            status("Pompa off %d detik*", config->maksimal_pompa_mati);
            return;
        }
        remain = config->valve_delay[VALVE_CURRENT] - (DETIK - VALVE_LAST_ON[VALVE_CURRENT]);
        if (remain > 0)
        {
            status("%s ON %d detik", valveName(VALVE_CURRENT), remain);
            return;
        }
    }

    int8_t needWater = findValveThatNeedWater();
    if (needWater < 0 && VALVE_CURRENT < 0)
    {
        // Nothing todo
        return;
    }

    // same valve need water, check the gap period
    if (needWater == VALVE_CURRENT && getPompa() == HIGH)
    {
        int nextOn = VALVE_LAST_ON[VALVE_CURRENT] + config->valve_delay[VALVE_CURRENT] + config->valve_gap[VALVE_CURRENT];
        int gap_remain = nextOn - DETIK;
        if (gap_remain > 0)
        {
            if (getValve(VALVE_CURRENT) == HIGH)
            {
                pompa_mati_sampai = DETIK + config->valve_gap[VALVE_CURRENT];
                // still high, need to turn it off
                forceTempOff("VALVE_GAP");
            }
            status("%s delay %d dtk", valveName(VALVE_CURRENT), gap_remain);
            return;
        }
    }

    valveOn(needWater);
}
