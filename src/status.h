#pragma once

#include <Arduino.h>

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
    bool connected;
    int8_t rssi;
};

struct SystemData {
    PowerData powerData;
    WifiData wifiData;
};
