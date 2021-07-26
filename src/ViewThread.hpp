#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Ticker.h>
#include <lvgl.h>
#include <Thread.h>
#include <SparkFunBME280.h>
#include <TFT_eSPI.h>
#include "Model.hpp"
#include "Testboard_ui.h"

class ViewThread : public Thread {
public:
    bool init() {
        if(!m_SensorModel.init()) return false;
        setInterval(5);
        
        pinMode(TFT_BL, OUTPUT);

        // LVGL INITIALIZATION ROUTINES HERE!
        lv_init();

        tft.init(); /* TFT init */
        tft.setRotation(TFT_ROTATION); /* Landscape orientation */
        tft.invertDisplay(TFT_INVERTED);
        //Log.noticeln("TFT initialized.");
        
        lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

        lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = TFT_HEIGHT;
        disp_drv.ver_res = TFT_WIDTH; //rotation
        disp_drv.flush_cb = my_disp_flush;
        disp_drv.user_data = (void*)(&tft);
        disp_drv.buffer = &disp_buf;
        lv_disp_drv_register(&disp_drv);
        m_Ticker.attach_ms(lv_tick_interval_ms, lv_tick_handler);

        BuildPages();
        lv_scr_load(Screen1);
        // XXX: allow brightness control?
        digitalWrite(TFT_BL, TFT_BL_ON);
        return true;
    }
    void run() {
        if(m_SensorModel.get(&m_Measurements)) {
            String tempString = String((int)(m_Measurements.temperature * 1.8f + 32)) + String("°F");
            String humidString = String((int)m_Measurements.humidity) + String('%');
            
            lv_label_set_text(TempValue, tempString.c_str());
            lv_label_set_text(HumidityValue, humidString.c_str());
        }
        
        lv_task_handler();
        runned();
    }
    Model<BME280_SensorMeasurements>* sensorModel() { return &m_SensorModel; }
protected:
    lv_disp_buf_t disp_buf;
    lv_color_t buf[LV_HOR_RES_MAX * 10];
    Ticker m_Ticker;
    TFT_eSPI tft;
    BME280_SensorMeasurements m_Measurements;
    Model<BME280_SensorMeasurements> m_SensorModel;
private:
    static const int lv_tick_interval_ms = 1;
    static void lv_tick_handler(void) { lv_tick_inc(lv_tick_interval_ms); }
    static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
    {
        TFT_eSPI* tft = (TFT_eSPI*)disp->user_data;
        uint32_t w = (area->x2 - area->x1 + 1);
        uint32_t h = (area->y2 - area->y1 + 1);
        tft->startWrite();
        tft->setAddrWindow(area->x1, area->y1, w, h);
        tft->pushColors( ( uint16_t * )&color_p->full, w * h, true );
        tft->endWrite();
        lv_disp_flush_ready(disp);
    }
};
