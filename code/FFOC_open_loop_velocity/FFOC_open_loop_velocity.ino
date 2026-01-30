#include <Arduino.h>
#include "FacileFOC.hpp"


//实例化对象
Motor motor1(7, 12.0, 19, 17, 5, 18); //int motor_pole_pairs, float voltage_power_supply, int pin_enable, int pin_PWM_A, int pin_PWM_B, int pin_PWM_C





void setup() {
 motor1.init();
 Serial.begin(115200);
 delay(3000);
}

void loop() {
  FFOC_openLoopVelocity(motor1, 10);
}
