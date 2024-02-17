#pragma once

#include <Arduino.h>

#define LED_ON true
#define LED_OFF false

#define USER_1_LED 1
#define USER_2_LED 2
#define USER_3_LED 3
#define MQTT_STATUS_LED 4
#define ESP_NOW_STATUS_LED 5

#define NUM_LEDS 5
#define ALL_LEDS_ON ((1 << NUM_LEDS) - 1)
#define STATUS_LEDS_ON 0b00011000

void initLeds();

/**
 * Set the state of a single LED. The LED is identified by a number from 1 to NUM_LEDS. 1-based because pin 0 of the shift register is not wired.
 */
void setLed(int led, boolean on);

boolean getLed(int led);

void setLeds(uint8_t state);

void toggleLed(int led);

void blinkLed(int led, int cycles);

void setGlobalLedState(boolean on);

boolean getGlobalLedState();
