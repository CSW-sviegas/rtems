/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCancel
 */

/*
 * Copyright (C) 2026 Critical Software S.A.
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCancel spec:/c/pthread/req/cancel
 *
 * The worker thread is a Classic API task. Its termination is observed through
 * pthread_join() which returns the exit value of the worker thread.
 *
 * For the ExecutingFromISR pre-condition, pthread_cancel() is called within
 * interrupt context with the identifier of the interrupted thread. The
 * implementation then calls ``_Thread_Cancel()`` with the thread to cancel
 * being the executing thread. This violates the assertion ``_Assert(
 * the_thread != executing )`` in ``cpukit/score/src/threadrestart.c``, so this
 * variant fails in builds with ``RTEMS_DEBUG`` enabled. In builds without
 * ``RTEMS_DEBUG``, the cancellation request is processed according to the
 * cancelability of the interrupted thread.
 *
 * @{
 */

typedef enum {
  CPthreadReqCancel_Pre_Thread_NoObj,
  CPthreadReqCancel_Pre_Thread_Other,
  CPthreadReqCancel_Pre_Thread_Executing,
  CPthreadReqCancel_Pre_Thread_ExecutingFromISR,
  CPthreadReqCancel_Pre_Thread_NA
} CPthreadReqCancel_Pre_Thread;

typedef enum {
  CPthreadReqCancel_Pre_Cancelable_Async,
  CPthreadReqCancel_Pre_Cancelable_Deferred,
  CPthreadReqCancel_Pre_Cancelable_Disabled,
  CPthreadReqCancel_Pre_Cancelable_NA
} CPthreadReqCancel_Pre_Cancelable;

typedef enum {
  CPthreadReqCancel_Post_Status_Zero,
  CPthreadReqCancel_Post_Status_ESRCH,
  CPthreadReqCancel_Post_Status_NA
} CPthreadReqCancel_Post_Status;

typedef enum {
  CPthreadReqCancel_Post_Canceled_Yes,
  CPthreadReqCancel_Post_Canceled_Pending,
  CPthreadReqCancel_Post_Canceled_No,
  CPthreadReqCancel_Post_Canceled_NA
} CPthreadReqCancel_Post_Canceled;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Thread_NA : 1;
  uint8_t Pre_Cancelable_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Canceled : 2;
} CPthreadReqCancel_Entry;

typedef enum {
  KIND_NONE,
  KIND_OTHER,
  KIND_EXECUTING,
  KIND_EXECUTING_FROM_ISR
} WorkerKind;

/**
 * @brief Test context for spec:/c/pthread/req/cancel test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `thread` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member specifies what the worker thread does.
   */
  WorkerKind kind;

  /**
   * @brief This member specifies the thread cancel state attribute of the
   *   worker thread.
   */
  int cancel_state;

  /**
   * @brief This member specifies the thread cancel type attribute of the
   *   worker thread.
   */
  int cancel_type;

  /**
   * @brief This member contains the identifier of the worker thread.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the exit value obtained by joining the worker
   *   thread.
   */
  void *worker_value;

  /**
   * @brief This member is true, if the worker thread continued its execution
   *   after the cancellation request.
   */
  bool resumed;

  /**
   * @brief This member is true, if the worker thread continued its execution
   *   after the call to pthread_testcancel().
   */
  bool tested;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 2 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    CPthreadReqCancel_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCancel_Context;

static CPthreadReqCancel_Context
  CPthreadReqCancel_Instance;

static const char * const CPthreadReqCancel_PreDesc_Thread[] = {
  "NoObj",
  "Other",
  "Executing",
  "ExecutingFromISR",
  "NA"
};

static const char * const CPthreadReqCancel_PreDesc_Cancelable[] = {
  "Async",
  "Deferred",
  "Disabled",
  "NA"
};

static const char * const * const CPthreadReqCancel_PreDesc[] = {
  CPthreadReqCancel_PreDesc_Thread,
  CPthreadReqCancel_PreDesc_Cancelable,
  NULL
};

typedef CPthreadReqCancel_Context Context;

