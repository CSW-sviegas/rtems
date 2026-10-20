/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqGetspecific
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

#include <pthread.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqGetspecific spec:/c/pthread/req/getspecific
 *
 * @{
 */

typedef enum {
  CPthreadReqGetspecific_Pre_Key_Valid,
  CPthreadReqGetspecific_Pre_Key_Invalid,
  CPthreadReqGetspecific_Pre_Key_NA
} CPthreadReqGetspecific_Pre_Key;

typedef enum {
  CPthreadReqGetspecific_Pre_Value_Set,
  CPthreadReqGetspecific_Pre_Value_Unset,
  CPthreadReqGetspecific_Pre_Value_NA
} CPthreadReqGetspecific_Pre_Value;

typedef enum {
  CPthreadReqGetspecific_Post_Result_Value,
  CPthreadReqGetspecific_Post_Result_Null,
  CPthreadReqGetspecific_Post_Result_NA
} CPthreadReqGetspecific_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Key_NA : 1;
  uint8_t Pre_Value_NA : 1;
  uint8_t Post_Result : 2;
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
   * @brief This member provides the key created for the test.
   */
  pthread_key_t key_value;

  /**
   * @brief This member contains the return value.
   */
  void *retval;

  /**
   * @brief This member provides a value to associate with the key.
   */
  int value;

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
    CPthreadReqGetspecific_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqGetspecific_Context;

static CPthreadReqGetspecific_Context
  CPthreadReqGetspecific_Instance;

static const char * const CPthreadReqGetspecific_PreDesc_Key[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqGetspecific_PreDesc_Value[] = {
  "Set",
  "Unset",
  "NA"
};

static const char * const * const CPthreadReqGetspecific_PreDesc[] = {
  CPthreadReqGetspecific_PreDesc_Key,
  CPthreadReqGetspecific_PreDesc_Value,
  NULL
};

#define KEY_INVALID ( (pthread_key_t) 0xffffffff )

static void CPthreadReqGetspecific_Pre_Key_Prepare(
  CPthreadReqGetspecific_Context *ctx,
  CPthreadReqGetspecific_Pre_Key  state
)
{
  switch ( state ) {
    case CPthreadReqGetspecific_Pre_Key_Valid: {
      /*
       * While the `key` parameter is associated with a thread-specific data
       * key.
       */
      ctx->key = ctx->key_value;
      break;
    }

    case CPthreadReqGetspecific_Pre_Key_Invalid: {
      /*
       * While the `key` parameter is not associated with a thread-specific
       * data key.
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
  int eno;

  switch ( state ) {
    case CPthreadReqGetspecific_Pre_Value_Set: {
      /*
       * While the calling thread has a value associated with the key specified
       * by the `key` parameter.
       */
      eno = pthread_setspecific( ctx->key_value, &ctx->value );
      T_eno_success( eno );
      break;
    }

    case CPthreadReqGetspecific_Pre_Value_Unset: {
      /*
       * While the calling thread has no value associated with the key
       * specified by the `key` parameter.
       */
      /* Nothing to do */
      break;
    }

    case CPthreadReqGetspecific_Pre_Value_NA:
      break;
  }
}

static void CPthreadReqGetspecific_Post_Result_Check(
  CPthreadReqGetspecific_Context    *ctx,
  CPthreadReqGetspecific_Post_Result state
)
{
  switch ( state ) {
    case CPthreadReqGetspecific_Post_Result_Value: {
      /*
       * The return value of pthread_getspecific() shall be equal to the value
       * associated with the key specified by the `key` parameter for the
       * calling thread.
       */
      T_eq_ptr( ctx->retval, &ctx->value );
      break;
    }

    case CPthreadReqGetspecific_Post_Result_Null: {
      /*
       * The return value of pthread_getspecific() shall be equal to NULL.
       */
      T_null( ctx->retval );
      break;
    }

    case CPthreadReqGetspecific_Post_Result_NA:
      break;
  }
}

static void CPthreadReqGetspecific_Prepare(
  CPthreadReqGetspecific_Context *ctx
)
{
  int eno;

  eno = pthread_key_create( &ctx->key_value, NULL );
  T_eno_success( eno );
}

static void CPthreadReqGetspecific_Action(
  CPthreadReqGetspecific_Context *ctx
)
{
  ctx->retval = pthread_getspecific( ctx->key );
}

static void CPthreadReqGetspecific_Cleanup(
  CPthreadReqGetspecific_Context *ctx
)
{
  int eno;

  eno = pthread_key_delete( ctx->key_value );
  T_eno_success( eno );
}

static const CPthreadReqGetspecific_Entry
CPthreadReqGetspecific_Entries[] = {
  { 0, 0, 1, CPthreadReqGetspecific_Post_Result_Null },
  { 0, 0, 0, CPthreadReqGetspecific_Post_Result_Value },
  { 0, 0, 0, CPthreadReqGetspecific_Post_Result_Null }
};

static const uint8_t
CPthreadReqGetspecific_Map[] = {
  1, 2, 0, 0
};

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
  .setup = NULL,
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
  return CPthreadReqGetspecific_Entries[
    CPthreadReqGetspecific_Map[ index ]
  ];
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
}

static void CPthreadReqGetspecific_TestVariant(
  CPthreadReqGetspecific_Context *ctx
)
{
  CPthreadReqGetspecific_Pre_Key_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqGetspecific_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqGetspecific_Action( ctx );
  CPthreadReqGetspecific_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
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
      ctx->Map.pci[ 1 ] = CPthreadReqGetspecific_Pre_Value_Set;
      ctx->Map.pci[ 1 ] < CPthreadReqGetspecific_Pre_Value_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqGetspecific_PopEntry( ctx );
      CPthreadReqGetspecific_SetPreConditionStates( ctx );
      CPthreadReqGetspecific_Prepare( ctx );
      CPthreadReqGetspecific_TestVariant( ctx );
      CPthreadReqGetspecific_Cleanup( ctx );
    }
  }
}

/** @} */
