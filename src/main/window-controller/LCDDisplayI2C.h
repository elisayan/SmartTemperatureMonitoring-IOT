#ifndef __LCD_DISPLAY_I2C__
#define __LCD_DISPLAY_I2C__

#include <LiquidCrystal_I2C.h>

class LCDDisplayI2C {
public:
  LCDDisplayI2C();
  void welcome();
  void enterWaste();
  void pressClose();
  void wasteReceived();
  void containerFull();
  void problemDetected();
  void turnOff();
  void turnOn();

private:
  LiquidCrystal_I2C lcd;
};

#endif
