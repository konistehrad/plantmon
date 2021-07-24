
#include <Arduino.h>
#include <ArduinoLog.h>
#include <Thread.h>
#include "board.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>

#include <SparkFunBME280.h>

#include "Testboard_ui.h"

Adafruit_LvGL_Glue glue;

#define CELSIUS_SCALE 0 //Default
#define FAHRENHEIT_SCALE 1
BME280 climateSensor;
bool climateSensorInitialized = false;

static BME280_SensorMeasurements climateMeasurements;
bool climateMeasurementsDirty;
Thread sensorThread = Thread();
Thread lvglThread = Thread();

static void lvTick()
{
  if(climateMeasurementsDirty) {
    String tempString = String((int)climateMeasurements.temperature) + String("°F");
    String humidString = String((int)climateMeasurements.humidity) + String('%');
    
    lv_label_set_text(TempValue, tempString.c_str());
    lv_label_set_text(HumidityValue, humidString.c_str());

    climateMeasurementsDirty = false;
  }

  lv_task_handler();
}

static void sensorTick()
{
  //Log.infoln("Sensor task starting...");
  if(climateSensor.isMeasuring())
  {
    sensorThread.setInterval(10);
  }
  else
  {
    climateSensor.readAllMeasurements(&climateMeasurements, FAHRENHEIT_SCALE);
    climateMeasurementsDirty = true;
    sensorThread.setInterval(500);
  }
}

#if defined(LV_USE_LOG) && LV_USE_LOG != 0
void my_log_cb(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char * dsc)
{
  if(level == LV_LOG_LEVEL_ERROR) Log.error(dsc);
  if(level == LV_LOG_LEVEL_WARN)  Log.warningln(dsc);
  if(level == LV_LOG_LEVEL_INFO)  Log.infoln(dsc);
  if(level == LV_LOG_LEVEL_TRACE) Log.traceln(dsc);
}
#endif

void setup() {
  Serial.begin(115200);
  
  // wait until serial attaches or 4s passes...
  while(!Serial && !Serial.available() && millis() < 2000);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  beginBoard();
  
  pinMode(LCD_BACKLIGHT, OUTPUT);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  Log.noticeln("GPIO initialization completed.");

  tft.begin(LCD_BUSSPEED);
  tft.setRotation(LCD_ROTATION);
  tft.invertDisplay(LCD_INVERTED);
#ifdef MADCTL_ML
  tft.sendCommand(0x36, TFTCOMMAND, 1);
#endif
  digitalWrite(LCD_BACKLIGHT, LCD_BACKLIGHT_ON);
  Log.noticeln("TFT initialization completed.");
  
  Wire.begin();
  Wire.setClock(1000000ul);
  if (!(climateSensorInitialized = climateSensor.beginI2C(Wire))) {
    Log.errorln("Could not start BME280 sensor!");
  } else {
    climateSensor.setTempOverSample(1);
    climateSensor.setHumidityOverSample(1);
    climateSensor.setPressureOverSample(0);
    Log.noticeln("BME280 initialization completed.");
  }

  // Initialize glue, passing in address of display
#if defined(LV_USE_LOG) && LV_USE_LOG != 0
  lv_log_register_print_cb(my_log_cb);
#endif
  LvGLStatus status = glue.begin(&tft);
  if(status != LVGL_OK) {
    Log.fatalln("LvGL initialization failed: %d", (int)status);
    for(;;);
  }

  BuildPages();
  lv_scr_load(Screen1);
  
  sensorThread.onRun(sensorTick);
  sensorThread.setInterval(500);

  lvglThread.onRun(lvTick);
  lvglThread.setInterval(5);
}

// must be exported, but unusued?
void loop() {
  if(climateSensorInitialized) {
    if(sensorThread.shouldRun()) sensorThread.run();
  }
  if(lvglThread.shouldRun()) lvglThread.run();
}
