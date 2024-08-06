/*
 * EicApp.c
 *
 *  Created on: 2024年8月5日
 *      Author: Administrator
 */
#include "app/inc/EicApp.h"

/******************************************************************************
    * Global variable
    *****************************************************************************/
uint32_t tddi_interrupt_flag = false;

/******************************************************************************
     * Switch interrupt configuration structure
     *****************************************************************************/
const cy_stc_sysint_t tddi_intr_config = {
.intrSrc = TDDI_USER_BIN_IRQ,                  /* Source of interrupt signal */
.intrPriority = TDDI_INTR_PRIORITY            /* Interrupt priority */
};

void EicAppInitial(void)
{
    
 
}

void TDDI_ISR()
{
    /* Clears the triggered pin interrupt */
    Cy_GPIO_ClearInterrupt(TDDI_USER_BIN_PORT, TDDI_USER_BIN_NUM);
    NVIC_ClearPendingIRQ(tddi_intr_config.intrSrc);

    /* Set interrupt flag */
    tddi_interrupt_flag = true;
}
