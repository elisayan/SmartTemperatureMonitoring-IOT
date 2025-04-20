#ifndef __AUTOMATIC_TASK__
#define __AUTOMATIC_TASK__

#include "Task.h"
#include "MsgService.h"
#include "WindowControllerPlant.h"

class AutomaticTask : public Task {
public:
  AutomaticTask(WindowControllerPlant* pPlant);
  void tick();

private:
  void processLine(const String line);

  enum {
    RECEIVING,
    UPDATE
  } state;

  WindowControllerPlant* pPlant;
  Msg* msg;
};

#endif