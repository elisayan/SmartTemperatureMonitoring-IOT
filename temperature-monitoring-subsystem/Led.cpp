#include "Led.h"
#include <Arduino.h>

Led::Led(int p) : pin(p) { pinMode(pin, OUTPUT); }

void Led::on() { 
  digitalWrite(pin, HIGH);
  blinking = false;
}

void Led::off() { 
  digitalWrite(pin, LOW);
  blinking = false;
}

void Led::blink(unsigned long interval) {
  blinkInterval = interval;
  blinking = true;
  lastToggle = millis();
}

void Led::update() {
  if(blinking && (millis() - lastToggle > blinkInterval)) {
    digitalWrite(pin, !digitalRead(pin));
    lastToggle = millis();
  }
}