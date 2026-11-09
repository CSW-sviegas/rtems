/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqGetspecific
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

#include <pthread.h>
#include <rtems/posix/keyimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqGetspecific spec:/c/pthread/req/getspecific
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CPthreadReqGetspecific_Pre_Key_Valid,
  CPthreadReqGetspecific_Pre_Key_Invalid,
  CPthreadReqGetspecific_Pre_Key_NA
} CPthreadReqGetspecific_Pre_Key;

typedef enum {
  CPthreadReqGetspecific_Pre_Value_Obj,
  CPthreadReqGetspecific_Pre_Value_Null,
  CPthreadReqGetspecific_Pre_Value_NA
} CPthreadReqGetspecific_Pre_Value;

typedef enum {
  CPthreadReqGetspecific_Pre_Other_Yes,
  CPthreadReqGetspecific_Pre_Other_No,
  CPthreadReqGetspecific_Pre_Other_NA
} CPthreadReqGetspecific_Pre_Other;

typedef enum {
  CPthreadReqGetspecific_Post_Result_Obj,
  CPthreadReqGetspecific_Post_Result_Null,
  CPthreadReqGetspecific_Post_Result_NA
} CPthreadReqGetspecific_Post_Result;

typedef enum {
  CPthreadReqGetspecific_Post_Values_Nop,
  CPthreadReqGetspecific_Post_Values_NA
} CPthreadReqGetspecific_Post_Values;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Key_NA : 1;
  uint8_t Pre_Value_NA : 1;
  uint8_t Pre_Other_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_Values : 1;
} CPthreadReqGetspecific_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/getspecific test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `key` parameter value.
   */
  pthread_key_t key;

  /**
   * @brief This member contains the return value of the pthread_getspecific()
   *   call.
   */
  void *result;

  /**
   * @brief This member contains the key created for the test.
   */
  pthread_key_t key_obj;

  /**
   * @brief This member contains the value which the calling thread associated
   *   with the key before the call.
   */
  void *executing_before;

  /**
   * @brief This member contains the value which the worker task associated
   *   with the key before the call.
   */
  void *other_before;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member references the TCB of the worker task.
   */
  Thread_Control *worker_tcb;

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
    CPthreadReqGetspecific_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqGetspecific_Context;

static CPthreadReqGetspecific_Context CPthreadReqGetspecific_Instance;

static const char *const CPthreadReqGetspecific_PreDesc_Key[] =
  { "Valid", "Invalid", "NA" };

static const char *const CPthreadReqGetspecific_PreDesc_Value[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqGetspecific_PreDesc_Other[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqGetspecific_PreDesc[] = {
  CPthreadReqGetspecific_PreDesc_Key,
  CPthreadReqGetspecific_PreDesc_Value,
  CPthreadReqGetspecific_PreDesc_Other,
  NULL
};

#define KEY_INVALID ( (pthread_key_t) 0xffffffff )

typedef CPthreadReqGetspecific_Context Context;

static int executing_value;

static int other_value;

static void *GetValue( pthread_key_t key, const Thread_Control *thread )
{
  const POSIX_Keys_Key_value_pair *pair;

  pair = _POSIX_Keys_Key_value_find( key, thread );

  if ( pair == NULL ) {
    return NULL;
  }

  return pair->value;
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  T_eq_int( pthread_setspecific( ctx->key_obj, &other_value ), 0 );
  SendEvents( ctx->runner_id, RTEMS_EVENT_0 );

  while ( true ) {
    (void) ReceiveAnyEvents();
  }
}

static void CPthreadReqGetspecific_Pre_Key_Prepare(
  CPthreadReqGetspecific_Context *ctx,
  CPthreadReqGetspecific_Pre_Key  state
)
{
  switch ( state ) {
    case CPthreadReqGetspecific_Pre_Key_Valid: {
      /*
       * While the `key` parameter is a valid thread-specific data key.
       */
      ctx->key = ctx->key_obj;
      break;
    }

    case CPthreadReqGetspecific_Pre_Key_Invalid: {
      /*
       * While the `key` parameter is not a valid thread-specific data key.
       */
      ctx->key = KEY_INVALID;
      break;
    }

    case CPthreadReqGetspecific_Pre_Key_NA:
      break;
  }
}

static void CPthreadReqGetspecific_Pre_Value_Prepare(
  CPthreadReqGetspecific_Context  *ctx,
  CPthreadReqGetspecific_Pre_Value state
)
{
  switch ( state ) {
    case CPthreadReqGetspecific_Pre_Value_Obj: {
      /*
       * While the calling thread has a value associated with the key specified
       * by the `key` parameter.
       */
      T_eq_int( pthread_setspecific( ctx->key_obj, &executing_value ), 0 );
      break;
    }

    case CPthreadReqGetspecific_Pre_Value_Null: {
      /*
       * While the calling thread has no value associated with the key
       * specified by the `key` parameter.
       */
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, GetExecuting() ) );
      break;
    }

    case CPthreadReqGetspecific_Pre_Value_NA:
      break;
  }
}

static void CPthreadReqGetspecific_Pre_Other_Prepare(
  CPthreadReqGetspecific_Context  *ctx,
  CPthreadReqGetspecific_Pre_Other state
)
{
  switch ( state ) {
    case CPthreadReqGetspecific_Pre_Other_Yes: {
      /*
       * While a thread other than the calling thread has a value associated
       * with the key specified by the `key` parameter.
       */
      StartTask( ctx->worker_id, Worker, ctx );
      (void) ReceiveAnyEvents();
      break;
    }

    case CPthreadReqGetspecific_Pre_Other_No: {
      /*
       * While no thread other than the calling thread has a value associated
       * with the key specified by the `key` parameter.
       */
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, ctx->worker_tcb ) );
      break;
    }

    case CPthreadReqGetspecific_Pre_Other_NA:
      break;
  }
}

