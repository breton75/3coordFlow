#include "tdc.h"


#define SKO_size 12

void tdc_init(void)
{
    SPI_InitTypeDef SPI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //   NVIC_InitTypeDef NVIC_InitStructure;


    /** STM32 & SPI  **/
    ///
    GPIO_InitStructure.GPIO_Pin = STM32_TRIGGER_1_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    
    ///
    GPIO_InitStructure.GPIO_Pin = STM32_TRIGGER_2_Pin | STM32_TRIGGER_3_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);   
    
    ///
    GPIO_InitStructure.GPIO_Pin = SPI_CLK_TDC_Pin | SPI_MISO_TDC_Pin | SPI_MOSI_TDC_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    ///
    GPIO_InitStructure.GPIO_Pin = STM_TDC_INT_1_Pin | STM_TDC_INT_2_Pin | STM_TDC_INT_3_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /** TDC7200 **/
    ///
    GPIO_InitStructure.GPIO_Pin = STM_TDC7200_EN1_Pin | STM_TDC7200_EN2_Pin | STM_TDC7200_EN3_Pin;                      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    ///
    GPIO_InitStructure.GPIO_Pin = TDC7200_SPI_CSB_1_Pin | TDC7200_SPI_CSB_2_Pin | TDC7200_SPI_CSB_3_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    
    /** TDC1000 **/
    ///
    GPIO_InitStructure.GPIO_Pin = TDC1000_SPI_CSB_1_Pin| TDC1000_SPI_CSB_2_Pin | TDC1000_SPI_CSB_3_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ///
    GPIO_InitStructure.GPIO_Pin = TDC1000_CHSEL_Pin | TDC1000_RESET_Pin | TDC1000_ENABLE_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    ///
    GPIO_InitStructure.GPIO_Pin = TDC1000_ERRB_1_Pin | TDC1000_ERRB_2_Pin | TDC1000_ERRB_3_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ///
    GPIO_InitStructure.GPIO_Pin = TDC1000_STM_START_1_Pin | TDC1000_STM_STOP_1_Pin | 
                                  TDC1000_STM_START_2_Pin | TDC1000_STM_STOP_2_Pin | 
                                  TDC1000_STM_START_3_Pin | TDC1000_STM_STOP_3_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &GPIO_InitStructure);



    /** Запускаем тактирование пинов **/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
    
    /** SPI1 Configuration ----------------------------------------------------*/
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low ;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;

    /** SPI_BaudRatePrescaler
     *  000 — fPCLK/2,   001 — fPCLK/4,  010 — fPCLK/8,
     *  011 — fPCLK/16,  100 — fPCLK/32, 101 — fPCLK/64,
     *  110 — fPCLK/128, 111 — fPCLK/256                        **/

    /** запускаем SPI **/
    SPI_Init(SPI2, &SPI_InitStruct);
    SPI_Cmd(SPI2, ENABLE);
    
    /** передергиваем ENABLE, чтобы сбросить все микросхемы **/
    /* 1. After powering up the device, the EN pin needs to be low.
     * There is one low to high transition required while VDD is supplied for correct initialization of the device. */
    GPIO_ResetBits(GPIOB, STM_TDC7200_EN1_Pin | STM_TDC7200_EN2_Pin | STM_TDC7200_EN3_Pin);
    GPIO_ResetBits(GPIOC, TDC1000_ENABLE_Pin | TDC1000_RESET_Pin);
    GPIO_ResetBits(GPIOE, STM_TDC_INT_1_Pin | STM_TDC_INT_2_Pin | STM_TDC_INT_3_Pin);
//     for(ifor = 0; ifor < 500000; ifor++); // ??? зачем ??
    GPIO_SetBits(GPIOB, STM_TDC7200_EN1_Pin | STM_TDC7200_EN2_Pin | STM_TDC7200_EN3_Pin);
    GPIO_SetBits(GPIOC, TDC1000_ENABLE_Pin);//  | TDC1000_RESET);

    
    /*! пишем конфигурации в микросхемы TDC !*/
    configure7200(TDC7200_1);
    configure7200(TDC7200_2);
    configure7200(TDC7200_3);
    
    configure1000(TDC1000_1);
    configure1000(TDC1000_2);
    configure1000(TDC1000_3);
    
}

