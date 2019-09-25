#include "spi_flash_driver.hpp"

spi_flash_driver::spi_flash_driver() { asm("nop"); }

void spi_flash_driver::spi_flash_hold_disable() {
  GPIO_SetBits(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
}

void spi_flash_driver::spi_flash_hold_enable() {
  GPIO_ResetBits(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_HOLD_PIN);
}

void spi_flash_driver::spi_flash_wp_enable() { GPIO_ResetBits(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN); }

void spi_flash_driver::spi_flash_wp_disable() { GPIO_SetBits(SPI_FLASH_WP_HOLD_PORT, SPI_FLASH_WP_HOLD_GPIO_WP_PIN); }

void spi_flash_driver::spi_flash_chip_select() { GPIO_ResetBits(SPI_FLASH_GPIO_PORT, SPI_FLASH_GPIO_NSS_PIN); }

void spi_flash_driver::spi_flash_chip_deselect() { GPIO_SetBits(SPI_FLASH_GPIO_PORT, SPI_FLASH_GPIO_NSS_PIN); }

void spi_flash_driver::spi_flash_write_enable() {
  spi_flash_driver::spi_flash_wp_enable();
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_CMD_WriteEnable);
  spi_flash_driver::spi_flash_chip_deselect();
}

spi_flash_driver::~spi_flash_driver() {
  SPI_Cmd(SPI_FLASH_SPI_PORT, DISABLE);
  SPI_I2S_DeInit(SPI_FLASH_SPI_PORT);
}

void spi_flash_driver::spi_flash_erase_sector(uint32_t sec_addr) {
  spi_flash_driver::spi_flash_write_enable();
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_CMD_SectorErase);
  spi_flash_driver::spi_flash_send_byte((sec_addr & 0xFF0000) >> 16);
  spi_flash_driver::spi_flash_send_byte((sec_addr & 0xFF00) >> 8);
  spi_flash_driver::spi_flash_send_byte(sec_addr & 0xFF);
  spi_flash_driver::spi_flash_chip_deselect();
  spi_flash_driver::spi_flash_wait_for_write_end();
}

void spi_flash_driver::spi_flash_erase_bulk() {
  spi_flash_driver::spi_flash_write_enable();
  spi_flash_driver::spi_flash_wait_for_write_end();
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_CMD_ChipErase);
  spi_flash_driver::spi_flash_chip_deselect();
  spi_flash_driver::spi_flash_wait_for_write_end();
}

void spi_flash_driver::spi_flash_write_page(uint8_t *buffer_ptr, uint32_t write_addr, uint16_t num_bytes_for_write) {
  spi_flash_driver::spi_flash_write_enable();
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_CMD_PageProgram);
  spi_flash_driver::spi_flash_send_byte((write_addr & 0xFF0000) >> 16);
  spi_flash_driver::spi_flash_send_byte((write_addr & 0xFF00) >> 8);
  spi_flash_driver::spi_flash_send_byte(write_addr & 0xFF);

  while (num_bytes_for_write--) {

    spi_flash_driver::spi_flash_send_byte(*buffer_ptr);
    buffer_ptr++;
  }

  spi_flash_driver::spi_flash_chip_deselect();
  spi_flash_driver::spi_flash_wait_for_write_end();
}

void spi_flash_driver::spi_flash_write_buffer(uint8_t *buffer_ptr, uint32_t write_addr, uint16_t num_bytes_for_write) {
  uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

  addr = write_addr % W25X_SPI_PAGESIZE;
  count = W25X_SPI_PAGESIZE - addr;
  num_of_page = num_bytes_for_write / W25X_SPI_PAGESIZE;
  num_of_single = num_bytes_for_write % W25X_SPI_PAGESIZE;

  if (addr == 0) {

    if (num_of_page == 0) {

      spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, num_bytes_for_write);

    } else {

      while (num_of_page--) {

        spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, W25X_SPI_PAGESIZE);
        write_addr += W25X_SPI_PAGESIZE;
        buffer_ptr += W25X_SPI_PAGESIZE;
      }

      spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, num_of_single);
    }
  } else {

    if (num_of_page == 0) {

      if (num_of_single > count) {

        temp = num_of_single - count;
        spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, count);
        write_addr += count;
        buffer_ptr += count;
        spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, temp);

      } else {

        spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, num_bytes_for_write);
      }
    } else {

      num_bytes_for_write -= count;
      num_of_page = num_bytes_for_write / W25X_SPI_PAGESIZE;
      num_of_single = num_bytes_for_write % W25X_SPI_PAGESIZE;
      spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, count);
      write_addr += count;
      buffer_ptr += count;

      while (num_of_page--) {

        spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, W25X_SPI_PAGESIZE);
        write_addr += W25X_SPI_PAGESIZE;
        buffer_ptr += W25X_SPI_PAGESIZE;
      }

      if (num_of_single != 0) {

        spi_flash_driver::spi_flash_write_page(buffer_ptr, write_addr, num_of_single);
      }
    }
  }
}

