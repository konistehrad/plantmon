
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <Thread.h>
#include <StaticThreadController.h>
#include <ArduinoLog.h>

#include <Adafruit_ZeroTimer.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_I2CDevice.h>

#include "lv_conf.h" // use our override conf...
#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>

#include <SparkFunBME280.h>

#include "Testboard_ui.h"

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
#define TIMER5_OUT0 5
#define TIMER5_OUT1 6

Adafruit_ILI9341 tft = Adafruit_ILI9341(&LCD_SPI, LCD_DC, LCD_SS_PIN, LCD_RESET);
Adafruit_LvGL_Glue glue;

#define CELSIUS_SCALE 0 //Default
#define FAHRENHEIT_SCALE 1
BME280 climateSensor;
BME280_SensorMeasurements climateMeasurements;
bool climateSensorInitialized = false;

static void lvTick()
{
  lv_task_handler();
}

static void sensorTick()
{
  if(!climateSensorInitialized || climateSensor.isMeasuring()) return;
  climateSensor.readAllMeasurements(&climateMeasurements, FAHRENHEIT_SCALE); // Return temperature in Celsius
  
  int iTemp = int(climateMeasurements.temperature);
  int iHumidity = int(climateMeasurements.humidity);
  // String str = String::format("Temperature: %d F; Humidity: %d", iTemp, iHumidity);
  String tempString = String::format("%d°F", iTemp);
  lv_label_set_text(TempValue, tempString.c_str());
  String humidString = String(iHumidity) + String('%');
  lv_label_set_text(HumidityValue, humidString.c_str());
  // Log.infoln(str.c_str());
  // lv_label_set_text(label, str.c_str());
  // lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

void setup() {
  Serial.begin(115200);
  
  // wait until serial attaches or 4s passes...
  while(!Serial && !Serial.available() && millis() < 4000);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  pinMode(LCD_BACKLIGHT, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  Log.noticeln("GPIO initialization completed.");

  tft.begin();
  tft.setRotation(3);
  digitalWrite(LCD_BACKLIGHT, HIGH);
  Log.noticeln("TFT initialization completed.");
  
  // Initialize glue, passing in address of display
  LvGLStatus status = glue.begin(&tft);
  if(status != LVGL_OK) {
    Log.fatalln("LvGL initialization failed: %d", (int)status);
    for(;;);
  }

  BuildPages();
  lv_scr_load(Screen1);
  Log.noticeln("LvGL initialization completed.");

  if (!(climateSensorInitialized = climateSensor.beginI2C())) {
    Log.fatalln("Could not start BME280 sensor!");
  } else {
    climateSensor.setTempOverSample(1);
    climateSensor.setHumidityOverSample(1);
    climateSensor.setPressureOverSample(0);
    Log.noticeln("BME280 initialization completed.");
  }
}

Thread lvglThread(lvTick, 5);
Thread sensorThread(sensorTick, 50);
StaticThreadController<2> threads(&lvglThread, &sensorThread);

void loop() {
  threads.run();
}