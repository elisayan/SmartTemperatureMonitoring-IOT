#ifndef __MANUAL_TASK__
#define __MANUAL_TASK__

#include "Task.h"
#include "WindowControllerPlant.h"

class ManualTask : public Task {
public:
  ManualTask(WindowControllerPlant* pPlant);
  void tick();

private:
  enum {
    LISTEN,
    UPDATE
  } state;

  WindowControllerPlant* pPlant;
  int previousValue = -1; 
};

#endif