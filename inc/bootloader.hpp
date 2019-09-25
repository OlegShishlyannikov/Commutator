#ifndef BOOTLOADER_HPP
#define BOOTLOADER_HPP

#include "bsp_macro.hpp"
#include "rtos_core.hpp"

class bootloader {

public:
  bootloader();            /* Bootloader constuctor */
  ~bootloader();           /* Bootloader desctructor */
  void init_hardware();    /* Hardware preinit function */
  void setup_interrupts(); /* Setup NVIC */
  void load_rtos();        /* Load RTOS core */
  void start_rtos();       /* Start RTOS */

  /* Contacts */
  const char *get_hardware_name();
  const char *get_author_name();
  const char *get_author_mail();
  const char *get_company_name();
  const char *get_company_address();
  const char *get_hardware_version();
  const char *get_software_version();

private:
  rtos_core *os_core_ptr;

  std::string *hardware_name_ptr;
  std::string *author_name_ptr;
  std::string *author_mail_ptr;
  std::string *company_name_ptr;
  std::string *company_address_ptr;
  std::string *hardware_version_ptr;
  std::string *software_version_ptr;

  uint32_t adcclk_freq = 0;
  uint32_t sysclk_freq = 0;
  uint32_t hclk_freq = 0;
  uint32_t pclk1_freq = 0;
  uint32_t pclk2_freq = 0;

protected:
};

#endif /* BOOTLOADER_HPP */