void configure7200(uint8_t tdc7200chip)
{
  chip_select(tdc7200chip);
  write_spi(TDC7200_CONFIG1_ADRESS, TDC7200_CONFIG1);
  write_spi(TDC7200_CONFIG2_ADRESS, TDC7200_CONFIG2);
  write_spi(TDC7200_INT_STATUS_ADRESS, TDC7200_INT_STATUS);
  write_spi(TDC7200_INT_MASK_ADRESS, TDC7200_INT_MASK);
  write_spi(TDC7200_COARSE_CNTR_OVF_H_ADRESS, TDC7200_COARSE_CNTR_OVF_H);
  write_spi(TDC7200_COARSE_CNTR_OVF_H_ADRESS, TDC7200_COARSE_CNTR_OVF_H);
  write_spi(TDC7200_CLOCK_CNTR_OVF_H_ADRESS, TDC7200_CLOCK_CNTR_OVF_H);
  write_spi(TDC7200_CLOCK_CNTR_STOP_MASK_H_ADRESS, TDC7200_CLOCK_CNTR_STOP_MASK_H);
  write_spi(TDC7200_CLOCK_CNTR_STOP_MASK_L_ADRESS, TDC7200_CLOCK_CNTR_STOP_MASK_L);
  chip_select(TDC_NONE);
}

void configure1000(uint8_t tdc1000chip)
{
  chip_select(tdc1000chip);
  write_spi(TDC1000_CONFIG_1_ADRESS, TDC1000_CONFIG_1);
  write_spi(TDC1000_CONFIG_0_ADRESS, TDC1000_CONFIG_0);
  write_spi(TDC1000_CONFIG_2_ADRESS, TDC1000_CONFIG_2);
  write_spi(TDC1000_CONFIG_3_ADRESS, TDC1000_CONFIG_3);
  write_spi(TDC1000_CONFIG_4_ADRESS, TDC1000_CONFIG_4);
  write_spi(TDC1000_TOF_1_ADRESS, TDC1000_TOF_1);
  write_spi(TDC1000_TOF_0_ADRESS, TDC1000_TOF_0);
  write_spi(TDC1000_TIMEOUT_ADRESS, TDC1000_TIMEOUT);
  write_spi(TDC1000_CLOCK_RATE_ADRESS, TDC1000_CLOCK_RATE);
  chip_select(TDC_NONE);
  
}

MEASURE_RESULT measure(MEASURE_SET mset)
{
  MEASURE_RESULT result;
  memset(&result, 0, sizeof(MEASURE_RESULT));
  result.chip = mset.tdc7200;
  
  /* сбрасываем регистр ошибок и сигнал ERRB на TDC1000 */
  write_chip(mset.tdc1000, TDC1000_ERROR_FLAGS_ADRESS, 1);
  
  /** производим измерение TOF в одну сторону **/
  /* устанавливаем на TDC1000 1ый канал для измерений */
  GPIO_ResetBits(GPIOC, TDC1000_CHSEL_Pin);
  
  /* получаем tof 1 */
	MEASURE_RESULT r1 = get_tof(mset);
  if((r1.err | r1.tdc1000err | r1.tdc7200err) != 0)
    return r1;
  
  result.tof1 = r1.tof1;
  
  
  /*! производим измерение TOF в обратную сторону */
  /* устанавливаем на TDC1000 2ой канал для измерений */
  GPIO_SetBits(GPIOC, TDC1000_CHSEL_Pin);
  
  /* получаем tof 2 */
  MEASURE_RESULT r2 = get_tof(mset);
  if((r2.err | r2.tdc1000err | r2.tdc7200err) != 0)
    return r2;
  
  result.tof2 = r1.tof2;
  
  return result;
  
}

void write_chip(uint8_t chip, uint16_t addr, uint8_t data)
{
  chip_select(chip);
  
  uint16_t spi_data =  TDC_WRITE | TDC_ADDR(addr) | data;
  
  /* ждем готовность шины к записи */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) ;
  SPI_I2S_SendData(SPI2, spi_data);
  
  /* ждем готовность шины */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET) ;
  SPI_I2S_ReceiveData(SPI2);
  
  chip_select(TDC_NONE);
  
}

