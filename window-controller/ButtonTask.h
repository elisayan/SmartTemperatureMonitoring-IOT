#ifndef __BUTTON_TASK__
#define __BUTTON_TASK__

#include "Task.h"
#include "Config.h"

class ButtonTask : public Task {
  int pin;
  bool manualMode;
  bool lastState;
  unsigned long lastPressTime;

public:
  ButtonTask(int p, bool mode);
  void tick();

};

#endif
