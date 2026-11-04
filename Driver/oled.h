#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

// OLED I2C 器件地址（0x3C 左移 1 位）。若屏幕不亮，可尝试 0x7A
#define OLED_ADDRESS  0x78

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Refresh(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t on);
void OLED_ShowChar(uint8_t x, uint8_t y, char chr);
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

#endif /* __OLED_H */
