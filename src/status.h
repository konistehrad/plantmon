#pragma once

#include <Arduino.h>
#include <WiFi.h>

struct PowerData {
  enum PowerSourceType : uint8_t {
    BATTERY = 0,
    VIN = 1,
  };
  bool batteryFull;
  uint8_t percentage;
  PowerSourceType powerSource;
  bool equals(const PowerData& d) const { 
    if(&d == NULL) return false;
    return memcmp(this, &d, sizeof(PowerData)) == 0; 
  }
};

struct WifiData {
  wl_status_t status;
  bool connected() { return status == WL_CONNECTED; }
  int8_t rssi;
  bool equals(const WifiData& d) const { 
    if(&d == NULL) return false;
    return memcmp(this, &d, sizeof(WifiData)) == 0; 
  }
};

struct SystemData {
  PowerData powerData;
  WifiData wifiData;
  bool equals(const SystemData& d) const { 
    if(&d == NULL) return false;
    return powerData.equals(d.powerData) && wifiData.equals(d.wifiData);
  }
};
