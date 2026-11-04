#include "oled.h"
#include "i2c.h"
#include "oledfont.h"
#include "delay.h"

// 显存：8 页 x 128 列，每字节对应纵向 8 个像素
static uint8_t OLED_GRAM[8][128];

// 写命令
static void OLED_WriteCmd(uint8_t cmd)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(OLED_ADDRESS);
    I2C_Soft_WaitAck();
    I2C_Soft_SendByte(0x00);   // 控制字节 0x00 = 命令
    I2C_Soft_WaitAck();
    I2C_Soft_SendByte(cmd);
    I2C_Soft_WaitAck();
    I2C_Soft_Stop();
}

// 写数据
static void OLED_WriteData(uint8_t data)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(OLED_ADDRESS);
    I2C_Soft_WaitAck();
    I2C_Soft_SendByte(0x40);   // 控制字节 0x40 = 数据
    I2C_Soft_WaitAck();
    I2C_Soft_SendByte(data);
    I2C_Soft_WaitAck();
    I2C_Soft_Stop();
}

// OLED 初始化（SSD1306，128x64）
void OLED_Init(void)
{
    uint8_t init_cmds[] = {
        0xAE,             // 关闭显示
        0xD5, 0x80,       // 设置显示时钟分频
        0xA8, 0x3F,       // 多路复用率 1/64
        0xD3, 0x00,       // 显示偏移 0
        0x40,             // 显示起始行 0
        0x8D, 0x14,       // 电荷泵开启 (0x14)
        0x20, 0x00,       // 水平寻址模式
        0xA1,             // 段重映射 (左右翻转)
        0xC8,             // 行扫描方向 (上下翻转)
        0xDA, 0x12,       // COM 引脚硬件配置
        0x81, 0xCF,       // 对比度设置
        0xD9, 0xF1,       // 预充电周期
        0xDB, 0x40,       // VCOMH 电平
        0xA4,             // 显示内容跟随 RAM
        0xA6,             // 正常显示 (非反色)
        0xAF              // 开启显示
    };
    uint8_t i;
    I2C_Soft_Init();
    for (i = 0; i < sizeof(init_cmds); i++)
        OLED_WriteCmd(init_cmds[i]);
    OLED_Clear();
}

// 清屏（同时刷新到屏幕）
void OLED_Clear(void)
{
    uint8_t i, j;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 128; j++)
            OLED_GRAM[i][j] = 0x00;
    OLED_Refresh();
}

// 把显存刷新到屏幕
void OLED_Refresh(void)
{
    uint8_t i, j;
    for (i = 0; i < 8; i++)
    {
        OLED_WriteCmd(0xB0 + i);   // 设置页地址
        OLED_WriteCmd(0x00);       // 列地址低 4 位
        OLED_WriteCmd(0x10);       // 列地址高 4 位
        for (j = 0; j < 128; j++)
            OLED_WriteData(OLED_GRAM[i][j]);
    }
}

// 画点：(x,y) 处亮/灭
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t on)
{
    uint8_t page, bit;
    if (x > 127 || y > 63) return;
    page = y / 8;
    bit  = y % 8;
    if (on)
        OLED_GRAM[page][x] |=  (1 << bit);
    else
        OLED_GRAM[page][x] &= ~(1 << bit);
}

// 显示一个 8x16 ASCII 字符
void OLED_ShowChar(uint8_t x, uint8_t y, char chr)
{
    uint8_t i, j, c = (uint8_t)chr - 32;
    if (c > 94) c = 0;            // 超出字体范围显示空格
    for (i = 0; i < 16; i++)      // 16 行
    {
        uint8_t line = OLED_F8x16[c][i];
        for (j = 0; j < 8; j++)   // 8 列
        {
            if (line & (0x80 >> j))
                OLED_DrawPoint(x + j, y + i, 1);
            else
                OLED_DrawPoint(x + j, y + i, 0);
        }
    }
}

// 显示字符串（自动按 8 像素宽度递增）
void OLED_ShowString(uint8_t x, uint8_t y, const char *str)
{
    uint8_t i = 0;
    while (str[i] != '\0')
    {
        OLED_ShowChar(x, y, str[i]);
        x += 8;
        if (x > 120) { x = 0; y += 16; }
        i++;
    }
}

// 显示数字（len 为显示位数，不足补 0）
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    uint8_t i;
    uint32_t div = 1;
    for (i = 1; i < len; i++) div *= 10;
    for (i = 0; i < len; i++)
    {
        uint8_t d = (num / div) % 10;
        OLED_ShowChar(x + i * 8, y, '0' + d);
        div /= 10;
    }
}
