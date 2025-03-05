#include "ServoMotor.h"
#include "Arduino.h"

ServoMotor::ServoMotor(int pin) {
  this->pin = pin;
}

void ServoMotor::on() {
  motor.attach(pin);
}

void ServoMotor::setPosition(int angle) {
  if (angle > 90) {
    angle = 90;
  } else if (angle < -90) {
    angle = -90;
  }

  float coeff = (2400.0 - 544.0) / 180.0;
  int pulseWidth = 544 + (angle + 90) * coeff;

  motor.write(pulseWidth);
}

void ServoMotor::off() {
  motor.detach();
}
