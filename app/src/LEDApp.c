/*
 * LedDriveApp.c
 *
 *  Created on: 2024年8月7日
 *      Author: Administrator
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "app/inc/LEDApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/StackTaskApp.h"
#include "app/inc/BacklightApp.h"
#include "driver/inc/PwmDriver.h"
#include "driver/inc/AdcDriver.h"
#include "driver/inc/UartDriver.h"
#include "driver/inc/PortDriver.h"
#include "app/inc/TC0App.h"
#include "driver/inc/I2C4MDriver.h"

void LEDDrive_FaultREead(void)
{
    uint8_t rxbuffer[6]={0x00U};
    uint8_t txbuffer[1]={0x00U};
    txbuffer[0]=0x0E;
    I2C4MDriver_WriteRead(LP8864_SLAVE_ADDRESS, &txbuffer[0],sizeof(txbuffer), &rxbuffer[0], sizeof(rxbuffer));
}
