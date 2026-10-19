#include <Arduino.h>
#include "hc_sr04.h"
#include "pinout.h"


/**
 * @brief  超声波传感器初始化函数
 * @param  无
 * @retval 无
 * @note  初始化超声波传感器的引脚模式
 */
void hc_sr04_init() {
    pinMode(TRIGGER, OUTPUT);
    pinMode(ECHOPIN, INPUT);
}


/**
 * @brief  超声波传感器触发函数
 * @param  无
 * @retval 无
 * @note  触发超声波传感器进行测距
 */
void hc_sr04_trigger() {
    digitalWrite(TRIGGER, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER, LOW);
}

/**
 * @brief  超声波传感器测距函数
 * @param  无
 * @retval 距离值（单位：厘米）
 * @note  读取超声波传感器的回响时间并计算距离
 */
float hc_sr04_read_distance() {
    hc_sr04_trigger(); // 先发送超声波触发信号
    long duration = pulseIn(ECHOPIN, HIGH);  
    float distance = (duration * 0.034) / 2; // 声速为340米/秒，除以2是因为往返时间
    return distance;
}

