/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSetspecific
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
 * @defgroup CPthreadReqSetspecific spec:/c/pthread/req/setspecific
 *
 * @{
 */

typedef enum {
  CPthreadReqSetspecific_Pre_Key_Valid,
  CPthreadReqSetspecific_Pre_Key_Invalid,
  CPthreadReqSetspecific_Pre_Key_NA
} CPthreadReqSetspecific_Pre_Key;

typedef enum {
  CPthreadReqSetspecific_Pre_Value_NonNull,
  CPthreadReqSetspecific_Pre_Value_Null,
  CPthreadReqSetspecific_Pre_Value_NA
} CPthreadReqSetspecific_Pre_Value;

typedef enum {
  CPthreadReqSetspecific_Pre_Current_Set,
  CPthreadReqSetspecific_Pre_Current_Unset,
  CPthreadReqSetspecific_Pre_Current_NA
} CPthreadReqSetspecific_Pre_Current;

typedef enum {
  CPthreadReqSetspecific_Pre_Memory_Available,
  CPthreadReqSetspecific_Pre_Memory_None,
  CPthreadReqSetspecific_Pre_Memory_NA
} CPthreadReqSetspecific_Pre_Memory;

typedef enum {
  CPthreadReqSetspecific_Post_Status_Zero,
  CPthreadReqSetspecific_Post_Status_EINVAL,
  CPthreadReqSetspecific_Post_Status_ENOMEM,
  CPthreadReqSetspecific_Post_Status_NA
} CPthreadReqSetspecific_Post_Status;

typedef enum {
  CPthreadReqSetspecific_Post_Value_Stored,
  CPthreadReqSetspecific_Post_Value_Cleared,
  CPthreadReqSetspecific_Post_Value_Nop,
  CPthreadReqSetspecific_Post_Value_NA
} CPthreadReqSetspecific_Post_Value;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Key_NA : 1;
  uint16_t Pre_Value_NA : 1;
  uint16_t Pre_Current_NA : 1;
  uint16_t Pre_Memory_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Value : 2;
} CPthreadReqSetspecific_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/setspecific test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `key` parameter value.
   */
  pthread_key_t key;

  /**
   * @brief This member specifies the `value` parameter value.
   */
  const void *value;

  /**
   * @brief This member provides the key created for the test.
   */
  pthread_key_t key_value;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the keys created by the worker task to use up
   *   the inactive key value pair objects.
   */
  pthread_key_t extra_keys[ 32 ];

  /**
   * @brief This member contains the count of valid elements of the extra keys
   *   member.
   */
  size_t extra_used;

  /**
   * @brief This member contains the status of the last key operation of the
   *   worker task.
   */
  int extra_status;

  /**
   * @brief This member provides the value associated with the key by the
   *   ``Current`` pre-condition.
   */
  int current_value;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    CPthreadReqSetspecific_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSetspecific_Context;

static CPthreadReqSetspecific_Context
  CPthreadReqSetspecific_Instance;

static const char * const CPthreadReqSetspecific_PreDesc_Key[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqSetspecific_PreDesc_Value[] = {
  "NonNull",
  "Null",
  "NA"
};

static const char * const CPthreadReqSetspecific_PreDesc_Current[] = {
  "Set",
  "Unset",
  "NA"
};

static const char * const CPthreadReqSetspecific_PreDesc_Memory[] = {
  "Available",
  "None",
  "NA"
};

static const char * const * const CPthreadReqSetspecific_PreDesc[] = {
  CPthreadReqSetspecific_PreDesc_Key,
  CPthreadReqSetspecific_PreDesc_Value,
  CPthreadReqSetspecific_PreDesc_Current,
  CPthreadReqSetspecific_PreDesc_Memory,
  NULL
};

#define KEY_INVALID ( (pthread_key_t) 0xffffffff )

typedef CPthreadReqSetspecific_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  /*
   * Associate a value with the key of the test first.  Then create keys and
   * associate values with them until no inactive key value pair object is
   * available or no key can be created.  The number of keys is limited by
   * CONFIGURE_MAXIMUM_POSIX_KEYS, so the key value pair objects may be
   * used up before the key objects.  The loop is bounded by the size of the
   * extra keys member.
   */
  ctx->extra_status = pthread_setspecific( ctx->key_value, ctx );

  while ( ctx->extra_status == 0 &&
          ctx->extra_used < RTEMS_ARRAY_SIZE( ctx->extra_keys ) ) {
    pthread_key_t *key;

    key = &ctx->extra_keys[ ctx->extra_used ];
    ctx->extra_status = pthread_key_create( key, NULL );

    if ( ctx->extra_status != 0 ) {
      break;
    }

    ++ctx->extra_used;
    ctx->extra_status = pthread_setspecific( *key, ctx );

    if ( ctx->extra_status != 0 ) {
      break;
    }
  }

  SendEvents( ctx->runner_id, RTEMS_EVENT_0 );

  while ( true ) {
    (void) ReceiveAnyEvents();
  }
}

