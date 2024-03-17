#include "Ev3Communication.h"
#include <esp_err.h>
#include <camera-functions/camera_algorithms.h>
#include <Wire.h>

void Ev3Communication::onRequestCommand()
{
  switch (last_command)
  {
  case COMMAND_GET_COLOR:
  {
    lego_color_t color = COLOR_NONE;
    getColorOfObject(color);
    Wire.write(color);
    break;
  }
  case COMMAND_GET_LINES:
  {
    // TODO:
    //  lego_color_t color;
    //  getFastLine(color);
    //  Wire.write((uint8_t)color);
    break;
  }
  }
}

void Ev3Communication::onRecieveCommand(int bytes)
{
  last_command = (ev3_command_t)(Wire.read());
  ESP_LOGI("A", "New command recieved %i", (int)last_command);
}

esp_err_t Ev3Communication::begin(uint8_t sda_port, uint8_t scl_port, uint8_t address)
{
  Wire.setPins(sda_port, scl_port);
  Wire.onReceive(onRecieveCommand);
  Wire.onRequest(onRequestCommand);
  return Wire.begin(address) ? ESP_OK : ESP_FAIL;
}
