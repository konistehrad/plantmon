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
  ledThread.init();

  // if use M5GO button, need set gpio15 OD or PP mode to avoid affecting the wifi signal
  pinMode(15, OUTPUT_OPEN_DRAIN);

  // wait until serial attaches or 4s passes...
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  Wire.begin();
  // bootstrap power supply...
  statusThread.init();
  homeSpanThread.init();
  sensorThread.init();
  viewThread.init();

  sensorThread.addModel(homeSpanThread.sensorModel());
  sensorThread.addModel(viewThread.sensorModel());
}

void loop() {
  threads.run();
}
