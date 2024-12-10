/* ************************************************************************** */

/** Descriptive File Name

  @Company
    AUO

  @File Name
    WdtApp.c

  @Summary
    Implement WDT function.

  @Description
    -Wdt Clean function.
    -Wdt Reset Cause dectection.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "driver/inc/WdtDriver.h"
#include "driver/inc/UartDriver.h"
#include "driver/inc/PortDriver.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/TC0App.h"
#include "app/inc/StackTaskApp.h"
#include "app/inc/WdtApp.h"

static uint8_t u8TxWdtBuffer[60] = {0};

#if 0
volatile bool isr_flag = false;

static void WdtApp_ISR(void)
{
    /* Avoid continue interrupt*/
    Cy_WDT_MaskInterrupt();
    isr_flag = true;
}
#endif
//LDRA_EXCLUDE_START 496 S
//LDRA_EXCLUDE_START 106 D
void WdtApp_CheckResetCause(void)
{
    switch (Cy_SysLib_GetResetReason())
    {
    case CY_SYSLIB_RESET_HWWDT:
        /* code */
        sprintf((char *)u8TxWdtBuffer,"Reset Cause by 0x%04lX:HWWDT!\r\n",CY_SYSLIB_RESET_HWWDT);
        UartDriver_TxWriteString(u8TxWdtBuffer);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_RESET_CAUSE,0x02);
        break;

    case CY_SYSLIB_PROT_FAULT:
        /* code */
        sprintf((char *)u8TxWdtBuffer,"Reset Cause by 0x%04lX:FAULT!\r\n",CY_SYSLIB_PROT_FAULT);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_RESET_CAUSE,0x04);
        UartDriver_TxWriteString(u8TxWdtBuffer);
        break;

    case CY_SYSLIB_RESET_SOFT:
        /* code */
        sprintf((char *)u8TxWdtBuffer,"Reset Cause by 0x%04lX:SOFT!\r\n",CY_SYSLIB_RESET_SOFT);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_RESET_CAUSE,0x08);
        UartDriver_TxWriteString(u8TxWdtBuffer);
        break;

    default:
        sprintf((char *)u8TxWdtBuffer,"Reset Cause by 0x%04lX:UNKOWN!\r\n",Cy_SysLib_GetResetReason());
        RegisterApp_DHU_Setup(CMD_DTC,DTC_RESET_CAUSE,0x01);
        UartDriver_TxWriteString(u8TxWdtBuffer);
        break;
    }
}

void WdtApp_InterruptCallback(void)
{
    uint8_t wdt_casue = 0U;
    wdt_casue = StackTaskApp_TaskNumberReturn();
    sprintf((char *)u8TxWdtBuffer,"WDT Timeout by 0x%02X:FAULT!\r\n",wdt_casue);
    RegisterApp_DHU_Setup(CMD_DTC,DTC_WDT_ALARM,wdt_casue);
    UartDriver_TxWriteString(u8TxWdtBuffer);
}

void WdtApp_CleanCounter(void)
{
    Cy_WDT_ClearInterrupt();
    Cy_WDT_ClearWatchdog();
    TC0App_TimerReset(TIMER_WDTCOUNT);
#if 0
    if (isr_flag == true)
    {
        Cy_WDT_UnmaskInterrupt();
        isr_flag = false;
        /* Update the match count */
        WdtDriver_UpdateMatchCount();
        
    }
#endif
    // PortDriver_PinToggle(P1V2_EN_PORT,P1V2_EN_PIN);
}

void WdtApp_Initial(void)
{
    Cy_WDT_ClearWatchdog();
    WdtDriver_Disable();
    (void)WdtDriver_Initial();
    (void)WdtDriver_SetMatchIfPeriodMode();
#if 0
    (void)WdtDriver_RegisterISR(WdtApp_ISR);
#endif
    (void)WdtDriver_Enable();
    WdtDriver_RegisterDSCallback();
}
//LDRA_EXCLUDE_END 496 S
//LDRA_EXCLUDE_END 106 D
/* *****************************************************************************
 End of File
*/