
/**
 * 8 Input Analog/Digital
 * 16 Output digital 
 * 
 * OUT IC 595 (2 chain) Serial output mapping:
 * 0 - 2    To control input multiplexer IC 4051 (SA - SC)
 * 3        Pompa DC
 * 4 - 6    Unused
 * 8 - 14   Control Solenoid Valve/Led (see: OUTSTART)
 * 15       Solenoid pengembunan
 * 
 * IN IC 4051 Analog/Digital mapping:
 * 0 - 5    Sensor tanah (analog)
 * 6        DHT22 (digital)
 */

#define dataPin 13  //D7 --> DS Outputs the byte to transfer
#define loadPin 15  //D8 --> RCLK Controls the internal transference of data in SN74HC595 internal registers
#define clockPin 14 //D5 --> SRCLK Generates the clock signal to control the transference of data
// OUTUT/LED PIN, 
#define OUTSTART 8

// IC595 -> IC4051
#define SA 0
#define SB 1
#define SC 2

#ifdef LOG_ON
#define LOG(...) printf(##__VA_ARGS__)
#endif

#ifndef LOG
#define LOG(...) \
    do           \
    {            \
        (void)0; \
    } while (0)
#endif

// Delay for checking input sensors, call set it before smartgarden_setup()
extern uint8 REG[];
extern unsigned long smartgarden_delay;

// Current analogs signal state
extern uint8 ANALOG_SENSOR[];
extern float HUMIDITY;
extern float TEMPERATURE;
// Setup pin and start ticker
void smartgarden_setup();

// Apply REG to Serial Pararel Chained IC 595
void smartgarden_apply();

