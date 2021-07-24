#pragma once


#ifdef USE_HOMESPAN
#include <SparkFunBME280.h>
#include <HomeSpan.h>
#include "Model.hpp"

Model<BME280_SensorMeasurements> homespanClimateModel;

static Service::TemperatureSensor* tempSensor;
static Characteristic::CurrentTemperature* tempSensorCurrentTemp;
static Service::HumiditySensor* humiditySensor;
static Characteristic::CurrentRelativeHumidity* humiditySensorCurrentHumidity;
void homespanInit() {
  homespanClimateModel.init();
  homeSpan.setControlPin(BUTTON_A);

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
    
    tempSensor = new Service::TemperatureSensor();
      tempSensorCurrentTemp = new Characteristic::CurrentTemperature();

    humiditySensor = new Service::HumiditySensor();
      humiditySensorCurrentHumidity = new Characteristic::CurrentRelativeHumidity();

}

void homespanLoop()
{
  static BME280_SensorMeasurements climateMeasurements;
  if(homespanClimateModel.get(&climateMeasurements)) {
    tempSensorCurrentTemp->setVal(climateMeasurements.temperature);
    humiditySensorCurrentHumidity->setVal(climateMeasurements.humidity);
  }
  homeSpan.poll();
}

#endif
