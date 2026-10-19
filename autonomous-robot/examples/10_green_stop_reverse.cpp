#include <Arduino.h>
#include "Infrared_recognition.h"
#include "motor_drive.h"
#include "pid.h"
#include "hc_sr04.h"
#include "color_recognition.h"

// 声明外部传感器数组以便判断全黑
extern int sensor[3];

// 状态机定义
int car_state = 0; 
// 0: 正常循迹，等待全黑
// 1: 直行设定时间
// 2: 恢复循迹并进行超声波检测
// 3: 降速循迹并进行颜色检测
// 4: 检测到绿色，短暂倒车抵消惯性
// 5: 停车3秒
// 6: 恢复全速循迹，等待全白
// 7: 遇到全白，直线行走

unsigned long straight_start_time = 0;
const unsigned long STRAIGHT_DURATION = 3000; // 直行设定时间：4000毫秒 (可按需调整)

unsigned long last_ultrasonic_time = 0; // 用于非阻塞调用超声波防止卡死

// 用于识别 4mm 宽黑线的变量
bool is_all_black = false;
unsigned long black_line_start_time = 0;

int green_count = 0; // 用于过滤绿色的误检测，连续检测到多次才认为有效

unsigned long reverse_start_time = 0;
const unsigned long REVERSE_DURATION = 250; // 短暂倒车时间(毫秒)，可根据惯性大小调整

unsigned long stop_start_time = 0;
const unsigned long STOP_DURATION = 3000; // 停车时间(毫秒)

void setup() {
  Serial.begin(115200); // 颜色传感器需要 115200，统一修改波特率
  hc_sr04_init();
  Infrared_recognition_Init();
  motor_drive_Init();
  color_recognition_Init(); // 初始化颜色传感器
}

void loop() {
  // 1. 读取红外传感器值更新误差并计算PID
  read_sensor_values(); 
  calculate_pid(); 

  // 2. 根据状态机执行动作
  switch(car_state) {
    case 0:
      // 状态0：全速循迹，检测 4mm 宽的黑线
      motor_control(255, 255); // 基础速度200，限幅255
      
      // 当传感器全黑时 (0、1、2 传感器全为 1)，开始计时
      if (sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1) {
        if (!is_all_black) {
          is_all_black = true;
          black_line_start_time = millis(); // 记录全黑开始的时间
        }
      } else {
        // 当脱离全黑区域时，计算维持全黑的时间
        if (is_all_black) {
          is_all_black = false;
          unsigned long black_duration = millis() - black_line_start_time;
          
          // 若车速较快，4mm宽的黑线经过时间极短（如 5ms~60ms 之间）。
          // 大于或小于此范围可能认为是噪点或是较宽的十字路口/起始线，不予触发。
          // 注意：如果你发现小车未能识别，可以适当调宽下方的 60 阈值。
          if (black_duration >= 2 && black_duration <= 60) {
            car_state = 1;
            straight_start_time = millis();
            Serial.print("Detected 4mm Black Line! Duration ms: ");
            Serial.println(black_duration);
          }
        }
      }
      break;

    case 1:
      // 状态1：脱离黑线，直线运行设定时间
      motor_a_run(255, 255); // 左右电机全速直行
      
      if (millis() - straight_start_time >= STRAIGHT_DURATION) {
        car_state = 2; // 后续恢复循迹
        Serial.println("Straight duration ended. Resuming tracking & ultrasonic started.");
      }
      break;

    case 2:
      // 状态2：恢复循迹，启动超声波检测
      motor_control(255, 255); // 继续全速循迹
      
      // 超声波读取需间隔至少60ms，防止阻塞循迹PID
      if (millis() - last_ultrasonic_time > 60) {
        last_ultrasonic_time = millis();
        float distance = hc_sr04_read_distance();
        
        if (distance > 0 && distance < 20.0) { // 检测到距离小于20
          car_state = 3;
          Serial.println("Obstacle < 20cm! slowing down and turning on Color Sensor...");
        }
      }
      break;

    case 3:
      // 状态3：降低循迹速度，打开颜色传感器检测
      motor_control(100, 255); // 将基础速度降到100
      
      {
        String color = get_color();
        if (color == "green") {
          green_count++;
          if (green_count >= 3) { // 连续检测到3次绿色才触发
            car_state = 4;
            reverse_start_time = millis(); // 记录倒车起始时间
            green_count = 0; // 重置计数器
            Serial.println("Green detected! Reversing...");
          }
        } else {
          green_count = 0; // 如果检测到非绿色，重置计数
        }
      }
      break;

    case 4:
      // 状态4：短暂倒车，抵消向前的惯性
      motor_a_back_M0(150); // M0 电机后退，速度 150 (可按需调整)
      motor_a_back_M1(150); // M1 电机后退，速度 150
      
      if (millis() - reverse_start_time >= REVERSE_DURATION) {
        car_state = 5; // 倒车时间到，进入停车状态
        stop_start_time = millis();
        Serial.println("Reverse finished. Fully stopping for 3s.");
      }
      break;

    case 5:
      // 状态5：停车 3S
      motor_d_stop(); 
      if (millis() - stop_start_time >= STOP_DURATION) {
        car_state = 6;
        Serial.println("Stop finished. Resuming full speed tracking until all white.");
      }
      break;

    case 6:
      // 状态6：恢复全速循迹，等待全白（所有传感器为0）
      motor_control(255, 255); 
      // 检查是否全白
      if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0) {
        car_state = 7;
        Serial.println("All white detected! Going straight...");
      }
      break;

    case 7:
      // 状态7：按直线行走
      motor_a_run(255, 255); // 左右电机全速直行
      break;
  }
}
