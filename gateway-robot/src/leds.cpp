#include "leds.h"

#include <Arduino.h>

#include <TimeOut.h>

#include "mqtthandler.h"

/* NodeMCU to M54HC595 PIN Connections

M54HC595  -  NodeMCU

  11 SCK CLOCK (serial register clock)   - D1 GPIO5
  12 RCK LATCH (storage register clock)  - D2 GPIO4
  14 SI  DATA (serial in)                - D3 GPIO0

*/

#define SR_DATA_PIN 0  // Data pin
#define SR_CLOCK_PIN 5 // Clock pin
#define SR_LATCH_PIN 4 // Latch pin

uint8_t led_state = 0;

uint16_t led_blink_repetitions[NUM_LEDS];

Interval blink_interval;

boolean global_led_state = LED_ON;

void blinkLedsCallback()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (led_blink_repetitions[i] > 0)
        {
            toggleLed(i + 1);
            led_blink_repetitions[i]--;
        }
    }
}

// update the shift register with data and latch it to display on the LEDs
void updateShiftRegister(byte data)
{
    // ignore if global led state is off
    if (global_led_state == LED_OFF)
    {
        return;
    }

    digitalWrite(SR_LATCH_PIN, LOW);                     // Pull latch pin low to start sending data
    shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, data); // Send the data
    digitalWrite(SR_LATCH_PIN, HIGH);                    // Pull latch pin high to update the output
}

void initLeds()
{
    // Set shift register pins as outputs
    pinMode(SR_DATA_PIN, OUTPUT);
    pinMode(SR_CLOCK_PIN, OUTPUT);
    pinMode(SR_LATCH_PIN, OUTPUT);

    // Initialize the shift register with all LEDs off
    updateShiftRegister(led_state);
    // initialize the blink repetitions array
    for (int i = 0; i < NUM_LEDS; i++)
    {
        led_blink_repetitions[i] = 0;
    }
    blink_interval.interval(500, blinkLedsCallback);
}

void setGlobalLedState(boolean on)
{
    if (on)
    {
        global_led_state = LED_ON;
        updateShiftRegister(led_state);
    }
    else
    {
        updateShiftRegister(0);
        global_led_state = LED_OFF;
    }
}

boolean getGlobalLedState()
{
    return global_led_state;
}

void setLed(int led, boolean on)
{
    if (led < 1 || led > NUM_LEDS)
    {
        return;
    }

    if (on)
    {
        led_state |= 1 << led;
    }
    else
    {
        led_state &= ~(1 << led);
    }

    updateShiftRegister(led_state);
}

boolean getLed(int led)
{
    if (led < 1 || led > NUM_LEDS)
    {
        return false;
    }

    return (led_state >> led) & 1;
}

void setLeds(uint8_t state)
{
    led_state = state << 1;
    updateShiftRegister(led_state);
}

void toggleLed(int led)
{
    if (led < 1 || led > NUM_LEDS)
    {
        return;
    }

    led_state ^= 1 << led;
    updateShiftRegister(led_state);
}

void blinkLed(int led, int cycles)
{
    if (led < 1 || led > NUM_LEDS)
    {
        return;
    }

    int cycle_adjustment = led_blink_repetitions[led - 1] % 2;

    led_blink_repetitions[led - 1] = cycles * 2 + cycle_adjustment;
}
