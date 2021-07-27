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
};

struct WifiData {
    wl_status_t status;
    bool connected() { return status == WL_CONNECTED; }
    int8_t rssi;
};

struct SystemData {
    PowerData powerData;
    WifiData wifiData;
};
