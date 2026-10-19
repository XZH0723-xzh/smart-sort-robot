import sensor, image, time
from pyb import UART
from shape_detector import get_shape # 导入我们刚封装的神级形状识别算法!

# 初始化串口 3，波特率 115200 (引脚: P4 为 TX, P5 为 RX)
uart = UART(3, 115200)

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)      # 320x240
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

clock = time.clock()

# 将感兴趣区域局限在只包含黑底或白底的中央部分，排除外界干扰
ROI = (60, 60, 200, 160)

WHITE_THRESHOLD = (140, 255) # 用于识别立体物块（白块黑底）
BLACK_THRESHOLD = (0, 110)    # 用于识别二维平面图形（黑形白底）

# 状态机：
# state = 0: 待机状态 (连Arduino时用)
# state = 1: 正在识别 3D 物体 (白块黑底)
# state = 2: 正在识别 2D 图形 (黑图白底)

# --- 电脑调试开关 ---
# 既然现在只和电脑通信，就把默认状态改成 2 (直接不停地识别黑图形)
# 或者你想测立体物块就把它改成 1
state = 2

target_shape = "" # 记忆在状态 '1' 中识别到的立体图形所对应的横切面（目标图形）

while(True):
    clock.tick()#   计算帧率
    img = sensor.snapshot()#    拍摄一帧图像
    img.draw_rectangle(ROI, color = 127)#    img.draw_string(ROI[0], ROI[1] - 15, "ROI", color=127)

    # --- 接收串口控制指令 ---
    if uart.any():
        # 读取串口数据并查找是否包含指令字符
        recv_data = uart.read() # 读取所有可用数据
        if recv_data:   # 确保数据非空
            if b'1' in recv_data:#  如果收到 '1'，进入状态 1，准备识别立体物块
                state = 1
                target_shape = "" # 重新开始识别新立体物块，清空记忆
                print(">>> 收到指令 1：开始识别立体物块(白块黑底)")
            elif b'2' in recv_data:
                state = 2
                print(f">>> 收到指令 2：开始识别并比对二维图形 (黑图白底). 当前需比对的目标: {target_shape}")

    # ================= 状态 1：识别立体物块 =================
    if state == 1:
        blobs = img.find_blobs([WHITE_THRESHOLD], roi=ROI, pixels_threshold=200, area_threshold=200, merge=True)
        if blobs:
            # 提取面积最大的一个色块以防干扰
            largest_blob = max(blobs, key=lambda b: b.pixels())
            bx, by, bw, bh = largest_blob.rect()

            # 直接调用模块化的算法进行识别！
            shape_name = get_shape(largest_blob, img=img, draw_debug=True)

            if shape_name != "Unknown":
                target_shape = shape_name
                # 发送物块的形状特征
                uart.write(f"{shape_name}\r\n")
                print(f"----> 立体物块识别成功，特征: {shape_name}")
                # state = 0 # 【电脑调试期间将其注释掉】：这样就不会检测一次就停机，会一直疯狂检测并打印在IDE里

            img.draw_string(bx, by - 15, f"{shape_name} (3D)", color=255, scale=2)

    # ================= 状态 2：识别二维图形与比对 =================
    elif state == 2:
        blobs = img.find_blobs([BLACK_THRESHOLD], roi=ROI, pixels_threshold=30, area_threshold=30, merge=True)
        if blobs:
            largest_blob = max(blobs, key=lambda b: b.pixels())
            img.draw_rectangle(largest_blob.rect(), color=255)
            img.draw_cross(largest_blob.cx(), largest_blob.cy(), color=255)

            bx, by, bw, bh = largest_blob.rect()

            # 同样直接调用模块化的算法进行识别！
            current_shape = get_shape(largest_blob, img=img, draw_debug=True)

            img.draw_string(bx, by - 15, current_shape, color=255, scale=2)

            # 比对：若识别出来的特征，与第一步(信号1)识别到的图形一致
            if current_shape == target_shape and target_shape != "":
                # 发送匹配成功信号 3
                uart.write("3\r\n")
                print(f"----> [完全匹配成功！] 二维图形({current_shape})与目标一致！发送信号 3")
                # state = 0 # 【电脑调试期间将其注释掉】：防止匹配成功一次就罢工
            elif current_shape != "Unknown":
                # 在电脑调试时，只要识别出来的不是Unknown，就疯狂在终端打印它看到了什么
                print(f"终端直显: 当前画面中识别到的形状是 ===> {current_shape}")

    # print("FPS: ", clock.fps())

