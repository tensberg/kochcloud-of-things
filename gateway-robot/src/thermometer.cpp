#include "thermometer.h"

#include <Adafruit_ST7735.h>

#define NORMAL_MIN 15
#define NORMAL_MAX 35
#define EXTREME_MIN -20
#define EXTREME_MAX 50

int16_t previousTriangleY = 0;

int16_t temperatureLeft = 20;
int16_t temperatureRight = 20;

void drawThermometerBackground()
{
    drawGradient(0);
    drawGradient(tft.width() - gradientWidth);
}

void drawTriangle(Location location, int16_t y, uint16_t color)
{
    int16_t x = location == LEFT ? gradientWidth + 2 : tft.width() - gradientWidth - 2;
    int8_t direction = location == LEFT ? 1 : -1;
    tft.fillTriangle(x, y, x + 3 * direction, y - 3, x + 3 * direction, y + 3, color);
}

void drawTemperature(int16_t temperature, Location location)
{
    // write temperature as text
    char text[10];
    sprintf(text, "%d°C", temperature);
    int16_t x, y;
    uint16_t textWidth, textHeight;
    tft.getTextBounds(text, 0, 0, &x, &y, &textWidth, &textHeight);
    x = location == LEFT ? gradientWidth + 2 : tft.width() - gradientWidth - textWidth - 2;
    y = tft.height() - textHeight - 2;
    drawText(text, ST7735_BLACK, x, y);

    // draw temperature triangle
    int16_t triangleY;
    if (temperature >= NORMAL_MIN && temperature <= NORMAL_MAX) {
        triangleY = map(temperature, NORMAL_MIN, NORMAL_MAX, tft.height() - 1, 0);
    } else {
        triangleY = map(temperature, EXTREME_MIN, EXTREME_MAX, tft.height() - 1, 0);
    }

    drawTriangle(location, previousTriangleY, ST7735_WHITE);
    uint16_t color;
    if (location == LEFT) {
        temperatureLeft = temperature;
        color = ST7735_BLACK;
    } else {
        temperatureRight = temperature;
    }
    color = temperatureRight <= temperatureLeft ? ST7735_GREEN : ST7735_RED;
    drawTriangle(location, triangleY, color);
    previousTriangleY = triangleY;
}