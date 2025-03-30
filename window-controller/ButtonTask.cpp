#include "ButtonTask.h"
#include "Arduino.h"

ButtonTask::ButtonTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
  this->lastState = LOW;
  this->lastPressTime = 0;
}

void ButtonTask::tick() {
  bool currentState = pPlant->isButtonPressed();

  if (currentState && !lastState && (millis() - lastPressTime) > 200) {
    pPlant->handleButtonPress();
    lastPressTime = millis();

    Serial.println("SOURCE:ARDUINO");
    Serial.print("MODE:");
    Serial.println(pPlant->isInManualMode() ? "MANUAL" : "AUTOMATIC");
  }

  lastState = currentState;
}
