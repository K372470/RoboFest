#include "CommandManager.h"
#include "camera_algorithms.h"
#include <SerialCommand.h>
#include <esp_err.h>

CommandManager::CommandManager(HardwareSerial *serial) : SerialCommand(serial)
{
  instance = this;
}

SerialCommand *CommandManager::instance = NULL;

void CommandManager::findBoxColor()
{
  lego_color_t result = COLOR_NONE;
  getColorOfObject(result);
  instance->HWSerial->write(result);
}

void CommandManager::searchForLineWithColor()
{
  char *arg;
  lego_color_t color;
  arg = instance->next();
  if (arg != NULL)
    color = (lego_color_t)(arg[0]);
  instance->HWSerial->print(color);
}

void CommandManager::registerCommands()
{
  this->addCommand("find_color", findBoxColor);
  this->addCommand("search_for_line", searchForLineWithColor);
}