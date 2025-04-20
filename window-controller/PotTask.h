#ifndef __POT_TASK__
#define __POT_TASK__

#include "Task.h"
#include "WindowControllerPlant.h"

class PotTask : public Task {
public:
  PotTask(WindowControllerPlant* pPlant);
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