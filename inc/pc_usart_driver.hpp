#ifndef PC_USART_DRIVER_HPP
#define PC_USART_DRIVER_HPP

#include "bsp_macro.hpp"
#include <cstdarg>

class pc_usart_driver
{
public:

  pc_usart_driver();
  virtual ~pc_usart_driver();
  
  static void send_char( const char c );
  static int send_format_string( const char * format_str, ... );
  static void send_string( const char * str );
  static const char read_char();
  
};

#endif /* PC_USART_DRIVER_HPP */
