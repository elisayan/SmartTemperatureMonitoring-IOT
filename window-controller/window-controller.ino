#include "Scheduler.h"
#include "WindowControllerPlant.h"
#include "ServoMotor.h"
#include "Config.h"
#include "MsgService.h"
#include "MsgReceiverTask.h"
#include "ButtonTask.h"
#include "PotTask.h"
#include "LCDDisplayI2C.h"

Scheduler scheduler;
WindowControllerPlant* windowPlant;
LCDDisplayI2C* lcd;

void setup() {
  Serial.begin(115200);

  windowPlant = new WindowControllerPlant();

  MsgService.init();
  windowPlant->init();
  scheduler.init(100);

  Task* msgTask = new MsgReceiverTask(windowPlant);
  Task* btnTask = new ButtonTask(windowPlant);
  Task* potTask = new PotTask(windowPlant);

  msgTask->init(500);
  btnTask->init(100);
  potTask->init(1000);

  scheduler.addTask(msgTask);
  scheduler.addTask(btnTask);
  scheduler.addTask(potTask);
}

void loop() {
  scheduler.schedule();
}