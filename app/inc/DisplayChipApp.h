#ifndef DISPLAYCHIPAPP_H__
#define DISPLAYCHIPAPP_H__

#include "main.h"

#define CHIP_ADDR   0x6FU

#define EXIT_STANDBY_MODE   0x00U
#define ENTER_STANDBY_MODE  0x01U
#define CHOOSE_BIST_MODE    0x02U
#define CHOOSE_CHROMA_MODE  0x03U

void DisplayChipApp_WorkMode(uint8_t u8ModeState);
void DisplayChipApp_FaultCheck(void);
void DisplayChipApp_VerCheck(void);

#endif
