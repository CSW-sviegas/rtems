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
 * @defgroup CPthreadReqCondBroadcast spec:/c/pthread/req/cond-broadcast
 *
 * @{
 */

typedef enum {
  CPthreadReqCondBroadcast_Pre_Cond_Valid,
  CPthreadReqCondBroadcast_Pre_Cond_Zeroed,
  CPthreadReqCondBroadcast_Pre_Cond_Null,
  CPthreadReqCondBroadcast_Pre_Cond_Invalid,
  CPthreadReqCondBroadcast_Pre_Cond_NA
} CPthreadReqCondBroadcast_Pre_Cond;

typedef enum {
  CPthreadReqCondBroadcast_Pre_Waiters_Some,
  CPthreadReqCondBroadcast_Pre_Waiters_None,
  CPthreadReqCondBroadcast_Pre_Waiters_NA
} CPthreadReqCondBroadcast_Pre_Waiters;

typedef enum {
  CPthreadReqCondBroadcast_Post_Status_Zero,
  CPthreadReqCondBroadcast_Post_Status_EINVAL,
  CPthreadReqCondBroadcast_Post_Status_NA
} CPthreadReqCondBroadcast_Post_Status;

typedef enum {
  CPthreadReqCondBroadcast_Post_Threads_Unblocked,
  CPthreadReqCondBroadcast_Post_Threads_Nop,
  CPthreadReqCondBroadcast_Post_Threads_NA
} CPthreadReqCondBroadcast_Post_Threads;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Cond_NA : 1;
  uint8_t Pre_Waiters_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Threads : 2;
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
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the condition variable.
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
    CPthreadReqCondBroadcast_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondBroadcast_Context;

static CPthreadReqCondBroadcast_Context
  CPthreadReqCondBroadcast_Instance;

static const char * const CPthreadReqCondBroadcast_PreDesc_Cond[] = {
  "Valid",
  "Zeroed",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqCondBroadcast_PreDesc_Waiters[] = {
  "Some",
  "None",
  "NA"
};

static const char * const * const CPthreadReqCondBroadcast_PreDesc[] = {
  CPthreadReqCondBroadcast_PreDesc_Cond,
  CPthreadReqCondBroadcast_PreDesc_Waiters,
  NULL
};

typedef CPthreadReqCondBroadcast_Context Context;

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

static void CPthreadReqCondBroadcast_Pre_Cond_Prepare(
  CPthreadReqCondBroadcast_Context *ctx,
  CPthreadReqCondBroadcast_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Pre_Cond_Valid: {
      /*
       * While the `cond` parameter references an initialized condition
       * variable.
       */
      ctx->cond = &ctx->cond_obj;
      T_eq_int( pthread_cond_init( ctx->cond, NULL ), 0 );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Cond_Zeroed: {
      /*
       * While the `cond` parameter references a condition variable object
       * filled with zero bytes.
       */
      ctx->cond = &ctx->cond_obj;
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Cond_Invalid: {
      /*
       * While the `cond` parameter references an invalid condition variable.
       */
      ctx->cond = &ctx->cond_obj;
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Pre_Waiters_Prepare(
  CPthreadReqCondBroadcast_Context    *ctx,
  CPthreadReqCondBroadcast_Pre_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Pre_Waiters_Some: {
      /*
       * While threads are blocked on the condition variable.
       */
      StartWorkers( ctx );
      break;
    }

    case CPthreadReqCondBroadcast_Pre_Waiters_None: {
      /*
       * While no thread is blocked on the condition variable.
       */
      /* Nothing to do */
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
    case CPthreadReqCondBroadcast_Post_Status_Zero: {
      /*
       * The return value of pthread_cond_broadcast() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Status_EINVAL: {
      /*
       * The return value of pthread_cond_broadcast() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Post_Threads_Check(
  CPthreadReqCondBroadcast_Context     *ctx,
  CPthreadReqCondBroadcast_Post_Threads state
)
{
  switch ( state ) {
    case CPthreadReqCondBroadcast_Post_Threads_Unblocked: {
      /*
       * All threads blocked on the condition variable shall be unblocked.
       */
      T_eq_u32( ctx->woken, 2 );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Threads_Nop: {
      /*
       * No thread shall be unblocked.
       */
      T_eq_u32( ctx->woken, 0 );
      break;
    }

    case CPthreadReqCondBroadcast_Post_Threads_NA:
      break;
  }
}

static void CPthreadReqCondBroadcast_Setup(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  SetSelfPriority( PRIO_NORMAL );
  T_eq_int( pthread_mutex_init( &ctx->mutex, NULL ), 0 );
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
  (void) pthread_mutex_destroy( &ctx->mutex );
  RestoreRunnerPriority();
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
  ctx->woken = 0;
  ctx->worker_id[ 0 ] = 0;
  ctx->worker_id[ 1 ] = 0;
}

static void CPthreadReqCondBroadcast_Action(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  ctx->retval = pthread_cond_broadcast( ctx->cond );
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

static const CPthreadReqCondBroadcast_Entry
CPthreadReqCondBroadcast_Entries[] = {
  { 1, 0, 0, CPthreadReqCondBroadcast_Post_Status_NA,
    CPthreadReqCondBroadcast_Post_Threads_NA },
  { 0, 0, 0, CPthreadReqCondBroadcast_Post_Status_Zero,
    CPthreadReqCondBroadcast_Post_Threads_Nop },
  { 0, 0, 0, CPthreadReqCondBroadcast_Post_Status_EINVAL,
    CPthreadReqCondBroadcast_Post_Threads_Nop },
  { 0, 0, 0, CPthreadReqCondBroadcast_Post_Status_Zero,
    CPthreadReqCondBroadcast_Post_Threads_Unblocked }
};

static const uint8_t
CPthreadReqCondBroadcast_Map[] = {
  3, 1, 0, 1, 0, 2, 0, 2
};

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
  return CPthreadReqCondBroadcast_Entries[
    CPthreadReqCondBroadcast_Map[ index ]
  ];
}

static void CPthreadReqCondBroadcast_TestVariant(
  CPthreadReqCondBroadcast_Context *ctx
)
{
  CPthreadReqCondBroadcast_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondBroadcast_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondBroadcast_Action( ctx );
  CPthreadReqCondBroadcast_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondBroadcast_Post_Threads_Check(
    ctx,
    ctx->Map.entry.Post_Threads
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
    ctx->Map.pcs[ 0 ] = CPthreadReqCondBroadcast_Pre_Cond_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqCondBroadcast_Pre_Cond_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqCondBroadcast_Pre_Waiters_Some;
      ctx->Map.pcs[ 1 ] < CPthreadReqCondBroadcast_Pre_Waiters_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCondBroadcast_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      CPthreadReqCondBroadcast_Prepare( ctx );
      CPthreadReqCondBroadcast_TestVariant( ctx );
      CPthreadReqCondBroadcast_Cleanup( ctx );
    }
  }
}

/** @} */
