/*
lowpass_filter.cpp
version: 1.0
*/

/*attention：
  1.基于灯哥开源
  2.本项目基于GNU协议开源，任何修改或衍生的作品必须以相同的方式公开发布

/*note:
  1. not available!!!
*/

#include "lowpass_filter.h"
#include <Arduino.h>


//构造函数
LowpassFilter::LowpassFilter(float time_constant):
  time_constant(time_constant){

}

//初始化
void LowpassFilter::init(){
  previous_time_us = micros();
  previous_y = 0;
}

//计算滤波后结果
float LowpassFilter::filter(float x){
  current_time_us = micros();
  float delta_time = (current_time_us - previous_time_us) / 1000000.0;

  if(delta_time < 0.0f){
    delta_time = 1e-3f;
  }
  //若delta_time过大，则previous_y参考意义小，则忽略previous_y
  if(delta_time > 0.3f){
    current_y = x;
  }else{
    float alpha = time_constant / (time_constant + delta_time);
    current_y = alpha * previous_y + (1.0f - alpha) * x;
  }
  previous_time_us =  current_time_us;
  previous_y = current_y;
  return current_y;
}
