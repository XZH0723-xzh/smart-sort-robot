#include <Arduino.h>
#include "Infrared_recognition.h"
#include "motor_drive.h"
#include "pid.h"
#include "robotic_arm.h"
#include "pinout.h"
#include "openmv.h"

extern int sensor[3];

enum RobotState {// 定义状态枚举，方便状态管理
  STATE_IDLE = 1,
  STATE_GRAB,
  STATE_ALIGN,
  STATE_TRACK_TO_BOX,
  STATE_DEPOSIT,
  STATE_TRACK_TO_GRID,
  STATE_GRID_BLIND,
  STATE_RETURN_HOME
};

RobotState currentState = STATE_IDLE;// 当前状态变量
String targetShape = "";

unsigned long blindStartTime = 0;// 盲冲时间(毫秒)
const unsigned long BLIND_DURATION = 1500; // 盲冲时间(毫秒)

void setup() {
  // openmv_init 包含了 Serial.begin(115200);
  openmv_init(115200); 

  Infrared_recognition_Init(); 
  motor_drive_Init();          
  robotic_arm_init();          
  
  // 确保处于静止，准备开始
  motor_d_stop();
  currentState = STATE_IDLE;
  Serial.println("System Ready! Waiting for shape detection...");
  
  // 发送指令1以开启3D识别
  openmv_send_command(1);
}

void loop() {
  // 持续读取传感器和计算PID误差
  read_sensor_values(); 
  calculate_pid();

  switch(currentState) {
    case STATE_IDLE: {
      motor_d_stop();
      if(openmv_is_available()) {
        String msg = openmv_read_message(); 
        if(msg == "Circle" || msg == "Square" || msg == "Triangle") {
          targetShape = msg;
          currentState = STATE_GRAB;
        }
      }
      break;
    }
    
    case STATE_GRAB: {
      motor_d_stop();
      arm_grab_sequence(); // 调用封装好的抓取复位序列
      currentState = STATE_ALIGN;
      break;
    }
    
    case STATE_ALIGN: {
      motor_d_sleft(); // 开启原地左转(转并寻线)
      // 只要有任意一个传感器重新压中黑线，立即停止旋转
      if(sensor[0] == 1 || sensor[1] == 1 || sensor[2] == 1) { 
        motor_d_stop();
        // 发送指令2进入识别装箱图形模式
        openmv_send_command(2);
        currentState = STATE_TRACK_TO_BOX;
      }
      break;
    }
    
    case STATE_TRACK_TO_BOX: {
      // 开启低速PID循迹
      motor_control(100, 150); 
      
      // 检测摄像头标志
      if(openmv_is_available()) {
        String msg = openmv_read_message();
        if(msg == "3") { // 收到匹配成功
          motor_d_stop(); // 立刻急停
          currentState = STATE_DEPOSIT;
        }
      }
      break;
    }
    
    case STATE_DEPOSIT: {
      motor_d_stop();
      arm_deposit_sequence(); // 伸展并投放，然后复位
      currentState = STATE_TRACK_TO_GRID;
      break;
    }
    
    case STATE_TRACK_TO_GRID: {
      // 正常速度PID循迹, 应对左大弯
      motor_control(200, 255);
      
      // 循迹过弯时全黑代表到了十字标志线
      if(sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1) {
        motor_d_stop();
        blindStartTime = millis();
        currentState = STATE_GRID_BLIND;
      }
      break;
    }
    
    case STATE_GRID_BLIND: {
      // 关闭PID循迹，直线盲冲(定时)
      motor_a_run(150, 150); // 左右同速盲冲
      if(millis() - blindStartTime >= BLIND_DURATION) {
        currentState = STATE_RETURN_HOME;
      }
      break;
    }
    
    case STATE_RETURN_HOME: {
      // 恢复PID，减速寻终点
      motor_control(100, 180);
      
      // 检测等待区的直角横线(全黑)
      if(sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1) {
        motor_d_stop();
        currentState = STATE_IDLE; // 回到状态1
        openmv_send_command(1); // 再次启动识别
      }
      break;
    }
  }
}
