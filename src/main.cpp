#include <Arduino.h>
#include <esp32-hal.h>
#include <Ticker.h>
#include <Wire.h>
#include <SPI.h>

#include <Thread.h>
#include <ArduinoLog.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <SparkFunBME280.h>
#include "IP5306.h"
#include "Testboard_ui.h"
#include "Model.hpp"

#define M5_NEO_NUM 10
#define M5_NEO_PIN 15

#ifdef USE_HOMESPAN
  #include <HomeSpan.h>
  #include "hs.hpp"
  TaskHandle_t homespanTaskHandle;
#endif


#define CELSIUS_SCALE 0 //Default
#define FAHRENHEIT_SCALE 1
BME280 climateSensor;
bool climateSensorInitialized = false;
Model<BME280_SensorMeasurements> lvglClimateModel;

Ticker tick;
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

Thread lvglThread = Thread();
Thread climateSensorThread = Thread();
Thread batteryWifiCheckThread = Thread();

static void batteryWifiCheckTask() {
  if(WiFi.status() == WL_CONNECTED) {
    int8_t rssi = WiFi.RSSI();
    // Serial.print("RSSI: "); Serial.println(rssi);
    if(rssi >= -60) {
      lv_img_set_src(WifiIcon, &wifi_connected_full);
    } else if(rssi >= -70) {
      lv_img_set_src(WifiIcon, &wifi_connected_66);
    } else if(rssi >= -80) {
      lv_img_set_src(WifiIcon, &wifi_connected_50);
    } else if(rssi >= -90) {
      lv_img_set_src(WifiIcon, &wifi_connected_33);
    } else {
      lv_img_set_src(WifiIcon, &wifi_connected_0);
    }
  } else {
    lv_img_set_src(WifiIcon, &wifi_disconnected);
  }
  
  if(IP5306_GetPowerSource() == 1) {
    if(IP5306_GetBatteryFull()) {
      lv_img_set_src(BatteryIcon, &battery_discharging_full);
    } else {
      lv_img_set_src(BatteryIcon, &battery_charging);
    }
  } else {
    uint8_t pct = IP5306_LEDS2PCT(IP5306_GetLevelLeds());
    if(pct >= 75) {
      lv_img_set_src(BatteryIcon, &battery_discharging_full);
    } else if(pct >= 50) {
      lv_img_set_src(BatteryIcon, &battery_discharging_66);
    } else if(pct >= 33) {
      lv_img_set_src(BatteryIcon, &battery_discharging_33);
    } else {
      lv_img_set_src(BatteryIcon, &battery_discharging_0);
    }
  }
}

static void lvglTask() {
  static BME280_SensorMeasurements climateMeasurements;
  if(lvglClimateModel.get(&climateMeasurements)) {
    String tempString = String((int)(climateMeasurements.temperature * 1.8f + 32)) + String("°F");
    String humidString = String((int)climateMeasurements.humidity) + String('%');
    
    lv_label_set_text(TempValue, tempString.c_str());
    lv_label_set_text(HumidityValue, humidString.c_str());
  }

  lv_task_handler();
}

static void climateSensorTask()
{
  static BME280_SensorMeasurements climateMeasurements;
  if(climateSensor.isMeasuring()) {
    climateSensorThread.setInterval(10);
    return;
  }
  climateSensor.readAllMeasurements(&climateMeasurements, CELSIUS_SCALE);
  lvglClimateModel.set(climateMeasurements);
#ifdef USE_HOMESPAN
  homespanClimateModel.set(climateMeasurements);
#endif    
  climateSensorThread.setInterval(1000);
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

static const int lv_tick_interval_ms = 1;
static void lv_tick_handler(void) { lv_tick_inc(lv_tick_interval_ms); }

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

void setup() {
  Serial.begin(115200);
  
  // wait until serial attaches or 4s passes...
  while(!Serial && !Serial.available() && millis() < 2000);
  //Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  
  // if use M5GO button, need set gpio15 OD or PP mode to avoid affecting the wifi signal  
  // grrrrr
  pinMode(15, OUTPUT_OPEN_DRAIN);
  pinMode(TFT_BL, OUTPUT);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  //Log.noticeln("GPIO initialization completed.");
  
  Wire.begin();
  if (!(climateSensorInitialized = climateSensor.beginI2C(Wire))) {
    //Log.errorln("Could not start BME280 sensor!");
  } else {
    climateSensor.setTempOverSample(1);
    climateSensor.setHumidityOverSample(1);
    climateSensor.setPressureOverSample(0);
    //Log.noticeln("BME280 initialization completed.");
  }

  // LVGL INITIALIZATION ROUTINES HERE!
  lv_init();
#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_log_cb);
#endif

  tft.init(); /* TFT init */
  tft.setRotation(TFT_ROTATION); /* Landscape orientation */
  tft.invertDisplay(TFT_INVERTED);
  //Log.noticeln("TFT initialized.");
  
  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_HEIGHT;
  disp_drv.ver_res = TFT_WIDTH; //rotation
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);
  tick.attach_ms(lv_tick_interval_ms, lv_tick_handler);

  BuildPages();
  lv_scr_load(Screen1);
  digitalWrite(TFT_BL, TFT_BL_ON);
  //Log.noticeln("LVGL initialized.");

  lvglClimateModel.init();

#ifdef USE_HOMESPAN
  homespanInit();
#endif

  lvglThread.onRun(lvglTask);
  lvglThread.setInterval(5);

  climateSensorThread.onRun(climateSensorTask);
  climateSensorThread.setInterval(0); // interval is set within the task

  batteryWifiCheckThread.onRun(batteryWifiCheckTask);
  batteryWifiCheckThread.setInterval(3000);
}

void loop() {
  if(climateSensorThread.shouldRun()) climateSensorThread.run();
  if(lvglThread.shouldRun()) lvglThread.run();
  if(batteryWifiCheckThread.shouldRun()) batteryWifiCheckThread.run();
#ifdef USE_HOMESPAN
  homespanLoop();
#endif
}
