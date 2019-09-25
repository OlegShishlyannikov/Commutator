#include "main_task.hpp"

#define DEBUG

main_task::main_task() {
  xTaskCreate(&main_task_code_c_wrapper, "main_task", configMINIMAL_STACK_SIZE, NULL, MAIN_TASK_PRIORITY,
              this->main_task_handle); /* Create main_task */
}

void main_task::main_task_code(void *pvParams)

{
#define MAIN_TASK_QUEUE_LENGTH 1024
  char char_from_queue;
  std::string *command_str = new std::string;
  command_str->resize(MAIN_TASK_QUEUE_LENGTH / 4);
  command_str->clear();

  main_task_queue_handle = xQueueCreate(MAIN_TASK_QUEUE_LENGTH, sizeof(char));

#ifdef DEBUG
  if (main_task_queue_handle == NULL)
    std::printf("[!] Main task Queue creation error!\r\n");
  else
    std::printf("[+] Main task Queue created!\r\n");
  std::cout << "Hello World!" << std::endl;
#endif /*DEBUG*/

  vSemaphoreCreateBinary(main_task_semaphore_handle);

#ifdef DEBUG
  if (main_task_semaphore_handle == NULL)
    std::printf("[!] Main task Semaphore creation error!\r\n");
  else
    std::printf("[+] Main task Semaphore created!\r\n");
#endif /*DEBUG*/

  pc_usart_driver::send_format_string("kvarta4_commutator#~ ");

  while (true) {

    xQueueReceive(main_task_queue_handle, &char_from_queue, portMAX_DELAY);
    if (char_from_queue == '\r') {

      std::vector<std::string> parsed_command = this->parse_command(command_str);
      this->exec_command(parsed_command);
      command_str->clear();
      pc_usart_driver::send_format_string("kvarta4_commutator#~ ");

    } else {

#define BACKSPACE_CODE 0x7f

      if (char_from_queue == BACKSPACE_CODE) {

        asm("nop");

      } else {

        std::strncat((char *)&*command_str->c_str(), &char_from_queue, 1);
        pc_usart_driver::send_format_string("%c", char_from_queue);
      }
    }
  }

  delete command_str;
  vTaskDelete(NULL);
}

std::vector<std::string> main_task::parse_command(std::string *command) {
  std::vector<std::string> args_list;
  std::stringstream command_stream(command->data());
  args_list.reserve(64);
  args_list.clear();

  for (std::string str; command_stream >> str;)
    args_list.push_back(str);

#ifdef DEBUG
  std::printf("\r\n[!] Command : %s, Number of strings : %u\r\n", command->c_str(), (unsigned int)args_list.size());
  pc_usart_driver::send_format_string("\r\n[!] Command : %s, Number of strings : %u\r\n", command->c_str(),
                                      (unsigned int)args_list.size());
#endif /* DEBUG */
  return args_list;
}

