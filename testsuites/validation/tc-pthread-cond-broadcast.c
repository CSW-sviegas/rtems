/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondBroadcast
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
 * @defgroup CPthreadReqCondBroadcast spec:/c/pthread/req/cond-broadcast
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondBroadcast_Pre_Cond_Obj,
  CPthreadReqCondBroadcast_Pre_Cond_Null,
  CPthreadReqCondBroadcast_Pre_Cond_NA
} CPthreadReqCondBroadcast_Pre_Cond;

typedef enum {
  CPthreadReqCondBroadcast_Pre_State_Initialized,
  CPthreadReqCondBroadcast_Pre_State_Zeroed,
  CPthreadReqCondBroadcast_Pre_State_Other,
  CPthreadReqCondBroadcast_Pre_State_NA
} CPthreadReqCondBroadcast_Pre_State;

typedef enum {
  CPthreadReqCondBroadcast_Pre_Waiters_None,
  CPthreadReqCondBroadcast_Pre_Waiters_One,
  CPthreadReqCondBroadcast_Pre_Waiters_Two,
  CPthreadReqCondBroadcast_Pre_Waiters_NA
} CPthreadReqCondBroadcast_Pre_Waiters;

typedef enum {
  CPthreadReqCondBroadcast_Post_Status_Ok,
  CPthreadReqCondBroadcast_Post_Status_Inval,
  CPthreadReqCondBroadcast_Post_Status_NA
} CPthreadReqCondBroadcast_Post_Status;

typedef enum {
  CPthreadReqCondBroadcast_Post_Unblock_All,
  CPthreadReqCondBroadcast_Post_Unblock_Nop,
  CPthreadReqCondBroadcast_Post_Unblock_NA
} CPthreadReqCondBroadcast_Post_Unblock;

typedef enum {
  CPthreadReqCondBroadcast_Post_State_Initialized,
  CPthreadReqCondBroadcast_Post_State_Nop,
  CPthreadReqCondBroadcast_Post_State_NA
} CPthreadReqCondBroadcast_Post_State;

