# ##############################################################################
# mm/umm_heap/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for
# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

# User heap allocator

set(SRCS
    umm_globals.c
    umm_initialize.c
    umm_addregion.c
    umm_malloc_size.c
    umm_brkaddr.c
    umm_calloc.c
    umm_extend.c
    umm_free.c
    umm_mallinfo.c
    umm_malloc.c
    umm_memalign.c
    umm_realloc.c
    umm_zalloc.c
    umm_heapmember.c
    umm_memdump.c)

if(CONFIG_BUILD_KERNEL)
  list(APPEND SRCS umm_sbrk.c)
endif()

if(CONFIG_DEBUG_MM)
  list(APPEND SRCS umm_checkcorruption.c)
endif()

target_sources(mm PRIVATE ${SRCS})
