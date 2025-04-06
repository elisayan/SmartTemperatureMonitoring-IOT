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
  Serial.print("MODE:");
  Serial.println(isInManualMode() ? "MANUAL" : "AUTOMATIC");
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

int WindowControllerPlant::readPotentiometer() {
  return pPot->getValue();
}

void WindowControllerPlant::setWindowFromPotentiometer() {
  if (manualMode) {
    int potValue = readPotentiometer();
    int newPos = map(potValue, 0, 1023, 0, 100);

    if (newPos != currentOpening) {
      setWindowOpening(newPos);
      Serial.print("POS:");
      Serial.println(newPos);
    }
  }
}

void WindowControllerPlant::setWindowOpening(int percentage) {
  pMotor->setPosition(percentage);
  currentOpening = percentage;
  updateDisplay();
  state = MOVING_WINDOW;
}

bool WindowControllerPlant::isButtonPressed() {
  return state == BUTTON_PRESSED;
}

void WindowControllerPlant::checkButtonState() {
  pButton->sync();

  if (pButton->isClicked()) {
    handleButtonPress();

    Serial.println("SOURCE:ARDUINO");
    Serial.print("MODE:");
    Serial.println(isInManualMode() ? "MANUAL" : "AUTOMATIC");
  }
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
  if (manualMode) {
    pLcd->updateManualData(currentOpening, currentTemperature);
  } else {
    pLcd->updateAutoData(currentOpening);
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
