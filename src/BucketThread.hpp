#pragma once

#include <freertos/FreeRTOS.h>
#include <vector>

class BucketThread {
protected:
  static SemaphoreHandle_t wireMutex;
  static BaseType_t TakeWireMutex() {
    do {} while (xSemaphoreTakeRecursive(wireMutex, portMAX_DELAY) != pdPASS);
    return pdPASS;
  }
  static BaseType_t GiveWireMutex() {
    xSemaphoreGive(wireMutex);
    return pdPASS;
  }
  static std::vector<BucketThread*> allThreads;
  static void runTrampoline(void* pThis) { 
    BucketThread* thread = (BucketThread*)pThis;
    for(;;) {
      if(thread->m_interval > 0) {
        delay(thread->m_interval);
      } else {
        yield();
      }
      thread->run();
    }
  }
public:
  static void bootstrap() { 
    wireMutex = xSemaphoreCreateMutex();
  }
  static void ready() {
    for(auto t : allThreads) {
      // notify threads that we can start doing the thing
      xTaskCreatePinnedToCore(
        BucketThread::runTrampoline, /* Function to implement the task */
        t->name(),                   /* Name of the task */
        4096,                        /* Stack size in words */
        t,                           /* Task input parameter */
        1,                           /* Priority of the task */
        &t->m_taskHandle,            /* Task handle. */
        CONFIG_ARDUINO_RUNNING_CORE  /* Core where the task should run */
      );        
    }
  }
  static void service() { }

  virtual bool init() { 
    m_interval = 0;
    allThreads.push_back(this);
    return true;
  }

  void setInterval(uint32_t i) {
    m_interval = i;
  }

  virtual void run() = 0;
  virtual const char* name() { return "UNASSIGNED"; }
protected:
  // backcomp
  void runned() {  }
  TaskHandle_t m_taskHandle;
  uint32_t m_interval;
};

SemaphoreHandle_t BucketThread::wireMutex;
std::vector<BucketThread*> BucketThread::allThreads;
