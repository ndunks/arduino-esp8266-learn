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