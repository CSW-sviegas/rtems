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
#include <rtems.h>
#include <string.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondSignal spec:/c/pthread/req/cond-signal
 *
 * @{
 */

typedef enum {
  CPthreadReqCondSignal_Pre_Cond_Valid,
  CPthreadReqCondSignal_Pre_Cond_Zeroed,
  CPthreadReqCondSignal_Pre_Cond_Invalid,
  CPthreadReqCondSignal_Pre_Cond_Null,
  CPthreadReqCondSignal_Pre_Cond_NA
} CPthreadReqCondSignal_Pre_Cond;

typedef enum {
  CPthreadReqCondSignal_Pre_Waiters_Yes,
  CPthreadReqCondSignal_Pre_Waiters_No,
  CPthreadReqCondSignal_Pre_Waiters_NA
} CPthreadReqCondSignal_Pre_Waiters;

typedef enum {
  CPthreadReqCondSignal_Post_Status_Zero,
  CPthreadReqCondSignal_Post_Status_EINVAL,
  CPthreadReqCondSignal_Post_Status_NA
} CPthreadReqCondSignal_Post_Status;

typedef enum {
  CPthreadReqCondSignal_Post_Threads_One,
  CPthreadReqCondSignal_Post_Threads_Nop,
  CPthreadReqCondSignal_Post_Threads_NA
} CPthreadReqCondSignal_Post_Threads;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Cond_NA : 1;
  uint8_t Pre_Waiters_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Threads : 2;
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
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the condition variable object.
   */
  pthread_cond_t cond_obj;

  /**
   * @brief This member contains the mutex used by the workers.
   */
  pthread_mutex_t mutex;

  /**
   * @brief This member contains the worker identifiers.
   */
  rtems_id worker_id[ 2 ];

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
    CPthreadReqCondSignal_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondSignal_Context;

static CPthreadReqCondSignal_Context
  CPthreadReqCondSignal_Instance;

static const char * const CPthreadReqCondSignal_PreDesc_Cond[] = {
  "Valid",
  "Zeroed",
  "Invalid",
  "Null",
  "NA"
};

static const char * const CPthreadReqCondSignal_PreDesc_Waiters[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const CPthreadReqCondSignal_PreDesc[] = {
  CPthreadReqCondSignal_PreDesc_Cond,
  CPthreadReqCondSignal_PreDesc_Waiters,
  NULL
};

typedef CPthreadReqCondSignal_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  T_eq_int( pthread_mutex_lock( &ctx->mutex ), 0 );
  T_eq_int( pthread_cond_wait( &ctx->cond_obj, &ctx->mutex ), 0 );
  ++ctx->woken;
  T_eq_int( pthread_mutex_unlock( &ctx->mutex ), 0 );

  SuspendSelf();
}

static void StartWorkers( Context *ctx )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->worker_id ); ++i ) {
    ctx->worker_id[ i ] = CreateTask( "WORK", PRIO_HIGH );
    StartTask( ctx->worker_id[ i ], Worker, ctx );
  }
}

