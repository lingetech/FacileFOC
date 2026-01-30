#ifndef ENCODER_H
#define ENCODER_H


class Encoder
{
public:
    Encoder(int pinSDA, int pinSCL, int I2C_address, int high_register_address, int low_register_address, int direction);
    float getRadWithoutRotation();
    float getRad();
    float getVelocity();
    void init();
private:
    int readData();

    int pinSDA;
    int pinSCL;
    int I2C_address;
    int high_register_address;
    int low_register_address;
    int direction;

    int full_rotation;
    float velocity;
    float previous_rad_without_rotation;
    float previous_rad;
    unsigned long previous_time_us;
    float current_rad_without_rotation;
    float current_rad;
    unsigned long current_time_us;
};

#endif