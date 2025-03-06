#include "TempSensorTMP36.h"
#include <Arduino.h>

TempSensorTMP36::TempSensorTMP36(int p) : pin(p) {}

float TempSensorTMP36::getTemperature() {
  int raw = analogRead(pin);
  float voltage = raw * (3.3 / 4095.0);
  return (voltage - 0.5) * 100;
}