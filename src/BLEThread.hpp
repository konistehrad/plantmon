#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <SparkFunBME280.h>
#include "status.h"
#include "Model.hpp"
#include "BucketThread.hpp"

#define BLE_SERVICE_UUID (BLEUUID((uint16_t)0x181A))
#define BLE_CHARACTERISTIC_TEMP_UUID (BLEUUID((uint16_t)0x2A6E))
#define BLE_CHARACTERISTIC_HUMIDITY_UUID (BLEUUID((uint16_t)0x2A6F))
#define BLE_DESCRIPTOR_USERDESC_UUID (BLEUUID((uint16_t)0x2901))

class BLEThread : 
   public BucketThread
  ,public Subscriber<BME280_SensorMeasurements>
{
public:
  BLEThread()
    : BucketThread(), Subscriber<BME280_SensorMeasurements>(),
      m_TempCharacter(BLE_CHARACTERISTIC_TEMP_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY),
      m_HumidityCharacter(BLE_CHARACTERISTIC_HUMIDITY_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY),
      m_TempDesc(BLE_DESCRIPTOR_USERDESC_UUID),
      m_HumidityDesc(BLE_DESCRIPTOR_USERDESC_UUID)
  {

  }

  const char* name() override { return "BLEThread"; }

  bool init() override {
    uint16_t u16zero = 0;
    int16_t i16zero = 0;
    if(!BucketThread::init()) return false;
    if(!Subscriber<BME280_SensorMeasurements>::init()) return false;

    setInterval(1000);

    BLEDevice::init("Helen of Troy (ESP32-BLE)");
    m_Server = BLEDevice::createServer();
    m_Service = m_Server->createService(BLE_SERVICE_UUID);

    m_TempCharacter.setValue((uint8_t*)&i16zero, 2);
    m_HumidityCharacter.setValue((uint8_t*)&u16zero, 2);
    m_TempDesc.setValue("Temperature");
    m_HumidityDesc.setValue("Humidity");
    m_TempCharacter.addDescriptor(new BLE2902());
    m_HumidityCharacter.addDescriptor(new BLE2902());
    m_TempCharacter.addDescriptor(&m_TempDesc);
    m_HumidityCharacter.addDescriptor(&m_HumidityDesc);

    m_Service->addCharacteristic(&m_TempCharacter);
    m_Service->addCharacteristic(&m_HumidityCharacter);

    m_Service->start();

    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(BLE_SERVICE_UUID);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    advertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    return true;
  }

  void run() override {
    BME280_SensorMeasurements measurements;
    if(Subscriber<BME280_SensorMeasurements>::get(&measurements)) {
      uint16_t humidity = (uint16_t)(measurements.humidity * 100);
      int16_t temperature = (int16_t)(measurements.temperature * 100);
      m_TempCharacter.setValue((uint8_t*)&temperature, 2);
      m_HumidityCharacter.setValue((uint8_t*)&humidity, 2);
      m_TempCharacter.notify();
      m_HumidityCharacter.notify();
    }
  }
private:
  BLEServer* m_Server;
  BLEService* m_Service;
  BLECharacteristic m_TempCharacter;
  BLECharacteristic m_HumidityCharacter;
  BLEDescriptor m_TempDesc;
  BLEDescriptor m_HumidityDesc;
};