void write_spi(uint16_t addr, uint8_t data)
{
  uint16_t spi_data =  TDC_WRITE | TDC_ADDR(addr) | data;
  
  /* ждем готовность шины к записи */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) ;
  SPI_I2S_SendData(SPI2, spi_data);
  
  /* ждем готовность шины */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET) ;
  SPI_I2S_ReceiveData(SPI2);
}

uint16_t read_chip(uint8_t chip, uint16_t addr)
{
  chip_select(chip);
      
  uint16_t spi_data =  TDC_READ | TDC_ADDR(addr) | 0x10;
  
  /* ждем готовность шины */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET) ;
  SPI_I2S_SendData(SPI2, spi_data);
  
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET);
  
  uint16_t out_data = SPI_I2S_ReceiveData(SPI2);
  
  chip_select(TDC_NONE);
  
  return out_data;
}

uint16_t read_spi(uint16_t addr)
{
  uint16_t spi_data =  TDC_READ | TDC_ADDR(addr) | 0x10;
  
  /* ждем готовность шины */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET) ;
  SPI_I2S_SendData(SPI2, spi_data);
  
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET);
  
  uint16_t out_data = SPI_I2S_ReceiveData(SPI2);
  
  return out_data;
}

void chip_select(uint8_t chip)
{
  /** сбрасываем CSB на всех микросхемах TDC **/
  GPIO_SetBits(GPIOC, TDC7200_SPI_CSB_1_Pin | TDC7200_SPI_CSB_2_Pin | TDC7200_SPI_CSB_3_Pin | 
                      TDC1000_SPI_CSB_1_Pin | TDC1000_SPI_CSB_2_Pin | TDC1000_SPI_CSB_3_Pin);
  
  /** выставляем CSB на нужной микросхеме **/
  switch (chip) {
    case TDC7200_1:
      GPIO_ResetBits(GPIOC, TDC7200_SPI_CSB_1_Pin);
      break;
      
    case TDC7200_2:
      GPIO_ResetBits(GPIOC, TDC7200_SPI_CSB_2_Pin);
      break;
      
    case TDC7200_3:
      GPIO_ResetBits(GPIOC, TDC7200_SPI_CSB_3_Pin);
      break;
      
    case TDC1000_1:
      GPIO_ResetBits(GPIOC, TDC1000_SPI_CSB_1_Pin);
      break;
      
    case TDC1000_2:
      GPIO_ResetBits(GPIOC, TDC1000_SPI_CSB_2_Pin);
      break;
      
    case TDC1000_3:
      GPIO_ResetBits(GPIOC, TDC1000_SPI_CSB_3_Pin);
      break;

  }
  
}

