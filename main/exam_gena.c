
#include "exam_gena.h"
#define SDCARD_CS_LOW()        spi_cs_low()  //GPIOA->BSRR = GPIO_BSRR_BR4;         
#define SDCARD_CS_HIGH()    	 spi_cs_high()  //GPIOA->BSRR = GPIO_BSRR_BS4;

// Tokens (necessary  because at NPO/IDLE (and CS active) only 0xff is on the data/command line)
#define MMC_START_DATA_BLOCK_TOKEN          0xfe   // Data token start byte, Start Single Block Read
#define MMC_START_DATA_MULTIPLE_BLOCK_READ  0xfe   // Data token start byte, Start Multiple Block Read
#define MMC_START_DATA_BLOCK_WRITE          0xfe   // Data token start byte, Start Single Block Write
#define MMC_START_DATA_MULTIPLE_BLOCK_WRITE 0xfc   // Data token start byte, Start Multiple Block Write
#define MMC_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xfd   // Data toke stop byte, Stop Multiple Block Write

#define MMC_R1_RESPONSE       0x00

// commands: first bit 0 (start bit), second 1 (transmission bit); CMD-number + Offsett 0x40
#define MMC_GO_IDLE_STATE          0x40     //CMD0
#define MMC_SEND_OP_COND           0x41     //CMD1
#define MMC_SEND_IF_COND           0x48     //CMD8 - added by mgs
#define MMC_READ_CSD               0x49     //CMD9
#define MMC_SEND_CID               0x4a     //CMD10
#define MMC_STOP_TRANSMISSION      0x4c     //CMD12
#define MMC_SEND_STATUS            0x4d     //CMD13
#define MMC_SET_BLOCKLEN           0x50     //CMD16 Set block length for next read/write
#define MMC_READ_SINGLE_BLOCK      0x51     //CMD17 Read block from memory
#define MMC_READ_MULTIPLE_BLOCK    0x52     //CMD18
#define MMC_CMD_WRITEBLOCK         0x54     //CMD20 Write block to memory
#define MMC_WRITE_BLOCK            0x58     //CMD24
#define MMC_WRITE_MULTIPLE_BLOCK   0x59     //CMD25
#define MMC_WRITE_CSD              0x5b     //CMD27 PROGRAM_CSD
#define MMC_SET_WRITE_PROT         0x5c     //CMD28
#define MMC_CLR_WRITE_PROT         0x5d     //CMD29
#define MMC_SEND_WRITE_PROT        0x5e     //CMD30
#define MMC_TAG_SECTOR_START       0x60     //CMD32
#define MMC_TAG_SECTOR_END         0x61     //CMD33
#define MMC_UNTAG_SECTOR           0x62     //CMD34
#define MMC_TAG_EREASE_GROUP_START 0x63     //CMD35
#define MMC_TAG_EREASE_GROUP_END   0x64     //CMD36
#define MMC_UNTAG_EREASE_GROUP     0x65     //CMD37
#define MMC_EREASE                 0x66     //CMD38
#define MMC_READ_OCR               0x67     //CMD39
#define MMC_CRC_ON_OFF             0x68     //CMD40


// error/success codes
#define MMC_SUCCESS           0x00
#define MMC_BLOCK_SET_ERROR   0x01
#define MMC_RESPONSE_ERROR    0x02
#define MMC_DATA_TOKEN_ERROR  0x03
#define MMC_INIT_ERROR        0x04
#define MMC_CRC_ERROR         0x10
#define MMC_WRITE_ERROR       0x11
#define MMC_OTHER_ERROR       0x12
#define MMC_TIMEOUT_ERROR     0xFF

#define MMC_ACMD23	(0x40 + 23)	// ACMD23
#define MMC_ACMD41	(0x40 + 41)	// ACMD41

#define MMC_APP_CMD	(0x40 + 55)		// prefix command - application command - next ACMDxx
#define MMC_GEN_CMD	(0x40 + 56)		// Used either to transfer a Data Block to the card or to get a Data Block from the card for general purpose/applicat

