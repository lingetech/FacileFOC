/*
close_loop_position.ino
version: 1.0
*/

/*attention：
  1.基于灯哥开源
  2.本项目基于GNU协议开源，任何修改或衍生的作品必须以相同的方式公开发布

/*note:
  1. available!!!
*/

//导入库
#include "AS5600.h"

// 定义电机极对数
#define motor_pole_pairs 7
// 定义电机电源电压
#define voltage_power_supply 12.0
// 定义编码器方向
#define encodor_direction -1 //1：逆时针；-1：顺时针。

// 定义电机Enable引脚
#define M1_EN 19
// PWM输出引脚定义
#define pwmA 17
#define pwmB 5
#define pwmC 18

//初始变量及函数定义
#define _constrain(amt, low, high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
//宏定义实现的一个约束函数,用于限制一个值的范围。
//具体来说，该宏定义的名称为 _constrain，接受三个参数 amt、low 和 high，分别表示要限制的值、最小值和最大值。该宏定义的实现使用了三元运算符，根据 amt 是否小于 low 或大于 high，返回其中的最大或最小值，或者返回原值。
//换句话说，如果 amt 小于 low，则返回 low；如果 amt 大于 high，则返回 high；否则返回 amt。这样，_constrain(amt, low, high) 就会将 amt 约束在 [low, high] 的范围内。
#define _deadzone(amt, low, high) ((amt)<(low)?(amt):((amt)>(high)?(amt):0))

float error_rad = 0; //误差角度
float initial_electricalRad = error_rad;
float target_rad = 0;
float Uq = 0, Ud = 0;
float Ualpha = 0, Ubeta = 0;
float Ua = 0, Ub = 0, Uc = 0; //电机三项电压
float dc_a = 0, dc_b = 0, dc_c = 0; //占空比

//求解回正力矩系数Kp
float Kp = voltage_power_supply/2/45; //voltage_power_supply/2：有效可调整电压，电压振幅的一半；45：最大偏转角度。
//确定回正力矩Uq死区大小
float Uq_deadzone = 0.01;

//归一化角度到 [0,2PI]
float _normailizeRad(float rad){
  float a = fmod(rad, 2*PI);   //取余运算可以用于归一化
  return a >= 0 ? a : (a + 2*PI);  
  /*
    fmode函数用于求余数，其返回值的符号与被除数相同。Ex：
    fmod(7.100000, 3.000000) = 1.100000
    fmod(-4.200000, 2.500000) = -1.700000
    fmod(10.000000, -3.500000) = 3.000000
  */
  /*三目运算符
    格式：condition ? expr1 : expr2
    其中，condition 是要求值的条件表达式，如果条件成立，则返回 expr1 的值，否则返回 expr2 的值。
  */
}

//求解需要旋转的电弧度
float _electricalRad(){
  return  _normailizeRad((float)(encodor_direction * motor_pole_pairs) * AS5600_getRad_Without_track()-initial_electricalRad);
}

//求解回正力矩
float _aligningTorque(float target_rad, float practical_angle, float aligning_torque_coefficient){
  //误差角度e
  float e = (target_rad - encodor_direction * practical_angle) * 180 / PI;
  float Uq = aligning_torque_coefficient * e;
  Uq = _deadzone(Uq, -Uq_deadzone, Uq_deadzone);
  Uq = _constrain(Uq, -6, 6);
  return Uq;
}

// 设置PWM到控制器输出
void setPwm(float Ua, float Ub, float Uc) {

  // 限制上限
  Ua = _constrain(Ua, 0.0f, voltage_power_supply);
  Ub = _constrain(Ub, 0.0f, voltage_power_supply);
  Uc = _constrain(Uc, 0.0f, voltage_power_supply);
  // 计算占空比
  // 限制占空比从0到1
  dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
  dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );

  //写入PWM到pwmA、pwmB、pwmC通道
  ledcWrite(pwmA, dc_a*255);
  ledcWrite(pwmB, dc_b*255);
  ledcWrite(pwmC, dc_c*255);
}

void setPhaseVoltage(float Uq, float Ud, float angle_el) {
  angle_el = _normailizeRad(angle_el);
  // 帕克逆变换
  Ualpha = -Uq*sin(angle_el); 
  Ubeta = Uq*cos(angle_el); 

  // 克拉克逆变换
  Ua = Ualpha + voltage_power_supply/2;
  Ub = (sqrt(3)*Ubeta-Ualpha)/2 + voltage_power_supply/2;
  Uc = (-Ualpha-sqrt(3)*Ubeta)/2 + voltage_power_supply/2;
  setPwm(Ua, Ub, Uc);
}

//从串口读弧度作为target_rad
float serialRead(){
  if(Serial.available() > 0){
    String input = Serial.readStringUntil('\n'); // 读取直到换行符
    input.trim(); // 去除首尾空白字符
    target_rad = input.toFloat();
  }
  return target_rad;
}

void setup() {
  // put your setup code here, to run once:
  //硬件初始化
  Serial.begin(115200);
  AS5600_setup();
  initial_electricalRad = _electricalRad();
  //PWM设置
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(pwmC, OUTPUT);
  ledcAttach(pwmA, 3000, 8);  //pwm频道, 频率, 精度
  ledcAttach(pwmB, 3000, 8);  //pwm频道, 频率, 精度
  ledcAttach(pwmC, 3000, 8);  //pwm频道, 频率, 精度

  Serial.println("完成PWM初始化设置");

  //Enable电机
  pinMode(M1_EN, OUTPUT);
  digitalWrite(M1_EN, HIGH);
  delay(3000);
}


void loop() {
  // put your main code here, to run repeatedly:
  float currant_rad = AS5600_getRad();
  Serial.println(currant_rad);
  target_rad = serialRead();
  Uq = _aligningTorque(target_rad, currant_rad, Kp);
  setPhaseVoltage(Uq, Ud, _electricalRad());
}