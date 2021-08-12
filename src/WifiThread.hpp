#pragma once

#include <Arduino.h>
#include <vector>
#include <ArduinoLog.h>

#include <ESPmDNS.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <WiFiManager.h>

#include "BucketThread.hpp"
#include "Model.hpp"
#include "status.h"

const char MQTT_DEFAULT_SERVER[MQTT_SERVER_MAX_LEN] = "monokuma.baohaus";
const char MQTT_DEFAULT_PORT[MQTT_PORT_MAX_LEN] = "1883";
const char MQTT_DEFAULT_TOKEN[MQTT_TOKEN_MAX_LEN] = "";
const char NVS_MQTT_SERVER_KEY[] = "PMQTserver";
const char NVS_MQTT_PORT_KEY[] = "PMQTport";
const char NVS_MQTT_TOKEN_KEY[] = "PMQTtoken";

class WifiThread : 
  public BucketThread,
  public Publisher<WifiData>,
  public Publisher<MqttServerData>
{
public:
  WifiThread() : 
    m_ServerData(MQTT_DEFAULT_SERVER, MQTT_DEFAULT_PORT, MQTT_DEFAULT_TOKEN),
    m_MqttServerParam(NULL),
    m_MqttPortParam(NULL),
    m_MqttTokenParam(NULL)
  {}
  ~WifiThread() {
    delete m_MqttServerParam;
    delete m_MqttPortParam;
    delete m_MqttTokenParam;
  }
  const char* name() override { return "WifiThread"; }

  bool init() override {
    if(!BucketThread::init()) return false;
    
    WiFi.mode(WIFI_STA);

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        if(nvs_flash_erase() != ESP_OK) {
          Log.fatalln("Could not format NVS: %s", esp_err_to_name(err));
          return false;
        }
        err = nvs_flash_init();
    }
    if(err != ESP_OK) {
      Log.fatalln("Failed to init NVS: %s", esp_err_to_name(err));
      return false;
    }
    err = nvs_open("storage", NVS_READWRITE, &m_NvsHandle);
    if(err != ESP_OK) {
      Log.fatalln("Failed to open NVS: %s", esp_err_to_name(err));
      return false;
    }

    err = loadParams();
    if(err != ESP_OK){
      Log.fatalln("Failed to load NVS params: %s", esp_err_to_name(err));
      return false;
    }
    
    m_WifiManager.setConfigPortalBlocking(false);
    m_WifiManager.setBreakAfterConfig(true);
    m_WifiManager.setSaveConfigCallback([&](){this->saveParams();});
    m_WifiManager.setConfigResetCallback([&](){this->clearParams();});

    m_MqttServerParam = new WiFiManagerParameter("server", "MQTT Server", m_ServerData.address.c_str(), MQTT_SERVER_MAX_LEN);
    m_MqttPortParam = new WiFiManagerParameter("port", "MQTT Port", m_ServerData.port.c_str(), MQTT_PORT_MAX_LEN);
    m_MqttTokenParam = new WiFiManagerParameter("token", "MQTT Token", m_ServerData.token.c_str(), MQTT_TOKEN_MAX_LEN);
    m_WifiManager.addParameter(m_MqttServerParam);
    m_WifiManager.addParameter(m_MqttPortParam);
    m_WifiManager.addParameter(m_MqttTokenParam);

    return true;
  }

  void start() override {
    String wifiName = String("PlantMon_") + String(WIFI_getChipId());
    m_WifiManager.setHostname(wifiName.c_str());
    m_WifiManager.setTimeout(0);
    if (m_WifiManager.autoConnect(wifiName.c_str())) {
      Log.infoln("Wifi connected.");
      m_WifiData.status = WL_CONNECTED;
      m_WifiData.rssi = WiFi.RSSI();
      Publisher<WifiData>::publish(m_WifiData);
    } else {
      Log.noticeln("Wifi did not connect, looping config portal.");
    }
  }
  
  void run() override {
    m_WifiManager.process();

  }

private:
  void clearParams() {
    esp_err_t err = nvs_erase_all(m_NvsHandle);
    if(err != ESP_OK) {
      Log.errorln("ClearParams failed: %s", esp_err_to_name(err));
    }
  }

  esp_err_t loadParams() {
    char buf[256];
    size_t size;
    esp_err_t err;
    
    size = MQTT_SERVER_MAX_LEN;
    err = nvs_get_str(m_NvsHandle, NVS_MQTT_SERVER_KEY, buf, &size);
    if(err == ESP_OK) m_ServerData.address = String(buf);
    else if(err != ESP_ERR_NVS_NOT_FOUND) return err;
    
    size = MQTT_PORT_MAX_LEN;
    err = nvs_get_str(m_NvsHandle, NVS_MQTT_PORT_KEY, buf, &size);
    if(err == ESP_OK) m_ServerData.port = String(buf);
    else if(err != ESP_ERR_NVS_NOT_FOUND) return err;

    size = MQTT_TOKEN_MAX_LEN;
    err = nvs_get_str(m_NvsHandle, NVS_MQTT_TOKEN_KEY, buf, &size);
    if(err == ESP_OK) m_ServerData.token = String(buf);
    else if(err != ESP_ERR_NVS_NOT_FOUND) return err;

    return ESP_OK;
  }

  esp_err_t saveParam(WiFiManagerParameter* param, const char* key, String& backing) {
    esp_err_t err;
    backing = String(param->getValue());
    err = nvs_set_str(m_NvsHandle, key, backing.c_str());
    if(err != ESP_OK) {
      Log.fatalln("Failed to save '%s' to NVS: %s", key, esp_err_to_name(err));
    }
    return err;
  }
  void saveParams() {
    saveParam(m_MqttServerParam, NVS_MQTT_SERVER_KEY, m_ServerData.address);
    saveParam(m_MqttPortParam, NVS_MQTT_PORT_KEY, m_ServerData.port);
    saveParam(m_MqttTokenParam, NVS_MQTT_TOKEN_KEY, m_ServerData.token);
    Publisher<MqttServerData>::publish(m_ServerData);
  }

  WifiData m_WifiData;
  MqttServerData m_ServerData;
  nvs_handle  m_NvsHandle;
  WiFiManager m_WifiManager;
  WiFiManagerParameter* m_MqttServerParam;
  WiFiManagerParameter* m_MqttPortParam;
  WiFiManagerParameter* m_MqttTokenParam;
};
