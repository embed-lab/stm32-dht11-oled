# STM32F103C8T6 DHT11温湿度采集 + 0.96寸OLED显示

> 嵌入式寒假实习练手项目，标准库 V3.5，Keil MDK5

## 硬件清单
- STM32F103C8T6 最小系统板
- DHT11 温湿度传感器
- 0.96寸 I2C OLED 屏
- USB-TTL 下载器

## 接线
- DHT11 DATA → PA0
- OLED SDA → PB7
- OLED SCL → PB6
- 共 GND，5V 供电

## 软件环境
- Keil MDK5
- STM32F10x 标准外设库 V3.5.0
- 串口调试助手（可选）

## 功能说明
1. DHT11 周期性读取环境温度、湿度
2. 数据实时刷新在 OLED 屏幕上
3. 1 秒刷新一次采集数据

## 使用方法
1. 使用 Keil 打开 `stm32f10x.uvprojx`
2. 确认芯片包 STM32F103C8 已安装
3. 编译，使用 JLink / ST-Link 下载到开发板
4. 上电，OLED 显示温湿度

## 项目难点
- DHT11 单总线时序精准延时
- 软件 I2C 驱动 OLED 屏幕
- 标准库工程环境搭建、头文件路径配置
