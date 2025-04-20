#ifndef __MSG_RECEIVER_TASK__
#define __MSG_RECEIVER_TASK__

#include "Task.h"
#include "MsgService.h"
#include "WindowControllerPlant.h"

class MsgReceiverTask : public Task {
public:
  MsgReceiverTask(WindowControllerPlant* pPlant);
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