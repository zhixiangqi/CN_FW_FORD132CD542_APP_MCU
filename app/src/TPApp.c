/*
 * TPApp.c
 *
 *  Created on: 2024年8月16日
 *      Author: Administrator
 */
#include "app/inc/TPApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/INTBApp.h"
#include "driver/inc/EicDriver.h"
#include "driver/inc/TC0Driver.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/UartDriver.h"
static uint8_t u8TPCount;
static uint8_t u8TxBuffer[60] = {0};
static uint8_t u8TCH_EN_State = 0U;

void TPApp_TCHENMonitor(void)
{
    u8TCH_EN_State = RegisterApp_DHU_Read(CMD_DISP_EN,CMD_DATA_POS);
    if (u8TCH_EN_State == DISPLAY_ON_TOUCH_ON)
    {
        PortDriver_PinSet(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
    }else if(u8TCH_EN_State == DISPLAY_ON_TOUCH_OFF)
    {
        PortDriver_PinClear(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
    }else{/*Do nothing*/}

    sprintf((char *)u8TxBuffer,"DISP&TP STATE %d\r\n",u8TCH_EN_State);
    UartDriver_TxWriteString((uint8_t*)u8TxBuffer);
}

void TPApp_TCHFlow(void)
{
    if (u8TCH_EN_State == DISPLAY_ON_TOUCH_ON)
    {
        if (tp_interr_low_flag == TRUE)
        {
            INTBApp_PullReqSetOrClear(INTB_REQ_SET);
            RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_TCH_SET | RegisterApp_DHU_Read(CMD_ISR_STATUS,CMD_DATA_POS));
            tp_interr_low_flag = FALSE;
        }else if (tp_interr_high_flag == TRUE)
        {
            RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_ERR_SET & RegisterApp_DHU_Read(CMD_ISR_STATUS,CMD_DATA_POS));
            tp_interr_high_flag = FALSE;
        }else if (PortDrvier_PinRead(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN) == PIN_LOW)
        {
            u8TPCount++;
            if (u8TPCount > 20U)
            {
                INTBApp_PullReqSetOrClear(INTB_REQ_SET);
                RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_TCH_SET | RegisterApp_DHU_Read(CMD_ISR_STATUS,CMD_DATA_POS));
                u8TPCount = 0;
            }
        }else{/*Do nothing*/}
    }else{/*Do nothing*/}
}