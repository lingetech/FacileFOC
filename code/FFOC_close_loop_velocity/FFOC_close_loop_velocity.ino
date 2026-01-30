#include <Arduino.h>
#include "FacileFOC.hpp"


//实例化对象
Motor motor(7, 12.0, 19, 17, 5, 18); //int motor_pole_pairs, float voltage_power_supply, int pin_enable, int pin_PWM_A, int pin_PWM_B, int pin_PWM_C
Encoder encoder(4, 16, 0x36, 0x0c, 0x0d, -1); //int pin_SDA, int pin_SCL, int I2C_address, int high_register_address, int low_register_address, int direction
PIDController velocity_PID_controller(0.0015, 0.15, 0.0015, 500, motor.voltage_power_supply/2);  //float P, float I, float D, float output_ramp_limit, float output_limit
LowpassFilter velocity_lowpass_filter(0.1);  //float time_constant

//全局变量
float target_velocity = NAN;
int number_of_loop = 0;

void setup() {
  Serial.begin(115200);
  motor.init();
  encoder.init();
  velocity_PID_controller.init();
  velocity_lowpass_filter.init();
  delay(3000);
}

void loop() {
  float value = FFOC_readFloatFromSerial();
  if(!isnan(value)){
    target_velocity = value;
  }
  if(!isnan(target_velocity)){
    FFOC_closeLoopVelocity(motor, encoder, velocity_PID_controller, velocity_lowpass_filter, target_velocity);
  }
  //每101次循环串口打印current_velocity
  number_of_loop++;
  if(number_of_loop >= 100){
    number_of_loop = 0;
    Serial.println(velocity_lowpass_filter.filter(encoder.getVelocity()));
  }
}
