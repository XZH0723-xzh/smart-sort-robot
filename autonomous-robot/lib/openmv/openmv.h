#ifndef OPENMV_H
#define OPENMV_H

#include <Arduino.h>

// 初始化与 OpenMV 的通信
void openmv_init(long baud_rate);

// 向 OpenMV 发送状态切换指令
void openmv_send_command(int cmd);

// 检查 OpenMV 是否有数据返回
bool openmv_is_available();

// 读取 OpenMV 返回的字符串数据
String openmv_read_message();

#endif
