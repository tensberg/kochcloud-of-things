#pragma once

#include <Arduino.h>
#include <Adafruit_ST7735.h>

enum Location {
    LEFT,
    RIGHT
};

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
void drawGradient(int16_t x);
void setBacklight(uint8_t brightness);

extern Adafruit_ST7735 tft;
extern int16_t gradientWidth;
extern DisplayState displayState;