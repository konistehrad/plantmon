
// Needed for LCD library
//#define LCD_SPI SPI3
//#define LCD_MISO_PIN PIN_SPI3_MISO
//#define LCD_MOSI_PIN PIN_SPI3_MOSI
//#define LCD_SCK_PIN PIN_SPI3_SCK
//#define LCD_SS_PIN PIN_SPI3_SS
//#define LCD_DC (70ul)
//#define LCD_RESET (71Ul)
//#define LCD_BACKLIGHT (72Ul)
//#define LCD_XL (73ul)
//#define LCD_YU (74ul)
//#define LCD_XR (75ul)
//#define LCD_YD (76ul)

//#define HASSPI 1
//#define SPICOM   LCD_SPI
//#define TFT_CS   LCD_SS_PIN
//#define TFT_DC   LCD_DC
//#define TFT_RST  LCD_RESET  //Set TFT_RST to -1 if the display RESET is connected to RST or 3.3V
//#define TFT_BL   LCD_BACKLIGHT
//#define TFT_BACKLIGHT_ON HIGH
//#define TFT_BACKLINGT_V 2000

#include <Arduino.h>
#include <ArduinoLog.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_I2CDevice.h>

#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>

Adafruit_ILI9341 tft = Adafruit_ILI9341(&LCD_SPI, LCD_DC, LCD_SS_PIN, LCD_RESET);
Adafruit_LvGL_Glue glue;

void setup() {
  Serial.begin(115200);
  
  // wait until serial attaches or 2s passes...
  while(!Serial && !Serial.available() && millis() < 4000){};
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  pinMode(LCD_BACKLIGHT, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  Log.noticeln(F("GPIO initialization completed."));

  tft.begin();
  tft.setRotation(3);
  digitalWrite(LCD_BACKLIGHT, HIGH);
  Log.noticeln(F("TFT initialization completed."));
  
  // Initialize glue, passing in address of display
  LvGLStatus status = glue.begin(&tft);
  if(status != LVGL_OK) {
    Log.fatalln(F("LvGL initialization failed: %d"), (int)status);
    for(;;);
  }

  // Create simple label centered on screen
  lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Hello Arduino");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
  Log.noticeln(F("LvGL initialization completed."));

}

static bool state = HIGH;
void loop() {
  lv_task_handler(); // Call LittleVGL task handler periodically
  delay(5);
  state = !state;
  digitalWrite(LED_BUILTIN, state);
}