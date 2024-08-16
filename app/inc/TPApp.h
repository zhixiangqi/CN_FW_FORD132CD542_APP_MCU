/*
 * TPApp.h
 *
 *  Created on: 2024年8月16日
 *      Author: Administrator
 */

#ifndef APP_INC_TPAPP_H_
#define APP_INC_TPAPP_H_


#include "main.h"

#define DISPLAY_OFF_TOUCH_OFF				0x00U
#define DISPLAY_ON_TOUCH_OFF				0x01U
#define DISPLAY_OFF_TOUCH_ON				0x02U
#define DISPLAY_ON_TOUCH_ON					0x03U

void TPApp_TCHFlow(void);
void TPApp_TCHENMonitor(void);
#endif /* APP_INC_TPAPP_H_ */
