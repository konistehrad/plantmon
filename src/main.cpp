
#include "board.h"
#include <Seeed_Arduino_FreeRTOS.h>

#include <ArduinoLog.h>

#include <Adafruit_ZeroTimer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>

#include <SparkFunBME280.h>

#include "Testboard_ui.h"

const UBaseType_t tskPRIORITY_NORMAL = tskIDLE_PRIORITY + 4;

Adafruit_ILI9341 tft = Adafruit_ILI9341(&LCD_SPI, LCD_DC, LCD_SS_PIN, LCD_RESET);
Adafruit_LvGL_Glue glue;

#define CELSIUS_SCALE 0 //Default
#define FAHRENHEIT_SCALE 1
BME280 climateSensor;
bool climateSensorInitialized = false;

QueueHandle_t Handle_climateQueue;
TaskHandle_t Handle_lvglTask;
TaskHandle_t Handle_sensorTask;

static void lvTick(void * pvParameters)
{
  static BME280_SensorMeasurements climateMeasurements;
  Log.infoln("LvGL task starting...");
  for(;;) {
    if(xQueueReceive(Handle_climateQueue, &climateMeasurements, 0)) {
      int iTemp = int(climateMeasurements.temperature);
      int iHumidity = int(climateMeasurements.humidity);
      String tempString = String::format("%d°F", iTemp);
      String humidString = String(iHumidity) + String('%');
      
      lv_label_set_text(TempValue, tempString.c_str());
      lv_label_set_text(HumidityValue, humidString.c_str());
    }

    lv_task_handler();
    delay(5);
  }
  Log.errorln("LvGL task completed? This shouldn't happen!");
}

static void sensorTick(void * pvParameters)
{ 
  static BME280_SensorMeasurements climateMeasurements;
  Log.infoln("Sensor task starting...");
  for(;;) {
    while(climateSensor.isMeasuring()) taskYIELD();
    climateSensor.readAllMeasurements(&climateMeasurements, FAHRENHEIT_SCALE); // Return temperature in Celsius
    xQueueOverwrite(Handle_climateQueue, &climateMeasurements);

    delay(50);
  }
  Log.errorln("Sensor task completed? This shouldn't happen!");
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
  xTaskCreate(lvTick, "lvgl task", 1024, NULL, tskPRIORITY_NORMAL, &Handle_lvglTask);
  Log.noticeln("LvGL initialization completed.");

  Wire.begin();
  Wire.setClock(1000000ul);
  if (!(climateSensorInitialized = climateSensor.beginI2C(Wire))) {
    Log.errorln("Could not start BME280 sensor!");
  } else {
    climateSensor.setTempOverSample(1);
    climateSensor.setHumidityOverSample(1);
    climateSensor.setPressureOverSample(0);
    xTaskCreate(sensorTick, "sensor task", 256, NULL, tskPRIORITY_NORMAL, &Handle_sensorTask);
    Log.noticeln("BME280 initialization completed.");
  }

  Handle_climateQueue = xQueueCreate(1, sizeof(BME280_SensorMeasurements));
  Log.infoln("Tasks and queue created; starting scheduler!");
  // never returns!
  vTaskStartScheduler();
  Log.fatalln("Scheduler failed to start; you shouldn't be here!");
  while(1){};
}

// this is where you would pump coroutines, if you had 'em
void vApplicationIdleHook(void){}

// must be exported, but unusued
void loop() {}