#define MMC_SECTORSIZE 512


#define WITHSPISW 	1

#define targetsdcard SPI3


static uint_fast8_t mmcCardVersion2;
// Для НЕ SDHC (адресация идёт в байтах)
// Для SDHC (адресация идёт в 512-ти байтовых блоках)
static uint_fast32_t mmcAddressMultiplier;

char mmc_detectcard(void)
{
		int i, ifor1;
//	sd_power_cycle();

	char ec;
	SDCARD_CS_LOW();
	ec = mmcInit();
	SDCARD_CS_HIGH();


	//if (ec != MMC_SUCCESS)
		return ec;

//	ec = mmcSetBlockLength(MMC_SECTORSIZE);
	//return ec;
}

char mmcInit(void)
{
	char ec;
	int i, ifor1;

	

	SDCARD_CS_HIGH();
//	sdcard_spi_setfreq(400000);
	SDCARD_CS_LOW();

	for(i = 0; i <= 9; i ++)
		spi_read_g();

	ec = mmcGoIdle();	// Вызывается, подразумевая что CS установлен

	if (ec == 0)
	{
		if (mmcCardVersion2 != 0)
		{
			
			SDCARD_CS_HIGH();
			//sdcard_spi_setfreq(24000000);
			SDCARD_CS_LOW();
		}
		else
		{
			
			SDCARD_CS_HIGH();
	//		sdcard_spi_setfreq(12000000);
			SDCARD_CS_LOW();
		}
	}
	else
	{
			
	}
	
	return ec;
}


uint8_t spi_send_g (uint8_t data)
{ 
  while (!(SPI_SD->SR & SPI_SR_TXE));      //убедиться, что предыдущая передача завершена
  SPI_SD->DR = data;                       //загружаем данные для передачи
  while (!(SPI_SD->SR & SPI_SR_RXNE));     //ждем окончания обмена
  return (SPI_SD->DR);		         //читаем принятые данные
}

//*********************************************************************************************
//function  прием байт по SPI1                                                               //
//argument  none                                                                             //
//return    принятый байт                                                                    //
//*********************************************************************************************
uint8_t spi_read_g (void)
{ 
  return spi_send_g(0xff);		  //читаем принятые данные
}


