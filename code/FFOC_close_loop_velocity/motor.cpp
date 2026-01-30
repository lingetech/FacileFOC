#include "motor.h"
#include <Arduino.h>


//构造函数
Motor::Motor(int motor_pole_pairs, float voltage_power_supply, int pin_enable, int pin_PWM_A, int pin_PWM_B, int pin_PWM_C):
  motor_pole_pairs(motor_pole_pairs), voltage_power_supply(voltage_power_supply),
  pin_enable(pin_enable), pin_PWM_A(pin_PWM_A), pin_PWM_B(pin_PWM_B), pin_PWM_C(pin_PWM_C){

}

//初始化
void Motor::init(){
  //PWM设置
  pinMode(pin_PWM_A, OUTPUT);
  pinMode(pin_PWM_B, OUTPUT);
  pinMode(pin_PWM_C, OUTPUT);
  ledcAttach(pin_PWM_A, 3000, 8);  //pwm频道, 频率, 精度
  ledcAttach(pin_PWM_B, 3000, 8);  //pwm频道, 频率, 精度
  ledcAttach(pin_PWM_C, 3000, 8);  //pwm频道, 频率, 精度
  //Enable电机驱动器
  pinMode(pin_enable, OUTPUT);
  digitalWrite(pin_enable, HIGH);
}