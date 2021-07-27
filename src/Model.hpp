#pragma once

#include <freertos/FreeRTOS.h>

template <class T>
class Model 
{
public:
  Model() { m_Queue = NULL; }
  ~Model() { if(m_Queue) vQueueDelete(m_Queue); m_Queue = NULL; }
  bool init() { m_Queue = xQueueCreate(1, sizeof(T)); return m_Queue; }
  bool peek(T* val) { return xQueuePeek(m_Queue, val, 0); }
  bool get(T* val) { return xQueueReceive(m_Queue, val, 0); }
  bool set(T& val) { return xQueueOverwrite(m_Queue, &val); }
private:
  QueueHandle_t m_Queue;
};
