/*
 * SPIMDriver.h
 *
 *  Created on: 2024年9月10日
 *      Author: Administrator
 */

#ifndef DRIVER_INC_SPIMDRIVER_H_
#define DRIVER_INC_SPIMDRIVER_H_

/* ************************************************************************** */
/** Global Function Definition
 */
/* ************************************************************************** */
#include "main.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/* Re-Name Macro for project use*/
#define SPI0M_MCU_HW    SCB_SPI0M_HW
#define SPI0M_MCU_IRQ   SCB_SPI0M_IRQ
#define SPI0_MCU_config  SCB_SPI0M_config

/* Assign SPI interrupt priority */
#define SPI0M_INTR_PRIORITY  (3U)

void SPIMDriver_Interrupt(void);
bool SPIMDriver_Initial(void);
cy_en_scb_spi_status_t SPIMDriver_SendReceivePacket(uint8_t *txBuffer, uint8_t *rxBuffer);

#endif /* DRIVER_INC_SPIMDRIVER_H_ */
