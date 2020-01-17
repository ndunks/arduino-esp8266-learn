#include "IRremoteESP8266.h"
#include "IRrecv.h"
#include "IRutils.h"
#include "constant.h"

struct CodeMap
{
    const char *name;
    uint16 code;
};
