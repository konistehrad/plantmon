#include <Arduino.h>
#include <Thread.h>
#include <StaticThreadController.h>
#include <FastLED.h>

#include "Model.hpp"
#include "LedThread.hpp"
#include "HomeSpanThread.hpp"
#include "SensorThread.hpp"
#include "StatusThread.hpp"
#include "ViewThread.hpp"

const uint8_t M5_NEO_NUM = 10;
const uint8_t M5_NEO_PIN = 15;

LedThread<M5_NEO_PIN, M5_NEO_NUM> ledThread;
HomeSpanThread homeSpanThread;
SensorThread sensorThread;
StatusThread statusThread;
ViewThread viewThread;

void setup() {
  BucketThread::bootstrap();

  Serial.begin(115200);
  while (!Serial && millis() < 2000);

  // if use M5GO button, need set gpio15 OD or PP mode to avoid affecting the wifi signal
  pinMode(15, OUTPUT_OPEN_DRAIN);

  Wire.begin();
  if(!ledThread.init()) for(;;);
  // bootstrap power supply...
  if(!statusThread.init()) for(;;);
  if(!homeSpanThread.init()) for(;;);
  if(!sensorThread.init()) for(;;);
  if(!viewThread.init()) for(;;);

  sensorThread.subscribe(homeSpanThread, viewThread);
  statusThread.subscribe(ledThread, viewThread);
  
  BucketThread::ready();
}

void loop() {
  BucketThread::service();
}
