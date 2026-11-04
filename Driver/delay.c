#include "delay.h"

// 1us 对应的 SysTick 节拍数（在 Delay_Init 中根据主频计算）
static uint32_t fac_us = 0;

// 基于 SysTick 的延时初始化（使用 HCLK/8 作为时钟源）
void Delay_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 外部时钟 = HCLK/8
    fac_us = SystemCoreClock / 8000000;                   // 72MHz -> 9，8MHz -> 1
}

// 微秒级延时（nus 上限约 2^24/fac_us，72MHz 下约 1.86 秒）
void Delay_us(uint32_t nus)
{
    uint32_t temp;
    SysTick->LOAD = (uint32_t)nus * fac_us;  // 装载计数初值
    SysTick->VAL  = 0x00;                    // 清空当前值
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 启动计数
    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // 等待计数到 0
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       // 关闭计数器
    SysTick->VAL  = 0x00;                            // 清空当前值
}

void Delay_ms(uint32_t nms)
{
    while (nms--) Delay_us(1000);
}

void Delay_s(uint32_t ns)
{
    while (ns--) Delay_ms(1000);
}
