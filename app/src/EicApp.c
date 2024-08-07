/*
 * EicApp.c
 *
 *  Created on: 2024年8月5日
 *      Author: Administrator
 */
#include "app/inc/EicApp.h"

bool bf_TddiInterStatus = false;

void Interrupt_Handler_TDDI(void)
{
   Cy_GPIO_ClearInterrupt(TDDI_ATTN_PORT, TDDI_ATTN_PORT_NUM);
   bf_TddiInterStatus = !bf_TddiInterStatus;
}

void EicAppInitial(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    // Interrupt config structure
    cy_stc_sysint_t intrCfg =
    {
        .intrSrc = TDDI_ATTN_IRQ, /* Interrupt source is GPIO port 3 interrupt */
        .intrPriority = 4U /* Interrupt priority is 7 */
    };

    /* Initialize the interrupt with vector at Interrupt_Handler_Port3() */
    Cy_SysInt_Init(&intrCfg, &Interrupt_Handler_TDDI);
    // // Send the button through the glitch filter
    Cy_GPIO_SetFilter(TDDI_ATTN_PORT, TDDI_ATTN_PORT_NUM);
    // //Enable the interrupt mask for the GPIO pin.
    Cy_GPIO_SetInterruptMask(TDDI_ATTN_PORT, TDDI_ATTN_PORT_NUM, 1U);
    // // Falling edge interrupt
    Cy_GPIO_SetInterruptEdge(TDDI_ATTN_PORT, TDDI_ATTN_PORT_NUM, CY_GPIO_INTR_FALLING);
    /* Enable the interrupt */
    NVIC_EnableIRQ(intrCfg.intrSrc);
 
}

