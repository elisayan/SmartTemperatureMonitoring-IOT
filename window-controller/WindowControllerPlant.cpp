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
  arduinoMode = "AUTOMATIC";
  manualSource = "ARDUINO";
  pLcd->welcome();
  pLcd->updateAutoData(currentOpening);
  pMotor->on();
  Serial.print("MODE:");
  Serial.println(isInManualMode() ? "MANUAL" : "AUTOMATIC");
  state = INIT;
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

void WindowControllerPlant::setManualSource(String source) {
  manualSource = source;
}

String WindowControllerPlant::getManualSource() {
  return manualSource;
}

int WindowControllerPlant::readPotentiometer() {
  return pPot->getValue();
}

void WindowControllerPlant::setWindowOpening(int percentage) {
  pMotor->setPosition(percentage);
  currentOpening = percentage;
  updateDisplay();
}

void WindowControllerPlant::checkButtonState() {
  pButton->sync();
  if (pButton->isClicked()) {
    String newMode = (arduinoMode == "MANUAL") ? "AUTOMATIC" : "MANUAL";
    handleModeChange(newMode);
    setManualSource("ARDUINO");

    Serial.print("MODE:");
    Serial.println(isInManualMode() ? "MANUAL" : "AUTOMATIC");
  }
}

void WindowControllerPlant::handleModeChange(String mode) {
  arduinoMode = mode;
  if (arduinoMode == "MANUAL") {
    switchToManualMode();
  } else if (arduinoMode == "AUTOMATIC") {
    switchToAutomaticMode();
  }
  updateDisplay();
}

void WindowControllerPlant::setCurrentTemperature(float temp) {
  currentTemperature = temp;
  updateDisplay();
}

void WindowControllerPlant::updateDisplay() {
  if (arduinoMode == "MANUAL") {
    pLcd->updateManualData(currentOpening, currentTemperature);
  } else if (arduinoMode == "AUTOMATIC") {
    pLcd->updateAutoData(currentOpening);
  }
}
