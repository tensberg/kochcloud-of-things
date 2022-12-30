#include "Arduino.h"

#include "display.h"
#include "features.h"
#include "log.h"

#ifdef ENABLE_DISPLAY

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3c
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#endif

void initDisplay()
{
#ifdef ENABLE_DISPLAY
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("SSD1306 allocation failed");
    }

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    log("enabling display");
    display.display();
    delay(500);
    // Clear the buffer
    display.clearDisplay();
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.cp437(true);                 // Use full 256 char 'Code Page 437' font
#endif
}

void displayData(ZisternensensorData &zsData)
{
#ifdef ENABLE_DISPLAY
    display.clearDisplay();
    display.setCursor(0, 0);

    display.print("Temperature = ");
    display.print(zsData.temperature, 1);
    display.println(" *C");
    display.print("Pressure = ");
    display.print(zsData.pressure, 0);
    display.println(" hPa");
    display.print("Humidity = ");
    display.print(zsData.humidity, 0);
    display.println(" %");
    display.print("Distance = ");
    display.print(zsData.distance);
    display.println(" cm");

    display.display();
#endif
}
