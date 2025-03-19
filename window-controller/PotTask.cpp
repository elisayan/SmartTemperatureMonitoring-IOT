#include "PotTask.h"
#include "Arduino.h"

PotTask::PotTask(int p, int& pos, bool& mode, MsgServiceClass& service)
  : pin(p), windowPosition(pos), manualMode(mode), msgService(service) {}

void PotTask::init(int period) {
  Task::init(period);
}

void PotTask::tick() {
  if (manualMode) {
    int val = analogRead(pin);
    windowPosition = map(val, 0, 1023, 0, 90);
  }
}