MEASURE_RESULT get_tof(MEASURE_SET mset)
{
  MEASURE_RESULT result;
//  memset(&result, 0, sizeof(TOF_MEASURE));
  result.chip = mset.tdc7200;
  
  /* записываем 1 в последний бит регистра CONFIG1 на TDC7200 */
  write_chip(mset.tdc7200, TDC7200_CONFIG1_ADRESS, TDC7200_CONFIG1 | 1);
  
  /* ждем сигнал завершения измерений */
  while(GPIO_ReadInputDataBit(GPIOE, mset.int_pin) != 0) ;
  
  /* разбираемся, были ли ошибки при измерении */
  result.tdc1000err = read_chip(mset.tdc1000, TDC1000_ERROR_FLAGS_ADRESS);
  
  if(result.tdc1000err != 0)
    return result;
  
  result.tdc7200err = read_chip(mset.tdc7200, TDC7200_INT_STATUS_ADRESS);
  
  if(result.tdc7200err != 0)
    return result;
  
  
  /** если измерение прошло нормально, то производми вычисления **/
  /*! пока только для одного сигнала STOP */
  
  /* читаем регистры tdc7200 с полученными данными */
  chip_select(mset.tdc7200);
  
  uint16_t time1 = read_spi(TDC7200_TIME1_ADRESS);
  uint16_t time2 = read_spi(TDC7200_TIME2_ADRESS);
//  uint16_t time3 = read_spi(tdc7200chip, TDC7200_TIME3_ADRESS);
//  uint16_t time4 = read_spi(tdc7200chip, TDC7200_TIME4_ADRESS);
//  uint16_t time5 = read_spi(tdc7200chip, TDC7200_TIME5_ADRESS);
//  uint16_t time6 = read_spi(tdc7200chip, TDC7200_TIME6_ADRESS);

  uint16_t clock_count1 = read_spi(TDC7200_CLOCK_COUNT1_ADRESS);
//  uint16_t clock_count2 = read_spi(tdc7200chip, TDC7200_CLOCK_COUNT2_ADRESS);
//  uint16_t clock_count3 = read_spi(tdc7200chip, TDC7200_CLOCK_COUNT3_ADRESS);
//  uint16_t clock_count4 = read_spi(tdc7200chip, TDC7200_CLOCK_COUNT4_ADRESS);
//  uint16_t clock_count5 = read_spi(tdc7200chip, TDC7200_CLOCK_COUNT5_ADRESS);
//  uint16_t clock_count6 = read_spi(tdc7200chip, TDC7200_CLOCK_COUNT6_ADRESS);
  
  uint16_t calibration1 = read_spi(TDC7200_CALIBRATION1_ADRESS);
  uint16_t calibration2 = read_spi(TDC7200_CALIBRATION2_ADRESS);
  uint8_t calibration2periods = read_spi(TDC7200_CONFIG2_ADRESS);
  
  chip_select(TDC_NONE);
  
  /* определяем количество калибровок */
  switch (calibration2periods & 0xC0) {
    case 0:
      calibration2periods = 2;
      break;
    case 0x40:
      calibration2periods = 10;
      break;
    case 0x80:
      calibration2periods = 20;
      break;
    case 0xC0:
      calibration2periods = 40;
      break;
  }
  
  double clock_period = 1.0 / FREQ;
  double calCount = (calibration2 - calibration1) / (calibration2periods - 1);
  double normLSB = (1.0 / FREQ) / calCount;
  
  result.tof1 = normLSB * time1 + clock_count1 * clock_period - normLSB * time2;
  
  return result;
}


