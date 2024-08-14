/*
 * EicDriver.c
 *
 *  Created on: 2024年8月9日
 *      Author: Administrator
 */
#include "app/inc/TC0App.h"
#include "driver/inc/EicDriver.h"
#include "driver/inc/TC0Driver.h"

bool tp_interr_flag = false;

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

    /*Start count debounce times*/
    tp_interr_flag = true;
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