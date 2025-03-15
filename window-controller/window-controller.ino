#include <LiquidCrystal_I2C.h>
#include "Scheduler.h"
#include "ServoMotor.h"
#include "Config.h"
#include "MsgService.h"
#include "MsgReceiverTask.h"
#include "ButtonTask.h"
#include "PotTask.h"
#include "DisplayTask.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);
ServoMotor windowServo(SERVO_PIN);

bool manualMode = false;
int windowPosition = 0;
float currentTemperature = 0.0;

Scheduler scheduler;

void setup() {
  Serial.begin(115200);
  MsgService.init();

  lcd.init();
  lcd.backlight();
  windowServo.on();

  scheduler.init(100);

  Task* msgTask = new MsgReceiverTask(MsgService, manualMode, windowPosition, currentTemperature);
  Task* btnTask = new ButtonTask(BUTTON_PIN, manualMode, MsgService);
  Task* potTask = new PotTask(POT_PIN, windowPosition, manualMode, MsgService);
  Task* dispTask = new DisplayTask(lcd, manualMode, windowPosition, currentTemperature);

  scheduler.addTask(msgTask);
  scheduler.addTask(btnTask);
  scheduler.addTask(potTask);
  scheduler.addTask(dispTask);

  msgTask->init(100);
  btnTask->init(200);
  potTask->init(300);
  dispTask->init(1000);
}

void loop() {
  scheduler.schedule();
  windowServo.setPosition(windowPosition);
  delay(50);
}