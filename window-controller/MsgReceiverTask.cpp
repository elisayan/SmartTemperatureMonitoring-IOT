#include "MsgReceiverTask.h"
#include "Arduino.h"

MsgReceiverTask::MsgReceiverTask(WindowControllerPlant* pPlant) {
  this->pPlant = pPlant;
}

void MsgReceiverTask::tick() {
  if (MsgService.isMsgAvailable()) {
    Msg* msg = MsgService.receiveMsg();
    pPlant->setManualSource("DASHBOARD");
    if (msg) {
      processLine(msg->getContent());
      delete msg;
    }
  }
}

void MsgReceiverTask::processLine(const String line) {
  if (line.startsWith("MODE:")) {
    String mode = line.substring(5);
    pPlant->handleButtonPress();
    // if(mode == "MANUAL"){
    //   pPlant->setManualSource("DASHBOARD");
    // }
  }

  if (line.startsWith("DATA:")) {
    String data = line.substring(5);

    data.trim();

    int tempStartIndex = data.indexOf("TEMP:") + 5;
    int tempEndIndex = data.indexOf(" POS:");
    String tempString = data.substring(tempStartIndex, tempEndIndex);
    float temp = tempString.toFloat();

    int posStartIndex = data.indexOf("POS:") + 4;
    String posString = data.substring(posStartIndex);
    int pos = posString.toInt();

    pPlant->setCurrentTemperature(temp);
    pPlant->setWindowOpening(pos);
  }
}