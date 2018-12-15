#include "bootloader.hpp"

bootloader::bootloader()
{  
  char inp;
  this->init_hardware();
  this->setup_interrupts();
  
  /* Fill data */
  this->hardware_name_ptr = new std::string( "Kvarta4 Commutator" );
  this->author_name_ptr = new std::string( "Oleg Shishlyannikov" );
  this->author_mail_ptr = new std::string( "oleg.shishlyannikov.1992@gmail.com, bofpes@gmail.com" );
  this->company_name_ptr = new std::string( "VZPP-S" );
  this->company_address_ptr = new std::string( "Russian Federation, Voronezh city, Leninskiy prospekt 119A" );
  this->hardware_version_ptr = new std::string( "1.0.0" );
  this->software_version_ptr = new std::string( "1.0.0" );

  std::printf( "\r\nName : %s\r\nHardware version : %s\r\nSoftware version : %s\r\nAuthor : %s\r\nAuthor contacts : %s\r\nCompany : %s\r\nCompany address : %s\r\n", this->hardware_name_ptr->c_str(), this->hardware_version_ptr->c_str(), this->software_version_ptr->c_str(), this->author_name_ptr->c_str(), this->author_mail_ptr->c_str(), this->company_name_ptr->c_str(), this->company_address_ptr->c_str() );
  std::printf( "\r\nADCCLK : %u Hz\r\nSYSCLK : %u Hz\r\nHCLK : %u Hz\r\nPCLK1 : %u Hz\r\nPCLK2 : %u Hz\r\n\r\n", this->adcclk_freq, this->sysclk_freq, this->hclk_freq, this->pclk1_freq, this->pclk2_freq );
}

const char * bootloader::get_hardware_name()
{
  return this->hardware_name_ptr->c_str();
}

const char * bootloader::get_author_name()
{
  return this->author_name_ptr->c_str();
}

const char * bootloader::get_author_mail()
{
  return this->author_mail_ptr->c_str();
}

const char * bootloader::get_company_name()
{
  return this->company_name_ptr->c_str();
}

const char * bootloader::get_company_address()
{
  return this->company_address_ptr->c_str();
}

const char * bootloader::get_hardware_version()
{
  return this->hardware_version_ptr->c_str();
}

const char * bootloader::get_software_version()
{
  return this->software_version_ptr->c_str();
}