// set MMC in Idle mode
// Вызывается, подразумевая что CS установлен
static char mmcGoIdle(void)
{
	unsigned char response;
	unsigned long cmd8answer = 0;
	int ifor1;
	SDCARD_CS_HIGH();
	/////spi_read_byte(targetsdcard, 0xff);
	SDCARD_CS_LOW();
	mmcSendDummyByte();
	//Send Command 0 to put MMC in SPI mode
	mmcSendCmdCRC7(MMC_GO_IDLE_STATE, 0);	// CMD0 - Обязательно с правильным CRC
	//Now wait for READY RESPONSE
	if ((response = mmcGetResponseR1()) != 0x01)
	{
		return MMC_INIT_ERROR;
	}

	mmcSendDummyByte();
	mmcSendCmdCRC7(MMC_SEND_IF_COND, 0x000001aa);	// CMD8 - Обязательно с правильным CRC. 3.3 Volt VCC
	response = mmcGetResponseR7(& cmd8answer);
	if ((response & 0x04) != 0)	// illegal command or pattern not match
	{
		unsigned long cmd58answer = 0;
		mmcAddressMultiplier = MMC_SECTORSIZE;
		mmcCardVersion2 = 0;

		// if no responce
		// Ver2.00 or later SD Memory Card(voltage mismatch)
		// or Ver1.X SD Memory Card
		// or not SD Memory Card
		
		// check voltage range here
		mmcSendDummyByte();
		mmcSendCmd(0x40 + 58, 0);	// CMD58
		response = mmcGetResponseR3(& cmd58answer);
		
		if (response == 0x00 || response == 0x01)
		{
			if ((cmd58answer & (1UL << 21)) || (cmd58answer & (1UL << 20)))
			{
	;//			debug_printf_P(PSTR("3.3 volt VCC suitable.\n"));
			}
			else
			{
				;;//debug_printf_P(PSTR("3.3 volt VCC NOT suitable.\n"));
				return MMC_INIT_ERROR;
			}
		}

		for (;;)
		{
			mmcSendDummyByte();
			mmcSendCmd(MMC_APP_CMD, 0);	// APP_CMD
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
//			debug_printf_P(PSTR("mmcGoIdle: 1: APP_CMD responce R1 = %02x\n"), response);
			if (response != 0x01 && response != 0x00)
				continue;

			mmcSendDummyByte();
			mmcSendCmd(MMC_ACMD41, 0);	// ACMD41(0)
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
//			debug_printf_P(PSTR("ACMD41 responce R1 = %02x\n"), response);
			if (response == 0x00)
				break;
		}
		//debug_printf_P(PSTR("ACMD41 has responce %02x\n"), response); 

		//return MMC_INIT_ERROR;
	}
	else if ((cmd8answer & 0xff) != 0xaa)
	{
		
		return MMC_INIT_ERROR;
	}
	else
	{
		unsigned long cmd58answer = 0;
		mmcCardVersion2 = 1;

		// Ver2.00 or later SD Memory Card
	//	debug_printf_P(PSTR("Ver2.00 or later SD Memory Card\n")); 	

		// check voltage range here
		mmcSendDummyByte();
		mmcSendCmd(0x40 + 58, 0);	// CMD58
		response = mmcGetResponseR3(& cmd58answer);
//		debug_printf_P("2: CMD58 has responce %02x, value = %08lx\n", response, cmd58answer); 
		if (response == 0x00 || response == 0x01)
		{
			if ((cmd58answer & (1UL << 21)) || (cmd58answer & (1UL << 20)))
			{
;//				debug_printf_P(PSTR("3.3 volt VCC suitable.\n"));
			}
			else
			{
//				debug_printf_P(PSTR("3.3 volt VCC NOT suitable.\n"));
				return MMC_INIT_ERROR;
			}
		}

		mmcSendDummyByte();
		mmcSendCmd(0x40 + 59, 1);	// shitch CRC check on - required before ACMD41
		response = mmcGetResponseR1();
		//debug_printf_P(PSTR("CMD59 has responce %02x\n"), response); 

		for (;;)
		{
			mmcSendDummyByte();
			mmcSendCmdCRC7(MMC_APP_CMD, 0);	// APP_CMD
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			//debug_printf_P(PSTR("mmcGoIdle: 2: APP_CMD responce R1 = %02x\n"), response);
			if (response != 0x01 && response != 0x00)
				continue;

			mmcSendDummyByte();
			mmcSendCmdCRC7(MMC_ACMD41, 0x40000000);	// HCS (High Capacity Support)
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			//debug_printf_P(PSTR("ACMD41 responce R1 = %02x\n"), response);
			if (response == 0x00)
				break;
		}

		mmcSendDummyByte();
		mmcSendCmdCRC7(0x40 + 59, 0);	// shitch CRC check off
		response = mmcGetResponseR1();
		//debug_printf_P("CMD59 has responce %02x\n", response); 

		// Check CCS bit (capacity) bit
		mmcSendDummyByte();
		mmcSendCmd(0x40 + 58, 0);	// CMD58
		response = mmcGetResponseR3(& cmd58answer);
//		debug_printf_P(PSTR("3: CMD58 has responce %02x, value = %08lx\n"), response, cmd58answer); 
		if (response == 0x00 || response == 0x01)
		{
			if ((cmd58answer & 0x40000000) != 0)	//CCS (Card Capacity Status)
			{
//				debug_printf_P(PSTR("SDHC or SDXC (High Capacity)\n"));
				mmcAddressMultiplier = 1;	// Для SDHC (адресация идёт в 512-ти байтовых блоках)
			}
			else
			{
//				debug_printf_P(PSTR("SDSD - up to 2GB\n"));
				mmcAddressMultiplier = MMC_SECTORSIZE;	// Для обычных SD карт
			}
		}
	}

	//debug_printf_P("mmcGoIdle() done.\n");

	spi_read_byte(targetsdcard, 0xff);
	return (MMC_SUCCESS);
}

