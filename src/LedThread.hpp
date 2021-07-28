#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "status.h"
#include "Model.hpp"
#include "BucketThread.hpp"
#include "FastFX.h"
#include "FFXCoreEffects.h"

template <int PIN, int NUM_LEDS>
class LedThread : 
  public BucketThread, 
  public Subscriber<SystemData>
{
public:
  const char* name() override { return "LedThread"; }

  LedThread() : m_baseFX(NULL), m_overlayFX(NULL) {}

  bool init() override {
    if(!BucketThread::init()) return false;
    if(!Subscriber<SystemData>::init()) return false;
    FastLED.addLeds<NEOPIXEL, PIN>(m_leds, NUM_LEDS);
    FastLED.clear();
    m_fxctrlr.initialize(new FFXFastLEDPixelController(m_leds, NUM_LEDS));
    baseFX(new SolidFX(NUM_LEDS));
    brightness(100);
    return true;
  }

  void setColorImmediate(const CRGB& color) {
    if(m_baseFX) m_baseFX->setColor(color);
  }

  void run() override {
    SystemData systemData;
    if(Subscriber<SystemData>::get(&systemData)) {
      if(!m_systemData.wifiData.connected() && systemData.wifiData.connected()) {
        brightness(30);
        overlayFX(new PulseOverlayFX(NUM_LEDS, 20, 2, NamedPalettes::getInstance()["green"]));
      } else if(m_systemData.wifiData.connected() && !systemData.wifiData.connected()) {
        brightness(100);
        overlayFX(new PulseOverlayFX(NUM_LEDS, 50, 0, NamedPalettes::getInstance()["red"]));
      }
      m_systemData = std::move(systemData);
    }
    m_fxctrlr.update();
    runned();
  }
protected:
  void brightness(uint8_t bri) { m_fxctrlr.getPrimarySegment()->setBrightness(bri); }
  uint8_t brightness() { return m_fxctrlr.getPrimarySegment()->getBrightness(); }

  FFXBase* baseFX() { return m_baseFX; }
  FFXBase* baseFX(FFXBase* basefx) {
    m_baseFX = basefx;
    // this calls `delete`
    m_fxctrlr.getPrimarySegment()->setFX(basefx);
    return basefx;
  }

  FFXOverlay* overlayFX(FFXOverlay* overlayfx) {
    if(!overlayfx) {
      clearOverlay();
    } else {
      m_overlayFX = overlayfx;
      // calls `delete` on the old one if required
      m_fxctrlr.getPrimarySegment()->setOverlay(overlayfx);
    }
    return overlayfx;
  }
  void clearOverlay() { 
    m_overlayFX = NULL;
    m_fxctrlr.getPrimarySegment()->removeOverlay();
  }

  CRGB m_leds[NUM_LEDS];
  FFXController m_fxctrlr;
  FFXBase* m_baseFX;
  FFXOverlay* m_overlayFX;
  SystemData m_systemData;
};
