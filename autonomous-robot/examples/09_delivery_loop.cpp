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
// 0: 正常循迹，等待全黑，或全白遇障停5s
// 1: 直行设定时间
// 2: 恢复循迹并进行超声波检测
// 3: 降速循迹并进行颜色检测
// 4: 检测到绿色，停车5s后恢复
// 5: 传感器全白且超声波<20，停车5s后恢复

unsigned long straight_start_time = 0;
const unsigned long STRAIGHT_DURATION = 2000; // 直行设定时间：2000毫秒 (可按需调整)

unsigned long last_ultrasonic_time = 0; // 用于非阻塞调用超声波防止卡死
unsigned long stop_start_time = 0; // 用于停车5s倒计时

// 用于识别 4mm 宽黑线的变量
bool is_all_black = false;
unsigned long black_line_start_time = 0;

// 用于全白状态防误检的变量
bool is_all_white = false;
unsigned long white_start_time = 0;

// 用于颜色传感器防误检的变量
int green_detect_count = 0;
const int GREEN_DETECT_THRESHOLD = 3; // 连续检测到3次才算真正识别为主

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
      // 状态0：全速循迹，检测 4mm 宽的黑线 和 (全白持续防误检+超声波<20)
      motor_control(200, 255); // 基础速度200，限幅255
      
      // 全白防误检逻辑：记录连续全白的时间
      if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0) {
        if (!is_all_white) {
          is_all_white = true;
          white_start_time = millis(); // 记录刚刚全白的时间
        }
      } else {
        is_all_white = false; // 只要红外碰到了线，立即取消全白状态
      }

      // 非阻塞超声波检查 (每60ms)
      if (millis() - last_ultrasonic_time > 60) {
        last_ultrasonic_time = millis();
        float distance = hc_sr04_read_distance();
        
        // 传感器全白必须持续超过 50ms 并且 超声波判定距离 < 20 才能触发
        if (is_all_white && (millis() - white_start_time >= 50) && distance > 0 && distance < 20.0) {
          car_state = 5;
          stop_start_time = millis(); // 记录停车时间
          //Serial.println("Debounced All White & Obstacle < 20cm! Stopping 5s.");
          break; // 跳出 case 0，不再往后执行这帧的黑线判断
        }
      }
      
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
            //Serial.print("Detected 4mm Black Line! Duration ms: ");
            //Serial.println(black_duration);
          }
        }
      }
      break;

    case 1:
      // 状态1：脱离黑线，直线运行设定时间
      motor_a_run(200, 255); // 左右电机全速直行
      
      if (millis() - straight_start_time >= STRAIGHT_DURATION) {
        car_state = 2; // 后续恢复循迹
        //Serial.println("Straight duration ended. Resuming tracking & ultrasonic started.");
      }
      break;

    case 2:
      // 状态2：恢复循迹，启动超声波检测
      motor_control(200, 255); // 继续全速循迹
      
      // 超声波读取需间隔至少60ms，防止阻塞循迹PID
      if (millis() - last_ultrasonic_time > 60) {
        last_ultrasonic_time = millis();
        float distance = hc_sr04_read_distance();
        
        if (distance > 0 && distance < 20.0) { // 检测到距离小于20
          car_state = 3;
          //Serial.println("Obstacle < 20cm! slowing down and turning on Color Sensor...");
        }
      }
      break;

    case 3:
      // 状态3：降低循迹速度，打开颜色传感器检测
      motor_control(100, 150); // 将基础速度降到150，继续循迹
      
      {
        String color = get_color();
        if (color == "green") {
          green_detect_count++; // 检测到绿色，计数+1
          if (green_detect_count >= GREEN_DETECT_THRESHOLD) {
            car_state = 4;
            stop_start_time = millis(); // 记录停车时间
            //Serial.println("Green reliably detected! Stopping 5s.");
          }
        } else {
          // 如果中间杂入其他颜色，重置计数，防止累积误判
          if (green_detect_count > 0) {
            green_detect_count = 0; 
          }
        }
      }
      break;

    case 4:
      // 状态4：检测到绿色后停车5s，恢复循迹
      motor_d_stop(); 
      if (millis() - stop_start_time >= 1000) {
        car_state = 0;       // 切回默认循迹状态
        is_all_black = false; // 清零全黑计数的状态标志
        green_detect_count = 0; // 重置颜色识别计数
        is_all_white = false; // 清零全白防误判标志
        //Serial.println("5s stop over (Green). Resuming tracking...");
      }
      break;

    case 5:
      // 状态5：全白并且超声波小于20，停车5s，恢复循迹
      motor_d_stop();
      if (millis() - stop_start_time >= 1000) {
        car_state = 0;       // 切回默认循迹状态
        is_all_black = false; // "全黑计数清零"，重新等待识别4mm黑线
        is_all_white = false; // 清零全白防误判标志
        //Serial.println("5s stop over (All White & Obstacle). Resuming tracking...");
      }
      break;
  }
}
