#include <Esp.h>
#include <esp_camera.h>
#include <esp_err.h>
#include "camera_algorithms.h"

const size_t FRAME_COUNT = 5;
const size_t SEARCH_SQUARE_SIZE = 12;

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

void getAverage(uint8_t *rgb_buffer, size_t rgb_buffer_len, size_t W, size_t H, uint8_t &R, uint8_t &G, uint8_t &B)
{
  uint rSum = 0;
  uint gSum = 0;
  uint bSum = 0;
  size_t pixel_count = 0;
  for (size_t y = (H - SEARCH_SQUARE_SIZE) / 2; y < (H + SEARCH_SQUARE_SIZE) / 2; y++)
    for (size_t x = (W - SEARCH_SQUARE_SIZE) / 2 * 3; x < (W + SEARCH_SQUARE_SIZE) / 2 * 3; x += 3)
    {
      size_t i = x + y * W * 3;
      bSum += rgb_buffer[i];
      gSum += rgb_buffer[i + 1];
      rSum += rgb_buffer[i + 2];
      pixel_count++;
    }
  R = (uint8_t)(rSum / pixel_count);
  G = (uint8_t)(gSum / pixel_count);
  B = (uint8_t)(bSum / pixel_count);
}

esp_err_t getColorOfObject(lego_color_t &result)
{
  camera_fb_t *fb = NULL;
  esp_err_t err = ESP_OK;
  uint8_t *rgb_buffer = NULL;
  long startTime = esp_timer_get_time();
  // Initialise rgb buffer
  fb = esp_camera_fb_get();
  if (!fb)
  {
    ESP_LOGE(CAMERA_TAG, "Camera capture failed");
    esp_camera_fb_return(fb);
    return ESP_FAIL;
  }

  const size_t W = fb->width;
  const size_t H = fb->height;
  const size_t rgb_buffer_len = fb->width * fb->height * 3;
  esp_camera_fb_return(fb);
  if (ESP.getFreePsram() < rgb_buffer_len)
  {
    ESP_LOGE(CAMERA_TAG, "Unable to allocate enought PSRAM");
    return ESP_ERR_NO_MEM;
  }

  uint8_t R, G, B; // for each frame
  rgb_buffer = (uint8_t *)ps_malloc(rgb_buffer_len * sizeof(uint8_t));

  lego_color_t *results = (lego_color_t *)malloc(FRAME_COUNT * sizeof(lego_color_t));
  // capture many photos to be sure about everything
  for (size_t N = 0; N < FRAME_COUNT; N++)
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
    err = fmt2rgb888(fb->buf, fb->len, fb->format, rgb_buffer);
    esp_camera_fb_return(fb);
    if (!err)
    {
      free(rgb_buffer);
      free(results);
      ESP_LOGE(CAMERA_TAG, "Unable to transfer jpeg to rgb");
      return ESP_FAIL;
    }

    // TODO: average color in center
    getAverage(rgb_buffer, rgb_buffer_len, W, H, R, G, B);

    HslColor color = HslColor(R, G, B);

    lego_color_t clr = color.convertToLego();
    results[N] = clr;

    ESP_LOGD(CAMERA_TAG, "Received color HSV: (%f %f %f) and Prediction: [%u]", color.GetHue(), color.GetSaturation(), color.GetLightness(), clr);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  result = (lego_color_t)mostFrequent(results, FRAME_COUNT);

  free(results);
  free(rgb_buffer);

  int completion_time = (int)((esp_timer_get_time() - startTime) / 1000);
  ESP_LOGI(CAMERA_TAG, "Color Prediction [%u] in %ims", result, completion_time);

  return ESP_OK;
}

esp_err_t getFastLine(lego_color_t &result)
{
  result = COLOR_NONE;
  camera_fb_t *fb = NULL;
  esp_err_t err = ESP_OK;
  uint8_t *rgb_buffer = NULL;
  long startTime = esp_timer_get_time();

  // Initialise rgb buffer
  fb = esp_camera_fb_get();
  if (!fb)
  {
    ESP_LOGE(CAMERA_TAG, "Camera capture failed");
    esp_camera_fb_return(fb);
    return ESP_FAIL;
  }

  const size_t W = fb->width;
  const size_t H = fb->height;
  const size_t rgb_buffer_len = fb->width * fb->height * 3;
  if (ESP.getFreePsram() < rgb_buffer_len)
  {
    esp_camera_fb_return(fb);
    ESP_LOGE(CAMERA_TAG, "Unable to allocate enought PSRAM");
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

  size_t i = (W / 2 + (H - 1) * W) * 3;
  uint8_t R = rgb_buffer[i + 2];
  uint8_t G = rgb_buffer[i + 1];
  uint8_t B = rgb_buffer[i];

  free(rgb_buffer);
  HslColor color = HslColor(R, G, B);
  result = color.convertToLego();

  int completion_time = (int)((esp_timer_get_time() - startTime) / 1000);
  ESP_LOGI(CAMERA_TAG, "Color Prediction [%u] in %ims", result, completion_time);

  return ESP_OK;
}

esp_err_t getLines(lego_color_t *result, size_t lenght)
{
  return esp_err_t();
}
