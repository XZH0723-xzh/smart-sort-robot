# 第三方组件与许可说明

本仓库根目录的 [LICENSE](LICENSE)（MIT）仅适用于本项目原创代码。以下第三方组件按其原始许可证分发，不适用 MIT 条款：

| 组件 | 位置 | 许可证 | 来源 / 说明 |
| --- | --- | --- | --- |
| MH_TCS34725 | `autonomous-robot/lib/MH_TCS34725/` | GNU LGPL | DFRobot / MH ColorSensor 库，头文件与 README 中保留原始版权声明 |
| SoftwareSerial | `remote-control-robot/lib/SoftwareSerial/` | GNU LGPL 2.1 | Arduino 软件串口库，源文件头部保留原始版权声明 |
| ServoTimer2 | 由 PlatformIO 按 `platformio.ini` 拉取，不随仓库分发 | 见上游仓库 | `nabontra/ServoTimer2` |
| ArduinoUart.py | `vision/openmv/ArduinoUart.py` | MIT | OpenMV LLC 官方示例，文件头部保留原始版权声明 |

如果重新分发本仓库，请同时保留上述组件的版权声明与许可证文本。