static void mmcSendDummyByte(void)
{
	spi_progval8_p1(targetsdcard, 0xff);	// dummy byte
	spi_complete(targetsdcard);
}

static void mmcSendCmdCRC7(uint_fast8_t cmd, uint_fast32_t data)
{
	uint_fast8_t crc;

	crc = crc7b8(0x00, cmd);		
	crc = crc7b8(crc, data >> 24);	
	crc = crc7b8(crc, data >> 16);	
	crc = crc7b8(crc, data >> 8);	
	crc = crc7b8(crc, data >> 0);	

	spi_progval8_p1(targetsdcard, cmd);	// command
	spi_progval8_p2(targetsdcard, data >> 24);
	spi_progval8_p2(targetsdcard, data >> 16);
	spi_progval8_p2(targetsdcard, data >> 8);
	spi_progval8_p2(targetsdcard, data >> 0);
	spi_progval8_p2(targetsdcard, crc | 0x01);		// CRC and end transmit bit
	spi_complete(targetsdcard);
}

#define RESPLIMIT 64000U
#define RESPLIMITREAD 640000L

static unsigned char mmcGetResponseR1(void)
{
	unsigned long i;

	unsigned char response;

	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		//if ((response & 0x80) == 0)
		if ((response & 0x80) == 0)
			break;
	}
	return response;
}

// mmc Get Responce (five bytes)
static unsigned char mmcGetResponseR7(unsigned long * rp)
{
	//Response comes 1-8bytes after command
	//the first bit will be a 0
	//followed by an error code
	//data will be 0xff until response
	unsigned long i;

	unsigned char response;

	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		if ((response & 0x80) != 0)
			continue;
		{
			//R1print("R7", response);
			unsigned long r = 0;
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			spi_read_byte(targetsdcard, 0xff);	// crc and end bit
			* rp = r;
			break;
		}

	}
	return response;
}
// send command to MMC
static void mmcSendCmd(uint_fast8_t cmd, uint_fast32_t data)
{
	spi_progval8_p1(targetsdcard, cmd);	// command
	spi_progval8_p2(targetsdcard, data >> 24);
	spi_progval8_p2(targetsdcard, data >> 16);
	spi_progval8_p2(targetsdcard, data >> 8);
	spi_progval8_p2(targetsdcard, data >> 0);
	spi_progval8_p2(targetsdcard, 0xff);	// CRC
	spi_complete(targetsdcard);
}
/* вспомогательная подпрограмма расчёта CRC для одного байта */
static uint_fast8_t crc7b8(uint_fast8_t crc, uint_fast8_t v8)
{
	crc = crc7b1(crc, v8 & 0x80);
	crc = crc7b1(crc, v8 & 0x40);
	crc = crc7b1(crc, v8 & 0x20);
	crc = crc7b1(crc, v8 & 0x10);
	crc = crc7b1(crc, v8 & 0x08);
	crc = crc7b1(crc, v8 & 0x04);
	crc = crc7b1(crc, v8 & 0x02);
	crc = crc7b1(crc, v8 & 0x01);

	return crc;
}

static unsigned char mmcGetResponseR3(unsigned long * rp)
{
	//Response comes 1-8bytes after command
	//the first bit will be a 0
	//followed by an error code
	//data will be 0xff until response
	unsigned long i;

	unsigned char response;

	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		if ((response & 0x80) != 0)
			continue;
		{
			//R1print("R3", response);
			unsigned long r = 0;
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);

			* rp = r;
			break;
		}
	}
	return response;
}
/*
void 
prog_spi_send_frame(
	spitarget_t target,
	const uint8_t * buff, 
	unsigned int size
	)
{
	spi_progval8_p1(target, * buff);
	while (-- size)
		spi_progval8_p2(target, * ++ buff);
	spi_complete(target);
}
*/

