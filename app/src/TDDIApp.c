/*
 * TDDIApp.c
 *
 *  Created on: 2024年8月2日
 *      Author: Administrator
 */

/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "app/inc/RegisterApp.h"
#include "app/inc/StackTaskApp.h"
#include "app/inc/BacklightApp.h"
#include "driver/inc/PwmDriver.h"
#include "driver/inc/AdcDriver.h"
#include "driver/inc/UartDriver.h"
#include "driver/inc/PortDriver.h"
#include "app/inc/TC0App.h"
#include "app/inc/TDDIApp.h"
#include "driver/inc/I2C4MDriver.h"


void TDDI_ExitStandbyMode(void)
{
    uint8_t txbuffer[2]={0x00U};
    txbuffer[0]=0x1E;
    txbuffer[1]=0x21;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));

    txbuffer[0]=0x09;
    txbuffer[1]=0xA5;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));

    txbuffer[0]=0x1E;
    txbuffer[1]=0x20;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));

    txbuffer[0]=0x1D;
    txbuffer[1]=0x03;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));

    txbuffer[0]=0x07;
    txbuffer[1]=0xA8;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));

    // bist auto run
    // txbuffer[0]=0x02;
    // txbuffer[1]=0x3F;
    // I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));

    txbuffer[0]=0x01;
    txbuffer[1]=0x07;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));
}