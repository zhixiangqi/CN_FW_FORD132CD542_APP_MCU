/*
 * TPApp.c
 *
 *  Created on: 2024年8月16日
 *      Author: Administrator
 */
#include "app/inc/TPApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/INTBApp.h"
#include "app/inc/DiagApp.h"
#include "driver/inc/EicDriver.h"
#include "driver/inc/TC0Driver.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/UartDriver.h"
static uint8_t u8TPCount;
static uint8_t u8TxBuffer[60] = {0};

void TPApp_TCHENFlow(void)
{
    uint8_t u8CmdDispEnState;
    u8CmdDispEnState = RegisterApp_DHU_Read(CMD_DISP_EN,1U);
    /*Check TSC_EN*/
    switch (u8CmdDispEnState)
    {
    case DISPLAY_OFF_TOUCH_OFF:
    case DISPLAY_ON_TOUCH_OFF:
    case DISPLAY_OFF_TOUCH_ON:
        PortDriver_PinClear(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
        DiagApp_DispStatusClear(DISP_STATUS_BYTE1,DISP1_TSCST_MASK);
        break;

    case DISPLAY_ON_TOUCH_ON:
        PortDriver_PinSet(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
        DiagApp_DispStatusSet(DISP_STATUS_BYTE1,DISP1_TSCST_MASK);
        break;
    
    default:
        break;
    }
}

void TPApp_TCHINTFlow(void)
{
    uint8_t u8CmdDispEnState;
    u8CmdDispEnState = RegisterApp_DHU_Read(CMD_DISP_EN,1U);
    if (u8CmdDispEnState == DISPLAY_ON_TOUCH_ON)
    {
        /*First,read ISR state to judge*/
        uint8_t u8ISRState = RegisterApp_DHU_Read(CMD_ISR_STATUS,CMD_DATA_POS);
        /*Falling edge trigger*/
        if (tp_interr_low_flag == TRUE)
        {
            INTBApp_PullReqSetOrClear(INTB_REQ_SET);
            RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_TCH_SET | u8ISRState);
            tp_interr_low_flag = FALSE;
            sprintf((char *)u8TxBuffer,"TP_INT LOW %d\r\n",tp_interr_low_flag);
            UartDriver_TxWriteString((uint8_t*)u8TxBuffer);
        }
        /*Rising edge trigger*/
        else if (tp_interr_high_flag == TRUE)
        {
            RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_ERR_SET & u8ISRState);
            tp_interr_high_flag = FALSE;
            sprintf((char *)u8TxBuffer,"TP_INT HIGH %d\r\n",tp_interr_high_flag);
            UartDriver_TxWriteString((uint8_t*)u8TxBuffer);
        }
        /*If lost trigger,judge PIN whether is LOW,debouce 20 times,about 40ms*/
        else if ((PortDrvier_PinRead(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN) == PIN_LOW) && ((u8ISRState & INTB_INT_TCH_SET) != INTB_INT_TCH_SET))
        {
            u8TPCount++;
            if (u8TPCount > 20U)
            {
                INTBApp_PullReqSetOrClear(INTB_REQ_SET);
                RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_TCH_SET | INTB_INT_TCH_SET);
                u8TPCount = 0;
                sprintf((char *)u8TxBuffer,"TP_INT KEPEP LOW %d\r\n",u8TPCount);
                UartDriver_TxWriteString((uint8_t*)u8TxBuffer);
            }
        }else{/*Do nothing*/}
    }else{/*Do nothing*/}
}
