#include "MsgReceiverTask.h"
#include "Arduino.h"

MsgReceiverTask::MsgReceiverTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
}

void MsgReceiverTask::tick() {
  if (MsgService.isMsgAvailable()) {
    Msg* msg = MsgService.receiveMsg();
    if (msg) {
      processLine(msg->getContent());
      delete msg;
    }
  }
}

void MsgReceiverTask::processLine(const String line) {
  if (line.startsWith("MODE:")) {
    String mode = line.substring(5);
    pPlant->setManualSource("DASHBOARD");
    pPlant->handleButtonPress(mode);
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
}