void bootloader::init_hardware()
{
  RCC_DeInit(); /* Clear prevous RCC configuration */
  RCC_HSEConfig( RCC_HSE_ON ); /* Enable HSE */
  RCC_WaitForHSEStartUp(); /* Wait for HSE startup */
  RCC_HSICmd( DISABLE ); /* Disable HSI */
  RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 ); /* Setup HSE */
  RCC_PLLCmd( ENABLE ); /* Enable PLL */

  while( !RCC_GetFlagStatus( RCC_FLAG_PLLRDY )); /* Wait for PLL startup */
  RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK ); /* Set SYSCLK clock source */
  while( RCC_GetSYSCLKSource() != 0x08 ); /* Wait for sysclk source */
  
  RCC_ADCCLKConfig( RCC_PCLK2_Div2 ); /* Set ADCCLK clock source */
  RCC_HCLKConfig( RCC_SYSCLK_Div1 ); /* Set HCLK clock source */
  RCC_PCLK1Config( RCC_HCLK_Div1 ); /* Set PCLK1 clock source */
  RCC_PCLK2Config( RCC_HCLK_Div1 ); /* Set PCLK2 clock source */
  
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2 | RCC_APB1Periph_SPI2, ENABLE ); /* Enable clocking on PC_USART and FPGA SPI */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_SPI1, ENABLE ); /* Enable clocking on all GPIO ports and SPI_FLASH spi port */
  
  SystemCoreClockUpdate();
  
  /* Get clocks frequency */
  RCC_ClocksTypeDef * RCC_Clocks_ptr = new RCC_ClocksTypeDef;
  RCC_GetClocksFreq( RCC_Clocks_ptr );

  /* Remember clocks frequency */
  this->adcclk_freq = RCC_Clocks_ptr->ADCCLK_Frequency;
  this->sysclk_freq = RCC_Clocks_ptr->SYSCLK_Frequency;
  this->hclk_freq = RCC_Clocks_ptr->HCLK_Frequency;
  this->pclk1_freq = RCC_Clocks_ptr->PCLK1_Frequency;
  this->pclk2_freq = RCC_Clocks_ptr->PCLK2_Frequency;

  /* Setup GPIO's */
  GPIO_InitTypeDef * GPIO_InitStruct_ptr = new GPIO_InitTypeDef;
  
  /* DEBUG_USART GPIO settings */
  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = DEBUG_USART_GPIO_RX_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( DEBUG_USART_GPIO_PORT, GPIO_InitStruct_ptr );

  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = DEBUG_USART_GPIO_TX_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( DEBUG_USART_GPIO_PORT, GPIO_InitStruct_ptr );

  /* PC_USART GPIO settings */
  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = PC_USART_GPIO_RX_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( PC_USART_GPIO_PORT, GPIO_InitStruct_ptr );

  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = PC_USART_GPIO_TX_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( PC_USART_GPIO_PORT, GPIO_InitStruct_ptr );

  /* DEBUG_LEDS GPIO settings */
  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = DEBUG_LED1_GPIO_PIN | DEBUG_LED2_GPIO_PIN | DEBUG_LED3_GPIO_PIN | DEBUG_LED4_GPIO_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( DEBUG_LEDS_GPIO_PORT, GPIO_InitStruct_ptr );

  /* SPI_FLASH GPIO settings */
  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = SPI_FLASH_GPIO_SCK_PIN | SPI_FLASH_GPIO_MOSI_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( SPI_FLASH_GPIO_PORT, GPIO_InitStruct_ptr );

  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = SPI_FLASH_GPIO_MISO_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( SPI_FLASH_GPIO_PORT, GPIO_InitStruct_ptr );

  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = SPI_FLASH_GPIO_NSS_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( SPI_FLASH_GPIO_PORT, GPIO_InitStruct_ptr );
  
  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = SPI_FLASH_WP_HOLD_GPIO_WP_PIN | SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( SPI_FLASH_WP_HOLD_PORT, GPIO_InitStruct_ptr );

  /* FPGA GPIO settings */
  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = FPGA_GPIO_MOSI_PIN | FPGA_GPIO_SCK_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( FPGA_GPIO_PORT, GPIO_InitStruct_ptr );

  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = FPGA_GPIO_NSS_PIN | FPGA_GPIO_START_PIN | FPGA_GPIO_RST_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( FPGA_GPIO_PORT, GPIO_InitStruct_ptr );

  GPIO_StructInit( GPIO_InitStruct_ptr );
  GPIO_InitStruct_ptr->GPIO_Pin = FPGA_GPIO_MISO_PIN;
  GPIO_InitStruct_ptr->GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStruct_ptr->GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( FPGA_GPIO_PORT, GPIO_InitStruct_ptr );

  GPIO_SetBits( FPGA_GPIO_PORT, FPGA_GPIO_MOSI_PIN | FPGA_GPIO_SCK_PIN );
  
  /* Setup usart's */
  USART_InitTypeDef * USART_InitStruct_ptr = new USART_InitTypeDef;
  USART_DeInit( DEBUG_USART_PORT );

  /* DEBUG_USART settings */
  USART_StructInit( USART_InitStruct_ptr );
  USART_InitStruct_ptr->USART_BaudRate = 115200;
  USART_InitStruct_ptr->USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct_ptr->USART_Parity = USART_Parity_No;
  USART_InitStruct_ptr->USART_WordLength = USART_WordLength_8b;
  USART_InitStruct_ptr->USART_StopBits = USART_StopBits_1;
  USART_InitStruct_ptr->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_ITConfig( DEBUG_USART_PORT, DEBUG_USART_IT_VECTOR_TYPE, ENABLE );
  USART_Init( DEBUG_USART_PORT, USART_InitStruct_ptr );
  USART_Cmd( DEBUG_USART_PORT, ENABLE );
  
  /* PC_USART settings */
  USART_StructInit( USART_InitStruct_ptr );
  USART_InitStruct_ptr->USART_BaudRate = 115200;
  USART_InitStruct_ptr->USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct_ptr->USART_Parity = USART_Parity_No;
  USART_InitStruct_ptr->USART_WordLength = USART_WordLength_8b;
  USART_InitStruct_ptr->USART_StopBits = USART_StopBits_1;
  USART_InitStruct_ptr->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_ITConfig( PC_USART_PORT, PC_USART_IT_VECTOR_TYPE, ENABLE );
  USART_Init( PC_USART_PORT, USART_InitStruct_ptr );
  USART_Cmd( PC_USART_PORT, ENABLE );

  /* Setup SPI's */
  SPI_InitTypeDef * SPI_InitStruct_ptr = new SPI_InitTypeDef;

  /* Init spi flash port */
  SPI_StructInit( SPI_InitStruct_ptr );
  SPI_InitStruct_ptr->SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct_ptr->SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct_ptr->SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct_ptr->SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct_ptr->SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct_ptr->SPI_NSS = SPI_NSS_Soft;
  SPI_InitStruct_ptr->SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStruct_ptr->SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct_ptr->SPI_CRCPolynomial = 7;
  SPI_Init( SPI_FLASH_SPI_PORT, SPI_InitStruct_ptr );
  SPI_Cmd( SPI_FLASH_SPI_PORT, ENABLE );

  /* Init FPGA SPI port */
  SPI_StructInit( SPI_InitStruct_ptr );
  SPI_InitStruct_ptr->SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct_ptr->SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct_ptr->SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct_ptr->SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct_ptr->SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct_ptr->SPI_NSS = SPI_NSS_Soft;
  SPI_InitStruct_ptr->SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStruct_ptr->SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct_ptr->SPI_CRCPolynomial = 7;
  SPI_Init( FPGA_SPI_PORT, SPI_InitStruct_ptr );
  SPI_Cmd( FPGA_SPI_PORT, DISABLE );
  
  /* Clear memory */
  delete RCC_Clocks_ptr;
  delete GPIO_InitStruct_ptr;
  delete USART_InitStruct_ptr;
  delete SPI_InitStruct_ptr;
}

void bootloader::setup_interrupts()
{
  /* Create NVIC structure */
  NVIC_InitTypeDef * NVIC_InitStruct_ptr = new NVIC_InitTypeDef;

  /* Setup PC_USART interrupts */
  NVIC_InitStruct_ptr->NVIC_IRQChannel = PC_USART_IT_VECTOR;
  NVIC_InitStruct_ptr->NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStruct_ptr->NVIC_IRQChannelSubPriority = 12;
  NVIC_InitStruct_ptr->NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( NVIC_InitStruct_ptr );

  NVIC_SetPriority( PC_USART_IT_VECTOR, 12 );
  
  // Enable interrupts
  NVIC_EnableIRQ( PC_USART_IT_VECTOR );

  /* Clear memory */
  delete NVIC_InitStruct_ptr;
}

void bootloader::load_rtos()
{
  this->os_core_ptr = new rtos_core;
  this->os_core_ptr->init();
}

void bootloader::start_rtos()
{
  std::printf( "Starting RTOS ...\r\n" );
  vTaskStartScheduler();
}

bootloader::~bootloader()
{
  delete this->os_core_ptr;
  asm( "nop" );
}
