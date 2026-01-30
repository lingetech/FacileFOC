#include <Arduino.h>
#include "FacileFOC.hpp"


//实例化对象
Motor motor(7, 12.0, 19, 17, 5, 18); //int motor_pole_pairs, float voltage_power_supply, int pin_enable, int pin_PWM_A, int pin_PWM_B, int pin_PWM_C
Encoder encoder(4, 16, 0x36, 0x0c, 0x0d, -1); //int pin_SDA, int pin_SCL, int I2C_address, int high_register_address, int low_register_address, int direction
PIDController position_PID_controller(motor.voltage_power_supply / 2/45, 0, 0, 1000, motor.voltage_power_supply/2);  //float P, float I, float D, float output_ramp_limit, float output_limit


float target_rad = NAN;

void setup() {
  Serial.begin(115200);
  motor.init();
  encoder.init();
  position_PID_controller.init();
  delay(3000);
}

void loop() {
  float value = FFOC_readFloatFromSerial();
  if(!isnan(value)){
    target_rad = value;
  }
  if(!isnan(target_rad)){
    FFOC_closeLoopPosition(motor, encoder, position_PID_controller, target_rad);
  }
}
