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

class StatusThread : 
  public BucketThread,
  public Publisher<SystemData>
{
public:
  bool init() override { 
    IP5306_SetVinCurrent(CURRENT_400MA);
    IP5306_SetChargingFullStopVoltage(BAT_4_2V);
    IP5306_SetEndChargeCurrentDetection(0); // End charge current 200ma
    IP5306_SetVoltagePressure(2); // Add volt 28mv
    IP5306_SetChargeCCLoop(1); // Vin charge CC
    setInterval(2000);
    return true; 
  }

  void run() {
    SystemData systemData;
    
    systemData.powerData.powerSource = (PowerData::PowerSourceType)IP5306_GetPowerSource();
    if(systemData.powerData.powerSource == PowerData::VIN) {
      systemData.powerData.batteryFull = IP5306_GetBatteryFull();
    } else {
      systemData.powerData.percentage = IP5306_LEDS2PCT(IP5306_GetLevelLeds());
    }

    systemData.wifiData.status = WiFi.status();
    if(systemData.wifiData.connected()) {
      systemData.wifiData.rssi = WiFi.RSSI();
    }
    
    if(!m_systemData.equals(systemData)) {
      m_systemData = std::move(systemData);
      Publisher<SystemData>::publish(m_systemData);
    }
    runned();
  }
private:
  SystemData m_systemData;  
};
