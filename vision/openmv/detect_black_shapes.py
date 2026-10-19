# 这个示例代码演示了如何使用 OpenMV 摄像头识别黑色的二维图形（正方形、圆形、三角形）
# 并通过串口发送识别结果给OPENMV IDE串口
import sensor, image, time  # 导入必要的库

from pyb import UART # 导入 UART 模块用于串口通信

# 初始化串口 3，波特率 115200 (引脚: P4 为 TX, P5 为 RX)
uart = UART(3, 115200)

# 1. 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE) # 依然使用灰度图，对比度最高
sensor.set_framesize(sensor.QVGA)      # 320x240
sensor.skip_frames(time = 2000)        # 等待摄像头稳定
sensor.set_auto_gain(False)            # 关闭自动增益，避免环境光变化导致的图像亮度波动
sensor.set_auto_whitebal(False)        # 关闭自动白平衡，保持图像色调稳定

clock = time.clock()                   # 创建一个时钟对象用于计算帧率 (FPS)

# 2. 划定感兴趣区域 (ROI: Region of Interest) 排除外边缘干扰
ROI = (60, 60, 200, 160)

# 3. 设置黑色的阈值范围 [灰度最小值, 灰度最大值]
# 这里的 0, 10 代表非常暗的区域，用于在白色背景中提取黑色图形
BLACK_THRESHOLD = (0, 120)# 你可以根据实际环境调整这个阈值范围，确保能够准确捕捉到黑色图形

while(True):
    clock.tick()# 计算帧率
    img = sensor.snapshot()# 捕获一帧图像

    # 绘制感兴趣区域以便调试
    img.draw_rectangle(ROI, color = 127)# 在图像上画出 ROI 区域的边框，颜色为灰色 (127)，方便你在调试时看到这个区域的位置和大小

    # 4. 在 ROI 区域内寻找黑色斑块 (blobs)
    # 因为你要识别的黑色图形由于距离或比例原因在画面中显得较小，
    # 这里将像素（pixels_threshold）和面积（area_threshold）的过滤阈值大幅降低，防止小图形被当成噪点过滤掉
    blobs = img.find_blobs([BLACK_THRESHOLD], roi=ROI, pixels_threshold=30, area_threshold=30, merge=True)

    if blobs:
        # 提取像素面积最大的一个色块，确保只识别一个主要图形
        largest_blob = max(blobs, key=lambda b: b.pixels())

        # 绘制外接矩形和中心点
        img.draw_rectangle(largest_blob.rect(), color=255)
        img.draw_cross(largest_blob.cx(), largest_blob.cy(), color=255)

        import math
        bx, by, bw, bh = largest_blob.rect()
        shape_name = "Unknown"

        # --- 终极识别算法：最小二乘旋转框 (Min Bounding Box) 面积特征法 ---
        # 抛弃容易被反光干扰的直线检测，改用完全免受平面旋转影响的几何定律
        corners = largest_blob.min_corners()

        # 调试用：在被测物体周围画出贴合其边缘的最小倾斜矩形（白框）
        # 即使你把三角形/正方形斜着放，外面的这个宽会紧紧跟着斜着包围它
        for i in range(4):
            img.draw_line(corners[i][0], corners[i][1], corners[(i+1)%4][0], corners[(i+1)%4][1], color=255, thickness=2)

        # 计算这个"倾斜外框"的真实物理宽和高
        w_rot = math.sqrt((corners[1][0] - corners[0][0])**2 + (corners[1][1] - corners[0][1])**2)
        h_rot = math.sqrt((corners[2][0] - corners[1][0])**2 + (corners[2][1] - corners[1][1])**2)

        box_area = w_rot * h_rot
        if box_area <= 0: box_area = 1

        # 核心算式：真占比 = 图形自身实心面积 / 旋转框面积
        # 因为旋转框不论怎么斜都紧紧贴着物品，所以这个真占比是恒定不变的：
        # 三角形永远约等于 0.5，圆形永远约等于 0.785，正方形永远约等于 1.0!
        true_extent = largest_blob.pixels() / box_area

        if true_extent >= 0.86:
            shape_name = "Square"
        elif 0.65 <= true_extent < 0.86:
            shape_name = "Circle"
        else:
            shape_name = "Triangle"

        # 屏幕显示与串口/电脑终端打印输出
        img.draw_string(bx, by - 15, shape_name, color=255, scale=2)
        # 将最新的特征占比(T_Ext)打印出来，用于你直观微调阈值
        img.draw_string(bx, by + bh + 5, f"T_Ext:{true_extent:.2f}", color=255, scale=1.5)

        # 物理串口发送给Arduino
        uart.write(f"{shape_name}\r\n")

        # 打印到电脑 IDE 的串行终端
        print(f"----> 识别成功! 黑色目标形状: {shape_name}")

    # IDE端查看FPS
    print("FPS: ", clock.fps())
