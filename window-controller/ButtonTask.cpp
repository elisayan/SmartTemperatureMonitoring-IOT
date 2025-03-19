#include "ButtonTask.h"
#include "Arduino.h"

ButtonTask::ButtonTask(int p, bool& mode)
  : pin(p), manualMode(mode), lastState(LOW), lastPressTime(0) {}

void ButtonTask::init(int period) {
  Task::init(period);
  pinMode(pin, INPUT);
}

void ButtonTask::tick() {
  bool currentState = digitalRead(pin);
  if (currentState == HIGH && lastState == LOW && (millis() - lastPressTime) > 200) {
    manualMode = !manualMode;
    lastPressTime = millis();
  }

  lastState = currentState;
}