static void CPthreadReqSetspecific_Pre_Key_Prepare(
  CPthreadReqSetspecific_Context *ctx,
  CPthreadReqSetspecific_Pre_Key  state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Pre_Key_Valid: {
      /*
       * While the `key` parameter is associated with a thread-specific data
       * key.
       */
      ctx->key = ctx->key_value;
      break;
    }

    case CPthreadReqSetspecific_Pre_Key_Invalid: {
      /*
       * While the `key` parameter is not associated with a thread-specific
       * data key.
       */
      ctx->key = KEY_INVALID;
      break;
    }

    case CPthreadReqSetspecific_Pre_Key_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Pre_Value_Prepare(
  CPthreadReqSetspecific_Context  *ctx,
  CPthreadReqSetspecific_Pre_Value state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Pre_Value_NonNull: {
      /*
       * While the `value` parameter is not equal to NULL.
       */
      ctx->value = ctx;
      break;
    }

    case CPthreadReqSetspecific_Pre_Value_Null: {
      /*
       * While the `value` parameter is equal to NULL.
       */
      ctx->value = NULL;
      break;
    }

    case CPthreadReqSetspecific_Pre_Value_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Pre_Current_Prepare(
  CPthreadReqSetspecific_Context    *ctx,
  CPthreadReqSetspecific_Pre_Current state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqSetspecific_Pre_Current_Set: {
      /*
       * While the calling thread has a value associated with the
       * thread-specific data key.
       */
      eno = pthread_setspecific( ctx->key_value, &ctx->current_value );
      T_eq_int( eno, 0 );
      break;
    }

    case CPthreadReqSetspecific_Pre_Current_Unset: {
      /*
       * While the calling thread has no value associated with the
       * thread-specific data key.
       */
      /* Nothing to do */
      break;
    }

    case CPthreadReqSetspecific_Pre_Current_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Pre_Memory_Prepare(
  CPthreadReqSetspecific_Context   *ctx,
  CPthreadReqSetspecific_Pre_Memory state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Pre_Memory_Available: {
      /*
       * While the system has at least one inactive key value pair object
       * available, see CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      /* Nothing to do */
      break;
    }

    case CPthreadReqSetspecific_Pre_Memory_None: {
      /*
       * While the system has no inactive key value pair object available, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
      StartTask( ctx->worker_id, Worker, ctx );
      (void) ReceiveAnyEvents();

      /*
       * Ensure that the worker stopped due to a failed operation and not due
       * to the bound given by the size of the extra keys member.
       */
      T_ne_int( ctx->extra_status, 0 );
      T_lt_sz( ctx->extra_used, RTEMS_ARRAY_SIZE( ctx->extra_keys ) );
      break;
    }

    case CPthreadReqSetspecific_Pre_Memory_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Post_Status_Check(
  CPthreadReqSetspecific_Context    *ctx,
  CPthreadReqSetspecific_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Post_Status_Zero: {
      /*
       * The return value of pthread_setspecific() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqSetspecific_Post_Status_EINVAL: {
      /*
       * The return value of pthread_setspecific() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqSetspecific_Post_Status_ENOMEM: {
      /*
       * The return value of pthread_setspecific() shall be equal to ENOMEM.
       */
      T_eq_int( ctx->retval, ENOMEM );
      break;
    }

    case CPthreadReqSetspecific_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Post_Value_Check(
  CPthreadReqSetspecific_Context   *ctx,
  CPthreadReqSetspecific_Post_Value state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Post_Value_Stored: {
      /*
       * The value associated with the thread-specific data key for the calling
       * thread shall be equal to the `value` parameter.
       */
      T_eq_ptr( pthread_getspecific( ctx->key_value ), ctx->value );
      break;
    }

    case CPthreadReqSetspecific_Post_Value_Cleared: {
      /*
       * The calling thread shall have no value associated with the
       * thread-specific data key.
       */
      T_null( pthread_getspecific( ctx->key_value ) );
      break;
    }

    case CPthreadReqSetspecific_Post_Value_Nop: {
      /*
       * The value associated with the thread-specific data key for the calling
       * thread shall not be changed.
       */
      T_null( pthread_getspecific( ctx->key_value ) );
      break;
    }

    case CPthreadReqSetspecific_Post_Value_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Prepare(
  CPthreadReqSetspecific_Context *ctx
)
{
  int eno;

  ctx->worker_id = 0;
  ctx->extra_used = 0;
  ctx->extra_status = 0;
  ctx->runner_id = rtems_task_self();

  eno = pthread_key_create( &ctx->key_value, NULL );
  T_eq_int( eno, 0 );
}

static void CPthreadReqSetspecific_Action(
  CPthreadReqSetspecific_Context *ctx
)
{
  ctx->retval = pthread_setspecific( ctx->key, ctx->value );
}

static void CPthreadReqSetspecific_Cleanup(
  CPthreadReqSetspecific_Context *ctx
)
{
  int    eno;
  size_t i;

  if ( ctx->key_value != KEY_INVALID ) {
    eno = pthread_key_delete( ctx->key_value );
    T_eq_int( eno, 0 );
  }

  for ( i = 0; i < ctx->extra_used; ++i ) {
    eno = pthread_key_delete( ctx->extra_keys[ i ] );
    T_eq_int( eno, 0 );
  }

  DeleteTask( ctx->worker_id );
}

static const CPthreadReqSetspecific_Entry
CPthreadReqSetspecific_Entries[] = {
  { 0, 0, 0, 1, 1, CPthreadReqSetspecific_Post_Status_EINVAL,
    CPthreadReqSetspecific_Post_Value_NA },
  { 0, 0, 0, 0, 1, CPthreadReqSetspecific_Post_Status_Zero,
    CPthreadReqSetspecific_Post_Value_Cleared },
  { 0, 0, 0, 0, 1, CPthreadReqSetspecific_Post_Status_Zero,
    CPthreadReqSetspecific_Post_Value_Stored },
  { 0, 0, 0, 0, 0, CPthreadReqSetspecific_Post_Status_Zero,
    CPthreadReqSetspecific_Post_Value_Stored },
  { 0, 0, 0, 0, 0, CPthreadReqSetspecific_Post_Status_ENOMEM,
    CPthreadReqSetspecific_Post_Value_Nop }
};

static const uint8_t
CPthreadReqSetspecific_Map[] = {
  2, 2, 3, 4, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
};

static size_t CPthreadReqSetspecific_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSetspecific_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqSetspecific_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqSetspecific_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqSetspecific_Scope,
  .initial_context = &CPthreadReqSetspecific_Instance
};

static inline CPthreadReqSetspecific_Entry CPthreadReqSetspecific_PopEntry(
  CPthreadReqSetspecific_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSetspecific_Entries[
    CPthreadReqSetspecific_Map[ index ]
  ];
}

static void CPthreadReqSetspecific_SetPreConditionStates(
  CPthreadReqSetspecific_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Current_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqSetspecific_Pre_Current_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Memory_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqSetspecific_Pre_Memory_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CPthreadReqSetspecific_TestVariant(
  CPthreadReqSetspecific_Context *ctx
)
{
  CPthreadReqSetspecific_Pre_Key_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSetspecific_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSetspecific_Pre_Current_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqSetspecific_Pre_Memory_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqSetspecific_Action( ctx );
  CPthreadReqSetspecific_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqSetspecific_Post_Value_Check( ctx, ctx->Map.entry.Post_Value );
}

/**
 * @fn void T_case_body_CPthreadReqSetspecific( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqSetspecific, &CPthreadReqSetspecific_Fixture )
{
  CPthreadReqSetspecific_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqSetspecific_Pre_Key_Valid;
    ctx->Map.pci[ 0 ] < CPthreadReqSetspecific_Pre_Key_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqSetspecific_Pre_Value_NonNull;
      ctx->Map.pci[ 1 ] < CPthreadReqSetspecific_Pre_Value_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqSetspecific_Pre_Current_Set;
        ctx->Map.pci[ 2 ] < CPthreadReqSetspecific_Pre_Current_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqSetspecific_Pre_Memory_Available;
          ctx->Map.pci[ 3 ] < CPthreadReqSetspecific_Pre_Memory_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqSetspecific_PopEntry( ctx );
          CPthreadReqSetspecific_SetPreConditionStates( ctx );
          CPthreadReqSetspecific_Prepare( ctx );
          CPthreadReqSetspecific_TestVariant( ctx );
          CPthreadReqSetspecific_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