void main_task::exec_command(std::vector<std::string> &args_list) {
  /* Erase command */
  if (args_list[0] == "#erase") {

#ifdef DEBUG
    std::printf("\r\n[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? std::printf("%s, ", args_list[i].data())
                                   : std::printf("%s\r\n", args_list[i].data());
    pc_usart_driver::send_format_string("\r\n[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? pc_usart_driver::send_format_string("%s, ", args_list[i].data())
                                   : pc_usart_driver::send_format_string("%s\r\n", args_list[i].data());
#endif /* DEBUG */

    /* Erase full */
    if (args_list[1] == "all") {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

#ifdef DEBUG
      std::printf("[!] Erasing SPI chip ...\r\n");
      pc_usart_driver::send_format_string("[!] Erasing SPI chip ...\r\n");
#endif /* DEBUG */

      spi_flash_driver::spi_flash_erase_bulk();

#ifdef DEBUG
      pc_usart_driver::send_format_string("[+] OK!\r\n");
#endif /* DEBUG */

      /* Erase sector */
    } else if (args_list[1] == "sector") {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

      unsigned int sector_addr;
      std::stringstream *sector_addr_stream = new std::stringstream;
      *sector_addr_stream << std::hex << args_list[2];
      *sector_addr_stream >> sector_addr;
#ifdef DEBUG
      std::printf("[!] Erasing SPI chip sector addr : %#x!\r\n", sector_addr);
      pc_usart_driver::send_format_string("[!] Erasing SPI chip sector addr : %#x!\r\n", sector_addr);
#endif /* DEBUG */

      spi_flash_driver::spi_flash_erase_sector(sector_addr);

#ifdef DEBUG
      pc_usart_driver::send_format_string("[+] OK!\r\n");
#endif /* DEBUG */

      delete sector_addr_stream;

    } else {

      std::printf("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
#ifdef DEBUG
      pc_usart_driver::send_format_string("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
#endif /* DEBUG */
    }

  } else if (args_list[0] == "#read") {

#ifdef DEBUG
    std::printf("\r\n[+] Command \"%s\" parsed! Args :", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? std::printf("%s, ", args_list[i].data())
                                   : std::printf("%s\r\n", args_list[i].data());
    pc_usart_driver::send_format_string("\r\n[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? pc_usart_driver::send_format_string("%s, ", args_list[i].data())
                                   : pc_usart_driver::send_format_string("%s\r\n", args_list[i].data());
#endif /* DEBUG */

    /* Read pages from range */
    if ((args_list[1] == "pages") && (args_list[3] == "to")) {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

      /* Start page */
      unsigned int start_page;
      std::stringstream *start_page_stream = new std::stringstream;
      *start_page_stream << std::dec << args_list[2];
      *start_page_stream >> start_page;
      delete start_page_stream;

      /* Stop page */
      unsigned int stop_page;
      std::stringstream *stop_page_stream = new std::stringstream;
      *stop_page_stream << std::dec << args_list[4];
      *stop_page_stream >> stop_page;
      delete stop_page_stream;

#ifdef DEBUG
      std::printf("[!] Start reading flash from %u page to %u ...\r\n", start_page, stop_page);
      pc_usart_driver::send_format_string("[!] Start reading flash from %u page to %u ...\r\n", start_page, stop_page);
#endif /* DEBUG */

      for (unsigned int i = start_page; i <= stop_page; i++) {

        std::string *page_buffer = new std::string;
        page_buffer->resize(W25X_SPI_PAGESIZE);
        spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*page_buffer->c_str(),
                                                SPI_FLASH_ORIGIN_ADDR + i * W25X_SPI_PAGESIZE, W25X_SPI_PAGESIZE);
        pc_usart_driver::send_format_string("[+] Flash data at page #%u, addr %#x : %s\r\n", i,
                                            SPI_FLASH_ORIGIN_ADDR + i * W25X_SPI_PAGESIZE, page_buffer->c_str());
        delete page_buffer;
      }

    } else if (args_list[1] == "page") {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

      unsigned int page_num;
      std::stringstream *page_num_stream = new std::stringstream;
      *page_num_stream << std::dec << args_list[2];
      *page_num_stream >> page_num;
      delete page_num_stream;

#ifdef DEBUG
      std::printf("[!] Start reading flash page #%u ...\r\n", page_num);
      pc_usart_driver::send_format_string("[!] Start reading flash page #%u ...\r\n", page_num);
#endif /* DEBUG */

      std::string *page_buffer = new std::string;
      page_buffer->resize(W25X_SPI_PAGESIZE);
      spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*page_buffer->c_str(),
                                              SPI_FLASH_ORIGIN_ADDR + page_num * W25X_SPI_PAGESIZE, W25X_SPI_PAGESIZE);
      pc_usart_driver::send_format_string("[+] Flash data at page #%u, addr %#x : %s\r\n", page_num,
                                          SPI_FLASH_ORIGIN_ADDR + page_num * W25X_SPI_PAGESIZE, page_buffer->c_str());
      delete page_buffer;
      pc_usart_driver::send_format_string("[+] OK!\r\n");

    } else if (args_list[1] == "test") {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

      unsigned int test_num;
      std::stringstream *test_num_stream = new std::stringstream;
      *test_num_stream << std::dec << args_list[2];
      *test_num_stream >> test_num;
      delete test_num_stream;

#ifdef DEBUG
      std::printf("[!] Start reading test #%u data ...\r\n", test_num);
      pc_usart_driver::send_format_string("[!] Start reading test #%u data ...\r\n", test_num);
#endif /* DEBUG */

      std::string *page_buffer = new std::string;
      page_buffer->resize(RELAYS_COUNT);
      spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*page_buffer->c_str(),
                                              SPI_FLASH_FILE_ADDR + test_num * W25X_SPI_PAGESIZE, RELAYS_COUNT);
      pc_usart_driver::send_format_string("[+] Test #%u data at addr %#x : %s\r\n", test_num,
                                          SPI_FLASH_FILE_ADDR + test_num * W25X_SPI_PAGESIZE, page_buffer->c_str());
      delete page_buffer;
      pc_usart_driver::send_format_string("[+] OK!\r\n");

    } else if ((args_list[1] == "tests") && (args_list[3] == "to")) {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

      /* Start page */
      unsigned int start_test;
      std::stringstream *start_test_stream = new std::stringstream;
      *start_test_stream << std::dec << args_list[2];
      *start_test_stream >> start_test;
      delete start_test_stream;

      /* Stop page */
      unsigned int stop_test;
      std::stringstream *stop_test_stream = new std::stringstream;
      *stop_test_stream << std::dec << args_list[4];
      *stop_test_stream >> stop_test;
      delete stop_test_stream;

#ifdef DEBUG
      std::printf("[!] Start reading tests from %u to %u ...\r\n", start_test, stop_test);
      pc_usart_driver::send_format_string("[!] Start reading tests from %u to %u ...\r\n", start_test, stop_test);
#endif /* DEBUG */

      for (unsigned int i = start_test; i <= stop_test; i++) {

        std::string *page_buffer = new std::string;
        page_buffer->resize(RELAYS_COUNT);
        spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*page_buffer->c_str(),
                                                SPI_FLASH_FILE_ADDR + i * W25X_SPI_PAGESIZE, RELAYS_COUNT);
        pc_usart_driver::send_format_string("[+] Test #%u data at addr %#x : %s\r\n", i,
                                            SPI_FLASH_FILE_ADDR + i * W25X_SPI_PAGESIZE, page_buffer->c_str());
        delete page_buffer;
      }

    } else {

#ifdef DEBUG
      std::printf("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
      pc_usart_driver::send_format_string("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
#endif /* DEBUG */
    }

  } else if (args_list[0] == "#write") {

#ifdef DEBUG
    std::printf("\r\n[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? std::printf("%s, ", args_list[i].data())
                                   : std::printf("%s\r\n", args_list[i].data());
    pc_usart_driver::send_format_string("\r\n[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? pc_usart_driver::send_format_string("%s, ", args_list[i].data())
                                   : pc_usart_driver::send_format_string("%s\r\n", args_list[i].data());
#endif /* DEBUG */

    /* Write page */
    if (args_list[1] == "page") {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

      unsigned int page_num;
      std::stringstream *page_num_stream = new std::stringstream;
      *page_num_stream << std::dec << args_list[2];
      *page_num_stream >> page_num;
      delete page_num_stream;

#ifdef DEBUG
      std::printf("[!] Start writing flash page #%u : %s\r\n", page_num, args_list[3].c_str());
      pc_usart_driver::send_format_string("[!] Start writing flash page #%u : %s\r\n", page_num, args_list[3].c_str());
#endif /* DEBUG */

      std::string *page_buffer = new std::string;
      page_buffer->resize(W25X_SPI_PAGESIZE);
      spi_flash_driver::spi_flash_write_page((uint8_t *)&*args_list[3].c_str(),
                                             SPI_FLASH_ORIGIN_ADDR + page_num * W25X_SPI_PAGESIZE,
                                             args_list[3].length());
      spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*page_buffer->c_str(),
                                              SPI_FLASH_ORIGIN_ADDR + page_num * W25X_SPI_PAGESIZE, W25X_SPI_PAGESIZE);

#ifdef DEBUG
      pc_usart_driver::send_format_string("[+] Flash data at page #%u, addr %#x : %s\r\n", page_num,
                                          SPI_FLASH_ORIGIN_ADDR + page_num * W25X_SPI_PAGESIZE, page_buffer->c_str());
#endif /* DEBUG */

      pc_usart_driver::send_format_string("[+] OK!\r\n");
      delete page_buffer;

    } else if (args_list[1] == "test") {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

      unsigned int test_num;
      std::stringstream *test_num_stream = new std::stringstream;
      *test_num_stream << std::dec << args_list[2];
      *test_num_stream >> test_num;
      delete test_num_stream;

      /* Check if sector erased */
      uint8_t *buffer = new uint8_t;
      spi_flash_driver::spi_flash_read_buffer(buffer, SPI_FLASH_FILE_ADDR + test_num * W25X_SPI_PAGESIZE, 1);

      if (*buffer != 0xff) {

#ifdef DEBUG
        std::printf("[!] Erasing sector at address : %#x\r\n",
                    SPI_FLASH_FILE_ADDR + W25X_SPI_SECTOR_SIZE * (test_num * W25X_SPI_PAGESIZE) / W25X_SPI_SECTOR_SIZE);
        pc_usart_driver::send_format_string(
            "[!] Erasing sector at address : %#x\r\n",
            SPI_FLASH_FILE_ADDR + W25X_SPI_SECTOR_SIZE * (test_num * W25X_SPI_PAGESIZE) / W25X_SPI_SECTOR_SIZE);
#endif /* DEBUG */

        spi_flash_driver::spi_flash_erase_sector(
            SPI_FLASH_FILE_ADDR + W25X_SPI_SECTOR_SIZE * (test_num * W25X_SPI_PAGESIZE) / W25X_SPI_SECTOR_SIZE);

      } else {

#ifdef DEBUG
        std::printf("[+] Sector at address %#x already erased.\r\n",
                    SPI_FLASH_FILE_ADDR + W25X_SPI_SECTOR_SIZE * (test_num * W25X_SPI_PAGESIZE) / W25X_SPI_SECTOR_SIZE);
        pc_usart_driver::send_format_string(
            "[+] Sector at address %#x already erased.\r\n",
            SPI_FLASH_FILE_ADDR + W25X_SPI_SECTOR_SIZE * (test_num * W25X_SPI_PAGESIZE) / W25X_SPI_SECTOR_SIZE);
#endif /* DEBUG */
      }

      delete buffer;

#ifdef DEBUG
      std::printf("[!] Start writing test #%u : %s\r\n", test_num, args_list[3].c_str());
      pc_usart_driver::send_format_string("[!] Start writing test #%u : %s\r\n", test_num, args_list[3].c_str());
#endif /* DEBUG */

      std::string *page_buffer = new std::string;
      page_buffer->resize(RELAYS_COUNT);
      spi_flash_driver::spi_flash_write_page((uint8_t *)&*args_list[3].c_str(),
                                             SPI_FLASH_FILE_ADDR + test_num * W25X_SPI_PAGESIZE, args_list[3].length());
      spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*page_buffer->c_str(),
                                              SPI_FLASH_FILE_ADDR + test_num * W25X_SPI_PAGESIZE, RELAYS_COUNT);
#ifdef DEBUG
      pc_usart_driver::send_format_string("[+] Flash data at test #%u, addr %#x : %s\r\n", test_num,
                                          SPI_FLASH_FILE_ADDR + test_num * W25X_SPI_PAGESIZE, page_buffer->c_str());
#endif /* DEBUG */

      pc_usart_driver::send_format_string("[+] OK!\r\n");
      delete page_buffer;

    } else {

#ifdef DEBUG
      std::printf("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
      pc_usart_driver::send_format_string("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
#endif /* DEBUG */
    }

  } else if (args_list[0] == "#check") {

#ifdef DEBUG
    std::printf("\r\n[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? std::printf("%s, ", args_list[i].data())
                                   : std::printf("%s\r\n", args_list[i].data());
    pc_usart_driver::send_format_string("\r\n[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? pc_usart_driver::send_format_string("%s, ", args_list[i].data())
                                   : pc_usart_driver::send_format_string("%s\r\n", args_list[i].data());
#endif /* DEBUG */

    /* Test fpga */
    if (args_list[1] == "fpga") {

#ifdef DEBUG
      std::printf("[!] Start FPGA test ... \r\n");
      pc_usart_driver::send_format_string("[!] Start FPGA test ... \r\n");
#endif /* DEBUG */

      this->fpga_initial_test();
      pc_usart_driver::send_format_string("[+] OK!\r\n");

    } else if (args_list[1] == "flash") {

#ifdef DEBUG
      std::printf("[!] Start FLASH test ... \r\n");
      pc_usart_driver::send_format_string("[!] Start FLASH test ... \r\n");
#endif /* DEBUG */
      this->flash_test();
      pc_usart_driver::send_format_string("[+] OK!\r\n");

    } else if (args_list[1] == "global") {

#ifdef DEBUG
      std::printf("[!] Start Global test ... \r\n");
      pc_usart_driver::send_format_string("[!] Start Global test ... \r\n");
#endif /* DEBUG */
      this->global_test();
      pc_usart_driver::send_format_string("[+] OK!\r\n");

    } else {

#ifdef DEBUG
      std::printf("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
      pc_usart_driver::send_format_string("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
#endif /* DEBUG */
    }
  } else if (args_list[0] == "#kvarta") {

#ifdef DEBUG
    std::printf("[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? std::printf("%s, ", args_list[i].data())
                                   : std::printf("%s\r\n", args_list[i].data());
    pc_usart_driver::send_format_string("[+] Command \"%s\" parsed! Args : ", args_list[0].c_str());
    for (unsigned int i = 0; i < args_list.size(); i++)
      if (i != 0)
        (i < args_list.size() - 1) ? pc_usart_driver::send_format_string("%s, ", args_list[i].data())
                                   : pc_usart_driver::send_format_string("%s\r\n", args_list[i].data());
#endif /* DEBUG */

    /* Test start */
    if (args_list[1] == "load_test") {

      /* Disable write protection */
      spi_flash_driver::spi_flash_wp_disable();
      spi_flash_driver::spi_flash_chip_deselect();

      /* Reset flash memory */
      spi_flash_driver::spi_flash_hold_disable();
      spi_flash_driver::spi_flash_hold_enable();
      spi_flash_driver::spi_flash_hold_disable();

      /* Lock /WP and /HOLD pins */
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
      GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

      unsigned int test_num;
      std::stringstream *test_num_stream = new std::stringstream;
      *test_num_stream << std::dec << args_list[2];
      *test_num_stream >> test_num;
      delete test_num_stream;

#ifdef DEBUG
      std::printf("[!] Loading test #%u\r\n", test_num);
      pc_usart_driver::send_format_string("[!] Loading test #%u\r\n", test_num);
#endif /* DEBUG */

      std::string *page_buffer = new std::string;
      page_buffer->resize(RELAYS_COUNT);
      spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*page_buffer->c_str(),
                                              SPI_FLASH_FILE_ADDR + test_num * W25X_SPI_PAGESIZE, RELAYS_COUNT);
      std::bitset<RELAYS_COUNT> *fpga_stream;
      ((page_buffer->c_str()[0] == '0') || (page_buffer->c_str()[0] == '1'))
          ? fpga_stream = new std::bitset<RELAYS_COUNT>(page_buffer->c_str())
          : fpga_stream = new std::bitset<RELAYS_COUNT>(0b0);

#ifdef DEBUG
      pc_usart_driver::send_format_string("[+] Test #%u data : %s\r\n", test_num, fpga_stream->to_string().c_str());
#endif /* DEBUG */
      fpga_driver::fpga_reset();
      fpga_driver::fpga_send_stream(*fpga_stream);
      delete page_buffer;
      delete fpga_stream;
      pc_usart_driver::send_format_string("[+] OK!\r\n");

    } else if (args_list[1] == "start") {

#ifdef DEBUG
      std::printf("[!] Start test!\r\n");
      pc_usart_driver::send_format_string("[!] Start test!\r\n");
#endif /* DEBUG */
      fpga_driver::fpga_start();

    } else if (args_list[1] == "stop") {

#ifdef DEBUG
      std::printf("[!] Stop test!\r\n");
      pc_usart_driver::send_format_string("[!] Stop test!\r\n");
#endif /* DEBUG */
      fpga_driver::fpga_stop();

    } else {

#ifdef DEBUG
      std::printf("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
      pc_usart_driver::send_format_string("[!] Unknown parameter : %s\r\n", args_list[1].c_str());
#endif /* DEBUG */
    }

  } else {

#ifdef DEBUG
    std::printf("\r\n[!] Unknown command : %s\r\n", args_list[0].c_str());
    pc_usart_driver::send_format_string("\r\n[!] Unknown command : %s\r\n", args_list[0].c_str());
#endif /* DEBUG */
  }
  args_list.clear();
}

void main_task::pc_usart_callback(USART_TypeDef *USARTx) {
  if (USARTx == PC_USART_PORT) {

    if (USART_GetITStatus(USARTx, PC_USART_IT_VECTOR_TYPE)) {

      char buf;
      portBASE_TYPE usart_irq_main_task_woken;
      buf = USART_ReceiveData(USARTx);
      xQueueSendToBackFromISR(main_task_queue_handle, (void *)&buf, &usart_irq_main_task_woken);
      if (usart_irq_main_task_woken == pdTRUE)
        taskYIELD();
    }
  }
}

bool main_task::flash_test() {
  /* Test 1 */
  debug_leds_driver::write_led1(ENABLE);
  std::printf("Start SPI_FLASH test ...\r\n");

  /* Disable write protection */
  spi_flash_driver::spi_flash_wp_disable();
  spi_flash_driver::spi_flash_chip_deselect();

  /* Reset flash memory */
  spi_flash_driver::spi_flash_hold_disable();
  spi_flash_driver::spi_flash_hold_enable();
  spi_flash_driver::spi_flash_hold_disable();

  /* Lock /WP and /HOLD pins */
  GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
  GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

#ifdef DEBUG
  std::printf("[!] SPI chip manufacturer device ID : %#x\r\n", spi_flash_driver::spi_flash_read_id());
  std::printf("[!] SPI chip jedec device ID : %#x\r\n", spi_flash_driver::spi_flash_read_jedec_id());
  std::printf("[!] SPI chip unique device ID : %#x\r\n", spi_flash_driver::spi_flash_read_unique_id());
  pc_usart_driver::send_format_string("[!] SPI chip manufacturer device ID : %#x\r\n",
                                      spi_flash_driver::spi_flash_read_id());
  pc_usart_driver::send_format_string("[!] SPI chip jedec device ID : %#x\r\n",
                                      spi_flash_driver::spi_flash_read_jedec_id());
  pc_usart_driver::send_format_string("[!] SPI chip unique device ID : %#x\r\n",
                                      spi_flash_driver::spi_flash_read_unique_id());
#endif /* DEBUG */

  spi_flash_driver::spi_flash_erase_sector(SPI_FLASH_ORIGIN_ADDR);
  std::string *write_buffer = new std::string("FLASH TEST!");

#ifdef DEBUG
  std::printf("[!] Test flash: write \"%s\"\r\n", write_buffer->c_str());
  pc_usart_driver::send_format_string("[!] Test flash: write \"%s\"\r\n", write_buffer->c_str());
#endif /* DEBUG */

  spi_flash_driver::spi_flash_write_page((uint8_t *)&*write_buffer->c_str(), SPI_FLASH_ORIGIN_ADDR,
                                         std::strlen("FLASH TEST!"));

#ifdef DEBUG
  std::printf("[!] Test flash: read from address %#x : ", SPI_FLASH_ORIGIN_ADDR);
  pc_usart_driver::send_format_string("[!] Test flash: read from address %#x : ", SPI_FLASH_ORIGIN_ADDR);
#endif /* DEBUG */

  std::string *read_buffer = new std::string;
  read_buffer->resize(std::strlen(write_buffer->c_str()));
  spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*read_buffer->c_str(), SPI_FLASH_ORIGIN_ADDR,
                                          std::strlen("FLASH TEST!"));

#ifdef DEBUG
  std::printf("%s\r\n", read_buffer->c_str());
  pc_usart_driver::send_format_string("%s\r\n", read_buffer->c_str());
#endif /* DEBUG */

  if (std::strcmp(write_buffer->c_str(), read_buffer->c_str()) == 0) {

    delete read_buffer;
    delete write_buffer;
    debug_leds_driver::write_led1(DISABLE);
    return true;

  } else {

    delete read_buffer;
    delete write_buffer;
    debug_leds_driver::write_led1(DISABLE);
    return false;
  }
}

bool main_task::fpga_test() {
  /* Test 2 */
  debug_leds_driver::write_led2(ENABLE);

#ifdef DEBUG
  std::printf("Start FPGA-RELAYS test ...\r\n");
#endif /* DEBUG */

  std::bitset<RELAYS_COUNT> *fpga_write_stream = new std::bitset<RELAYS_COUNT>(0b1);

  for (unsigned int i = 0; i < RELAYS_COUNT; i++) {

    fpga_driver::fpga_reset();
    fpga_driver::fpga_send_stream(*fpga_write_stream);
    fpga_driver::fpga_start();

#ifdef DEBUG
    (i < 10 - 1) ? std::printf("[!] Relay test #%u :  %s\r\n", i + 1, fpga_write_stream->to_string().c_str())
                 : std::printf("[!] Relay test #%u : %s\r\n", i + 1, fpga_write_stream->to_string().c_str());
    std::printf("[!] Press ENTER key to continue ...");
#endif /* DEBUG */

    *fpga_write_stream <<= 1;
    char buf;
    std::scanf("%c", &buf);
    fpga_driver::fpga_stop();
  }

  delete fpga_write_stream;
  debug_leds_driver::write_led2(DISABLE);
  return true;
}

bool main_task::global_test() {
  /* Test 3 */
  debug_leds_driver::write_led3(ENABLE);

#ifdef DEBUG
  std::printf("Start SPI_FLASH-FPGA test ...\r\n");
  pc_usart_driver::send_format_string("Start SPI_FLASH-FPGA test ...\r\n");
#endif /* DEBUG */

  std::bitset<RELAYS_COUNT> *fpga_write_stream = new std::bitset<RELAYS_COUNT>(0b1);

  /* Disable write protection */
  spi_flash_driver::spi_flash_wp_disable();
  spi_flash_driver::spi_flash_chip_deselect();

  /* Reset flash memory */
  spi_flash_driver::spi_flash_hold_disable();
  spi_flash_driver::spi_flash_hold_enable();
  spi_flash_driver::spi_flash_hold_disable();

  /* Lock /WP and /HOLD pins */
  GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
  GPIO_PinLockConfig(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN);

#ifdef DEBUG
  std::printf("[!] SPI chip manufacturer device ID : %#x\r\n", spi_flash_driver::spi_flash_read_id());
  std::printf("[!] SPI chip jedec device ID : %#x\r\n", spi_flash_driver::spi_flash_read_jedec_id());
  std::printf("[!] SPI chip unique device ID : %#x\r\n", spi_flash_driver::spi_flash_read_unique_id());
  std::printf("[!] Erase sectors of flash memory from %#x to %#x ...\r\n", SPI_FLASH_ORIGIN_ADDR,
              SPI_FLASH_ORIGIN_ADDR +
                  W25X_SPI_SECTOR_SIZE * (RELAYS_COUNT / (W25X_SPI_SECTOR_SIZE / W25X_SPI_PAGESIZE)));
  pc_usart_driver::send_format_string("[!] SPI chip manufacturer device ID : %#x\r\n",
                                      spi_flash_driver::spi_flash_read_id());
  pc_usart_driver::send_format_string("[!] SPI chip jedec device ID : %#x\r\n",
                                      spi_flash_driver::spi_flash_read_jedec_id());
  pc_usart_driver::send_format_string("[!] SPI chip unique device ID : %#x\r\n",
                                      spi_flash_driver::spi_flash_read_unique_id());
  pc_usart_driver::send_format_string(
      "[!] Erase sectors of flash memory from %#x to %#x ...\r\n", SPI_FLASH_ORIGIN_ADDR,
      SPI_FLASH_ORIGIN_ADDR + W25X_SPI_SECTOR_SIZE * (RELAYS_COUNT / (W25X_SPI_SECTOR_SIZE / W25X_SPI_PAGESIZE)));
#endif /* DEBUG */

  for (unsigned int i = 0; i < RELAYS_COUNT / (W25X_SPI_SECTOR_SIZE / W25X_SPI_PAGESIZE); i++)
    spi_flash_driver::spi_flash_erase_sector(SPI_FLASH_ORIGIN_ADDR + W25X_SPI_SECTOR_SIZE * i);

  for (unsigned int i = 0; i < RELAYS_COUNT; i++) {

#ifdef DEBUG
    std::printf("[!] Write fpga bitstream to flash memory ( Page #%u, addr = %#x ) : %s\r\n", i + 1,
                SPI_FLASH_ORIGIN_ADDR + i * W25X_SPI_PAGESIZE, fpga_write_stream->to_string().c_str());
    pc_usart_driver::send_format_string("[!] Write fpga bitstream to flash memory ( Page #%u, addr = %#x ) : %s\r\n",
                                        i + 1, SPI_FLASH_ORIGIN_ADDR + i * W25X_SPI_PAGESIZE,
                                        fpga_write_stream->to_string().c_str());
#endif /* DEBUG */

    spi_flash_driver::spi_flash_write_page((uint8_t *)fpga_write_stream->to_string().c_str(),
                                           SPI_FLASH_ORIGIN_ADDR + i * W25X_SPI_PAGESIZE,
                                           fpga_write_stream->to_string().length());
    std::string *read_buffer = new std::string;
    read_buffer->resize(std::strlen(fpga_write_stream->to_string().c_str()));
    spi_flash_driver::spi_flash_read_buffer((uint8_t *)&*read_buffer->c_str(),
                                            SPI_FLASH_ORIGIN_ADDR + i * W25X_SPI_PAGESIZE,
                                            fpga_write_stream->to_string().length());
    std::bitset<RELAYS_COUNT> *fpga_read_stream = new std::bitset<RELAYS_COUNT>(*read_buffer);
#ifdef DEBUG
    std::printf("[!] Reading data from flash ( Page #%u, addr = %#x ) : %s\r\n", i + 1,
                SPI_FLASH_ORIGIN_ADDR + i * W25X_SPI_PAGESIZE, fpga_read_stream->to_string().c_str());
    pc_usart_driver::send_format_string("[!] Reading data from flash ( Page #%u, addr = %#x ) : %s\r\n", i + 1,
                                        SPI_FLASH_ORIGIN_ADDR + i * W25X_SPI_PAGESIZE,
                                        fpga_read_stream->to_string().c_str());
#endif /* DEBUG */

    fpga_driver::fpga_reset();
    fpga_driver::fpga_send_stream(*fpga_read_stream);
    fpga_driver::fpga_start();

    if (std::strcmp(fpga_write_stream->to_string().c_str(), read_buffer->c_str()) != 0)
      return false;

    delete read_buffer;
    delete fpga_read_stream;
    *fpga_write_stream <<= 1;
  }

  fpga_driver::fpga_stop();
  delete fpga_write_stream;
  debug_leds_driver::write_led3(DISABLE);
  return true;
}

inline void main_task::delay_us(uint32_t us) {
  uint32_t ticks = 0;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  ticks = (RCC_Clocks.SYSCLK_Frequency / 10000000) * us;
  while (ticks--)
    ;
}

inline void main_task::delay_ms(uint32_t ms) {
  uint32_t ticks = 0;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  ticks = (RCC_Clocks.SYSCLK_Frequency / 10000) * ms;
  while (ticks--)
    ;
}

bool main_task::fpga_initial_test() {
  /* Test 4 */
  debug_leds_driver::write_led4(ENABLE);

#ifdef DEBUG
  std::printf("Start FPGA-RELAYS initial test ...\r\n ");
#endif /* DEBUG */

  std::bitset<RELAYS_COUNT> *fpga_write_stream = new std::bitset<RELAYS_COUNT>(0b1);

  for (unsigned int i = 0; i < RELAYS_COUNT; i++) {

    fpga_driver::fpga_reset();
    fpga_driver::fpga_send_stream(*fpga_write_stream);
    fpga_driver::fpga_start();

#ifdef RELEASE
    (i < 10 - 1) ? std::printf("[!] Relay test #%u :  %s\r\n", i + 1, fpga_write_stream->to_string().c_str())
                 : std::printf("[!] Relay test #%u : %s\r\n", i + 1, fpga_write_stream->to_string().c_str());
#endif /* RELEASE */

    *fpga_write_stream <<= 1;
    fpga_driver::fpga_stop();
  }

  delete fpga_write_stream;
  debug_leds_driver::write_led4(DISABLE);
  return true;
}

main_task::~main_task() { asm("nop"); }

void main_task_code_c_wrapper(void *pvParams) { (reinterpret_cast<main_task *>(pvParams))->main_task_code(pvParams); }
void USART2_IRQHandler(main_task *task) { task->pc_usart_callback(PC_USART_PORT); }
