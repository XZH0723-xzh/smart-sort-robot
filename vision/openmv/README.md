# OpenMV 视觉脚本

本目录保存省赛阶段运行在 **OpenMV H7 Plus** 上的 MicroPython 脚本。视觉模块通过 UART3 与主控通信，用于二维图形识别、立体物块识别与形状匹配。

## 文件说明

| 文件 | 作用 |
| --- | --- |
| `main.py` | 主程序：状态 1 识别立体物块（白块黑底），状态 2 识别二维图形（黑图白底）并与目标比对 |
| `shape_detector.py` | 形状分类算法：基于最小外接矩形填充度区分 Square / Circle / Triangle |
| `Material_identification.py` | 立体物块材质 / 形状识别：输出 Prism / Cylinder / Cube |
| `detect_black_shapes.py` | 二维黑色图形识别独立示例，便于调阈值 |
| `ArduinoUart.py` | OpenMV 官方 UART 通信示例，用于验证接线与波特率 |

> `main.py` 通过 `from shape_detector import get_shape` 导入算法，部署时两个文件必须同时保存到 OpenMV 文件系统中。

## 接线

| OpenMV | Arduino Uno | 说明 |
| --- | --- | --- |
| `P4`（UART3 TX） | `D0`（RX） | OpenMV → 主控 |
| `P5`（UART3 RX） | `D1`（TX） | 主控 → OpenMV |
| `GND` | `GND` | 必须共地 |
| `VIN` / `5V` | `5V` | 按模块供电要求连接 |

- 波特率：**115200**；
- UART3 的 P4/P5 与 OpenMV IDE 的 USB 调试互不冲突；
- Arduino 的 D0/D1 与 USB 串口共用，烧录或串口调试时需断开 OpenMV，否则会出现占用冲突。

## 通信协议

主控 → OpenMV：

| 指令 | 含义 |
| --- | --- |
| `1` | 进入立体物块识别（白块黑底），并记录目标形状 |
| `2` | 进入二维图形识别（黑图白底），与状态 1 记录的目标形状比对 |

OpenMV → 主控：

| 返回 | 触发条件 |
| --- | --- |
| `Square\r\n` / `Circle\r\n` / `Triangle\r\n` | 状态 1 识别到立体物块的横截面形状 |
| `3\r\n` | 状态 2 识别到的二维图形与状态 1 的目标形状一致 |
| `Prism\r\n` / `Cylinder\r\n` / `Cube\r\n` | `Material_identification.py` 识别到的立体物块类型 |

## 形状分类阈值

`shape_detector.py` 计算色块像素面积与最小外接矩形面积的比值 `true_extent`：

| `true_extent` | 形状 |
| --- | --- |
| ≥ 0.86 | Square |
| 0.65 ~ 0.86 | Circle |
| < 0.65 | Triangle |

`Material_identification.py` 对立体物块使用另一组阈值：

| `true_extent` | 类型 |
| --- | --- |
| < 0.72 | Prism |
| 0.72 ~ 0.82 | Cylinder |
| > 0.82 | Cube |

阈值与现场光照、物块尺寸、拍摄距离相关，建议在比赛场地用 OpenMV IDE 的直方图与 `draw_debug` 输出重新标定。

## 部署步骤

1. 用 OpenMV IDE 连接 OpenMV H7 Plus；
2. 打开 `main.py`，另存到 OpenMV 文件系统为 `main.py`；
3. 将 `shape_detector.py` 保存到同一目录（OpenMV 根目录）；
4. 复位运行，IDE 串口终端会打印识别结果与 FPS；
5. 如需调整阈值，修改 `BLACK_THRESHOLD`（默认 `(0, 110)`）、`WHITE_THRESHOLD`（默认 `(140, 255)`）与 ROI（默认 `(60, 60, 200, 160)`）。

## 调试建议

- 关闭自动增益与自动白平衡，避免环境光变化导致阈值漂移；
- 固定 ROI，排除场地边缘与观众区域；
- 先运行 `detect_black_shapes.py` 观察色块与 `true_extent` 数值，再回到 `main.py`；
- 若识别抖动，可对连续多帧结果做投票，而不是单帧判定。