static void CPthreadReqGetspecific_Post_Result_Check(
  CPthreadReqGetspecific_Context    *ctx,
  CPthreadReqGetspecific_Post_Result state
)
{
  switch ( state ) {
    case CPthreadReqGetspecific_Post_Result_Obj: {
      /*
       * The return value of pthread_getspecific() shall be the value which the
       * calling thread associated with the key specified by the `key`
       * parameter.
       */
      T_eq_ptr( ctx->result, &executing_value );
      break;
    }

    case CPthreadReqGetspecific_Post_Result_Null: {
      /*
       * The return value of pthread_getspecific() shall be NULL.
       */
      T_null( ctx->result );
      break;
    }

    case CPthreadReqGetspecific_Post_Result_NA:
      break;
  }
}

static void CPthreadReqGetspecific_Post_Values_Check(
  CPthreadReqGetspecific_Context    *ctx,
  CPthreadReqGetspecific_Post_Values state
)
{
  switch ( state ) {
    case CPthreadReqGetspecific_Post_Values_Nop: {
      /*
       * The values associated with the key specified by the `key` parameter
       * shall not be modified.
       */
      T_eq_ptr(
        GetValue( ctx->key_obj, GetExecuting() ),
        ctx->executing_before
      );
      T_eq_ptr( GetValue( ctx->key_obj, ctx->worker_tcb ), ctx->other_before );
      break;
    }

    case CPthreadReqGetspecific_Post_Values_NA:
      break;
  }
}

static void CPthreadReqGetspecific_Setup( CPthreadReqGetspecific_Context *ctx )
{
  ctx->runner_id = rtems_task_self();
}

static void CPthreadReqGetspecific_Setup_Wrap( void *arg )
{
  CPthreadReqGetspecific_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqGetspecific_Setup( ctx );
}

static void CPthreadReqGetspecific_Prepare(
  CPthreadReqGetspecific_Context *ctx
)
{
  T_eq_int( pthread_key_create( &ctx->key_obj, NULL ), 0 );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  ctx->worker_tcb = GetThread( ctx->worker_id );
}

static void CPthreadReqGetspecific_Action(
  CPthreadReqGetspecific_Context *ctx
)
{
  ctx->executing_before = GetValue( ctx->key_obj, GetExecuting() );
  ctx->other_before = GetValue( ctx->key_obj, ctx->worker_tcb );
  ctx->result = pthread_getspecific( ctx->key );
}

static void CPthreadReqGetspecific_Cleanup(
  CPthreadReqGetspecific_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  T_eq_int( pthread_key_delete( ctx->key_obj ), 0 );
}

/* clang-format off */

static const CPthreadReqGetspecific_Entry
CPthreadReqGetspecific_Entries[] = {
  { 1, 0, 0, 0, CPthreadReqGetspecific_Post_Result_NA,
    CPthreadReqGetspecific_Post_Values_NA },
  { 0, 0, 0, 0, CPthreadReqGetspecific_Post_Result_Obj,
    CPthreadReqGetspecific_Post_Values_Nop },
  { 0, 0, 0, 0, CPthreadReqGetspecific_Post_Result_Null,
    CPthreadReqGetspecific_Post_Values_Nop },
  { 0, 0, 1, 1, CPthreadReqGetspecific_Post_Result_Null,
    CPthreadReqGetspecific_Post_Values_NA }
};

static const uint8_t
CPthreadReqGetspecific_Map[] = {
  1, 1, 2, 2, 3, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqGetspecific_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqGetspecific_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqGetspecific_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqGetspecific_Fixture = {
  .setup = CPthreadReqGetspecific_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqGetspecific_Scope,
  .initial_context = &CPthreadReqGetspecific_Instance
};

static inline CPthreadReqGetspecific_Entry CPthreadReqGetspecific_PopEntry(
  CPthreadReqGetspecific_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqGetspecific_Entries[ CPthreadReqGetspecific_Map[ index ] ];
}

static void CPthreadReqGetspecific_SetPreConditionStates(
  CPthreadReqGetspecific_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Value_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqGetspecific_Pre_Value_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Other_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqGetspecific_Pre_Other_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CPthreadReqGetspecific_TestVariant(
  CPthreadReqGetspecific_Context *ctx
)
{
  CPthreadReqGetspecific_Pre_Key_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqGetspecific_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqGetspecific_Pre_Other_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqGetspecific_Action( ctx );
  CPthreadReqGetspecific_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CPthreadReqGetspecific_Post_Values_Check( ctx, ctx->Map.entry.Post_Values );
}

/**
 * @fn void T_case_body_CPthreadReqGetspecific( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqGetspecific, &CPthreadReqGetspecific_Fixture )
{
  CPthreadReqGetspecific_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqGetspecific_Pre_Key_Valid;
    ctx->Map.pci[ 0 ] < CPthreadReqGetspecific_Pre_Key_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqGetspecific_Pre_Value_Obj;
      ctx->Map.pci[ 1 ] < CPthreadReqGetspecific_Pre_Value_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqGetspecific_Pre_Other_Yes;
        ctx->Map.pci[ 2 ] < CPthreadReqGetspecific_Pre_Other_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqGetspecific_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqGetspecific_SetPreConditionStates( ctx );
        CPthreadReqGetspecific_Prepare( ctx );
        CPthreadReqGetspecific_TestVariant( ctx );
        CPthreadReqGetspecific_Cleanup( ctx );
      }
    }
  }
}

/** @} */
