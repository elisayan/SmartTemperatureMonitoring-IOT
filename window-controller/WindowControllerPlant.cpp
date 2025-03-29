#include "WindowControllerPlant.h"
#include "Config.h"

WindowControllerPlant::WindowControllerPlant() {
  pMotor = new ServoMotor(SERVO_PIN);
  pButton = new ButtonImpl(BUTTON_PIN);
  pPot = new Potentiometer(POT_PIN);
}

void WindowControllerPlant::init() {
  pMotor->on();
  currentOpening = 0;
  manualMode = false;
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
  if (manualMode) {
    switchToManualMode();
  } else {
    switchToAutomaticMode();
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
