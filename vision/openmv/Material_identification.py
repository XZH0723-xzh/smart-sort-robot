import sensor, image, time
from pyb import UART

# 初始化串口 3，波特率 115200 (引脚: P4 为 TX, P5 为 RX)
uart = UART(3, 115200)

# 1. 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)      # 320x240
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

clock = time.clock()

# 我们将感兴趣区域局限在只包含黑色的部分，排除外界干扰
ROI = (20, 20, 280, 200) 
WHITE_THRESHOLD = (140, 255) # 白色物体的灰度阈值

def get_angle_diff(a1, a2):
    """计算两个角度的最小夹角 (OpenMV中角度在0~179度之间)"""
    diff = abs(a1 - a2)
    if diff > 90:
        diff = 180 - diff
    return diff

while(True):
    clock.tick()
    img = sensor.snapshot()
    img.draw_rectangle(ROI, color = 127)

    # 1. 先用色块寻找锁定白色物体的范围
    blobs = img.find_blobs([WHITE_THRESHOLD], roi=ROI, pixels_threshold=200, area_threshold=200, merge=True)
    
    if blobs:
        # 只提取面积最大的那一个白色物块，忽略其它较小的反光或假块
        largest_blob = max(blobs, key=lambda b: b.pixels())
        
        bx, by, bw, bh = largest_blob.rect()
        
        # 核心改动：获取能【跟随物体一起旋转】的最小倾斜外接矩形 (MER: Minimum Enclosing Rectangle)
        min_corners = largest_blob.min_corners()
        
        # 在画面中把倾斜外接框画出来（连线），让你直观看到它是永远紧紧贴合物块的
        for i in range(4):
            img.draw_line(min_corners[i][0], min_corners[i][1], min_corners[(i+1)%4][0], min_corners[(i+1)%4][1], color=255, thickness=2)
            
        # 计算这个倾斜(抗旋转)外接矩形的实际面积：宽*高 (勾股定理计算边长)
        w_min = ((min_corners[0][0] - min_corners[1][0])**2 + (min_corners[0][1] - min_corners[1][1])**2) ** 0.5
        h_min = ((min_corners[1][0] - min_corners[2][0])**2 + (min_corners[1][1] - min_corners[2][1])**2) ** 0.5
        mer_area = w_min * h_min
        
        # --- 旋转无视法：抗旋转占位面积比 (Rotated-Extent) ---
        # 无论物块怎么转，倾斜外框都会跟着转，从而分母 mer_area 保持稳定极小！
        extent = largest_blob.pixels() / mer_area if mer_area > 0 else 0
        roundness = largest_blob.roundness()
        
        shape_name = "Unknown"
        
        # 因为我们改用了抗倾斜包装盒，这三个占位比将会恢复到你正位摆放时的绝对真理！
        # 1. 三棱柱 (Prism): 倾斜框里的三角形就算贴得再紧，也总是空着一半区域，恒定在 0.67 左右
        if extent < 0.72:
            shape_name = "Prism"
            
        # 2. 圆柱体 (Cylinder): 柱型带弧帽，居中，恒定在 0.78 左右
        elif 0.72 <= extent <= 0.82:
            shape_name = "Cylinder"
            
        # 3. 正方体 (Cube): 无论倾斜导致它变成了正方形还是菱形，倾斜框贴身包裹它都是严丝合缝！恒定 > 0.84
        else: # extent > 0.82
            shape_name = "Cube"
                
        # 屏幕显示：在图形上方输出最终判定，在下方输出特征数值供调试
        img.draw_string(bx, by - 15, shape_name, color=255, scale=2)
        img.draw_string(bx, by + bh + 5, f"R-Ext:{extent:.2f}", color=255, scale=1.5)
        
        # --- 输出识别结果 ---
        # 1. 经过物理引脚发送给控制板 (Arduino等)
        uart.write(f"{shape_name}\r\n")
        # 2. 经过 USB 发送到电脑，用于在“电脑串口助手或终端”中查看结果
        print(f"----> 识别成功! 目标形状: {shape_name} | 特征值-> 占宽比: {extent:.2f}, 圆度: {roundness:.2f}")

    print("FPS: ", clock.fps())