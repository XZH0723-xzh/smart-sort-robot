#include <Arduino.h>
#include "color_recognition.h"



void setup() {
  Serial.begin(115200); // 颜色传感器需要 115200，统一修改波特率
  color_recognition_Init(); // 初始化颜色传感器
}

void loop() {
  String color = get_color(); // 获取识别到的颜色
  Serial.println(color); // 打印识别到的颜色
  delay(1000); // 每秒更新一次颜色识别结果
}
