#define GO_IDLE_STATE            0              //Программная перезагрузка   
#define SEND_IF_COND             8              //Для SDC V2 - проверка диапазона напряжений   
#define READ_SINGLE_BLOCK        17             //Чтение указанного блока данных  
#define WRITE_SINGLE_BLOCK       24             //Запись указанного блока данных
#define SD_SEND_OP_COND	         41             //Начало процесса инициализации  
#define APP_CMD			 55             //Главная команда из ACMD <n> команд
#define READ_OCR		 58             //Чтение регистра OCR


#include "exam_spi.h"


//глобальная переменная для определения типа карты 
uint8_t  SDHC = 1;            
//макроопределения для управления выводом SS

#define CS_ENABLE        spi_cs_low()  //GPIOA->BSRR = GPIO_BSRR_BR4;         
#define CS_DISABLE    	 spi_cs_high()  //GPIOA->BSRR = GPIO_BSRR_BS4;  

//********************************************************************************************
//function	 посылка команды в SD                                		            //
//Arguments	 команда и ее аргумент                                                      //
//return	 0xff - нет ответа   			                                    //
//********************************************************************************************
uint8_t SD_sendCommand(uint8_t cmd, uint32_t arg)
{
  uint8_t response, wait=0, tmp;     
	int ifor1;
  //для карт памяти SD выполнить корекцию адреса, т.к. для них адресация побайтная 
  if(SDHC == 0)		
  if(cmd == READ_SINGLE_BLOCK || cmd == WRITE_SINGLE_BLOCK )  {arg = arg << 9;}
  //для SDHC корекцию адреса блока выполнять не нужно(постраничная адресация)	
 
  CS_ENABLE;
 
  //передать код команды и ее аргумент
  spi_send(cmd | 0x40);
  spi_send(arg>>24);
  spi_send(arg>>16);
  spi_send(arg>>8);
  spi_send(arg);
 
  //передать CRC (учитываем только для двух команд)
  if(cmd == SEND_IF_COND) spi_send(0x87);            
  else                    spi_send(0x95); 
 
  //ожидаем ответ
  while((response = spi_read()) == 0xff) 
   if(wait++ > 0xfe) break;                //таймаут, не получили ответ на команду
 
  //проверка ответа если посылалась команда READ_OCR
  if(response == 0x00 && cmd == 58)     
  {
    tmp = spi_read();                      //прочитат один байт регистра OCR            
    if(tmp & 0x40) SDHC = 1;               //обнаружена карта SDHC 
    else           SDHC = 0;               //обнаружена карта SD
    //прочитать три оставшихся байта регистра OCR
    spi_read(); 
    spi_read(); 
    spi_read(); 
  }
 
  spi_read();
 
  CS_DISABLE; 
 
  return response;
}

//********************************************************************************************
//function	 инициализация карты памяти                         			    //
//return	 0 - карта инициализирована  					            //
//********************************************************************************************
uint8_t SD_init(void)
{
  uint8_t   i;
  uint8_t   response;
  uint8_t   SD_version = 2;	          //по умолчанию версия SD = 2
  uint16_t  retry = 0 ;
	uint32_t 	ifor, ifor1;
 
	CS_DISABLE;
	for(ifor=0;ifor<100;ifor++);
	
	
	
 /// spi_init();                            //инициализировать модуль SPI                        
  for(i=0;i<10;i++) spi_send(0xff);      //послать свыше 74 единиц   
 
  //выполним программный сброс карты
  CS_ENABLE;
  while(SD_sendCommand(GO_IDLE_STATE, 0)!=0x01)                                   
    if(retry++>0x20)  return 1;                    
  CS_DISABLE;
  spi_send (0xff);
  spi_send (0xff);
 
  retry = 0;                                     
  while(SD_sendCommand(SEND_IF_COND,0x000001AA)!=0x01)
  { 
    if(retry++>0xfe) 
    { 
      SD_version = 1;
      break;
    } 
  }
 
 retry = 0;                                     
 do
 {
   response = SD_sendCommand(APP_CMD,0); 
   response = SD_sendCommand(SD_SEND_OP_COND,0x40000000);
   retry++;
   if(retry>0xffe) return 1;                     
 }while(response != 0x00);                      
 
 
 //читаем регистр OCR, чтобы определить тип карты
 retry = 0;
 SDHC = 0;
 if (SD_version == 2)
 { 
   while(SD_sendCommand(READ_OCR,0)!=0x00)
	 if(retry++>0xfe)  break;
 }
 
 return 0; 
}
//********************************************************************************************
//function	 чтение выбранного сектора SD                         			    //
//аrguments	 номер сектора,указатель на буфер размером 512 байт                         //
//return	 0 - сектор прочитан успешно   					            //
//********************************************************************************************
uint8_t SD_ReadSector(uint32_t BlockNumb,uint8_t *buff)
{ 
  uint16_t i=0;
	int ifor1;
  //послать команду "чтение одного блока" с указанием его номера
  if(SD_sendCommand(READ_SINGLE_BLOCK, BlockNumb)) return 1;  
  CS_ENABLE;
  //ожидание  маркера данных
  while(spi_read() != 0xfe)                
  if(i++ > 0xfffe) {CS_DISABLE; return 1;}       
 
  //чтение 512 байт	выбранного сектора
  for(i=0; i<512; i++) *buff++ = spi_read();
 
  spi_read(); 
  spi_read(); 
  spi_read(); 
 
  CS_DISABLE;
 
  return 0;
}

//*********************************************************************************************
//function  обмен данными по SPI1                                                            //
//argument  передаваемый байт                                                                //
//return    принятый байт                                                                    //
//*********************************************************************************************
uint8_t spi_send (uint8_t data)
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
uint8_t spi_read (void)
{ 
  return spi_send(0xff);		  //читаем принятые данные
}