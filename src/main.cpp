#include <Arduino.h>
#include <FastLED.h>

#include "Model.hpp"
#include "LedThread.hpp"
#include "SensorThread.hpp"
#include "PowerThread.hpp"
#include "ViewThread.hpp"

const uint8_t M5_NEO_NUM = 10;
const uint8_t M5_NEO_PIN = 15;

LedThread<M5_NEO_PIN, M5_NEO_NUM> ledThread;
SensorThread sensorThread;
PowerThread powerThread;
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
  if(!powerThread.init()) for(;;);
  if(!sensorThread.init()) for(;;);
  if(!viewThread.init()) for(;;);

  sensorThread.subscribe(viewThread);
  powerThread.subscribe(viewThread);
  
  BucketThread::ready();
}

void loop() {
  BucketThread::service();
}