static void CPthreadReqCondSignal_Pre_Cond_Prepare(
  CPthreadReqCondSignal_Context *ctx,
  CPthreadReqCondSignal_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Pre_Cond_Valid: {
      /*
       * While the `cond` parameter references an initialized condition
       * variable object.
       */
      T_eq_int( pthread_cond_init( &ctx->cond_obj, NULL ), 0 );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondSignal_Pre_Cond_Zeroed: {
      /*
       * While the `cond` parameter references an object filled with zero
       * bytes.
       */
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondSignal_Pre_Cond_Invalid: {
      /*
       * While the `cond` parameter references an object which is neither
       * initialized nor filled with zero bytes.
       */
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
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

static void CPthreadReqCondSignal_Pre_Waiters_Prepare(
  CPthreadReqCondSignal_Context    *ctx,
  CPthreadReqCondSignal_Pre_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Pre_Waiters_Yes: {
      /*
       * While more than one thread is blocked on the condition variable.
       */
      StartWorkers( ctx );
      break;
    }

    case CPthreadReqCondSignal_Pre_Waiters_No: {
      /*
       * While no thread is blocked on the condition variable.
       */
      /* Nothing to do */
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
    case CPthreadReqCondSignal_Post_Status_Zero: {
      /*
       * The return value of pthread_cond_signal() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondSignal_Post_Status_EINVAL: {
      /*
       * The return value of pthread_cond_signal() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondSignal_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Post_Threads_Check(
  CPthreadReqCondSignal_Context     *ctx,
  CPthreadReqCondSignal_Post_Threads state
)
{
  switch ( state ) {
    case CPthreadReqCondSignal_Post_Threads_One: {
      /*
       * Exactly one of the threads blocked on the condition variable shall be
       * unblocked.
       */
      T_eq_u32( ctx->woken, 1 );
      break;
    }

    case CPthreadReqCondSignal_Post_Threads_Nop: {
      /*
       * No thread shall be unblocked.
       */
      T_eq_u32( ctx->woken, 0 );
      break;
    }

    case CPthreadReqCondSignal_Post_Threads_NA:
      break;
  }
}

static void CPthreadReqCondSignal_Setup( CPthreadReqCondSignal_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  T_eq_int( pthread_mutex_init( &ctx->mutex, NULL ), 0 );
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
  (void) pthread_mutex_destroy( &ctx->mutex );
  RestoreRunnerPriority();
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
  ctx->woken = 0;
  ctx->worker_id[ 0 ] = 0;
  ctx->worker_id[ 1 ] = 0;
}

static void CPthreadReqCondSignal_Action( CPthreadReqCondSignal_Context *ctx )
{
  ctx->retval = pthread_cond_signal( ctx->cond );
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

static const CPthreadReqCondSignal_Entry
CPthreadReqCondSignal_Entries[] = {
  { 0, 0, 1, CPthreadReqCondSignal_Post_Status_EINVAL,
    CPthreadReqCondSignal_Post_Threads_Nop },
  { 0, 0, 1, CPthreadReqCondSignal_Post_Status_Zero,
    CPthreadReqCondSignal_Post_Threads_Nop },
  { 0, 0, 0, CPthreadReqCondSignal_Post_Status_Zero,
    CPthreadReqCondSignal_Post_Threads_One },
  { 0, 0, 0, CPthreadReqCondSignal_Post_Status_Zero,
    CPthreadReqCondSignal_Post_Threads_Nop }
};

static const uint8_t
CPthreadReqCondSignal_Map[] = {
  2, 3, 1, 1, 0, 0, 0, 0
};

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
  return CPthreadReqCondSignal_Entries[
    CPthreadReqCondSignal_Map[ index ]
  ];
}

static void CPthreadReqCondSignal_SetPreConditionStates(
  CPthreadReqCondSignal_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Waiters_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondSignal_Pre_Waiters_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CPthreadReqCondSignal_TestVariant(
  CPthreadReqCondSignal_Context *ctx
)
{
  CPthreadReqCondSignal_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondSignal_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondSignal_Action( ctx );
  CPthreadReqCondSignal_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondSignal_Post_Threads_Check( ctx, ctx->Map.entry.Post_Threads );
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
    ctx->Map.pci[ 0 ] = CPthreadReqCondSignal_Pre_Cond_Valid;
    ctx->Map.pci[ 0 ] < CPthreadReqCondSignal_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondSignal_Pre_Waiters_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCondSignal_Pre_Waiters_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCondSignal_PopEntry( ctx );
      CPthreadReqCondSignal_SetPreConditionStates( ctx );
      CPthreadReqCondSignal_Prepare( ctx );
      CPthreadReqCondSignal_TestVariant( ctx );
      CPthreadReqCondSignal_Cleanup( ctx );
    }
  }
}

/** @} */
