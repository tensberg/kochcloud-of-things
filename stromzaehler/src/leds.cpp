#include "Arduino.h"

#include "iopins.h"
#include "leds.h"

void initLeds() {
  pinMode(LED_STATUS_HAUSHALT, OUTPUT);
  digitalWrite(LED_STATUS_HAUSHALT, LOW);
  pinMode(LED_STATUS_WAERME, OUTPUT);
  digitalWrite(LED_STATUS_WAERME, LOW);
}

void blinkLedHaushalt(unsigned int delayMs) {
    ledSignal(true, false, 1, delayMs, 0);
}

void blinkLedWaerme(unsigned int delayMs) {
    ledSignal(false, true, 1, delayMs, 0);
}

void ledSignal(bool ledHaushaltHigh, bool ledWaermeHigh, unsigned int loops, unsigned int delayOnMs, unsigned int delayOffMs) {
  for (int i=0; i<3; i++) {
    digitalWrite(LED_STATUS_HAUSHALT, ledHaushaltHigh ? HIGH : LOW);
    digitalWrite(LED_STATUS_WAERME, ledWaermeHigh ? HIGH : LOW);
    delay(delayOnMs);
    digitalWrite(LED_STATUS_HAUSHALT, ledHaushaltHigh ? LOW : HIGH);
    digitalWrite(LED_STATUS_WAERME, ledWaermeHigh ? LOW : HIGH);
    delay(delayOffMs);
  }

  digitalWrite(LED_STATUS_HAUSHALT, LOW);
  digitalWrite(LED_STATUS_WAERME, LOW);
}