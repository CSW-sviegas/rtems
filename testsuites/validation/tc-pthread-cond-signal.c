/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondSignal
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <rtems.h>
#include <stdint.h>
#include <string.h>
#include <rtems/posix/condimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondSignal spec:/c/pthread/req/cond-signal
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondSignal_Pre_Cond_Obj,
  CPthreadReqCondSignal_Pre_Cond_Null,
  CPthreadReqCondSignal_Pre_Cond_NA
} CPthreadReqCondSignal_Pre_Cond;

typedef enum {
  CPthreadReqCondSignal_Pre_State_Initialized,
  CPthreadReqCondSignal_Pre_State_Zeroed,
  CPthreadReqCondSignal_Pre_State_Other,
  CPthreadReqCondSignal_Pre_State_NA
} CPthreadReqCondSignal_Pre_State;

typedef enum {
  CPthreadReqCondSignal_Pre_Waiters_None,
  CPthreadReqCondSignal_Pre_Waiters_One,
  CPthreadReqCondSignal_Pre_Waiters_Two,
  CPthreadReqCondSignal_Pre_Waiters_NA
} CPthreadReqCondSignal_Pre_Waiters;

typedef enum {
  CPthreadReqCondSignal_Post_Status_Ok,
  CPthreadReqCondSignal_Post_Status_Inval,
  CPthreadReqCondSignal_Post_Status_NA
} CPthreadReqCondSignal_Post_Status;

typedef enum {
  CPthreadReqCondSignal_Post_Unblock_One,
  CPthreadReqCondSignal_Post_Unblock_Nop,
  CPthreadReqCondSignal_Post_Unblock_NA
} CPthreadReqCondSignal_Post_Unblock;

typedef enum {
  CPthreadReqCondSignal_Post_State_Initialized,
  CPthreadReqCondSignal_Post_State_Nop,
  CPthreadReqCondSignal_Post_State_NA
} CPthreadReqCondSignal_Post_State;

typedef enum {
  CPthreadReqCondSignal_Post_Binding_None,
  CPthreadReqCondSignal_Post_Binding_Nop,
  CPthreadReqCondSignal_Post_Binding_NA
} CPthreadReqCondSignal_Post_Binding;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Cond_NA : 1;
  uint16_t Pre_State_NA : 1;
  uint16_t Pre_Waiters_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Unblock : 2;
  uint16_t Post_State : 2;
  uint16_t Post_Binding : 2;
} CPthreadReqCondSignal_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/cond-signal test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `cond` parameter value.
   */
  pthread_cond_t *cond;

  /**
   * @brief This member contains the return value of the pthread_cond_signal()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the condition variable object.
   */
  pthread_cond_t cond_obj;

  /**
   * @brief This member contains a copy of the condition variable object before
   *   the call.
   */
  pthread_cond_t cond_before;

  /**
   * @brief This member contains the mutex used by the workers.
   */
  pthread_mutex_t mutex_obj;

  /**
   * @brief This member contains the worker identifiers.
   */
  rtems_id worker_id[ 2 ];

  /**
   * @brief This member contains the count of workers blocked in the wait.
   */
  uint32_t waiting;

  /**
   * @brief This member contains the count of workers which returned from the
   *   wait.
   */
  uint32_t woken;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 3 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    CPthreadReqCondSignal_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondSignal_Context;

static CPthreadReqCondSignal_Context CPthreadReqCondSignal_Instance;

