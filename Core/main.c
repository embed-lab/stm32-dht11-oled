#include "stm32f10x.h"
#include "delay.h"
#include "i2c.h"
#include "oled.h"
#include "dht11.h"

int main(void)
{
    uint8_t temp = 0, humi = 0;

    Delay_Init();     // 延迟初始化
    OLED_Init();      // OLED 初始化
    DHT11_Init();     // DHT11 引脚初始化

    // 静态界面
    OLED_ShowString(0, 0, "DHT11+OLED DEMO");
    OLED_ShowString(0, 2, "Temp:");
    OLED_ShowString(72, 2, "C");
    OLED_ShowString(0, 4, "Humi:");
    OLED_ShowString(72, 4, "%");
    OLED_Refresh();

    while (1)
    {
        // 周期性读取温湿度（整数值）
        if (DHT11_Read_Data(&temp, &humi) == 0)
        {
            OLED_ShowNum(48, 2, temp, 2);
            OLED_ShowNum(48, 4, humi, 2);
            OLED_Refresh();
        }
        Delay_s(1);   // 1 秒刷新一次
    }
}

// 供标准库 system_stm32f10x.c 使用的空 SysTick _handler 占位
// （本项目未使用 SysTick 中断，仅作保留）
void SysTick_Handler(void)
{
}
