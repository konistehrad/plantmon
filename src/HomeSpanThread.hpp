#pragma once

#include <Arduino.h>
#include <SparkFunBME280.h>
#include <HomeSpan.h>
#include "BucketThread.hpp"
#include "Model.hpp"

class HomeSpanThread : 
    public BucketThread,
    public Subscriber<BME280_SensorMeasurements>
{
public:
  const char* name() override { return "HomespanThread"; }
  
  bool init() override {
    if(!BucketThread::init()) return false;
    if(!Subscriber<BME280_SensorMeasurements>::init()) return false;

    setInterval(0);
    homeSpan.setControlPin(5);
    homeSpan.setStatusPin(2);
    homeSpan.begin(Category::Other, "Troy");
    new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Helen of Troy");
      new Characteristic::Manufacturer("HomeSpan/M5Stack");
      new Characteristic::SerialNumber("666-6969");
      new Characteristic::Model("DIY");
      new Characteristic::FirmwareRevision("0.9");
      new Characteristic::Identify();
        
    new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
        
    new Service::TemperatureSensor();
      m_TempSensorCurrentTemp = new Characteristic::CurrentTemperature();

    new Service::HumiditySensor();
      m_HumiditySensorCurrentHumidity = new Characteristic::CurrentRelativeHumidity();

    return true;
  }
  void run() override {
    BME280_SensorMeasurements measurements;
    if(Subscriber<BME280_SensorMeasurements>::get(&measurements)) {
      m_TempSensorCurrentTemp->setVal(measurements.temperature);
      m_HumiditySensorCurrentHumidity->setVal(measurements.humidity);
    }
    homeSpan.poll();
    runned();
  }
protected:
  Characteristic::CurrentTemperature* m_TempSensorCurrentTemp;
  Characteristic::CurrentRelativeHumidity* m_HumiditySensorCurrentHumidity;
};
