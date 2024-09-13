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
#define ERROR_TX_NONE           0x00U
#define ERROR_TX_TRIGGER        CY_SCB_SPI_TX_TRIGGER
#define ERROR_TX_NOT_FULL       CY_SCB_SPI_TX_NOT_FULL
#define ERROR_TX_EMPTY          CY_SCB_SPI_TX_EMPTY
#define ERROR_TX_OVERFLOW       CY_SCB_SPI_TX_OVERFLOW
#define ERROR_TX_UNDERFLOW      CY_SCB_SPI_TX_UNDERFLOW
#define ERROR_TX_FAIL           0xFFU

/* Re-Name Macro for project use*/
#define SPI0M_MCU_HW    SCB_SPI0M_HW
#define SPI0M_MCU_IRQ   SCB_SPI0M_IRQ
#define SPI0_MCU_config  SCB_SPI0M_config

/* Assign SPI interrupt priority */
#define SPI0M_INTR_PRIORITY  (3U)

void SPIMDriver_Interrupt(void);
bool SPIMDriver_Initialize(void);
uint8_t SPIMDriver_Transfer(uint8_t *txBuffer, uint8_t *rxBuffer, uint32_t txSize, uint32_t rxSize);

#endif /* DRIVER_INC_SPIMDRIVER_H_ */
