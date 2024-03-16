#include <SerialCommand.h>
class CommandManager : public SerialCommand
{
private:
  static SerialCommand *instance;
  static void findBoxColor();
  static void searchForLineWithColor();

public:
  CommandManager(HardwareSerial *serial);
  void registerCommands();
};