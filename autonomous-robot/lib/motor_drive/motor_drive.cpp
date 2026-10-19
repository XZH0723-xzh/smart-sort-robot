#include <Arduino.h>
#include "pinout.h"
#include "pid.h"

/*************************************************
 * @brief  电机驱动初始化函数
 * @param  无
 * @retval 无
 * @note  初始化电机驱动的引脚模式
 *************************************************/
void motor_drive_Init() {
  pinMode(Motor0_IN1, OUTPUT); // 设置电机0的输入引脚1为输出模式
  pinMode(Motor0_IN2, OUTPUT); // 设置电机0的输入引脚2为输出模式
  pinMode(Motor1_IN1, OUTPUT); // 设置电机1的输入引脚1为输出模式
  pinMode(Motor1_IN2, OUTPUT); // 设置电机1的输入引脚2为输出模式
}
//数字电机驱动函数，控制电机的前进、后退、停止、左转和右转
/*************************************************
 * @brief   M0前进函数
 * @param  无
 * @retval 无
 * @note  控制电机实现前进
 *************************************************/
void motor_d_forward_M0(){
  digitalWrite(Motor0_IN1, HIGH); // 电机0正转
  digitalWrite(Motor0_IN2, LOW);
}

/*************************************************
 * @brief  M1前进函数
 * @param  无
 * @retval 无
 * @note  控制电机实现前进
 *************************************************/
void motor_d_forward_M1(){
  digitalWrite(Motor1_IN1, HIGH); // 电机1正转
  digitalWrite(Motor1_IN2, LOW);
}

/*************************************************
 * @brief   M0掉电函数
 * @param  无
 * @retval 无
 * @note  控制电机实现掉电
 *************************************************/
void motor_d_stop_M0(){
  digitalWrite(Motor0_IN1, LOW); // 电机0掉电
  digitalWrite(Motor0_IN2, LOW);
}

/*************************************************
 * @brief   M1掉电函数
 * @param  无
 * @retval 无
 * @note  控制电机实现掉电
 *************************************************/
void motor_d_stop_M1(){
  digitalWrite(Motor1_IN1, LOW); // 电机1掉电
  digitalWrite(Motor1_IN2, LOW);
}

/*************************************************
 * @brief   M0反转函数
 * @param  无
 * @retval 无
 * @note  控制电机实现反转
 *************************************************/
void motor_d_back_M0(){
  digitalWrite(Motor0_IN1, LOW); // 电机0反转
  digitalWrite(Motor0_IN2, HIGH);
}

/*************************************************
 * @brief   M1反转函数
 * @param  无
 * @retval 无
 * @note  控制电机实现反转
 *************************************************/
void motor_d_back_M1(){
  digitalWrite(Motor1_IN1, LOW); // 电机1反转
  digitalWrite(Motor1_IN2, HIGH);
}

/*************************************************
 * @brief   M0刹车函数
 * @param  无
 * @retval 无
 * @note  控制电机实现刹车
 *************************************************/
void motor_d_brake_M0(){
  digitalWrite(Motor0_IN1, HIGH); // 电机0刹车
  digitalWrite(Motor0_IN2, HIGH);
}

/*************************************************
 * @brief   M1刹车函数
 * @param  无
 * @retval 无
 * @note  控制电机实现刹车
 *************************************************/
void motor_d_brake_M1(){
  digitalWrite(Motor1_IN1, HIGH); // 电机1刹车
  digitalWrite(Motor1_IN2, HIGH);
}

/*************************************************
 * @brief  小车前进函数
 * @param  无
 * @retval 无
 * @note  控制小车实现前进
 *************************************************/
void motor_d_forward() {
  motor_d_forward_M0(); // 电机0前进
  motor_d_forward_M1(); // 电机1前进
}


/*************************************************
 * @brief  小车停止函数
 * @param  无
 * @retval 无
 * @note  停止小车的移动
 *************************************************/
void motor_d_stop() {
  motor_d_stop_M0(); // 电机0停止
  motor_d_stop_M1(); // 电机1停止
}

/*************************************************
 * @brief  小车后退函数
 * @param  无
 * @retval 无
 * @note  控制小车实现后退
 *************************************************/
void motor_d_back() {
  motor_d_back_M0(); // 电机0反转
  motor_d_back_M1(); // 电机1反转
}

/*************************************************
 * @brief  小车左转函数
 * @param  无
 * @retval 无
 * @note  控制小车实现左转
 *************************************************/
void motor_d_left() {
  motor_d_forward_M0(); // 电机0前进
  motor_d_stop_M1(); // 电机1停止
}

/*************************************************
 * @brief  小车右转函数
 * @param  无
 * @retval 无
 * @note  控制小车实现右转
 *************************************************/
