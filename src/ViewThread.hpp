#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <esp32-hal.h>
#include <Ticker.h>
#include <lvgl.h>
#include <SparkFunBME280.h>
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp> 
#include <JC_Button.h>
#include "status.h"
#include "BucketThread.hpp"
#include "Model.hpp"
#include "Testboard_ui.h"

const uint8_t BLK_PWM_CHANNEL = 7;

class ViewThread : 
   public BucketThread
  ,public Subscriber<BME280_SensorMeasurements>
#if PLANTMON_HAS_BATTERY==1
  ,public Subscriber<PowerData>
#endif
#if PLANTMON_USE_WIFI == 1
  ,public Subscriber<WifiData>
#endif
{
private:
  static const int lv_tick_interval_ms = 1;
  static void lv_tick_handler(void) { lv_tick_inc(lv_tick_interval_ms); }
  static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
  {
    LGFX* lcd = (LGFX*)disp->user_data;
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    lcd->pushImageDMA(area->x1, area->y1, w, h, ( uint16_t * )&color_p->full);
    lv_disp_flush_ready(disp);
  }

public:
  ViewThread() : m_Button(BUTTON_A) {}

  const char* name() override { return "ViewThread"; }

  bool init() override {
    if(!BucketThread::init()) return false;
    if(!Subscriber<BME280_SensorMeasurements>::init()) return false;
#if PLANTMON_HAS_BATTERY==1
    if(!Subscriber<PowerData>::init()) return false;
#endif
#if PLANTMON_USE_WIFI == 1
    if(!Subscriber<WifiData>::init()) return false;
#endif
    setInterval(5);
    
    // LVGL INITIALIZATION ROUTINES HERE!
    lv_init();

    m_Lcd.init(); 
    m_Lcd.initDMA();
    m_Lcd.setRotation(1);
    m_Lcd.startWrite(); // hold the line.
    
    lv_disp_buf_init(&m_DispBuf, m_Buf1, m_Buf2, LV_HOR_RES_MAX * 10);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.user_data = (void*)(&m_Lcd);
    disp_drv.buffer = &m_DispBuf;
    lv_disp_drv_register(&disp_drv);
    m_Ticker.attach_ms(lv_tick_interval_ms, lv_tick_handler);

    BuildPages();
    lv_scr_load(Screen1);

    m_Button.begin();
#if PLANTMON_USE_WIFI == 1
    lv_img_set_src(WifiIcon, &wifi_disconnected);
#endif
    return true;
  }
  
  void run() {
    if(millis() < 500) return;
    checkInput();
    checkSubs();
    lv_task_handler();
  }

  void brightness(uint8_t b) { m_Lcd.setBrightness(b); }
  uint8_t brightness() { return m_Lcd.getBrightness(); }

protected:
  Button m_Button;
  lv_disp_buf_t m_DispBuf;
  lv_color_t m_Buf1[LV_HOR_RES_MAX * 10];
  lv_color_t m_Buf2[LV_HOR_RES_MAX * 10];
  Ticker m_Ticker;
  LGFX m_Lcd;
  int m_Brightness;

  void checkInput() {
    m_Button.read();
    if(brightness() == 0 && m_Button.wasPressed()) {
      brightness(200);
    } else if(brightness() > 0 && m_Button.releasedFor(20*1000)) {
      brightness(0);
    }
  }

  void checkSubs() {
    PowerData powerData;
    BME280_SensorMeasurements measurements;
    if(Subscriber<BME280_SensorMeasurements>::get(&measurements)) {
      String tempString = String((int)(measurements.temperature * 1.8f + 32)) + String("°F");
      String humidString = String((int)measurements.humidity) + String('%');
      
      lv_label_set_text(TempValue, tempString.c_str());
      lv_label_set_text(HumidityValue, humidString.c_str());
    }

#if PLANTMON_HAS_BATTERY==1
    if(Subscriber<PowerData>::get(&powerData)) {
      if(powerData.powerSource == PowerData::VIN) {
        if(powerData.batteryFull) {
          lv_img_set_src(BatteryIcon, &battery_discharging_full);
        } else {
          lv_img_set_src(BatteryIcon, &battery_charging);
        }
      } else {
        if(powerData.percentage >= 75) {
          lv_img_set_src(BatteryIcon, &battery_discharging_full);
        } else if(powerData.percentage >= 50) {
          lv_img_set_src(BatteryIcon, &battery_discharging_66);
        } else if(powerData.percentage >= 33) {
          lv_img_set_src(BatteryIcon, &battery_discharging_33);
        } else {
          lv_img_set_src(BatteryIcon, &battery_discharging_0);
        }
      }
    }
#endif

#if PLANTMON_USE_WIFI == 1
    WifiData wifiData;
    if(Subscriber<WifiData>::get(&wifiData)) {
      if(wifiData.connected()) {
        if(wifiData.rssi >= -60) {
          lv_img_set_src(WifiIcon, &wifi_connected_full);
        } else if(wifiData.rssi >= -70) {
          lv_img_set_src(WifiIcon, &wifi_connected_66);
        } else if(wifiData.rssi >= -80) {
          lv_img_set_src(WifiIcon, &wifi_connected_50);
        } else if(wifiData.rssi >= -90) {
          lv_img_set_src(WifiIcon, &wifi_connected_33);
        } else {
          lv_img_set_src(WifiIcon, &wifi_connected_0);
        }
      } else {
        lv_img_set_src(WifiIcon, &wifi_disconnected);
      }
    }
#endif
  }
};
