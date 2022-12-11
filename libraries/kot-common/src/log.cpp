#include "Arduino.h"

#include "log.h"

#define ENABLE_LOGGING true

void log(const char* message) {
#ifdef ENABLE_LOGGING
    Serial.println(message);
#endif
}