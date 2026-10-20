/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqKeyCreate
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqKeyCreate spec:/c/pthread/req/key-create
 *
 * @{
 */

typedef enum {
  CPthreadReqKeyCreate_Pre_KeyResources_Available,
  CPthreadReqKeyCreate_Pre_KeyResources_Exhausted,
  CPthreadReqKeyCreate_Pre_KeyResources_NA
} CPthreadReqKeyCreate_Pre_KeyResources;

typedef enum {
  CPthreadReqKeyCreate_Pre_Destructor_Valid,
  CPthreadReqKeyCreate_Pre_Destructor_Null,
  CPthreadReqKeyCreate_Pre_Destructor_NA
} CPthreadReqKeyCreate_Pre_Destructor;

typedef enum {
  CPthreadReqKeyCreate_Post_Status_Zero,
  CPthreadReqKeyCreate_Post_Status_EAGAIN,
  CPthreadReqKeyCreate_Post_Status_NA
} CPthreadReqKeyCreate_Post_Status;

typedef enum {
  CPthreadReqKeyCreate_Post_Destructor_Called,
  CPthreadReqKeyCreate_Post_Destructor_NotCalled,
  CPthreadReqKeyCreate_Post_Destructor_NA
} CPthreadReqKeyCreate_Post_Destructor;

typedef enum {
  CPthreadReqKeyCreate_Post_Key_Valid,
  CPthreadReqKeyCreate_Post_Key_Nop,
  CPthreadReqKeyCreate_Post_Key_NA
} CPthreadReqKeyCreate_Post_Key;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_KeyResources_NA : 1;
  uint16_t Pre_Destructor_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Destructor : 2;
  uint16_t Post_Key : 2;
} CPthreadReqKeyCreate_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/key-create test case.
 */
