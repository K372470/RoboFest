#include <esp_err.h>
#include <colors/HslColor.h>
#define CAMERA_TAG "Thinker"

esp_err_t getColorOfObject(lego_color_t &result);
esp_err_t getFastLine(lego_color_t &result);
esp_err_t getLines(lego_color_t* result,size_t &lenght);