#include "fpga_driver.hpp"

fpga_driver::fpga_driver()
{
  fpga_driver::fpga_reset();
  fpga_driver::fpga_chip_deselect();
}

void fpga_driver::fpga_start()
{
  GPIO_SetBits( FPGA_GPIO_PORT, FPGA_GPIO_START_PIN );
}

void fpga_driver::fpga_stop()
{
  GPIO_ResetBits( FPGA_GPIO_PORT, FPGA_GPIO_START_PIN );
}

void fpga_driver::fpga_reset()
{
  GPIO_SetBits( FPGA_GPIO_PORT, FPGA_GPIO_RST_PIN );
  fpga_driver::fpga_tick();
  GPIO_ResetBits( FPGA_GPIO_PORT, FPGA_GPIO_RST_PIN );
}

void fpga_driver::fpga_tick()
{
  GPIO_SetBits( FPGA_GPIO_PORT, FPGA_GPIO_SCK_PIN );
  GPIO_ResetBits( FPGA_GPIO_PORT, FPGA_GPIO_SCK_PIN );
}

void fpga_driver::fpga_send_low()
{
  GPIO_ResetBits( FPGA_GPIO_PORT, FPGA_GPIO_MOSI_PIN );
  fpga_driver::fpga_tick();
  GPIO_ResetBits( FPGA_GPIO_PORT, FPGA_GPIO_MOSI_PIN );
}

void fpga_driver::fpga_send_high()
{
  GPIO_SetBits( FPGA_GPIO_PORT, FPGA_GPIO_MOSI_PIN );
  fpga_driver::fpga_tick();
  GPIO_ResetBits( FPGA_GPIO_PORT, FPGA_GPIO_MOSI_PIN );
}

void fpga_driver::fpga_send_stream( std::bitset< RELAYS_COUNT > & fpga_stream )
{
  for( unsigned int i = 0; i < RELAYS_COUNT; i++ ){

	fpga_driver::fpga_chip_select();
	( fpga_stream[ i ] ) ? fpga_driver::fpga_send_high() : fpga_driver::fpga_send_low();
	fpga_driver::fpga_chip_deselect();
	
  }
}

void fpga_driver::fpga_chip_select()
{
  GPIO_ResetBits( FPGA_GPIO_PORT, FPGA_GPIO_NSS_PIN );
}

void fpga_driver::fpga_chip_deselect()
{
  GPIO_SetBits( FPGA_GPIO_PORT, FPGA_GPIO_NSS_PIN );
}

fpga_driver::~fpga_driver()
{
  asm( "nop" );
}