void motor_d_right() {
  motor_d_stop_M0(); // 电机0停止
  motor_d_forward_M1(); // 电机1前进
}

/*************************************************
 * @brief  小车原地左转函数
 * @param  无
 * @retval 无
 * @note  控制小车实现原地左转
 *************************************************/
void motor_d_sleft() {
  motor_d_forward_M0(); // 电机0前进
  motor_d_back_M1(); // 电机1反转
}

/*************************************************
 * @brief  小车原地右转函数
 * @param  无
 * @retval 无
 * @note  控制小车实现原地右转
 *************************************************/
void motor_d_sright() {
  motor_d_back_M0(); // 电机0反转
  motor_d_forward_M1(); // 电机1前进
}

/*************************************************
 * @brief  小车刹车函数
 * @param  无
 * @retval 无
 * @note  控制小车实现刹车
 *************************************************/
void motor_d_brake() {
  motor_d_brake_M0(); // 电机0刹车
  motor_d_brake_M1(); // 电机1刹车
}


//模拟电机驱动函数，控制电机的前进、后退、停止、左转和右转
/*************************************************
 * @brief   M0前进函数
 * @param  无
 * @retval 无
 * @note  控制电机实现前进
 *************************************************/
void motor_a_forward_M0(float speed){
  analogWrite(Motor0_IN1, speed); // 电机0以指定速度正转
  analogWrite(Motor0_IN2, 0);
}

/*************************************************
 * @brief   M1前进函数
 * @param  无
 * @retval 无
 * @note  控制电机实现前进
 *************************************************/
void motor_a_forward_M1(float speed){
  analogWrite(Motor1_IN1, speed); // 电机1以指定速度正转
  analogWrite(Motor1_IN2, 0);
}

/*************************************************
 * @brief   M0掉电函数
 * @param  无
 * @retval 无
 * @note  控制电机实现掉电
 *************************************************/
void motor_a_stop_M0(){
  analogWrite(Motor0_IN1, 0); // 电机0掉电
  analogWrite(Motor0_IN2, 0);
}

/*************************************************
 * @brief   M1掉电函数
 * @param  无
 * @retval 无
 * @note  控制电机实现掉电
 *************************************************/
void motor_a_stop_M1(){
  analogWrite(Motor1_IN1, 0); // 电机1掉电
  analogWrite(Motor1_IN2, 0);
}

/*************************************************
 * @brief   M0反转函数
 * @param  无
 * @retval 无
 * @note  控制电机实现反转
 *************************************************/
void motor_a_back_M0(float speed){
  analogWrite(Motor0_IN1, 0); // 电机0以指定速度反转
  analogWrite(Motor0_IN2, speed);
}

/*************************************************
 * @brief   M1反转函数
 * @param  无
 * @retval 无
 * @note  控制电机实现反转
 *************************************************/
void motor_a_back_M1(float speed){
  analogWrite(Motor1_IN1, 0); // 电机1以指定速度反转
  analogWrite(Motor1_IN2, speed);
}


/*************************************************
 * @brief  小车运行函数
 * @param  无
 * @retval 无
 * @note  控制小车实现前进、停止、左转、右转
 *************************************************/
void motor_a_run(int speed1, int speed2) {
  // 控制左电机 (对应 Motor1)
  if (speed1 > 0) {
    analogWrite(Motor1_IN1, speed1);
    analogWrite(Motor1_IN2, 0);
  } else {
    analogWrite(Motor1_IN1, 0);
    analogWrite(Motor1_IN2, abs(speed1));
  }
  
  // 控制右电机 (对应 Motor0)
  if (speed2 > 0) {
    analogWrite(Motor0_IN1, speed2);
    analogWrite(Motor0_IN2, 0);
  } else {
    analogWrite(Motor0_IN1, 0);
    analogWrite(Motor0_IN2, abs(speed2));
  }
}

/*************************************************
 * @brief  小车控制函数
 * @param  initial_motor_speed 初始电机速度
 * @param  s_range 电机速度范围
 * @retval 无
 * @note  根据PID控制算法的输出值调整电机速度，实现小车的自动控制
 *************************************************/
void motor_control(float initial_motor_speed, float s_range)
{
  // 计算有效电机转速：
  int left_motor_speed = initial_motor_speed - PID_value;//左电机速度
  int right_motor_speed = initial_motor_speed + PID_value;//右电机速度
  left_motor_speed = constrain(left_motor_speed, -s_range, s_range);//设定范围
  right_motor_speed = constrain(right_motor_speed, -s_range, s_range);
   if((error>=-2)&&(error<=2))
   {
    motor_a_run(left_motor_speed, right_motor_speed);
   }
   else if(error<-2)
   {
      
      //motor_d_sright();
      motor_a_run(250,-250);
   }
   else
   {
      //motor_d_sleft();
      motor_a_run(-250,250);
   }
}

