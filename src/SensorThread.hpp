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
  bool init() override {
    if (!m_ClimateSensor.beginI2C(Wire)) return false;
    m_ClimateSensor.setTempOverSample(1);
    m_ClimateSensor.setHumidityOverSample(1);
    m_ClimateSensor.setPressureOverSample(0);
    return true;
  }
  void run() {
    if(m_ClimateSensor.isMeasuring()) {
      setInterval(10);
      runned();
      return;
    }

    m_ClimateSensor.readAllMeasurements(&m_ClimateMeasurements, CELSIUS_SCALE);
    Publisher<BME280_SensorMeasurements>::publish(m_ClimateMeasurements);
    setInterval(500);
    runned();
  }

protected:
  BME280 m_ClimateSensor;
  BME280_SensorMeasurements m_ClimateMeasurements;
};