static const char *const CPthreadReqCondSignal_PreDesc_Cond[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondSignal_PreDesc_State[] =
  { "Initialized", "Zeroed", "Other", "NA" };

static const char *const CPthreadReqCondSignal_PreDesc_Waiters[] =
  { "None", "One", "Two", "NA" };

static const char *const *const CPthreadReqCondSignal_PreDesc[] = {
  CPthreadReqCondSignal_PreDesc_Cond,
  CPthreadReqCondSignal_PreDesc_State,
  CPthreadReqCondSignal_PreDesc_Waiters,
  NULL
};

typedef CPthreadReqCondSignal_Context Context;

static unsigned long InitializedFlags( const pthread_cond_t *cond )
{
  return ( (uintptr_t) cond ^ POSIX_CONDITION_VARIABLES_MAGIC ) &
         ~POSIX_CONDITION_VARIABLES_FLAGS_MASK;
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  T_eno_success( pthread_mutex_lock( &ctx->mutex_obj ) );
  ++ctx->waiting;
  T_eno_success( pthread_cond_wait( &ctx->cond_obj, &ctx->mutex_obj ) );
  --ctx->waiting;
  ++ctx->woken;
  T_eno_success( pthread_mutex_unlock( &ctx->mutex_obj ) );

  SuspendSelf();
}

static void StartWorkers( Context *ctx, uint32_t count )
{
  uint32_t i;

  for ( i = 0; i < count; ++i ) {
    rtems_id id;

    id = CreateTask( "WORK", PRIO_HIGH );
    ctx->worker_id[ i ] = id;
    SetAffinityOne( id, 0 );
    StartTask( id, Worker, ctx );
    WaitForExecutionStop( id );
  }

  T_eq_u32( ctx->waiting, count );
}

static void CPthreadReqCondSignal_Pre_Cond_Prepare(
  CPthreadReqCondSignal_Context *ctx,
  CPthreadReqCondSignal_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Pre_Cond_Obj: {
      /*
       * While the `cond` parameter references an object of type
       * pthread_cond_t.
       */
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondSignal_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondSignal_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Pre_State_Prepare(
  CPthreadReqCondSignal_Context  *ctx,
  CPthreadReqCondSignal_Pre_State state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Pre_State_Initialized: {
      /*
       * While the object referenced by the `cond` parameter is an initialized
       * condition variable.
       */
      T_eno_success( pthread_cond_init( &ctx->cond_obj, NULL ) );
      ctx->cond_obj._mutex = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondSignal_Pre_State_Zeroed: {
      /*
       * While the object referenced by the `cond` parameter is filled with
       * zero bytes.
       */
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondSignal_Pre_State_Other: {
      /*
       * While the object referenced by the `cond` parameter is neither an
       * initialized condition variable nor filled with zero bytes.
       */
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondSignal_Pre_State_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Pre_Waiters_Prepare(
  CPthreadReqCondSignal_Context    *ctx,
  CPthreadReqCondSignal_Pre_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Pre_Waiters_None: {
      /*
       * While no thread is blocked on the condition variable referenced by the
       * `cond` parameter.
       */
      StartWorkers( ctx, 0 );
      break;
    }

    case CPthreadReqCondSignal_Pre_Waiters_One: {
      /*
       * While exactly one thread is blocked on the condition variable
       * referenced by the `cond` parameter.
       */
      StartWorkers( ctx, 1 );
      break;
    }

    case CPthreadReqCondSignal_Pre_Waiters_Two: {
      /*
       * While two threads are blocked on the condition variable referenced by
       * the `cond` parameter.
       */
      StartWorkers( ctx, 2 );
      break;
    }

    case CPthreadReqCondSignal_Pre_Waiters_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Post_Status_Check(
  CPthreadReqCondSignal_Context    *ctx,
  CPthreadReqCondSignal_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Post_Status_Ok: {
      /*
       * The return value of pthread_cond_signal() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondSignal_Post_Status_Inval: {
      /*
       * The return value of pthread_cond_signal() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondSignal_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Post_Unblock_Check(
  CPthreadReqCondSignal_Context     *ctx,
  CPthreadReqCondSignal_Post_Unblock state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Post_Unblock_One: {
      /*
       * Exactly one of the threads blocked on the condition variable
       * referenced by the `cond` parameter shall be unblocked.
       */
      T_eq_u32( ctx->woken, 1 );
      break;
    }

    case CPthreadReqCondSignal_Post_Unblock_Nop: {
      /*
       * No thread shall be unblocked.
       */
      T_eq_u32( ctx->woken, 0 );
      break;
    }

    case CPthreadReqCondSignal_Post_Unblock_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Post_State_Check(
  CPthreadReqCondSignal_Context   *ctx,
  CPthreadReqCondSignal_Post_State state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Post_State_Initialized: {
      /*
       * The object referenced by the `cond` parameter shall be an initialized
       * condition variable with the condition variable clock attribute
       * CLOCK_REALTIME.
       */
      T_eq_ulong( ctx->cond_obj._flags, InitializedFlags( &ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondSignal_Post_State_Nop: {
      /*
       * Whether the object referenced by the `cond` parameter is an
       * initialized condition variable and its condition variable clock
       * attribute shall not be modified.
       */
      T_eq_ulong( ctx->cond_obj._flags, ctx->cond_before._flags );
      break;
    }

    case CPthreadReqCondSignal_Post_State_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Post_Binding_Check(
  CPthreadReqCondSignal_Context     *ctx,
  CPthreadReqCondSignal_Post_Binding state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Post_Binding_None: {
      /*
       * The condition variable referenced by the `cond` parameter shall be
       * bound to no mutex.
       */
      T_null( ctx->cond_obj._mutex );
      break;
    }

    case CPthreadReqCondSignal_Post_Binding_Nop: {
      /*
       * The binding of the condition variable referenced by the `cond`
       * parameter to a mutex shall not be modified.
       */
      T_eq_ptr( ctx->cond_obj._mutex, ctx->cond_before._mutex );
      break;
    }

    case CPthreadReqCondSignal_Post_Binding_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Setup( CPthreadReqCondSignal_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  SetSelfAffinityOne( 0 );
  T_eno_success( pthread_mutex_init( &ctx->mutex_obj, NULL ) );
}

static void CPthreadReqCondSignal_Setup_Wrap( void *arg )
{
  CPthreadReqCondSignal_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondSignal_Setup( ctx );
}

static void CPthreadReqCondSignal_Teardown(
  CPthreadReqCondSignal_Context *ctx
)
{
  T_eno_success( pthread_mutex_destroy( &ctx->mutex_obj ) );
  RestoreRunnerPriority();
  SetSelfAffinityAll();
}

static void CPthreadReqCondSignal_Teardown_Wrap( void *arg )
{
  CPthreadReqCondSignal_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondSignal_Teardown( ctx );
}

static void CPthreadReqCondSignal_Prepare( CPthreadReqCondSignal_Context *ctx )
{
  ctx->waiting = 0;
  ctx->woken = 0;
  ctx->worker_id[ 0 ] = 0;
  ctx->worker_id[ 1 ] = 0;
}

static void CPthreadReqCondSignal_Action( CPthreadReqCondSignal_Context *ctx )
{
  memcpy( &ctx->cond_before, &ctx->cond_obj, sizeof( ctx->cond_before ) );
  ctx->status = pthread_cond_signal( ctx->cond );
}

static void CPthreadReqCondSignal_Cleanup( CPthreadReqCondSignal_Context *ctx )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->worker_id ); ++i ) {
    if ( ctx->worker_id[ i ] != 0 ) {
      DeleteTask( ctx->worker_id[ i ] );
    }
  }

  (void) pthread_cond_destroy( &ctx->cond_obj );
}

/* clang-format off */

static const CPthreadReqCondSignal_Entry
CPthreadReqCondSignal_Entries[] = {
  { 1, 0, 0, 0, CPthreadReqCondSignal_Post_Status_NA,
    CPthreadReqCondSignal_Post_Unblock_NA, CPthreadReqCondSignal_Post_State_NA,
    CPthreadReqCondSignal_Post_Binding_NA },
  { 0, 0, 0, 0, CPthreadReqCondSignal_Post_Status_Ok,
    CPthreadReqCondSignal_Post_Unblock_One,
    CPthreadReqCondSignal_Post_State_Nop,
    CPthreadReqCondSignal_Post_Binding_Nop },
  { 0, 0, 0, 0, CPthreadReqCondSignal_Post_Status_Ok,
    CPthreadReqCondSignal_Post_Unblock_Nop,
    CPthreadReqCondSignal_Post_State_Nop,
    CPthreadReqCondSignal_Post_Binding_None },
  { 0, 0, 0, 0, CPthreadReqCondSignal_Post_Status_Ok,
    CPthreadReqCondSignal_Post_Unblock_Nop,
    CPthreadReqCondSignal_Post_State_Initialized,
    CPthreadReqCondSignal_Post_Binding_None },
  { 0, 0, 0, 0, CPthreadReqCondSignal_Post_Status_Inval,
    CPthreadReqCondSignal_Post_Unblock_Nop,
    CPthreadReqCondSignal_Post_State_Nop,
    CPthreadReqCondSignal_Post_Binding_Nop },
  { 0, 0, 1, 1, CPthreadReqCondSignal_Post_Status_Inval,
    CPthreadReqCondSignal_Post_Unblock_NA, CPthreadReqCondSignal_Post_State_NA,
    CPthreadReqCondSignal_Post_Binding_NA }
};

static const uint8_t
CPthreadReqCondSignal_Map[] = {
  2, 1, 1, 3, 0, 0, 4, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondSignal_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondSignal_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqCondSignal_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqCondSignal_Fixture = {
  .setup = CPthreadReqCondSignal_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqCondSignal_Teardown_Wrap,
  .scope = CPthreadReqCondSignal_Scope,
  .initial_context = &CPthreadReqCondSignal_Instance
};

static inline CPthreadReqCondSignal_Entry CPthreadReqCondSignal_PopEntry(
  CPthreadReqCondSignal_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondSignal_Entries[ CPthreadReqCondSignal_Map[ index ] ];
}

static void CPthreadReqCondSignal_SetPreConditionStates(
  CPthreadReqCondSignal_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_State_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondSignal_Pre_State_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Waiters_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondSignal_Pre_Waiters_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CPthreadReqCondSignal_TestVariant(
  CPthreadReqCondSignal_Context *ctx
)
{
  CPthreadReqCondSignal_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondSignal_Pre_State_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondSignal_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondSignal_Action( ctx );
  CPthreadReqCondSignal_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondSignal_Post_Unblock_Check( ctx, ctx->Map.entry.Post_Unblock );
  CPthreadReqCondSignal_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  CPthreadReqCondSignal_Post_Binding_Check( ctx, ctx->Map.entry.Post_Binding );
}

/**
 * @fn void T_case_body_CPthreadReqCondSignal( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqCondSignal, &CPthreadReqCondSignal_Fixture )
{
  CPthreadReqCondSignal_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondSignal_Pre_Cond_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondSignal_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondSignal_Pre_State_Initialized;
      ctx->Map.pci[ 1 ] < CPthreadReqCondSignal_Pre_State_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondSignal_Pre_Waiters_None;
        ctx->Map.pci[ 2 ] < CPthreadReqCondSignal_Pre_Waiters_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqCondSignal_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqCondSignal_SetPreConditionStates( ctx );
        CPthreadReqCondSignal_Prepare( ctx );
        CPthreadReqCondSignal_TestVariant( ctx );
        CPthreadReqCondSignal_Cleanup( ctx );
      }
    }
  }
}

/** @} */
