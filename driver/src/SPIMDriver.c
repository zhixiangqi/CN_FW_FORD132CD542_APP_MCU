/*
 * SPIMDriver.c
 *
 *  Created on: 2024年9月10日
 *      Author: Administrator
 */
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "driver/inc/SPIMDriver.h"

// *****************************************************************************
// *****************************************************************************
// Section: PORT Implementation
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
cy_stc_scb_spi_context_t SPI0M_MCU_context;

/*******************************************************************************
 * Function Name: SPIMDriver_Interrupt
 *******************************************************************************
 *
 * Invokes the Cy_SCB_SPI_Interrupt() PDL driver function.
 *
 ******************************************************************************/
void SPIMDriver_Interrupt(void)
{
    Cy_SCB_SPI_Interrupt(SCB_SPI0M_HW, &SPI0M_MCU_context);
}

/*******************************************************************************
 * Function Name: SPIMDriver_Initial
 *******************************************************************************
 *
 * Summary:
 * This function initializes the SPI master based on the configuration done in
 * design.modus file.
 *
 * Parameters:
 * None
 *
 * Return:
 * uint32_t - Returns INIT_SUCCESS if the initialization is successful.
 * Otherwise it returns INIT_FAILURE
 *
 ******************************************************************************/

bool SPIMDriver_Initial(void)
{
    bool bresult = true;

    cy_en_scb_spi_status_t initStatus;
    cy_en_sysint_status_t sysSpistatus;

    /* Configure the SPI block */

    initStatus = Cy_SCB_SPI_Init(SPI0M_MCU_HW, &SPI0_MCU_config, &SPI0M_MCU_context);
    if( initStatus != CY_SCB_SPI_SUCCESS)
    {
        bresult = false;
    }

    /* Set active slave select to line 0 */
    Cy_SCB_SPI_SetActiveSlaveSelect(SPI0M_MCU_HW, CY_SCB_SPI_SLAVE_SELECT0);

    /* Populate configuration structure */

    const cy_stc_sysint_t CYBSP_SPI0M_SCB_IRQ_cfg =
    {
            .intrSrc      = SPI0M_MCU_IRQ,
            .intrPriority = SPI0M_INTR_PRIORITY
    };

    /* Hook interrupt service routine and enable interrupt */
    sysSpistatus = Cy_SysInt_Init(&CYBSP_SPI0M_SCB_IRQ_cfg, &SPIMDriver_Interrupt);
    if(sysSpistatus != CY_SYSINT_SUCCESS)
    {
        bresult = false;
    }
    /* Enable interrupt in NVIC */
    NVIC_EnableIRQ(SPI0M_MCU_IRQ);

    /* Enable the SPI Master block */
    Cy_SCB_SPI_Enable(SPI0M_MCU_HW);

    return bresult;
}

/*******************************************************************************
 * Function Name: sendPacket
 *******************************************************************************
 *
 * Summary:
 * This function sends the data to the slave.
 *
 * Parameters:
 * txBuffer - Pointer to the transmit buffer
 * transferSize - Number of bytes to be transmitted
 *
 * Return:
 * cy_en_scb_spi_status_t - CY_SCB_SPI_SUCCESS if the transaction completes
 * successfully. Otherwise it returns the error status
 *
 ******************************************************************************/
cy_en_scb_spi_status_t I2C4MDriver_SendPacket(uint8_t *txBuffer, uint32_t transferSize)
{
    cy_en_scb_spi_status_t masterStatus;

    /* Initiate SPI Master write transaction. */
    masterStatus = Cy_SCB_SPI_Transfer(SPI0M_MCU_HW, txBuffer, NULL,
                                       transferSize, &SPI0M_MCU_context);

    /* Blocking wait for transfer completion */
    while (0UL != (CY_SCB_SPI_TRANSFER_ACTIVE &
                   Cy_SCB_SPI_GetTransferStatus(SPI0M_MCU_HW, &SPI0M_MCU_context)))
    {
    }

    return (masterStatus);
}
