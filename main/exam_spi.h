#ifndef _EXAM_SPI_H_
#define _EXAM_SPI_H_


#include "main.h"
uint8_t SD_sendCommand(uint8_t cmd, uint32_t arg);
uint8_t SD_init(void);
uint8_t SD_ReadSector(uint32_t BlockNumb,uint8_t *buff);
uint8_t spi_read (void);
uint8_t spi_send (uint8_t data);

#endif
