# 调试例程

本目录保存比赛期间用于分模块验证的 Arduino 例程。它们**不是 PlatformIO 单元测试**，也不会被 `pio run` 自动编译；每个文件都是一个完整的 `setup()` / `loop()` 草图，用于单独验证传感器、执行器或某段流程。

| 文件 | 用途 |
| --- | --- |
| `01_ir_sensor_test.cpp` | 打印三路红外的模拟值，用于标定黑 / 白阈值 |
| `02_ultrasonic_test.cpp` | HC-SR04 测距输出 |
| `03_color_sensor_test.cpp` | TCS34725 颜色识别输出 |
| `04_color_ultrasonic_test.cpp` | 颜色 + 超声波联合测试（注意 A4/A5 引脚冲突） |
| `05_single_line_follow.cpp` | 最小循迹闭环：红外 + PID + 电机 |
| `06_straight_drive.cpp` | 电机直行测试 |
| `07_arm_test.cpp` | 单个舵机 / 机械臂关节测试 |
| `08_green_stop.cpp` | 循迹 + 全黑标志线 + 颜色停车流程 |
| `09_delivery_loop.cpp` | 多轮投放循环与全白 / 超声波条件 |
| `10_green_stop_reverse.cpp` | 识别绿色后短暂倒车抵消惯性，再停车 |
| `11_openmv_state_machine.cpp` | OpenMV 形状识别 + 抓取 / 投放状态机（早期版本） |
| `12_main_early_version.cpp` | 最终主程序的早期版本（盲冲时间等参数不同） |

## 使用方式

这些草图依赖 `autonomous-robot/lib/` 下的项目库。若要单独编译某一个：

1. 备份 `src/main.cpp`；
2. 将目标例程复制为 `src/main.cpp`；
3. 执行 `pio run -t upload`；
4. 测试完成后恢复原 `main.cpp`。

## 注意

- `08_green_stop.cpp`、`09_delivery_loop.cpp`、`10_green_stop_reverse.cpp` 同时使用颜色传感器与超声波，而两者的引脚在 `pinout.h` 中重叠（A4/A5），实际运行前需改线；
- 部分例程是比赛现场的中间版本，变量命名与注释风格不统一，仅作归档参考；
- 例程中的参数（速度、延时、阈值）与最终版 `src/main.cpp` 可能不同。
