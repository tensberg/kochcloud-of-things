#include "leds.h"

#include <Arduino.h>

/* NodeMCU to M54HC595 PIN Connections

M54HC595  -  NodeMCU

  11 SCK CLOCK (serial register clock)   - D1 GPIO5
  12 RCK LATCH (storage register clock)  - D2 GPIO4
  14 SI  DATA (serial in)                - D3 GPIO0

*/

#define SR_DATA_PIN 0  // Data pin
#define SR_CLOCK_PIN 5 // Clock pin
#define SR_LATCH_PIN 4 // Latch pin

uint8_t led_state;

// update the shift register with data and latch it to display on the LEDs
void updateShiftRegister(byte data)
{
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
    led_state = 0;
    updateShiftRegister(led_state);
}

void setLed(int led, bool on)
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
