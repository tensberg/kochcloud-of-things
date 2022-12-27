#pragma once

// Hichi IR ttl
#define VCC_READER_HAUSHALT 12 // D6
#define RX_READER_HAUSHALT 14 // D5
#define VCC_READER_WAERME 5 // D1
#define RX_READER_WAERME 4 // D2

// LEDs
#define LED_STATUS_HAUSHALT 15 // D8
#define LED_STATUS_WAERME 13 // D7

// button
// the LED pins also serve as button pins because of limited I/Os on a D1 Mini
#define BUTTON_VCC 15 // D8
#define BUTTON_READ 13 // D7
