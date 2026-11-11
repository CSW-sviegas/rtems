/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondDestroy
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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <rtems/posix/condimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondDestroy spec:/c/pthread/req/cond-destroy
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondDestroy_Pre_Cond_Obj,
  CPthreadReqCondDestroy_Pre_Cond_Null,
  CPthreadReqCondDestroy_Pre_Cond_NA
} CPthreadReqCondDestroy_Pre_Cond;

typedef enum {
  CPthreadReqCondDestroy_Pre_State_Initialized,
  CPthreadReqCondDestroy_Pre_State_Zeroed,
  CPthreadReqCondDestroy_Pre_State_Other,
  CPthreadReqCondDestroy_Pre_State_NA
} CPthreadReqCondDestroy_Pre_State;

typedef enum {
  CPthreadReqCondDestroy_Pre_Waiters_None,
  CPthreadReqCondDestroy_Pre_Waiters_One,
  CPthreadReqCondDestroy_Pre_Waiters_NA
} CPthreadReqCondDestroy_Pre_Waiters;

typedef enum {
  CPthreadReqCondDestroy_Post_Status_Ok,
  CPthreadReqCondDestroy_Post_Status_Busy,
  CPthreadReqCondDestroy_Post_Status_Inval,
  CPthreadReqCondDestroy_Post_Status_NA
} CPthreadReqCondDestroy_Post_Status;

typedef enum {
  CPthreadReqCondDestroy_Post_State_Destroyed,
  CPthreadReqCondDestroy_Post_State_Nop,
  CPthreadReqCondDestroy_Post_State_NA
} CPthreadReqCondDestroy_Post_State;

typedef enum {
  CPthreadReqCondDestroy_Post_Other_Nop,
  CPthreadReqCondDestroy_Post_Other_NA
} CPthreadReqCondDestroy_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Cond_NA : 1;
  uint16_t Pre_State_NA : 1;
  uint16_t Pre_Waiters_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_State : 2;
  uint16_t Post_Other : 1;
} CPthreadReqCondDestroy_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/cond-destroy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `cond` parameter value.
   */
  pthread_cond_t *cond;

  /**
   * @brief This member contains the return value of the pthread_cond_destroy()
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
   * @brief This member contains the mutex used by the worker.
   */
  pthread_mutex_t mutex_obj;

  /**
   * @brief This member contains the worker identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member is true, if the worker is blocked in the wait.
   */
  bool waiting;

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
    CPthreadReqCondDestroy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondDestroy_Context;

static CPthreadReqCondDestroy_Context CPthreadReqCondDestroy_Instance;

static const char *const CPthreadReqCondDestroy_PreDesc_Cond[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondDestroy_PreDesc_State[] =
  { "Initialized", "Zeroed", "Other", "NA" };

static const char *const CPthreadReqCondDestroy_PreDesc_Waiters[] =
  { "None", "One", "NA" };

static const char *const *const CPthreadReqCondDestroy_PreDesc[] = {
  CPthreadReqCondDestroy_PreDesc_Cond,
  CPthreadReqCondDestroy_PreDesc_State,
  CPthreadReqCondDestroy_PreDesc_Waiters,
  NULL
};

typedef CPthreadReqCondDestroy_Context Context;

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
  ctx->waiting = true;
  T_eno_success( pthread_cond_wait( &ctx->cond_obj, &ctx->mutex_obj ) );
  ctx->waiting = false;
  T_eno_success( pthread_mutex_unlock( &ctx->mutex_obj ) );

  SuspendSelf();
}

static void StartWorker( Context *ctx, bool start )
{
  if ( start ) {
    ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
    SetAffinityOne( ctx->worker_id, 0 );
    StartTask( ctx->worker_id, Worker, ctx );
    WaitForExecutionStop( ctx->worker_id );
  }

  T_eq_int( ctx->waiting, start );
}

