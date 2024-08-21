#ifndef APP_INC_TDDIApp_H__
#define APP_INC_TDDIApp_H__

#include "main.h"

/*For NT51926*/
#define NT51926_SLAVE_ADDRESS		        0x6FU


void DDIApp_ExitStandbyMode(void);
void DDIApp_DiagRead(uint8_t u8Register);
#endif
