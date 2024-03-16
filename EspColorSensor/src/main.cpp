#include <Arduino.h>
#include <esp32-hal-log.h>
#include "CameraWebServer.h"
#include "CommandManager.h"
#include "SerialCommand.h"
#include "camera_config.h"
#include <esp_camera.h>
#include "camera_algorithms.h"
#include "wifi_settings.h"

const char *MAIN_TAG = "Main";

// CommandManager cmdManager(&Serial);
// CameraWebServer server;

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

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb)
    ESP_LOGE(MAIN_TAG, "Camera capture failed");
  else
    ESP_LOGI(MAIN_TAG,"Success");
  esp_camera_fb_return(fb);

}

void setup()
{
  Serial.begin(115200, SERIAL_8N1, RX, TX);
  init_camera();
#ifndef WIFI_SSID
#warning "Try adding wifi_settings.h"
#endif
  // server.initWifi(WIFI_SSID, WIFI_PASS);
  // server.begin();

  // cmdManager.registerCommands();
}

void loop()
{
  // cmdManager.readSerial();
  lego_color_t color;
  getColorOfObject(color);
  Serial.println(color);
  delay(100);
}