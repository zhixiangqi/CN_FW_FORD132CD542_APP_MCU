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
uint8_t u8TPCount;
void TPApp_TCHFlow(void)
{
    if (tp_interr_flag == TRUE)
    {
        INTBApp_PullReqSetOrClear(INTB_REQ_SET);
        if (RegisterApp_DHU_Read(CMD_ISR_STATUS,CMD_DATA_POS) == INTB_INT_ERR_SET)
        {
            RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_TCH_SET+1U);
        }else{
            RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_TCH_SET);
        }
        tp_interr_flag = FALSE;
    }else if (PortDrvier_PinRead(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN) == PIN_LOW)
    {
        u8TPCount++;
        if (u8TPCount > 20U)
        {
            if (RegisterApp_DHU_Read(CMD_ISR_STATUS,CMD_DATA_POS) == INTB_INT_ERR_SET)
            {
                RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_TCH_SET+1U);
            }else{
                RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_TCH_SET);
            }
            u8TPCount = 0;
        }
    }else{/*Do nothing*/}
    
}
void TPApp_TCHENMonitor(void)
{
    uint8_t u8TCH_EN_State = 0U;
    u8TCH_EN_State = RegisterApp_DHU_Read(CMD_DISP_EN,CMD_DATA_POS);
    switch (u8TCH_EN_State)
    {
        case DISPLAY_OFF_TOUCH_OFF:
            PortDriver_PinClear(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
            break;

        case DISPLAY_ON_TOUCH_OFF:
            PortDriver_PinClear(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
            break;

        case DISPLAY_OFF_TOUCH_ON:
            PortDriver_PinSet(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
            break;

        case DISPLAY_ON_TOUCH_ON:
            PortDriver_PinSet(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
            break;
        
        default:
            /*Do nothing*/
            break;
    }
    
    
}
