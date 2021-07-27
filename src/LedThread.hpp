#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "BucketThread.hpp"

template <int PIN, int N>
class LedThread : public BucketThread {
public:
  bool init() override {
    FastLED.addLeds<NEOPIXEL, PIN>(leds, N);
    FastLED.clear(true);
    FastLED.show();
    return true;
  }

  void setColorImmediate(const CRGB& color) {
    FastLED.showColor(color);
    FastLED.show();
  }

  void run() override {
    FastLED.show();
    runned();
  }
protected:
  CRGB leds[N];
};
