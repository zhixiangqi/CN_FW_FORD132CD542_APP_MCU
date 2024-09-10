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

const cy_stc_sysint_t intTsc_intr_config =
{
    .intrSrc = U301_TSC_ATTN_IRQ,	/* Interrupt source is U301_TSC_ATTN_PIN interrupt */
    .intrPriority = 3UL,    		/* Interrupt priority is 3 */
};

void EicDriver_U301_TSC_ATTN_ISR(void)
{
    /* Clears the triggered pin interrupt */
	Cy_GPIO_ClearInterrupt(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN);
	NVIC_ClearPendingIRQ(intTsc_intr_config.intrSrc);

    /*Both edge,by reading PIN state to judge*/
    if (Cy_GPIO_Read(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN) == PIN_LOW)
    {
        tp_interr_low_flag = TRUE;
    }else{
        tp_interr_high_flag = TRUE;
    }
}

bool EicDriver_Initial(void)
{
    bool bresult = true;

    cy_en_sysint_status_t sysStatus;

    /* Initialize and enable GPIO interrupt */
    sysStatus = Cy_SysInt_Init(&intTsc_intr_config, EicDriver_U301_TSC_ATTN_ISR);
    if(sysStatus != CY_SYSINT_SUCCESS)
    {
        bresult = false;
    }
    /* Clearing and enabling the GPIO interrupt in NVIC */
    NVIC_ClearPendingIRQ(intTsc_intr_config.intrSrc);
    NVIC_EnableIRQ(intTsc_intr_config.intrSrc);

    return bresult;
}