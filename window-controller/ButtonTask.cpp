#include "ButtonTask.h"
#include "Arduino.h"

ButtonTask::ButtonTask(int p, bool& mode, MsgServiceClass& service)
  : pin(p), manualMode(mode), msgService(service), lastState(LOW), lastPressTime(0) {}

void ButtonTask::init(int period) {
  Task::init(period);
  pinMode(pin, INPUT);
}

void ButtonTask::tick() {
  bool currentState = digitalRead(pin);
  if (currentState == HIGH && lastState == LOW && (millis() - lastPressTime) > 200) {
    manualMode = !manualMode;
    if (manualMode) {
      msgService.sendMsg("MODE:MANUAL");
    } else {
      msgService.sendMsg("MODE:AUTOMATIC");
    }
    msgService.sendMsg("SOURCE:Arduino");
    lastPressTime = millis();
  }
  lastState = currentState;
}
