#ifndef SPI_FLASH_DRIVER_HPP
#define SPI_FLASH_DRIVER_HPP

#include "bsp_macro.hpp"

#define W25Q128_DeviceID 0xEF17
 
#define W25X_CMD_WriteEnable 0x06 /* Write enable instruction */
#define W25X_CMD_WriteDisable 0x04  /*! Write to Memory Disable */
#define W25X_CMD_WriteStatusReg 0x01 /* Write Status Register instruction */
 
#define W25X_CMD_PageProgram 0x02 /* Write enable instruction */
#define W25X_CMD_QuadPageProgram 0x32 /* Write enable instruction */
 
#define W25X_CMD_BlockErase64 0xD8 /* Block 64k Erase instruction */
#define W25X_CMD_BlockErase32 0x52 /* Block 32k Erase instruction */
#define W25X_CMD_ChipErase 0xC7 /* Bulk Erase instruction */
#define W25X_CMD_SectorErase 0x20 /* Sector 4k Erase instruction */
#define W25X_CMD_EraseSuspend 0x75 /* Sector 4k Erase instruction */
#define W25X_CMD_EraseResume 0x7a /* Sector 4k Erase instruction */
 
#define W25X_CMD_ReadStatusReg1 0x05 /* Read Status Register instruction */
#define W25X_CMD_ReadStatusReg2 0x35 /* Read Status Register instruction */
 
#define W25X_CMD_High_Perform_Mode 0xa3
#define W25X_CMD_Conti_Read_Mode_Ret 0xff
 
#define W25X_WakeUp 0xAB
#define W25X_JedecDeviceID 0x9F /* Read identification */
#define W25X_ManufactDeviceID 0x90 /* Read identification */
#define W25X_ReadUniqueID 0x4B
 
#define W25X_Power_Down 0xB9 /* Sector 4k Erase instruction */
 
#define W25X_CMD_ReadData 0x03 /* Read from Memory instruction */
#define W25X_CMD_FastRead 0x0b /* Read from Memory instruction */
#define W25X_CMD_FastReadDualOut 0x3b /*Read from Memory instruction */
#define W25X_CMD_FastReadDualIO 0xBB /* Read from Memory instruction */
#define W25X_CMD_FastReadQuadOut 0x6b /* Read from Memory instruction */
#define W25X_CMD_FastReadQuadIO 0xeb /* Read from Memory instruction */
#define W25X_CMD_OctalWordRead 0xe3 /* Read from Memory instruction */

#define W25X_DUMMY_BYTE 0xff //0xA5
#define W25X_SPI_SECTOR_SIZE 4096
#define W25X_SPI_PAGESIZE 0x100
#define W25X_SPI_PAGES_COUNT 65536

#define SPI_FLASH_ORIGIN_ADDR 0x00000000
#define SPI_FLASH_FILE_ADDR SPI_FLASH_ORIGIN_ADDR + W25X_SPI_SECTOR_SIZE * 10

class spi_flash_driver
{
public:

  spi_flash_driver();
  virtual ~spi_flash_driver();
  
  static void spi_flash_deinit( void );
  static void spi_flash_init( void );
  static void spi_flash_erase_sector( uint32_t sec_addr );
  static void spi_flash_erase_bulk( void );
  static void spi_flash_write_page( uint8_t * buffer_ptr, uint32_t write_addr, uint16_t num_bytes_for_write );
  static void spi_flash_write_buffer( uint8_t * buffer_ptr, uint32_t write_addr, uint16_t num_bytes_for_write );
  static void spi_flash_read_buffer( uint8_t * buffer_ptr, uint32_t read_addr, uint16_t num_bytes_for_read );
  static uint32_t spi_flash_read_unique_id( void );
  static uint32_t spi_flash_read_jedec_id( void );
  static uint32_t spi_flash_read_id( void );
  static void spi_flash_start_read_sequence( uint32_t read_addr );

  /* Low layer functions */
  static uint8_t spi_flash_read_byte( void );
  static uint8_t spi_flash_send_byte( uint8_t byte );
  static uint16_t spi_flash_send_halfword( uint16_t half_word );
  static void spi_flash_write_enable( void );
  static void spi_flash_wait_for_write_end( void );
  static void spi_flash_chip_select();
  static void spi_flash_chip_deselect();
  static void spi_flash_wp_enable();
  static void spi_flash_wp_disable();
  static void spi_flash_hold_enable();
  static void spi_flash_hold_disable();
  
};

#endif /* SPI_FLASH_DRIVER_HPP */
