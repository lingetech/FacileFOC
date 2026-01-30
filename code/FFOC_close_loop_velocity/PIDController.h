#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H


class PIDController
{
public:
  PIDController(float P, float I, float D, float output_ramp_limit, float output_limit);
  void init();
  float PID(float error);
private:
  float P;
  float I;
  float D;
  float output_ramp_limit;
  float output_limit;
  float previous_error;
  float previous_integral;
  float previous_output;
  unsigned long previous_time_us;
  unsigned long current_time_us;
};

#endif
