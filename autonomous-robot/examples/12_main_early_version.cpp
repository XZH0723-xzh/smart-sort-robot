#include <Arduino.h>
#include "Infrared_recognition.h"
#include "motor_drive.h"
#include "pid.h"
#include "robotic_arm.h"
#include "pinout.h"
#include "hc_sr04.h"

extern int sensor[3];

enum RobotState {
  STATE_wait,         // 等待区域抓取状态，执行抓取动作
  STATE_ALIGN,        // 对齐状态，原地旋转寻找黑线
  STATE_TRACK_TO_GRID,// 正常循迹状态到网格
  STATE_GRID,         // 网格状态，检测到十字标志线后盲冲通过网格
  STATE_GRID_TO_INVEST, // 慢速循迹状态从网格到Investigation区域
  STATE_INVEST_TO_wait   // Investigation区域等待状态，检测超声波距离并执行投放动作
};

RobotState currentState = STATE_wait; // 当前状态变量
unsigned long stateStartTime = 0;
unsigned int alignConfirmCount = 0;
unsigned int trackConfirmCount = 0;
unsigned int exitConfirmCount = 0;

const unsigned long BLIND_DURATION = 2200;      // 盲冲时间(毫秒)
const unsigned int ALIGN_DEBOUNCE_COUNT = 100;    // ALIGN 状态稳定检测次数
const unsigned int TRACK_DEBOUNCE_COUNT = 150;    // TRACK_TO_GRID 状态稳定检测次数
const unsigned int WAIT_DISTANCE_DEBOUNCE_COUNT = 300; // 等待区超声波距离防误检测次数
const unsigned int INVEST_DISTANCE_DEBOUNCE_COUNT = 20; // Investigation区域超声波距离防误检测次数
const unsigned long INVEST_DISTANCE_TIME = 1000; // 后退补偿时间(毫秒)

// 需要在全局变量区域（或者该函数外部）定义以下变量
unsigned char global_run_cycle = 1; // 大循环状态：1、2 或 3（根据你的描述，初始为1）
static boolean last_box_detected = false; // 记录上一周期是否检测到箱子，用于边缘去重
static unsigned int box_appearance_count = 0; // 当前赛道投掷阶段，超声波检测到箱子的总次数

// 速度与距离配置常量（统一管理，易于修改）
const float TRIGGER_DISTANCE = 25;           // 超声波触发距离阈值 (cm)
const int SPEED_NORMAL_BASE = 250;             // 正常循迹基础速度
const int SPEED_NORMAL_MAX = 255;              // 正常循迹最大速度
const int SPEED_SLOW_BASE = 200;               // 慢速循迹基础速度
const int SPEED_SLOW_MAX = 200;                // 慢速循迹最大速度
const int SPEED_BLIND = 255;                   // 盲冲执行速度
const unsigned long STRAIGHT_DURATION = 300;   // 丢失黑线后直行时间(毫秒)

// 状态切换函数
void changeState(RobotState nextState) {
  currentState = nextState;
  stateStartTime = millis();
  alignConfirmCount = 0;
  trackConfirmCount = 0;
  exitConfirmCount = 0;
}

// 传感器状态判断函数
bool isCenterDetected() {
  return sensor[0] == 1 || sensor[1] == 1 || sensor[2] == 1;
}

// 判断是否检测到全线（十字路口）
bool isAllLineDetected() {
  return sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1;
}

// 判断是否全线丢失
bool isAllLineLost() {
  return sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0;
}


void setup() {
  Serial.begin(9600);
  Infrared_recognition_Init();  // 初始化红外传感器
  motor_drive_Init();           // 初始化电机驱动
  robotic_arm_init();           // 初始化机械臂
  hc_sr04_init();              // 初始化超声波传感器
  motor_d_stop();
  changeState(STATE_wait);      // 初始状态
}

// ================= 各状态处理函数 =================

void handleStateWait() {
  static unsigned int waitDistanceConfirmCount = 0;
  float distance = hc_sr04_read_distance();
  
  if (distance > 0 && distance < TRIGGER_DISTANCE) {// 检测到物体进入范围
    waitDistanceConfirmCount++;// 增加确认计数
    if (waitDistanceConfirmCount >= WAIT_DISTANCE_DEBOUNCE_COUNT) {
      waitDistanceConfirmCount = 0;
      arm_grab_sequence(); // 调用封装好的抓取复位序列
      Serial.println("STATE_wait -> STATE_ALIGN");
      changeState(STATE_ALIGN);
    } else {
      motor_d_stop(); // 检测到但未达到确认次数，继续等待
    }
  } else {
    waitDistanceConfirmCount = 0; // 未检测到，清零计数器
    motor_d_stop(); // 没有检测到物体，保持等待状态
  }
}

