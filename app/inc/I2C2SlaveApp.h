#ifndef APP_INC_I2C2SLAVEAPP_H__
#define APP_INC_I2C2SLAVEAPP_H__

#include "main.h"

bool I2C2SlaveApp_Initial(void);
void I2CSlaveApp_UpdateCmdChecksumSet(uint8_t subaddr);

#endif