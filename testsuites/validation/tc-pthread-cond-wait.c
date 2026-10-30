/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondWait
 */

/*
 * Copyright (C) 2026 Critical Software S.A.
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

#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondWait spec:/c/pthread/req/cond-wait
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondWait_Pre_Cond_Null,
  CPthreadReqCondWait_Pre_Cond_Invalid,
  CPthreadReqCondWait_Pre_Cond_Zeroed,
  CPthreadReqCondWait_Pre_Cond_Valid,
  CPthreadReqCondWait_Pre_Cond_NA
} CPthreadReqCondWait_Pre_Cond;

typedef enum {
  CPthreadReqCondWait_Pre_Waiters_No,
  CPthreadReqCondWait_Pre_Waiters_OtherMutex,
  CPthreadReqCondWait_Pre_Waiters_SameMutex,
  CPthreadReqCondWait_Pre_Waiters_NA
} CPthreadReqCondWait_Pre_Waiters;

typedef enum {
  CPthreadReqCondWait_Pre_Mutex_Null,
  CPthreadReqCondWait_Pre_Mutex_Invalid,
  CPthreadReqCondWait_Pre_Mutex_Unlocked,
  CPthreadReqCondWait_Pre_Mutex_Owned,
  CPthreadReqCondWait_Pre_Mutex_NA
} CPthreadReqCondWait_Pre_Mutex;

typedef enum {
  CPthreadReqCondWait_Pre_Wakeup_Broadcast,
  CPthreadReqCondWait_Pre_Wakeup_MutexDestroyed,
  CPthreadReqCondWait_Pre_Wakeup_NA
} CPthreadReqCondWait_Pre_Wakeup;

typedef enum {
  CPthreadReqCondWait_Post_Status_Zero,
  CPthreadReqCondWait_Post_Status_EINVAL,
  CPthreadReqCondWait_Post_Status_EPERM,
  CPthreadReqCondWait_Post_Status_NA
} CPthreadReqCondWait_Post_Status;

typedef enum {
  CPthreadReqCondWait_Post_Owner_Caller,
  CPthreadReqCondWait_Post_Owner_Nobody,
  CPthreadReqCondWait_Post_Owner_NA
} CPthreadReqCondWait_Post_Owner;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Cond_NA : 1;
  uint16_t Pre_Waiters_NA : 1;
  uint16_t Pre_Mutex_NA : 1;
  uint16_t Pre_Wakeup_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Owner : 2;
} CPthreadReqCondWait_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/cond-wait test case.
 */
