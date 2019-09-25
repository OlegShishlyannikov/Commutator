#ifndef DEBUG_USART_DRIVER_HPP
#define DEBUG_USART_DRIVER_HPP
#include "bsp_macro.hpp"

class debug_usart_driver {
public:
  debug_usart_driver();
  virtual ~debug_usart_driver();

  static void send_char(const char c);
  static void send_string(const char *str);
  static const char read_char();
};

#endif /* DEBUG_USART_DRIVER_HPP */
