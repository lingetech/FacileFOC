#ifndef CURRENT_SENSOR_H
#define CURRENT_SENSOR_H


class CurrentSensor{
public:
  CurrentSensor(int pin, float sampling_resistor, float gain);    //pin：esp32 ADC引脚；sampling_resistor：采样电阻阻值；gain：采样芯片增益（放大倍数）
  void init();
  float getPhaseCurrent();
private:
  float getCurrent();
  void calibrateSensor();
  int pin;
  float sampling_resistor;
  float gain;
  float current;
  float offset_current;
};

#endif