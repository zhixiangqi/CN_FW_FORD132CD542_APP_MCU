/*******************************************************************************
 * File Name: cycfg_routing.h
 *
 * Description:
 * Establishes all necessary connections between hardware elements.
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.20.0
 * device-db 4.17.0.6514
 * mtb-pdl-cat2 2.12.0.12916
 *
 *******************************************************************************
 * Copyright 2024 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#if !defined(CYCFG_ROUTING_H)
#define CYCFG_ROUTING_H

#include "cycfg_notices.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define ioss_0_port_0_pin_4_ANALOG P0_4_WCO_WCO_IN
#define ioss_0_port_0_pin_5_ANALOG P0_5_WCO_WCO_OUT
#define ioss_0_port_1_pin_2_HSIOM P1_2_SCB2_I2C_SCL
#define ioss_0_port_1_pin_3_HSIOM P1_3_SCB2_I2C_SDA
#define ioss_0_port_2_pin_0_ANALOG P2_0_PASS0_SARMUX_PADS0
#define ioss_0_port_2_pin_1_ANALOG P2_1_PASS0_SARMUX_PADS1
#define ioss_0_port_2_pin_2_ANALOG P2_2_PASS0_SARMUX_PADS2
#define ioss_0_port_2_pin_3_ANALOG P2_3_PASS0_SARMUX_PADS3
#define ioss_0_port_3_pin_2_HSIOM P3_2_CPUSS_SWD_DATA
#define ioss_0_port_3_pin_3_HSIOM P3_3_CPUSS_SWD_CLK
#define ioss_0_port_4_pin_0_HSIOM P4_0_SCB0_SPI_MOSI
#define ioss_0_port_4_pin_1_HSIOM P4_1_SCB0_SPI_MISO
#define ioss_0_port_4_pin_2_HSIOM P4_2_SCB0_SPI_CLK
#define ioss_0_port_4_pin_3_HSIOM P4_3_SCB0_SPI_SELECT0
#define ioss_0_port_5_pin_6_HSIOM P5_6_TCPWM_LINE7
#define ioss_0_port_6_pin_4_HSIOM P6_4_SCB4_I2C_SCL
#define ioss_0_port_6_pin_5_HSIOM P6_5_SCB4_I2C_SDA
#define ioss_0_port_7_pin_0_HSIOM P7_0_SCB3_UART_RX
#define ioss_0_port_7_pin_1_HSIOM P7_1_SCB3_UART_TX
#define SAR0_VPLUS0_PIN 0
#define SAR0_VPLUS0_PORT 0
#define SAR0_VPLUS1_PIN 1
#define SAR0_VPLUS1_PORT 0
#define SAR0_VPLUS2_PIN 2
#define SAR0_VPLUS2_PORT 0
#define SAR0_VPLUS3_PIN 3
#define SAR0_VPLUS3_PORT 0

void init_cycfg_routing(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_ROUTING_H */
