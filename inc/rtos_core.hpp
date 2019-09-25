#ifndef RTOS_CORE_HPP
#define RTOS_CORE_HPP

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "debug_leds_driver.hpp"
#include "debug_usart_driver.hpp"
#include "fpga_driver.hpp"
#include "spi_flash_driver.hpp"

#include "main_task.hpp"

class rtos_core {
public:
  rtos_core();
  ~rtos_core();
  void init();

private:
  RCC_ClocksTypeDef *RCC_Clocks;

  main_task *main_task_ptr;

  inline void delay_us(uint32_t us);
  inline void delay_ms(uint32_t ms);
};

extern "C" {

/* RTOS IRQ handlers */
void vApplicationMallocFailedHook();
void vApplicationTickHook();
void vApplicationIdleHook();
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
}

#endif /* RTOS_CORE_HPP */
