#include "MsgReceiverTask.h"
#include "Arduino.h"

MsgReceiverTask::MsgReceiverTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
  state = RECEIVING;
}

void MsgReceiverTask::tick() {
  switch (state) {
    case RECEIVING:
      if (MsgService.isMsgAvailable()) {
        msg = MsgService.receiveMsg();
        if (msg) {
          state = UPDATE;
        }
      }
      break;
    case UPDATE:
      String line = msg->getContent();
      if (line.startsWith("MODE:")) {
        String mode = line.substring(5);
        pPlant->setManualSource("DASHBOARD");
        pPlant->handleModeChange(mode);
      }

      if (line.startsWith("TEMP:")) {
        float temp = line.substring(5).toFloat();
        pPlant->setCurrentTemperature(temp);
      }

      if (line.startsWith("POS:")) {
        int pos = line.substring(4).toInt();
        pPlant->setManualSource("DASHBOARD");
        pPlant->setWindowOpening(pos);
      }
      state = RECEIVING;
      break;
  }
}
