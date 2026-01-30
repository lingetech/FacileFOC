#ifndef LOWPASS_FILTER_H
#define LOWPASS_FILTER_H


class LowpassFilter
{
public:
  LowpassFilter(float time_constant);
  void init();
  float filter(float x);
private:
  float time_constant;
  unsigned long previous_time_us;
  float previous_y;
  unsigned long current_time_us;
  float current_y;
};

#endif
