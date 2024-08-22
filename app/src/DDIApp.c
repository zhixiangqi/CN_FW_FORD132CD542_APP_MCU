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

#include "app/inc/DDIApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/StackTaskApp.h"
#include "app/inc/BacklightApp.h"
#include "app/inc/TC0App.h"
#include "driver/inc/PwmDriver.h"
#include "driver/inc/AdcDriver.h"
#include "driver/inc/UartDriver.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/I2C4MDriver.h"

static uint8_t u8TxBuffer[60] = {0};
void DDIApp_StandbyMode(uint8_t u8ModeState)
{

    uint8_t txbuffer[2]={0x00U};
    txbuffer[0]=0x1E;
    txbuffer[1]=0x20;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));
    if (u8ModeState == EXIT_STANDBY_MODE)
    {
        txbuffer[0]=0x01;
        txbuffer[1]=0x07;
        I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));
    }else if(u8ModeState == ENTER_STANDBY_MODE)
    {
        txbuffer[0]=0x01;
        txbuffer[1]=0x06;
        I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));
    }else if(u8ModeState == CHOOSE_BIST_MODE)
    {
        txbuffer[0]=0x02;
        txbuffer[1]=0x3F;
        I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));

        txbuffer[0]=0x01;
        txbuffer[1]=0x07;
        I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));
    }else{/*Do nothing*/}
    /*
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
    txbuffer[0]=0x02;
    txbuffer[1]=0x3F;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));

    txbuffer[0]=0x01;
    txbuffer[1]=0x07;
    I2C4MDriver_Write(NT51926_SLAVE_ADDRESS, txbuffer, sizeof(txbuffer));
    */
}

void DDIApp_DiagRead(uint8_t u8Register)
{
    uint8_t rxbuffer[1]={0x00U};
    uint8_t u8Return;
    u8Return = I2C4MDriver_WriteRead(NT51926_SLAVE_ADDRESS, &u8Register, sizeof(rxbuffer), rxbuffer,sizeof(rxbuffer));
    if (u8Return == ERROR_NONE)
    {
        sprintf((char *)u8TxBuffer,"NT51926 STATE %d\r\n",rxbuffer[0]);
    }
    else{
        sprintf((char *)u8TxBuffer,"NT51926 STATE %d\r\n",0xFF);
    }
    UartDriver_TxWriteString(u8TxBuffer);
}
