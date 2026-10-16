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
#include <string.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondDestroy spec:/c/pthread/req/cond-destroy
 *
 * @{
 */

typedef enum {
  CPthreadReqCondDestroy_Pre_Cond_Null,
  CPthreadReqCondDestroy_Pre_Cond_Invalid,
  CPthreadReqCondDestroy_Pre_Cond_Zeroed,
  CPthreadReqCondDestroy_Pre_Cond_Valid,
  CPthreadReqCondDestroy_Pre_Cond_NA
} CPthreadReqCondDestroy_Pre_Cond;

typedef enum {
  CPthreadReqCondDestroy_Pre_Waiters_No,
  CPthreadReqCondDestroy_Pre_Waiters_Yes,
  CPthreadReqCondDestroy_Pre_Waiters_NA
} CPthreadReqCondDestroy_Pre_Waiters;

typedef enum {
  CPthreadReqCondDestroy_Post_Status_Zero,
  CPthreadReqCondDestroy_Post_Status_EBUSY,
  CPthreadReqCondDestroy_Post_Status_EINVAL,
  CPthreadReqCondDestroy_Post_Status_NA
} CPthreadReqCondDestroy_Post_Status;

typedef enum {
  CPthreadReqCondDestroy_Post_Object_Destroyed,
  CPthreadReqCondDestroy_Post_Object_Valid,
  CPthreadReqCondDestroy_Post_Object_NA
} CPthreadReqCondDestroy_Post_Object;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Cond_NA : 1;
  uint8_t Pre_Waiters_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Object : 2;
} CPthreadReqCondDestroy_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/cond-destroy test case.
 */
typedef struct {
  /**
   * @brief This member provides the condition variable object.
   */
  pthread_cond_t cond_obj;

  /**
   * @brief This member provides the mutex used by the waiting worker.
   */
  pthread_mutex_t mutex;

  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member specifies the `cond` parameter value.
   */
  pthread_cond_t *cond;

  /**
   * @brief This member contains the return value.
   */
  int retval;

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
    CPthreadReqCondDestroy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondDestroy_Context;

static CPthreadReqCondDestroy_Context
  CPthreadReqCondDestroy_Instance;

static const char * const CPthreadReqCondDestroy_PreDesc_Cond[] = {
  "Null",
  "Invalid",
  "Zeroed",
  "Valid",
  "NA"
};

static const char * const CPthreadReqCondDestroy_PreDesc_Waiters[] = {
  "No",
  "Yes",
  "NA"
};

static const char * const * const CPthreadReqCondDestroy_PreDesc[] = {
  CPthreadReqCondDestroy_PreDesc_Cond,
  CPthreadReqCondDestroy_PreDesc_Waiters,
  NULL
};

typedef CPthreadReqCondDestroy_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;
  int      eno;

  ctx = (Context *) arg;

  eno = pthread_mutex_lock( &ctx->mutex );
  T_eq_int( eno, 0 );

  eno = pthread_cond_wait( &ctx->cond_obj, &ctx->mutex );
  T_eq_int( eno, 0 );

  eno = pthread_mutex_unlock( &ctx->mutex );
  T_eq_int( eno, 0 );

  SuspendSelf();
}

