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
#include "app/inc/TC0App.h"
#include "driver/inc/EicDriver.h"
#include "driver/inc/TC0Driver.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/UartDriver.h"

static uint8_t u8TouchCount;
bool bTscAttnState = FALSE;
void TPApp_TscEnFlow(void)
{
    uint8_t u8TscEnState;
    u8TscEnState = RegisterApp_DHU_Read(CMD_DISP_EN,1U);
    if (u8TscEnState == DISPLAY_ON_TOUCH_ON)
    {
        PortDriver_PinSet(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
        if (PortDrvier_PinRead(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN) == PIN_HIGH)
        {
            bTscAttnState = TRUE;
            DiagApp_DispStatusSet(DISP_STATUS_BYTE1,DISP1_TSCST_MASK);
        }
    }else{
        PortDriver_PinClear(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
        DiagApp_DispStatusClear(DISP_STATUS_BYTE1,DISP1_TSCST_MASK);
    }
}
void TPApp_TscIntFlow(void)
{    
    if (bTscAttnState)
    {
        /*Falling edge trigger*/
        if (tp_interr_low_flag == TRUE)
        {
            INTBApp_PullReqSetOrClear(INTB_REQ_SET);
            DiagApp_RtnIsrCheck(true,INTB_INT_TSC_MASK);
            tp_interr_low_flag = FALSE;
            // sprintf((char *)u8TxBuffer,"TP_INT LOW %d\r\n",tp_interr_low_flag);
        }
        /*Rising edge trigger*/
        else if (tp_interr_high_flag == TRUE)
        {
            tp_interr_high_flag = FALSE;
            DiagApp_RtnIsrCheck(false,INTB_INT_TSC_MASK);
            // sprintf((char *)u8TxBuffer,"TP_INT HIGH %d\r\n",tp_interr_high_flag);
        }
        /*If lost trigger,judge PIN whether is LOW,debouce 400ms*/
        else if ((PortDrvier_PinRead(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN) == PIN_LOW) && (tp_interr_high_flag == FALSE))
        {
            if (bTscIntKeepLow && u8TouchCount < 5U)
            {
                u8TouchCount++;
                bTscIntKeepLow = FALSE;
                INTBApp_PullReqSetOrClear(INTB_REQ_SET);
                DiagApp_RtnIsrCheck(true,INTB_INT_TSC_MASK);
                // sprintf((char *)u8TxBuffer,"TP_INT KEPEP LOW %d\r\n",u8ISRState);
            }
        }else{
            u8TouchCount =0U;
            bTscIntKeepLow = FALSE;
        }
        // UartDriver_TxWriteString((uint8_t*)u8TxBuffer);
    }else{
        bTscIntKeepLow = FALSE;
    }
}