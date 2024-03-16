#include <Arduino.h>
#include "lego_color_t.h"
class HslColor
{
public:
  HslColor(uint8_t r, uint8_t g, uint8_t b);

  lego_color_t convertToLego();

  float GetHue();
  float GetSaturation();
  float GetLightness();

private:
  float h = 0, s = 0, l = 0;
};