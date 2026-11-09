/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqKeyDelete
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
#include <rtems/posix/keyimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqKeyDelete spec:/c/pthread/req/key-delete
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CPthreadReqKeyDelete_Pre_Key_Valid,
  CPthreadReqKeyDelete_Pre_Key_Invalid,
  CPthreadReqKeyDelete_Pre_Key_NA
} CPthreadReqKeyDelete_Pre_Key;

typedef enum {
  CPthreadReqKeyDelete_Pre_Executing_Yes,
  CPthreadReqKeyDelete_Pre_Executing_No,
  CPthreadReqKeyDelete_Pre_Executing_NA
} CPthreadReqKeyDelete_Pre_Executing;

typedef enum {
  CPthreadReqKeyDelete_Pre_Other_Yes,
  CPthreadReqKeyDelete_Pre_Other_No,
  CPthreadReqKeyDelete_Pre_Other_NA
} CPthreadReqKeyDelete_Pre_Other;

typedef enum {
  CPthreadReqKeyDelete_Post_Status_Ok,
  CPthreadReqKeyDelete_Post_Status_Inval,
  CPthreadReqKeyDelete_Post_Status_NA
} CPthreadReqKeyDelete_Post_Status;

typedef enum {
  CPthreadReqKeyDelete_Post_Key_Deleted,
  CPthreadReqKeyDelete_Post_Key_Nop,
  CPthreadReqKeyDelete_Post_Key_NA
} CPthreadReqKeyDelete_Post_Key;

typedef enum {
  CPthreadReqKeyDelete_Post_Values_Removed,
  CPthreadReqKeyDelete_Post_Values_NA
} CPthreadReqKeyDelete_Post_Values;

typedef enum {
  CPthreadReqKeyDelete_Post_Pairs_Two,
  CPthreadReqKeyDelete_Post_Pairs_One,
  CPthreadReqKeyDelete_Post_Pairs_Nop,
  CPthreadReqKeyDelete_Post_Pairs_NA
} CPthreadReqKeyDelete_Post_Pairs;

typedef enum {
  CPthreadReqKeyDelete_Post_Destructor_Nop,
  CPthreadReqKeyDelete_Post_Destructor_NA
} CPthreadReqKeyDelete_Post_Destructor;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Key_NA : 1;
  uint16_t Pre_Executing_NA : 1;
  uint16_t Pre_Other_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Key : 2;
  uint16_t Post_Values : 1;
  uint16_t Post_Pairs : 2;
  uint16_t Post_Destructor : 1;
} CPthreadReqKeyDelete_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/key-delete test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `key` parameter value.
   */
  pthread_key_t key;

  /**
   * @brief This member contains the return value of the pthread_key_delete()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the key created for the test.
   */
  pthread_key_t key_obj;

  /**
   * @brief This member contains the count of inactive key value pairs before
   *   the call.
   */
  size_t pairs_before;

  /**
   * @brief This member contains the count of destructor function calls.
   */
  uint32_t destructor_calls;

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
    CPthreadReqKeyDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqKeyDelete_Context;

static CPthreadReqKeyDelete_Context CPthreadReqKeyDelete_Instance;

static const char *const CPthreadReqKeyDelete_PreDesc_Key[] =
  { "Valid", "Invalid", "NA" };

