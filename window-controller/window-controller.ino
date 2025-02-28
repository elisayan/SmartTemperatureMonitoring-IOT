#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "MsgService.h"

#define SERVO_PIN 9
#define BUTTON_PIN 7
#define POT_PIN A0

LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo windowServo;

bool manualMode = false;
int windowPosition = 0;
float currentTemperature = 0.0;

void setup() {
  Serial.begin(9600);
  MsgService.init();

  lcd.init();
  lcd.backlight();
  lcd.print("Mode: AUTOMATIC");

  windowServo.attach(SERVO_PIN);
  windowServo.write(0);

  pinMode(BUTTON_PIN, INPUT);
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  lcd.print(manualMode ? "MANUAL" : "AUTOMATIC");

  lcd.setCursor(0, 1);
  lcd.print("Window: ");
  lcd.print(windowPosition);
  lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("Temp: ");
  lcd.print(currentTemperature);
  lcd.print("C");
}

void loop() {
  if (MsgService.isMsgAvailable()) {
    Msg* msg = MsgService.receiveMsg();
    String content = msg->getContent();
    
    if (content.startsWith("POS:")) {
      windowPosition = content.substring(4).toInt();
      windowServo.write(windowPosition);
    } else if (content.startsWith("TEMP:")) {
      currentTemperature = content.substring(5).toFloat();
    } else if (content.startsWith("MODE:")) {
      String mode = content.substring(5);
      manualMode = (mode == "MANUAL");
    }

    delete msg;
    updateDisplay();
  }

  if (digitalRead(BUTTON_PIN) == HIGH) {
    delay(200);
    manualMode = !manualMode;
    MsgService.sendMsg("MODE:" + (manualMode ? "MANUAL" : "AUTOMATIC"));
    updateDisplay();
    delay(1000);
  }

  if (manualMode) {
    int potValue = analogRead(POT_PIN);
    windowPosition = map(potValue, 0, 1023, 0, 90);
    windowServo.write(windowPosition);
    MsgService.sendMsg("POS:" + String(windowPosition));
    updateDisplay();
  }

  delay(100);
}