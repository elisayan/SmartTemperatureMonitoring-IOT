#ifndef __BUTTON_TASK__
#define __BUTTON_TASK__

#include "Task.h"
#include "Config.h"

class ButtonTask : public Task {
  const int pin;
  bool& manualMode;
  bool lastState;
  unsigned long lastPressTime;

public:
  ButtonTask(int p, bool& mode);
  void init(int period) override;
  void tick() override;
};

#endif