static void CPthreadReqCondDestroy_Pre_Cond_Prepare(
  CPthreadReqCondDestroy_Context *ctx,
  CPthreadReqCondDestroy_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondDestroy_Pre_Cond_Obj: {
      /*
       * While the `cond` parameter references an object of type
       * pthread_cond_t.
       */
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondDestroy_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondDestroy_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Pre_State_Prepare(
  CPthreadReqCondDestroy_Context  *ctx,
  CPthreadReqCondDestroy_Pre_State state
)
{
  switch ( state ) {
    case CPthreadReqCondDestroy_Pre_State_Initialized: {
      /*
       * While the object referenced by the `cond` parameter is an initialized
       * condition variable.
       */
      T_eno_success( pthread_cond_init( &ctx->cond_obj, NULL ) );
      break;
    }

    case CPthreadReqCondDestroy_Pre_State_Zeroed: {
      /*
       * While the object referenced by the `cond` parameter is filled with
       * zero bytes.
       */
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondDestroy_Pre_State_Other: {
      /*
       * While the object referenced by the `cond` parameter is neither an
       * initialized condition variable nor filled with zero bytes.
       */
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondDestroy_Pre_State_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Pre_Waiters_Prepare(
  CPthreadReqCondDestroy_Context    *ctx,
  CPthreadReqCondDestroy_Pre_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqCondDestroy_Pre_Waiters_None: {
      /*
       * While no thread is blocked on the condition variable referenced by the
       * `cond` parameter.
       */
      StartWorker( ctx, false );
      break;
    }

    case CPthreadReqCondDestroy_Pre_Waiters_One: {
      /*
       * While exactly one thread is blocked on the condition variable
       * referenced by the `cond` parameter.
       */
      StartWorker( ctx, true );
      break;
    }

    case CPthreadReqCondDestroy_Pre_Waiters_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Post_Status_Check(
  CPthreadReqCondDestroy_Context    *ctx,
  CPthreadReqCondDestroy_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondDestroy_Post_Status_Ok: {
      /*
       * The return value of pthread_cond_destroy() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondDestroy_Post_Status_Busy: {
      /*
       * The return value of pthread_cond_destroy() shall be EBUSY.
       */
      T_eq_int( ctx->status, EBUSY );
      break;
    }

    case CPthreadReqCondDestroy_Post_Status_Inval: {
      /*
       * The return value of pthread_cond_destroy() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondDestroy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Post_State_Check(
  CPthreadReqCondDestroy_Context   *ctx,
  CPthreadReqCondDestroy_Post_State state
)
{
  switch ( state ) {
    case CPthreadReqCondDestroy_Post_State_Destroyed: {
      /*
       * The object referenced by the `cond` parameter shall be neither an
       * initialized condition variable nor filled with zero bytes.
       */
      T_eq_ulong( ctx->cond_obj._flags, ~InitializedFlags( &ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondDestroy_Post_State_Nop: {
      /*
       * Whether the object referenced by the `cond` parameter is an
       * initialized condition variable shall not be modified.
       */
      T_eq_ulong( ctx->cond_obj._flags, ctx->cond_before._flags );
      break;
    }

    case CPthreadReqCondDestroy_Post_State_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Post_Other_Check(
  CPthreadReqCondDestroy_Context   *ctx,
  CPthreadReqCondDestroy_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqCondDestroy_Post_Other_Nop: {
      /*
       * The content of the object referenced by the `cond` parameter, other
       * than whether it is an initialized condition variable and the lock
       * which protects it, shall not be modified.
       */
      pthread_cond_t expected;

      memcpy( &expected, &ctx->cond_before, sizeof( expected ) );
      expected._flags = ctx->cond_obj._flags;
      expected._Queue._Lock = ctx->cond_obj._Queue._Lock;
      T_eq_mem( &ctx->cond_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqCondDestroy_Post_Other_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Setup( CPthreadReqCondDestroy_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  SetSelfAffinityOne( 0 );
  T_eno_success( pthread_mutex_init( &ctx->mutex_obj, NULL ) );
}

static void CPthreadReqCondDestroy_Setup_Wrap( void *arg )
{
  CPthreadReqCondDestroy_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondDestroy_Setup( ctx );
}

static void CPthreadReqCondDestroy_Teardown(
  CPthreadReqCondDestroy_Context *ctx
)
{
  T_eno_success( pthread_mutex_destroy( &ctx->mutex_obj ) );
  RestoreRunnerPriority();
  SetSelfAffinityAll();
}

static void CPthreadReqCondDestroy_Teardown_Wrap( void *arg )
{
  CPthreadReqCondDestroy_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondDestroy_Teardown( ctx );
}

static void CPthreadReqCondDestroy_Prepare(
  CPthreadReqCondDestroy_Context *ctx
)
{
  ctx->worker_id = 0;
  ctx->waiting = false;
}

static void CPthreadReqCondDestroy_Action(
  CPthreadReqCondDestroy_Context *ctx
)
{
  memcpy( &ctx->cond_before, &ctx->cond_obj, sizeof( ctx->cond_before ) );
  ctx->status = pthread_cond_destroy( ctx->cond );
}

static void CPthreadReqCondDestroy_Cleanup(
  CPthreadReqCondDestroy_Context *ctx
)
{
  if ( ctx->worker_id != 0 ) {
    (void) pthread_cond_signal( &ctx->cond_obj );
    DeleteTask( ctx->worker_id );
  }
}

/* clang-format off */

static const CPthreadReqCondDestroy_Entry
CPthreadReqCondDestroy_Entries[] = {
  { 1, 0, 0, 0, CPthreadReqCondDestroy_Post_Status_NA,
    CPthreadReqCondDestroy_Post_State_NA, CPthreadReqCondDestroy_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqCondDestroy_Post_Status_Ok,
    CPthreadReqCondDestroy_Post_State_Destroyed,
    CPthreadReqCondDestroy_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqCondDestroy_Post_Status_Busy,
    CPthreadReqCondDestroy_Post_State_Nop,
    CPthreadReqCondDestroy_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqCondDestroy_Post_Status_Inval,
    CPthreadReqCondDestroy_Post_State_Nop,
    CPthreadReqCondDestroy_Post_Other_Nop },
  { 0, 0, 1, 1, CPthreadReqCondDestroy_Post_Status_Inval,
    CPthreadReqCondDestroy_Post_State_NA, CPthreadReqCondDestroy_Post_Other_NA }
};

static const uint8_t
CPthreadReqCondDestroy_Map[] = {
  1, 2, 1, 0, 3, 0, 4, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondDestroy_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondDestroy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqCondDestroy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqCondDestroy_Fixture = {
  .setup = CPthreadReqCondDestroy_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqCondDestroy_Teardown_Wrap,
  .scope = CPthreadReqCondDestroy_Scope,
  .initial_context = &CPthreadReqCondDestroy_Instance
};

static inline CPthreadReqCondDestroy_Entry CPthreadReqCondDestroy_PopEntry(
  CPthreadReqCondDestroy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondDestroy_Entries[ CPthreadReqCondDestroy_Map[ index ] ];
}

static void CPthreadReqCondDestroy_SetPreConditionStates(
  CPthreadReqCondDestroy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_State_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondDestroy_Pre_State_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Waiters_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondDestroy_Pre_Waiters_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CPthreadReqCondDestroy_TestVariant(
  CPthreadReqCondDestroy_Context *ctx
)
{
  CPthreadReqCondDestroy_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondDestroy_Pre_State_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondDestroy_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondDestroy_Action( ctx );
  CPthreadReqCondDestroy_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondDestroy_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  CPthreadReqCondDestroy_Post_Other_Check( ctx, ctx->Map.entry.Post_Other );
}

/**
 * @fn void T_case_body_CPthreadReqCondDestroy( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqCondDestroy, &CPthreadReqCondDestroy_Fixture )
{
  CPthreadReqCondDestroy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondDestroy_Pre_Cond_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondDestroy_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondDestroy_Pre_State_Initialized;
      ctx->Map.pci[ 1 ] < CPthreadReqCondDestroy_Pre_State_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondDestroy_Pre_Waiters_None;
        ctx->Map.pci[ 2 ] < CPthreadReqCondDestroy_Pre_Waiters_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqCondDestroy_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqCondDestroy_SetPreConditionStates( ctx );
        CPthreadReqCondDestroy_Prepare( ctx );
        CPthreadReqCondDestroy_TestVariant( ctx );
        CPthreadReqCondDestroy_Cleanup( ctx );
      }
    }
  }
}

/** @} */
