#ifndef __INIT_TDC_SPI__
#define __INIT_TDC_SPI__

#include "TDC1000_registers.h"
#include "TDC7200_registers.h"
#include "stm32_pins.h"

#define TDC7200_1 0
#define TDC1000_1 1
#define TDC7200_2 2
#define TDC1000_2 3
#define TDC7200_3 4
#define TDC1000_3 5

#define FREQ 8000000.0

typedef struct {
  uint8_t err;
  uint8_t tdc1000err;
  uint8_t tdc7200err;
  float tof1;
  float tof2;
  float sound_speed;  
} TOF;

void TDC_SPI_Init(void);



void write_spi(int chip, uint16_t addr, uint8_t data);
uint16_t read_spi(int chip, uint16_t addr);
void chip_select(int chip = -1);
TOF get_tof(uint8_t tdc7200chip);





char TDC_write_data(char ch,unsigned char addr,char data_for_send);
char TDC_read_data(char ch,unsigned char addr);

int TDC_read24_data(char ch,unsigned char addr);

double TDC_Calibrate(double time_delay, float first_max_delay, int max_steps,double dis1, double dis2, double dis3);

double TDC_wait_for_stabilization(char ch);


#endif
