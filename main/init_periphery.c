#include "init_periphery.h"
void Init_USART1(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStruct;
	// Включение тактирования USART, порта А, и порта алтернативных функций
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_USART1 ), ENABLE);
	
	//пин-ресет
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
 	// инициализация вывода  - Тх
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; //настройка скорости работы
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP; // Режим альтернативной функции в двух состояниях
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Инициализация вывода -Rx
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; //настройка скорости работы
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; //Вход без подтяжки.
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Инициализируем работу USART
	USART_InitStruct.USART_BaudRate = BaudRate; // Скорость 9600 бод/с
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;  //Длина слова - 8 бит
	USART_InitStruct.USART_StopBits=USART_StopBits_1; //1 стоп-бит
	USART_InitStruct.USART_Parity=USART_Parity_No; //без проверки четности
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //без аппаратного контроля
	USART_InitStruct.USART_Mode=USART_Mode_Rx | USART_Mode_Tx; // устанавливаем режимы чтения и записи (включен передатчик и приемник)


USART_Init(USART1, &USART_InitStruct);
USART_Cmd(USART1, ENABLE);

			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //прерывание по приему	
			NVIC_EnableIRQ (USART1_IRQn); //разрешаем прерывания USART2
	
	
//NVIC_EnableIRQ (USART1_IRQn); //разрешаем прерывания от USART1
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

}
void Init_RCC(void)
{
	/////////////////////////////////////////////////////////////
	///Устанавливаем тактирование для контроллера STM32F107VC ///
	/////////////////////////////////////////////////////////////
	ErrorStatus HSEStartUpStatus;
	RCC_ClocksTypeDef RCC_ClockFreq;

	// Сбрасываем клокинг в "0"
	RCC_DeInit();

	// Включаем внешний кварц
	RCC_HSEConfig(RCC_HSE_ON);

	// Ждём пока устанавливается внешний ВЧ тактовый сигнал
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus != ERROR)
	{
		// Внешний тактовый сигнал появился, стабильный
		// разрешаем использование буфера команд
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//-------------------------------------------------------------------
		// итак
		//Внешний кварц 25МГц
		//Тактовая частота ядра: 72МГц
		// частота шины PCLK2=72МГц
		// частота шины PCLK1=36МГц
		//-------------------------------------------------------------------

		// Flash 2 wait state  двойная задержка
		FLASH_SetLatency(FLASH_Latency_2);

		RCC_HCLKConfig(RCC_SYSCLK_Div1); // HCLK = SYSCLK    AHB clock = SYSCLK

		RCC_PCLK1Config(RCC_HCLK_Div2); // PCLK1 = HCLK/2

		RCC_PCLK2Config(RCC_HCLK_Div1);  // PCLK2 = HCLK 

		// Выставляем делители / множители частоты тактирования
		RCC_PREDIV2Config( RCC_PREDIV2_Div10 );  //   HSE/10 делим частоту от внешнего генератора на 10

		RCC_PLL2Config(RCC_PLL2Mul_16);  //умножаем частоту от внешнего генератора (HSE/5)*16=40

//RCC->CFGR2->
//	RCC_I2S3CLKConfig(RCC_I2S3CLKSource_PLL3_VCO); //не проканало. на SPI другие такты

		//Разрешаем PLL2
		RCC_PLL2Cmd(ENABLE);

		// Ждём пока устаканится PLL2
		while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
		{}

		// PLL2 - устаканился
		// конфигурируем PLL1
		RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div4); //выбираем для PREDIV1SCR в качестве источника PLL2 и делем на 5 ->((HSE/10)*16)/4=10
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_5);	//выбираем для PLLSCR в качестве источника PREDIV1 и умножаем на 9->(((HSE/10)*16)/4)*5=50

		// Разрешаем PLL
		RCC_PLLCmd(ENABLE);

		// Ждём готовности PLL
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{}

		// Выбираем PLL как источник тактирования системы
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		// Ждём пока PLL не станет клокингом системы
		while (RCC_GetSYSCLKSource() != 0x08)
		{}
	}

	RCC_GetClocksFreq(&RCC_ClockFreq);

	// Тактирование Ethernet
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

	// Тактирование портов
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE| RCC_APB2Periph_AFIO , ENABLE);

	// задача: выдать 50МГц на ногу MCO
	// Получаем 50МГц на множителе PLL3
	RCC_PLL3Config(RCC_PLL3Mul_10); // (HSE/10)*10
	
	// запускаем множитель
	RCC_PLL3Cmd(ENABLE);
	// ждём готовность
	while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
	{}
	 // Выдаём сигнал на ногу MCO
	RCC_MCOConfig(RCC_MCO_PLL3CLK); //выбираем в качесве источника для MCO PLL3CLK
}
void Init_gpio(uint16_t USB_pinchec)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	
	//LED
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//sd_card_mode

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//USB detect
	 GPIO_InitStructure.GPIO_Pin = USB_pinchec;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
    //Butt
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
}
void Init_SPI_SD()
{
		GPIO_InitTypeDef GPIO_InitStructure;
  //int dummyread;
	SPI_InitTypeDef  SPI_InitStructure;
	
	/* Enable GPIO clock for CS */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CS | RCC_APB2Periph_GPIO_SPI_SD, ENABLE);
	
	/* Enable SPI clock, SPI1: APB2, SPI2: APB1 */
	RCC_APBPeriphClockCmd_SPI_SD(RCC_APBPeriph_SPI_SD, ENABLE);

	/* Configure I/O for Flash Chip select */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_CS;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_CS, &GPIO_InitStructure);


	//GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

	/* Configure SPI pins: SCK and MOSI with default alternate function (not re-mapped) push-pull */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);
	/* Configure MISO as Input with internal pull-up */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_MISO;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);

	

	/* SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_SPI_SD; // 72000kHz/256=281kHz < 400kHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI_SD, &SPI_InitStructure);
	SPI_CalculateCRC(SPI_SD, DISABLE);
	SPI_Cmd(SPI_SD, ENABLE);

	/* drain SPI */
	while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE) == RESET) { ; }
	/*dummyread = */SPI_I2S_ReceiveData(SPI_SD);
	
}



