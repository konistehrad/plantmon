#pragma once

#include <Arduino.h>
#include <SparkFunBME280.h>
#include <HomeSpan.h>
#include "BucketThread.hpp"
#include "Model.hpp"

class HomeSpanThread : public BucketThread {
public:
    bool init() override {
        if(!m_SensorModel.init()) return false;
        setInterval(0);
        // homeSpan.setControlPin(BUTTON_A);
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
        if(m_SensorModel.get(&m_Measurements)) {
            m_TempSensorCurrentTemp->setVal(m_Measurements.temperature);
            m_HumiditySensorCurrentHumidity->setVal(m_Measurements.humidity);
        }
        homeSpan.poll();
        runned();
    }
    Model<BME280_SensorMeasurements>* sensorModel() { return &m_SensorModel; }
protected:
    BME280_SensorMeasurements m_Measurements;
    Model<BME280_SensorMeasurements> m_SensorModel;
    Characteristic::CurrentTemperature* m_TempSensorCurrentTemp;
    Characteristic::CurrentRelativeHumidity* m_HumiditySensorCurrentHumidity;
};
