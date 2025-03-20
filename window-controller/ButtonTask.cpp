#include "ButtonTask.h"
#include "Arduino.h"

ButtonTask::ButtonTask(int p, bool mode) {
  this->pin = p;
  this->manualMode = mode;
  pinMode(pin, INPUT);
}

void ButtonTask::tick() {
  bool currentState = digitalRead(pin);
  if (currentState == HIGH && lastState == LOW && (millis() - lastPressTime) > 200) {
    manualMode = !manualMode;
    lastPressTime = millis();

    Serial.println("SOURCE:ARDUINO");
    if (manualMode) {
      Serial.println("MODE:MANUAL");
    } else {
      Serial.println("MODE:AUTOMATIC");
    }
  }

  lastState = currentState;
}
