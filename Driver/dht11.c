#include "dht11.h"
#include "delay.h"

// DHT11 数据引脚：PA0
#define DHT11_PORT  GPIOA
#define DHT11_PIN   GPIO_Pin_0

// 配置引脚为推挽输出
static void DHT11_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

// 配置引脚为上拉输入
static void DHT11_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

// 读取 1 位：高电平持续 ~70us 为 1，~28us 为 0
static uint8_t DHT11_ReadBit(void)
{
    uint8_t retry = 0;
    // 等待 50us 低电平结束
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 1 && retry < 100)
    { retry++; Delay_us(1); }
    retry = 0;
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 0 && retry < 100)
    { retry++; Delay_us(1); }
    // 高电平开始后再延时 40us 采样电平
    Delay_us(40);
    if (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 1)
        return 1;
    else
        return 0;
}

// 读取 1 字节（高位在前）
static uint8_t DHT11_ReadByte(void)
{
    uint8_t i, dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_ReadBit();
    }
    return dat;
}

// 初始化 DHT11（配置引脚并释放总线）
uint8_t DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);  // 释放总线（高电平）
    return 0;
}

// 读取温湿度：成功返回 0，temp/humi 为整数值（摄氏度 / 百分比）
// 失败（无应答或校验错）返回 1
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
{
    uint8_t retry = 0, i;
    uint8_t buf[5];

    // 主机拉低总线至少 18ms 触发采集
    DHT11_OUT();
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
    Delay_ms(20);
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    Delay_us(30);
    DHT11_IN();

    // 等待 DHT11 拉低 80us（应答信号开始）
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 1 && retry < 100)
    { retry++; Delay_us(1); }
    if (retry >= 100) return 1;   // 无应答
    retry = 0;
    // 等待 80us 低电平结束
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 0 && retry < 100)
    { retry++; Delay_us(1); }
    retry = 0;
    // 等待 80us 高电平结束
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 1 && retry < 100)
    { retry++; Delay_us(1); }

    // 读取 40 位：湿度整数、湿度小数、温度整数、温度小数、校验和
    for (i = 0; i < 5; i++)
        buf[i] = DHT11_ReadByte();

    // 释放总线
    DHT11_OUT();
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);

    // 校验：前四字节之和 = 校验和
    if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
    {
        if (humi) *humi = buf[0];
        if (temp) *temp = buf[2];
        return 0;
    }
    return 1;
}
