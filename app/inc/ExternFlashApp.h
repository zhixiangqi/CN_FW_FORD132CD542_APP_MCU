/*
 * ExternFlashApp.h
 *
 *  Created on: 2024年12月9日
 *      Author: ZhixiangQi
 */

#ifndef APP_INC_EXTERNFLASHAPP_H_
#define APP_INC_EXTERNFLASHAPP_H_

#include "main.h"

#define CHIP_ERASE_FALG  	0xC7U
#define WrittenVaild        0xAABBU
#define WrittenInvaild      0xAAFFU
#define Writable            0xFFFFU

#define FixedSectorAddr     200U

extern uint16_t u16CurLogSN;

void ExternFlashApp_Verify(void);
void ExternFlashApp_Write(void);
#endif /* APP_INC_EXTERNFLASHAPP_H_ */
