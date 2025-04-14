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
    pPlant->setManualSource("DASHBOARD");
    pPlant->handleButtonPress();
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

  // if (line.startsWith("DATA:")) {
  //   String data = line.substring(5);

  //   data.trim();

  //   int tempStartIndex = data.indexOf("TEMP:") + 5;
  //   int tempEndIndex = data.indexOf(" POS:");
  //   String tempString = data.substring(tempStartIndex, tempEndIndex);
  //   float temp = tempString.toFloat();

  //   int posStartIndex = data.indexOf("POS:") + 4;
  //   String posString = data.substring(posStartIndex);
  //   int pos = posString.toInt();

  //   pPlant->setCurrentTemperature(temp);
  //   pPlant->setWindowOpening(pos);
  // }
}