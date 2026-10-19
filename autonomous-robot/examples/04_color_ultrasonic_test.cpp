#include <Arduino.h>
#include "color_recognition.h"
#include "hc_sr04.h" // 引入超声波传感器的头文件

void setup() {
  Serial.begin(115200); // 保持和之前配置的一致的波特率
  color_recognition_Init(); // 初始化颜色传感器
  hc_sr04_init();       // 初始化超声波传感器
  
  Serial.println("Ultrasonic Sensor Test Started!");
}

void loop() {
  float distance = hc_sr04_read_distance(); // 读取距离
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  String color = get_color(); // 获取识别到的颜色
  Serial.println(color); // 打印识别到的颜色
  
  delay(1000); // 延时 1000 毫秒，超声波每次测量至少需要间隔 60 毫秒以上防止回波干扰
}
