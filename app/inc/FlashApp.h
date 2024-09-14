#ifndef APP_INC_FLASHAPP_H__
#define APP_INC_FLASHAPP_H__

#include "main.h"

#define  FLASH_WRITE_ENABLE_CMD 		    0x06U
#define  FLASH_WRITE_DISABLE_CMD		    0x04U
#define  FLASH_READ_SR1_CMD				    0x05U
#define  FLASH_READ_SR2_CMD				    0x35U
#define  FLASH_WRITE_SR_CMD				    0x01U
#define  FLASH_READ_DATA_CMD				0x03U
#define  FLASH_FASTREAD_DATA_CMD			0x0BU
#define  FLASH_PAGE_PROGRAM_CMD			    0x02U
#define  FLASH_SECTOR_ERASE_CMD      		0x20U
#define	 FLASH_BLOCK_ERASE_32k_CMD			0x52U
#define	 FLASH_BLOCK_ERASE_64k_CMD			0xD8U
#define	 FLASH_ERASE_CHIP_CMD				0xC7U
#define  FLASH_DEEP_POWER_DOWN_CMD			0xB9U
#define  FLASH_RELEASE_POWER_DOWN_CMD       0xABU
#define  FLASH_READ_DEVICE_ID_CMD      		0x90U
#define  FLASH_READ_JEDEC_ID_CMD      		0x9FU

void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length);
void FlashApp_WriteEnable();
void FlashApp_WriteDisable();
uint8_t FlashApp_ReadStatusRegister(uint8_t u8RDSR);
void FlashApp_WriteStatusRegister(uint8_t dataByte1, uint8_t dataByte2);
void FlashApp_ReadDataBytes(uint32_t u32SelectAddr, uint16_t u16length);
void FlashApp_SectorErase(uint32_t u32SelectAddr);
void FlashApp_EraseBlock(uint8_t u8BlockSize, uint32_t u32SelectAddr);
void FlashApp_EraseChip(uint32_t u32SelectAddr, uint16_t u16UseByte);
void Flash_WritePage(uint32_t u32SelectAddr, uint8_t *u8sendBuffer, uint16_t u16length);
uint16_t Flash_ReadDeviceId();
uint32_t Flash_ReadJedecId();
#endif