#include "Arduino.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "meterdisplay.h"
#include "iopins.h"
#include "log.h"

// display configuration
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3c
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// global variables
boolean displayAvailable;
char buffer[50];
char floatBuffer[20];

void initDisplay() {
  Wire.pins(I2C_SDA, I2C_SCL);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  displayAvailable = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  if (displayAvailable) {
    log("display initialized");

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(500);
    // Clear the buffer
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.display();
  } else {
    log("display not available");
  }
}

void updateDisplay(StromzaehlerMessage& message) {
  if (!displayAvailable) {
    return;
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  sprintf(buffer, "Iteration: %lu", message.status.iteration);

  dtostrf(message.dataHaushalt.powerSumWh, 10, 1, floatBuffer);
  sprintf(buffer, "H Sum: %s", floatBuffer);
  display.println(buffer);

  dtostrf(message.dataHaushalt.powerCurrentW, 10, 1, floatBuffer);
  sprintf(buffer, "H Cur: %s", floatBuffer);
  display.println(buffer);

  dtostrf(message.dataWaerme.powerSumWh, 10, 1, floatBuffer);
  sprintf(buffer, "W Sum: %s", floatBuffer);
  display.println(buffer);

  dtostrf(message.dataWaerme.powerCurrentW, 10, 1, floatBuffer);
  sprintf(buffer, "W Cur: %s", floatBuffer);
  display.println(buffer);

  display.display();
}