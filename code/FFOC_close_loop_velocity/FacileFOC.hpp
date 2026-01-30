#ifndef FACILEFOC_HPP
#define FACILEFOC_HPP

#include <Arduino.h>
#include "motor.h"
#include "encoder.h"
#include "lowpass_filter.h"
#include "current_sensor.h"
#include "PIDController.h"


float _normailizeRad(float rad);
float _electricalRad(Motor &motor, Encoder &encoder);
float filterValue(LowpassFilter &lowpass_filter, float value);
void setPwm(Motor &motor, float Ua, float Ub, float Uc);
void setPhaseVoltage(Motor &motor, float Uq, float Ud, float electrical_rad);
float calTorque(Motor &motor, PIDController &PID_controller, float target, float current);
float calIq(float current_a, float current_b, float electrical_rad);
float FFOC_readFloatFromSerial();

//无刷电机控制算法
//开环速度控制
float FFOC_openLoopVelocity(Motor &motor, float target_velocity);
//闭环位置控制
void FFOC_closeLoopPosition(Motor &motor, Encoder &encoder, PIDController &PID_controller, float target_rad);
//闭环速度控制
void FFOC_closeLoopVelocity(Motor &motor, Encoder &encoder, PIDController &velocity_PID_controller, LowpassFilter &velocity_lowpass_filter, float target_velocity);



//当amt属于[low, high]时，amt = 0
#define _deadzone(amt, low, high) ((amt)<(low)?(amt):((amt)>(high)?(amt):0))


//全局变量
float initial_electrical_rad = 0;
unsigned long previous_time_us = 0;
float shaft_rad = 0;  //计算需要转动的轴弧度
float Uq_deadzone = 0.0002;  //确定回正力矩Uq死区大小


//归一化角度到 [0,2PI]
float _normailizeRad(float rad){
  float a = fmod(rad, 2*PI);   //取余运算可以用于归一化
  return a >= 0 ? a : (a + 2*PI);  
}

//求解电角度
float _electricalRad(Motor &motor, Encoder &encoder){
  return  _normailizeRad((float)(motor.motor_pole_pairs) * encoder.getRadWithoutRotation() - initial_electrical_rad);
}

//滤波
float filterValue(LowpassFilter &lowpass_filter, float value){
  float y = lowpass_filter.filter(value);
  return y;
}

// 设置PWM到控制器输出
void setPwm(Motor &motor, float Ua, float Ub, float Uc) {

  // 限制上限
  Ua = constrain(Ua, 0.0f, motor.voltage_power_supply);
  Ub = constrain(Ub, 0.0f, motor.voltage_power_supply);
  Uc = constrain(Uc, 0.0f, motor.voltage_power_supply);
  // 计算占空比
  // 限制占空比从0到1
  float dc_a = constrain(Ua / motor.voltage_power_supply, 0.0f , 1.0f );
  float dc_b = constrain(Ub / motor.voltage_power_supply, 0.0f , 1.0f );
  float dc_c = constrain(Uc / motor.voltage_power_supply, 0.0f , 1.0f );

  //写入PWM到pwmA、pwmB、pwmC通道
  ledcWrite(motor.pin_PWM_A, dc_a*255);
  ledcWrite(motor.pin_PWM_B, dc_b*255);
  ledcWrite(motor.pin_PWM_C, dc_c*255);
}

//设置相电压：计算相电压并以PWM形式设置相电压
void setPhaseVoltage(Motor &motor, float Uq, float Ud, float electrical_rad){
  electrical_rad = _normailizeRad(electrical_rad);
  //帕克逆变换
  float Ualpha = -Uq*sin(electrical_rad); 
  float Ubeta = Uq*cos(electrical_rad); 
  //克拉克逆变换
  float Ua = Ualpha + motor.voltage_power_supply/2;
  float Ub = (sqrt(3)*Ubeta-Ualpha)/2 + motor.voltage_power_supply/2;
  float Uc = (-Ualpha-sqrt(3)*Ubeta)/2 + motor.voltage_power_supply/2;
  setPwm(motor, Ua, Ub, Uc);
}

//求解回正力矩（Uq）
float calTorque(Motor &motor, PIDController &PID_controller, float target, float current){
  //误差角度error
  float error = (current - target) * 180 / PI;
  float Uq = PID_controller.PID(error);
  Uq = _deadzone(Uq, -Uq_deadzone, Uq_deadzone);
  Uq = constrain(Uq, -motor.voltage_power_supply/2, motor.voltage_power_supply/2);
  return Uq;
}

//求解Iq
float calIq(float current_a, float current_b, float electrical_rad){
  //克拉克变换
  float Ialpha = current_a;
  float Ibeta = 1/sqrt(3)*current_a + 2/sqrt(3)*current_b;
  //帕克变换
  float Iq = Ibeta*cos(electrical_rad) - Ialpha*sin(electrical_rad);
  //float Id = Ialpha*cos(electrical_rad) + Ibeta*sin(electrical_rad);
  return Iq;
}

float FFOC_readFloatFromSerial(){
  float value = NAN;
  if (Serial.available() > 0) {  // 检查是否有数据可读
    String input = Serial.readStringUntil('\n'); // 读取直到换行符
    input.trim(); // 去除首尾空白字符
    value = input.toFloat();
  }
  return value;
}


//无刷电机控制算法
//开环速度控制
float FFOC_openLoopVelocity(Motor &motor, float target_velocity){
  unsigned long current_time_us = micros();
  //计算时间差。无符号减法自动处理 micros() 溢出
  float delta_time = (current_time_us - previous_time_us) * 1e-6f;
  if(delta_time <= 0 || delta_time > 0.5f){
    delta_time = 1e-3f;
  }
  //计算需要转动的轴弧度
  shaft_rad = _normailizeRad(shaft_rad + target_velocity*delta_time);
  float Uq = motor.voltage_power_supply / 3;  //固定Uq为电机额定电压的1/3。Uq最大只能设置为电机额定电压的1/2，否则ua,ub,uc会超出供电电压限幅。
  float Ud = 0;
  setPhaseVoltage(motor, Uq, Ud, shaft_rad * motor.motor_pole_pairs);
  previous_time_us = current_time_us;
  return Uq;
}

//闭环位置控制
void FFOC_closeLoopPosition(Motor &motor, Encoder &encoder, PIDController &position_PID_controller, float target_rad){
  float current_rad = encoder.getRad();
  Serial.println(current_rad);
  float Uq = calTorque(motor, position_PID_controller, target_rad, current_rad);
  float Ud = 0;
  setPhaseVoltage(motor, Uq, Ud, _electricalRad(motor, encoder));
}

//闭环速度控制
void FFOC_closeLoopVelocity(Motor &motor, Encoder &encoder, PIDController &velocity_PID_controller, LowpassFilter &velocity_lowpass_filter, float target_velocity){
  float current_velocity = encoder.getVelocity();
  current_velocity = velocity_lowpass_filter.filter(current_velocity);
  float Uq = calTorque(motor, velocity_PID_controller, target_velocity, current_velocity);
  float Ud = 0;
  setPhaseVoltage(motor, Uq, Ud, _electricalRad(motor, encoder));
}

#endif