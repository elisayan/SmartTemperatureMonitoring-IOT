#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "MsgService.h"
#include "Scheduler.h"
#include "Config.h"

Scheduler sched;
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(115200);
  MsgService.init();
  sched.init(100);

  
}

void loop() {
  sched.schedule();
}