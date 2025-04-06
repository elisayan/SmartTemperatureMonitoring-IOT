#include "PotTask.h"
#include "Arduino.h"

PotTask::PotTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
}

void PotTask::tick() {
  pPlant->setWindowFromPotentiometer();
}