#include "LCDDisplayI2C.h"
#include <Arduino.h>

LCDDisplayI2C::LCDDisplayI2C()
  : lcd(0x27, 20, 4) {
  lcd.init();
  lcd.backlight();
}

void LCDDisplayI2C::welcome() {
  lcd.clear();
  
  lcd.setCursor(1, 0);
  lcd.print("  * WELCOME TO *");
  lcd.setCursor(1, 1);
  lcd.print("** SMART WINDOW **");
  lcd.setCursor(2, 2);
  lcd.print("** CONTROLLER **");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  for(int i = 0; i < 5; i++) {
    lcd.print(".");
    delay(500);
  }

  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print(" System Ready ");
  delay(2000);
  lcd.clear();
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