/* СТАРЫЕ ФУНКЦИИ 
void TDC_switch_chanel(char ch)
{
    int ifor;
    //<0 or >5 - all off
    if (ch == 0) {
      
        GPIO_ResetBits(GPIOC, TDC7200_SPI_CSB_1_Pin);
    }
    else {
      
        GPIO_SetBits(GPIOC, TDC7200_SPI_CSB_1_Pin);
    }

    if (ch == 2) {
      
        GPIO_ResetBits(GPIOC, TDC7200_SPI_CSB_2_Pin);
    }
    else {
       
        GPIO_SetBits(GPIOC, TDC7200_SPI_CSB_2_Pin);
    }

    if (ch == 4) {
      
        GPIO_ResetBits(GPIOC, TDC7200_SPI_CSB_3_Pin);
    }
    else {
      
        GPIO_SetBits(GPIOC, TDC7200_SPI_CSB_3_Pin);
    }

    if (ch == 1) {
      
        GPIO_ResetBits(GPIOC, TDC1000_SPI_CSB_1_Pin);
    }
    else
    {
        GPIO_SetBits(GPIOC, TDC1000_SPI_CSB_1_Pin);
    }

    if (ch==3)
    {
        GPIO_ResetBits(GPIOC, TDC1000_SPI_CSB_2_Pin);
    }
    else
    {
        GPIO_SetBits(GPIOC, TDC1000_SPI_CSB_2_Pin);
    }

    if (ch==5)
    {
        GPIO_ResetBits(GPIOC, TDC1000_SPI_CSB_3_Pin);
    }
    else
    {
        GPIO_SetBits(GPIOC, TDC1000_SPI_CSB_3_Pin);
    }

    if (ch<0 || ch>5)
    {
        for(ifor=0; ifor<5000; ifor++);
    }
    else
    {
        //for(ifor=0; ifor<500; ifor++);
    }

}

char TDC_write_data(char ch, unsigned char addr, char data_for_send)
{
    char out_data;
    short sd;
    
    TDC_switch_chanel(ch);

    sd = TDC_WRITE | TDC_ADDR(addr) | data_for_send;
    
    SPI_I2S_SendData(SPI2, sd);
    
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET);
    
    out_data = SPI_I2S_ReceiveData(SPI2);

//    out_data=0x0010;
//        sd=TDC_READ | TDC_ADDR(addr) | out_data;
//        SPI_I2S_SendData(SPI2, sd);
//  while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) != SET);
//  out_data = SPI_I2S_ReceiveData(SPI2);
       
    TDC_switch_chanel(10);
    return out_data;
}

int TDC_read24_data(char ch,unsigned char addr)
{
    int out_data;
    char* df=(char*)&out_data;
    short sd;
    char* ddf=(char*)&sd;
    TDC_switch_chanel(ch);

    out_data=0x0010;
    sd=TDC_READ | TDC_ADDR(addr);
    SPI_I2S_SendData(SPI2, sd);
    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) != SET);
    sd = SPI_I2S_ReceiveData(SPI2);
    df[3]=ddf[1];
    df[2]=ddf[0];
    SPI_I2S_SendData(SPI2, 0x00);
    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) != SET);
    sd = SPI_I2S_ReceiveData(SPI2);
    df[1]=ddf[1];
    df[0]=ddf[0];
    out_data = out_data & 0x7FFFFF;
    TDC_switch_chanel(10);
    return out_data;
}

char TDC_read_data(char ch,unsigned char addr)
{
    char out_data;
    short sd;
    TDC_switch_chanel(ch);
    out_data=0x0010;
    sd=TDC_READ | TDC_ADDR(addr) | out_data;
    SPI_I2S_SendData(SPI2, sd);
    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) != SET);
    out_data = SPI_I2S_ReceiveData(SPI2);

    TDC_switch_chanel(10);
    return out_data;
}

int the_printf(const char *str, ...);

int TDC_one_TOF(char ch, double* calc_time)
{
    int ifor2;
    int CALIBRATION2,CALIBRATION1,CLOCK_COUNT1,TIME1,TIME2,TDC_period, TDC_buff;
    double calCount,normLSB;


    TDC_write_data(ch,TI_TDC720x_CONFIG1_REG,TDC7200_CONFIG1 | 0x01);
    ifor2=0;
    while ((TDC_buff=TDC_read_data(ch,TI_TDC720x_CONFIG1_REG) % 2)==1 && ifor2<500) ifor2++;
    if (TDC_buff!=0)
    {
        the_printf("TDC_GetTOF error %d\n\r",TDC_buff);
        return TDC_buff;
    }

    TDC_period= TDC_read_data(ch,TDC7200_CONFIG2) & 0xC0;
    TDC_period = TDC_period>>6;

    switch (TDC_period)
    {
    case (0):
        TDC_period=2;
        break;
    case (1):
        TDC_period=10;
        break;

    case (2):
        TDC_period=20;
        break;
    case (3):
        TDC_period=40;
        break;

    default:
        TDC_period=10;

    }

    
//                the_printf("TDC_GetTOF TIME1 %d\n\r",TDC_read24_data(ch,TI_TDC720x_TIME1_REG));
//                the_printf("TDC_GetTOF TIME2 %d\n\r",TDC_read24_data(ch,TI_TDC720x_TIME2_REG));
//                the_printf("TDC_GetTOF TIME3 %d\n\r",TDC_read24_data(ch,TI_TDC720x_TIME3_REG));
//                the_printf("TDC_GetTOF TIME4 %d\n\r",TDC_read24_data(ch,TI_TDC720x_TIME4_REG));
//                the_printf("TDC_GetTOF TIME5 %d\n\r",TDC_read24_data(ch,TI_TDC720x_TIME5_REG));
//                the_printf("TDC_GetTOF TIME6 %d\n\r",TDC_read24_data(ch,TI_TDC720x_TIME6_REG));
    

    if (TDC7200_WORK_MODE==1)
    {
        CALIBRATION1=TDC_read24_data(ch,TI_TDC720x_CALIBRATION1_REG);
        CALIBRATION2=TDC_read24_data(ch,TI_TDC720x_CALIBRATION2_REG);
        TIME1=TDC_read24_data(ch,TI_TDC720x_TIME1_REG);
        CLOCK_COUNT1=TDC_read24_data(ch,TI_TDC720x_CLOCK_COUNT1_REG);
        calCount = (CALIBRATION2-CALIBRATION1)/(TDC_period-1);
        normLSB = (1./8000000.)/calCount;
        *calc_time=TIME1*normLSB;
        return 0;
    }

    if (TDC7200_WORK_MODE==2)
    {
        CALIBRATION1=TDC_read24_data(ch,TI_TDC720x_CALIBRATION1_REG);
        CALIBRATION2=TDC_read24_data(ch,TI_TDC720x_CALIBRATION2_REG);
        TIME1=TDC_read24_data(ch,TI_TDC720x_TIME1_REG);
        TIME2=TDC_read24_data(ch,TI_TDC720x_TIME2_REG);
        CLOCK_COUNT1=TDC_read24_data(ch,TI_TDC720x_CLOCK_COUNT1_REG);
        calCount = (CALIBRATION2-CALIBRATION1)/(TDC_period-1);
        normLSB = (1./8000000.)/calCount;
        *calc_time=TIME1*normLSB+(CLOCK_COUNT1)*(1./8000000.)-TIME2*normLSB;
        return 0;
    }
    return -2;

}

int TDC_GetTOF(char ch,double* tof_dir,double* tof_back, double hard_correction)
{
    int CALIBRATION2,CALIBRATION1,CLOCK_COUNT1,TIME1,TIME2,TDC_period, TDC_buff;

    int ifor2;
	
		//the_printf("TDC_wait_for_stabilization(%d) %g\n\r",ch,TDC_wait_for_stabilization(ch));


    TDC_write_data(ch+1,TI_TDC1000_CONFIG2_REG,TI_TDC1000_CONFIG2_REG_VALUE_CH1);
    if (TDC_one_TOF(ch,tof_dir)!=0)
    {
        return -1;
    }

    TDC_write_data(ch+1,TI_TDC1000_CONFIG2_REG,TI_TDC1000_CONFIG2_REG_VALUE_CH2);
    if (TDC_one_TOF(ch,tof_back)!=0)
    {
        return -1;
    }

    *tof_dir+=hard_correction;
    *tof_back+=hard_correction;
    return 0;

}

double TDC_wait_for_stabilization(char ch)
{
	int iteration,i2,koef;
    double V_temp[SKO_size],SKO;
	double t_dir,t_back,VV;
	int steps_done=0;
	
	for (iteration=0;iteration<12;iteration++)
    {
        V_temp[iteration]=iteration*1000;
    }
	
	    for (iteration=0;iteration<SKO_size;iteration++)
    {
        V_temp[iteration]=iteration*1000;
    }
    SKO=1000;
    iteration=0;
		koef=1;
    while (SKO>1.5e-15)
    {
        iteration++;
			steps_done++;
        if (iteration>=SKO_size) iteration=0;
				if (steps_done>100)
				{					
        TDC_write_data(ch+1,TI_TDC1000_CONFIG3_REG,TI_TDC1000_CONFIG3_REG_VALUE | koef);
					the_printf("Change TDC1000 setting , %d\n\r",koef);
					steps_done=0;
					koef++;
					if (koef>=7) return -1;
				}
        if (TDC_GetTOF(ch,&t_dir,&t_back,0)==0)
        {
            V_temp[iteration]=(t_dir+t_back)/2;
        }
        else
        {
            V_temp[iteration]=iteration*1000;
            the_printf("Error to get TOF 1\n\r");
						return -1;
        }
        SKO=0;
        for (i2=0;i2<SKO_size;i2++)
        {
            SKO+=V_temp[i2];
        }
        VV=SKO/12;
        SKO=0;
        for (i2=0;i2<SKO_size;i2++)
        {
            SKO+=(V_temp[i2]-VV)*(V_temp[i2]-VV);
        }
        SKO/=SKO_size;


    }
		
		the_printf("TDC_stabilization(%d) on time  %g\n\r",ch,VV);
		return SKO;

	
}

double TDC_Calibrate(double time_delay, float first_max_delay, int max_steps,double dis1, double dis2, double dis3)
{
    double t_dir,t_back;
    double t1,t2,t3;
    double hard_delay=time_delay;
    double V1,V2,V3,Vs;
    double max_delta=first_max_delay;
    int iteration;
	
		the_printf("TDC_wait_for_stabilization(4) %g\n\r",TDC_wait_for_stabilization(4));
		the_printf("TDC_wait_for_stabilization(2) %g\n\r",TDC_wait_for_stabilization(2));
	the_printf("TDC_wait_for_stabilization(0) %g\n\r",TDC_wait_for_stabilization(0));

    if (TDC_GetTOF(0,&t_dir,&t_back,hard_delay)==0)
    {
        t1=(t_dir+t_back)/2;
        V1=(dis1/t_dir+dis1/t_back)/2;
    }
    else
    {
        V1=-1;
        the_printf("Error to get TOF 1\n\r");
    }

    iteration=0;
    while (max_delta>1)
    {
        iteration++;
        if (iteration>max_steps)
        {
            if(max_delta>0.99*first_max_delay)
            {
                iteration=0;
                max_delta*=1.1;
            }
            else
            {
                max_delta*=0.9;
            }
        }
        the_printf("hard_correct = %d    \r",iteration);
        if (TDC_GetTOF(0,&t_dir,&t_back,hard_delay)==0)
        {
            t1=(t_dir+t_back)/2;
            V1=(dis1/t_dir+dis1/t_back)/2;
        }
        else
        {
            V1=-1;
            the_printf("Error to get TOF 1\n\r");
        }

        if (TDC_GetTOF(2,&t_dir,&t_back,hard_delay)==0)
        {
            t2=(t_dir+t_back)/2;
            V2=(dis2/t_dir+dis2/t_back)/2;
        }
        else
        {
            V2=-1;
            the_printf("Error to get TOF 2\n\r");
        }

        if (TDC_GetTOF(4,&t_dir,&t_back,hard_delay)==0)
        {
            t3=(t_dir+t_back)/2;
            V3=(dis3/t_dir+dis3/t_back)/2;
        }
        else
        {
            V3=-1;
            the_printf("Error to get TOF 3\n\r");
        }

        if (V1>0 && V2>0 && V3>0)
        {
            if (V1-V2>max_delta || V1-V2<-max_delta)
            {
                //the_printf("fabs(V1-V2) = %f\n\r",V1-V2);
            }
            else
            {
                if (V1-V3>max_delta || V1-V3<-max_delta)
                {
                    //the_printf("fabs(V1-V3) = %f\n\r",V1-V3);
                }
                else
                {
                    if (V2-V3>max_delta || V2-V3<-max_delta)
                    {
                        //the_printf("fabs(V2-V3) = %f\n\r",V2-V3);
                    }
                    else
                    {
                        //korrection
                        iteration=0;
                        max_delta*=0.99;
                        //V1=(V1+V2+V3)/3;
                        Vs=(dis3-dis1)/(t3-t1);
                        //Vs+=(dis3-dis2)/(t3-t2);
                        //Vs+=(dis2-dis1)/(t2-t1);
                        //Vs/=3;
                        the_printf("_V_ = %f\n\r",Vs);
												if (max_delta<10 && ((V1+V2+V3)/3-Vs>5 || (V1+V2+V3)/3-Vs<-5)) max_delta=10;
                        hard_delay+=max_delta*((V1+V2+V3)/3-Vs)*1e-10;
                        the_printf("hard_correct = %g (V1=%f\t,V2=%f\t,V3=%f\t)\n\r",hard_delay,V1,V2,V3);
                    }
                }
            }
        }
        else
        {
            the_printf("V undef value V1=%f V2=%f V3=%f\n\r",V1,V2,V3);
        }
    }

    return hard_delay;

}
**/