static void CPthreadReqCondDestroy_Pre_Cond_Prepare(
  CPthreadReqCondDestroy_Context *ctx,
  CPthreadReqCondDestroy_Pre_Cond state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqCondDestroy_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondDestroy_Pre_Cond_Invalid: {
      /*
       * While the `cond` parameter references an object which is neither
       * initialized nor filled with zero bytes.
       */
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondDestroy_Pre_Cond_Zeroed: {
      /*
       * While the `cond` parameter references an object filled with zero
       * bytes.
       */
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondDestroy_Pre_Cond_Valid: {
      /*
       * While the `cond` parameter references an initialized condition
       * variable object.
       */
      eno = pthread_cond_init( &ctx->cond_obj, NULL );
      T_eq_int( eno, 0 );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondDestroy_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Pre_Waiters_Prepare(
  CPthreadReqCondDestroy_Context    *ctx,
  CPthreadReqCondDestroy_Pre_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqCondDestroy_Pre_Waiters_No: {
      /*
       * While no thread waits on the condition variable object.
       */
      /* Nothing to prepare */
      break;
    }

    case CPthreadReqCondDestroy_Pre_Waiters_Yes: {
      /*
       * While at least one thread waits on the condition variable object.
       */
      ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
      StartTask( ctx->worker_id, Worker, ctx );
      WaitForExecutionStop( ctx->worker_id );
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
    case CPthreadReqCondDestroy_Post_Status_Zero: {
      /*
       * The return value of pthread_cond_destroy() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondDestroy_Post_Status_EBUSY: {
      /*
       * The return value of pthread_cond_destroy() shall be equal to EBUSY.
       */
      T_eq_int( ctx->retval, EBUSY );
      break;
    }

    case CPthreadReqCondDestroy_Post_Status_EINVAL: {
      /*
       * The return value of pthread_cond_destroy() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondDestroy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Post_Object_Check(
  CPthreadReqCondDestroy_Context    *ctx,
  CPthreadReqCondDestroy_Post_Object state
)
{
  switch ( state ) {
    case CPthreadReqCondDestroy_Post_Object_Destroyed: {
      /*
       * The condition variable object shall be destroyed.
       */
      T_eq_int( pthread_cond_destroy( &ctx->cond_obj ), EINVAL );
      break;
    }

    case CPthreadReqCondDestroy_Post_Object_Valid: {
      /*
       * The condition variable object shall remain initialized.
       */
      T_eq_int( pthread_cond_destroy( &ctx->cond_obj ), EBUSY );
      break;
    }

    case CPthreadReqCondDestroy_Post_Object_NA:
      break;
  }
}

static void CPthreadReqCondDestroy_Setup( CPthreadReqCondDestroy_Context *ctx )
{
  int eno;

  memset( ctx, 0, sizeof( *ctx ) );
  SetSelfPriority( PRIO_NORMAL );
  eno = pthread_mutex_init( &ctx->mutex, NULL );
  T_eq_int( eno, 0 );
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
  (void) pthread_mutex_destroy( &ctx->mutex );
  RestoreRunnerPriority();
}

static void CPthreadReqCondDestroy_Teardown_Wrap( void *arg )
{
  CPthreadReqCondDestroy_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondDestroy_Teardown( ctx );
}

static void CPthreadReqCondDestroy_Action(
  CPthreadReqCondDestroy_Context *ctx
)
{
  ctx->retval = pthread_cond_destroy( ctx->cond );
}

static void CPthreadReqCondDestroy_Cleanup(
  CPthreadReqCondDestroy_Context *ctx
)
{
  if ( ctx->worker_id != 0 ) {
    DeleteTask( ctx->worker_id );
    ctx->worker_id = 0;
  }
}

static const CPthreadReqCondDestroy_Entry
CPthreadReqCondDestroy_Entries[] = {
  { 0, 0, 1, CPthreadReqCondDestroy_Post_Status_EINVAL,
    CPthreadReqCondDestroy_Post_Object_NA },
  { 0, 0, 1, CPthreadReqCondDestroy_Post_Status_Zero,
    CPthreadReqCondDestroy_Post_Object_Destroyed },
  { 0, 0, 0, CPthreadReqCondDestroy_Post_Status_Zero,
    CPthreadReqCondDestroy_Post_Object_Destroyed },
  { 0, 0, 0, CPthreadReqCondDestroy_Post_Status_EBUSY,
    CPthreadReqCondDestroy_Post_Object_Valid }
};

static const uint8_t
CPthreadReqCondDestroy_Map[] = {
  0, 0, 0, 0, 1, 1, 2, 3
};

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
  return CPthreadReqCondDestroy_Entries[
    CPthreadReqCondDestroy_Map[ index ]
  ];
}

static void CPthreadReqCondDestroy_SetPreConditionStates(
  CPthreadReqCondDestroy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Waiters_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondDestroy_Pre_Waiters_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CPthreadReqCondDestroy_TestVariant(
  CPthreadReqCondDestroy_Context *ctx
)
{
  CPthreadReqCondDestroy_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondDestroy_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondDestroy_Action( ctx );
  CPthreadReqCondDestroy_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondDestroy_Post_Object_Check( ctx, ctx->Map.entry.Post_Object );
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
    ctx->Map.pci[ 0 ] = CPthreadReqCondDestroy_Pre_Cond_Null;
    ctx->Map.pci[ 0 ] < CPthreadReqCondDestroy_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondDestroy_Pre_Waiters_No;
      ctx->Map.pci[ 1 ] < CPthreadReqCondDestroy_Pre_Waiters_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCondDestroy_PopEntry( ctx );
      CPthreadReqCondDestroy_SetPreConditionStates( ctx );
      CPthreadReqCondDestroy_TestVariant( ctx );
      CPthreadReqCondDestroy_Cleanup( ctx );
    }
  }
}

/** @} */
