#include "Testboard_ui.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * Screen1;
lv_obj_t * RootPanel;
lv_obj_t * TempIcon;
lv_obj_t * TempValue;
lv_obj_t * TempLabel;
lv_obj_t * HumidityIcon;
lv_obj_t * HumidityValue;
lv_obj_t * HumidityLabel;
lv_obj_t * FanIcon;
lv_obj_t * FanValue;
lv_obj_t * FanLabel;

///////////////////// IMAGES ////////////////////
LV_IMG_DECLARE(fanicon);
LV_IMG_DECLARE(humidicon);
LV_IMG_DECLARE(thermicon);

///////////////////// FUNCTIONS ////////////////////
#define BAR_PROPERTY_VALUE 0
#define BAR_PROPERTY_VALUE_WITH_ANIM 1

void SetBarProperty(lv_obj_t * target, int id, int val)
{
    if(id == BAR_PROPERTY_VALUE_WITH_ANIM) lv_bar_set_value(target, val, LV_ANIM_ON);
    if(id == BAR_PROPERTY_VALUE) lv_bar_set_value(target, val, LV_ANIM_OFF);
}

#define BASIC_PROPERTY_POSITION_X 0
#define BASIC_PROPERTY_POSITION_Y 1
#define BASIC_PROPERTY_WIDTH 2
#define BASIC_PROPERTY_HEIGHT 3
#define BASIC_PROPERTY_CLICKABLE 4
#define BASIC_PROPERTY_HIDDEN 5
#define BASIC_PROPERTY_DRAGABLE 6
#define BASIC_PROPERTY_DISABLED 7

void SetBasicProperty(lv_obj_t * target, int id, int val)
{
    if(id == BASIC_PROPERTY_POSITION_X) lv_obj_set_x(target, val);
    if(id == BASIC_PROPERTY_POSITION_Y) lv_obj_set_y(target, val);
    if(id == BASIC_PROPERTY_WIDTH) lv_obj_set_width(target, val);
    if(id == BASIC_PROPERTY_HEIGHT) lv_obj_set_height(target, val);
}

void SetBasicPropertyB(lv_obj_t * target, int id, bool val)
{
    if(id == BASIC_PROPERTY_CLICKABLE) lv_obj_set_click(target, val);
    if(id == BASIC_PROPERTY_HIDDEN) lv_obj_set_hidden(target, val);
    if(id == BASIC_PROPERTY_DRAGABLE) lv_obj_set_drag(target, val);
    if(id == BASIC_PROPERTY_DISABLED) {
        if(val) lv_obj_add_state(target, LV_STATE_DISABLED);
        else lv_obj_clear_state(target, LV_STATE_DISABLED);
    }
}

#define BUTTON_PROPERTY_TOGGLE 0
#define BUTTON_PROPERTY_CHECKED 1

