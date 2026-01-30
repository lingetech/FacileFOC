#include "PIDController.h"
#include <Arduino.h>


//构造函数
PIDController::PIDController(float P, float I, float D, float output_ramp_limit, float output_limit):
  P(P), I(I), D(D), output_ramp_limit(output_ramp_limit), output_limit(output_limit){

  }

//初始化
void PIDController::init(){
  previous_error = 0.0f;
  previous_integral = 0.0f;
  previous_output = 0.0f;

  previous_time_us = micros();
}

//PID
float PIDController::PID(float error){
  current_time_us = micros();
  float delta_time = (current_time_us - previous_time_us) / 1000000.0f;
  if(delta_time <= 0 || delta_time > 0.5f){
    delta_time = 1e-3f;
  }
  //P环
  float proportional = P * error;
  // Tustin 散点积分（I环）
  float integral = previous_integral + I * delta_time * 0.5f * (error + previous_error);
  integral = constrain(integral, -output_limit, output_limit);
  //D环
  float derivative = D * (error - previous_error) / delta_time;
  //计算输出
  float output = proportional + integral + derivative;
  output = constrain(output, -output_limit, output_limit);
  //限制output变化率
  if(output_ramp_limit != NAN){
    float output_ramp = (output - previous_output) / delta_time;
    if(output_ramp > output_ramp_limit){
      output = previous_output + output_ramp_limit * delta_time;
    }else if (output_ramp < -output_ramp_limit) {
    output = previous_output - output_ramp_limit * delta_time;
    }
  }
  //保存值
  previous_error = error;
  previous_integral = integral;
  previous_output = output;
  previous_time_us = current_time_us;

  return output;
}