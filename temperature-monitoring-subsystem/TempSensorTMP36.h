#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

class TempSensorTMP36 {
public:
  TempSensorTMP36(int pin);
  float getTemperature();
  
private:
  int pin;
  float readVoltage();
};

#endif