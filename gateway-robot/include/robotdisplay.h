#pragma once

#include <Arduino.h>

void initDisplay();
void fillScreen(uint16_t color);
void drawImage();
void drawText(const char* text, uint16_t color, int16_t x, int16_t y);
void setBacklight(uint8_t brightness);
