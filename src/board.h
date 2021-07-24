#pragma once 

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_I2CDevice.h>

// Needed for LCD library
#ifdef IS_WIOTERMINAL
  #include <Adafruit_ZeroTimer.h>
  
  #include <Adafruit_GFX.h>
  #include <Adafruit_ILI9341.h>

  #ifndef (LCD_SPI)
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
  #ifndef LCD_BACKLIGHT_ON
    #define LCD_BACKLIGHT_ON HIGH
  #endif

  #define LCD_BUSSPEED 24000000

  #define BUTTON_A WIO_KEY_A
  #define BUTTON_B WIO_KEY_B
  #define BUTTON_C WIO_KEY_C
  
  // SAMD51 timer layouts for ZeroTimer
  #define TIMER3_OUT0 10
  #define TIMER3_OUT1 11
  #define TIMER4_OUT0 A4
  #define TIMER4_OUT1 A5
  #define TIMER5_OUT1 6

  #define LCD_ROTATION 3
  #define LCD_INVERTED false

  Adafruit_ILI9341 tft = Adafruit_ILI9341(&LCD_SPI, LCD_DC, LCD_SS_PIN, LCD_RESET);

  bool beginTFT()
  {
    tft.begin(LCD_BUSSPEED);
    tft.setRotation(LCD_ROTATION);
    tft.invertDisplay(LCD_INVERTED);
    digitalWrite(LCD_BACKLIGHT, LCD_BACKLIGHT_ON);
    return true;
  }

  bool beginBoard()
  {
    return true;
  }
#endif

#ifdef IS_M5STACKFIRE
  #ifdef USE_HOMESPAN
    #include <HomeSpan.h>
  #endif
  
  #include <Adafruit_GFX.h>
  #include <Adafruit_ILI9341.h>

  #define TFT_MISO MISO
  #define TFT_MOSI MOSI
  #define TFT_SCLK SCK
  #define TFT_CS   14  // Chip select control pin
  #define TFT_DC   27  // Data Command control pin
  #define TFT_RST  33  // Reset pin (could connect to Arduino RESET pin)
  #define TFT_BL   32  // LED back-light (required for M5Stack)
  
  #define LCD_SPI SPI
  #define LCD_MISO_PIN TFT_MISO
  #define LCD_MOSI_PIN TFT_MOSI
  #define LCD_SCK_PIN TFT_SCLK
  #define LCD_SS_PIN TFT_CS
  #define LCD_DC TFT_DC
  #define LCD_BACKLIGHT TFT_BL
  #define LCD_RESET TFT_RST
  #define LCD_BACKLIGHT_ON HIGH

  #define LCD_ROTATION 0
  #define LCD_INVERTED true
  #define LCD_BUSSPEED 40000000

  #define BUTTON_A (39u)
  #define BUTTON_B (38u)
  #define BUTTON_C (37u)

  #define MADCTL_ML  	0x10
  #define TFT_RGB_BGR	0x08
  
  static uint8_t TFTCOMMAND[1] = {(MADCTL_ML | TFT_RGB_BGR)};
  Adafruit_ILI9341 tft = Adafruit_ILI9341(&LCD_SPI, LCD_DC, LCD_SS_PIN, LCD_RESET);

  bool beginTFT()
  {
    tft.begin(LCD_BUSSPEED);
    tft.setRotation(LCD_ROTATION);
    tft.invertDisplay(LCD_INVERTED);
    // tft.sendCommand(0x36, TFTCOMMAND, 1);
    digitalWrite(LCD_BACKLIGHT, LCD_BACKLIGHT_ON);
    
    tft.fillRect(0, 0, 500, 500, 0xaaaa);
    return true;
  }

  bool beginBoard()
  {
    return true;
  }
#endif

