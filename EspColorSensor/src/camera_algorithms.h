#include <esp_err.h>
#include "HslColor.h"

esp_err_t getColorOfObject(lego_color_t &result);
esp_err_t searchForLineWithColor(const lego_color_t search, int &rotationFromCenter);
