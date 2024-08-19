/*
 * EicDriver.c
 *
 *  Created on: 2024年8月9日
 *      Author: Administrator
 */
#include "app/inc/TC0App.h"
#include "driver/inc/EicDriver.h"
#include "driver/inc/TC0Driver.h"
#include "driver/inc/PortDriver.h"

bool tp_interr_low_flag = false;
bool tp_interr_high_flag = false;
static uint8_t u8ISRState = 0;
const static cy_stc_sysint_t gtdATTNCfg =
{
    .intrSrc = U301_TSC_ATTN_IRQ,	/* Interrupt source is U301_TSC_ATTN_PIN interrupt */
    .intrPriority = 3UL,    		/* Interrupt priority is 3 */
};

static void EicDriver_U301_TSC_ATTN_ISR(void)
{
	/* Clears the triggered pin interrupt */
	Cy_GPIO_ClearInterrupt(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN);
	NVIC_ClearPendingIRQ(gtdATTNCfg.intrSrc);

    u8ISRState = PortDrvier_PinRead(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN);
    if (u8ISRState == PIN_LOW)
    {
        tp_interr_low_flag = TRUE;
    }else{
        tp_interr_high_flag = TRUE;
    }
}

void EicDriver_Initial(void)
{
    cy_rslt_t result;

    /* Initialize and enable GPIO interrupt */
    result = Cy_SysInt_Init(&gtdATTNCfg, EicDriver_U301_TSC_ATTN_ISR);
    if(result != CY_SYSINT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /* Clearing and enabling the GPIO interrupt in NVIC */
    NVIC_ClearPendingIRQ(gtdATTNCfg.intrSrc);
    NVIC_EnableIRQ(gtdATTNCfg.intrSrc);
}