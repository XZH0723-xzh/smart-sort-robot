#ifndef MOTOR_DRIVE_H
#define MOTOR_DRIVE_H

void motor_drive_Init();// 电机驱动初始化函数
void motor_d_forward_M0(); // M0前进函数
void motor_d_forward_M1(); // M1前进函数
void motor_d_forward();// 小车前进函数
void motor_d_stop_M0(); // M0掉电函数
void motor_d_stop_M1(); // M1掉电函数
void motor_d_forward(); // 小车前进函数
void motor_d_stop(); // 小车停止函数
void motor_d_back(); // 小车后退函数
void motor_d_left(); // 小车左转函数
void motor_d_right(); // 小车右转函数
void motor_d_sleft(); // 小车原地左转函数
void motor_d_sright(); // 小车原地右转函数
void motor_d_brake(); // 小车刹车函数
void motor_a_forward_M0(float speed); // M0前进函数（模拟）
void motor_a_forward_M1(float speed); // M1前进函数（模拟）
void motor_a_stop_M0(); // M0掉电函数（模拟）
void motor_a_stop_M1(); // M1掉电函数（模拟）
void motor_a_back_M0(float speed); // M0反转函数（模拟）
void motor_a_back_M1(float speed); // M1反转函数（模拟）
void motor_a_run(int speed1, int speed2); // 小车运行函数（模拟）
void motor_control(float initial_motor_speed, float s_range); // 小车控制函数（模拟）

#endif