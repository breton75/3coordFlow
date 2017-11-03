

#ifndef __INIT_PERIPHERY__
#define __INIT_PERIPHERY__

#include "main.h"



void Init_USART1(uint32_t BaudRate);
void Init_RCC(void);
void Init_gpio(uint16_t);
void Init_SPI_SD(void);
void Init_I2C(void);
void Init_SPI_TDC(void);
void Init_USART2_NAV  (uint32_t BaudRate);



#endif
