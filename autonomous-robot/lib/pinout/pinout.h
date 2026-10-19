#ifndef PINOUT_H
#define PINOUT_H

#include <Arduino.h>

//OPENMV h7 plus通信引脚
#define OPENMV_TX 0 // OPENMV的TX引脚连接到Arduino的数字口0
#define OPENMV_RX 1 // OPENMV的RX引脚连接到Arduino的数字口1

//三路数字红外传感器引脚定义
#define Left_1 A0 // 左边第一个红外传感器连接到模拟口0
#define middle A2 // 中间的红外传感器连接到模拟口2
#define Right_1 A3 // 右边第一个红外传感器连接到模拟口3

//电机引脚定义
#define Motor0_IN1 5 // 电机1的输入引脚1连接到数字口5
#define Motor0_IN2 6 // 电机1的输入引脚2连接到数字口6
#define Motor1_IN1 9 // 电机2的输入引脚1连接到数字口9
#define Motor1_IN2 10 // 电机2的输入引脚2连接到数字口10

//机械臂引脚定义 (改为三轴舵机)
#define Servo_Pitch_PIN 4  // 俯仰舵机
#define Servo_Reach_PIN 7  // 伸缩舵机
#define Servo_Claw_PIN  11 // 夹取舵机

//超声波引脚定义
#define TRIGGER A5 // 超声波模块的触发(Trig)引脚连接到数字口12
#define ECHOPIN A4 // 超声波模块的回响(Echo)引脚连接到数字口3

#endif