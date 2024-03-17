#include <Arduino.h>
#include <esp_log.h>
#include "comunication/Ev3Communication.h"
#include "camera-functions/camera_config.h"
#include <esp_camera.h>
#include <Wire.h>
#include <esp_err.h>
#include "wifi-server/wifi_settings.h"
#include "wifi-server/CameraWebServer.h"
#include "camera-functions/camera_algorithms.h"

#define MAIN_TAG "Main"

Ev3Communication cmdManager;
CameraWebServer server;

void init_camera()
{
  esp_err_t err = esp_camera_init(&DEFAULT_CAMERA_CONFIG);
  if (err != ESP_OK)
  {
    ESP_LOGE(MAIN_TAG, "Unable to initialise camera");
    ESP.restart();
  }
  ESP_LOGI(MAIN_TAG, "Initialising camera...", ESP.getPsramSize());
  delay(5000);

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
    ESP_LOGE(MAIN_TAG, "Unable to load camera");
  else
    ESP_LOGI(MAIN_TAG, "Camera loaded corectly");
  esp_camera_fb_return(fb);
}

void setup()
{
  Serial.begin(115200, SERIAL_8N1, RX, TX);
  init_camera();

#ifndef WIFI_SSID
#warning "Try adding wifi_settings.h"
  sleep(10);
  ESP.restart();
#endif
  server.initWifi(WIFI_SSID, WIFI_PASS);

  server.begin();
  // cmdManager.begin(14,15,0x04)
}

void loop()
{
  vTaskDelete(NULL);
}