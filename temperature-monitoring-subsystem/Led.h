#ifndef LED_H
#define LED_H

class Led {
public:
  Led(int pin);
  void on();
  void off();
  void blink(unsigned long interval);
  void update();
  
private:
  int pin;
  bool blinking = false;
  unsigned long lastToggle = 0;
  unsigned long blinkInterval = 0;
};

#endif