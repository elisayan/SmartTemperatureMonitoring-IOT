#include "Scheduler.h"
#include "WindowControllerPlant.h"
#include "ServoMotor.h"
#include "Config.h"
#include "MsgService.h"
#include "AutomaticTask.h"
#include "ButtonTask.h"
#include "ManualTask.h"
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

  Task* autoTask = new AutomaticTask(windowPlant);
  Task* manualTask = new ManualTask(windowPlant);

  autoTask->init(500);
  manualTask->init(1000);

  scheduler.addTask(autoTask);
  scheduler.addTask(manualTask);
}

void loop() {
  scheduler.schedule();
}