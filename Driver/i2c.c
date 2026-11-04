#include "i2c.h"
#include "delay.h"

#define SCL_HIGH()  GPIO_SetBits(I2C_SOFT_PORT, I2C_SOFT_SCL)
#define SCL_LOW()   GPIO_ResetBits(I2C_SOFT_PORT, I2C_SOFT_SCL)
#define SDA_HIGH()  GPIO_SetBits(I2C_SOFT_PORT, I2C_SOFT_SDA)
#define SDA_LOW()   GPIO_ResetBits(I2C_SOFT_PORT, I2C_SOFT_SDA)
#define SDA_READ()  GPIO_ReadInputDataBit(I2C_SOFT_PORT, I2C_SOFT_SDA)

// SDA 切换为输入（用于读应答 / 读数据）
static void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = I2C_SOFT_SDA;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;     // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_SOFT_PORT, &GPIO_InitStructure);
}

// SDA 切换为开漏输出（用于发数据 / 起始停止）
static void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = I2C_SOFT_SDA;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;  // 开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_SOFT_PORT, &GPIO_InitStructure);
}

// 初始化软件 I2C 引脚（SCL/SDA 均开漏上拉）
void I2C_Soft_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = I2C_SOFT_SCL | I2C_SOFT_SDA;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_SOFT_PORT, &GPIO_InitStructure);

    SCL_HIGH();
    SDA_HIGH();
}

// 发送起始信号：SCL 高时 SDA 由高变低
void I2C_Soft_Start(void)
{
    SDA_OUT();
    SDA_HIGH();
    SCL_HIGH();
    Delay_us(4);
    SDA_LOW();
    Delay_us(4);
    SCL_LOW();   // 钳住总线，准备发送数据
}

// 发送停止信号：SCL 高时 SDA 由低变高
void I2C_Soft_Stop(void)
{
    SDA_OUT();
    SCL_LOW();
    SDA_LOW();
    Delay_us(4);
    SCL_HIGH();
    Delay_us(4);
    SDA_HIGH();
    Delay_us(4);
}

// 等待从机应答，返回 0=有应答，1=无应答
uint8_t I2C_Soft_WaitAck(void)
{
    uint8_t ack = 0;
    SDA_IN();
    SCL_HIGH();
    Delay_us(2);
    if (SDA_READ()) ack = 1;   // 高电平表示无应答
    SCL_LOW();
    Delay_us(2);
    SDA_OUT();
    return ack;
}

// 主机发送应答
void I2C_Soft_SendAck(void)
{
    SDA_OUT();
    SCL_LOW();
    SDA_LOW();
    Delay_us(2);
    SCL_HIGH();
    Delay_us(2);
    SCL_LOW();
}

// 主机发送非应答
void I2C_Soft_SendNotAck(void)
{
    SDA_OUT();
    SCL_LOW();
    SDA_HIGH();
    Delay_us(2);
    SCL_HIGH();
    Delay_us(2);
    SCL_LOW();
}

// 发送一个字节（高位在前）
void I2C_Soft_SendByte(uint8_t byte)
{
    uint8_t i;
    SDA_OUT();
    SCL_LOW();
    for (i = 0; i < 8; i++)
    {
        if (byte & 0x80) SDA_HIGH();
        else SDA_LOW();
        Delay_us(2);
        SCL_HIGH();
        Delay_us(2);
        SCL_LOW();
        byte <<= 1;
    }
}

// 读取一个字节，ack=1 时回应答，ack=0 时回非应答
uint8_t I2C_Soft_ReadByte(uint8_t ack)
{
    uint8_t i, recv = 0;
    SDA_IN();
    for (i = 0; i < 8; i++)
    {
        recv <<= 1;
        SCL_HIGH();
        Delay_us(2);
        if (SDA_READ()) recv |= 0x01;
        SCL_LOW();
        Delay_us(2);
    }
    if (ack) I2C_Soft_SendAck();
    else     I2C_Soft_SendNotAck();
    return recv;
}
