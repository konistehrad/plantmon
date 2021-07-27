#pragma once

#include <vector>
#include <freertos/FreeRTOS.h>

template <class T>
class Model 
{
public:
  Model() { m_Queue = NULL; }
  ~Model() { if(m_Queue) vQueueDelete(m_Queue); m_Queue = NULL; }
  bool init() { m_Queue = xQueueCreate(1, sizeof(T)); return m_Queue; }
  bool available() { return uxQueueMessagesWaiting(m_Queue) > 0; }
  bool peek(T* val) { return xQueuePeek(m_Queue, val, 0); }
  bool get(T* val) { return xQueueReceive(m_Queue, val, 0); }
  bool set(T& val) { return xQueueOverwrite(m_Queue, &val); }
private:
  QueueHandle_t m_Queue;
};

template <class T>
class ModelList
{
public:
  void add(Model<T>* model) { m_Models.push_back(model); }
  void set(T& val) { for(Model<T>* m : m_Models) m->set(val); }
private:
  std::vector<Model<T>*> m_Models;
};

template <class T>
class Subscriber
{
public:
  bool init() { return m_Model.init(); }
  Model<T>* model() { return &m_Model; }
  bool available() { return m_Model.available(); }
  bool peek(T* val) { return m_Model.peek(val); }
  bool get(T* val) { return m_Model.get(val); }
protected:
  Model<T> m_Model;
};

template <class T>
class Publisher
{
public:
  void subscribe(Subscriber<T>& sub) { m_Models.add(sub.model()); }
  void subscribe(Subscriber<T>& sub0, Subscriber<T>& sub1) { 
    m_Models.add(sub0.model()); 
    m_Models.add(sub1.model());
  }
  void publish(T& val) { m_Models.set(val); }
protected:
  ModelList<T> m_Models;
};
