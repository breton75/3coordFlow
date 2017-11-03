
#ifndef _EXAM_GENA_SPI_H_
#define _EXAM_GENA_SPI_H_

#define MMC_SUCCESS           0x00
#define MMC_BLOCK_SET_ERROR   0x01
#define MMC_RESPONSE_ERROR    0x02
#define MMC_DATA_TOKEN_ERROR  0x03
#define MMC_INIT_ERROR        0x04
#define MMC_CRC_ERROR         0x10
#define MMC_WRITE_ERROR       0x11
#define MMC_OTHER_ERROR       0x12
#define MMC_TIMEOUT_ERROR     0xFF





#include "main.h"
#include ".\..\FatFs\source\integer.h"


#define HARDWARE_SDIOSENSE_CD() 1  //((GPIOC->IDR & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */


char mmcInit(void);
uint8_t spi_read_g (void);
uint8_t spi_send_g (uint8_t data);
static char mmcGoIdle(void);
static void mmcSendDummyByte(void);
static void mmcSendCmdCRC7(uint_fast8_t cmd, uint_fast32_t data);
static unsigned char mmcGetResponseR1(void);
static unsigned char mmcGetResponseR7(unsigned long * rp);
static void mmcSendCmd(uint_fast8_t cmd, uint_fast32_t data);
static uint_fast8_t crc7b8(uint_fast8_t crc, uint_fast8_t v8);
static unsigned char mmcGetResponseR3(unsigned long * rp);
static  uint_fast8_t crc7b1(uint_fast8_t crc, uint_fast8_t v1);

void spi_complete( SPI_TypeDef * SPI);
void spi_progval8_p1(SPI_TypeDef * SPI, uint8_t cmd);
void spi_progval8_p2(SPI_TypeDef * SPI, uint32_t data);
uint8_t spi_read_byte(SPI_TypeDef * SPI, uint8_t cmd);


char mmc_detectcard(void);
char mmcWriteSectors(	const BYTE *buff,	DWORD sector,	UINT count);
unsigned char waitwhilebusy(void);
uint_fast8_t mmcGetXXResponse(uint_fast8_t resp);
	char mmcCheckBusy(void);
void spi_send_frame(SPI_TypeDef * SPI, const uint8_t * buffer, unsigned int size	)	;
char mmcReadSectors(BYTE *buff,	DWORD sector,	UINT count);
unsigned char mmcGetResponseR1b(void);
void spi_read_frame(SPI_TypeDef * SPI, uint8_t * buffer, unsigned int size);
#endif
	