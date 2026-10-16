/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the wrapper of the default interrupt
 *   handler.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <bsp/irq-generic.h>

RTEMS_INTERRUPT_LOCK_DEFINE(
  static,
  default_interrupt_handler_lock,
  "Default Interrupt Handler"
)

static DefaultInterruptHandler default_interrupt_handler;

static void *default_interrupt_handler_arg;

void SetDefaultInterruptHandler( DefaultInterruptHandler handler, void *arg )
{
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire(
    &default_interrupt_handler_lock,
    &lock_context
  );
  default_interrupt_handler = handler;
  default_interrupt_handler_arg = arg;
  rtems_interrupt_lock_release(
    &default_interrupt_handler_lock,
    &lock_context
  );
}

void __real_bsp_interrupt_handler_default( rtems_vector_number vector );

void __wrap_bsp_interrupt_handler_default( rtems_vector_number vector );

void __wrap_bsp_interrupt_handler_default( rtems_vector_number vector )
{
  rtems_interrupt_lock_context lock_context;
  DefaultInterruptHandler      handler;
  void                        *arg;

  rtems_interrupt_lock_acquire_isr(
    &default_interrupt_handler_lock,
    &lock_context
  );
  handler = default_interrupt_handler;
  arg = default_interrupt_handler_arg;
  rtems_interrupt_lock_release_isr(
    &default_interrupt_handler_lock,
    &lock_context
  );

  if ( handler != NULL ) {
    ( *handler )( arg, vector );
  } else {
    __real_bsp_interrupt_handler_default( vector );
  }
}
