#include "PotTask.h"
#include "Arduino.h"

PotTask::PotTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
  state = LISTEN;
}

void PotTask::tick() {
  switch(state) {
    case LISTEN:
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