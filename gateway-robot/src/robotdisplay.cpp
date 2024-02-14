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
Adafruit_Image eyes; // An image loaded into RAM

DisplayState displayState;

void setImageName(const char* image_name)
{
  strlcpy(displayState.image_name, image_name, sizeof(displayState.image_name));
}

void resetImageName() 
{
  displayState.image_name[0] = '\0';
}

void initDisplay()
{
  // initialize TFT display
  pinMode(TFT_LED, OUTPUT);
  setBacklight(0);           // turn off backlight
  tft.initR(INITR_BLACKTAB); // Initialize screen
  tft.setRotation(1);
  fillScreen(ST77XX_BLACK);

  // initialize SD card
  SD.begin(SD_CS, SD_SCK_MHZ(25));
  reader.loadBMP("/eyes.bmp", eyes);
  resetImageName();
}

void fillScreen(uint16_t color)
{
  tft.fillScreen(color);
  displayState.color = color;
  resetImageName();
}

void drawEyes()
{
  eyes.draw(tft, 0, 0);
  setImageName("eyes");
}

void drawImage(const char *image_name, int16_t x, int16_t y)
{
  if (strcmp(image_name, "eyes") == 0)
  {
    eyes.draw(tft, x, y);
    setImageName("eyes");
  }
  else 
  {
    char image_filename[strlen(image_name) + 5];
    sprintf(image_filename, "/%s.bmp", image_name);

    ImageReturnCode result = reader.drawBMP(image_filename, tft, x, y, true);
    if (result == IMAGE_SUCCESS)
    {
      setImageName(image_name);
    }
  }
}

void drawText(const char *text, uint16_t color, int16_t x, int16_t y)
{
  if (x >= 0 && y >= 0)
  {
    tft.setCursor(x, y);    
  }
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void setBacklight(uint8_t brightness)
{
  analogWrite(TFT_LED, brightness);
  displayState.backlight = brightness;
}