typedef struct {
  /**
   * @brief This member provides the condition variable object.
   */
  pthread_cond_t cond_obj;

  /**
   * @brief This member provides the mutex object used by the calling thread.
   */
  pthread_mutex_t mutex_obj;

  /**
   * @brief This member provides the mutex object used by the worker.
   */
  pthread_mutex_t other_mutex_obj;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member references the mutex object used by the worker.
   */
  pthread_mutex_t *worker_mutex;

  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the identifier of the waker task.
   */
  rtems_id waker_id;

  /**
   * @brief This member indicates if the waker task destroys the mutex object
   *   before it signals the condition variable object.
   */
  bool destroy_mutex;

  /**
   * @brief This member specifies the `cond` parameter value.
   */
  pthread_cond_t *cond;

  /**
   * @brief This member specifies the `mutex` parameter value.
   */
  pthread_mutex_t *mutex;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    CPthreadReqCondWait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondWait_Context;

static CPthreadReqCondWait_Context CPthreadReqCondWait_Instance;

static const char *const CPthreadReqCondWait_PreDesc_Cond[] =
  { "Null", "Invalid", "Zeroed", "Valid", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_Waiters[] =
  { "No", "OtherMutex", "SameMutex", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_Mutex[] =
  { "Null", "Invalid", "Unlocked", "Owned", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_Wakeup[] =
  { "Broadcast", "MutexDestroyed", "NA" };

static const char *const *const CPthreadReqCondWait_PreDesc[] = {
  CPthreadReqCondWait_PreDesc_Cond,
  CPthreadReqCondWait_PreDesc_Waiters,
  CPthreadReqCondWait_PreDesc_Mutex,
  CPthreadReqCondWait_PreDesc_Wakeup,
  NULL
};

typedef CPthreadReqCondWait_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;
  int      eno;

  ctx = (Context *) arg;

  eno = pthread_mutex_lock( ctx->worker_mutex );
  T_eq_int( eno, 0 );

  (void) pthread_cond_wait( &ctx->cond_obj, ctx->worker_mutex );
  (void) pthread_mutex_unlock( ctx->worker_mutex );

  SuspendSelf();
}

static void Waker( rtems_task_argument arg )
{
  Context        *ctx;
  Thread_Control *runner;
  int             eno;

  ctx = (Context *) arg;
  runner = GetThread( ctx->runner_id );

  while (
    ( runner->current_state & STATES_WAITING_FOR_CONDITION_VARIABLE ) == 0
  ) {
    /* Wait */
  }

  if ( ctx->destroy_mutex ) {
    do {
      eno = pthread_mutex_destroy( &ctx->mutex_obj );
    } while ( eno == EBUSY );

    T_eq_int( eno, 0 );
  }

  eno = pthread_cond_broadcast( &ctx->cond_obj );
  T_eq_int( eno, 0 );

  SuspendSelf();
}

static void CPthreadReqCondWait_Pre_Cond_Prepare(
  CPthreadReqCondWait_Context *ctx,
  CPthreadReqCondWait_Pre_Cond state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqCondWait_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondWait_Pre_Cond_Invalid: {
      /*
       * While the `cond` parameter references an object which is neither
       * initialized nor filled with zero bytes.
       */
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Cond_Zeroed: {
      /*
       * While the `cond` parameter references an object filled with zero
       * bytes.
       */
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Cond_Valid: {
      /*
       * While the `cond` parameter references an initialized condition
       * variable object.
       */
      eno = pthread_cond_init( &ctx->cond_obj, NULL );
      T_eq_int( eno, 0 );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_Waiters_Prepare(
  CPthreadReqCondWait_Context    *ctx,
  CPthreadReqCondWait_Pre_Waiters state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqCondWait_Pre_Waiters_No: {
      /*
       * While no thread waits on the condition variable object.
       */
      /* Nothing to prepare */
      break;
    }

    case CPthreadReqCondWait_Pre_Waiters_OtherMutex: {
      /*
       * While a thread waits on the condition variable object using a mutex
       * other than the one referenced by the `mutex` parameter.
       */
      ctx->worker_mutex = &ctx->other_mutex_obj;
      ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
      StartTask( ctx->worker_id, Worker, ctx );
      WaitForExecutionStop( ctx->worker_id );
      break;
    }

    case CPthreadReqCondWait_Pre_Waiters_SameMutex: {
      /*
       * While a thread waits on the condition variable object using the mutex
       * referenced by the `mutex` parameter.
       */
      eno = pthread_mutex_init( &ctx->mutex_obj, NULL );
      T_eq_int( eno, 0 );
      ctx->worker_mutex = &ctx->mutex_obj;
      ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
      StartTask( ctx->worker_id, Worker, ctx );
      WaitForExecutionStop( ctx->worker_id );
      break;
    }

    case CPthreadReqCondWait_Pre_Waiters_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_Mutex_Prepare(
  CPthreadReqCondWait_Context  *ctx,
  CPthreadReqCondWait_Pre_Mutex state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqCondWait_Pre_Mutex_Null: {
      /*
       * While the `mutex` parameter is NULL.
       */
      ctx->mutex = NULL;
      break;
    }

    case CPthreadReqCondWait_Pre_Mutex_Invalid: {
      /*
       * While the `mutex` parameter references an object which is neither
       * initialized nor filled with zero bytes.
       */
      memset( &ctx->mutex_obj, 0xff, sizeof( ctx->mutex_obj ) );
      ctx->mutex = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Mutex_Unlocked: {
      /*
       * While the `mutex` parameter references an initialized mutex object
       * which is not owned by any thread.
       */
      eno = pthread_mutex_init( &ctx->mutex_obj, NULL );
      T_eq_int( eno, 0 );
      ctx->mutex = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Mutex_Owned: {
      /*
       * While the `mutex` parameter references an initialized mutex object
       * which is owned by the calling thread.
       */
      eno = pthread_mutex_init( &ctx->mutex_obj, NULL );
      T_eq_int( eno, 0 );
      eno = pthread_mutex_lock( &ctx->mutex_obj );
      T_eq_int( eno, 0 );
      ctx->mutex = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Mutex_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_Wakeup_Prepare(
  CPthreadReqCondWait_Context   *ctx,
  CPthreadReqCondWait_Pre_Wakeup state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Pre_Wakeup_Broadcast: {
      /*
       * While the condition variable object is broadcast after the calling
       * thread started to wait.
       */
      ctx->waker_id = CreateTask( "WAKE", PRIO_LOW );
      StartTask( ctx->waker_id, Waker, ctx );
      break;
    }

    case CPthreadReqCondWait_Pre_Wakeup_MutexDestroyed: {
      /*
       * While the mutex object is destroyed and then the condition variable
       * object is broadcast after the calling thread started to wait.
       */
      ctx->destroy_mutex = true;
      ctx->waker_id = CreateTask( "WAKE", PRIO_LOW );
      StartTask( ctx->waker_id, Waker, ctx );
      break;
    }

    case CPthreadReqCondWait_Pre_Wakeup_NA:
      break;
  }
}

static void CPthreadReqCondWait_Post_Status_Check(
  CPthreadReqCondWait_Context    *ctx,
  CPthreadReqCondWait_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Post_Status_Zero: {
      /*
       * The return value of pthread_cond_wait() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondWait_Post_Status_EINVAL: {
      /*
       * The return value of pthread_cond_wait() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondWait_Post_Status_EPERM: {
      /*
       * The return value of pthread_cond_wait() shall be equal to EPERM.
       */
      T_eq_int( ctx->retval, EPERM );
      break;
    }

    case CPthreadReqCondWait_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondWait_Post_Owner_Check(
  CPthreadReqCondWait_Context   *ctx,
  CPthreadReqCondWait_Post_Owner state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqCondWait_Post_Owner_Caller: {
      /*
       * The mutex object shall be owned by the calling thread.
       */
      eno = pthread_mutex_unlock( ctx->mutex );
      T_eq_int( eno, 0 );
      break;
    }

    case CPthreadReqCondWait_Post_Owner_Nobody: {
      /*
       * The mutex object shall not be owned by any thread.
       */
      eno = pthread_mutex_unlock( ctx->mutex );
      T_eq_int( eno, EPERM );
      break;
    }

    case CPthreadReqCondWait_Post_Owner_NA:
      break;
  }
}

static void CPthreadReqCondWait_Setup( CPthreadReqCondWait_Context *ctx )
{
  int eno;

  memset( ctx, 0, sizeof( *ctx ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->runner_id = rtems_task_self();
  eno = pthread_mutex_init( &ctx->other_mutex_obj, NULL );
  T_eq_int( eno, 0 );
}

static void CPthreadReqCondWait_Setup_Wrap( void *arg )
{
  CPthreadReqCondWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondWait_Setup( ctx );
}

static void CPthreadReqCondWait_Teardown( CPthreadReqCondWait_Context *ctx )
{
  (void) pthread_mutex_destroy( &ctx->other_mutex_obj );
  RestoreRunnerPriority();
}

static void CPthreadReqCondWait_Teardown_Wrap( void *arg )
{
  CPthreadReqCondWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondWait_Teardown( ctx );
}

static void CPthreadReqCondWait_Prepare( CPthreadReqCondWait_Context *ctx )
{
  ctx->cond = NULL;
  ctx->mutex = NULL;
  ctx->worker_mutex = NULL;
  ctx->worker_id = 0;
  ctx->waker_id = 0;
  ctx->destroy_mutex = false;
  ctx->retval = -1;
}

static void CPthreadReqCondWait_Action( CPthreadReqCondWait_Context *ctx )
{
  ctx->retval = pthread_cond_wait( ctx->cond, ctx->mutex );
}

static void CPthreadReqCondWait_Cleanup( CPthreadReqCondWait_Context *ctx )
{
  if ( ctx->worker_id != 0 ) {
    DeleteTask( ctx->worker_id );
  }

  if ( ctx->waker_id != 0 ) {
    DeleteTask( ctx->waker_id );
  }

  (void) pthread_cond_destroy( &ctx->cond_obj );
  (void) pthread_mutex_destroy( &ctx->mutex_obj );
}

/* clang-format off */

static const CPthreadReqCondWait_Entry
CPthreadReqCondWait_Entries[] = {
  { 1, 0, 0, 0, 0, CPthreadReqCondWait_Post_Status_NA,
    CPthreadReqCondWait_Post_Owner_NA },
  { 0, 0, 1, 0, 1, CPthreadReqCondWait_Post_Status_EPERM,
    CPthreadReqCondWait_Post_Owner_NA },
  { 0, 0, 1, 1, 1, CPthreadReqCondWait_Post_Status_EINVAL,
    CPthreadReqCondWait_Post_Owner_NA },
  { 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_EPERM,
    CPthreadReqCondWait_Post_Owner_NA },
  { 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_EPERM,
    CPthreadReqCondWait_Post_Owner_Nobody },
  { 0, 0, 0, 0, 0, CPthreadReqCondWait_Post_Status_Zero,
    CPthreadReqCondWait_Post_Owner_Caller },
  { 0, 0, 0, 0, 0, CPthreadReqCondWait_Post_Status_EINVAL,
    CPthreadReqCondWait_Post_Owner_NA },
  { 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_EINVAL,
    CPthreadReqCondWait_Post_Owner_NA },
  { 0, 0, 1, 0, 1, CPthreadReqCondWait_Post_Status_EPERM,
    CPthreadReqCondWait_Post_Owner_Nobody },
  { 0, 0, 1, 0, 0, CPthreadReqCondWait_Post_Status_Zero,
    CPthreadReqCondWait_Post_Owner_Caller },
  { 0, 0, 1, 0, 0, CPthreadReqCondWait_Post_Status_EINVAL,
    CPthreadReqCondWait_Post_Owner_NA },
  { 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_EINVAL,
    CPthreadReqCondWait_Post_Owner_Nobody },
  { 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_EINVAL,
    CPthreadReqCondWait_Post_Owner_Caller }
};

static const uint8_t
CPthreadReqCondWait_Map[] = {
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,
  8, 0, 9, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 4,
  0, 5, 6, 7, 0, 7, 0, 11, 0, 12, 0, 1, 0, 1, 0, 4, 0, 5, 6
};

/* clang-format on */

static size_t CPthreadReqCondWait_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondWait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqCondWait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqCondWait_Fixture = {
  .setup = CPthreadReqCondWait_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqCondWait_Teardown_Wrap,
  .scope = CPthreadReqCondWait_Scope,
  .initial_context = &CPthreadReqCondWait_Instance
};

static inline CPthreadReqCondWait_Entry CPthreadReqCondWait_PopEntry(
  CPthreadReqCondWait_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondWait_Entries[ CPthreadReqCondWait_Map[ index ] ];
}

static void CPthreadReqCondWait_SetPreConditionStates(
  CPthreadReqCondWait_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Waiters_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondWait_Pre_Waiters_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Mutex_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondWait_Pre_Mutex_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Wakeup_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqCondWait_Pre_Wakeup_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CPthreadReqCondWait_TestVariant( CPthreadReqCondWait_Context *ctx )
{
  CPthreadReqCondWait_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondWait_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondWait_Pre_Mutex_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondWait_Pre_Wakeup_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqCondWait_Action( ctx );
  CPthreadReqCondWait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondWait_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
}

/**
 * @fn void T_case_body_CPthreadReqCondWait( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqCondWait, &CPthreadReqCondWait_Fixture )
{
  CPthreadReqCondWait_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondWait_Pre_Cond_Null;
    ctx->Map.pci[ 0 ] < CPthreadReqCondWait_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondWait_Pre_Waiters_No;
      ctx->Map.pci[ 1 ] < CPthreadReqCondWait_Pre_Waiters_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondWait_Pre_Mutex_Null;
        ctx->Map.pci[ 2 ] < CPthreadReqCondWait_Pre_Mutex_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqCondWait_Pre_Wakeup_Broadcast;
          ctx->Map.pci[ 3 ] < CPthreadReqCondWait_Pre_Wakeup_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqCondWait_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CPthreadReqCondWait_SetPreConditionStates( ctx );
          CPthreadReqCondWait_Prepare( ctx );
          CPthreadReqCondWait_TestVariant( ctx );
          CPthreadReqCondWait_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
