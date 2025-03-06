#include "FSM.h"
#include "Config.h"
#include "Arduino.h"

SystemState FSM::getState(float temp) {
  if (temp < TEMP_T1) {
    currentState = NORMAL;
  } else if (temp <= TEMP_T2) {
    currentState = HOT;
  } else {
    currentState = TOO_HOT;
  }
  return currentState;
}

unsigned long FSM::getSamplingInterval() {
  switch (currentState) {
    case NORMAL: return SAMPLING_F1;
    case HOT:    return SAMPLING_F2;
    default:     return SAMPLING_F3;
  }
}

bool FSM::checkAlarmCondition(unsigned long startTime) {
  if (currentState == TOO_HOT) {
    return (millis() - startTime) >= ALARM_DT;
  }
  return false;
}