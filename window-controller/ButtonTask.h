#ifndef __BUTTON_TASK__
#define __BUTTON_TASK__

#include "Task.h"
#include "WindowControllerPlant.h"

class ButtonTask : public Task {
  WindowControllerPlant* pPlant;

public:
  ButtonTask(WindowControllerPlant* pPlant);
  void tick();
};

#endif
