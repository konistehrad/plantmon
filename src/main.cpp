#include <Arduino.h>
#include <ArduinoLog.h>
#include <FastLED.h>

#include "Model.hpp"
#include "LedThread.hpp"
#include "SensorThread.hpp"
#include "PowerThread.hpp"
#include "ViewThread.hpp"
#if PLANTMON_USE_BLE == 1
#include "BLEThread.hpp"
#endif
#if PLANTMON_USE_WIFI == 1
#include "WifiThread.hpp"
#endif

const uint8_t M5_NEO_NUM = 10;
const uint8_t M5_NEO_PIN = 15;

LedThread<M5_NEO_PIN, M5_NEO_NUM> ledThread;
SensorThread sensorThread;
PowerThread powerThread;
ViewThread viewThread;
#if PLANTMON_USE_WIFI == 1
WifiThread wifiThread;
#endif
#if PLANTMON_USE_BLE == 1
BLEThread bleThread;
#endif

static void printAndDie(const char* message) {
  String out = String(message) + String(" failed. Please restart.");
  Log.fatalln(out.c_str());
  for(;;);
}

void setup() {
  // if use M5GO button, need set gpio15 OD or PP mode to avoid affecting the wifi signal
  pinMode(15, OUTPUT_OPEN_DRAIN);

  Serial.begin(115200);
  Log.begin(LOG_LEVEL, &Serial);
  while (!Serial && millis() < 2000) delay(10);
  if(!BucketThread::bootstrap()) printAndDie("BucketThread::bootstrap");

  Wire.begin();
  if(!ledThread.init()) printAndDie("ledThread.init");
#if PLANTMON_HAS_BATTERY==1
  if(!powerThread.init()) printAndDie("powerThread.init");
#endif
  if(!sensorThread.init()) printAndDie("sensorThread.init");
  if(!viewThread.init()) printAndDie("viewThread.init");
#if PLANTMON_USE_WIFI == 1
  if(!wifiThread.init()) printAndDie("wifiThread.init");
#endif
#if PLANTMON_USE_BLE == 1
  if(!bleThread.init()) printAndDie("bleThread.init");
  sensorThread.subscribe(bleThread);
#endif
#if PLANTMON_USE_WIFI == 1
  wifiThread.Publisher<WifiData>::subscribe(viewThread);
#endif

  sensorThread.subscribe(viewThread);
  powerThread.subscribe(viewThread);
  
  BucketThread::ready();
}

void loop() {
  BucketThread::service();
}
