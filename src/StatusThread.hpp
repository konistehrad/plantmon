#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>
#include "BucketThread.hpp"
#include "Model.hpp"
#include "Testboard_ui.h"
#include "IP5306.h"
#include "status.h"

#define CURRENT_100MA  (0x01 << 0)
#define CURRENT_200MA  (0x01 << 1)
#define CURRENT_400MA  (0x01 << 2)
#define CURRENT_800MA  (0x01 << 3)
#define CURRENT_1600MA  (0x01 << 4)

#define BAT_4_2V      (0x00)
#define BAT_4_3V      (0x01)
#define BAT_4_3_5V    (0x02)
#define BAT_4_4V      (0x03)

#define CHG_CC_BIT    (0x20)

class StatusThread : public BucketThread {
public:
  bool init() override { 
    IP5306_SetVinCurrent(CURRENT_400MA);
    IP5306_SetChargingFullStopVoltage(BAT_4_2V);
    IP5306_SetEndChargeCurrentDetection(0); // End charge current 200ma
    IP5306_SetVoltagePressure(2); // Add volt 28mv
    IP5306_SetChargeCCLoop(1); // Vin charge CC
    setInterval(2000);
    if(!m_SystemDataModel.init()) return false;
    return true; 
  }

  void run() {
    m_SystemData.wifiData.connected = WiFi.status() == WL_CONNECTED;
    if(m_SystemData.wifiData.connected) {
      m_SystemData.wifiData.rssi = WiFi.RSSI();
      if(m_SystemData.wifiData.rssi >= -60) {
        lv_img_set_src(WifiIcon, &wifi_connected_full);
      } else if(m_SystemData.wifiData.rssi >= -70) {
        lv_img_set_src(WifiIcon, &wifi_connected_66);
      } else if(m_SystemData.wifiData.rssi >= -80) {
        lv_img_set_src(WifiIcon, &wifi_connected_50);
      } else if(m_SystemData.wifiData.rssi >= -90) {
        lv_img_set_src(WifiIcon, &wifi_connected_33);
      } else {
        lv_img_set_src(WifiIcon, &wifi_connected_0);
      }
    } else {
      lv_img_set_src(WifiIcon, &wifi_disconnected);
    }

    m_SystemData.powerData.powerSource = (PowerData::PowerSourceType)IP5306_GetPowerSource();
    if(m_SystemData.powerData.powerSource == PowerData::VIN) {
      m_SystemData.powerData.batteryFull = IP5306_GetBatteryFull();
      if(m_SystemData.powerData.batteryFull) {
        lv_img_set_src(BatteryIcon, &battery_discharging_full);
      } else {
        lv_img_set_src(BatteryIcon, &battery_charging);
      }
    } else {
      m_SystemData.powerData.percentage = IP5306_LEDS2PCT(IP5306_GetLevelLeds());
      if(m_SystemData.powerData.percentage >= 75) {
        lv_img_set_src(BatteryIcon, &battery_discharging_full);
      } else if(m_SystemData.powerData.percentage >= 50) {
        lv_img_set_src(BatteryIcon, &battery_discharging_66);
      } else if(m_SystemData.powerData.percentage >= 33) {
        lv_img_set_src(BatteryIcon, &battery_discharging_33);
      } else {
        lv_img_set_src(BatteryIcon, &battery_discharging_0);
      }
    }
    
    runned();
  }

protected:
  SystemData m_SystemData;
  Model<SystemData> m_SystemDataModel;
};
