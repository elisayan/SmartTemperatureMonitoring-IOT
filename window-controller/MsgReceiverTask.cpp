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
    if (line.substring(5) == "MANUAL") {
      pPlant->switchToManualMode();
    } else {
      pPlant->switchToAutomaticMode();
    }
  }
}

// void MsgReceiverTask::tick() {

//   while (Serial.available() > 0) {
//     Serial.read();
//   }
//   if (MsgService.isMsgAvailable()) {
//     Msg* msg = MsgService.receiveMsg();
//     if (msg) {
//       String line = msg->getContent();
//       processLine(line);

//       if (manualMode) {
//         lcd->updateManualData(windowPosition, currentTemperature);
//       } else {
//         lcd->updateAutoData(windowPosition);
//       }

//       // char buffer[50];
//       // snprintf(buffer, sizeof(buffer), "RECEIVED: %s %d %.2f",
//       //          manualMode ? "MANUAL" : "AUTOMATIC", windowPosition, currentTemperature);
//       // Serial.println(buffer);
//       // Serial.println(currentTemperature);
//       //delete msg;
//     }

//     if (msg == nullptr) {
//       Serial.println("MsgService NULL!");
//       return;
//     }
//   }
// }

// void MsgReceiverTask::processLine(const String line) {
//   if (line.indexOf("TEMP:") != -1) {
//     currentTemperature = extractValue(line, "TEMP:").toFloat();
//   }

//   if (line.indexOf("POS:") != -1) {
//     windowPosition = extractValue(line, "POS:").toInt();
//   }

//   if (line.indexOf("MODE:") != -1) {
//     manualMode = extractValue(line, "MODE:") == "MANUAL";
//   }
// }

// String MsgReceiverTask::extractValue(const String line, const String key) {
//   int keyIndex = line.indexOf(key);
//   if (keyIndex == -1) {
//     return "";
//   }

//   int colonIndex = line.indexOf(':', keyIndex);
//   if (colonIndex == -1) {
//     return "";
//   }

//   int nextSpace = line.indexOf(' ', colonIndex);
//   if (nextSpace == -1) {
//     nextSpace = line.length();
//   }

//   String value = line.substring(colonIndex + 1, nextSpace);
//   value.trim();
//   return value;
// }