static void CancelFromISR( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->retval = pthread_cancel( pthread_self() );
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;
  int      eno;

  ctx = (Context *) arg;

  eno = pthread_setcanceltype( ctx->cancel_type, NULL );
  T_eno_success( eno );
  eno = pthread_setcancelstate( ctx->cancel_state, NULL );
  T_eno_success( eno );

  if ( ctx->kind == KIND_OTHER ) {
    SuspendSelf();
  } else if ( ctx->kind == KIND_EXECUTING ) {
    (void) pthread_cancel( pthread_self() );
  } else {
    CallWithinISR( CancelFromISR, ctx );
  }

  ctx->resumed = true;
  pthread_testcancel();
  ctx->tested = true;

  /* Accept the pending cancellation request */
  (void) pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
  pthread_testcancel();
  T_unreachable();
}

static void CPthreadReqCancel_Pre_Thread_Prepare(
  CPthreadReqCancel_Context   *ctx,
  CPthreadReqCancel_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_Thread_NoObj: {
      /*
       * While the `thread` parameter is not associated with a thread.
       */
      ctx->kind = KIND_NONE;
      ctx->thread = 0xffffffff;
      break;
    }

    case CPthreadReqCancel_Pre_Thread_Other: {
      /*
       * While the `thread` parameter is associated with a thread other than
       * the calling thread.
       */
      ctx->kind = KIND_OTHER;
      break;
    }

    case CPthreadReqCancel_Pre_Thread_Executing: {
      /*
       * While the `thread` parameter is associated with the calling thread,
       * while the calling thread is not in interrupt context.
       */
      ctx->kind = KIND_EXECUTING;
      break;
    }

    case CPthreadReqCancel_Pre_Thread_ExecutingFromISR: {
      /*
       * While the `thread` parameter is associated with the calling thread,
       * while the calling thread is in interrupt context.
       */
      ctx->kind = KIND_EXECUTING_FROM_ISR;
      break;
    }

    case CPthreadReqCancel_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqCancel_Pre_Cancelable_Prepare(
  CPthreadReqCancel_Context       *ctx,
  CPthreadReqCancel_Pre_Cancelable state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_Cancelable_Async: {
      /*
       * While the thread cancel state attribute of the thread specified by the
       * `thread` parameter is PTHREAD_CANCEL_ENABLE, while the thread cancel
       * type attribute of the thread is PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->cancel_state = PTHREAD_CANCEL_ENABLE;
      ctx->cancel_type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqCancel_Pre_Cancelable_Deferred: {
      /*
       * While the thread cancel state attribute of the thread specified by the
       * `thread` parameter is PTHREAD_CANCEL_ENABLE, while the thread cancel
       * type attribute of the thread is PTHREAD_CANCEL_DEFERRED.
       */
      ctx->cancel_state = PTHREAD_CANCEL_ENABLE;
      ctx->cancel_type = PTHREAD_CANCEL_DEFERRED;
      break;
    }

    case CPthreadReqCancel_Pre_Cancelable_Disabled: {
      /*
       * While the thread cancel state attribute of the thread specified by the
       * `thread` parameter is PTHREAD_CANCEL_DISABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_DISABLE;
      ctx->cancel_type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqCancel_Pre_Cancelable_NA:
      break;
  }

  if ( ctx->kind != KIND_NONE ) {
    ctx->worker_id = CreateTask( "WORK", GetSelfPriority() );
    StartTask( ctx->worker_id, Worker, ctx );
  }

  if ( ctx->kind == KIND_OTHER ) {
    ctx->thread = ctx->worker_id;

    /* Let the worker set its cancelability and block itself */
    Yield();
  }
}

static void CPthreadReqCancel_Post_Status_Check(
  CPthreadReqCancel_Context    *ctx,
  CPthreadReqCancel_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_Status_Zero: {
      /*
       * The return value of pthread_cancel() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCancel_Post_Status_ESRCH: {
      /*
       * The return value of pthread_cancel() shall be equal to ESRCH.
       */
      T_eq_int( ctx->retval, ESRCH );
      break;
    }

    case CPthreadReqCancel_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_Canceled_Check(
  CPthreadReqCancel_Context      *ctx,
  CPthreadReqCancel_Post_Canceled state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_Canceled_Yes: {
      /*
       * The thread specified by the `thread` parameter shall terminate with
       * the exit value PTHREAD_CANCELED without continuing its execution after
       * the cancellation request.
       */
      T_eq_ptr( ctx->worker_value, PTHREAD_CANCELED );
      T_false( ctx->resumed );
      break;
    }

    case CPthreadReqCancel_Post_Canceled_Pending: {
      /*
       * The thread specified by the `thread` parameter shall continue its
       * execution after the cancellation request and terminate with the exit
       * value PTHREAD_CANCELED when it calls pthread_testcancel().
       */
      T_eq_ptr( ctx->worker_value, PTHREAD_CANCELED );
      T_true( ctx->resumed );
      T_false( ctx->tested );
      break;
    }

    case CPthreadReqCancel_Post_Canceled_No: {
      /*
       * The thread specified by the `thread` parameter shall continue its
       * execution after a call to pthread_testcancel().
       */
      T_true( ctx->resumed );
      T_true( ctx->tested );
      break;
    }

    case CPthreadReqCancel_Post_Canceled_NA:
      break;
  }
}

static void CPthreadReqCancel_Prepare( CPthreadReqCancel_Context *ctx )
{
  ctx->retval = -1;
  ctx->worker_id = 0;
  ctx->worker_value = NULL;
  ctx->resumed = false;
  ctx->tested = false;
}

static void CPthreadReqCancel_Action( CPthreadReqCancel_Context *ctx )
{
  if ( ctx->kind == KIND_NONE || ctx->kind == KIND_OTHER ) {
    ctx->retval = pthread_cancel( ctx->thread );
  }

  if ( ctx->worker_id != 0 ) {
    int eno;

    eno = pthread_join( ctx->worker_id, &ctx->worker_value );
    T_eno_success( eno );
  }
}

static const CPthreadReqCancel_Entry
CPthreadReqCancel_Entries[] = {
  { 0, 0, 1, CPthreadReqCancel_Post_Status_ESRCH,
    CPthreadReqCancel_Post_Canceled_NA },
  { 0, 0, 0, CPthreadReqCancel_Post_Status_NA,
    CPthreadReqCancel_Post_Canceled_Yes },
  { 0, 0, 0, CPthreadReqCancel_Post_Status_Zero,
    CPthreadReqCancel_Post_Canceled_Yes },
  { 0, 0, 0, CPthreadReqCancel_Post_Status_Zero,
    CPthreadReqCancel_Post_Canceled_Pending },
  { 0, 0, 0, CPthreadReqCancel_Post_Status_Zero,
    CPthreadReqCancel_Post_Canceled_No }
};

static const uint8_t
CPthreadReqCancel_Map[] = {
  0, 0, 0, 2, 3, 4, 1, 1, 1, 2, 3, 4
};

static size_t CPthreadReqCancel_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCancel_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqCancel_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqCancel_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCancel_Scope,
  .initial_context = &CPthreadReqCancel_Instance
};

