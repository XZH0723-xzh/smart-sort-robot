# 本作品采用MIT许可证授权。
# 版权所有 (c) 2013-2023 OpenMV LLC。保留所有权利。
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# OpenMV 和 Arduino Uno 之间的基本 UART 通信。

# 1) 将你的 OpenMV Cam 连接到 Arduino Uno，如下所示：
#
# OpenMV Cam 地线 ----> Arduino 地线
# OpenMV Cam UART3_TX(P4) ----> Arduino Uno UART_RX(0)
# OpenMV Cam UART3_RX(P5) ----> Arduino Uno UART_TX(1)

# 2) 取消注释并将以下代码上传到 Arduino：
#
# void setup() {
#   // put your setup code here, to run once:
#   Serial.begin(19200);
# }
#
# void loop() {
#   // put your main code here, to run repeatedly:
#   if (Serial.available()) {
#     // Read the most recent byte
#     byte byteRead = Serial.read();
#     // ECHO the value that was read
#     Serial.write(byteRead);
#   }
# }

# 3) Run the following script in OpenMV IDE:

import time
from pyb import UART

# UART 3, and baudrate.
uart = UART(3, 19200, timeout_char=200)

while True:
    uart.write("Hello World!\n")
    if uart.any():
        print(uart.read())
    time.sleep_ms(1000)