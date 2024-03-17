#include "ev3_command.h"
#include <esp_err.h>
class Ev3Communication
{
private:
  static void onRequestCommand();
  static void onRecieveCommand(int bytes);
  static ev3_command_t last_command;

public:
  static esp_err_t begin(uint8_t sda_port, uint8_t scl_port, uint8_t address);
};