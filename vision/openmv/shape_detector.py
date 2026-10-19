import math

def get_shape(blob, img=None, draw_debug=True):
    """
    输入一个 blob（色块）对象，使用“最小二乘旋转框 (Min Bounding Box)”算法识别其形状。
    返回: "Square", "Circle", "Triangle" （字符串）
    
    参数:
    - blob: 从 img.find_blobs() 提取出来的单个色块对象
    - img: 当前帧的图像对象，用于在屏幕上画线调试（默认需要传入）
    - draw_debug: 是否在图像上画出旋转框和数值，默认为 True
    """
    corners = blob.min_corners()
    
    # 画出贴边旋转框，用于直观展示识别算法
    if draw_debug and img:
        for i in range(4):
            img.draw_line(corners[i][0], corners[i][1], corners[(i+1)%4][0], corners[(i+1)%4][1], color=255, thickness=2)

    # 计算倾斜框的物理长宽
    w_rot = math.sqrt((corners[1][0] - corners[0][0])**2 + (corners[1][1] - corners[0][1])**2)
    h_rot = math.sqrt((corners[2][0] - corners[1][0])**2 + (corners[2][1] - corners[1][1])**2)
    
    box_area = w_rot * h_rot
    if box_area <= 0: 
        box_area = 1
        
    # 计算免旋转的实体比例 (True Extent)
    true_extent = blob.pixels() / box_area

    if true_extent >= 0.86:
        shape_name = "Square"
    elif 0.65 <= true_extent < 0.86:
        shape_name = "Circle"
    else:
        shape_name = "Triangle"

    # 在图形底部打印具体的比例值，方便在 IDE 中精准微调阈值
    if draw_debug and img:
        bx, by, bw, bh = blob.rect()
        img.draw_string(bx, by + bh + 5, f"T_Ext:{true_extent:.2f}", color=255, scale=1.5)

    return shape_name
