#include <Arduino.h>

float Kp = 200, Ki = 0, Kd = 100; //PID控制算法
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;//P比例I积分D微分
float previous_error = 0, previous_I = 0;//微分和积分常量


/*************************************************
 * @brief  PID控制算法计算函数
  * @param  无
 * @retval 无
 * @note  根据当前误差值计算PID控制算法的输出值，并更新积分和微分的历史值
 *************************************************/
void calculate_pid()
{
  P = error;
  I = I + previous_I; // 修改这里：将当前误差累加给积分项
  D = error - previous_error;
  PID_value = (Kp * P) + (Ki * I) + (Kd * D);//PID算式
  // Serial.println(PID_value); // 调好后建议注释掉打印，提高运行效率
  previous_I = I;
  previous_error = error;
}
