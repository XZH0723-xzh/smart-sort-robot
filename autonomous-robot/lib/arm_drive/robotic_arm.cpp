#include "robotic_arm.h"

MyServo pitchServo; // 1轴 俯仰
MyServo reachServo; // 2轴 伸缩
MyServo clawServo;  // 3轴 夹取

// 复位中位角度
int pitch_mid = 90;// 俯仰中位角度
int reach_mid = 0;// 伸缩中位角度 (完全收回)最大43
int claw_open = 50;// 夹爪打开角度，数值越小越开，最大90（完全闭合）
int claw_close = 90;// 夹爪闭合角度，数值越大越紧，最小0（完全打开）

/*************************************************
 * @brief  机械臂初始化函数
 * @param  无
 * @retval 无
 * @note  初始化机械臂的舵机连接，并将机械臂复位到初始位置
 **************************************************/
void robotic_arm_init() {
    // 根据pinout.h中的定义初始化舵机连接，如果没有定义则使用默认引脚
#ifdef Servo_Pitch_PIN
    pitchServo.attach(Servo_Pitch_PIN);
#else
    pitchServo.attach(4);
#endif

#ifdef Servo_Reach_PIN
    reachServo.attach(Servo_Reach_PIN);
#else
    reachServo.attach(7);
#endif

#ifdef Servo_Claw_PIN
    clawServo.attach(Servo_Claw_PIN);
#else
    clawServo.attach(11);
#endif

    armReset();// 复位机械臂到初始位置
}

/*************************************************
 * @brief  机械臂复位函数
 * @param  无
 * @retval 无  
 * @note  将机械臂的各个关节复位到中位角度，并打开夹爪
 **************************************************/
void armReset()
{
  pitchServo.write(pitch_mid);
  reachServo.write(reach_mid);
  clawServo.write(claw_open);
  delay(800);// 等待机械臂复位完成
}

/*************************************************
 * @brief  改变指定关节角度（平滑过渡）
 * @param  joint: 关节编号 (0: 俯仰, 1: 伸缩, 2: 夹爪)
 * @param  angle: 目标角度 (0-270)
 * @param  step_delay: 每次微调的延时(毫秒)，数值越大越慢，默认 15
 * @retval 无
 **************************************************/
void setJointAngle(int joint, int angle, int step_delay) {
    // 限制角度在安全范围内，防止机械臂过度伸展或夹爪过紧
    angle = constrain(angle, 0, 270);
    
    // 获取对应的舵机对象，并根据关节编号选择正确的舵机
    MyServo* targetServo = nullptr;
    switch (joint) {
        case JOINT_PITCH: targetServo = &pitchServo; break;
        case JOINT_REACH: targetServo = &reachServo; break;
        case JOINT_CLAW:  targetServo = &clawServo; break;
        default: return; // 错误关节编号直接返回
    }

    // 获取当前角度
    int currentAngle = targetServo->read();
    
    // 步进式旋转到目标角度
    if (currentAngle < angle) {
        for (int i = currentAngle; i <= angle; i++) {
            targetServo->write(i);
            delay(step_delay);
        }
    } else {
        for (int i = currentAngle; i >= angle; i--) {
            targetServo->write(i);
            delay(step_delay);
        }
    }
}

/*************************************************
 * @brief  抓取动作序列 (STATE_GRAB状态下调用)
 * @param  无
 * @retval 无
 * @note  执行一系列预设的动作来完成抓取和复位，适用于抓取物体后将其放回原位的场景
 **************************************************/
void arm_grab_sequence() {
    // 示例动作序列：先伸展到物体位置，夹取物体，然后复位
    setJointAngle(JOINT_CLAW, claw_open, 10);     // 打开夹爪
    delay(1000);
    setJointAngle(JOINT_PITCH, 10, 50);   // 调整俯仰角度以适应物体高度
    delay(5000);
    setJointAngle(JOINT_REACH, 45, 10);  // 伸展到物体位置

    delay(2000);
    setJointAngle(JOINT_CLAW, claw_close, 10);    // 关闭夹爪，夹取物体
    delay(200);
    setJointAngle(JOINT_PITCH, 90, 10);   // 复位俯仰角度
    setJointAngle(JOINT_REACH, 0, 10);   // 复位伸缩位置
    delay(200);
}

/*************************************************
 * @brief  投放动作序列 (STATE_DEPOSIT状态下调用)
 * @param  无
 * @retval 无
 * @note  执行一系列预设的动作来完成投掷操作，放缓动作防翻车
 **************************************************/
void arm_deposit_sequence() {
    // 放缓速度伸展
    setJointAngle(JOINT_CLAW, claw_close, 30);
    setJointAngle(JOINT_REACH, 50, 30);  // 伸展到投放位置
    setJointAngle(JOINT_PITCH, 40, 30);   // 调整俯仰角度以适应投放位置
    delay(500);
    setJointAngle(JOINT_CLAW, claw_open, 30);     // 打开夹爪，释放物体
    delay(500);
    
    // 复位机械臂到初始位置，准备下一轮操作
    armReset(); 
}
