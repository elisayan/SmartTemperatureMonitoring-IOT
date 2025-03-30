#ifndef __POT_TASK__
#define __POT_TASK__

#include "Task.h"
#include "WindowControllerPlant.h"

class PotTask : public Task {
  WindowControllerPlant* pPlant;

public:
  PotTask(WindowControllerPlant* pPlant);
  void tick();
};

#endif