typedef struct {
  /**
   * @brief This member contains the key created by the action.
   */
  pthread_key_t key;

  /**
   * @brief This member contains the keys created to use up the available ones.
   */
  pthread_key_t keys[ 32 ];

  /**
   * @brief This member contains the count of valid elements of the keys
   *   member.
   */
  size_t used;

  /**
   * @brief This member specifies the `destructor` parameter value.
   */
  void ( *destructor )( void * );

  /**
   * @brief This member contains the count of destructor function calls.
   */
  uint32_t destructor_calls;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

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

static CPthreadReqKeyCreate_Context
  CPthreadReqKeyCreate_Instance;

static const char * const CPthreadReqKeyCreate_PreDesc_KeyResources[] = {
  "Available",
  "Exhausted",
  "NA"
};

static const char * const CPthreadReqKeyCreate_PreDesc_Destructor[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqKeyCreate_PreDesc[] = {
  CPthreadReqKeyCreate_PreDesc_KeyResources,
  CPthreadReqKeyCreate_PreDesc_Destructor,
  NULL
};

#define KEY_UNSET 0xffffffff

typedef CPthreadReqKeyCreate_Context Context;

static void Destructor( void *value )
{
  Context *ctx;

  ctx = value;
  ++ctx->destructor_calls;
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;
  int      eno;

  ctx = (Context *) arg;

  eno = pthread_setspecific( ctx->key, ctx );
  T_eq_int( eno, 0 );

  SendEvents( ctx->runner_id, RTEMS_EVENT_0 );

  while ( true ) {
    (void) ReceiveAnyEvents();
  }
}

static void RunWorker( Context *ctx )
{
  rtems_id worker_id;

  worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( worker_id, Worker, ctx );
  (void) ReceiveAnyEvents();

  /*
   * The task deletion waits until the worker terminated, so the
   * destructors were run when DeleteTask() returns.
   */
  DeleteTask( worker_id );
}

static void CPthreadReqKeyCreate_Pre_KeyResources_Prepare(
  CPthreadReqKeyCreate_Context         *ctx,
  CPthreadReqKeyCreate_Pre_KeyResources state
)
{
  switch ( state ) {
    case CPthreadReqKeyCreate_Pre_KeyResources_Available: {
      /*
       * While the system has resources to create another thread-specific data
       * key.
       */
      /* Ensured by the test suite configuration */
      break;
    }

    case CPthreadReqKeyCreate_Pre_KeyResources_Exhausted: {
      /*
       * While the system has no resources to create another thread-specific
       * data key.
       */
      while (
        ctx->used < RTEMS_ARRAY_SIZE( ctx->keys ) &&
        pthread_key_create( &ctx->keys[ ctx->used ], NULL ) == 0
      ) {
        ++ctx->used;
      }

      T_lt_sz( ctx->used, RTEMS_ARRAY_SIZE( ctx->keys ) );
      break;
    }

    case CPthreadReqKeyCreate_Pre_KeyResources_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Pre_Destructor_Prepare(
  CPthreadReqKeyCreate_Context       *ctx,
  CPthreadReqKeyCreate_Pre_Destructor state
)
{
  switch ( state ) {
    case CPthreadReqKeyCreate_Pre_Destructor_Valid: {
      /*
       * While the `destructor` parameter references a destructor function.
       */
      ctx->destructor = Destructor;
      break;
    }

    case CPthreadReqKeyCreate_Pre_Destructor_Null: {
      /*
       * While the `destructor` parameter is equal to NULL.
       */
      ctx->destructor = NULL;
      break;
    }

    case CPthreadReqKeyCreate_Pre_Destructor_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Post_Status_Check(
  CPthreadReqKeyCreate_Context    *ctx,
  CPthreadReqKeyCreate_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqKeyCreate_Post_Status_Zero: {
      /*
       * The return value of pthread_key_create() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqKeyCreate_Post_Status_EAGAIN: {
      /*
       * The return value of pthread_key_create() shall be equal to EAGAIN.
       */
      T_eq_int( ctx->retval, EAGAIN );
      break;
    }

    case CPthreadReqKeyCreate_Post_Status_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Post_Destructor_Check(
  CPthreadReqKeyCreate_Context        *ctx,
  CPthreadReqKeyCreate_Post_Destructor state
)
{
  switch ( state ) {
    case CPthreadReqKeyCreate_Post_Destructor_Called: {
      /*
       * When a thread which has a value other than NULL associated with the
       * created thread-specific data key terminates, the destructor function
       * specified by the `destructor` parameter shall be called once with this
       * value as its argument.
       */
      RunWorker( ctx );
      T_eq_u32( ctx->destructor_calls, 1 );
      break;
    }

    case CPthreadReqKeyCreate_Post_Destructor_NotCalled: {
      /*
       * When a thread which has a value associated with the created
       * thread-specific data key terminates, no destructor function shall be
       * called for this value.
       */
      RunWorker( ctx );
      T_eq_u32( ctx->destructor_calls, 0 );
      break;
    }

    case CPthreadReqKeyCreate_Post_Destructor_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Post_Key_Check(
  CPthreadReqKeyCreate_Context *ctx,
  CPthreadReqKeyCreate_Post_Key state
)
{
  switch ( state ) {
    case CPthreadReqKeyCreate_Post_Key_Valid: {
      /*
       * The value of the object referenced by the `key` parameter shall
       * identify the created thread-specific data key.
       */
      T_ne_u32( ctx->key, KEY_UNSET );
      T_eq_int( pthread_key_delete( ctx->key ), 0 );
      break;
    }

    case CPthreadReqKeyCreate_Post_Key_Nop: {
      /*
       * The object referenced by the `key` parameter shall not be modified by
       * the pthread_key_create() call.
       */
      T_eq_u32( ctx->key, KEY_UNSET );
      break;
    }

    case CPthreadReqKeyCreate_Post_Key_NA:
      break;
  }
}

static void CPthreadReqKeyCreate_Prepare( CPthreadReqKeyCreate_Context *ctx )
{
  ctx->key = KEY_UNSET;
  ctx->used = 0;
  ctx->destructor_calls = 0;
  ctx->runner_id = rtems_task_self();
}

static void CPthreadReqKeyCreate_Action( CPthreadReqKeyCreate_Context *ctx )
{
  ctx->retval = pthread_key_create( &ctx->key, ctx->destructor );
}

static void CPthreadReqKeyCreate_Cleanup( CPthreadReqKeyCreate_Context *ctx )
{
  while ( ctx->used > 0 ) {
    --ctx->used;
    T_eq_int( pthread_key_delete( ctx->keys[ ctx->used ] ), 0 );
  }
}

static const CPthreadReqKeyCreate_Entry
CPthreadReqKeyCreate_Entries[] = {
  { 0, 0, 0, CPthreadReqKeyCreate_Post_Status_EAGAIN,
    CPthreadReqKeyCreate_Post_Destructor_NA, CPthreadReqKeyCreate_Post_Key_Nop },
  { 0, 0, 0, CPthreadReqKeyCreate_Post_Status_Zero,
    CPthreadReqKeyCreate_Post_Destructor_Called,
    CPthreadReqKeyCreate_Post_Key_Valid },
  { 0, 0, 0, CPthreadReqKeyCreate_Post_Status_Zero,
    CPthreadReqKeyCreate_Post_Destructor_NotCalled,
    CPthreadReqKeyCreate_Post_Key_Valid }
};

static const uint8_t
CPthreadReqKeyCreate_Map[] = {
  1, 2, 0, 0
};

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
  .setup = NULL,
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
  return CPthreadReqKeyCreate_Entries[
    CPthreadReqKeyCreate_Map[ index ]
  ];
}

static void CPthreadReqKeyCreate_TestVariant(
  CPthreadReqKeyCreate_Context *ctx
)
{
  CPthreadReqKeyCreate_Pre_KeyResources_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqKeyCreate_Pre_Destructor_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqKeyCreate_Action( ctx );
  CPthreadReqKeyCreate_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqKeyCreate_Post_Destructor_Check(
    ctx,
    ctx->Map.entry.Post_Destructor
  );
  CPthreadReqKeyCreate_Post_Key_Check( ctx, ctx->Map.entry.Post_Key );
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
    ctx->Map.pcs[ 0 ] = CPthreadReqKeyCreate_Pre_KeyResources_Available;
    ctx->Map.pcs[ 0 ] < CPthreadReqKeyCreate_Pre_KeyResources_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqKeyCreate_Pre_Destructor_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqKeyCreate_Pre_Destructor_NA;
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
