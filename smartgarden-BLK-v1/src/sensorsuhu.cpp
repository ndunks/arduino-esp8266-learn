#include "header.h"

DHT_MODEL_t sensorsuhu_model = DHT22;
DHT_ERROR_t sensorsuhu_error = ERROR_NONE;
unsigned long lastReadTime = millis() - 3000;

void sensorsuhu_read()
{
    // Make sure we don't poll the sensor too often
    // - Max sample rate DHT11 is 1 Hz   (duty cicle 1000 ms)
    // - Max sample rate DHT22 is 0.5 Hz (duty cicle 2000 ms)
    unsigned long startTime = millis();
    if ((unsigned long)(startTime - lastReadTime) < (sensorsuhu_model == DHT11 ? 999L : 1999L))
    {
        return;
    }
    lastReadTime = startTime;
    TEMPERATURE = 0;
    HUMIDITY = 0;

    uint16_t rawHumidity = 0;
    uint16_t rawTemperature = 0;
    uint16_t data = 0;

    // Request sample
    digitalWrite(SENSOR_SUHU_PIN, LOW); // Send start signal
    pinMode(SENSOR_SUHU_PIN, OUTPUT);
    if (sensorsuhu_model == DHT11)
    {
        delay(18);
    }
    else
    {
        // This will fail for a DHT11 - that's how we can detect such a device
        delay(2);
    }

    pinMode(SENSOR_SUHU_PIN, INPUT);
    digitalWrite(SENSOR_SUHU_PIN, HIGH); // Switch bus to receive data

    // We're going to read 83 edges:
    // - First a FALLING, RISING, and FALLING edge for the start bit
    // - Then 40 bits: RISING and then a FALLING edge per bit
    // To keep our code simple, we accept any HIGH or LOW reading if it's max 85 usecs long

#ifdef ESP32
    // ESP32 is a multi core / multi processing chip
    // It is necessary to disable task switches during the readings
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
#else
    //   cli();
    noInterrupts();
#endif
    for (int8_t i = -3; i < 2 * 40; i++)
    {
        byte age;
        startTime = micros();

        do
        {
            age = (unsigned long)(micros() - startTime);
            if (age > 90)
            {
                sensorsuhu_error = ERROR_TIMEOUT;
#ifdef ESP32
                portEXIT_CRITICAL(&mux);
#else
                // sei();
                interrupts();
#endif
                return;
            }
        } while (digitalRead(SENSOR_SUHU_PIN) == (i & 1) ? HIGH : LOW);

        if (i >= 0 && (i & 1))
        {
            // Now we are being fed our 40 bits
            data <<= 1;

            // A zero max 30 usecs, a one at least 68 usecs.
            if (age > 30)
            {
                data |= 1; // we got a one
            }
        }

        switch (i)
        {
        case 31:
            rawHumidity = data;
            break;
        case 63:
            rawTemperature = data;
            data = 0;
            break;
        }
    }

#ifdef ESP32
    portEXIT_CRITICAL(&mux);
#else
    //   sei();
    interrupts();
#endif

    // Verify checksum

    if ((byte)(((byte)rawHumidity) + (rawHumidity >> 8) + ((byte)rawTemperature) + (rawTemperature >> 8)) != data)
    {
        sensorsuhu_error = ERROR_CHECKSUM;
        return;
    }

    // Store readings

    if (sensorsuhu_model == DHT11)
    {
        HUMIDITY = byte((rawHumidity >> 8) + ((rawHumidity & 0x00FF) * 0.1));
        TEMPERATURE = byte((rawTemperature >> 8) + ((rawTemperature & 0x00FF) * 0.1));
    }
    else
    {
        HUMIDITY = byte(rawHumidity * 0.1);

        if (rawTemperature & 0x8000)
        {
            rawTemperature = -(int16_t)(rawTemperature & 0x7FFF);
        }
        TEMPERATURE = byte(((int16_t)rawTemperature) * 0.1);
    }

    sensorsuhu_error = ERROR_NONE;
}