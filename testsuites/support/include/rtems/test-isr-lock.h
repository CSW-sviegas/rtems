/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the functions to test the ISR locks and
 *   the locks of the APIs which use the same score implementation.
 *
 * The test runs of the ISR lock score layer get an ISRLockContext.  The test
 * case of an API fills it with the lock operations of the API.  The lock
 * operations of the test case set ISRLockContext::restore and
 * ISRLockContext::method to the variant of the API.
 *
 * In SMP configurations, an ISRLockWorker acquires the lock on another
 * processor.  It runs as a per-processor job in interrupt context.
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

#ifndef _RTEMS_TEST_ISR_LOCK_H
#define _RTEMS_TEST_ISR_LOCK_H

#include <rtems/test-support.h>

#include <rtems.h>
#include <rtems/score/atomic.h>
#include <rtems/score/isrlevel.h>

#if defined( RTEMS_SMP )
#include <rtems/score/percpu.h>
#include <rtems/score/smplockticket.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSTestSuitesValidation
 *
 * @{
 */

/**
 * @brief This enumeration provides the places where an acquire saves the
 *   status of maskable interrupts.
 */
typedef enum {
  /**
   * @brief The acquire disables maskable interrupts and saves their previous
   *   status in the lock context.
   */
  ISR_LOCK_RESTORE_TO_CONTEXT,

  /**
   * @brief The acquire disables maskable interrupts and saves their previous
   *   status in the lock object.
   */
  ISR_LOCK_RESTORE_TO_OBJECT,

  /**
   * @brief The acquire does not change the status of maskable interrupts.
   */
  ISR_LOCK_RESTORE_NONE
} ISRLockRestore;

/**
 * @brief This enumeration provides the methods which initialize a lock.
 */
typedef enum {
  /**
   * @brief An initializer for static initialization initializes the lock.
   */
  ISR_LOCK_METHOD_INITIALIZER,

  /**
   * @brief A function initializes the lock.
   */
  ISR_LOCK_METHOD_FUNCTION
} ISRLockMethod;

typedef struct ISRLockContext ISRLockContext;

/**
 * @brief This type is a lock operation which uses a lock context.
 */
typedef void ( *ISRLockOperation )(
  ISRLockContext   *ctx,
  ISR_lock_Context *lock_context
);

/**
 * @brief This structure contains the lock under test and its operations.
 */
struct ISRLockContext {
  /**
   * @brief This member specifies where the acquire saves the status of
   *   maskable interrupts.
   */
  ISRLockRestore restore;

  /**
   * @brief This member specifies the method of ISRLockContext::initialize.
   */
  ISRLockMethod method;

  /**
   * @brief This member references the lock object.
   */
  void *lock;

  /**
   * @brief This member contains the size of the lock object in bytes.
   */
  size_t lock_size;

  /**
   * @brief This member initializes the lock with the method of
   *   ISRLockContext::method.
   */
  void ( *initialize )( ISRLockContext *ctx );

  /**
   * @brief This member destroys the lock.
   */
  void ( *destroy )( ISRLockContext *ctx );

  /**
   * @brief This member acquires the lock.
   */
  ISRLockOperation acquire;

  /**
   * @brief This member releases the lock which ISRLockContext::acquire
   *   acquired.
   */
  ISRLockOperation release;

  /**
   * @brief This member disables maskable interrupts and stores their previous
   *   status in the lock context.
   *
   * It is NULL, if ISRLockContext::disable_acquire_isr is NULL.
   */
  ISRLockOperation interrupt_disable;

  /**
   * @brief This member disables maskable interrupts and acquires the lock with
   *   two separate calls.
   *
   * It is NULL, if the API does not define a release for this pair.
   */
  ISRLockOperation disable_acquire_isr;

  /**
   * @brief This member restores the status of maskable interrupts which
   *   ISRLockContext::interrupt_disable or
   *   ISRLockContext::disable_acquire_isr saved.
   *
   * It is NULL, if ISRLockContext::disable_acquire_isr is NULL.
   */
  ISRLockOperation interrupt_enable;

  /**
   * @brief This member gets the status of maskable interrupts which the
   *   acquire saved.
   *
   * It is NULL for ISR_LOCK_RESTORE_NONE.
   */
  ISR_Level ( *get_saved_level )(
    const ISRLockContext   *ctx,
    const ISR_lock_Context *lock_context
  );

#if defined( RTEMS_SMP )
  /**
   * @brief This member gets the ticket lock which implements the lock.
   */
  const SMP_ticket_lock_Control *( *get_ticket_lock )(
    const ISRLockContext *ctx
  );
#endif

  /**
   * @brief This member contains the next sequence number of an acquire.
   *
   * An ISRLockWorker reads and increments it while it owns the lock.
   */
  unsigned int sequence;
};

/**
 * @brief Checks whether the lock is free.
 *
 * @param ctx is the lock context.
 *
 * @retval true No processor owns the lock.  This is always the case in
 *   uniprocessor configurations.
 * @retval false A processor owns the lock.
 */
bool ISRLockIsFree( const ISRLockContext *ctx );

/**
 * @brief Sets the lock operations of the Classic interrupt lock directives.
 *
 * The method of ISRLockContext::initialize is ISR_LOCK_METHOD_FUNCTION.
 *
 * @param[out] ctx is the lock context.
 *
 * @param lock is the lock under test.
 *
 * @param restore selects the directives.  For ISR_LOCK_RESTORE_TO_CONTEXT,
 *   the directives are rtems_interrupt_lock_acquire() and
 *   rtems_interrupt_lock_release().  For ISR_LOCK_RESTORE_NONE, they are
 *   rtems_interrupt_lock_acquire_isr() and rtems_interrupt_lock_release_isr().
 *   ISR_LOCK_RESTORE_TO_OBJECT is not allowed, because no Classic directive
 *   saves the status of maskable interrupts in the lock object.
 */
void ISRLockSetClassic(
  ISRLockContext       *ctx,
  rtems_interrupt_lock *lock,
  ISRLockRestore        restore
);

/**
 * @brief This structure contains the state of a worker which acquires the
 *   lock on another processor.
 */
typedef struct {
  /**
   * @brief This member references the lock context.
   */
  ISRLockContext *ctx;

  /**
   * @brief This member contains the index of the processor of the worker.
   */
  uint32_t cpu_index;

  /**
   * @brief This member contains the count of processors which shall wait for
   *   the lock before the worker releases it.
   */
  unsigned int release_after_waiters;

  /**
   * @brief While this member is non-zero, the worker does not release the
   *   lock.
   */
  Atomic_Uint hold;

  /**
   * @brief This member is non-zero, if the worker is about to release the
   *   lock.
   */
  Atomic_Uint released;

  /**
   * @brief This member contains the sequence number of the acquire of the
   *   worker.
   */
  unsigned int sequence;

  /**
   * @brief This member contains the lock context of the worker.
   */
  ISR_lock_Context lock_context;

#if defined( RTEMS_SMP )
  /**
   * @brief This member contains the job context of the worker.
   */
  Per_CPU_Job_context job_context;

  /**
   * @brief This member contains the job of the worker.
   */
  Per_CPU_Job job;
#endif
} ISRLockWorker;

#if defined( RTEMS_SMP )

/**
 * @brief Gets the online processors other than the current processor.
 *
 * The current processor is the processor of the caller at the call.  Call
 * this function from a runner which cannot migrate.
 *
 * @param[out] cpu_indices is the array which receives the processor indices.
 * @param count is the count of elements of the array.
 *
 * @return Returns the count of processor indices stored in the array.
 */
uint32_t ISRLockGetOtherProcessors( uint32_t *cpu_indices, uint32_t count );

/**
 * @brief Starts the worker.
 *
 * The worker acquires the lock on the processor.  It assigns the next
 * sequence number to ISRLockWorker::sequence.  It releases the lock when
 * ISRLockWorker::release_after_waiters processors wait for the lock and the
 * hold is cleared.  Where ISRLockWorker::release_after_waiters is not zero,
 * the worker holds the lock for 100 microseconds after the processors wait.
 * An acquire which does not wait for the release then returns while
 * ISRLockWorkerReleased() is false.
 *
 * @param ctx is the lock context.
 * @param worker is the worker.
 * @param cpu_index is the index of the processor of the worker.
 * @param release_after_waiters is the count of processors which shall wait for
 *   the lock before the worker releases it.
 * @param hold is true, if the worker shall hold the lock until
 *   ISRLockClearWorkerHold() is called.
 */
void ISRLockStartWorker(
  ISRLockContext *ctx,
  ISRLockWorker  *worker,
  uint32_t        cpu_index,
  unsigned int    release_after_waiters,
  bool            hold
);

/**
 * @brief Clears the hold of the worker.
 *
 * @param worker is the worker.
 */
void ISRLockClearWorkerHold( ISRLockWorker *worker );

/**
 * @brief Checks whether the worker is about to release the lock.
 *
 * @param worker is the worker.
 *
 * @retval true The worker is about to release the lock or released it.
 * @retval false Otherwise.
 */
bool ISRLockWorkerReleased( const ISRLockWorker *worker );

/**
 * @brief Waits until the job of the worker is done.
 *
 * Call this function with maskable interrupts enabled.
 *
 * @param worker is the worker.
 */
void ISRLockWaitForWorker( ISRLockWorker *worker );
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_TEST_ISR_LOCK_H */
