#include "header.h"
#include <user_interface.h>
#include <EEPROM.h>

SmartGardenConfig *config = nullptr;
IPAddress local_IP(192, 168, 4, 1);

const uint8_t FLAG_IS_BOOTED = 0x1;

void dump_config()
{
    P("----Config DUMP----\n");
    P("Config Size %d ~ %d\n", sizeof(SmartGardenConfig), sizeof(config));
    P("Flag: %d\nWeb Password: %s\n", config->flag, config->password);
    P("maksimal_pompa_hidup: %lu\n", config->maksimal_pompa_hidup);
    P("maksimal_pompa_mati: %lu\n", config->maksimal_pompa_mati);
    P("valve_delay_default: %d\n", config->valve_delay_default);
    P("humidity_minimal_default: %d\n", config->humidity_minimal_default);
    P("sensor_delay: %d\n", config->sensor_delay);
    P("temperature_max: %d\n", config->temperature_max);
    P("displayText: %s\n", config->displayText);
    for (int no = 0; no < VALVE_COUNT; no++)
    {
        P("valve_delay[%d]: %d\n", no, config->valve_delay_default);
        P("humidity_minimal[%d]: %d\n", no, config->humidity_minimal_default);
    }
    P("---------------\n");
}

void config_default()
{
    // Default DHT sensor max heat
    config->temperature_max = (uint8_t) 32;
    config->valve_delay_default = (uint8_t) 5;
    config->humidity_minimal_default = (uint8_t) 50;
    config->sensor_delay = (uint8_t) 5;
    memcpy(config->displayText, "    BLKP KLAMPOK", 17);

#ifdef SMARTGARDEN_DEBUG
    config->maksimal_pompa_hidup = 7;
    config->maksimal_pompa_mati = 4;
#else
    smartgarden_config->maksimal_pompa_hidup = 60 * 60; // 1 jam
    smartgarden_config->maksimal_pompa_mati = 10 * 60;  // 10 menit
#endif
    for (int no = 0; no < VALVE_COUNT; no++)
    {
        config->valve_delay[no] = config->valve_delay_default;
        config->humidity_minimal[no] = config->humidity_minimal_default;
    }

    // spesial untuk humdity dari sensor DHT22
    config->humidity_minimal[PinSerial.Sprayer - PinSerial.Valve_0] = 30;
    if (config->password[0] == 0 || config->password[0] == 0xff)
    {
        // Default config
        P("Set default web password to \"admin\"\n");
        memcpy(config->password, "admin", 6);
        // make sure null terminated
        config->password[8] = 0;
    }
    config->flag = FLAG_IS_BOOTED;
}
void firstboot()
{
    // Load default config
    WiFi.persistent(true);
    config_default();
    String ssid("smart-");
    ssid += device_id;

    P("\n--- FIRST BOOT ---\n");
    P("Device ID: %s\n", device_id);
    WiFi.hostname(ssid.c_str());

    if (!WiFi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0)))
    {
        status("SoftAP IP Error");
        delay(1000);
    }
    if (!WiFi.softAP(ssid))
    {
        status("SoftAP SSID ERROR");
        delay(1000);
    }
    WiFi.enableSTA(false);
    WiFi.mode(WIFI_AP);
    WiFi.setAutoConnect(false);
    ssid.clear();
}
void cofig_reset()
{
    uint32 startSector = (0x405FB000 - 0x40200000) / SPI_FLASH_SEC_SIZE;
    uint32 sector = 0x0;
    status("RESET CONFIGRATION");
    delay(1000);
    //DELETE 5 Sector (EEPROM 1 sector, rfcal 1 sector, WIFI 3 Sectors)
    for (int i = 0; i < 5; i++)
    {
        sector = startSector + i;
        P("%08X ... ", sector);
        if (ESP.flashEraseSector(sector))
        {
            status("OK, %d", i + 1);
        }
        else
        {
            status("Fail, %d", i + 1);
        }
    }
    delay(1000);
    ESP.restart();
}

bool config_save()
{
    if (EEPROM.commit())
    {
        status("Config Saved");
        return true;
    }
    else
    {
        status("EEPROM ERROR");
        return false;
    }
}
void config_setup()
{

    // get chip id as device id, leave first byte, is zero
    uint32 num = system_get_chip_id();
    sprintf(device_id,
            "%02X%02X%02X",
            (byte)(num >> 16),
            (byte)(num >> 8),
            (byte)(num >> 0));

    // Setup EEPROM
    EEPROM.begin(sizeof(SmartGardenConfig));
    config = (SmartGardenConfig *)EEPROM.getDataPtr();
    // After erase flash, whole flash contain 0xff
    if (config->flag != FLAG_IS_BOOTED)
    {
        status("Firstbooting...");
        delay(1000);
        firstboot();
        config_save();
        delay(1000);
    }
    else
    {
        if (WiFi.getAutoConnect())
        {
            //wait connected
            status("Wifi Connecting...");
            int8 wifiStatus = WiFi.waitForConnectResult(15000);
            if (wifiStatus == WL_CONNECTED)
            {
                status("WiFi Connected");
                delay(1000);
            }
            else
            {
                status("WiFi Fail %d", wifiStatus);
                delay(2000);
            }
        }
    }
    dump_config();
}