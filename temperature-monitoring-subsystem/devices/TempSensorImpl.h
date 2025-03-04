#ifndef __TEMP_SENSOR_TMP36__
#define __TEMP_SENSOR_TMP36__

class TempSensorTMP36: public TempSensor {

public:
  TempSensorTMP36(int pin);	
  virtual float getTemperature();
  
private:
  int pin;
};


#endif

