#pragma once

#include <Arduino.h>
#include <WiFi.h>

const size_t MQTT_SERVER_MAX_LEN = 40;
const size_t MQTT_PORT_MAX_LEN = 6;
const size_t MQTT_TOKEN_MAX_LEN = 32;

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

struct MqttServerData {
  MqttServerData() {}
  MqttServerData(const char* addr, const char* pt, const char* tok) :
    address(addr), port(pt), token(tok) {}
  String address;
  String port;
  uint16_t portNum() { return atoi(port.c_str()); }
  String token;
};
