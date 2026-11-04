#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

// OLED 软件 I2C 引脚定义：PB6 = SCL，PB7 = SDA
#define I2C_SOFT_PORT  GPIOB
#define I2C_SOFT_SCL   GPIO_Pin_6
#define I2C_SOFT_SDA   GPIO_Pin_7

void I2C_Soft_Init(void);
void I2C_Soft_Start(void);
void I2C_Soft_Stop(void);
void I2C_Soft_SendByte(uint8_t byte);
uint8_t I2C_Soft_ReadByte(uint8_t ack);
uint8_t I2C_Soft_WaitAck(void);
void I2C_Soft_SendAck(void);
void I2C_Soft_SendNotAck(void);

#endif /* __I2C_H */