void handleStateAlign() {
  motor_a_run(-255,255); // 开启左转(转并寻线)
  if (isCenterDetected()) {
    alignConfirmCount++;
    if (alignConfirmCount >= ALIGN_DEBOUNCE_COUNT) {
      changeState(STATE_TRACK_TO_GRID);
    }
  } else {
    alignConfirmCount = 0;
  }
}

void handleStateTrackToGrid() {// 开启正常速度PID循迹
  motor_control(SPEED_NORMAL_BASE, SPEED_NORMAL_MAX); // 开启正常速度PID循迹
  if (isAllLineDetected()) {
    trackConfirmCount++;
    if (trackConfirmCount >= TRACK_DEBOUNCE_COUNT) {
      changeState(STATE_GRID);
    }
  } else {
    trackConfirmCount = 0;
  }
}

void handleStateGrid() {// 检测到十字标志线后盲冲通过网格
  motor_a_run(SPEED_BLIND, SPEED_BLIND); // 左右同速盲冲
  if (millis() - stateStartTime >= BLIND_DURATION) {
    motor_d_stop(); // 盲冲结束，停止小车
    changeState(STATE_GRID_TO_INVEST);
  }
}

void handleStateGridToInvest() {
  static unsigned int investDistanceConfirmCount = 0; // 超声波距离防误计数器
  float distance = hc_sr04_read_distance();
  
  // 1. 判断超声波是否“处于检测到箱子的范围内”
  if (distance > 0 && distance < TRIGGER_DISTANCE) {
    investDistanceConfirmCount++;
    
    if (investDistanceConfirmCount >= INVEST_DISTANCE_DEBOUNCE_COUNT) {
      investDistanceConfirmCount = 0; // 清零防误计数器
      
      // 【关键去重逻辑】只有从“没检测到”变成“检测到”的瞬间，箱子出现次数才加 1
      if (last_box_detected == false) {
        box_appearance_count++;
        last_box_detected = true; // 标记当前持续处于箱子范围内
        Serial.print("检测到新箱子！当前区域累计箱子数: ");
        Serial.println(box_appearance_count);
      }
      
      // 2. 根据当前大的全局循环状态（1、2、3），决定在第几个箱子进行投掷
      boolean should_deposit = false;
      
      if (global_run_cycle == 1) {
        // 状态1：第1次检测到再进行投掷▲
        if (box_appearance_count == 1) {
          should_deposit = true;
          global_run_cycle = 2; // 投掷后将该状态变为2
        }
      } 
      else if (global_run_cycle == 2) {
        // 状态2：第3次检测到时投掷正方形
        if (box_appearance_count == 3) {
          should_deposit = true;
          global_run_cycle = 3; // 投掷后将该状态变为3
        }
      } 
      else if (global_run_cycle == 3) {
        // 状态3：第一次检测到箱子就投掷
        if (box_appearance_count == 3) {
          should_deposit = true;
          global_run_cycle = 1; // 投掷后将该状态变为1
        }
      }
      
      // 3. 执行机构动作控制
      if (should_deposit) {
        motor_d_stop(); // 停止小车
        arm_deposit_sequence(); // 执行投放动作序列
        
        // 准备进入下一阶段，将本次赛道的箱子计数器清零，去重标志复位
        box_appearance_count = 0;
        last_box_detected = false;
        
        Serial.print("投掷成功！大循环状态切换为: ");
        Serial.println(global_run_cycle);
        Serial.println("STATE_GRID_TO_INVEST -> STATE_INVEST_TO_wait");
        changeState(STATE_INVEST_TO_wait); 
      } else {
        // 检测到了箱子，但不是目标箱子（例如状态1下的第1个箱子），继续慢速循迹前行
        motor_control(SPEED_SLOW_BASE, SPEED_SLOW_MAX);  
      }
    } else {
      // 未满足防误次数，保持慢速循迹
      motor_control(SPEED_SLOW_BASE, SPEED_SLOW_MAX);  
    }
  } 
  // 4. 超声波未检测到箱子（距离超出范围）
  else {
    investDistanceConfirmCount = 0; // 清零防误计数器
    last_box_detected = false;      // 离开箱子范围，复位标志位，允许检测下一个箱子
    motor_control(SPEED_SLOW_BASE, SPEED_SLOW_MAX);  // 正常慢速循迹
  }
}

