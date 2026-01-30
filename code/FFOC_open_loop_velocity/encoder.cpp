#include "encoder.h"
#include <Arduino.h>
#include "Wire.h"


//构造函数
Encoder::Encoder(int pinSDA, int pinSCL, int I2C_address, int high_register_address, int low_register_address, int direction):    //direction = 1：电机逆时针旋转，编码器数值增加；direction = -1：电机顺时针旋转，编码器数值增加
 pinSDA(pinSDA), pinSCL(pinSCL), I2C_address(I2C_address), high_register_address(high_register_address), low_register_address(low_register_address), direction(direction){

}

//得到测量值
int Encoder::readData(){
    int buffer = NAN;
	//读高位寄存器
	Wire.beginTransmission(I2C_address);
	Wire.write(high_register_address);
	Wire.endTransmission();
	Wire.requestFrom(I2C_address, 1);
	while(Wire.available() == 1);
	int high_buffer = Wire.read();
	//读低位寄存器
	Wire.beginTransmission(I2C_address);
	Wire.write(low_register_address);
	Wire.endTransmission();
	Wire.requestFrom(I2C_address, 1);
	while(Wire.available() == 1);
	int low_buffer = Wire.read();
	//拼接数据
	buffer = (high_buffer << 8) | low_buffer;
	return buffer;
}

//计算不考虑圈数的弧度
float Encoder::getRadWithoutRotation(){
	float rad_without_rotation = direction * readData() * 360.0f / 4096.0f * PI / 180.0f;	//readData()*360/pow(2, 12)得到角度；readData()*360/4096*PI/180得到弧度。
	return rad_without_rotation;
}

//计算考虑圈数的弧度
float Encoder::getRad(){
	current_rad_without_rotation = getRadWithoutRotation();
	float delta_rad_without_rotation = current_rad_without_rotation - previous_rad_without_rotation;
	//计算旋转圈数。若abs(delta_rad_without_rotation较大，则认为溢出一圈。
	if(abs(delta_rad_without_rotation) > (0.8f * 2 * PI)){
		if(delta_rad_without_rotation > 0){
			full_rotation += 1;
		}else{
			full_rotation -= 1;
		}
	}
	previous_rad_without_rotation = current_rad_without_rotation;
	current_rad = full_rotation * 2* PI + current_rad_without_rotation;
	return current_rad;
}

//得到速度
float Encoder::getVelocity(){
	current_rad = getRad();
	current_time_us = micros();
	float delta_rad = current_rad - previous_rad;
	//计算时间差。无符号减法自动处理 micros() 溢出
	float delta_time = (current_time_us - previous_time_us) / 1000000.0f;
	if(delta_time <= 0){
		delta_time = 1e-3f;
	}
	velocity = delta_rad / delta_time;
	previous_rad = current_rad;
	previous_time_us = current_time_us;
	return velocity;
}

//初始化
void Encoder::init(){
	Wire.begin(pinSDA, pinSCL, 400000UL);
	//赋初值
	full_rotation = 0;
	previous_rad_without_rotation = getRadWithoutRotation();
	previous_rad = previous_rad_without_rotation;
	previous_time_us = micros();
	velocity = 0.0f;
}