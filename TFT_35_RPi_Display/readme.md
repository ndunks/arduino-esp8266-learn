# NodeMCU/WeMos ESP8266 with RPi 3.5 TFT Display

RPi 3.5 Inch TFT Touch Display ILI9486/ILI9488 (KUMAN/WAVESHARE) 

**Config:**

``` CPP
#define TFT_CS   PIN_D2  // Chip select control pin D2
#define TFT_DC   PIN_D3  // Data Command control pin
#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)
#define TOUCH_CS PIN_D1     // Chip select pin (T_CS) of touch screen
```

## Wiring

| NodeMCU    | Display | LCD PIN Name |
|------------|---------|--------------|
| D1         | 26      | TP_CS        |
| D2         | 24      | LCD_CS       |
| D3         | 18      | LCD_RS       |
| D4         | 22      | RST          |
| D5 / HSCLK | 23      | LCD_SCK / TP_SCK LCD   |
| D6 / HMISO | 21      | TP_SO        |
| D7 / HMOSI | 19      | LCD_SI / TP_SI |
| G          | 6       | GND          |
| VCC/5V     | 2       | 5V           |


## Interface definition

| Pin number                      | Name           | Describe    |
|---------------------------------|----------------|-------------|
| 1, 17                           | 3.3V           | Power supply positive (3.3V power input) |
| 2, 4                            | 5V             | Power supply positive (5V power input) |
| 3, 5, 7, 8, 10, 12, 13, 15, 16  | NC             | NC |
| 6, 9, 14, 20, 25                | GND            | Power GND |
| 11                              | TP_IRQ         | Touch panel interrupt, detect the touch panel is pressed down to a low level |
| 18                              | LCD_RS         | Instruction / data register selection |
| 19                              | LCD_SI / TP_SI | LCD display / touch panel SPI data input |
| 21                              | TP_SO          | SPI data output of touch panel |
| 22                              | RST            | reset |
| 23                              | LCD_SCK / TP_SCK LCD  | SPI clock signal for display / touch panel |
| 24                              | LCD_CS	       | LCD chip select signal, low level LCD |
| 26                              | TP_CS	       | The touch panel chip select signal, low level selection of touch panel |

## TFT INFO

| Main                      | Parameter         |
|---------------------------|-------------------|
| Type                      | TFT               |
| Interface                 | SPI               |
| Touch panel control chip  | XPT2046           |
| The color index           | 65536             |
| Backlight                 | LED               |
| resolving power           | 320x480 (Pixel)   |
| Size proportion           | 8:5               |
| power waste               | TBD               |
| Back facet current        | TBD               |
| Working temperature (c)   | TBD               |
