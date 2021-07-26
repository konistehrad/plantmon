#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Thread.h>
#include <lvgl.h>
#include "Testboard_ui.h"
#include "IP5306.h"

class StatusThread : public Thread {
public:
    bool init() { 
        setInterval(2000);
        return true; 
    }
    void run() {
        if(WiFi.status() == WL_CONNECTED) {
            int8_t rssi = WiFi.RSSI();
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

        runned();
    }
};