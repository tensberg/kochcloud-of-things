#include "thermometer.h"

#include <Adafruit_ST7735.h>

#define NORMAL_MIN 15
#define NORMAL_MAX 35
#define EXTREME_MIN -20
#define EXTREME_MAX 50

#define TRIANGLE_WIDTH 5
#define TRIANGLE_HEIGHT 4
#define FONT_HEIGHT 8

typedef struct Temperatures {
    float min;
    float max;
    float current;
    float previous;
    Trend trend;
} Temperatures;

Temperatures temperatures[2] = {
    {TEMPERATURE_UNDEFINED, TEMPERATURE_UNDEFINED, TEMPERATURE_UNDEFINED, TREND_STABLE},
    {TEMPERATURE_UNDEFINED, TEMPERATURE_UNDEFINED, TEMPERATURE_UNDEFINED, TREND_STABLE}
};

const char emptyTrend[] = "";
const char trendRising[] = "\x18";
const char trendFalling[] = "\x19";
const char trendStable[] = "";

typedef struct Area {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
} Area;

typedef struct DisplayArea {
    Area hand;
    Area text;
} DisplayArea;

DisplayArea displayAreas[2];

void initThermometer() {
    displayAreas[LEFT].hand.x = gradientWidth;
    displayAreas[LEFT].hand.y = 0;
    displayAreas[LEFT].hand.width = TRIANGLE_WIDTH;
    displayAreas[LEFT].hand.height = tft.height();

    displayAreas[LEFT].text.x = gradientWidth + TRIANGLE_WIDTH;
    displayAreas[LEFT].text.y = tft.height() - FONT_HEIGHT - 2;
    displayAreas[LEFT].text.width = tft.width() / 2 - gradientWidth - TRIANGLE_WIDTH;
    displayAreas[LEFT].text.height = FONT_HEIGHT + 1;

    displayAreas[RIGHT].hand.x = tft.width() - gradientWidth - TRIANGLE_WIDTH;
    displayAreas[RIGHT].hand.y = 0;
    displayAreas[RIGHT].hand.width = TRIANGLE_WIDTH;
    displayAreas[RIGHT].hand.height = tft.height();

    displayAreas[RIGHT].text.x = tft.width() / 2;
    displayAreas[RIGHT].text.y = tft.height() - FONT_HEIGHT - 2;
    displayAreas[RIGHT].text.width = tft.width() / 2 - gradientWidth - TRIANGLE_WIDTH;
    displayAreas[RIGHT].text.height = FONT_HEIGHT + 1;
}

void drawThermometer()
{
    drawGradient(0); 
    drawGradient(tft.width() - gradientWidth);

    drawTemperature(LEFT);
    drawTemperature(RIGHT);
}

long mapftol(float x, long in_min, long in_max, long out_min, long out_max) {
    const long dividend = out_max - out_min;
    const long divisor = in_max - in_min;
    const float delta = x - in_min;

    return (delta * dividend + (divisor / 2)) / divisor + out_min;
}

void setTemperature(Location location, float temperature, float minTemperature, float maxTemperature, Trend trend)
{
    Temperatures *selected = &temperatures[location];
    selected->current = temperature;
    if (minTemperature != TEMPERATURE_UNDEFINED) {
        selected->min = minTemperature;
    } else if (temperature < selected->min || selected->min == TEMPERATURE_UNDEFINED) {
        selected->min = temperature;
    }
    if (maxTemperature != TEMPERATURE_UNDEFINED) {
        selected->max = maxTemperature;
    } else if (temperature > selected->max || selected->max == TEMPERATURE_UNDEFINED) {
        selected->max = temperature;
    }
    selected->trend = trend;
    drawTemperature(location);
}

void fillArea(Area area, uint16_t color)
{
    tft.fillRect(area.x, area.y, area.width, area.height, color);
}

void drawTemperatureText(Location location, float temperature, const char* trend, uint16_t color)
{
    const char* trendLeft = location == LEFT ? trend : emptyTrend;
    const char* trendRight = location == RIGHT ? trend : emptyTrend;
    char text[10];
    sprintf(text, "%s%.1f""\xf8""C%s", trendRight, temperature, trendLeft); // the trend of the right thermometer is displayed on the left side of the text
    int16_t x, y;
    uint16_t textWidth, textHeight;
    tft.getTextBounds(text, 0, 0, &x, &y, &textWidth, &textHeight);
    x = location == LEFT ? gradientWidth + TRIANGLE_WIDTH : tft.width() - gradientWidth - textWidth - TRIANGLE_WIDTH - 1;
    y = tft.height() - textHeight - 1;
    drawText(text, color, x, y);
}

void drawTemperatureHands(Location location, Temperatures *selected, uint16_t color)
{
    float temperature = selected->current;

    int16_t currentY, minY, maxY;
    int16_t inMinY, inMaxY;
    int16_t outMinY = tft.height() - 1;
    int16_t outMaxY = 0;
    if (temperature >= NORMAL_MIN && temperature <= NORMAL_MAX) {
        inMinY = NORMAL_MIN;
        inMaxY = NORMAL_MAX;
    } else {
        inMinY = EXTREME_MIN;
        inMaxY = EXTREME_MAX;
    }
    currentY = mapftol(temperature, inMinY, inMaxY, outMinY, outMaxY);
    if (selected->min == TEMPERATURE_UNDEFINED || selected->max == TEMPERATURE_UNDEFINED) {
        minY = maxY = currentY;
    } else {
        minY = mapftol(selected->min, inMinY, inMaxY, outMinY, outMaxY);
        maxY = mapftol(selected->max, inMinY, inMaxY, outMinY, outMaxY);
    }

    int16_t x = location == LEFT ? gradientWidth : tft.width() - gradientWidth - 1;
    int8_t direction = location == LEFT ? 1 : -1;
    int16_t x1 = x + TRIANGLE_WIDTH * direction - direction;
    tft.fillRect(x, minY, TRIANGLE_WIDTH * direction, 2, ST7735_BLUE);
    tft.fillRect(x, maxY - 1, TRIANGLE_WIDTH * direction, 2, ST7735_RED);
    tft.fillTriangle(x, currentY, x1, currentY - TRIANGLE_HEIGHT, x1, currentY + TRIANGLE_HEIGHT, color);
}

void drawTemperature(Location location)
{
    Temperatures *selected = &temperatures[location];
    float temperature = selected->current;

    // clear temperature area
    fillArea(displayAreas[location].hand, ST7735_WHITE);
    fillArea(displayAreas[location].text, ST7735_WHITE);

    if (temperature == TEMPERATURE_UNDEFINED) {
        return;
    }

    uint16_t color;
    if (location == LEFT) {
        color = ST7735_BLACK;
    } else {
        color = temperature < 26 || temperatures[LEFT].current >= temperature ? ST7735_BLACK : ST7735_RED;
    }

    const char* trendChar;
    
    if (selected -> trend == TREND_FALLING) {
        trendChar = trendFalling;
    } else if (selected -> trend == TREND_RISING) {
        trendChar = trendRising;
    } else {
        trendChar = trendStable;
    }

    drawTemperatureText(location, temperature, trendChar, color);
    drawTemperatureHands(location, selected, color);
}