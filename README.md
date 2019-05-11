# STM32_OLED12864_OV7670
show live stream captured by OV7670 on OLED12864

This project uses Roger Clark's Arduino core for stm32, see: https://github.com/rogerclarkmelbourne/Arduino_STM32

wire up:
OV7670
 A8 - MCLK
 PB4 - PCLK
 PB5 - vsync
 PB6 - i2c Clock
 PB7 - i2c data
 PB8..PB15 D0-D7
 
OLED12864
 DC      PA1
 CS      PA2
 RESET   PA4
 SCL/D0  PA5
 SDA/D1  PA7
