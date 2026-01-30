#ifndef MOTOR_H
#define MOTOR_H


class Motor
{
public:
  Motor(int motor_pole_pairs, float voltage_power_supply, int pin_enable, int pin_PWM_A, int pin_PWM_B, int pin_PWM_C);
  void init();
  int motor_pole_pairs;
  float voltage_power_supply;
  int pin_PWM_A;
  int pin_PWM_B;
  int pin_PWM_C;
private:
  int pin_enable;
};

#endif