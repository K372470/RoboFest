#include <Esp.h>
#include <esp_camera.h>
#include <esp_err.h>
#include "camera_algorithms.h"

const char *CAMERA_TAG = "Thinker";

size_t getPixelIndex(size_t x, size_t y, size_t w) { return x + y * w; }

esp_err_t getColorOfObject(lego_color_t &result)
{
  camera_fb_t *fb = NULL;
  esp_err_t err = ESP_OK;
  uint8_t *rgb_buffer = NULL;
  size_t rgb_buffer_len;
  size_t w, h = 0;

  fb = esp_camera_fb_get();
  if (!fb)
  {
    ESP_LOGE(CAMERA_TAG, "Camera capture failed");
    esp_camera_fb_return(fb);
    return ESP_FAIL;
  }
  w = fb->width;
  h = fb->height;
  rgb_buffer_len = fb->width * fb->height * 3;
  ESP_LOGI(CAMERA_TAG, "Trying to allocate RAM: %i Free PSRAM: %i", rgb_buffer_len, ESP.getFreePsram());
  if (ESP.getFreePsram() < rgb_buffer_len)
  {
    esp_camera_fb_return(fb);
    ESP_LOGE(CAMERA_TAG, "Camera capture failed");
    return ESP_ERR_NO_MEM;
  }

  rgb_buffer = (uint8_t *)ps_malloc(rgb_buffer_len * sizeof(uint8_t));
  err = fmt2rgb888(fb->buf, fb->len, fb->format, rgb_buffer);
  esp_camera_fb_return(fb);
  if (!err)
  {
    free(rgb_buffer);
    ESP_LOGE(CAMERA_TAG, "Unable to transfer jpeg to rgb");
    return ESP_FAIL;
  }

  // TODO: Find central mediana color and convert it to lego_color_t
  size_t i = w / 2 + h/2*w;
  i *= 3;

  ESP_LOGI(CAMERA_TAG, "Pixel x:%i y:%i", (i / 3) % w, (i / 3) / w);
  uint8_t B, G, R;
  B = rgb_buffer[i];
  G = rgb_buffer[i + 1];
  R = rgb_buffer[i + 2];

  ESP_LOGI(CAMERA_TAG, "Color: %u %u %u", R, G, B);

  HslColor color = HslColor(R, G, B);
  ESP_LOGI(CAMERA_TAG, "HSL: %f %f %f", color.GetHue(), color.GetSaturation(), color.GetLightness());

  result = color.convertToLego();

  free(rgb_buffer);
  return ESP_OK;
}

esp_err_t searchForLineWithColor(const lego_color_t search, int &rotationFromCenter)
{
  return esp_err_t();
}
