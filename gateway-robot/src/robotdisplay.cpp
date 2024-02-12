#include "robotdisplay.h"

#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ST7735.h>      // Hardware-specific library
#include <Adafruit_ImageReader.h> // Image-reading functions
#include <SdFat.h>                // SD card & FAT filesystem library

/* NodeMCU to ST7735 PIN connections

Display  -  NodeMCU
  LED    -  SD3 GPIO10
  SCK    -  D5 HSCLK GPIO14
  SDA    -  D7 HMOSI GPIO13
  A0     -  D4 TXD1  GPIO2
  RESET  - RST
  CS     -  D8 HCS   GPIO15
  GND    -  GND
  VCC    -  VIN
SD Card    -  NodeMCU
  SD_SCK   -  D5  HSCLK GPIO14
  SD_MISO  -  D6  HMISO
  SD_MOSI  -  D7  HMOSI GPIO13
  SD_CS    -  D0  GPIO16 WAKE

*/

#define TFT_LED 10 // SD3
#define SD_CS 16   // SD card select pin
#define TFT_CS 15  // TFT select pin
#define TFT_DC 2   // TFT display/command pin
#define TFT_RST -1 // not wired, unused

SdFat SD;                        // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Adafruit_Image img; // An image loaded into RAM

void initDisplay()
{
  // initialize TFT display
  pinMode(TFT_LED, OUTPUT);
  analogWrite(TFT_LED, 0);   // turn off backlight
  tft.initR(INITR_BLACKTAB); // Initialize screen
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  // initialize SD card
  SD.begin(SD_CS, SD_SCK_MHZ(10));
  reader.loadBMP("/eyes.bmp", img);
}

void drawImage()
{
  img.draw(tft, 0, 0);
}

void drawText(const char *text, uint16_t color)
{
  tft.setCursor(0, tft.height() - 8);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void setBacklight(uint8_t brightness)
{
  analogWrite(TFT_LED, brightness);
}