#ifndef __WINDOW_CONTROLLER_PLANT__
#define __WINDOW_CONTROLLER_PLANT__

#include <Arduino.h>
#include "ServoMotor.h"
#include "LCDDisplayI2C.h"
#include "ButtonImpl.h"
#include "Potentiometer.h"

class WindowControllerPlant {
public:
  WindowControllerPlant();
  void init();

  void switchToAutomaticMode();
  void switchToManualMode();
  bool isInAutomaticMode();
  bool isInManualMode();

  void setWindowOpening(int percentage);
  int readPotentiometer();
  void setWindowFromPotentiometer();

  bool isButtonPressed();
  void checkButtonState();
  void handleButtonPress();

  void setCurrentTemperature(float temp);
  float getCurrentTemperature();

  void updateDisplay();

  int getState();
  const char* getStateName();

private:
  enum {
    IDLE,
    AUTOMATIC_MODE,
    MANUAL_MODE,
    UPDATE_DISPLAY,
    MOVING_WINDOW,
    BUTTON_PRESSED
  } state;

  ServoMotor* pMotor;
  LCDDisplayI2C* pLcd;
  ButtonImpl* pButton;
  Potentiometer* pPot;
  int currentOpening;
  float currentTemperature;
  bool manualMode;
  unsigned long lastUpdateTime;
};

#endif