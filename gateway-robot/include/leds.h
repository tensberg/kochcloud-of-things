#pragma once

#define NUM_LEDS 5

void initLeds();

/**
 * Set the state of a single LED. The LED is identified by a number from 1 to NUM_LEDS. 1-based because pin 0 of the shift register is not wired.
 */
void setLed(int led, bool on);
