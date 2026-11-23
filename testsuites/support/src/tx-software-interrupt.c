/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of
 *   ClearSoftwareInterrupt(), GetSoftwareInterruptVector(), and
 *   RaiseSoftwareInterrupt().
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test-support.h>

#include <bsp.h>
#include <bsp/irq-generic.h>

extern uint32_t Interrupt_nest;

#define _RTEMS_TMTEST27

#include <tm27.h>

/*
 * This file provides the functions for a BSP without an alternative vector.
 * It does not depend on the call within ISR support, so a test which uses
 * only these functions leaves the vector of Install_tm27_vector() free.
 */
#if !defined( TM27_INTERRUPT_VECTOR_ALTERNATIVE )
rtems_vector_number GetSoftwareInterruptVector( void )
{
  return UINT32_MAX;
}

rtems_status_code RaiseSoftwareInterrupt( rtems_vector_number vector )
{
  return rtems_interrupt_raise( vector );
}

rtems_status_code ClearSoftwareInterrupt( rtems_vector_number vector )
{
  return rtems_interrupt_clear( vector );
}
#endif
