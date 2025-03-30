#include "WindowControllerPlant.h"
#include "Config.h"

WindowControllerPlant::WindowControllerPlant() {
  pMotor = new ServoMotor(SERVO_PIN);
  pButton = new ButtonImpl(BUTTON_PIN);
  pPot = new Potentiometer(POT_PIN);
  pLcd = new LCDDisplayI2C();
}

void WindowControllerPlant::init() {
  currentOpening = 0;
  currentTemperature = 0.0;
  manualMode = false;
  pLcd->updateAutoData(currentOpening);
  pMotor->on();
  state = IDLE;
}

void WindowControllerPlant::switchToAutomaticMode() {
  state = AUTOMATIC_MODE;
}

void WindowControllerPlant::switchToManualMode() {
  state = MANUAL_MODE;
}

bool WindowControllerPlant::isInAutomaticMode() {
  return state == AUTOMATIC_MODE;
}

bool WindowControllerPlant::isInManualMode() {
  return state == MANUAL_MODE;
}

void WindowControllerPlant::setWindowOpening(int percentage) {
  pMotor->setPosition(percentage);
  currentOpening = percentage;
  updateDisplay();
  state = MOVING_WINDOW;
}

int WindowControllerPlant::readPotentiometer() {
  return pPot->getValue();
}

bool WindowControllerPlant::isButtonPressed() {
  return state == BUTTON_PRESSED;
}

void WindowControllerPlant::handleButtonPress() {
  state = BUTTON_PRESSED;
  manualMode = !manualMode;
  updateDisplay();
  if (manualMode) {
    switchToManualMode();
  } else {
    switchToAutomaticMode();
  }
}

void WindowControllerPlant::setCurrentTemperature(float temp) {
  currentTemperature = temp;
  if (isInManualMode()) {
    updateDisplay();
  }
}

void WindowControllerPlant::updateDisplay() {
  static int lastOpening = -1;
  static float lastTemp = -100;
  static bool lastMode = false;

  if (lastOpening != currentOpening || lastTemp != currentTemperature || lastMode != manualMode) {
    if (manualMode) {
      pLcd->updateManualData(currentOpening, currentTemperature);
    } else {
      pLcd->updateAutoData(currentOpening);
    }
    lastOpening = currentOpening;
    lastTemp = currentTemperature;
    lastMode = manualMode;
  }
}

int WindowControllerPlant::getState() {
  return state;
}

const char* WindowControllerPlant::getStateName() {
  switch (state) {
    case IDLE:
      return "IDLE";
    case AUTOMATIC_MODE:
      return "AUTOMATIC_MODE";
    case MANUAL_MODE:
      return "MANUAL_MODE";
    case UPDATE_DISPLAY:
      return "UPDATE_DISPLAY";
    case MOVING_WINDOW:
      return "MOVING_WINDOW";
    case BUTTON_PRESSED:
      return "BUTTON_PRESSED";
    default:
      return "UNKNOWN";
  }
}
