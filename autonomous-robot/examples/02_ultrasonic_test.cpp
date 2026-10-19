#include <Arduino.h>
#include "hc_sr04.h" // 引入超声波传感器的头文件

void setup() {
  Serial.begin(115200); // 保持和之前配置的一致的波特率
  hc_sr04_init();       // 初始化超声波传感器
  
  Serial.println("Ultrasonic Sensor Test Started!");
}

void loop() {
  float distance = hc_sr04_read_distance(); // 读取距离
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(1000); // 延时 200 毫秒，超声波每次测量至少需要间隔 60 毫秒以上防止回波干扰
}
