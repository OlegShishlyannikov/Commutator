#include "pc_usart_driver.hpp"

pc_usart_driver::pc_usart_driver()
{
  asm( "nop" );
}

void pc_usart_driver::send_char( const char c )
{
  while( !USART_GetFlagStatus( PC_USART_PORT, USART_FLAG_TC ));
  USART_SendData( PC_USART_PORT, c );
}

int pc_usart_driver::send_format_string( const char * format_str, ... )
{
  std::va_list arg;
  int done;
  char * buffer = new char[ 1024 ]; 
  va_start( arg, format_str );
  done = vsprintf( buffer, format_str, arg );
  va_end( arg );
  pc_usart_driver::send_string( buffer );
  delete[] buffer;
  return done;
}

void pc_usart_driver::send_string( const char * str )
{
  for( int i = 0; i < std::strlen( str ); i++ ){

	pc_usart_driver::send_char( str[ i ]);
	
  }
}

const char pc_usart_driver::read_char()
{
  return USART_ReceiveData( PC_USART_PORT );
}

pc_usart_driver::~pc_usart_driver()
{
  USART_DeInit( PC_USART_PORT );
}