/* вспомогательная подпрограмма расчёта CRC для одного бита.
   CRC хранится в старших 7 битах.
*/

static  uint_fast8_t crc7b1(uint_fast8_t crc, uint_fast8_t v1)
{
	if ((v1 != 0) != ((crc & 0x80) != 0))
	{
		crc <<= 1;
		crc ^= (0x09 << 1);
	}
	else
	{
		crc <<= 1;
	}

	return crc;
}

	
void spi_complete( SPI_TypeDef * SPI)
{
	uint32_t t;
		while ((SPI->SR & SPI_SR_RXNE) == 0)			;
	t = SPI->DR & 0xFF;	/* clear SPI_SR_RXNE in status register */
	while ((SPI->SR & SPI_SR_BSY) != 0)	
		;
	//return t;
}

void spi_progval8_p1(SPI_TypeDef * SPI, uint8_t cmd)
{
	SPI->DR = cmd;
}

void spi_progval8_p2(SPI_TypeDef * SPI, uint32_t data)
{
	uint32_t t;
	while ((SPI->SR & SPI_SR_RXNE) == 0);	
	t = SPI->DR;
	SPI->DR = data & SPI_DR_DR;
}

uint8_t spi_read_byte(SPI_TypeDef * SPI, uint8_t cmd)
{	
	uint8_t t = 0;
	//spi_progval8_p1(SPI, 0xff);	/* передать символ */
	spi_progval8_p1(SPI, cmd);
	while ((SPI->SR & SPI_SR_RXNE) == 0)			;
	t = SPI->DR & 0xFF;	/* clear SPI_SR_RXNE in status register */
	while ((SPI->SR & SPI_SR_BSY) != 0)	;
	return t;	
}
//-------------------------------------------------------------------------------------------------------
void spi_send_frame(SPI_TypeDef * SPI,	/* addressing to chip */
	const uint8_t * buffer, 
	unsigned int size
	)	
{		
		spi_progval8_p1(targetsdcard, * buffer);	// command
		while(--size 	> 0)
		{
			spi_progval8_p2(targetsdcard, * (++buffer));
		}
		spi_complete(targetsdcard);		
}
void spi_read_frame(SPI_TypeDef * SPI,	/* addressing to chip */
	uint8_t * buffer, 
	unsigned int size	)	
{		
		while(size-- 	> 0)
		{
			* (buffer++) = spi_read_byte(SPI, 0xff);
		}		
}
	
