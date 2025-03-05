#ifndef __POT_TASK__
#define __POT_TASK__

#include "Task.h"
#include "Config.h"
#include "MsgService.h"

class PotTask : public Task {
  const int pin;
  int& windowPosition;
  bool& manualMode;
  MsgServiceClass& msgService;

public:
  PotTask(int p, int& pos, bool& mode, MsgServiceClass& service);
  void init(int period) override;
  void tick() override;
};

#endif