void handleStateInvestToWait() {//从投放区(Investigation)回等待区(Wait)
  // 使用状态机(phase)规范流程，增强可控性和防误触发稳定性
  static int phase = -1; // -1: 强制拉回黑线, 0: 第一段循迹, 1: 后退补偿, 2: 左转寻线, 3: 第二段循迹, 4: 切回初始
  static unsigned long actionStartTime = 0;// 各阶段动作开始时间
  static unsigned int whiteDebounceCount = 0;// 全白防误计数器
  static unsigned int lineDebounceCount = 0;// 寻线防误计数器

  // 可调参数：防误判次数与动作时间
  const unsigned int WHITE_DEBOUNCE_THRESHOLD = 200; // 全白防误判次数（连续检测到全白才确认）
  const unsigned int LINE_DEBOUNCE_THRESHOLD = 100;  // 重新找到黑线的防误判次数
  const unsigned long BACK_UP_TIME = 200;          // 冲出直角弯后的后退时间(毫秒)

  bool currentLost = isAllLineLost(); 
  bool centerLost = !isCenterDetected(); // 中间传感器从黑变白

  switch (phase) {
    case -1: // 阶段-1：前置安全接管。如果刚切入本状态时传感器在白区（因为刹车/惯性导致的车头冲出），则强制后退找回黑线
      if (currentLost) {
        motor_a_run(-200, -200); // 简单的强制退回
      } else {
        // 一旦至少压到一丁点黑线，说明车体回到安全循迹区，进入正式的阶段0
        lineDebounceCount++;
        if (lineDebounceCount >= 10) { 
          lineDebounceCount = 0;
          phase = 0; 
        }
      }
      break;

    case 0: // 阶段0：第一段PID循迹，等待遇到第一个全白（直角弯）
      // 判断条件改为：必须检测到中间传感器变白且全线丢失
      if (centerLost && currentLost) {
        whiteDebounceCount++;
        if (whiteDebounceCount >= WHITE_DEBOUNCE_THRESHOLD) {
          whiteDebounceCount = 0; // 清零给下次用
          actionStartTime = millis(); 
          phase = 1; // 确认全白，进入后退阶段
        }
      } else {
        whiteDebounceCount = 0;
        motor_control(SPEED_SLOW_BASE, SPEED_SLOW_MAX);
      }
      break;

    case 1: // 阶段1：严格后退一段时间
      if (millis() - actionStartTime < BACK_UP_TIME) {
        motor_a_run(-250, -250); 
      } else {
        phase = 2; // 后退时间到，进入寻线阶段
      }
      break;

    case 2: // 阶段2：原地左转，直到再次检测到黑线
      motor_a_run(-250, 250); 
      if (!currentLost) { // 只要不再是全白（压到一点黑线即可）
        lineDebounceCount++;
        if (lineDebounceCount >= LINE_DEBOUNCE_THRESHOLD) {
          lineDebounceCount = 0;
          phase = 3; // 确认找到黑线，进入第二段循迹
        }
      } else {
        lineDebounceCount = 0;
      }
      break;

    case 3: // 阶段3：第二段PID循迹，等待遇到第二个全白（终点停车区）
      // 判断条件改为：必须检测到中间传感器变白
      if (centerLost && currentLost) { // 确保全线丢失且中间传感器没有线
        whiteDebounceCount++;
        if (whiteDebounceCount >= WHITE_DEBOUNCE_THRESHOLD) {
          whiteDebounceCount = 0;
          phase = 4; // 确认达到终点全白，进入停车结束复位截断
        }
      } else {
        whiteDebounceCount = 0;
        motor_control(SPEED_SLOW_BASE, SPEED_SLOW_MAX);
      }
      break;

    case 4: // 阶段4：停车，重置标志位，切换回 Wait 系统状态
      motor_a_run(150,160);
      delay(STRAIGHT_DURATION); // 短暂前进确保完全进入停车区
      motor_d_stop();// 刹车停稳
      phase = -1; // 重置内部阶段为-1，保证下次送货能从头开始并执行安全检测
      Serial.println("STATE_INVEST_TO_wait -> STATE_wait");
      changeState(STATE_wait);
      break;
  }
}

// ================= 主循环 =================

void loop() {
  read_sensor_values();
  calculate_pid();

  switch (currentState) {
    case STATE_wait: 
      handleStateWait(); 
      break;
    case STATE_ALIGN: 
      handleStateAlign(); 
      break;
    case STATE_TRACK_TO_GRID: 
      handleStateTrackToGrid(); 
      break;
     case STATE_GRID: 
      handleStateGrid(); 
      break;
    case STATE_GRID_TO_INVEST: 
      handleStateGridToInvest(); 
      break;
    case STATE_INVEST_TO_wait: 
      handleStateInvestToWait(); 
      break;
  } 
}
