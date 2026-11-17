/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of the ISR lock test
 *   support.
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

#include <rtems/test-isr-lock.h>
#include <rtems/test.h>

#include <rtems/counter.h>

#if defined( RTEMS_SMP )
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>
#endif

bool ISRLockIsFree( const ISRLockContext *ctx )
{
#if defined( RTEMS_SMP )
  return TicketLockIsAvailable( ( *ctx->get_ticket_lock )( ctx ) );
#else
  (void) ctx;

  return true;
#endif
}

static rtems_interrupt_lock *ClassicLock( const ISRLockContext *ctx )
{
  return ctx->lock;
}

static void ClassicInitialize( ISRLockContext *ctx )
{
  if ( ctx->method == ISR_LOCK_METHOD_INITIALIZER ) {
    rtems_interrupt_lock lock = RTEMS_INTERRUPT_LOCK_INITIALIZER( "Test" );

    *ClassicLock( ctx ) = lock;
  } else {
    rtems_interrupt_lock_initialize( ClassicLock( ctx ), "Test" );
  }
}

static void ClassicDestroy( ISRLockContext *ctx )
{
  (void) ctx;
  rtems_interrupt_lock_destroy( ClassicLock( ctx ) );
}

static void ClassicAcquire(
  ISRLockContext               *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  (void) ctx;
  rtems_interrupt_lock_acquire( ClassicLock( ctx ), lock_context );
}

static void ClassicRelease(
  ISRLockContext               *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  (void) ctx;
  rtems_interrupt_lock_release( ClassicLock( ctx ), lock_context );
}

static void ClassicAcquireIsr(
  ISRLockContext               *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  (void) ctx;
  rtems_interrupt_lock_acquire_isr( ClassicLock( ctx ), lock_context );
}

static void ClassicReleaseIsr(
  ISRLockContext               *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  (void) ctx;
  rtems_interrupt_lock_release_isr( ClassicLock( ctx ), lock_context );
}

static void ClassicInterruptDisable(
  ISRLockContext               *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  (void) ctx;
  rtems_interrupt_lock_interrupt_disable( lock_context );
}

static void ClassicDisableAcquireIsr(
  ISRLockContext               *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  ClassicInterruptDisable( ctx, lock_context );
  ClassicAcquireIsr( ctx, lock_context );
}

static void ClassicInterruptEnable(
  ISRLockContext               *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  (void) ctx;
  rtems_interrupt_lock_interrupt_enable( lock_context );
}

static ISR_Level ClassicGetSavedLevel(
  const ISRLockContext               *ctx,
  const rtems_interrupt_lock_context *lock_context
)
{
  (void) ctx;

#if defined( RTEMS_SMP )
  return lock_context->Lock_context.isr_level;
#else
  return lock_context->isr_level;
#endif
}

#if defined( RTEMS_SMP )
static const SMP_ticket_lock_Control *ClassicGetTicketLock(
  const ISRLockContext *ctx
)
{
  return &ClassicLock( ctx )->Lock.Ticket_lock;
}
#endif

void ISRLockSetClassic(
  ISRLockContext       *ctx,
  rtems_interrupt_lock *lock,
  ISRLockRestore        restore
)
{
  T_quiet_ne_int( (int) restore, (int) ISR_LOCK_RESTORE_TO_OBJECT );

  ctx->restore = restore;
  ctx->method = ISR_LOCK_METHOD_FUNCTION;
  ctx->lock = lock;
  ctx->lock_size = sizeof( *lock );
  ctx->initialize = ClassicInitialize;
  ctx->destroy = ClassicDestroy;
  ctx->interrupt_disable = ClassicInterruptDisable;
  ctx->disable_acquire_isr = ClassicDisableAcquireIsr;
  ctx->interrupt_enable = ClassicInterruptEnable;
#if defined( RTEMS_SMP )
  ctx->get_ticket_lock = ClassicGetTicketLock;
#endif

  if ( restore == ISR_LOCK_RESTORE_NONE ) {
    ctx->acquire = ClassicAcquireIsr;
    ctx->release = ClassicReleaseIsr;
    ctx->get_saved_level = NULL;
  } else {
    ctx->acquire = ClassicAcquire;
    ctx->release = ClassicRelease;
    ctx->get_saved_level = ClassicGetSavedLevel;
  }
}

#if defined( RTEMS_SMP )
uint32_t ISRLockGetOtherProcessors( uint32_t *cpu_indices, uint32_t count )
{
  uint32_t cpu_max;
  uint32_t cpu_self;
  uint32_t cpu_index;
  uint32_t n;

  cpu_max = _SMP_Get_processor_maximum();
  cpu_self = rtems_scheduler_get_processor();
  n = 0;

  for ( cpu_index = 0; cpu_index < cpu_max && n < count; ++cpu_index ) {
    if (
      cpu_index != cpu_self &&
      _Per_CPU_Is_processor_online( _Per_CPU_Get_by_index( cpu_index ) )
    ) {
      cpu_indices[ n ] = cpu_index;
      ++n;
    }
  }

  return n;
}

static void ISRLockWorkerJob( void *arg )
{
  ISRLockWorker                 *worker;
  ISRLockContext                *ctx;
  const SMP_ticket_lock_Control *ticket_lock;
  ISR_Level                      level;

  worker = arg;
  ctx = worker->ctx;
  ticket_lock = ( *ctx->get_ticket_lock )( ctx );

  _ISR_Local_disable( level );
  ( *ctx->acquire )( ctx, &worker->lock_context );
  worker->sequence = ctx->sequence;
  ++ctx->sequence;

  while (
    TicketLockGetWaiters( ticket_lock ) < worker->release_after_waiters ||
    _Atomic_Load_uint( &worker->hold, ATOMIC_ORDER_ACQUIRE ) != 0
  ) {
    /* Wait */
  }

  if ( worker->release_after_waiters != 0 ) {
    rtems_counter_delay_nanoseconds( 100000 );
  }

  _Atomic_Store_uint( &worker->released, 1, ATOMIC_ORDER_RELAXED );
  ( *ctx->release )( ctx, &worker->lock_context );
  _ISR_Local_enable( level );
}

void ISRLockStartWorker(
  ISRLockContext *ctx,
  ISRLockWorker  *worker,
  uint32_t        cpu_index,
  unsigned int    release_after_waiters,
  bool            hold
)
{
  worker->ctx = ctx;
  worker->cpu_index = cpu_index;
  worker->release_after_waiters = release_after_waiters;
  _Atomic_Init_uint( &worker->hold, hold ? 1 : 0 );
  _Atomic_Init_uint( &worker->released, 0 );
  worker->sequence = 0;
  worker->job_context.handler = ISRLockWorkerJob;
  worker->job_context.arg = worker;
  worker->job.context = &worker->job_context;
  _Per_CPU_Submit_job( _Per_CPU_Get_by_index( cpu_index ), &worker->job );
}

void ISRLockClearWorkerHold( ISRLockWorker *worker )
{
  _Atomic_Store_uint( &worker->hold, 0, ATOMIC_ORDER_RELEASE );
}

bool ISRLockWorkerReleased( const ISRLockWorker *worker )
{
  return _Atomic_Load_uint( &worker->released, ATOMIC_ORDER_RELAXED ) != 0;
}

void ISRLockWaitForWorker( ISRLockWorker *worker )
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  _Per_CPU_Wait_for_job(
    _Per_CPU_Get_by_index( worker->cpu_index ),
    &worker->job
  );
  _Thread_Dispatch_enable( cpu_self );
}
#endif
