/*
AS5600.cpp
version: 1.2
*/

/*attention：
  1.基于灯哥开源
  2.本项目基于GNU协议开源，任何修改或衍生的作品必须以相同的方式公开发布

/*note:
  1. available!!!
*/


#include "Wire.h" 
#include "AS5600.h"
#include <Arduino.h> 

#define I2C_SDA 4
#define I2C_SCL 16

int _raw_ang_hi = 0x0c; //AS5600高位寄存器地址
int _raw_ang_lo = 0x0d; //AS5600低位寄存器地址
int _AS5600_Address = 0x36; //AS5600 I2C地址
int ledtime = 0;
int32_t full_rotations = 0; // full rotation tracking
float angle_prev=0; 

void AS5600_setup() {
  Wire.begin(I2C_SDA, I2C_SCL, 400000UL); //400000UL，表示400000Hz；UL（Unsigned Long）表示无符号长整型。
  delay(1000);
}

//AMS5600_read(int in_adr_hi, int in_adr_lo)这段代码是一个函数，其目的是从I2C设备（在代码中的变量名为_ams5600_Address）中读取两个字节数据，并将其合并成一个16位的无符号整数返回。
//具体来说，函数接受两个整型参数in_adr_hi和in_adr_lo，它们用于指定需要读取的两个字节数据的地址。函数中首先通过Wire库开始I2C传输，向设备写入in_adr_lo和in_adr_hi分别作为数据地址，然后读取相应的字节数据。
//在每个Wire.requestFrom()调用之后，通过一个while循环等待数据接收完毕。然后读取接收到的低字节和高字节，并使用位运算将它们合并成一个16位的无符号整数。
//最后，返回合并后的整数。如果读取过程中出现错误或者函数没有成功读取到数据，则函数返回-1。
word AS5600_read(int in_adr_hi, int in_adr_lo)
{
  word retVal = -1;
 
  /* 读低位 */
  Wire.beginTransmission(_AS5600_Address);
  Wire.write(in_adr_lo);
  Wire.endTransmission();
  Wire.requestFrom(_AS5600_Address, 1);
  while(Wire.available() == 0);
  int low = Wire.read();
 
  /* 读高位 */  
  Wire.beginTransmission(_AS5600_Address);
  Wire.write(in_adr_hi);
  Wire.endTransmission();
  Wire.requestFrom(_AS5600_Address, 1);
  while(Wire.available() == 0);
  int high = Wire.read();
  
  retVal = (high << 8) | low;
  
  return retVal;
}

word AS5600_getRawAngle() //从AS5600读出表示角度的12位二进制数
{
  return AS5600_read(_raw_ang_hi, _raw_ang_lo);
}

float AS5600_getRad_Without_track()
{
  return AS5600_getRawAngle()*360/4096*PI/180; //getRawAngle()*360/pow(2, 12)得到角度；getRawAngle()*360/2048*PI/180得到弧度。
}

float AS5600_getRad()
{
    float val = AS5600_getRad_Without_track();
    float d_angle = val - angle_prev;
    //计算旋转的总圈数
    //通过判断角度变化是否大于80%的一圈(0.8f*2*PI)来判断是否发生了溢出，如果发生了，则将full_rotations增加1（如果d_angle小于0）或减少1（如果d_angle大于0）。
    if(abs(d_angle) > (0.8f*2*PI) ) full_rotations += ( d_angle > 0 ) ? -1 : 1; 
    angle_prev = val;
    return (float)full_rotations*2*PI + angle_prev;
    
}
