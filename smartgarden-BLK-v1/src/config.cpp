#include "header.h"
#include <user_interface.h>
#include <EEPROM.h>
#include "display.h"
#include "ir_remote.h"

SmartGardenConfig *config = nullptr;
IPAddress local_IP(192, 168, 4, 1);

const uint8_t FLAG_IS_BOOTED = 0x1;

void dump_config()
{
    P("----Config DUMP----\n");
    P("Config Size %d ~ %d\n", sizeof(SmartGardenConfig), sizeof(config));
    P("name: %s\n", config->name);
    P("Flag: %d\nWeb Password: %s\n", config->flag, config->password);
    P("maksimal_pompa_hidup: %u\n", config->maksimal_pompa_hidup);
    P("maksimal_pompa_mati: %u\n", config->maksimal_pompa_mati);
    P("sensor_delay: %d\n", config->sensor_delay);
    P("temperature_max: %d\n", config->temperature_max);
    P("displayText: %s\n", config->displayText);
    for (int no = 0; no < VALVE_COUNT; no++)
    {
        P("valve_delay[%d]: %d\n", no, config->valve_delay[no]);
    }
    for (int no = 0; no < VALVE_COUNT; no++)
    {
        P("valve_gap[%d]: %d\n", no, config->valve_gap[no]);
    }
    for (int no = 0; no < VALVE_COUNT; no++)
    {
        P("humidity_minimal[%d]: %d\n", no, config->humidity_minimal[no]);
    }
    for (int no = 0; no < VALVE_COUNT; no++)
    {
        P("valve_manual[%d]: %d\n", no, (config->valve_manual >> no) & 0x1);
    }
    P("Hostname: %s\nSSID: %s\nConnected: %d\n", WiFi.hostname().c_str(), WiFi.SSID().c_str(), WiFi.isConnected());
    P("MacAddr: %s\n", WiFi.macAddress().c_str());
    P("Reboot Reason: %s\n", ESP.getResetReason().c_str());
    P("---------------\n");
}

void config_default()
{
    // get chip id as device id, leave first byte, is zero
    char device_id[7] = {};
    String name("smart");
    uint32 num = system_get_chip_id();

    sprintf(device_id,
            "%02X%02X%02X",
            (byte)(num >> 16),
            (byte)(num >> 8),
            (byte)(num >> 0));
    name += device_id;

    // Nulled all bytes in config
    for (unsigned long i = 0; i < sizeof(SmartGardenConfig); i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    // Device name
    memcpy(config->name, name.c_str(), name.length());

    // Default DHT sensor max heat
    config->temperature_max = 27;
    config->sensor_delay = 5;
    // All valves manual
    config->valve_manual = 0;
    memcpy(config->displayText, "    BLKP KLAMPOK", 17);

#ifdef SMARTGARDEN_DEBUG
    config->maksimal_pompa_hidup = 30;
    config->maksimal_pompa_mati = 10;
#else
    smartgarden_config->maksimal_pompa_hidup = 60 * 30; // 30 menit
    smartgarden_config->maksimal_pompa_mati = 60;       // 1 menit
#endif
    for (int no = 0; no < VALVE_COUNT; no++)
    {
        config->valve_delay[no] = 5U;
        config->valve_gap[no] = 5U;
        config->humidity_minimal[no] = 70U;
    }

    // spesial untuk humdity dari sensor DHT22
    config->humidity_minimal[PinSerial::Sprayer - PinSerial::Valve_0] = 50;
    if (config->password[0] == 0 || config->password[0] == 0xff)
    {
        // Default config
        memcpy(config->password, "admin", 6);
        P("Set default web password to \"%s\"\n", config->password);
        // make sure null terminated
        //config->password[8] = 0;
    }
    config->flag = FLAG_IS_BOOTED;
}
void firstboot()
{
    // Load default config
    P("\n--- FIRST BOOT ---\n");
    config_default();
    if (!WiFi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0)))
    {
        status("SoftAP IP Error");
        delay(1000);
    }
    if (!WiFi.softAP(config->name))
    {
        status("SoftAP SSID ERROR");
        delay(1000);
    }
    WiFi.enableSTA(false);
    // Save config
    config_save();
    P("checkFlashConfig: %d\n", ESP.checkFlashConfig(true));
    delay(1000);
}

void config_reset()
{
    size_t EEPROM_SECTOR = (0x405FB000 - 0x40200000) / SPI_FLASH_SEC_SIZE;
    status("MENGHAPUS DATA...");
    delay(1000);
    if (ESP.flashEraseSector(EEPROM_SECTOR))
    {
        status("OK");
    }
    else
    {
        status("Fail");
    }
    delay(1500);
    status("RESTARTING...");
    delay(1500);
    ESP.eraseConfig();
    ESP.restart();
}

bool config_save()
{
    // Make config is dirty, so it will comitted (saved to flash chip)
    EEPROM.getDataPtr();

    if (EEPROM.commit())
    {
        status("Config Updated");
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
    // Setup EEPROM
    EEPROM.begin(sizeof(SmartGardenConfig));
    config = (SmartGardenConfig *)EEPROM.getDataPtr();

    if (config->flag != FLAG_IS_BOOTED)
    {
        status("Firstbooting...");
        delay(1000);
        firstboot();
    }
    if (WiFi.SSID().length() > 0)
    {
        status("%s...", WiFi.SSID().c_str());
        P("\nConnecting to %s\n", WiFi.SSID().c_str());
        if (WiFi.psk().length() == 0)
        {
            WiFi.enableInsecureWEP(true);
        }
        WiFi.begin();
        int8 wifiStatus = WiFi.waitForConnectResult(15000);
        if (wifiStatus == WL_CONNECTED)
        {
            WiFi.hostname(config->name);
            String str("Connected to ");
            str += WiFi.SSID();
            status(str.c_str());
            P("OK, IP %s\n", WiFi.localIP().toString().c_str());
            str.clear();
        }
        else
        {
            WiFi.enableSTA(false);
            // WiFi.enableAP(true);
            if (wifiStatus == -1)
            {
                status("Gagal, Timeout");
            }
            else
            {
                status("Gagal Konek WiFi");
            }
            P("WiFi Fail %d\n", wifiStatus);
            delay(1000);
        }
    }

    dump_config();
    status(config->displayText);
}
