#pragma once

#include <Arduino.h>
#include <SparkFunBME280.h>
#include <vector>
#include "BucketThread.hpp"
#include "Model.hpp"

#define CELSIUS_SCALE 0 //Default
#define FAHRENHEIT_SCALE 1

class SensorThread : 
  public BucketThread,
  public Publisher<BME280_SensorMeasurements>
{
public:
  const char* name() override { return "SensorThread"; }

  bool init() override {
    if(!BucketThread::init()) return false;
    if(!m_ClimateSensor.beginI2C(Wire)) return false;
    m_ClimateSensor.setTempOverSample(1);
    m_ClimateSensor.setHumidityOverSample(1);
    m_ClimateSensor.setPressureOverSample(0);
    return true;
  }
  void run() {
    TakeWireMutex();
    if(m_ClimateSensor.isMeasuring()) {
      GiveWireMutex();
      setInterval(10);
      runned();
      return;
    }

    m_ClimateSensor.readAllMeasurements(&m_ClimateMeasurements, CELSIUS_SCALE);
    GiveWireMutex();
    Publisher<BME280_SensorMeasurements>::publish(m_ClimateMeasurements);
    setInterval(500);
  }

protected:
  BME280 m_ClimateSensor;
  BME280_SensorMeasurements m_ClimateMeasurements;
};
