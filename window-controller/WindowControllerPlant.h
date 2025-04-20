#ifndef __WINDOWCONTROLLERPLANT__
#define __WINDOWCONTROLLERPLANT__

#include <Arduino.h> 

#include "ServoMotor.h"
#include "ButtonImpl.h"
#include "Potentiometer.h"
#include "LCDDisplayI2C.h"

enum PlantState {
  INIT,
  AUTOMATIC_MODE,
  MANUAL_MODE
};

class WindowControllerPlant {
public:
  WindowControllerPlant();

  void init();
  void switchToAutomaticMode();
  void switchToManualMode();

  bool isInAutomaticMode();
  bool isInManualMode();

  void setManualSource(String source);
  String getManualSource();

  int readPotentiometer();
  void setWindowOpening(int percentage);

  void setCurrentTemperature(float temp);
  void handleModeChange(String mode);
  void checkButtonState();

private:
  ServoMotor* pMotor;
  Button* pButton;
  Potentiometer* pPot;
  LCDDisplayI2C* pLcd;

  int currentOpening;
  float currentTemperature;
  String arduinoMode;
  String manualSource;

  PlantState state;

  void updateDisplay();
};

#endif
