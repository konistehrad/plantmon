#pragma once

#include <Arduino.h>
#include <SparkFunBME280.h>
#include <vector>
#include "BucketThread.hpp"
#include "Model.hpp"

#define CELSIUS_SCALE 0 //Default
#define FAHRENHEIT_SCALE 1

class SensorThread : public BucketThread {
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

    // SET ALL INCOMING MODELS!
    m_ClimateSensor.readAllMeasurements(&m_ClimateMeasurements, CELSIUS_SCALE);
    std::for_each(m_Models.begin(), m_Models.end(), [&](Model<BME280_SensorMeasurements>* m){
      m->set(m_ClimateMeasurements);
    });

    setInterval(500);
    runned();
  }
  void addModel(Model<BME280_SensorMeasurements>* model) {
    m_Models.push_back(model);
  }
protected:
  std::vector<Model<BME280_SensorMeasurements>*> m_Models;
  BME280 m_ClimateSensor;
  BME280_SensorMeasurements m_ClimateMeasurements;

};
