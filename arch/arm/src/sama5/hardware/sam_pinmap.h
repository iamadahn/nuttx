/****************************************************************************
 * arch/arm/src/sama5/hardware/sam_pinmap.h
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

#ifndef __ARCH_ARM_SRC_SAMA5_HARDWARE_SAM_PINMAP_H
#define __ARCH_ARM_SRC_SAMA5_HARDWARE_SAM_PINMAP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <arch/sama5/chip.h>

#if defined(ATSAMA5D2)
#  include "hardware/_sama5d2x_pinmap.h"
#elif defined(ATSAMA5D3)
#  include "hardware/_sama5d3x_pinmap.h"
#elif defined(ATSAMA5D4)
#  include "hardware/_sama5d4x_pinmap.h"
#else
#  error Unrecognized SAMA5 architecture
#endif

#endif /* __ARCH_ARM_SRC_SAMA5_HARDWARE_SAM_PINMAP_H */
