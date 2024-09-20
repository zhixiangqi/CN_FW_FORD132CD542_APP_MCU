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
#include "driver/inc/PortDriver.h"

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
 * Function Name: SPIMDriver_Initialize
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

bool SPIMDriver_Initialize(void)
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

uint8_t SPIMDriver_Transfer(uint8_t *txBuffer, uint8_t *rxBuffer, uint32_t bufferSize)
{
    uint8_t status = ERROR_TX_FAIL;
    cy_en_scb_spi_status_t errorStatus;
    uint32_t masterStatus;
    /* Timeout 1 sec (one unit is us) */
    uint32_t timeout = 8000UL;

    /* Master: start a transfer. Slave: prepare for a transfer. */
    errorStatus = Cy_SCB_SPI_Transfer(SPI0M_MCU_HW, txBuffer, rxBuffer, bufferSize, &SPI0M_MCU_context);
    if (errorStatus == CY_SCB_SPI_SUCCESS)
    {
        /* Blocking wait for transfer completion */
        do
        {
            masterStatus = Cy_SCB_SPI_GetTransferStatus(SPI0M_MCU_HW, &SPI0M_MCU_context);
            Cy_SysLib_DelayUs(CY_SCB_WAIT_1_UNIT);
            timeout--;
        } while (0UL != (CY_SCB_SPI_TRANSFER_ACTIVE & masterStatus));
        if (timeout <= 0)
        {
            status = ERROR_TX_TIMEOUT;
            /* Timeout recovery */
            Cy_SCB_SPI_Disable(SPI0M_MCU_HW, &SPI0M_MCU_context);
            Cy_SCB_SPI_Enable(SPI0M_MCU_HW);
        }else{
            /* Check transfer status */
            switch (masterStatus & CY_SCB_SPI_TX_INTR_MASK)
            {
            case 0:
                status = ERROR_TX_NONE;
                break;
            case CY_SCB_SPI_TX_TRIGGER:
                /* code */
                status = ERROR_TX_TRIGGER;
                break;
            
            case CY_SCB_SPI_TX_NOT_FULL:
                /* code */
                status = ERROR_TX_NOT_FULL;
                break;

            case CY_SCB_SPI_TX_EMPTY:
                /* code */
                status = ERROR_TX_EMPTY;
                break;

            case CY_SCB_SPI_TX_OVERFLOW:
                /* code */
                status = ERROR_TX_OVERFLOW;
                break;

            case CY_SCB_SPI_TX_UNDERFLOW:
                /* code */
                status = ERROR_TX_UNDERFLOW;
                break;

            default:
                status = ERROR_TX_FAIL;
                break;
            }
        }
    }
    if(status != CY_SCB_SPI_SUCCESS){
        Cy_SCB_SPI_DeInit(SPI0M_MCU_HW);
        SPIMDriver_Initialize();
    }
    return (status);
}