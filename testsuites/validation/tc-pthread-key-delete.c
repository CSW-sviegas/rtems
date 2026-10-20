/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqKeyDelete
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

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqKeyDelete spec:/c/pthread/req/key-delete
 *
 * @{
 */

typedef enum {
  CPthreadReqKeyDelete_Pre_Key_Valid,
  CPthreadReqKeyDelete_Pre_Key_Invalid,
  CPthreadReqKeyDelete_Pre_Key_NA
} CPthreadReqKeyDelete_Pre_Key;

typedef enum {
  CPthreadReqKeyDelete_Pre_Value_Set,
  CPthreadReqKeyDelete_Pre_Value_Unset,
  CPthreadReqKeyDelete_Pre_Value_NA
} CPthreadReqKeyDelete_Pre_Value;

typedef enum {
  CPthreadReqKeyDelete_Post_Status_Zero,
  CPthreadReqKeyDelete_Post_Status_EINVAL,
  CPthreadReqKeyDelete_Post_Status_NA
} CPthreadReqKeyDelete_Post_Status;

typedef enum {
  CPthreadReqKeyDelete_Post_Key_Deleted,
  CPthreadReqKeyDelete_Post_Key_Nop,
  CPthreadReqKeyDelete_Post_Key_NA
} CPthreadReqKeyDelete_Post_Key;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Key_NA : 1;
  uint8_t Pre_Value_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Key : 2;
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
   * @brief This member provides the key created for the test.
   */
  pthread_key_t key_value;

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
    CPthreadReqKeyDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqKeyDelete_Context;

static CPthreadReqKeyDelete_Context
  CPthreadReqKeyDelete_Instance;

static const char * const CPthreadReqKeyDelete_PreDesc_Key[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqKeyDelete_PreDesc_Value[] = {
  "Set",
  "Unset",
  "NA"
};

static const char * const * const CPthreadReqKeyDelete_PreDesc[] = {
  CPthreadReqKeyDelete_PreDesc_Key,
  CPthreadReqKeyDelete_PreDesc_Value,
  NULL
};

#define KEY_INVALID ( (pthread_key_t) 0xffffffff )

static void CPthreadReqKeyDelete_Pre_Key_Prepare(
  CPthreadReqKeyDelete_Context *ctx,
  CPthreadReqKeyDelete_Pre_Key  state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Pre_Key_Valid: {
      /*
       * While the `key` parameter is associated with a thread-specific data
       * key.
       */
      ctx->key = ctx->key_value;
      break;
    }

    case CPthreadReqKeyDelete_Pre_Key_Invalid: {
      /*
       * While the `key` parameter is not associated with a thread-specific
       * data key.
       */
      ctx->key = KEY_INVALID;
      break;
    }

    case CPthreadReqKeyDelete_Pre_Key_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Pre_Value_Prepare(
  CPthreadReqKeyDelete_Context  *ctx,
  CPthreadReqKeyDelete_Pre_Value state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqKeyDelete_Pre_Value_Set: {
      /*
       * While at least one thread has a value associated with the
       * thread-specific data key.
       */
      eno = pthread_setspecific( ctx->key_value, ctx );
      T_eq_int( eno, 0 );
      break;
    }

    case CPthreadReqKeyDelete_Pre_Value_Unset: {
      /*
       * While no thread has a value associated with the thread-specific data
       * key.
       */
      /* Nothing to do */
      break;
    }

    case CPthreadReqKeyDelete_Pre_Value_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Post_Status_Check(
  CPthreadReqKeyDelete_Context    *ctx,
  CPthreadReqKeyDelete_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqKeyDelete_Post_Status_Zero: {
      /*
       * The return value of pthread_key_delete() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqKeyDelete_Post_Status_EINVAL: {
      /*
       * The return value of pthread_key_delete() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
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
  int eno;

  switch ( state ) {
    case CPthreadReqKeyDelete_Post_Key_Deleted: {
      /*
       * The thread-specific data key shall be deleted.
       */
      eno = pthread_setspecific( ctx->key_value, NULL );
      T_eq_int( eno, EINVAL );
      ctx->key_value = KEY_INVALID;
      break;
    }

    case CPthreadReqKeyDelete_Post_Key_Nop: {
      /*
       * No thread-specific data key shall be deleted by the
       * pthread_key_delete() call.
       */
      eno = pthread_setspecific( ctx->key_value, NULL );
      T_eq_int( eno, 0 );
      break;
    }

    case CPthreadReqKeyDelete_Post_Key_NA:
      break;
  }
}

static void CPthreadReqKeyDelete_Prepare( CPthreadReqKeyDelete_Context *ctx )
{
  int eno;

  eno = pthread_key_create( &ctx->key_value, NULL );
  T_eq_int( eno, 0 );
}

static void CPthreadReqKeyDelete_Action( CPthreadReqKeyDelete_Context *ctx )
{
  ctx->retval = pthread_key_delete( ctx->key );
}

static void CPthreadReqKeyDelete_Cleanup( CPthreadReqKeyDelete_Context *ctx )
{
  int eno;

  if ( ctx->key_value != KEY_INVALID ) {
    eno = pthread_key_delete( ctx->key_value );
    T_eq_int( eno, 0 );
  }
}

static const CPthreadReqKeyDelete_Entry
CPthreadReqKeyDelete_Entries[] = {
  { 0, 0, 0, CPthreadReqKeyDelete_Post_Status_Zero,
    CPthreadReqKeyDelete_Post_Key_Deleted },
  { 0, 0, 1, CPthreadReqKeyDelete_Post_Status_EINVAL,
    CPthreadReqKeyDelete_Post_Key_Nop }
};

static const uint8_t
CPthreadReqKeyDelete_Map[] = {
  0, 0, 1, 1
};

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
  .setup = NULL,
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
  return CPthreadReqKeyDelete_Entries[
    CPthreadReqKeyDelete_Map[ index ]
  ];
}

static void CPthreadReqKeyDelete_SetPreConditionStates(
  CPthreadReqKeyDelete_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Value_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqKeyDelete_Pre_Value_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CPthreadReqKeyDelete_TestVariant(
  CPthreadReqKeyDelete_Context *ctx
)
{
  CPthreadReqKeyDelete_Pre_Key_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqKeyDelete_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqKeyDelete_Action( ctx );
  CPthreadReqKeyDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqKeyDelete_Post_Key_Check( ctx, ctx->Map.entry.Post_Key );
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
      ctx->Map.pci[ 1 ] = CPthreadReqKeyDelete_Pre_Value_Set;
      ctx->Map.pci[ 1 ] < CPthreadReqKeyDelete_Pre_Value_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqKeyDelete_PopEntry( ctx );
      CPthreadReqKeyDelete_SetPreConditionStates( ctx );
      CPthreadReqKeyDelete_Prepare( ctx );
      CPthreadReqKeyDelete_TestVariant( ctx );
      CPthreadReqKeyDelete_Cleanup( ctx );
    }
  }
}

/** @} */