typedef enum {
  CPthreadReqCondBroadcast_Post_Binding_None,
  CPthreadReqCondBroadcast_Post_Binding_Nop,
  CPthreadReqCondBroadcast_Post_Binding_NA
} CPthreadReqCondBroadcast_Post_Binding;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Cond_NA : 1;
  uint16_t Pre_State_NA : 1;
  uint16_t Pre_Waiters_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Unblock : 2;
  uint16_t Post_State : 2;
  uint16_t Post_Binding : 2;
} CPthreadReqCondBroadcast_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/cond-broadcast test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `cond` parameter value.
   */
  pthread_cond_t *cond;

  /**
   * @brief This member contains the return value of the
   *   pthread_cond_broadcast() call.
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
    CPthreadReqCondBroadcast_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondBroadcast_Context;

static CPthreadReqCondBroadcast_Context CPthreadReqCondBroadcast_Instance;

static const char *const CPthreadReqCondBroadcast_PreDesc_Cond[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondBroadcast_PreDesc_State[] =
  { "Initialized", "Zeroed", "Other", "NA" };

static const char *const CPthreadReqCondBroadcast_PreDesc_Waiters[] =
  { "None", "One", "Two", "NA" };

static const char *const *const CPthreadReqCondBroadcast_PreDesc[] = {
  CPthreadReqCondBroadcast_PreDesc_Cond,
  CPthreadReqCondBroadcast_PreDesc_State,
  CPthreadReqCondBroadcast_PreDesc_Waiters,
  NULL
};

typedef CPthreadReqCondBroadcast_Context Context;

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

static void CPthreadReqCondBroadcast_Pre_Cond_Prepare(
  CPthreadReqCondBroadcast_Context *ctx,
  CPthreadReqCondBroadcast_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Pre_Cond_Obj: {
      /*
       * While the `cond` parameter references an object of type
       * pthread_cond_t.
       */
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Pre_State_Prepare(
  CPthreadReqCondBroadcast_Context  *ctx,
  CPthreadReqCondBroadcast_Pre_State state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Pre_State_Initialized: {
      /*
       * While the object referenced by the `cond` parameter is an initialized
       * condition variable.
       */
      T_eno_success( pthread_cond_init( &ctx->cond_obj, NULL ) );
      ctx->cond_obj._mutex = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondBroadcast_Pre_State_Zeroed: {
      /*
       * While the object referenced by the `cond` parameter is filled with
       * zero bytes.
       */
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_State_Other: {
      /*
       * While the object referenced by the `cond` parameter is neither an
       * initialized condition variable nor filled with zero bytes.
       */
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_State_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Pre_Waiters_Prepare(
  CPthreadReqCondBroadcast_Context    *ctx,
  CPthreadReqCondBroadcast_Pre_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Pre_Waiters_None: {
      /*
       * While no thread is blocked on the condition variable referenced by the
       * `cond` parameter.
       */
      StartWorkers( ctx, 0 );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Waiters_One: {
      /*
       * While exactly one thread is blocked on the condition variable
       * referenced by the `cond` parameter.
       */
      StartWorkers( ctx, 1 );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Waiters_Two: {
      /*
       * While two threads are blocked on the condition variable referenced by
       * the `cond` parameter.
       */
      StartWorkers( ctx, 2 );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Waiters_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Post_Status_Check(
  CPthreadReqCondBroadcast_Context    *ctx,
  CPthreadReqCondBroadcast_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Post_Status_Ok: {
      /*
       * The return value of pthread_cond_broadcast() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Status_Inval: {
      /*
       * The return value of pthread_cond_broadcast() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Post_Unblock_Check(
  CPthreadReqCondBroadcast_Context     *ctx,
  CPthreadReqCondBroadcast_Post_Unblock state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Post_Unblock_All: {
      /*
       * All threads blocked on the condition variable referenced by the `cond`
       * parameter shall be unblocked.
       */
      T_eq_u32( ctx->waiting, 0 );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Unblock_Nop: {
      /*
       * No thread shall be unblocked.
       */
      T_eq_u32( ctx->woken, 0 );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Unblock_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Post_State_Check(
  CPthreadReqCondBroadcast_Context   *ctx,
  CPthreadReqCondBroadcast_Post_State state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Post_State_Initialized: {
      /*
       * The object referenced by the `cond` parameter shall be an initialized
       * condition variable with the condition variable clock attribute
       * CLOCK_REALTIME.
       */
      T_eq_ulong( ctx->cond_obj._flags, InitializedFlags( &ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondBroadcast_Post_State_Nop: {
      /*
       * Whether the object referenced by the `cond` parameter is an
       * initialized condition variable and its condition variable clock
       * attribute shall not be modified.
       */
      T_eq_ulong( ctx->cond_obj._flags, ctx->cond_before._flags );
      break;
    }

    case CPthreadReqCondBroadcast_Post_State_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Post_Binding_Check(
  CPthreadReqCondBroadcast_Context     *ctx,
  CPthreadReqCondBroadcast_Post_Binding state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Post_Binding_None: {
      /*
       * The condition variable referenced by the `cond` parameter shall be
       * bound to no mutex.
       */
      T_null( ctx->cond_obj._mutex );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Binding_Nop: {
      /*
       * The binding of the condition variable referenced by the `cond`
       * parameter to a mutex shall not be modified.
       */
      T_eq_ptr( ctx->cond_obj._mutex, ctx->cond_before._mutex );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Binding_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Setup(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  SetSelfPriority( PRIO_NORMAL );
  SetSelfAffinityOne( 0 );
  T_eno_success( pthread_mutex_init( &ctx->mutex_obj, NULL ) );
}

static void CPthreadReqCondBroadcast_Setup_Wrap( void *arg )
{
  CPthreadReqCondBroadcast_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondBroadcast_Setup( ctx );
}

static void CPthreadReqCondBroadcast_Teardown(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  T_eno_success( pthread_mutex_destroy( &ctx->mutex_obj ) );
  RestoreRunnerPriority();
  SetSelfAffinityAll();
}

static void CPthreadReqCondBroadcast_Teardown_Wrap( void *arg )
{
  CPthreadReqCondBroadcast_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondBroadcast_Teardown( ctx );
}

static void CPthreadReqCondBroadcast_Prepare(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  ctx->waiting = 0;
  ctx->woken = 0;
  ctx->worker_id[ 0 ] = 0;
  ctx->worker_id[ 1 ] = 0;
}

static void CPthreadReqCondBroadcast_Action(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  memcpy( &ctx->cond_before, &ctx->cond_obj, sizeof( ctx->cond_before ) );
  ctx->status = pthread_cond_broadcast( ctx->cond );
}

static void CPthreadReqCondBroadcast_Cleanup(
  CPthreadReqCondBroadcast_Context *ctx
)
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

static const CPthreadReqCondBroadcast_Entry
CPthreadReqCondBroadcast_Entries[] = {
  { 1, 0, 0, 0, CPthreadReqCondBroadcast_Post_Status_NA,
    CPthreadReqCondBroadcast_Post_Unblock_NA,
    CPthreadReqCondBroadcast_Post_State_NA,
    CPthreadReqCondBroadcast_Post_Binding_NA },
  { 0, 0, 0, 0, CPthreadReqCondBroadcast_Post_Status_Ok,
    CPthreadReqCondBroadcast_Post_Unblock_All,
    CPthreadReqCondBroadcast_Post_State_Nop,
    CPthreadReqCondBroadcast_Post_Binding_None },
  { 0, 0, 0, 0, CPthreadReqCondBroadcast_Post_Status_Ok,
    CPthreadReqCondBroadcast_Post_Unblock_Nop,
    CPthreadReqCondBroadcast_Post_State_Nop,
    CPthreadReqCondBroadcast_Post_Binding_None },
  { 0, 0, 0, 0, CPthreadReqCondBroadcast_Post_Status_Ok,
    CPthreadReqCondBroadcast_Post_Unblock_Nop,
    CPthreadReqCondBroadcast_Post_State_Initialized,
    CPthreadReqCondBroadcast_Post_Binding_None },
  { 0, 0, 0, 0, CPthreadReqCondBroadcast_Post_Status_Inval,
    CPthreadReqCondBroadcast_Post_Unblock_Nop,
    CPthreadReqCondBroadcast_Post_State_Nop,
    CPthreadReqCondBroadcast_Post_Binding_Nop },
  { 0, 0, 1, 1, CPthreadReqCondBroadcast_Post_Status_Inval,
    CPthreadReqCondBroadcast_Post_Unblock_NA,
    CPthreadReqCondBroadcast_Post_State_NA,
    CPthreadReqCondBroadcast_Post_Binding_NA }
};

static const uint8_t
CPthreadReqCondBroadcast_Map[] = {
  2, 1, 1, 3, 0, 0, 4, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondBroadcast_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondBroadcast_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondBroadcast_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondBroadcast_Fixture = {
  .setup = CPthreadReqCondBroadcast_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqCondBroadcast_Teardown_Wrap,
  .scope = CPthreadReqCondBroadcast_Scope,
  .initial_context = &CPthreadReqCondBroadcast_Instance
};

static inline CPthreadReqCondBroadcast_Entry CPthreadReqCondBroadcast_PopEntry(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondBroadcast_Entries
    [ CPthreadReqCondBroadcast_Map[ index ] ];
}

static void CPthreadReqCondBroadcast_SetPreConditionStates(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_State_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondBroadcast_Pre_State_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Waiters_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondBroadcast_Pre_Waiters_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CPthreadReqCondBroadcast_TestVariant(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  CPthreadReqCondBroadcast_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondBroadcast_Pre_State_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondBroadcast_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondBroadcast_Action( ctx );
  CPthreadReqCondBroadcast_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondBroadcast_Post_Unblock_Check(
    ctx,
    ctx->Map.entry.Post_Unblock
  );
  CPthreadReqCondBroadcast_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  CPthreadReqCondBroadcast_Post_Binding_Check(
    ctx,
    ctx->Map.entry.Post_Binding
  );
}

/**
 * @fn void T_case_body_CPthreadReqCondBroadcast( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondBroadcast,
  &CPthreadReqCondBroadcast_Fixture
)
{
  CPthreadReqCondBroadcast_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondBroadcast_Pre_Cond_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondBroadcast_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondBroadcast_Pre_State_Initialized;
      ctx->Map.pci[ 1 ] < CPthreadReqCondBroadcast_Pre_State_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondBroadcast_Pre_Waiters_None;
        ctx->Map.pci[ 2 ] < CPthreadReqCondBroadcast_Pre_Waiters_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqCondBroadcast_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqCondBroadcast_SetPreConditionStates( ctx );
        CPthreadReqCondBroadcast_Prepare( ctx );
        CPthreadReqCondBroadcast_TestVariant( ctx );
        CPthreadReqCondBroadcast_Cleanup( ctx );
      }
    }
  }
}

/** @} */
