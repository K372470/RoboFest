#include <Esp.h>
#include <esp_camera.h>
#include <esp_err.h>
#include "camera_algorithms.h"

const char *CAMERA_TAG = "Thinker";

lego_color_t mostFrequent(lego_color_t *arr, size_t n)
{
  // code here
  size_t maxcount = 0;
  lego_color_t element_having_max_freq;
  for (size_t i = 0; i < n; i++)
  {
    size_t count = 0;
    for (size_t j = 0; j < n; j++)
    {
      if (arr[i] == arr[j])
        count++;
    }

    if (count > maxcount)
    {
      maxcount = count;
      element_having_max_freq = arr[i];
    }
  }

  return element_having_max_freq;
}

esp_err_t getColorOfObject(lego_color_t &result)
{
  camera_fb_t *fb = NULL;
  esp_err_t ok = ESP_OK;
  uint8_t *rgb_buffer = NULL;
  size_t rgb_buffer_len;
  size_t w, h = 0;
  const size_t accuracy = 5;
  lego_color_t *results = (lego_color_t *)malloc(accuracy * sizeof(lego_color_t));
  // Initialise rgb buffer
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
  esp_camera_fb_return(fb);
  if (ESP.getFreePsram() < rgb_buffer_len)
  {
    ESP_LOGE(CAMERA_TAG, "Unable to allocate enought PSRAM");
    return ESP_ERR_NO_MEM;
  }
  rgb_buffer = (uint8_t *)ps_malloc(rgb_buffer_len * sizeof(uint8_t));
  // capture many photos to be sure about everything
  for (size_t N = 0; N < accuracy; N++)
  {

    fb = esp_camera_fb_get();
    if (!fb)
    {
      ESP_LOGE(CAMERA_TAG, "Camera capture failed");
      esp_camera_fb_return(fb);
      free(rgb_buffer);
      free(results);
      return ESP_FAIL;
    }
    ok = fmt2rgb888(fb->buf, fb->len, fb->format, rgb_buffer);
    esp_camera_fb_return(fb);
    if (!ok)
    {
      free(rgb_buffer);
      free(results);
      ESP_LOGE(CAMERA_TAG, "Unable to transfer jpeg to rgb");
      return ESP_FAIL;
    }

    // TODO: average color in center
    float rSum = 0;
    float gSum = 0;
    float bSum = 0;
    size_t pixel_count = 0;
    const size_t size = 3;
    for (size_t y = h / 2 - size; y < h / 2 + size; y++)
      for (size_t x = (w / 2 - size) * 3; x < (w / 2 + size) * 3; x += 3)
      {
        size_t i = x + y * w * 3;
        bSum += rgb_buffer[i];
        gSum += rgb_buffer[i + 1];
        rSum += rgb_buffer[i + 2];
        pixel_count++;
      }
    uint8_t R, G, B;
    R = (uint8_t)(rSum / pixel_count);
    G = (uint8_t)(gSum / pixel_count);
    B = (uint8_t)(bSum / pixel_count);

    HslColor color = HslColor(R, G, B);

    lego_color_t clr = color.convertToLego();
    results[N] = clr;

    ESP_LOGD(CAMERA_TAG, "HSV: (%f %f %f) and Prediction: [%u]", color.GetHue(), color.GetSaturation(), color.GetLightness(), clr);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  result = (lego_color_t)mostFrequent(results, accuracy);
  ESP_LOGI(CAMERA_TAG, "Color Prediction: %u", result);

  free(results);
  free(rgb_buffer);
  return ESP_OK;
}

esp_err_t searchForLineWithColor(const lego_color_t search, int &rotationFromCenter)
{
  return esp_err_t();
}
