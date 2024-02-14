#pragma once

#include <Arduino.h>

typedef struct DisplayState {
    char image_name[50];
    uint8_t backlight;
    uint16_t color;
} DisplayState;

void initDisplay();
void fillScreen(uint16_t color);
void drawEyes();
void drawImage(const char *image_name, int16_t x, int16_t y);
void drawText(const char* text, uint16_t color, int16_t x, int16_t y);
void setBacklight(uint8_t brightness);

extern DisplayState displayState;