static const char *const CPthreadReqKeyDelete_PreDesc_Executing[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqKeyDelete_PreDesc_Other[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqKeyDelete_PreDesc[] = {
  CPthreadReqKeyDelete_PreDesc_Key,
  CPthreadReqKeyDelete_PreDesc_Executing,
  CPthreadReqKeyDelete_PreDesc_Other,
  NULL
};

#define KEY_INVALID ( (pthread_key_t) 0xffffffff )

typedef CPthreadReqKeyDelete_Context Context;

static int executing_value;

static int other_value;

static void Destructor( void *value )
{
  Context *ctx;

  (void) value;
  ctx = T_fixture_context();
  ++ctx->destructor_calls;
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

static size_t CountFreePairs( void )
{
  const Chain_Control *chain;
  const Chain_Node    *node;
  size_t               count;

  chain = &_POSIX_Keys_Keypool.Free;
  node = _Chain_Immutable_first( chain );
  count = 0;

  while ( node != _Chain_Immutable_tail( chain ) ) {
    ++count;
    node = _Chain_Immutable_next( node );
  }

  return count;
}

static void CPthreadReqKeyDelete_Pre_Key_Prepare(
  CPthreadReqKeyDelete_Context *ctx,
  CPthreadReqKeyDelete_Pre_Key  state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Pre_Key_Valid: {
      /*
       * While the `key` parameter is a valid thread-specific data key.
       */
      ctx->key = ctx->key_obj;
      break;
    }

    case CPthreadReqKeyDelete_Pre_Key_Invalid: {
      /*
       * While the `key` parameter is not a valid thread-specific data key.
       */
      ctx->key = KEY_INVALID;
      break;
    }

    case CPthreadReqKeyDelete_Pre_Key_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Pre_Executing_Prepare(
  CPthreadReqKeyDelete_Context      *ctx,
  CPthreadReqKeyDelete_Pre_Executing state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Pre_Executing_Yes: {
      /*
       * While the calling thread has a value associated with the key specified
       * by the `key` parameter.
       */
      T_eq_int( pthread_setspecific( ctx->key_obj, &executing_value ), 0 );
      break;
    }

    case CPthreadReqKeyDelete_Pre_Executing_No: {
      /*
       * While the calling thread has no value associated with the key
       * specified by the `key` parameter.
       */
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, GetExecuting() ) );
      break;
    }

    case CPthreadReqKeyDelete_Pre_Executing_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Pre_Other_Prepare(
  CPthreadReqKeyDelete_Context  *ctx,
  CPthreadReqKeyDelete_Pre_Other state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Pre_Other_Yes: {
      /*
       * While a thread other than the calling thread has a value associated
       * with the key specified by the `key` parameter.
       */
      StartTask( ctx->worker_id, Worker, ctx );
      (void) ReceiveAnyEvents();
      break;
    }

    case CPthreadReqKeyDelete_Pre_Other_No: {
      /*
       * While no thread other than the calling thread has a value associated
       * with the key specified by the `key` parameter.
       */
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, ctx->worker_tcb ) );
      break;
    }

    case CPthreadReqKeyDelete_Pre_Other_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Post_Status_Check(
  CPthreadReqKeyDelete_Context    *ctx,
  CPthreadReqKeyDelete_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Post_Status_Ok: {
      /*
       * The return value of pthread_key_delete() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqKeyDelete_Post_Status_Inval: {
      /*
       * The return value of pthread_key_delete() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqKeyDelete_Post_Status_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Post_Key_Check(
  CPthreadReqKeyDelete_Context *ctx,
  CPthreadReqKeyDelete_Post_Key state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Post_Key_Deleted: {
      /*
       * The thread-specific data key specified by the `key` parameter shall be
       * deleted.
       */
      T_null( _POSIX_Keys_Get( ctx->key_obj ) );
      break;
    }

    case CPthreadReqKeyDelete_Post_Key_Nop: {
      /*
       * No thread-specific data key shall be deleted by the
       * pthread_key_delete() call.
       */
      T_not_null( _POSIX_Keys_Get( ctx->key_obj ) );
      break;
    }

    case CPthreadReqKeyDelete_Post_Key_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Post_Values_Check(
  CPthreadReqKeyDelete_Context    *ctx,
  CPthreadReqKeyDelete_Post_Values state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Post_Values_Removed: {
      /*
       * No thread shall have a value associated with the thread-specific data
       * key specified by the `key` parameter.
       */
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, GetExecuting() ) );
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, ctx->worker_tcb ) );
      break;
    }

    case CPthreadReqKeyDelete_Post_Values_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Post_Pairs_Check(
  CPthreadReqKeyDelete_Context   *ctx,
  CPthreadReqKeyDelete_Post_Pairs state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Post_Pairs_Two: {
      /*
       * Two key value pairs shall become inactive, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      T_eq_sz( CountFreePairs(), ctx->pairs_before + 2 );
      break;
    }

    case CPthreadReqKeyDelete_Post_Pairs_One: {
      /*
       * One key value pair shall become inactive, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      T_eq_sz( CountFreePairs(), ctx->pairs_before + 1 );
      break;
    }

    case CPthreadReqKeyDelete_Post_Pairs_Nop: {
      /*
       * The count of inactive key value pairs shall not be modified, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      T_eq_sz( CountFreePairs(), ctx->pairs_before );
      break;
    }

    case CPthreadReqKeyDelete_Post_Pairs_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Post_Destructor_Check(
  CPthreadReqKeyDelete_Context        *ctx,
  CPthreadReqKeyDelete_Post_Destructor state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Post_Destructor_Nop: {
      /*
       * No destructor function shall be called by the pthread_key_delete()
       * call.
       */
      T_eq_u32( ctx->destructor_calls, 0 );
      break;
    }

    case CPthreadReqKeyDelete_Post_Destructor_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Setup( CPthreadReqKeyDelete_Context *ctx )
{
  ctx->runner_id = rtems_task_self();
}

static void CPthreadReqKeyDelete_Setup_Wrap( void *arg )
{
  CPthreadReqKeyDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqKeyDelete_Setup( ctx );
}

static void CPthreadReqKeyDelete_Prepare( CPthreadReqKeyDelete_Context *ctx )
{
  ctx->destructor_calls = 0;
  ctx->status = -1;
  T_eq_int( pthread_key_create( &ctx->key_obj, Destructor ), 0 );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  ctx->worker_tcb = GetThread( ctx->worker_id );
}

static void CPthreadReqKeyDelete_Action( CPthreadReqKeyDelete_Context *ctx )
{
  ctx->pairs_before = CountFreePairs();
  ctx->status = pthread_key_delete( ctx->key );
}

static void CPthreadReqKeyDelete_Cleanup( CPthreadReqKeyDelete_Context *ctx )
{
  DeleteTask( ctx->worker_id );

  if ( ctx->status != 0 ) {
    T_eq_int( pthread_key_delete( ctx->key_obj ), 0 );
  }
}

/* clang-format off */

static const CPthreadReqKeyDelete_Entry
CPthreadReqKeyDelete_Entries[] = {
  { 1, 0, 0, 0, CPthreadReqKeyDelete_Post_Status_NA,
    CPthreadReqKeyDelete_Post_Key_NA, CPthreadReqKeyDelete_Post_Values_NA,
    CPthreadReqKeyDelete_Post_Pairs_NA, CPthreadReqKeyDelete_Post_Destructor_NA },
  { 0, 0, 0, 0, CPthreadReqKeyDelete_Post_Status_Ok,
    CPthreadReqKeyDelete_Post_Key_Deleted,
    CPthreadReqKeyDelete_Post_Values_Removed,
    CPthreadReqKeyDelete_Post_Pairs_One,
    CPthreadReqKeyDelete_Post_Destructor_Nop },
  { 0, 0, 0, 0, CPthreadReqKeyDelete_Post_Status_Ok,
    CPthreadReqKeyDelete_Post_Key_Deleted,
    CPthreadReqKeyDelete_Post_Values_Removed,
    CPthreadReqKeyDelete_Post_Pairs_Two,
    CPthreadReqKeyDelete_Post_Destructor_Nop },
  { 0, 0, 0, 0, CPthreadReqKeyDelete_Post_Status_Ok,
    CPthreadReqKeyDelete_Post_Key_Deleted,
    CPthreadReqKeyDelete_Post_Values_Removed,
    CPthreadReqKeyDelete_Post_Pairs_Nop,
    CPthreadReqKeyDelete_Post_Destructor_Nop },
  { 0, 0, 1, 1, CPthreadReqKeyDelete_Post_Status_Inval,
    CPthreadReqKeyDelete_Post_Key_Nop, CPthreadReqKeyDelete_Post_Values_NA,
    CPthreadReqKeyDelete_Post_Pairs_Nop,
    CPthreadReqKeyDelete_Post_Destructor_Nop }
};

static const uint8_t
CPthreadReqKeyDelete_Map[] = {
  2, 1, 1, 3, 4, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqKeyDelete_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqKeyDelete_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqKeyDelete_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqKeyDelete_Fixture = {
  .setup = CPthreadReqKeyDelete_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqKeyDelete_Scope,
  .initial_context = &CPthreadReqKeyDelete_Instance
};

static inline CPthreadReqKeyDelete_Entry CPthreadReqKeyDelete_PopEntry(
  CPthreadReqKeyDelete_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqKeyDelete_Entries[ CPthreadReqKeyDelete_Map[ index ] ];
}

static void CPthreadReqKeyDelete_SetPreConditionStates(
  CPthreadReqKeyDelete_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Executing_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqKeyDelete_Pre_Executing_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Other_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqKeyDelete_Pre_Other_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CPthreadReqKeyDelete_TestVariant(
  CPthreadReqKeyDelete_Context *ctx
)
{
  CPthreadReqKeyDelete_Pre_Key_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqKeyDelete_Pre_Executing_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqKeyDelete_Pre_Other_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqKeyDelete_Action( ctx );
  CPthreadReqKeyDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqKeyDelete_Post_Key_Check( ctx, ctx->Map.entry.Post_Key );
  CPthreadReqKeyDelete_Post_Values_Check( ctx, ctx->Map.entry.Post_Values );
  CPthreadReqKeyDelete_Post_Pairs_Check( ctx, ctx->Map.entry.Post_Pairs );
  CPthreadReqKeyDelete_Post_Destructor_Check(
    ctx,
    ctx->Map.entry.Post_Destructor
  );
}

/**
 * @fn void T_case_body_CPthreadReqKeyDelete( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqKeyDelete, &CPthreadReqKeyDelete_Fixture )
{
  CPthreadReqKeyDelete_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqKeyDelete_Pre_Key_Valid;
    ctx->Map.pci[ 0 ] < CPthreadReqKeyDelete_Pre_Key_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqKeyDelete_Pre_Executing_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqKeyDelete_Pre_Executing_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqKeyDelete_Pre_Other_Yes;
        ctx->Map.pci[ 2 ] < CPthreadReqKeyDelete_Pre_Other_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqKeyDelete_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqKeyDelete_SetPreConditionStates( ctx );
        CPthreadReqKeyDelete_Prepare( ctx );
        CPthreadReqKeyDelete_TestVariant( ctx );
        CPthreadReqKeyDelete_Cleanup( ctx );
      }
    }
  }
}

/** @} */
