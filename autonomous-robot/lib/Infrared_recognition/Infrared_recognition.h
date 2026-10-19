#ifndef Infrared_recognition_h
#define Infrared_recognition_h

void Infrared_recognition_Init(); // 红外传感器初始化函数
void Infrared_recognition_Read(); // 读取红外传感器数据函数
void Infrared_recognition_Read_Gray(); // 读取红外传感器模拟值并转化为数字量函数
void read_sensor_values(); // 读取红外传感器值并更新误差函数
void record_black_count(); // 记录三路传感器全黑次数函数

#endif
