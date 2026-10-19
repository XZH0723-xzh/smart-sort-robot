#include <Arduino.h>
#include "robotic_arm.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Robotic Arm Test Start...");
  
  // 1. 初始化机械臂（里面内置了 armReset() 归中复位）
  robotic_arm_init();
  
  // 留出3秒钟让你看清楚初始归中姿态
  delay(3000); 
}

void loop() {
  setJointAngle(JOINT_CLAW, 60, 10);   // 调整俯仰角度
  //Serial.println("====== 准备抓取 ======");
  // 2. 执行抓取序列
  //arm_grab_sequence();
  
  // 抓完后等待3秒
  //Serial.println("抓取完成，等待3秒...");
  //delay(3000);
  
  //Serial.println("====== 准备投放 ======");
  // 3. 执行投放序列
  //arm_deposit_sequence();
  
  // 投放完成后复位（如果 deposit_sequence 里没有包含复位的话，可以手动调用）
  //armReset();
  
  // 投放和复位完后等待3秒，然后再次循环
  //Serial.println("投放复位完成，等待3秒...");
  //delay(3000);
}