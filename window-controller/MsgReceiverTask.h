#ifndef __MSG_RECEIVER_TASK__
#define __MSG_RECEIVER_TASK__

#include "Task.h"
#include "MsgService.h"
#include "WindowControllerPlant.h"

class MsgReceiverTask : public Task {
  WindowControllerPlant* pPlant;

  void processLine(const String line);

public:
  MsgReceiverTask(WindowControllerPlant* pPlant);
  void tick();
};

#endif