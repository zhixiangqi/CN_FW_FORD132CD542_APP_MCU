#ifndef APP_INC_FLASHAPP_H__
#define APP_INC_FLASHAPP_H__

#include "main.h"

#define ADDR_MCUFLASH_FABCTRL    0x0001F100U
#define ADDR_MCUFLASH_MAXIMUM    0x00020000U
#define ADDR_MCUFLASH_APPFLAG    0x0001F000U
#define ADDR_MCUFLASH_SNUMBER    0x0001F100U
#define ADDR_MCUFLASH_DIMMING    0x0001F200U

#define FIRST_PAGE_ADDR          0x0000U
#define SECOND_PAGE_ADDR         0x0100U
#define PAGE_SIZE                0x100U
#define LOG_LENGTH               0x40U
#define ERASE_FLAG               0xC7U

#define ERASE_FLAG_OFFEST        55U
#define WRITECOUNT_OFFEST        56U

void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length);
void FlashApp_CheckNorFlash(void);
void FlashApp_WriteNorFlash(void);

#endif