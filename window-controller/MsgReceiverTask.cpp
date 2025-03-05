#include "MsgReceiverTask.h"
#include "Arduino.h"

MsgReceiverTask::MsgReceiverTask(MsgServiceClass& service, bool& mode, int& pos, float& temp)
  : msgService(service), manualMode(mode), windowPosition(pos), currentTemperature(temp) {}

void MsgReceiverTask::init(int period) {
  Task::init(period);
}

void MsgReceiverTask::tick() {
  if (msgService.isMsgAvailable()) {
    Msg* msg = msgService.receiveMsg();
    String content = msg->getContent();

    if (content.startsWith("POS:")) {
      windowPosition = content.substring(4).toInt();
    } else if (content.startsWith("TEMP:")) {
      currentTemperature = content.substring(5).toFloat();
    } else if (content.startsWith("MODE:")) {
      manualMode = (content.substring(5) == "MANUAL");
    }
    delete msg;
  }
}