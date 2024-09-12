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

static uint8_t u8TouchCount = 0U;
static bool    bTscAttnSafeKey = FALSE;
static bool    bTscAttnState = FALSE;
static bool    bTscIntKeepLow = FALSE;
void TPApp_IntTscStateFlow(uint8_t u8TscEnState)
{
    if ((u8TscEnState & 0x02U) == 0x02U)
    {
        if (bTscAttnSafeKey == FALSE)
        {
            PortDriver_PinSet(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
            /* SWRA-01-06: Set DISP_STATUS 0x00 CMD Byte1 TSC_ST set as 1.*/
            DiagApp_DispStatusSet(DISP_STATUS_BYTE1,DISP1_TSCST_MASK);
            TC0App_TimerReset(TIMER_INT_ATTN_COUNT);
            TC0App_IntbAttnCountStartSet(TRUE);
            bTscAttnSafeKey = TRUE;
        }
        else
        {
            if(TC0App_TimerReturn(TIMER_INT_ATTN_COUNT) > 250U)
            {
                bTscAttnState = TRUE;
                TC0App_TimerReset(TIMER_INT_ATTN_COUNT);
                TC0App_IntbAttnCountStartSet(FALSE);
            }
        }
        if (bTscAttnState == TRUE)
        {
            /*Falling edge trigger*/
            if (tp_interr_low_flag == TRUE)
            {
                INTBApp_PullReqSetOrClear(INTB_REQ_SET);
                DiagApp_RtnIsrCheck(true,INTB_INT_TSC_MASK);
                tp_interr_low_flag = FALSE;
            }
            /*Rising edge trigger*/
            if (tp_interr_high_flag == TRUE)
            {
                tp_interr_high_flag = FALSE;
                DiagApp_RtnIsrCheck(false,INTB_INT_TSC_MASK);
            }
            /*If lost trigger,judge PIN whether is LOW,debouce 250ms*/
            else if ((tp_interr_low_flag == FALSE) && (PortDrvier_PinRead(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN) == PIN_LOW))
            {
                if (bTscIntKeepLow == FALSE)
                {
                    TC0App_TimerReset(TIMER_INT_ATTN_COUNT);
                    TC0App_IntbAttnCountStartSet(TRUE);
                    bTscIntKeepLow = TRUE;
                }
                else
                {
                    if(TC0App_TimerReturn(TIMER_INT_ATTN_COUNT) > 50U)
                    {
                        TC0App_TimerReset(TIMER_INT_ATTN_COUNT);
                        TC0App_IntbAttnCountStartSet(FALSE);
                        if (u8TouchCount < 5U)
                        {
                            u8TouchCount++;
                            bTscIntKeepLow = FALSE;
                            INTBApp_PullReqSetOrClear(INTB_REQ_SET);
                            DiagApp_RtnIsrCheck(true,INTB_INT_TSC_MASK);
                        }
                    }
                }
            }
        }else{
            /*Prevent false triggering of initialization power-up*/
            tp_interr_low_flag = FALSE;
            tp_interr_high_flag = FALSE;
        }
    }else{
        PortDriver_PinClear(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
        /* SWRA-01-06: Set DISP_STATUS 0x00 CMD Byte1 TSC_ST set as 0.*/
        DiagApp_DispStatusClear(DISP_STATUS_BYTE1,DISP1_TSCST_MASK);
        bTscAttnSafeKey = FALSE;
        bTscAttnState   = FALSE;
        bTscIntKeepLow  = FALSE;
        u8TouchCount    = 0U;
    }
}