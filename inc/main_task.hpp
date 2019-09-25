#ifndef MAIN_TASK_HPP
#define MAIN_TASK_HPP

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "bsp_macro.hpp"

#include "debug_leds_driver.hpp"
#include "fpga_driver.hpp"
#include "pc_usart_driver.hpp"
#include "spi_flash_driver.hpp"

#define MAIN_TASK_PRIORITY 1

class main_task {
public:
  main_task();
  virtual ~main_task();

  void main_task_code(void *pvParams);
  void pc_usart_callback(USART_TypeDef *USARTx);

  static bool flash_test();
  static bool fpga_test();
  static bool fpga_initial_test();
  static bool global_test();

private:
  xTaskHandle *main_task_handle = new xTaskHandle;
  std::vector<std::string> parse_command(std::string *command);
  void exec_command(std::vector<std::string> &args_list);
  static inline void delay_ms(uint32_t ms);
  static inline void delay_us(uint32_t us);
};

extern "C" {

static xSemaphoreHandle main_task_semaphore_handle = xSemaphoreCreateBinary();
static xQueueHandle main_task_queue_handle;

void USART2_IRQHandler(main_task *task);

void main_task_code_c_wrapper(void *pvParams);
}

#endif /* MAIN_TASK_HPP */
