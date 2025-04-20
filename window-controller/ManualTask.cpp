#include "ManualTask.h"
#include "Arduino.h"

ManualTask::ManualTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
  state = LISTEN;
}

void ManualTask::tick() {
  switch(state) {
    case LISTEN:
      pPlant->checkButtonState();
      if(pPlant->isInManualMode()){
        state = UPDATE;
      }
      break;

    case UPDATE:
      int potValue = pPlant->readPotentiometer();
      int newPos = map(potValue, 0, 1023, 0, 100);
      
      if (pPlant->isInManualMode() && newPos != previousValue && pPlant->getManualSource() == "ARDUINO") {
        previousValue = newPos;
        pPlant->setWindowOpening(newPos);
        Serial.print("POS:");
        Serial.println(newPos);
      }
      state = LISTEN;
      break;
  }
}