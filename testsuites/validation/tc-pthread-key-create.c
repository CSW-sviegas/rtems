/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqKeyCreate
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
 * @defgroup CPthreadReqKeyCreate spec:/c/pthread/req/key-create
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CPthreadReqKeyCreate_Pre_Destructor_Func,
  CPthreadReqKeyCreate_Pre_Destructor_Null,
  CPthreadReqKeyCreate_Pre_Destructor_NA
} CPthreadReqKeyCreate_Pre_Destructor;

typedef enum {
  CPthreadReqKeyCreate_Pre_Keys_Available,
  CPthreadReqKeyCreate_Pre_Keys_Exhausted,
  CPthreadReqKeyCreate_Pre_Keys_NA
} CPthreadReqKeyCreate_Pre_Keys;

typedef enum {
  CPthreadReqKeyCreate_Post_Status_Ok,
  CPthreadReqKeyCreate_Post_Status_Again,
  CPthreadReqKeyCreate_Post_Status_NA
} CPthreadReqKeyCreate_Post_Status;

typedef enum {
  CPthreadReqKeyCreate_Post_Key_Created,
  CPthreadReqKeyCreate_Post_Key_Nop,
  CPthreadReqKeyCreate_Post_Key_NA
} CPthreadReqKeyCreate_Post_Key;

typedef enum {
  CPthreadReqKeyCreate_Post_Value_Null,
  CPthreadReqKeyCreate_Post_Value_NA
} CPthreadReqKeyCreate_Post_Value;

typedef enum {
  CPthreadReqKeyCreate_Post_Destructor_Func,
  CPthreadReqKeyCreate_Post_Destructor_Null,
  CPthreadReqKeyCreate_Post_Destructor_NA
} CPthreadReqKeyCreate_Post_Destructor;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Destructor_NA : 1;
  uint16_t Pre_Keys_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Key : 2;
  uint16_t Post_Value : 1;
  uint16_t Post_Destructor : 2;
} CPthreadReqKeyCreate_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/key-create test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `destructor` parameter value.
   */
  void ( *destructor )( void * );

  /**
   * @brief This member contains the return value of the pthread_key_create()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the object referenced by the `key` parameter.
   */
  pthread_key_t key_obj;

  /**
   * @brief This member contains the keys created to use up the available keys.
   */
  pthread_key_t keys[ 16 ];

  /**
   * @brief This member contains the count of keys created to use up the
   *   available keys.
   */
  size_t keys_used;

  /**
   * @brief This member contains the count of destructor function calls.
   */
  uint32_t destructor_calls;

  /**
   * @brief This member contains the argument of the last destructor function
   *   call.
   */
  void *destructor_arg;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member is true, if the worker task was restarted.
   */
  bool worker_restarted;

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
    CPthreadReqKeyCreate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqKeyCreate_Context;

static CPthreadReqKeyCreate_Context CPthreadReqKeyCreate_Instance;

static const char *const CPthreadReqKeyCreate_PreDesc_Destructor[] =
  { "Func", "Null", "NA" };

static const char *const CPthreadReqKeyCreate_PreDesc_Keys[] =
  { "Available", "Exhausted", "NA" };

static const char *const *const CPthreadReqKeyCreate_PreDesc[] = {
  CPthreadReqKeyCreate_PreDesc_Destructor,
  CPthreadReqKeyCreate_PreDesc_Keys,
  NULL
};

#define KEY_POISON ( (pthread_key_t) 0xffffffff )

typedef CPthreadReqKeyCreate_Context Context;

static int worker_value;

static void Destructor( void *value )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->destructor_calls;
  ctx->destructor_arg = value;
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( !ctx->worker_restarted ) {
    T_eq_int( pthread_setspecific( ctx->key_obj, &worker_value ), 0 );
  }

  SendEvents( ctx->runner_id, RTEMS_EVENT_0 );

  while ( true ) {
    (void) ReceiveAnyEvents();
  }
}

/*
 * The task deletion by DeleteTask() returns after the worker terminated, so
 * the destructor functions were called when DeleteTask() returns.
 */
static rtems_id StartWorker( Context *ctx )
{
  rtems_id worker_id;

  ctx->worker_restarted = false;
  worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( worker_id, Worker, ctx );
  (void) ReceiveAnyEvents();

  return worker_id;
}

static void RestartWorker( Context *ctx, rtems_id worker_id )
{
  ctx->worker_restarted = true;
  RestartTask( worker_id, ctx );

  /*
   * The restarted worker sends the event after the thread restart extensions
   * called the destructor functions.
   */
  (void) ReceiveAnyEvents();
}

