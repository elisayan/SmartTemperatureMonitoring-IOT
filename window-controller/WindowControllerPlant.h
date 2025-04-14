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
  void setManualSource(String source);

  void setWindowOpening(int percentage);
  int readPotentiometer();
  void setWindowFromPotentiometer();

  bool isButtonPressed();
  void checkButtonState();
  void handleButtonPress();
  void handleButtonPress(String mode);

  void setCurrentTemperature(float temp);
  float getCurrentTemperature();

  void updateDisplay();

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
  String arduinoMode;
  unsigned long lastUpdateTime;
  String manualSource;
};

#endif