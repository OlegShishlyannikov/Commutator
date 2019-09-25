#include "rtos_core.hpp"

rtos_core::rtos_core() { asm("nop"); }

void rtos_core::init() {
  this->RCC_Clocks = new RCC_ClocksTypeDef;

  /* Start drivers */
  this->main_task_ptr = new main_task;
}

inline void rtos_core::delay_us(uint32_t us) {
  uint32_t ticks = 0;
  RCC_GetClocksFreq(this->RCC_Clocks);
  ticks = (this->RCC_Clocks->SYSCLK_Frequency / 10000000) * us;
  while (ticks--)
    ;
}

inline void rtos_core::delay_ms(uint32_t ms) {
  uint32_t ticks = 0;
  RCC_GetClocksFreq(this->RCC_Clocks);
  ticks = (this->RCC_Clocks->SYSCLK_Frequency / 10000) * ms;
  while (ticks--)
    ;
}

rtos_core::~rtos_core() { asm("nop"); }

void vApplicationTickHook() { asm("nop"); }

void vApplicationMallocFailedHook() { asm("nop"); }

void vApplicationIdleHook() { asm("nop"); }

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  std::printf("[!] Stack overflow in task : %s", pcTaskName);
}
