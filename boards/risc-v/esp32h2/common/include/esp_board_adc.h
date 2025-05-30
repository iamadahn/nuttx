/****************************************************************************
 * boards/risc-v/esp32h2/common/include/esp_board_adc.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_RISCV_ESP32C6_COMMON_INCLUDE_ESP32C6_BOARD_ADC_H
#define __BOARDS_RISCV_ESP32C6_COMMON_INCLUDE_ESP32C6_BOARD_ADC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: board_adc_init
 *
 * Description:
 *   This function configures and initializes the ADC driver for the board.
 *   It allocates memory for the ADC device structure, sets up the ADC
 *   hardware, and registers the ADC device with the system.
 *
 * Input Parameters:
 *   None.
 *
 * Returned Value:
 *   Returns zero (OK) on successful initialization and registration of the
 *   ADC device; a negated errno value is returned to indicate the nature
 *   of any failure.
 *
 ****************************************************************************/

#ifdef CONFIG_ESPRESSIF_ADC
int board_adc_init(void);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_RISCV_ESP32C6_COMMON_INCLUDE_ESP32C6_BOARD_ADC_H */
