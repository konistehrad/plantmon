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

StaticThreadController<5> threads(
  &ledThread,
  &homeSpanThread, 
  &sensorThread, 
  &statusThread, 
  &viewThread
);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2000);

  // if use M5GO button, need set gpio15 OD or PP mode to avoid affecting the wifi signal
  pinMode(15, OUTPUT_OPEN_DRAIN);

  Wire.begin();
  Serial.println("ledThread.init");
  if(!ledThread.init()) for(;;);
  // bootstrap power supply...
  Serial.println("statusThread.init");
  if(!statusThread.init()) for(;;);
  Serial.println("homeSpanThread.init");
  if(!homeSpanThread.init()) for(;;);
  Serial.println("sensorThread.init");
  if(!sensorThread.init()) for(;;);
  Serial.println("viewThread.init");
  if(!viewThread.init()) for(;;);

  Serial.println("sensorThread.subscribe(homeSpanThread, viewThread)");
  sensorThread.subscribe(homeSpanThread, viewThread);
  Serial.println("sensorThread.subscribe(ledThread, viewThread)");
  statusThread.subscribe(ledThread, viewThread);
  Serial.println("Setup done!");
}

void loop() {
  threads.run();
}
