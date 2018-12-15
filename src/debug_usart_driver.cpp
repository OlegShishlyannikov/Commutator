#include "debug_usart_driver.hpp"

debug_usart_driver::debug_usart_driver()
{
  asm( "nop" );
}

void debug_usart_driver::send_char( const char c )
{
  while( !USART_GetFlagStatus( DEBUG_USART_PORT, USART_FLAG_TC ));
  USART_SendData( DEBUG_USART_PORT, c );
}

void debug_usart_driver::send_string( const char * str )
{
  for( int i = 0; i < std::strlen( str ); i++ ){

	debug_usart_driver::send_char( str[ i ]);
	
  }
}

const char debug_usart_driver::read_char()
{
  return USART_ReceiveData( DEBUG_USART_PORT );
}

debug_usart_driver::~debug_usart_driver()
{
  USART_DeInit( DEBUG_USART_PORT );
}
