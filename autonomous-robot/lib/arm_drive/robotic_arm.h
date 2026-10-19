#ifndef __ROBOTIC_ARM_H__
#define __ROBOTIC_ARM_H__

#include <Arduino.h>
#include <ServoTimer2.h>
#include "pinout.h"

// 继承并封装 ServoTimer2，使其接口兼容标准 Servo（支持 0-270 度读写）
class MyServo : public ServoTimer2 {
public:
    void write(int degrees) {
        int us = map(degrees, 0, 270, 500, 2500);
        ServoTimer2::write(us);
    }
    int read() {
        int us = ServoTimer2::read();
        if (us == 0) return 0;
        return map(us, 500, 2500, 0, 270);
    }
};

// 关节枚举，方便使用
enum ArmJoint {
    JOINT_PITCH = 0, // 俯仰
    JOINT_REACH = 1, // 伸缩
    JOINT_CLAW  = 2  // 夹取
};

void robotic_arm_init();
void armReset();
void setJointAngle(int joint, int angle, int step_delay = 15);

// 核心动作抓取和投放
void arm_grab_sequence();
void arm_deposit_sequence();

#endif // __ROBOTIC_ARM_H__
