#include "ButtonTask.h"
#include "Arduino.h"

ButtonTask::ButtonTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
}

void ButtonTask::tick() {
  pPlant->checkButtonState();
}
