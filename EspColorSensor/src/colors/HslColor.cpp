#include "HslColor.h"
#include <cstdint>
#include <stdlib.h>

HslColor::HslColor(uint8_t r, uint8_t g, uint8_t b)
{
  {
    float dr = r / 255.0F, dg = g / 255.0F, db = b / 255.0F;
    float Cmin, Cmax, delta;
    Cmin = dr < dg ? dr : dg;
    Cmin = Cmin < db ? Cmin : db;
    Cmax = dr > dg ? dr : dg;
    Cmax = Cmax > db ? Cmax : db;
    delta = Cmax - Cmin;
    l = (Cmax + Cmin) / 2; // l

    // if (Cmax != 0)
    //   s = (delta / (1 - abs(2 * l - 1))); // s
    // else
    //   s = 0;

    if (dr == Cmax)
      h = (dg - db) / delta;
    else if (dg == Cmax)
      h = 2 + (db - dr) / delta;
    else if (db == Cmax)
      h = 4 + (dr - dg) / delta;
    h *= 60.0;
    // if (h < 0.0)
    // h += 360.0;
  }
}

lego_color_t HslColor::convertToLego()
{
  int hue = (int)(this->GetHue());
  int lgt = (int)(this->GetLightness() * 100);

  if (lgt < 7)
    return COLOR_BLACK;
  if (lgt > 67)
    return COLOR_WHITE;
  if (hue < 25)
    return COLOR_RED;
  if (hue < 75)
    return COLOR_YELLOW;
  if (hue < 160)
    return COLOR_GREEN;
  if (hue < 280)
    return COLOR_BLUE;

  return COLOR_NONE;
}

float HslColor::GetHue()
{
  return h;
}

float HslColor::GetLightness()
{
  return l;
}
