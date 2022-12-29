#include "Arduino.h"

#include "log.h"

#define ENABLE_LOGGING true

void log(const char *message)
{
#ifdef ENABLE_LOGGING
    Serial.println(message);
#endif
}

void logHex(unsigned char b, boolean println)
{
#ifdef ENABLE_LOGGING
    if (b < 0x10)
    {
        Serial.print('0');
    }
    Serial.print(b, HEX);
    if (println)
    {
        Serial.println();
    }
    else 
    {
        Serial.print(' ');
    }
#endif
}