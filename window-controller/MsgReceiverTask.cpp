#include "MsgReceiverTask.h"
#include "Arduino.h"

MsgReceiverTask::MsgReceiverTask(MsgServiceClass& service, bool& mode, int& pos, float& temp)
  : msgService(service), manualMode(mode), windowPosition(pos), currentTemperature(temp) {}

void MsgReceiverTask::init(int period) {
  Task::init(period);
}

void MsgReceiverTask::tick() {
  while (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    processLine(line);
  }
}

void MsgReceiverTask::processLine(const String& line) {
  if (line.startsWith("TEMP:")) {
    currentTemperature = line.substring(5).toFloat();
  } else if (line.startsWith("POS:")) {
    windowPosition = line.substring(4).toInt();
  } else if (line.startsWith("MODE:")) {
    manualMode = (line.substring(5) == "MANUAL");
  }
}