#include "Potentiometer.h"
#include "Config.h"
#include "Arduino.h"

Potentiometer::Potentiometer(int pin) {
    this->pin = pin;
}

int Potentiometer::getValue() {
    return analogRead(POT_PIN);
}
