#include "LCDDisplayI2C.h"
#include <Arduino.h>

LCDDisplayI2C::LCDDisplayI2C()
  : lcd(0x27, 20, 4) {
  lcd.init();
  lcd.backlight();
}

void LCDDisplayI2C::welcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("SMART WASTE SYSTEM");
  delay(5000);
}

void LCDDisplayI2C::enterWaste() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PRESS OPEN TO ENTER WASTE");
}

void LCDDisplayI2C::pressClose() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PRESS CLOSE WHEN ");
  lcd.setCursor(0, 1);
  lcd.print("DONE");
}

void LCDDisplayI2C::wasteReceived() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WASTE RECEIVED");
}

void LCDDisplayI2C::containerFull() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONTAINER FULL");
}

void LCDDisplayI2C::problemDetected() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PROBLEM DETECTED");
}

void LCDDisplayI2C::turnOff() {
  lcd.noDisplay();
}

void LCDDisplayI2C::turnOn() {
  lcd.display();
  lcd.backlight();
  lcd.clear();
}
