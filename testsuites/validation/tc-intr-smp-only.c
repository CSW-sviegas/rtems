/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrValIntrSmpOnly
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

#include <rtems.h>

#include "tr-isr-lock-fifo.h"
#include "tx-isr-lock.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrValIntrSmpOnly spec:/rtems/intr/val/intr-smp-only
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests some @ref RTEMSAPIClassicIntr directives.
 *
 * This test case performs the following actions:
 *
 * - Validate the interrupt lock directives.
 *
 *   - Assert that RTEMS_INTERRUPT_LOCK_NEEDS_OBJECT is a constant expression
 *     which evaluates to a non-zero value.
 *
 *   - Check that RTEMS_INTERRUPT_LOCK_REFERENCE() expanded to a lock reference
 *     definition. Check that the lock is available after static
 *     initialization.
 *
 *   - Check that the lock is available after initialization.
 *
 * - Validate the order in which processors acquire an interrupt lock.
 *
 *   - Validate the order for rtems_interrupt_lock_release().
 *
 *   - Validate the order for rtems_interrupt_lock_release_isr().
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/intr/val/intr-smp-only test case.
 */
typedef struct {
  /**
   * @brief This member contains the context of the ISR lock test runs.
   */
  ISRLockContext lock_ctx;

  /**
   * @brief This member contains the lock under test.
   */
  rtems_interrupt_lock lock;
} RtemsIntrValIntrSmpOnly_Context;

static RtemsIntrValIntrSmpOnly_Context RtemsIntrValIntrSmpOnly_Instance;

RTEMS_INTERRUPT_LOCK_DECLARE( static, the_lock )
RTEMS_INTERRUPT_LOCK_DEFINE( static, the_lock, "name " )

static T_fixture RtemsIntrValIntrSmpOnly_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsIntrValIntrSmpOnly_Instance
};

/**
 * @brief Validate the interrupt lock directives.
 */
static void RtemsIntrValIntrSmpOnly_Action_0( void )
{
  struct {
    int a;
    RTEMS_INTERRUPT_LOCK_MEMBER( member )
    int b;
  } lock = { .member = RTEMS_INTERRUPT_LOCK_INITIALIZER( "name" ) };

  RTEMS_INTERRUPT_LOCK_REFERENCE( ref, &the_lock )

  /*
   * Assert that RTEMS_INTERRUPT_LOCK_NEEDS_OBJECT is a constant expression
   * which evaluates to a non-zero value.
   */
  RTEMS_STATIC_ASSERT( RTEMS_INTERRUPT_LOCK_NEEDS_OBJECT, LOCK_NEEDS_OBJECT );

  /*
   * Check that RTEMS_INTERRUPT_LOCK_REFERENCE() expanded to a lock reference
   * definition. Check that the lock is available after static initialization.
   */
  T_true( ISRLockIsAvailable( ref ) );

  /*
   * Check that the lock is available after initialization.
   */
  T_true( ISRLockIsAvailable( &lock.member ) );
}

/**
 * @brief Validate the order in which processors acquire an interrupt lock.
 */
static void RtemsIntrValIntrSmpOnly_Action_1(
  RtemsIntrValIntrSmpOnly_Context *ctx
)
{
  ISRLockSetClassic( &ctx->lock_ctx, &ctx->lock, ISR_LOCK_RESTORE_TO_CONTEXT );

  /*
   * Validate the order for rtems_interrupt_lock_release().
   */
  ScoreIsrReqLockFifo_Run( &ctx->lock_ctx );

  /*
   * Validate the order for rtems_interrupt_lock_release_isr().
   */
  ISRLockSetClassic( &ctx->lock_ctx, &ctx->lock, ISR_LOCK_RESTORE_NONE );
  ScoreIsrReqLockFifo_Run( &ctx->lock_ctx );
}

/**
 * @fn void T_case_body_RtemsIntrValIntrSmpOnly( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrValIntrSmpOnly,
  &RtemsIntrValIntrSmpOnly_Fixture
)
{
  RtemsIntrValIntrSmpOnly_Context *ctx;

  ctx = T_fixture_context();

  RtemsIntrValIntrSmpOnly_Action_0();
  RtemsIntrValIntrSmpOnly_Action_1( ctx );
}

/** @} */