static void CPthreadReqKeyCreate_Pre_Destructor_Prepare(
  CPthreadReqKeyCreate_Context       *ctx,
  CPthreadReqKeyCreate_Pre_Destructor state
)
{
  switch ( state ) {
    case CPthreadReqKeyCreate_Pre_Destructor_Func: {
      /*
       * While the `destructor` parameter references a function.
       */
      ctx->destructor = Destructor;
      break;
    }

    case CPthreadReqKeyCreate_Pre_Destructor_Null: {
      /*
       * While the `destructor` parameter is NULL.
       */
      ctx->destructor = NULL;
      break;
    }

    case CPthreadReqKeyCreate_Pre_Destructor_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Pre_Keys_Prepare(
  CPthreadReqKeyCreate_Context *ctx,
  CPthreadReqKeyCreate_Pre_Keys state
)
{
  switch ( state ) {
    case CPthreadReqKeyCreate_Pre_Keys_Available: {
      /*
       * While the system has the resources to create another thread-specific
       * data key, see CONFIGURE_MAXIMUM_POSIX_KEYS.
       */
      T_false( _Chain_Is_empty( &_POSIX_Keys_Information.Inactive ) );
      break;
    }

    case CPthreadReqKeyCreate_Pre_Keys_Exhausted: {
      /*
       * While the system lacks the resources to create another thread-specific
       * data key, see CONFIGURE_MAXIMUM_POSIX_KEYS.
       */
      while ( ctx->keys_used < RTEMS_ARRAY_SIZE( ctx->keys ) ) {
        if ( pthread_key_create( &ctx->keys[ ctx->keys_used ], NULL ) != 0 ) {
          break;
        }

        ++ctx->keys_used;
      }

      T_lt_sz( ctx->keys_used, RTEMS_ARRAY_SIZE( ctx->keys ) );
      T_true( _Chain_Is_empty( &_POSIX_Keys_Information.Inactive ) );
      break;
    }

    case CPthreadReqKeyCreate_Pre_Keys_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Post_Status_Check(
  CPthreadReqKeyCreate_Context    *ctx,
  CPthreadReqKeyCreate_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqKeyCreate_Post_Status_Ok: {
      /*
       * The return value of pthread_key_create() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqKeyCreate_Post_Status_Again: {
      /*
       * The return value of pthread_key_create() shall be EAGAIN.
       */
      T_eq_int( ctx->status, EAGAIN );
      break;
    }

    case CPthreadReqKeyCreate_Post_Status_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Post_Key_Check(
  CPthreadReqKeyCreate_Context *ctx,
  CPthreadReqKeyCreate_Post_Key state
)
{
  const POSIX_Keys_Control *the_key;

  switch ( state ) {
    case CPthreadReqKeyCreate_Post_Key_Created: {
      /*
       * The object referenced by the `key` parameter shall be the
       * thread-specific data key created by the pthread_key_create() call.
       */
      the_key = _POSIX_Keys_Get( ctx->key_obj );
      T_assert_not_null( the_key );
      T_eq_u32( the_key->Object.id, ctx->key_obj );
      break;
    }

    case CPthreadReqKeyCreate_Post_Key_Nop: {
      /*
       * The object referenced by the `key` parameter shall not be modified.
       */
      T_eq_u32( ctx->key_obj, KEY_POISON );
      break;
    }

    case CPthreadReqKeyCreate_Post_Key_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Post_Value_Check(
  CPthreadReqKeyCreate_Context   *ctx,
  CPthreadReqKeyCreate_Post_Value state
)
{
  const POSIX_Keys_Control *the_key;

  switch ( state ) {
    case CPthreadReqKeyCreate_Post_Value_Null: {
      /*
       * The value NULL shall be associated with the created thread-specific
       * data key in each thread.
       */
      the_key = _POSIX_Keys_Get( ctx->key_obj );
      T_assert_not_null( the_key );
      T_true( _Chain_Is_empty( &the_key->Key_value_pairs ) );
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, GetExecuting() ) );
      break;
    }

    case CPthreadReqKeyCreate_Post_Value_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Post_Destructor_Check(
  CPthreadReqKeyCreate_Context        *ctx,
  CPthreadReqKeyCreate_Post_Destructor state
)
{
  rtems_id worker_id;

  switch ( state ) {
    case CPthreadReqKeyCreate_Post_Destructor_Func: {
      /*
       * When a thread terminates or is restarted which has a value other than
       * NULL associated with the created thread-specific data key, the
       * destructor function specified by the `destructor` parameter shall be
       * called once with this value as its argument.
       */
      worker_id = StartWorker( ctx );
      RestartWorker( ctx, worker_id );
      T_eq_u32( ctx->destructor_calls, 1 );
      T_eq_ptr( ctx->destructor_arg, &worker_value );
      DeleteTask( worker_id );
      T_eq_u32( ctx->destructor_calls, 1 );

      ctx->destructor_arg = NULL;
      worker_id = StartWorker( ctx );
      DeleteTask( worker_id );
      T_eq_u32( ctx->destructor_calls, 2 );
      T_eq_ptr( ctx->destructor_arg, &worker_value );
      break;
    }

    case CPthreadReqKeyCreate_Post_Destructor_Null: {
      /*
       * When a thread terminates or is restarted which has a value other than
       * NULL associated with the created thread-specific data key, no
       * destructor function shall be called.
       */
      worker_id = StartWorker( ctx );
      RestartWorker( ctx, worker_id );
      DeleteTask( worker_id );
      worker_id = StartWorker( ctx );
      DeleteTask( worker_id );
      T_eq_u32( ctx->destructor_calls, 0 );
      break;
    }

    case CPthreadReqKeyCreate_Post_Destructor_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Setup( CPthreadReqKeyCreate_Context *ctx )
{
  ctx->runner_id = rtems_task_self();
}

static void CPthreadReqKeyCreate_Setup_Wrap( void *arg )
{
  CPthreadReqKeyCreate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqKeyCreate_Setup( ctx );
}

static void CPthreadReqKeyCreate_Prepare( CPthreadReqKeyCreate_Context *ctx )
{
  ctx->key_obj = KEY_POISON;
  ctx->keys_used = 0;
  ctx->destructor_calls = 0;
  ctx->destructor_arg = NULL;
}

static void CPthreadReqKeyCreate_Action( CPthreadReqKeyCreate_Context *ctx )
{
  ctx->status = pthread_key_create( &ctx->key_obj, ctx->destructor );
}

static void CPthreadReqKeyCreate_Cleanup( CPthreadReqKeyCreate_Context *ctx )
{
  if ( ctx->status == 0 ) {
    T_eq_int( pthread_key_delete( ctx->key_obj ), 0 );
  }

  while ( ctx->keys_used > 0 ) {
    --ctx->keys_used;
    T_eq_int( pthread_key_delete( ctx->keys[ ctx->keys_used ] ), 0 );
  }
}

/* clang-format off */

static const CPthreadReqKeyCreate_Entry
CPthreadReqKeyCreate_Entries[] = {
  { 0, 0, 0, CPthreadReqKeyCreate_Post_Status_Again,
    CPthreadReqKeyCreate_Post_Key_Nop, CPthreadReqKeyCreate_Post_Value_NA,
    CPthreadReqKeyCreate_Post_Destructor_NA },
  { 0, 0, 0, CPthreadReqKeyCreate_Post_Status_Ok,
    CPthreadReqKeyCreate_Post_Key_Created,
    CPthreadReqKeyCreate_Post_Value_Null,
    CPthreadReqKeyCreate_Post_Destructor_Func },
  { 0, 0, 0, CPthreadReqKeyCreate_Post_Status_Ok,
    CPthreadReqKeyCreate_Post_Key_Created,
    CPthreadReqKeyCreate_Post_Value_Null,
    CPthreadReqKeyCreate_Post_Destructor_Null }
};

static const uint8_t
CPthreadReqKeyCreate_Map[] = {
  1, 0, 2, 0
};

/* clang-format on */

static size_t CPthreadReqKeyCreate_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqKeyCreate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqKeyCreate_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqKeyCreate_Fixture = {
  .setup = CPthreadReqKeyCreate_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqKeyCreate_Scope,
  .initial_context = &CPthreadReqKeyCreate_Instance
};

static inline CPthreadReqKeyCreate_Entry CPthreadReqKeyCreate_PopEntry(
  CPthreadReqKeyCreate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqKeyCreate_Entries[ CPthreadReqKeyCreate_Map[ index ] ];
}

static void CPthreadReqKeyCreate_TestVariant(
  CPthreadReqKeyCreate_Context *ctx
)
{
  CPthreadReqKeyCreate_Pre_Destructor_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqKeyCreate_Pre_Keys_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqKeyCreate_Action( ctx );
  CPthreadReqKeyCreate_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqKeyCreate_Post_Key_Check( ctx, ctx->Map.entry.Post_Key );
  CPthreadReqKeyCreate_Post_Value_Check( ctx, ctx->Map.entry.Post_Value );
  CPthreadReqKeyCreate_Post_Destructor_Check(
    ctx,
    ctx->Map.entry.Post_Destructor
  );
}

/**
 * @fn void T_case_body_CPthreadReqKeyCreate( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqKeyCreate, &CPthreadReqKeyCreate_Fixture )
{
  CPthreadReqKeyCreate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqKeyCreate_Pre_Destructor_Func;
    ctx->Map.pcs[ 0 ] < CPthreadReqKeyCreate_Pre_Destructor_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqKeyCreate_Pre_Keys_Available;
      ctx->Map.pcs[ 1 ] < CPthreadReqKeyCreate_Pre_Keys_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqKeyCreate_PopEntry( ctx );
      CPthreadReqKeyCreate_Prepare( ctx );
      CPthreadReqKeyCreate_TestVariant( ctx );
      CPthreadReqKeyCreate_Cleanup( ctx );
    }
  }
}

/** @} */
