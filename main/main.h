#ifndef __MAIN_H__
#define __MAIN_H__

#include "tdc.h"
#include <string.h >



#define SPI_SD                   SPI1
#define GPIO_CS                  GPIOA
#define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOA
#define GPIO_Pin_CS              GPIO_Pin_4
/*
#define DMA_Channel_SPI_SD_RX    DMA2_Channel1
#define DMA_Channel_SPI_SD_TX    DMA2_Channel2
#define DMA_FLAG_SPI_SD_TC_RX    DMA2_FLAG_TC1
#define DMA_FLAG_SPI_SD_TC_TX    DMA2_FLAG_TC1
*/
#define RCC_APB2Periph_GPIO_SPI_SD   RCC_APB2Periph_GPIOA
#define GPIO_SPI_SD              GPIOA
#define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_5
#define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_6
#define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_7

#define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB2PeriphClockCmd
#define RCC_APBPeriph_SPI_SD     RCC_APB2Periph_SPI1
#define SPI_BaudRatePrescaler_SPI_SD  SPI_BaudRatePrescaler_256

#define spi_cs_low() do { GPIO_CS->BRR = GPIO_Pin_CS;   for(ifor1 = 0; ifor1< 10000; ifor1 ++);} while (0)
#define spi_cs_high() do { GPIO_CS->BSRR = GPIO_Pin_CS; for(ifor1 = 0; ifor1< 10000; ifor1 ++); } while (0)


void print_measure_result(MEASURE_RESULT measure);

#endif
