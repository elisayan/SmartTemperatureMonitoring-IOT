#include "PotTask.h"
#include "Arduino.h"

PotTask::PotTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
}

void PotTask::tick() {
  if (pPlant->isInManualMode()) {
    int potValue = pPlant->readPotentiometer();
    int windowPos = map(potValue, 0, 1023, 0, 100);
    pPlant->setWindowOpening(windowPos);
    Serial.print("POS:");
    Serial.println(windowPos);
  }
}