#ifndef __SERVO_MOTOR__
#define __SERVO_MOTOR__

#include "Door.h"
#include <ServoTimer2.h>

class ServoMotor : public Door {

public:
  ServoMotor(int pin);

  void on();
  void setPosition(int angle);
  void off();

private:
  int pin;
  ServoTimer2 motor;
};

#endif
