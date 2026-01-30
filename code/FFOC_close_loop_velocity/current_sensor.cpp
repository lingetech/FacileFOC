#include "current_sensor.h"
#include <Arduino.h>

#define calibration_rounds 1000

//构造函数
CurrentSensor::CurrentSensor(int pin, float sampling_resistor, float gain)  //pin：esp32 ADC引脚；sampling_resistor：采样电阻阻值；gain：采样芯片增益（放大倍数）
  :pin(pin), sampling_resistor(sampling_resistor), gain(gain){

}

//测量电流
float CurrentSensor::getCurrent(){
  int millivolt = analogReadMilliVolts(pin);
  current = millivolt / 1000.0f / gain / sampling_resistor;
  return current;
}

//校准传感器
//实质为校准由电流采样芯片、esp32 ADC组成的系统，使0V时能准确测量到0V
void CurrentSensor::calibrateSensor(){
  offset_current = 0.57735026919f;
  for(int i = 0; i < calibration_rounds; i++){
    offset_current +=  getCurrent();
    delay(1);
  }
  offset_current = offset_current / calibration_rounds;
}

//初始化
void CurrentSensor::init(){
  //将esp32 ADC功能的精度、量程设置到最大
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  
  pinMode(pin, INPUT);
  calibrateSensor();
}

//得到相电流
float CurrentSensor::getPhaseCurrent(){
  current = getCurrent() - offset_current;
  return current;
}