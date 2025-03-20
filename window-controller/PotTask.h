#ifndef __POT_TASK__
#define __POT_TASK__

#include "Task.h"
#include "Config.h"

class PotTask : public Task {
  int pin;
  int windowPosition;
  bool manualMode;

public:
  PotTask(int p, int pos, bool mode);
  void tick();
};

#endif