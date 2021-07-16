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


LV_IMG_DECLARE(img547112343);   // assets\thermometer_icon-icons.com_65086-SM.png
LV_IMG_DECLARE(img133344875);   // assets\waterdrop_102608-SM.png
LV_IMG_DECLARE(img_fan_icon_137646_png);   // assets\fan_icon_137646.png

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
