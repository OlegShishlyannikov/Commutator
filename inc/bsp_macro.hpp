#ifndef BSP_MACRO_HPP
#define BSP_MACRO_HPP

#include "stm32f10x_conf.h"
#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdint>

#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>

/* DEBUG_USART GPIO settings */
#define DEBUG_USART_GPIO_PORT GPIOA
#define DEBUG_USART_GPIO_RX_PIN GPIO_Pin_10
#define DEBUG_USART_GPIO_TX_PIN GPIO_Pin_9

/* DEBUG_USART settings */
#define DEBUG_USART_PORT USART1
#define DEBUG_USART_IT_VECTOR USART1_IRQn
#define DEBUG_USART_IT_VECTOR_TYPE USART_IT_RXNE

/* PC_USART GPIO port settings */
#define PC_USART_GPIO_PORT GPIOA
#define PC_USART_GPIO_RX_PIN GPIO_Pin_3
#define PC_USART_GPIO_TX_PIN GPIO_Pin_2

/* PC_USART settings */
#define PC_USART_PORT USART2
#define PC_USART_IT_VECTOR USART2_IRQn
#define PC_USART_IT_VECTOR_TYPE USART_IT_RXNE

/* DEBUG LEDS GPIO settings */
#define DEBUG_LEDS_GPIO_PORT GPIOC
#define DEBUG_LED1_GPIO_PIN GPIO_Pin_6
#define DEBUG_LED2_GPIO_PIN GPIO_Pin_7
#define DEBUG_LED3_GPIO_PIN GPIO_Pin_8
#define DEBUG_LED4_GPIO_PIN GPIO_Pin_9

/* SPI Memory GPIO port settings */
#define SPI_FLASH_GPIO_PORT GPIOA
#define SPI_FLASH_GPIO_MOSI_PIN GPIO_Pin_7
#define SPI_FLASH_GPIO_MISO_PIN GPIO_Pin_6
#define SPI_FLASH_GPIO_SCK_PIN GPIO_Pin_5
#define SPI_FLASH_GPIO_NSS_PIN GPIO_Pin_4

/* SPI Memory port settings */
#define SPI_FLASH_SPI_PORT SPI1

/* SPI Memory RST & HOLD GPIO port settings */
#define SPI_FLASH_WP_HOLD_PORT GPIOC
#define SPI_FLASH_WP_HOLD_GPIO_WP_PIN GPIO_Pin_4
#define SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN GPIO_Pin_5

/* FPGA GPIO port settings */
#define FPGA_GPIO_PORT GPIOB
#define FPGA_GPIO_START_PIN GPIO_Pin_10
#define FPGA_GPIO_RST_PIN GPIO_Pin_11
#define FPGA_GPIO_NSS_PIN GPIO_Pin_12
#define FPGA_GPIO_SCK_PIN GPIO_Pin_13
#define FPGA_GPIO_MISO_PIN GPIO_Pin_14
#define FPGA_GPIO_MOSI_PIN GPIO_Pin_15

/* FPGA SPI port settings */
#define FPGA_SPI_PORT SPI2

/* Relays count */
#define RELAYS_COUNT 80

#endif /* BSP_MACRO_HPP */
