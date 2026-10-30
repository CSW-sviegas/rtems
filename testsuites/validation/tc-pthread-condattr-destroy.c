/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrDestroy
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
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondattrDestroy spec:/c/pthread/req/condattr-destroy
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrDestroy_Pre_Attr_Valid,
  CPthreadReqCondattrDestroy_Pre_Attr_Null,
  CPthreadReqCondattrDestroy_Pre_Attr_Invalid,
  CPthreadReqCondattrDestroy_Pre_Attr_NA
} CPthreadReqCondattrDestroy_Pre_Attr;

typedef enum {
  CPthreadReqCondattrDestroy_Post_Status_Zero,
  CPthreadReqCondattrDestroy_Post_Status_EINVAL,
  CPthreadReqCondattrDestroy_Post_Status_NA
} CPthreadReqCondattrDestroy_Post_Status;

typedef enum {
  CPthreadReqCondattrDestroy_Post_AttrObj_Destroyed,
  CPthreadReqCondattrDestroy_Post_AttrObj_Nop,
  CPthreadReqCondattrDestroy_Post_AttrObj_NA
} CPthreadReqCondattrDestroy_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqCondattrDestroy_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/condattr-destroy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_condattr_t *attr;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the attribute object.
   */
  pthread_condattr_t attr_obj;

  /**
   * @brief This member contains a copy of the attribute object taken before
   *   the action.
   */
  pthread_condattr_t attr_copy;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    CPthreadReqCondattrDestroy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondattrDestroy_Context;

static CPthreadReqCondattrDestroy_Context CPthreadReqCondattrDestroy_Instance;

static const char *const CPthreadReqCondattrDestroy_PreDesc_Attr[] =
  { "Valid", "Null", "Invalid", "NA" };

static const char *const *const CPthreadReqCondattrDestroy_PreDesc[] = {
  CPthreadReqCondattrDestroy_PreDesc_Attr,
  NULL
};

static void CPthreadReqCondattrDestroy_Pre_Attr_Prepare(
  CPthreadReqCondattrDestroy_Context *ctx,
  CPthreadReqCondattrDestroy_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrDestroy_Pre_Attr_Valid: {
      /*
       * While the condition variable attributes object referenced by the
       * `attr` parameter is valid.
       */
      ctx->attr = &ctx->attr_obj;
      T_eq_int( pthread_condattr_init( &ctx->attr_obj ), 0 );
      break;
    }

    case CPthreadReqCondattrDestroy_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqCondattrDestroy_Pre_Attr_Invalid: {
      /*
       * While the condition variable attributes object referenced by the
       * `attr` parameter is invalid.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqCondattrDestroy_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrDestroy_Post_Status_Check(
  CPthreadReqCondattrDestroy_Context    *ctx,
  CPthreadReqCondattrDestroy_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrDestroy_Post_Status_Zero: {
      /*
       * The return value of pthread_condattr_destroy() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_Status_EINVAL: {
      /*
       * The return value of pthread_condattr_destroy() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrDestroy_Post_AttrObj_Check(
  CPthreadReqCondattrDestroy_Context     *ctx,
  CPthreadReqCondattrDestroy_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqCondattrDestroy_Post_AttrObj_Destroyed: {
      /*
       * The condition variable attributes object referenced by the `attr`
       * parameter shall be invalid.
       */
      T_eq_int( pthread_condattr_destroy( &ctx->attr_obj ), EINVAL );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_AttrObj_Nop: {
      /*
       * The condition variable attributes object referenced by the `attr`
       * parameter shall not be modified.
       */
      T_eq_int(
        memcmp( &ctx->attr_obj, &ctx->attr_copy, sizeof( ctx->attr_obj ) ),
        0
      );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqCondattrDestroy_Action(
  CPthreadReqCondattrDestroy_Context *ctx
)
{
  ctx->attr_copy = ctx->attr_obj;
  ctx->retval = pthread_condattr_destroy( ctx->attr );
}

/* clang-format off */

static const CPthreadReqCondattrDestroy_Entry
CPthreadReqCondattrDestroy_Entries[] = {
  { 0, 0, CPthreadReqCondattrDestroy_Post_Status_EINVAL,
    CPthreadReqCondattrDestroy_Post_AttrObj_Nop },
  { 0, 0, CPthreadReqCondattrDestroy_Post_Status_Zero,
    CPthreadReqCondattrDestroy_Post_AttrObj_Destroyed }
};

static const uint8_t
CPthreadReqCondattrDestroy_Map[] = {
  1, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondattrDestroy_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqCondattrDestroy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondattrDestroy_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondattrDestroy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondattrDestroy_Scope,
  .initial_context = &CPthreadReqCondattrDestroy_Instance
};

static inline CPthreadReqCondattrDestroy_Entry
CPthreadReqCondattrDestroy_PopEntry( CPthreadReqCondattrDestroy_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondattrDestroy_Entries
    [ CPthreadReqCondattrDestroy_Map[ index ] ];
}

static void CPthreadReqCondattrDestroy_TestVariant(
  CPthreadReqCondattrDestroy_Context *ctx
)
{
  CPthreadReqCondattrDestroy_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrDestroy_Action( ctx );
  CPthreadReqCondattrDestroy_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrDestroy_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqCondattrDestroy( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondattrDestroy,
  &CPthreadReqCondattrDestroy_Fixture
)
{
  CPthreadReqCondattrDestroy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqCondattrDestroy_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqCondattrDestroy_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqCondattrDestroy_PopEntry( ctx );
    CPthreadReqCondattrDestroy_TestVariant( ctx );
  }
}

/** @} */
