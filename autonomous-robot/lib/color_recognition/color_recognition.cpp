#include <Arduino.h>
#include <Wire.h>        //调用IIC库函数

#include "MH_TCS34725.h" //调用颜色识别传感器库函数

#ifdef __AVR__

  #include <avr/power.h>

#endif

MH_TCS34725 tcs = MH_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); //设置颜色传感器采样周期50毫秒
//MH_TCS34725 tcs = MH_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);//设置颜色传感器采样周期700毫秒
//MH_TCS34725 tcs = MH_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);//设置颜色传感器采样周期50毫秒（不推荐）
//MH_TCS34725 tcs = MH_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);//设置颜色传感器采样周期50毫秒（不推荐）

/*************************************************
 * @brief  颜色识别传感器初始化函数
  * @param  无
 * @retval 无
 * @note  初始化颜色识别传感器，并检测是否有颜色识别传感器模块，如果没有检测到颜色识别传感器模块，程序将陷入死循环
 *************************************************/
void color_recognition_Init() {

  Serial.begin(115200);   //开启串口，并设置串口波特率位115200

  Serial.println("Color View Test!"); //串口打印：Color View Test!

  //检测是否有颜色传感器模块

  if (tcs.begin()) {                 //如果检测到颜色传感器模块

    Serial.println("Found sensor");   //串口打印 Found sensor

  } else {                           //如果没有检测到颜色传感器模块

    Serial.println("No TCS34725 found ... check your connections");//串口打印：没有找到颜色识别传感器模块

    while (1); // halt! //程序陷入死循环
  }

}

String get_color(){
    String color_data = "";
    uint16_t clear, red, green, blue; //分别定义用于存储红、绿、蓝三色值变量
    tcs.getRGBC(&red, &green, &blue, &clear); //将原始R/G/B值转换为色温（以度为单位）
    tcs.lock();  //禁用中断（可省略）
    uint32_t sum = clear;
    float r, g, b;                  // 计算红
    r = red; r /= sum;              // 绿、蓝
    g = green; g /= sum;            // 三色数
    b = blue; b /= sum;             // 值
    r *= 256; g *= 256; b *= 256;   //===========
    if( (r > 0) && (g>0) && (b>0) && (r>g) && (r>b) )//如果红色数值大于绿色数值和蓝色数值，则认为识别到的颜色为红色
    {
        color_data = "red";
    }

    else if( (r > 0) && (g>0) && (b>0) && (r<g) && (b<g) )//如果绿色数值大于红色数值和蓝色数值，则认为识别到的颜色为绿色
    {
        color_data = "green";
    }
  
  else if( (r > 0) && (g>0) && (b>0) && (r<b) && (g<b) )//如果蓝色数值大于红色数值和绿色数值，则认为识别到的颜色为蓝色
    {
        color_data = "blue";
    }

  else{                        
        color_data = "none";//如果红、绿、蓝三色数值都不大于0，或者红色数值不大于绿色数值和蓝色数值，或者绿色数值不大于红色数值和蓝色数值，或者蓝色数值不大于红色数值和绿色数值，则认为没有识别到颜色
    }
    return color_data;
}