#ifndef __DISPLAY_TASK__
#define __DISPLAY_TASK__

#include "Task.h"
#include <LiquidCrystal_I2C.h>

class DisplayTask : public Task {
  LiquidCrystal_I2C& lcd;
  const bool& manualMode;
  const int& windowPosition;
  const float& temp;

public:
  DisplayTask(LiquidCrystal_I2C& disp, const bool& mode, const int& pos, const float& t);
  void init(int period) override;
  void tick() override;
};

#endif