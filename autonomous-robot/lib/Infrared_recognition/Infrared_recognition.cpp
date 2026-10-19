#include <Arduino.h>
#include "pinout.h"
#include "pid.h"

int sensor[3] = {0, 0, 0}; // 存储三个红外传感器的状态

/**
 * @brief  红外传感器初始化函数
 * @param  无
 * @retval 无
 * @note  初始化红外传感器的引脚模式
 */
void Infrared_recognition_Init() {
  pinMode(Left_1, INPUT); // 设置左边第一个红外传感器为输入模式
  pinMode(middle, INPUT); // 设置中间的红外传感器为输入模式
  pinMode(Right_1, INPUT); // 设置右边第一个红外传感器为输入模式
}

/**
 * @brief  读取红外传感器数据函数
 * @param  无
 * @retval 无
 * @note  读取三个红外传感器的状态并存储在全局变量中
 */
void Infrared_recognition_Read() {
  sensor[0] = digitalRead(Left_1); // 读取左边第一个红外传感器的状态
  sensor[1] = digitalRead(middle); // 读取中间的红外传感器的状态
  sensor[2] = digitalRead(Right_1); // 读取右边第一个红外传感器的状态
}

void Infrared_recognition_Read_Gray() {
  // 读取三个红外传感器的模拟值并存储在全局变量中
  sensor[0] = analogRead(Left_1); // 读取左边第一个红外传感器的模拟值
  sensor[1] = analogRead(middle); // 读取中间的红外传感器的模拟值
  sensor[2] = analogRead(Right_1); // 读取右边第一个红外传感器的模拟值
  //Serial.print("Analog sensor values: ");
  //Serial.print(sensor[0]);
  //Serial.print(",");
  //Serial.print(sensor[1]);
  //Serial.print(",");
  //Serial.println(sensor[2]);
  if(sensor[0] < 200) // 根据设定的阈值将模拟值转化为数字量
  {
    sensor[0] = 1; // 黑色
  }
  else
  {
    sensor[0] = 0; // 白色
  }
  if(sensor[1] < 200)
  {
    sensor[1] = 1; // 黑色
  }
  else
  {
    sensor[1] = 0; // 白色
  }
  if(sensor[2] < 200)
  {
    sensor[2] = 1; // 黑色
  }
  else
  {
    sensor[2] = 0; // 白色
  }
}

/**
 * @brief  读取红外传感器数据函数（模拟灰度识别）
 * @param  无
 * @retval 无
 * @note  读取三个红外传感器的模拟值，根据设定的阈值将其转化为数字量，并根据传感器状态更新误差值
 */
void read_sensor_values()
{
  Infrared_recognition_Read_Gray(); // 读取红外传感器的模拟值并转化为数字量
  if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1))//001
  {
    error = -2;//需要原地右转修正
  }
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 1))//011
  {
    error = -1;//需要右转修正
  }
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 0))//010
  {
    error = 0;//直行
  }
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 0))//110
  {
    error = 1;//需要左转修正
  }
  else if ((sensor[0] == 1) && (sensor[1] == 0) && (sensor[2] == 0))
  {
    error = 2;//需要原地左转修正
  }
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 0))//跑上一状态
  {
    if (error > 0)
    {
      error = 3;
    }
    else
     {
      error = -3;
    }
  }
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1))
  {
      error = 0;//1 1 1直行
  }

  //Serial.print("Sensor values: ");
  //Serial.print(sensor[0]);
  //Serial.print(",");
  //Serial.print(sensor[1]);
  //Serial.print(",");
  //Serial.println(sensor[2]);
}

/**
 * @brief  记录三路传感器全黑次数
 * @param  无
 * @retval 无
 * @note  读取三个红外传感器的状态，如果全黑则记录全黑状态的次数
 */
void record_black_count()
{
  static int black_count = 0; // 记录全黑状态的次数
  if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1))
  {
    delay(100); // 延时100毫秒，避免误判
    if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1)) // 如果三个传感器全黑
    {
      black_count++; // 全黑状态次数加1
      Serial.print("Black count: ");
      Serial.println(black_count); // 输出全黑状态的次数
    }
  }
}