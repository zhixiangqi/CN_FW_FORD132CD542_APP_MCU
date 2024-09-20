#ifndef APP_INC_FLASHAPP_H__
#define APP_INC_FLASHAPP_H__

#include "main.h"

#define GD25Q80E_PAGE_LEN     256U
#define GD25Q80E_NUM_PAGES    16384U
#define GD25Q80E_NUM_BYTES    4194304U

#define GD25Q80E_WRITE_ENABLE   0x06
#define GD25Q80E_WRITE_DISABLE  0x04
#define GD25Q80E_CHIP_ERASE     0xC7
#define GD25Q80E_READ_STATUS1   0x05
#define GD25Q80E_WRITE_STATUS   0x01
#define GD25Q80E_PAGE_PROGRAM   0x02
#define GD25Q80E_DEVICE_ID      0x9F
#define GD25Q80E_UNIQUE_ID      0x90
#define GD25Q80E_READ_DATA      0x03

#define GD25Q80E_STATUS_BUSY    0x01

#define GD25Q80E_STATUS_SUCCESS 			 0x00
#define GD25Q80E_STATUS_FAIL 			 	 0x01
#define GD25Q80E_STATUS_ADDR_OUT_OF_BOUND    0x02

void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length);
uint8_t GD25Q80E_ReadStatusRegister(void);
uint32_t GD25Q80E_ReadJedecId(void);
void GD25Q80E_WaitWhileBusy(void);
uint8_t GD25Q80E_WriteEnable(bool enabled);
uint8_t GD25Q80E_WriteStatusRegister(uint8_t u8statusRegister);
uint8_t GD25Q80E_EraseChip(void);
uint8_t GD25Q80_ReadData(uint32_t u32addr, uint16_t u8len, uint8_t* u8data);
uint8_t GD25Q80E_PageProgram(uint32_t u32addr, uint16_t u8len, uint8_t* u8data);

#endif