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
#define ERROR_TX_TRIGGER        0x01U
#define ERROR_TX_NOT_FULL       0x02U
#define ERROR_TX_TIMEOUT        0x03U
#define ERROR_TX_EMPTY          0x10U
#define ERROR_TX_OVERFLOW       0x20U
#define ERROR_TX_UNDERFLOW      0x40U
#define ERROR_TX_FAIL           0x73U

/* Re-Name Macro for project use*/
#define SPI0M_MCU_HW    SCB_SPI0M_HW
#define SPI0M_MCU_IRQ   SCB_SPI0M_IRQ
#define SPI0_MCU_config  SCB_SPI0M_config

/* Assign SPI interrupt priority */
#define SPI0M_INTR_PRIORITY  (3U)

void SPIMDriver_Interrupt(void);
bool SPIMDriver_Initialize(void);
uint8_t SPIMDriver_Transfer(uint8_t *txBuffer, uint8_t *rxBuffer, uint32_t bufferSize);
#endif /* DRIVER_INC_SPIMDRIVER_H_ */
