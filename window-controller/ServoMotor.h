#ifndef __SERVO_MOTOR__
#define __SERVO_MOTOR__

#include "Window.h"
#include "Config.h"
#include <ServoTimer2.h>

class ServoMotor : public Window {
public:
  ServoMotor(int pin);
  void on() override;
  void setPosition(int percentage) override;
  void off() override;
  
  int getCurrentPercentage();

private:
  int pin;
  ServoTimer2* motor;
  int currentPercentage = 0;
};

#endif