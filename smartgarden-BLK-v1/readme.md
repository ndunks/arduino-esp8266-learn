# SMART GARDEN BLKP KLAMPOK - Banjarnegara


## PIN MAPPING - ESP8266

| WemMos/NodeMCU |  Connect to   |
|----------------|---------------|
| D7             | IC 595: DS    |
| D8             | IC 595: RCLK  |
| D5             | IC 595: SRCLK |
| D4             | IR Module: S  |
| D3             | DHT22: OUT    |
| D2/SDA         | I2C LCD: SDA  |
| D1/SCL         | I2C LCD: SCL  |
| VV/5V          | I2C LCD: VCC  |
| A0/ADC         | IC 4051: COMMON |



Data type size (bytes)
```
              bool      1
           boolean      1
              byte      1
              char      1
     unsigned char      1
           uint8_t      1
             short      2
          uint16_t      2
              word      2
               int      4
      unsigned int      4
            size_t      4
             float      4
              long      4
     unsigned long      4
          uint32_t      4
            double      8
         long long      8
unsigned long long      8
          uint64_t      8
```


## Board Info

[Flash Layout](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#flash-layout)

### FLASH LAYOUT

The NodeMCU v3 device features a 4 MB (32 Mb) flash memory organized in sectors of 4k each. The flash memory address starts at 0x40200000.

[EEPROM Addr](https://github.com/espressif/esptool/issues/335#issuecomment-409390009)
0x405fb000 - 0x40200000

LD Config:
https://github.com/esp8266/Arduino/blob/master/tools/sdk/ld/eagle.flash.4m2m.ld

```SPI_FLASH_SEC_SIZE = 4096 = 4KB```

** Flash Split for 4M chips **

| Name   | INFO             | Mem Offset | Flash Address | Sector|         Size         |
|--------|------------------|------------|---------------|-------|----------------------|
| sketch | Program (Code)   | 0x40200000 | 0x00000000    | 0x000 | (~1019KB) (1044464B) |
| empty  | OTA?             | 0x402FEFF0 | 0x000feff0    | 0x0fe | (~1028KB) (1052688B) |
| spiffs | Web File         | 0x40400000 | 0x00200000    | 0x200 | (~2024KB) (2072576B) |
| eeprom | User Data        | 0x405FB000 | 0x003fb000    | 0x3fb | (4KB)                |
| rfcal  | SDK              | 0x405FC000 | 0x003fc000    | 0x3fc | (4KB)                |
| wifi   | SDK Wifi Config  | 0x405FD000 | 0x003fd000    | 0x3fd | (12KB)               |

### Erasing flash
**EEPROM:**

```
esptool erase_region 0x003fb000 4096
```

To get Sector Address: ``` FLASH Addres / SECTOR_SIZE ```
** May usefull sometime**
```FREE_SPACE_AFTER_SKETCH=(ESP.getSketchSize() + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));```

### STACK DUMP 2 Code Line
```
## Store dumpfile in
# scratch/error-dump.txt

## xtensa
python tool/decoder.py -e .pio/build/development/firmware.elf scratch/error-dump2.txt

```

## WEB FRONTEND

### Project setup
```
yarn install
```

### Compiles and hot-reloads for development
```
yarn serve
```

### Compiles and minifies for production
```
yarn build
```

### Upload to Device SPIFS
```
yarn deploy
```
    If not work, use platformio menu on your IDE(Upload File System Image)