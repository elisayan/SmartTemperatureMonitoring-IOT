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

void WindowControllerPlant::setManualSource(String source) {
  manualSource = source;
}

int WindowControllerPlant::readPotentiometer() {
  return pPot->getValue();
}

void WindowControllerPlant::setWindowFromPotentiometer() {
  if (arduinoMode == "MANUAL" && manualSource == "ARDUINO") {
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
    setManualSource("ARDUINO");

    Serial.print("MODE:");
    Serial.println(isInManualMode() ? "MANUAL" : "AUTOMATIC");
  }
}

void WindowControllerPlant::handleButtonPress() {
  String newMode;
  if (arduinoMode == "MANUAL") {
    newMode = "AUTOMATIC";
  } else if (arduinoMode == "AUTOMATIC") {
    newMode = "MANUAL";
  }

  handleButtonPress(newMode);
}

void WindowControllerPlant::handleButtonPress(String mode) {
  state = BUTTON_PRESSED;
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
