#ifndef APP_INC_BATTERYAPP_H__
#define APP_INC_BATTERYAPP_H__

#include "main.h"

#define BT_NORMAL       0x00U
#define BT_LOWHIGH      0x01U
#define BT_OVERPOWER    0x02U

extern bool bSyncVolatgeState;

void BatteryApp_SYNCVolatgeCheck(void);
void BatteryApp_PowerMonitor(void);
void BatteryApp_Flow(void);

#endif

