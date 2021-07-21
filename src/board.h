#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// Needed for LCD library
#ifdef IS_WIOTERMINAL
  #include <Seeed_Arduino_FreeRTOS.h>
  #include <Adafruit_ZeroTimer.h>

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
  
  // SAMD51 timer layouts for ZeroTimer
  #define TIMER3_OUT0 10
  #define TIMER3_OUT1 11
  #define TIMER4_OUT0 A4
  #define TIMER4_OUT1 A5
  #define TIMER5_OUT1 6
#endif

#ifdef IS_M5STACKFIRE
  #include <freertos/FreeRTOS.h>

  #define TFT_MISO 19
  #define TFT_MOSI 23
  #define TFT_SCLK 18
  #define TFT_CS   14  // Chip select control pin
  #define TFT_DC   27  // Data Command control pin
  #define TFT_RST  33  // Reset pin (could connect to Arduino RESET pin)
  #define TFT_BL   32  // LED back-light (required for M5Stack)
  #define SPI_FREQUENCY  40000000 
  
  #define LCD_SPI SPI
  #define LCD_MISO_PIN TFT_MISO
  #define LCD_MOSI_PIN TFT_MOSI
  #define LCD_SCK_PIN TFT_SCLK
  #define LCD_SS_PIN TFT_CS
  #define LCD_DC TFT_DC
  #define LCD_BACKLIGHT TFT_BL
  #define LCD_RESET TFT_RST
  #define LCD_BACKLIGHT_ON HIGH
#endif

