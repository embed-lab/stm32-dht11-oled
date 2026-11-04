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

## 工程结构
```
stm32-dht11-oled/
├── Core/                # main.c 主程序
├── Driver/              # 外设驱动
│   ├── delay.c / .h      # 基于 SysTick 的 us/ms/s 延时
│   ├── i2c.c / .h        # 软件 I2C（PB6/SCL，PB7/SDA）
│   ├── oled.c / .h       # OLED(SSD1306) 驱动 + 显示接口
│   ├── oledfont.h        # 8x16 ASCII 点阵字库（脚本生成）
│   └── dht11.c / .h      # DHT11 单总线温湿度读取
├── Libraries/           # STM32F10x_StdPeriph_Driver（标准库，需自行放入）
├── Output/              # Keil 编译输出（已写入 .gitignore，不上传）
├── stm32f10x.uvprojx    # Keil 工程文件（在 Keil 中创建）
└── .gitignore
```

## 功能说明
1. DHT11 周期性读取环境温度、湿度
2. 数据实时刷新在 OLED 屏幕上
3. 1 秒刷新一次采集数据

## 编译前准备（重要）
本仓库只含项目源码与驱动，**不包含 ST 标准外设库**。编译前请把
`STM32F10x_StdPeriph_Lib_V3.5.0` 解压后的 `Libraries/` 目录内容
（含 `STM32F10x_StdPeriph_Driver/` 与 `CMSIS/`）复制到本工程的 `Libraries/` 中。

Keil 工程配置要点：
- 芯片：STM32F103C8
- 全局宏：`STM32F10X_MD, USE_STDPERIPH_DRIVER`
- 头文件包含路径：`./Core;./Driver;./Libraries/STM32F10x_StdPeriph_Driver/inc;./Libraries/CMSIS/Include`（按实际目录调整）
- 加入文件：`Core/main.c`、`Driver/*.c`、标准库的 `src/*.c`、`system_stm32f10x.c`、`startup_stm32f10x_md.s`

## 使用方法
1. 使用 Keil 打开（或新建）`stm32f10x.uvprojx`，按上述配置加入文件
2. 确认芯片包 STM32F103C8 已安装
3. 编译，使用 JLink / ST-Link 下载到开发板
4. 上电，OLED 显示温湿度

## 项目难点
- DHT11 单总线时序精准延时（`delay.c` 基于 SysTick）
- 软件 I2C 驱动 OLED 屏幕（`i2c.c` + `oled.c`）
- 标准库工程环境搭建、头文件路径配置
