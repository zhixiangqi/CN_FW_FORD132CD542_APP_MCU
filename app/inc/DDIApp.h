#ifndef APP_INC_TDDIApp_H__
#define APP_INC_TDDIApp_H__

#include "main.h"

/*For NT51926*/
#define NT51926_SLAVE_ADDRESS		        0x6FU

#define EXIT_STANDBY_MODE   0x00U
#define ENTER_STANDBY_MODE  0x01U
#define CHOOSE_BIST_MODE    0x02U

void DDIApp_StandbyMode(uint8_t u8ModeState);
void DDIApp_DiagRead(uint8_t u8Register);
#endif
