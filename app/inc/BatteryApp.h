#ifndef APP_INC_BATTERYAPP_H__
#define APP_INC_BATTERYAPP_H__

#include "main.h"

#define BT_NORMAL       0x00U
#define BT_LOWHIGH      0x01U
#define BT_OVERPOWER    0x02U

#define BT_VOLT10V      1510U
#define BT_VOLT15V      2265U

extern uint8_t u8SyncVolatgeState;
extern uint8_t u8UpdateVolatgeState;

void BatteryApp_SYNCVolatgeCheck(void);
void BatteryApp_PowerMonitor(void);
void BatteryApp_Flow(void);

#endif

