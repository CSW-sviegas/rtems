/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrValLock
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

#include <rtems.h>

#include "tr-isr-lock-acquire.h"
#include "tr-isr-lock-destroy.h"
#include "tr-isr-lock-initialize.h"
#include "tr-isr-lock-isr-disable.h"
#include "tr-isr-lock-isr-enable.h"
#include "tr-isr-lock-release.h"
#include "tx-isr-lock.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrValLock spec:/rtems/intr/val/lock
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @brief Tests the interrupt lock directives.
 *
 * This test case performs the following actions:
 *
 * - Validate the initialization of an interrupt lock.
 *
 *   - Validate rtems_interrupt_lock_initialize().
 *
 *   - Validate RTEMS_INTERRUPT_LOCK_INITIALIZER().
 *
 * - Validate the directives which acquire and release an interrupt lock and
 *   which disable and restore maskable interrupts with the lock context.
 *
 *   - Validate rtems_interrupt_lock_acquire().
 *
 *   - Validate rtems_interrupt_lock_release(). Validate also the release of a
 *     lock which rtems_interrupt_lock_acquire_isr() acquired after
 *     rtems_interrupt_lock_interrupt_disable().
 *
 * - Validate the directives which acquire and release an interrupt lock and
 *   which do not change the status of maskable interrupts.
 *
 *   - Validate rtems_interrupt_lock_acquire_isr().
 *
 *   - Validate rtems_interrupt_lock_release_isr().
 *
 * - Validate the directive which destroys an interrupt lock.
 *
 *   - Validate rtems_interrupt_lock_destroy().
 *
 * - Validate the directives which disable maskable interrupts and restore
 *   their status with a lock context.
 *
 *   - Validate rtems_interrupt_lock_interrupt_disable().
 *
 *   - Validate rtems_interrupt_lock_interrupt_enable().
 *
 *   - Acquire the lock with rtems_interrupt_lock_acquire() and release it with
 *     rtems_interrupt_lock_release_isr(). Check that the lock is free and that
 *     maskable interrupts are still disabled. Check that
 *     rtems_interrupt_lock_interrupt_enable() enables them.
 *
 *   - Destroy the lock.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/intr/val/lock test case.
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
} RtemsIntrValLock_Context;

static RtemsIntrValLock_Context RtemsIntrValLock_Instance;

static T_fixture RtemsIntrValLock_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsIntrValLock_Instance
};

/**
 * @brief Validate the initialization of an interrupt lock.
 */
static void RtemsIntrValLock_Action_0( RtemsIntrValLock_Context *ctx )
{
  ISRLockSetClassic( &ctx->lock_ctx, &ctx->lock, ISR_LOCK_RESTORE_TO_CONTEXT );

  /*
   * Validate rtems_interrupt_lock_initialize().
   */
  ctx->lock_ctx.method = ISR_LOCK_METHOD_FUNCTION;
  ScoreIsrReqLockInitialize_Run( &ctx->lock_ctx );

  /*
   * Validate RTEMS_INTERRUPT_LOCK_INITIALIZER().
   */
  ctx->lock_ctx.method = ISR_LOCK_METHOD_INITIALIZER;
  ScoreIsrReqLockInitialize_Run( &ctx->lock_ctx );
}

/**
 * @brief Validate the directives which acquire and release an interrupt lock
 *   and which disable and restore maskable interrupts with the lock context.
 */
static void RtemsIntrValLock_Action_1( RtemsIntrValLock_Context *ctx )
{
  ISRLockSetClassic( &ctx->lock_ctx, &ctx->lock, ISR_LOCK_RESTORE_TO_CONTEXT );

  /*
   * Validate rtems_interrupt_lock_acquire().
   */
  ScoreIsrReqLockAcquire_Run( &ctx->lock_ctx );

  /*
   * Validate rtems_interrupt_lock_release(). Validate also the release of a
   * lock which rtems_interrupt_lock_acquire_isr() acquired after
   * rtems_interrupt_lock_interrupt_disable().
   */
  ScoreIsrReqLockRelease_Run( &ctx->lock_ctx );
}

/**
 * @brief Validate the directives which acquire and release an interrupt lock
 *   and which do not change the status of maskable interrupts.
 */
static void RtemsIntrValLock_Action_2( RtemsIntrValLock_Context *ctx )
{
  ISRLockSetClassic( &ctx->lock_ctx, &ctx->lock, ISR_LOCK_RESTORE_NONE );

  /*
   * Validate rtems_interrupt_lock_acquire_isr().
   */
  ScoreIsrReqLockAcquire_Run( &ctx->lock_ctx );

  /*
   * Validate rtems_interrupt_lock_release_isr().
   */
  ScoreIsrReqLockRelease_Run( &ctx->lock_ctx );
}

/**
 * @brief Validate the directive which destroys an interrupt lock.
 */
static void RtemsIntrValLock_Action_3( RtemsIntrValLock_Context *ctx )
{
  ISRLockSetClassic( &ctx->lock_ctx, &ctx->lock, ISR_LOCK_RESTORE_TO_CONTEXT );

  /*
   * Validate rtems_interrupt_lock_destroy().
   */
  ScoreIsrReqLockDestroy_Run( &ctx->lock_ctx );
}

/**
 * @brief Validate the directives which disable maskable interrupts and restore
 *   their status with a lock context.
 */
static void RtemsIntrValLock_Action_4( RtemsIntrValLock_Context *ctx )
{
  rtems_interrupt_lock_context lock_context;
  bool                         enabled;
  bool                         is_free;

  ISRLockSetClassic( &ctx->lock_ctx, &ctx->lock, ISR_LOCK_RESTORE_TO_CONTEXT );
  rtems_interrupt_lock_initialize( &ctx->lock, "Test" );

  /*
   * Validate rtems_interrupt_lock_interrupt_disable().
   */
  ScoreIsrReqLockIsrDisable_Run( &ctx->lock_ctx );

  /*
   * Validate rtems_interrupt_lock_interrupt_enable().
   */
  ScoreIsrReqLockIsrEnable_Run( &ctx->lock_ctx );

  /*
   * Acquire the lock with rtems_interrupt_lock_acquire() and release it with
   * rtems_interrupt_lock_release_isr(). Check that the lock is free and that
   * maskable interrupts are still disabled. Check that
   * rtems_interrupt_lock_interrupt_enable() enables them.
   */
  rtems_interrupt_lock_acquire( &ctx->lock, &lock_context );
  rtems_interrupt_lock_release_isr( &ctx->lock, &lock_context );
  enabled = AreInterruptsEnabled();
  is_free = ISRLockIsFree( &ctx->lock_ctx );
  rtems_interrupt_lock_interrupt_enable( &lock_context );
  T_false( enabled );
  T_true( is_free );
  T_true( AreInterruptsEnabled() );

  /*
   * Destroy the lock.
   */
  rtems_interrupt_lock_destroy( &ctx->lock );
}

/**
 * @fn void T_case_body_RtemsIntrValLock( void )
 */
T_TEST_CASE_FIXTURE( RtemsIntrValLock, &RtemsIntrValLock_Fixture )
{
  RtemsIntrValLock_Context *ctx;

  ctx = T_fixture_context();

  RtemsIntrValLock_Action_0( ctx );
  RtemsIntrValLock_Action_1( ctx );
  RtemsIntrValLock_Action_2( ctx );
  RtemsIntrValLock_Action_3( ctx );
  RtemsIntrValLock_Action_4( ctx );
}

/** @} */
