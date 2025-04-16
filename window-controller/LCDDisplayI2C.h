#ifndef __LCD_DISPLAY_I2C__
#define __LCD_DISPLAY_I2C__

#include <LiquidCrystal_I2C.h>

class LCDDisplayI2C {
public:
  LCDDisplayI2C();
  void welcome();
  void updateAutoData(int windowPosition);
  void updateManualData(int windowPosition, float currentTemperature);

private:
  LiquidCrystal_I2C lcd;
};

#endif
