#include "PotTask.h"
#include "Arduino.h"

PotTask::PotTask(int p, int pos, bool mode){
  this->pin = p;
  this->windowPosition = pos;
  this->manualMode = mode;
}

void PotTask::tick() {
  if (manualMode) {
    int val = analogRead(pin);
    windowPosition = map(val, 0, 1023, 0, 90);
    Serial.println("POS:"+windowPosition);
  }
}