#include <Arduino.h>
#include "pinout.h" // 引入引脚定义

void setup() {
  Serial.begin(115200); 

  // 初始化这3个引脚为输入模式
  pinMode(Left_1, INPUT);
  pinMode(middle, INPUT);
  pinMode(Right_1, INPUT);

  Serial.println("IR Sensor Analog Value Test Started!");
}

void loop() {
  // 读取三个红外传感器的模拟值 (0-1023)
  int val_L = analogRead(Left_1);
  int val_M = analogRead(middle);
  int val_R = analogRead(Right_1);
  
  // 打印输出到串口，使用 \t 制表符对齐
  Serial.print("Left: ");
  Serial.print(val_L);
  Serial.print("\tMiddle: ");
  Serial.print(val_M);
  Serial.print("\tRight: ");
  Serial.println(val_R);
  
  delay(100); // 延时 100ms 方便查看
}
