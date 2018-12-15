#ifndef FPGA_DRIVER_HPP
#define FPGA_DRIVER_HPP

#include "bsp_macro.hpp"

class fpga_driver
{
public:

  fpga_driver();
  virtual ~fpga_driver();

  /* FPGA Driver functions */  
  static void fpga_send_stream( std::bitset< RELAYS_COUNT > & fpga_stream );
  static void fpga_reset();
  static void fpga_start();
  static void fpga_stop();
  
private:
  
  static void fpga_chip_select();
  static void fpga_chip_deselect();
  static void fpga_send_low();
  static void fpga_send_high();
  static void fpga_tick();
  
protected:
  
};

#endif /* FPGA_DRIVER_HPP */
