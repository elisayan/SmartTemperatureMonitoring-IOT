#ifndef __BUTTON_TASK__
#define __BUTTON_TASK__

#include "Task.h"
#include "MsgService.h"
#include "Config.h"

class ButtonTask : public Task {
  const int pin;
  bool& manualMode;
  MsgServiceClass& msgService;
  bool lastState;
  unsigned long lastPressTime;

public:
  ButtonTask(int p, bool& mode, MsgServiceClass& service);
  void init(int period) override;
  void tick() override;
};

#endif
