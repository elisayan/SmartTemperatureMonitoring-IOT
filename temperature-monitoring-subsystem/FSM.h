#ifndef FSM_H
#define FSM_H

#include "Config.h"

enum SystemState { NORMAL, HOT, TOO_HOT, ALARM };

class FSM {
public:
  SystemState getState(float temp);
  unsigned long getSamplingInterval();
  bool checkAlarmCondition(unsigned long startTime);

private:
  SystemState currentState = NORMAL;
};

#endif