char mmcWriteSectors(
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
	)
{
	uint32_t ifor1;
	char rvalue = MMC_RESPONSE_ERROR;         // MMC_SUCCESS;
	//  char c = 0x00;

	if (count == 1)
	{
		// signle block write
		// Set the block length to read
		//if (mmcSetBlockLength (count) == MMC_SUCCESS)   // block length could be set
		// CS = LOW (on)
		SDCARD_CS_LOW();
		// send write command
		mmcSendDummyByte();
		if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
		{
			SDCARD_CS_HIGH();
			return MMC_RESPONSE_ERROR;
		}

		mmcSendDummyByte();
		mmcSendCmd(MMC_WRITE_BLOCK, sector * mmcAddressMultiplier);

		// check if the MMC acknowledged the write block command
		// it will do this by sending an affirmative response
		// in the R1 format (0x00 is no errors)
		if (mmcGetXXResponse(MMC_R1_RESPONSE) == MMC_R1_RESPONSE)
		{
			spi_progval8_p1(targetsdcard, 0xff);
			// send the data token to signify the start of the data
			spi_progval8_p2(targetsdcard, MMC_START_DATA_BLOCK_WRITE);
			spi_complete(targetsdcard);

			// clock the actual data transfer and transmitt the bytes
			spi_send_frame(targetsdcard, buff, MMC_SECTORSIZE);

			// put CRC bytes (not really needed by us, but required by MMC)
			spi_progval8_p1(targetsdcard, 0xff);
			spi_progval8_p2(targetsdcard, 0xff);
			spi_complete(targetsdcard);
			// read the data response xxx0<status>1 : status 010: Data accected, status 101: Data
			//   rejected due to a crc error, status 110: Data rejected due to a Write error.
			mmcCheckBusy();
			rvalue = MMC_SUCCESS;
		}
		else
		{
			// the MMC never acknowledge the write command
			//debug_printf_P("MMC_BLOCK_SET_ERROR\n");
			rvalue = MMC_BLOCK_SET_ERROR;   // 2
		}
		// give the MMC the required clocks to finish up what ever it needs to do
		//  for (i = 0; i < 9; ++i)
		//    spi_progval8(targetsdcard, 0xff);

		SDCARD_CS_HIGH();
		// Send 8 Clock pulses of delay.
		////////spi_read_byte(targetsdcard, 0xff);
		return rvalue;
	}
	else
	{
		// write multiblock
		//debug_printf_P(PSTR("write multiblock, count=%d\n"), count);
		// Set the block length to read
		//if (mmcSetBlockLength (count) == MMC_SUCCESS)   // block length could be set
		// CS = LOW (on)
		SDCARD_CS_LOW();
		mmcSendDummyByte();
		if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
		{
			SDCARD_CS_HIGH();
			return MMC_RESPONSE_ERROR;
		}

		{
			unsigned char response;
			mmcSendDummyByte();
			mmcSendCmd(MMC_APP_CMD, 0);	// APP_CMD
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			if (/*response != 0x01 && */response != 0x00)
			{
			//	debug_printf_P(PSTR("mmcWriteSectors: 1: APP_CMD responce R1 = %02x\n"), response);
				SDCARD_CS_HIGH();
				return MMC_BLOCK_SET_ERROR;
			}

			mmcSendDummyByte();
			mmcSendCmd(MMC_ACMD23, count & 0x7FFFFF);	// ACMD23(number of blocks for erase) - 23 bits
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			if (/*response != 0x01 && */response != 0x00)
			{
		//		debug_printf_P(PSTR("mmcWriteSectors: 1: MMC_ACMD23 responce R1 = %02x\n"), response);
				SDCARD_CS_HIGH();
				return MMC_BLOCK_SET_ERROR;
			}
		}

		// send write command
		mmcSendDummyByte();
		mmcSendCmd(MMC_WRITE_MULTIPLE_BLOCK, sector * mmcAddressMultiplier);

		// check if the MMC acknowledged the write block command
		// it will do this by sending an affirmative response
		// in the R1 format (0x00 is no errors)
		if (mmcGetXXResponse(MMC_R1_RESPONSE) == MMC_R1_RESPONSE)
		{
			do
			{
				spi_progval8_p1(targetsdcard, 0xff);
				// send the data token to signify the start of the data
				spi_progval8_p2(targetsdcard, MMC_START_DATA_MULTIPLE_BLOCK_WRITE);
				spi_complete(targetsdcard);


				// clock the actual data transfer and transmitt the bytes
				spi_send_frame(targetsdcard, buff, MMC_SECTORSIZE);

				// put CRC bytes (not really needed by us, but required by MMC)
				spi_progval8_p1(targetsdcard, 0xff);
				spi_progval8_p2(targetsdcard, 0xff);
				spi_complete(targetsdcard);
				//unsigned char rr = spi_read_byte(targetsdcard, 0xff);
				//debug_printf_P(PSTR("mmcWriteSectors: 1: rr=%02x\n"), rr);
				//if ((rr & 0x1f) != 0x05)
				//{
				//	break;
				//}
				if (mmcCheckBusy() != MMC_SUCCESS)
					break;


				buff += MMC_SECTORSIZE;
			} while (-- count);
			// stop thh train
			//spi_progval8_p1(targetsdcard, 0xff);
			// send the data token to signify the start of the data
			spi_progval8_p1(targetsdcard, MMC_STOP_DATA_MULTIPLE_BLOCK_WRITE);
			spi_complete(targetsdcard);

			// read the data response xxx0<status>1 : status 010: Data accected, status 101: Data
			//   rejected due to a crc error, status 110: Data rejected due to a Write error.
			//waitwhilebusy();	// 1) отсюда убираю и вставляю в начало всех функций чтения/записи/sync

			rvalue = count == 0 ? MMC_SUCCESS : MMC_WRITE_ERROR;
		}
		else
		{
			// the MMC never acknowledge the write command
			//debug_printf_P("MMC_BLOCK_SET_ERROR\n");
			rvalue = MMC_BLOCK_SET_ERROR;   // 2
		}
		// give the MMC the required clocks to finish up what ever it needs to do
		//  for (i = 0; i < 9; ++i)
		//    spi_progval8(targetsdcard, 0xff);

		SDCARD_CS_HIGH();
		// Send 8 Clock pulses of delay.
		////////spi_read_byte(targetsdcard, 0xff);
		//debug_printf_P(PSTR("write multiblock, count=%d done\n"), count);
		return rvalue;
	}
} // mmc_write_block
unsigned char waitwhilebusy(void)
{
	unsigned long i;
	// skip busy
	for (i = 0; i <= RESPLIMITREAD; ++ i)
	{
		unsigned char v = spi_read_byte(targetsdcard, 0xff);
		//debug_printf_P(PSTR("mmcGetResponseR1b 2: %02x\n"), v);
		if (v != 0)
			return 0;
	}
	return 1;
}
uint_fast8_t mmcGetXXResponse(uint_fast8_t resp)
{
	//Response comes 1-8bytes after command
	//the first bit will be a 0
	//followed by an error code
	//data will be 0xff until response
	long i = 0;

	uint_fast8_t response;

	while (i <= RESPLIMITREAD)	// у больших карт памяти сильно увеличено время ожидания начала чтения данных
	{
		response = spi_read_byte(targetsdcard, 0xff);
		if (response == resp)
			break;
		i ++;
	}
	//debug_printf_P(PSTR("waiting count: %d\n"), i);
	return response;
}

