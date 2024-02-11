#pragma once

#include <Arduino.h>

void initDisplay();
void drawImage();
void drawText(const char* text, uint16_t color);
void setBacklight(uint8_t brightness);
