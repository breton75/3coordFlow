#ifndef __INIT_TDC_SPI__
#define __INIT_TDC_SPI__

#include "../CMSIS/stm32f10x.h"
#include "../STM32F10x_StdPeriph_Driver/inc/stm32f10x_gpio.h"
#include "../STM32F10x_StdPeriph_Driver/inc/stm32f10x_rcc.h"
#include "../STM32F10x_StdPeriph_Driver/inc/stm32f10x_spi.h"

#include <string.h>
#include <stdint.h>

#include "tdc1000_configuration.h"
#include "tdc7200_configuration.h"
#include "stm32_pins.h"

#define TDC_NONE  0

#define TDC7200_1 1
#define TDC7200_2 2
#define TDC7200_3 3

#define TDC1000_1 4
#define TDC1000_2 5
#define TDC1000_3 6

#define FREQ 8000000.0


typedef struct {
  uint8_t chip;
  uint8_t err;
  uint8_t tdc1000err;
  uint8_t tdc7200err;
  float tof1;
  float tof2;
} MEASURE_RESULT;

typedef struct {
  uint8_t tdc7200;
  uint8_t tdc1000;
  uint16_t int_pin;
} MEASURE_SET;

void tdc_init(void);

void write_chip(uint8_t chip, uint16_t addr, uint8_t data);
void write_spi(uint16_t addr, uint8_t data);

uint16_t read_chip(uint8_t chip, uint16_t addr);
uint16_t read_spi(uint16_t addr);

void chip_select(uint8_t chip);
MEASURE_RESULT measure(MEASURE_SET mset);
MEASURE_RESULT get_tof(MEASURE_SET mset);

void configure7200(uint8_t tdc7200chip);
void configure1000(uint8_t tdc1000chip);


//char TDC_write_data(char ch,unsigned char addr,char data_for_send);
//char TDC_read_data(char ch,unsigned char addr);

//int TDC_read24_data(char ch,unsigned char addr);

//double TDC_Calibrate(double time_delay, float first_max_delay, int max_steps,double dis1, double dis2, double dis3);

//double TDC_wait_for_stabilization(char ch);


#endif
