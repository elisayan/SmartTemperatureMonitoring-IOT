#include <LiquidCrystal_I2C.h>
#include <Servo.h>

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

  lcd.init();
  lcd.backlight();
  lcd.print("Mode: AUTOMATIC");

  windowServo.attach(SERVO_PIN);
  windowServo.write(0);

  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("POS:")) {
      windowPosition = input.substring(4).toInt();
      windowServo.write(windowPosition);
      Serial.println(windowPosition);
    } else if (input.startsWith("TEMP:")) {
      currentTemperature = input.substring(5).toFloat();
    } else if (input.startsWith("MODE:")) {
      String mode = input.substring(5);
      lcd.setCursor(0, 0);
      lcd.print("Mode: " + mode);
    }
  }

  if (digitalRead(BUTTON_PIN) == HIGH) {
    delay(200);
    manualMode = !manualMode;
    Serial.println("button pressed");
    lcd.clear();
    lcd.print(manualMode ? "Mode: MANUAL" : "Mode: AUTOMATIC");
    delay(1000);
  }

  if (manualMode) {
    int potValue = analogRead(POT_PIN);
    windowPosition = map(potValue, 0, 1023, 0, 90);
    windowServo.write(windowPosition);
    Serial.println("Window: ", windowPosition);
  } else {
    if (Serial.available()) {
      windowPosition = Serial.parseInt();
      windowServo.write(windowPosition);
    }
  }

  lcd.setCursor(0, 1);
  lcd.print("Window: ");
  lcd.print(windowPosition);
  lcd.print("% ");

  lcd.setCursor(0, 2);
  if (manualMode) {
    lcd.print("Temp: ");
    lcd.print(currentTemperature);
    lcd.print("C");
  }
  delay(100);
}
