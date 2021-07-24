#ifndef TESTBOARD_UI_H
#define TESTBOARD_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

extern lv_obj_t * Screen1;
extern lv_obj_t * RootPanel;
extern lv_obj_t * TempIcon;
extern lv_obj_t * TempValue;
extern lv_obj_t * TempLabel;
extern lv_obj_t * HumidityIcon;
extern lv_obj_t * HumidityValue;
extern lv_obj_t * HumidityLabel;
extern lv_obj_t * FanIcon;
extern lv_obj_t * FanValue;
extern lv_obj_t * FanLabel;
extern lv_obj_t * WifiIcon;
extern lv_obj_t * BatteryIcon;


LV_IMG_DECLARE(fanicon);
LV_IMG_DECLARE(humidicon);
LV_IMG_DECLARE(thermicon);
LV_IMG_DECLARE(battery_charging);
LV_IMG_DECLARE(battery_discharging_0);
LV_IMG_DECLARE(battery_discharging_33);
LV_IMG_DECLARE(battery_discharging_66);
LV_IMG_DECLARE(battery_discharging_full);
LV_IMG_DECLARE(wifi_connected_0);
LV_IMG_DECLARE(wifi_connected_33);
LV_IMG_DECLARE(wifi_connected_50);
LV_IMG_DECLARE(wifi_connected_66);
LV_IMG_DECLARE(wifi_connected_full);
LV_IMG_DECLARE(wifi_disconnected);

void BuildPages(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
