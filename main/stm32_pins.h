#ifndef STM32_PINS_H
#define STM32_PINS_H


#include "../STM32f107_Firmware/CMSIS/in_coc/stm32f10x.h"
#include "../STM32F10x_StdPeriph_Driver/inc/stm32f10x_gpio.h"
#include "../STM32F10x_StdPeriph_Driver/inc/stm32f10x_rcc.h"
#include "../STM32F10x_StdPeriph_Driver/inc/stm32f10x_spi.h"

/*! ------- STM32 & SPI ---------!*/
#define STM_TDC_INT_3_Pin       GPIO_Pin_5
#define STM_TDC_INT_2_Pin       GPIO_Pin_4
#define STM_TDC_INT_1_Pin       GPIO_Pin_3

#define SPI_CLK_TDC_Pin         GPIO_Pin_13
#define SPI_MISO_TDC_Pin        GPIO_Pin_14
#define SPI_MOSI_TDC_Pin        GPIO_Pin_15

#define STM32_TRIGGER_1_Pin     GPIO_Pin_8
#define STM32_TRIGGER_2_Pin     GPIO_Pin_10
#define STM32_TRIGGER_3_Pin     GPIO_Pin_11

#define TDC_WRITE 0x4000
#define TDC_READ	0
#define TDC_ADDR(addr)  (addr << 8)


/*! -------- TDC7200 ------- !*/
#define STM_TDC7200_EN1_Pin     GPIO_Pin_0
#define STM_TDC7200_EN2_Pin     GPIO_Pin_1
#define STM_TDC7200_EN3_Pin     GPIO_Pin_2

#define TDC7200_SPI_CSB_1_Pin   GPIO_Pin_5
#define TDC7200_SPI_CSB_2_Pin   GPIO_Pin_7
#define TDC7200_SPI_CSB_3_Pin   GPIO_Pin_9

/// ошибка здесь или на схеме????
//#define TDC7200_INTB_GROUP 			GPIOE
//#define TDC7200_INTB_1 GPIO_Pin_2 
//#define TDC7200_INTB_2 GPIO_Pin_3
//#define TDC7200_INTB_3 GPIO_Pin_4

/*! -------- TDC1000 ------- !*/
#define TDC1000_SPI_CSB_1_Pin   GPIO_Pin_4
#define TDC1000_SPI_CSB_2_Pin   GPIO_Pin_6
#define TDC1000_SPI_CSB_3_Pin   GPIO_Pin_8

#define TDC1000_CHSEL_Pin       GPIO_Pin_2
#define TDC1000_RESET_Pin       GPIO_Pin_1
#define TDC1000_ENABLE_Pin      GPIO_Pin_0

#define TDC1000_ERRB_1_Pin      GPIO_Pin_10
#define TDC1000_ERRB_2_Pin      GPIO_Pin_9
#define TDC1000_ERRB_3_Pin      GPIO_Pin_8

#define TDC1000_STM_START_1_Pin GPIO_Pin_15
#define TDC1000_STM_STOP_1_Pin	GPIO_Pin_14
#define TDC1000_STM_START_2_Pin GPIO_Pin_13
#define TDC1000_STM_STOP_2_Pin	GPIO_Pin_12
#define TDC1000_STM_START_3_Pin GPIO_Pin_11
#define TDC1000_STM_STOP_3_Pin	GPIO_Pin_10


#endif // STM32_PINS_H
