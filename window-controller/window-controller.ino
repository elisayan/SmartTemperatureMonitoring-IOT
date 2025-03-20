#include <LiquidCrystal_I2C.h>
#include "Scheduler.h"
#include "ServoMotor.h"
#include "Config.h"
#include "MsgService.h"
#include "MsgReceiverTask.h"
#include "ButtonTask.h"
#include "PotTask.h"
#include "LCDDisplayI2C.h"

LCDDisplayI2C* lcd;
ServoMotor* windowServo(SERVO_PIN);

bool manualMode = false;
int windowPosition = 0;
float currentTemperature = 0.0;

Scheduler scheduler;

void setup() {
  Serial.begin(115200);
  MsgService.init();

  lcd = new LCDDisplayI2C();
  windowServo = new ServoMotor(SERVO_PIN);
  
  lcd->updateData(manualMode, windowPosition, currentTemperature);
  windowServo->on();

  Serial.print("Buffer size: ");
  Serial.println(SERIAL_RX_BUFFER_SIZE);

  scheduler.init(100);

  Task* msgTask = new MsgReceiverTask(manualMode, windowPosition, currentTemperature, lcd);
  Task* btnTask = new ButtonTask(BUTTON_PIN, manualMode);
  Task* potTask = new PotTask(POT_PIN, windowPosition, manualMode);

  msgTask->init(100);
  btnTask->init(50);
  potTask->init(200);

  scheduler.addTask(msgTask);
  scheduler.addTask(btnTask);
  scheduler.addTask(potTask);
}

void loop() {
  scheduler.schedule();
  windowServo->setPosition(windowPosition);
  delay(2000);
}