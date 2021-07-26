#include <Arduino.h>
#include <Thread.h>
#include <StaticThreadController.h>

#define M5_NEO_NUM 10
#define M5_NEO_PIN 15

#include "Model.hpp"
#include "HomeSpanThread.hpp"
#include "SensorThread.hpp"
#include "StatusThread.hpp"
#include "ViewThread.hpp"

HomeSpanThread homeSpanThread;
SensorThread sensorThread;
StatusThread statusThread;
ViewThread viewThread;

StaticThreadController<4> threads(
  &homeSpanThread, 
  &sensorThread, 
  &statusThread, 
  &viewThread
);

void setup() {
  // if use M5GO button, need set gpio15 OD or PP mode to avoid affecting the wifi signal
  pinMode(15, OUTPUT_OPEN_DRAIN);
  Serial.begin(115200);
  
  // wait until serial attaches or 4s passes...
  while(!Serial && !Serial.available() && millis() < 2000);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  homeSpanThread.init();
  sensorThread.init();
  statusThread.init();
  viewThread.init();

  sensorThread.addModel(homeSpanThread.sensorModel());
  sensorThread.addModel(viewThread.sensorModel());
}

void loop() {
  threads.run();
}
