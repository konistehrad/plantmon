#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// Needed for LCD library
#ifndef LCD_SPI
  #define LCD_SPI SPI3
  #define LCD_MISO_PIN PIN_SPI3_MISO
  #define LCD_MOSI_PIN PIN_SPI3_MOSI
  #define LCD_SCK_PIN PIN_SPI3_SCK
  #define LCD_SS_PIN PIN_SPI3_SS
  #define LCD_DC (70ul)
  #define LCD_RESET (71Ul)
  #define LCD_BACKLIGHT (72Ul)
  #define LCD_XL (73ul)
  #define LCD_YU (74ul)
  #define LCD_XR (75ul)
  #define LCD_YD (76ul)
#endif


//#define HASSPI 1
//#define SPICOM   LCD_SPI
//#define TFT_CS   LCD_SS_PIN
//#define TFT_DC   LCD_DC
//#define TFT_RST  LCD_RESET  //Set TFT_RST to -1 if the display RESET is connected to RST or 3.3V
//#define TFT_BL   LCD_BACKLIGHT
//#define TFT_BACKLIGHT_ON HIGH
//#define TFT_BACKLINGT_V 2000

// SAMD51 timer layouts for ZeroTimer
#define TIMER3_OUT0 10
#define TIMER3_OUT1 11
#define TIMER4_OUT0 A4
#define TIMER4_OUT1 A5
#define TIMER5_OUT1 6
