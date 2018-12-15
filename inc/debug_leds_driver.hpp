#ifndef DEBUG_LEDS_DRIVER_HPP
#define DEBUG_LEDS_DRIVER_HPP

#include "bsp_macro.hpp"

class debug_leds_driver
{
public:

  debug_leds_driver();
  virtual ~debug_leds_driver();
  
  static void write_led1( FunctionalState NewState );
  static void write_led2( FunctionalState NewState );
  static void write_led3( FunctionalState NewState );
  static void write_led4( FunctionalState NewState );

  static bool read_led1();
  static bool read_led2();
  static bool read_led3();
  static bool read_led4();
};

#endif /* DEBUG_LEDS_DRIVER_HPP */