void spi_flash_driver::spi_flash_read_buffer(uint8_t *buffer_ptr, uint32_t read_addr, uint16_t num_bytes_for_read) {
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_CMD_ReadData);
  spi_flash_driver::spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
  spi_flash_driver::spi_flash_send_byte((read_addr & 0xFF00) >> 8);
  spi_flash_driver::spi_flash_send_byte(read_addr & 0xFF);

  while (num_bytes_for_read--) {

    *buffer_ptr = spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE);
    buffer_ptr++;
  }

  spi_flash_driver::spi_flash_chip_deselect();
}

uint32_t spi_flash_driver::spi_flash_read_unique_id() {
  uint32_t temp = 0;
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_ReadUniqueID);
  spi_flash_driver::spi_flash_send_byte(0x00);
  spi_flash_driver::spi_flash_send_byte(0x00);
  spi_flash_driver::spi_flash_send_byte(0x00);
  temp |= (spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE) << 8);
  temp |= spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE);
  spi_flash_driver::spi_flash_chip_deselect();
  return temp;
}

uint32_t spi_flash_driver::spi_flash_read_jedec_id() {
  uint32_t temp = 0;
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_JedecDeviceID);
  spi_flash_driver::spi_flash_send_byte(0x00);
  spi_flash_driver::spi_flash_send_byte(0x00);
  spi_flash_driver::spi_flash_send_byte(0x00);
  temp |= (spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE) << 8);
  temp |= spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE);
  spi_flash_driver::spi_flash_chip_deselect();
  return temp;
}

uint32_t spi_flash_driver::spi_flash_read_id() {
  uint32_t temp = 0;
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_ManufactDeviceID);
  spi_flash_driver::spi_flash_send_byte(0x00);
  spi_flash_driver::spi_flash_send_byte(0x00);
  spi_flash_driver::spi_flash_send_byte(0x00);
  temp |= (spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE) << 8);
  temp |= spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE);
  spi_flash_driver::spi_flash_chip_deselect();
  return temp;
}

void spi_flash_driver::spi_flash_start_read_sequence(uint32_t read_addr) {
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_CMD_ReadData);
  spi_flash_driver::spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
  spi_flash_driver::spi_flash_send_byte((read_addr & 0xFF00) >> 8);
  spi_flash_driver::spi_flash_send_byte(read_addr & 0xFF);
}

uint8_t spi_flash_driver::spi_flash_read_byte() { return (spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE)); }

uint8_t spi_flash_driver::spi_flash_send_byte(uint8_t byte) {
  while (SPI_I2S_GetFlagStatus(SPI_FLASH_SPI_PORT, SPI_I2S_FLAG_TXE) == RESET)
    ;
  SPI_I2S_SendData(SPI_FLASH_SPI_PORT, byte);
  while (SPI_I2S_GetFlagStatus(SPI_FLASH_SPI_PORT, SPI_I2S_FLAG_RXNE) == RESET)
    ;
  return SPI_I2S_ReceiveData(SPI_FLASH_SPI_PORT);
}

uint16_t spi_flash_driver::spi_flash_send_halfword(uint16_t half_word) {
  while (SPI_I2S_GetFlagStatus(SPI_FLASH_SPI_PORT, SPI_I2S_FLAG_TXE) == RESET)
    ;
  SPI_I2S_SendData(SPI_FLASH_SPI_PORT, half_word);
  while (SPI_I2S_GetFlagStatus(SPI_FLASH_SPI_PORT, SPI_I2S_FLAG_RXNE) == RESET)
    ;
  return SPI_I2S_ReceiveData(SPI_FLASH_SPI_PORT);
}

void spi_flash_driver::spi_flash_wait_for_write_end() {
  uint8_t flashstatus = 0;
  spi_flash_driver::spi_flash_chip_select();
  spi_flash_driver::spi_flash_send_byte(W25X_CMD_ReadStatusReg1);

  do {

    flashstatus = spi_flash_driver::spi_flash_send_byte(W25X_DUMMY_BYTE);

  } while ((flashstatus & W25X_CMD_WriteStatusReg) == SET);

  spi_flash_driver::spi_flash_chip_deselect();
}

/* Stubs */