static inline CPthreadReqCancel_Entry CPthreadReqCancel_PopEntry(
  CPthreadReqCancel_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCancel_Entries[
    CPthreadReqCancel_Map[ index ]
  ];
}

static void CPthreadReqCancel_SetPreConditionStates(
  CPthreadReqCancel_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Cancelable_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCancel_Pre_Cancelable_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CPthreadReqCancel_TestVariant( CPthreadReqCancel_Context *ctx )
{
  CPthreadReqCancel_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCancel_Pre_Cancelable_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCancel_Action( ctx );
  CPthreadReqCancel_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCancel_Post_Canceled_Check( ctx, ctx->Map.entry.Post_Canceled );
}

/**
 * @fn void T_case_body_CPthreadReqCancel( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqCancel, &CPthreadReqCancel_Fixture )
{
  CPthreadReqCancel_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCancel_Pre_Thread_NoObj;
    ctx->Map.pci[ 0 ] < CPthreadReqCancel_Pre_Thread_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCancel_Pre_Cancelable_Async;
      ctx->Map.pci[ 1 ] < CPthreadReqCancel_Pre_Cancelable_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCancel_PopEntry( ctx );
      CPthreadReqCancel_SetPreConditionStates( ctx );
      CPthreadReqCancel_Prepare( ctx );
      CPthreadReqCancel_TestVariant( ctx );
    }
  }
}

/** @} */
