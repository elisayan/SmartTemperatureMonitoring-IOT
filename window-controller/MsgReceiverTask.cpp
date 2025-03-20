#include "MsgReceiverTask.h"
#include "Arduino.h"

MsgReceiverTask::MsgReceiverTask(MsgServiceClass* service, bool mode, int pos, float temp, LCDDisplayI2C* lcd) {
  this->msgService = service;
  this->lcd = lcd;
  this->manualMode = mode;
  this->currentTemperature = temp;
  this->windowPosition = pos;
}

void MsgReceiverTask::tick() {
  if (msgService->isMsgAvailable()) {
    Msg* msg = msgService->receiveMsg();
    if (msg) {
      String line = msg->getContent();
      processLine(line);
      delete msg;
    }
  }
}

void MsgReceiverTask::processLine(const String line) {
  if (line.indexOf("TEMP:") != -1) {
    currentTemperature = extractValue(line, "TEMP:").toFloat();
  }

  if (line.indexOf("POS:") != -1) {
    windowPosition = extractValue(line, "POS:").toInt();
  }

  if (line.indexOf("MODE:") != -1) {
    manualMode = extractValue(line, "MODE:") == "MANUAL";
  }
}

String MsgReceiverTask::extractValue(const String line, const String key) {
  int keyIndex = line.indexOf(key);
  if (keyIndex == -1) {
    return "";
  }

  int colonIndex = line.indexOf(':', keyIndex);
  if (colonIndex == -1) {
    return "";
  }

  int nextSpace = line.indexOf(' ', colonIndex);
  if (nextSpace == -1) {
    nextSpace = line.length();
  }

  String value = line.substring(colonIndex + 1, nextSpace);
  value.trim();
  return value;
}
