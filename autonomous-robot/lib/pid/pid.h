#ifndef PID_H
#define PID_H

extern float Kp, Ki, Kd; // PID控制算法的比例、积分和微分常数
extern float error, P, I, D, PID_value; // 当前误差值、比例、积分、微分和PID控制算法的输出值
extern float previous_error, previous_I; // 上一次的误差值和积分值

void calculate_pid();// PID控制算法计算函数

#endif