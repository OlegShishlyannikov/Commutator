#include "bsp_macro.hpp"
#include "bootloader.hpp"

int main( int argc, char * argv[] )
{
  /* Create bootloader */
  bootloader * boot_ptr = new bootloader;
  
  /* Load & prepare RTOS */
  boot_ptr->load_rtos();
  
  /* Start RTOS */
  boot_ptr->start_rtos();
  
  /* Exit on error except */
  return 1;
}
