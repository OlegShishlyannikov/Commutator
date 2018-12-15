#include "debug_leds_driver.hpp"

debug_leds_driver::debug_leds_driver()
{
  asm( "nop" );
}

void debug_leds_driver::write_led1( FunctionalState NewState )
{
  GPIO_WriteBit( DEBUG_LEDS_GPIO_PORT, DEBUG_LED1_GPIO_PIN, ( NewState ) ? Bit_SET : Bit_RESET );
}

void debug_leds_driver::write_led2( FunctionalState NewState )
{
  GPIO_WriteBit( DEBUG_LEDS_GPIO_PORT, DEBUG_LED2_GPIO_PIN, ( NewState ) ? Bit_SET : Bit_RESET );
}

void debug_leds_driver::write_led3( FunctionalState NewState )
{
  GPIO_WriteBit( DEBUG_LEDS_GPIO_PORT, DEBUG_LED3_GPIO_PIN, ( NewState ) ? Bit_SET : Bit_RESET );
}

void debug_leds_driver::write_led4( FunctionalState NewState )
{
  GPIO_WriteBit( DEBUG_LEDS_GPIO_PORT, DEBUG_LED4_GPIO_PIN, ( NewState ) ? Bit_SET : Bit_RESET );
}

bool debug_leds_driver::read_led1()
{
  return GPIO_ReadOutputDataBit( DEBUG_LEDS_GPIO_PORT, DEBUG_LED1_GPIO_PIN );
}

bool debug_leds_driver::read_led2()
{
  return GPIO_ReadOutputDataBit( DEBUG_LEDS_GPIO_PORT, DEBUG_LED2_GPIO_PIN );  
}

bool debug_leds_driver::read_led3()
{
  return GPIO_ReadOutputDataBit( DEBUG_LEDS_GPIO_PORT, DEBUG_LED3_GPIO_PIN );
}

bool debug_leds_driver::read_led4()
{
  return GPIO_ReadOutputDataBit( DEBUG_LEDS_GPIO_PORT, DEBUG_LED4_GPIO_PIN );
}

debug_leds_driver::~debug_leds_driver()
{
  asm( "nop" );
}
