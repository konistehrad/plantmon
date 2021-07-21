#pragma once

#include <lvgl.h>
#ifdef IS_WIOTERMINAL
    #include <Seeed_Arduino_FreeRTOS.h>
#endif
#ifdef IS_M5STACKFIRE
    #include <freertos/FreeRTOS.h>
#endif

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

class View
{
public:
    View(lv_obj_t* obj) : obj(obj) {}
    virtual ~View() { obj = NULL; }
    virtual void render() = 0;
private:
    lv_obj_t* obj;
};

class TextView: public View
{
public:
    TextView(lv_obj_t* obj) : View(obj) {}
    virtual void render() override {
        
    }
private:
};
