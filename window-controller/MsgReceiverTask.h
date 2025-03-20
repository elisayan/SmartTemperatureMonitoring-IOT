#ifndef __MSG_RECEIVER_TASK__
#define __MSG_RECEIVER_TASK__

#include "Task.h"
#include "MsgService.h"
#include "LCDDisplayI2C.h"

class MsgReceiverTask : public Task {
  bool manualMode;
  int windowPosition;
  float currentTemperature;
  LCDDisplayI2C* lcd;

  void processLine(const String line);
  String extractValue(const String line, const String key);
  void updateDisplay();

public:
  MsgReceiverTask(bool mode, int pos, float temp, LCDDisplayI2C* lcd);
  void tick();
};

#endif