#include "openmv.h"

/*************************************************
 * @brief  OpenMV 通信初始化
 * @param  baud_rate: 通信波特率（需与 OpenMV 代码中保持一致，比如 115200）
 * @retval 无
 * @note   这里默认使用了 Arduino Uno 的硬件串口 Serial。
 *         如果后续使用 Mega 等拥有多个串口的板子，可以变更为 Serial1/2/3 等。
 *************************************************/
void openmv_init(long baud_rate) {
    // 为了不重复初始化，如果在 main 里面已经调用过 Serial.begin 则这里可简化。
    // 安全起见保留它，如果波特率一致就不会有问题
    Serial.begin(baud_rate);
}

/*************************************************
 * @brief  向 OpenMV 发送当前执行状态的请求指令
 * @param  cmd: 指令编号 (1 或 2)
 *              1: 让 OpenMV 开启 3D 立体物块识别
 *              2: 让 OpenMV 开启 2D 收纳图签识别匹配
 * @retval 无
 *************************************************/
void openmv_send_command(int cmd) {
    Serial.println(cmd);
}

/*************************************************
 * @brief  检查是否有来自 OpenMV 的数据
 * @param  无
 * @retval true表示有数据，false表示无数据
 *************************************************/
bool openmv_is_available() {
    return Serial.available() > 0;
}

/*************************************************
 * @brief  读取 OpenMV 返回的当前帧数据信息
 * @param  无
 * @retval 返回识别到的标志或形状 (如 "Circle", "Triangle", "3" 等)
 * @note   自动处理掉两端的换行符和空格空白残留
 *************************************************/
String openmv_read_message() {
    if (Serial.available() > 0) {
        String msg = Serial.readStringUntil('\n');
        msg.trim(); // 清除开头和末尾的空格、\r、\n 等不可见字符
        return msg;
    }
    return "";
}
