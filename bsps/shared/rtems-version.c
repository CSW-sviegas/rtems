/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the implementation of
 *   rtems_board_support_package() and the definition of ::_RTEMS_version.
 */

/*
 * Copyright (C) 2018, 2026 embedded brains GmbH & Co. KG
 * Copyright (C) 2003 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/version.h>
#include <rtems/score/copyrt.h>
#include <rtems/score/cpu.h>

#include <bspopts.h>

/*
 * RTEMS_XSTRING() turns an undefined RTEMS_BSP into the string "RTEMS_BSP".
 * The string concatenation below fails for each other undefined macro.
 */
#if !defined( RTEMS_BSP )
#error "the build shall define RTEMS_BSP"
#endif

const char _RTEMS_version[] =
  "rtems-" RTEMS_VERSION " (" CPU_NAME "/" CPU_MODEL_NAME "/"
  RTEMS_XSTRING( RTEMS_BSP ) ")";

const char *rtems_board_support_package( void )
{
  return RTEMS_XSTRING( RTEMS_BSP );
}
