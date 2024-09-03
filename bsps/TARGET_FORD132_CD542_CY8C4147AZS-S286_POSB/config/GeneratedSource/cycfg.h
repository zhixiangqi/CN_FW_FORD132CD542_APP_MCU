/*******************************************************************************
 * File Name: cycfg.h
 *
 * Description:
 * Simple wrapper containing all generated files and function to initialize
 * all generated code.
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.20.0
 * device-db 4.17.0.6514
 * mtb-pdl-cat2 2.11.0.12518
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

#if !defined(CYCFG_H)
#define CYCFG_H

#include "../../../TARGET_FORD132_CD542_CY8C4147AZS-S285/config/GeneratedSource/cycfg_clocks.h"
#include "../../../TARGET_FORD132_CD542_CY8C4147AZS-S285/config/GeneratedSource/cycfg_notices.h"
#include "../../../TARGET_FORD132_CD542_CY8C4147AZS-S285/config/GeneratedSource/cycfg_peripherals.h"
#include "../../../TARGET_FORD132_CD542_CY8C4147AZS-S285/config/GeneratedSource/cycfg_pins.h"
#include "../../../TARGET_FORD132_CD542_CY8C4147AZS-S285/config/GeneratedSource/cycfg_routing.h"
#include "../../../TARGET_FORD132_CD542_CY8C4147AZS-S285/config/GeneratedSource/cycfg_system.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

void init_cycfg_all(void);
void cycfg_config_init(void);
void cycfg_config_reservations(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_H */