/*
void Init_SPI_TDC()
{
		GPIO_InitTypeDef GPIO_InitStructure;
int dummyread;
	SPI_InitTypeDef  SPI_InitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	


	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;      //определение одно или двунаправленности
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;					//определиние мода работы(ведущий или ведомый(мастер и раб)) SPI_CR1 (2 разряд - MSTR)
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		//определяет размер данных    SPI_CR1(DFF-разряд11)
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;				//серийные часы устойчивого состояния "CPOL- задает полярность тактового сигнала"(1)
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;			//часы активного фронта "CPHA- задает фазу тактового сигнала;"(0)
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;				//будет управляться программно или аппаратно "SSM(9) – выбирает источник сигнала NSS
															//(0 — с внешнего вывода, 1 — программно;"

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;//было 4 //BR[2:0] — задает скорость обмена (000 — fPCLK/2, 001 — fPCLK/4,
																		 //010 — fPCLK/8, ) 011 — fPCLK/16, 100 — fPCLK/32, 101 — fPCLK/64,
																		 //110 — fPCLK/128, 111 — fPCLK/256,

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;		//определяет с какого бита начинаем слать (7)"LSBFIRST – задает способ передачи
															//(l —LSB младшим разрядом вперед, 0 - MSB)"
	//SPI_InitStruct.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	//SPI_CalculateCRC(SPI2, DISABLE);
	SPI_Cmd(SPI2, ENABLE);

	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) { ; }
	dummyread = SPI_I2S_ReceiveData(SPI2);
	
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE); // разрешение прерывания по приему. если нужно и по передаче, SPI_I2S_IT_RXNE | SPI_I2S_IT_TXNE
	
}
*/
void Init_USART2_NAV  (uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStruct;
	
//----------------------USART2--------------------------------
 //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
 	// инициализация вывода D5 - Тх
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; //настройка скорости работы
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP; // Режим альтернативной функции в двух состояниях
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//Инициализация вывода D6-Rx
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; //настройка скорости работы
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; //Вход без подтяжки.
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	
	// Включение тактирования USART, порта А, и порта алтернативных функций
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_USART1 ), ENABLE);
	
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	

	//Инициализируем работу USART
	USART_InitStruct.USART_BaudRate = BaudRate; // Скорость 9600 бод/с
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;  //Длина слова - 8 бит
	USART_InitStruct.USART_StopBits=USART_StopBits_1; //1 стоп-бит
	USART_InitStruct.USART_Parity=USART_Parity_No; //без проверки четности
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //без аппаратного контроля
	USART_InitStruct.USART_Mode=USART_Mode_Rx | USART_Mode_Tx; // устанавливаем режимы чтения и записи (включен передатчик и приемник)

USART_Init(USART2, &USART_InitStruct);

USART_Cmd(USART2, ENABLE);

			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //прерывание по приему	
			NVIC_EnableIRQ (USART2_IRQn); //разрешаем прерывания USART2
}
