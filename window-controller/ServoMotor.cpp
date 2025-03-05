#include "ServoMotor.h"
#include "Arduino.h"

ServoMotor::ServoMotor(int pin) {
  this->motor = new ServoTimer2();
  this->pin = pin;
}

void ServoMotor::on() {
  motor->attach(pin);
  motor->write(MIN_PULSE);
}

void ServoMotor::setPosition(int percentage) {
  if (!motor->attached()) return;

  percentage = constrain(percentage, 0, 100);
  currentPercentage = percentage;

  int degrees = map(percentage, 0, 100, 0, 90);
  int pulseWidth = map(degrees, 0, 90, MIN_PULSE, MAX_PULSE);

  motor->write(pulseWidth);
}

void ServoMotor::off() {
  if (motor->attached()) {
    motor->detach();
  }
}

int ServoMotor::getCurrentPercentage() {
  return currentPercentage;
}