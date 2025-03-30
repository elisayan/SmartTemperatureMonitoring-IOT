#include "LCDDisplayI2C.h"
#include <Arduino.h>

LCDDisplayI2C::LCDDisplayI2C()
  : lcd(0x27, 20, 4) {
  lcd.init();
  lcd.backlight();
}

void LCDDisplayI2C::updateAutoData(int windowPosition) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Mode: AUTOMATIC");

  lcd.setCursor(0, 1);
  lcd.print("Window: ");
  lcd.print(windowPosition);
  lcd.print("%");
}

void LCDDisplayI2C::updateManualData(int windowPosition, float currentTemperature) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Mode: MANUAL");

  lcd.setCursor(0, 1);
  lcd.print("Window: ");
  lcd.print(windowPosition);
  lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("Temp: ");
  lcd.print(currentTemperature);
  lcd.print("C");
}