void SetButtonProperty(lv_obj_t * target, int id, bool val)
{
    if(id == BUTTON_PROPERTY_TOGGLE) lv_btn_toggle(target);
    if(id == BUTTON_PROPERTY_CHECKED) lv_btn_set_state(target, val ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
}

#define DROPDOWN_PROPERTY_SELECTED 0

void SetDropdownProperty(lv_obj_t * target, int id, int val)
{
    if(id == DROPDOWN_PROPERTY_SELECTED) lv_dropdown_set_selected(target, val);
}

#define IMAGE_PROPERTY_IMAGE 0

void SetImageProperty(lv_obj_t * target, int id, uint8_t * val)
{
    if(id == IMAGE_PROPERTY_IMAGE) lv_img_set_src(target, val);
}

#define LABEL_PROPERTY_TEXT 0

void SetLabelProperty(lv_obj_t * target, int id, char * val)
{
    if(id == LABEL_PROPERTY_TEXT) lv_label_set_text(target, val);
}

#define ROLLER_PROPERTY_SELECTED 0
#define ROLLER_PROPERTY_SELECTED_WITH_ANIM 1

void SetRollerProperty(lv_obj_t * target, int id, int val)
{
    if(id == ROLLER_PROPERTY_SELECTED_WITH_ANIM) lv_roller_set_selected(target, val, LV_ANIM_ON);
    if(id == ROLLER_PROPERTY_SELECTED) lv_roller_set_selected(target, val, LV_ANIM_OFF);
}

#define SLIDER_PROPERTY_VALUE 0
#define SLIDER_PROPERTY_VALUE_WITH_ANIM 1

void SetSliderProperty(lv_obj_t * target, int id, int val)
{
    if(id == SLIDER_PROPERTY_VALUE_WITH_ANIM) lv_slider_set_value(target, val, LV_ANIM_ON);
    if(id == SLIDER_PROPERTY_VALUE) lv_slider_set_value(target, val, LV_ANIM_OFF);
}

void ChangeScreen(lv_obj_t * target, int fademode, int spd, int delay)
{
    lv_scr_load_anim(target, fademode, spd, delay, false);
}

void SetOpacity(lv_obj_t * target, int val)
{
    lv_obj_set_style_local_opa_scale(target, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, val);
}

void anim_callback_set_x(lv_anim_t * a, lv_anim_value_t v)
{
    lv_obj_set_x(a->user_data, v);
}

void anim_callback_set_y(lv_anim_t * a, lv_anim_value_t v)
{
    lv_obj_set_y(a->user_data, v);
}

void anim_callback_set_width(lv_anim_t * a, lv_anim_value_t v)
{
    lv_obj_set_width(a->user_data, v);
}

void anim_callback_set_height(lv_anim_t * a, lv_anim_value_t v)
{
    lv_obj_set_height(a->user_data, v);
}

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS2 ////////////////////

///////////////////// SCREENS ////////////////////
void BuildPages(void)
{
    Screen1 = lv_obj_create(NULL, NULL);

    RootPanel = lv_obj_create(Screen1, NULL);
    lv_obj_set_click(RootPanel, false);
    lv_obj_set_hidden(RootPanel, false);
    lv_obj_clear_state(RootPanel, LV_STATE_DISABLED);
    lv_obj_set_size(RootPanel, 320, 240);  // force: 0
    lv_obj_align(RootPanel, Screen1, LV_ALIGN_CENTER, 0, 0); // force: 320
    lv_obj_set_drag(RootPanel, false);
    lv_obj_set_style_local_bg_color(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                    lv_color_hex(0 * 256 * 256 + 92 * 256 + 101));
    lv_obj_set_style_local_bg_opa(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_border_color(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                        lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
    lv_obj_set_style_local_border_opa(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_outline_color(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                         lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
    lv_obj_set_style_local_outline_opa(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_outline_width(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 3);
    lv_obj_set_style_local_outline_pad(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_inner(RootPanel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 10);

    TempIcon = lv_img_create(RootPanel, NULL);
    lv_img_set_src(TempIcon, &thermicon);
    lv_obj_set_style_local_image_recolor(TempIcon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_image_recolor_opa(TempIcon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_click(TempIcon, false);
    lv_obj_set_hidden(TempIcon, false);
    lv_obj_set_size(TempIcon, 64, 64);
    lv_obj_align(TempIcon, RootPanel, LV_ALIGN_IN_TOP_LEFT, 8, 10);
    lv_obj_set_drag(TempIcon, false);

    lv_obj_clear_state(TempIcon, LV_STATE_DISABLED);

    TempValue = lv_label_create(RootPanel, NULL);
    lv_label_set_long_mode(TempValue, LV_LABEL_LONG_EXPAND);
    lv_label_set_align(TempValue, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(TempValue, "--°F");
    lv_obj_set_size(TempValue, 112, 52);  // force: 81
    lv_obj_set_click(TempValue, false);
    lv_obj_set_hidden(TempValue, false);
    lv_obj_clear_state(TempValue, LV_STATE_DISABLED);
    lv_obj_set_drag(TempValue, false);
    lv_obj_set_style_local_text_color(TempValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                      lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
    lv_obj_set_style_local_text_opa(TempValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_font(TempValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);

    lv_obj_align(TempValue, RootPanel, LV_ALIGN_IN_TOP_LEFT, 81, 9); // force: 112

    TempLabel = lv_label_create(RootPanel, NULL);
    lv_label_set_long_mode(TempLabel, LV_LABEL_LONG_EXPAND);
    lv_label_set_align(TempLabel, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(TempLabel, "Temperature");
    lv_obj_set_size(TempLabel, 95, 16);  // force: 85
    lv_obj_set_click(TempLabel, false);
    lv_obj_set_hidden(TempLabel, false);
    lv_obj_clear_state(TempLabel, LV_STATE_DISABLED);
    lv_obj_set_drag(TempLabel, false);
    lv_obj_set_style_local_text_color(TempLabel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                      lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
    lv_obj_set_style_local_text_opa(TempLabel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);

    lv_obj_align(TempLabel, RootPanel, LV_ALIGN_IN_TOP_LEFT, 85, 59); // force: 95

    HumidityIcon = lv_img_create(RootPanel, NULL);
    lv_img_set_src(HumidityIcon, &humidicon);
    lv_obj_set_style_local_image_recolor(HumidityIcon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_image_recolor_opa(HumidityIcon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);

    lv_obj_set_click(HumidityIcon, false);
    lv_obj_set_hidden(HumidityIcon, false);
    lv_obj_set_size(HumidityIcon, 64, 64);
    lv_obj_align(HumidityIcon, RootPanel, LV_ALIGN_IN_TOP_LEFT, 6, 88);
    lv_obj_set_drag(HumidityIcon, false);

    lv_obj_clear_state(HumidityIcon, LV_STATE_DISABLED);

    HumidityValue = lv_label_create(RootPanel, NULL);
    lv_label_set_long_mode(HumidityValue, LV_LABEL_LONG_EXPAND);
    lv_label_set_align(HumidityValue, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(HumidityValue, "--%");
    lv_obj_set_size(HumidityValue, 100, 52);  // force: 83
    lv_obj_set_click(HumidityValue, false);
    lv_obj_set_hidden(HumidityValue, false);
    lv_obj_clear_state(HumidityValue, LV_STATE_DISABLED);
    lv_obj_set_drag(HumidityValue, false);
    lv_obj_set_style_local_text_color(HumidityValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                      lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
    lv_obj_set_style_local_text_opa(HumidityValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_font(HumidityValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);

    lv_obj_align(HumidityValue, RootPanel, LV_ALIGN_IN_TOP_LEFT, 83, 85); // force: 100

    HumidityLabel = lv_label_create(RootPanel, NULL);
    lv_label_set_long_mode(HumidityLabel, LV_LABEL_LONG_EXPAND);
    lv_label_set_align(HumidityLabel, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(HumidityLabel, "Humidity");
    lv_obj_set_size(HumidityLabel, 68, 16);  // force: 85
    lv_obj_set_click(HumidityLabel, false);
    lv_obj_set_hidden(HumidityLabel, false);
    lv_obj_clear_state(HumidityLabel, LV_STATE_DISABLED);
    lv_obj_set_drag(HumidityLabel, false);
    lv_obj_set_style_local_text_color(HumidityLabel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                      lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
    lv_obj_set_style_local_text_opa(HumidityLabel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);

    lv_obj_align(HumidityLabel, RootPanel, LV_ALIGN_IN_TOP_LEFT, 85, 136); // force: 68

    FanIcon = lv_img_create(RootPanel, NULL);
    lv_img_set_src(FanIcon, &fanicon);
    lv_obj_set_style_local_image_recolor(FanIcon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_image_recolor_opa(FanIcon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);

    lv_obj_set_click(FanIcon, false);
    lv_obj_set_hidden(FanIcon, false);
    lv_obj_set_size(FanIcon, 64, 64);
    lv_obj_align(FanIcon, RootPanel, LV_ALIGN_CENTER, -121, 74);
    lv_obj_set_drag(FanIcon, false);

    lv_obj_clear_state(FanIcon, LV_STATE_DISABLED);

    FanValue = lv_label_create(RootPanel, NULL);
    lv_label_set_long_mode(FanValue, LV_LABEL_LONG_EXPAND);
    lv_label_set_align(FanValue, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(FanValue, "0%");
    lv_obj_set_size(FanValue, 72, 52);  // force: 83
    lv_obj_set_click(FanValue, false);
    lv_obj_set_hidden(FanValue, false);
    lv_obj_clear_state(FanValue, LV_STATE_DISABLED);
    lv_obj_set_drag(FanValue, false);
    lv_obj_set_style_local_text_color(FanValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                      lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
    lv_obj_set_style_local_text_opa(FanValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_font(FanValue, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);

    lv_obj_align(FanValue, RootPanel, LV_ALIGN_IN_TOP_LEFT, 83, 160); // force: 72

    FanLabel = lv_label_create(RootPanel, NULL);
    lv_label_set_long_mode(FanLabel, LV_LABEL_LONG_EXPAND);
    lv_label_set_align(FanLabel, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(FanLabel, "Fan Speed");
    lv_obj_set_size(FanLabel, 78, 16);  // force: 85
    lv_obj_set_click(FanLabel, false);
    lv_obj_set_hidden(FanLabel, false);
    lv_obj_clear_state(FanLabel, LV_STATE_DISABLED);
    lv_obj_set_drag(FanLabel, false);
    lv_obj_set_style_local_text_color(FanLabel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                      lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
    lv_obj_set_style_local_text_opa(FanLabel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);

    lv_obj_align(FanLabel, RootPanel, LV_ALIGN_IN_TOP_LEFT, 85, 208); // force: 78

}

