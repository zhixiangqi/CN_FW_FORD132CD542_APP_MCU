#ifndef APP_INC_FLASHAPP_H__
#define APP_INC_FLASHAPP_H__

#include "main.h"

void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length);
void FlashApp_CheckNorFlash(void);
void FlashApp_WriteNorFlash(uint32_t u32PageAddr, uint8_t u8length);

#endif