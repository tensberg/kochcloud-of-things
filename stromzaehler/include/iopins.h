#pragma once

// Hichi IR ttl
#define RX_READER_HAUSHALT 4 // D2
#define RX_READER_WAERME 5 // D1

// SSD1306 display connected to I2C 
#define I2C_SDA 14 // D5
#define I2C_SCL 12 // D6

// LEDs
#define LED_STATUS_HAUSHALT 13 // D7
#define LED_STATUS_WAERME 15 // D8

// button
// the LED pins also serve as button pins because of limited I/Os on a D1 Mini
#define BUTTON_VCC 15 // D7
#define BUTTON_READ 13 // D8
