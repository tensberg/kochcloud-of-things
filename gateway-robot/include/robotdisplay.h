#pragma once

#include <Arduino.h>
#include <Adafruit_ST7735.h>

enum Location {
    LEFT,
    RIGHT
};

typedef struct DisplayState {
    uint16_t background_color;
    char image[51];
    uint8_t brightness;
    uint16_t x;
    uint16_t y;
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