char mmcCheckBusy(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i = 0;

  char response;
  char rvalue;
  while (i <= 64)
  {
    response = spi_read_byte(targetsdcard, 0xff);
    response &= 0x1f;
    switch (response)
    {
      case 0x05: rvalue = MMC_SUCCESS; break;
      case 0x0b: return MMC_CRC_ERROR;
      case 0x0d: return MMC_WRITE_ERROR;
      default:
        rvalue = MMC_OTHER_ERROR;
        break;
    }
    if (rvalue == MMC_SUCCESS)
		break;
    i ++;
  }
  i = 0;
  do
  {
    response = spi_read_byte(targetsdcard, 0xff);
    i ++;
  } while (response == 0);
  return rvalue;
}
char mmcReadSectors(
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
	)
{
	uint32_t ifor1;
	char rvalue = MMC_RESPONSE_ERROR;

	if (count == 1)
	{
		// single block read
		// CS = LOW (on)
		SDCARD_CS_LOW();

		mmcSendDummyByte();
		if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
		{
			SDCARD_CS_HIGH();
			return MMC_RESPONSE_ERROR;
		}

		// send read command MMC_READ_SINGLE_BLOCK=CMD17
		mmcSendDummyByte();
		mmcSendCmd(MMC_READ_SINGLE_BLOCK, sector * mmcAddressMultiplier);
		//mmcCheckBusy();	// INSERTED

		// Send 8 Clock pulses of delay, check if the MMC acknowledged the read block command
		// it will do this by sending an affirmative response
		// in the R1 format (0x00 is no errors)
		if (mmcGetResponseR1() == 0x00)
		{
			// now look for the data token to signify the start of
			// the data
			if (mmcGetXXResponse(MMC_START_DATA_BLOCK_TOKEN) == MMC_START_DATA_BLOCK_TOKEN)
			{
				// clock the actual data transfer and receive the bytes; mmcGetXXResponse automatically finds the Data Block
				spi_read_frame(targetsdcard, buff, MMC_SECTORSIZE);
				// get CRC bytes (not really needed by us, but required by MMC)
				spi_progval8_p1(targetsdcard, 0xff);
				spi_progval8_p2(targetsdcard, 0xff);
				spi_complete(targetsdcard);
				rvalue = MMC_SUCCESS;
			}
			else
			{
				// the data token was never received
				//debug_printf_P("MMC_DATA_TOKEN_ERROR\n");
				rvalue = MMC_DATA_TOKEN_ERROR;      // 3
			}
		}
		else
		{
			// the MMC never acknowledge the read command
			//debug_printf_P("MMC_RESPONSE_ERROR\n");
			rvalue = MMC_RESPONSE_ERROR;          // 2
		}

		SDCARD_CS_HIGH();
		////////spi_read_byte(targetsdcard, 0xff);
		return rvalue;
	}
	else
	{
		// multi block read
		// CS = LOW (on)
		SDCARD_CS_LOW();

		mmcSendDummyByte();
		if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
		{
			SDCARD_CS_HIGH();
			return MMC_RESPONSE_ERROR;
		}


		// send read command MMC_READ_SINGLE_BLOCK=CMD17
		mmcSendDummyByte();
		mmcSendCmd(MMC_READ_MULTIPLE_BLOCK, sector * mmcAddressMultiplier);
		//mmcCheckBusy();	// INSERTED

		// Send 8 Clock pulses of delay, check if the MMC acknowledged the read block command
		// it will do this by sending an affirmative response
		// in the R1 format (0x00 is no errors)
		if (mmcGetResponseR1() == 0x00)
		{
			do
			{
				// now look for the data token to signify the start of
				// the data
				if (mmcGetXXResponse(MMC_START_DATA_BLOCK_TOKEN) == MMC_START_DATA_BLOCK_TOKEN)
				{
					// clock the actual data transfer and receive the bytes; mmcGetXXResponse automatically finds the Data Block
					spi_read_frame(targetsdcard, buff, MMC_SECTORSIZE);
					// get CRC bytes (not really needed by us, but required by MMC)
					spi_progval8_p1(targetsdcard, 0xff);
					spi_progval8_p2(targetsdcard, 0xff);
					spi_complete(targetsdcard);
					rvalue = MMC_SUCCESS;

					buff += MMC_SECTORSIZE;
				}
				else
				{
					// the data token was never received
				//	debug_printf_P("mmcReadSectors: MMC_DATA_TOKEN_ERROR\n");
					rvalue = MMC_DATA_TOKEN_ERROR;      // 3
					break;
				}
			} while (-- count);

			mmcSendCmd(MMC_STOP_TRANSMISSION, 0);
			mmcSendDummyByte();
			if (mmcGetResponseR1b() != 0x00)
			{
	//			debug_printf_P("mmcReadSectors: MMC_STOP_TRANSMISSION error\n");
				rvalue = MMC_RESPONSE_ERROR;    
			}
			mmcCheckBusy();
		}
		else
		{
			// the MMC never acknowledge the read command
//			debug_printf_P("SD_ReadSectors: MMC_READ_MULTIPLE_BLOCK: MMC_RESPONSE_ERROR\n");
			rvalue = MMC_RESPONSE_ERROR;          // 2
		}

		SDCARD_CS_HIGH();
		////////spi_read_byte(targetsdcard, 0xff);
		return rvalue;
	}
}// mmc_read_block

unsigned char mmcGetResponseR1b(void)
{
	unsigned long i;

	unsigned char response;

	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		//debug_printf_P(PSTR("mmcGetResponseR1b 1: %02x\n"), response);
		if ((response & 0x80) == 0)
			break;
	}
	// skip busy
	for (; i <= RESPLIMIT; ++ i)
	{
		unsigned char v = spi_read_byte(targetsdcard, 0xff);
		//debug_printf_P(PSTR("mmcGetResponseR1b 2: %02x\n"), v);
		if (v != 0)
			break;
	}
	return response;
}
