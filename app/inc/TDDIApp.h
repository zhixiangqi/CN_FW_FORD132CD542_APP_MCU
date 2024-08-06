#ifndef APP_INC_TDDIApp_H__
#define APP_INC_TDDIApp_H__

#include "main.h"

/*For NT51926*/
#define TOUCH_SLAVE_ADDRESS			        0x01U
#define TOUCH_SLAVE_ADDRESS_UP_FW	        0x62U	/* touch firmware update */
#define NT51926_SLAVE_ADDRESS		        0x6FU
#define NT51926_SLAVE_SECOND_ADDRESS		0x6DU

void TDDIApp_ExitStandbyMode(void);
#endif
