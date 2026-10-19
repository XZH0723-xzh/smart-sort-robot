#include <Arduino.h>
#include "Infrared_recognition.h"
#include "motor_drive.h"
#include "pid.h"
#include "pinout.h"




void setup() {
  Serial.begin(9600);
  Infrared_recognition_Init();  // 初始化红外传感器
  motor_drive_Init();           // 初始化电机驱动
}

void loop() {
  read_sensor_values();
  calculate_pid();
  motor_control(200, 255); // 开启正常速度PID循迹
}
