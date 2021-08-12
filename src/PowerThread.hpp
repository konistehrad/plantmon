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

class PowerThread : 
  public BucketThread,
  public Publisher<PowerData>
{
public:
  const char* name() override { return "PowerThread"; }

  bool init() override { 
    if(!BucketThread::init()) return false;
    IP5306_SetVinCurrent(CURRENT_400MA);
    IP5306_SetChargingFullStopVoltage(BAT_4_2V);
    IP5306_SetEndChargeCurrentDetection(0); // End charge current 200ma
    IP5306_SetVoltagePressure(2); // Add volt 28mv
    IP5306_SetChargeCCLoop(1); // Vin charge CC
    setInterval(2000);
    return true; 
  }

  void run() {
    PowerData powerData;
    
    TakeWireMutex();
    powerData.powerSource = (PowerData::PowerSourceType)ip5306_get_bits(IP5306_REG_READ_0, 3, 1);
    if(powerData.powerSource == PowerData::VIN) {
      powerData.batteryFull = IP5306_GetBatteryFull();
    } else {
      powerData.percentage = IP5306_LEDS2PCT(IP5306_GetLevelLeds());
    }
    GiveWireMutex();

    if(!m_PowerData.equals(powerData)) {
      m_PowerData = std::move(powerData);
      Publisher<PowerData>::publish(m_PowerData);
    }
  }
private:
  PowerData m_PowerData;  
};
