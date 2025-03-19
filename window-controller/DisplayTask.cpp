#include "DisplayTask.h"
#include "Arduino.h"

DisplayTask::DisplayTask(LiquidCrystal_I2C& disp, const bool& mode, const int& pos, const float& t, MsgServiceClass& service)
  : lcd(disp), manualMode(mode), windowPosition(pos), temp(t), msgService(service) {}

void DisplayTask::init(int period) {
  Task::init(period);
}

void DisplayTask::tick() {
  if (manualMode) {
    Serial.println("SOURCE:ARDUINO");
    Serial.println("MODE:MANUAL");
    Serial.println("POS:" + String(windowPosition));
  } else {
    Serial.println("MODE:AUTOMATIC");
  }

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  lcd.print(manualMode ? "MANUAL" : "AUTOMATIC");

  lcd.setCursor(0, 1);
  lcd.print("Window: ");
  lcd.print(windowPosition);
  lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("C");
}