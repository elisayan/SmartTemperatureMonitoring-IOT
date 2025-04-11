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
  if (line.startsWith("TEMP:")) {
    pPlant->setCurrentTemperature(line.substring(5).toFloat());
  } else if (line.startsWith("POS:")) {
    pPlant->setWindowOpening(line.substring(4).toInt());
  } else if (line.startsWith("MODE:")) {
    String mode = line.substring(5);
    if (mode.equals("MANUAL")) {
      pPlant->switchToManualMode();
    } else if (mode.equals("AUTOMATIC")) {
      pPlant->switchToAutomaticMode();
    }
  }
}