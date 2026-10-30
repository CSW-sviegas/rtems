/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrSetpshared
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

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondattrSetpshared \
 *   spec:/c/pthread/req/condattr-setpshared
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrSetpshared_Pre_Attr_Valid,
  CPthreadReqCondattrSetpshared_Pre_Attr_Null,
  CPthreadReqCondattrSetpshared_Pre_Attr_NA
} CPthreadReqCondattrSetpshared_Pre_Attr;

typedef enum {
  CPthreadReqCondattrSetpshared_Pre_Pshared_Shared,
  CPthreadReqCondattrSetpshared_Pre_Pshared_Private,
  CPthreadReqCondattrSetpshared_Pre_Pshared_Invalid,
  CPthreadReqCondattrSetpshared_Pre_Pshared_NA
} CPthreadReqCondattrSetpshared_Pre_Pshared;

typedef enum {
  CPthreadReqCondattrSetpshared_Post_Status_Zero,
  CPthreadReqCondattrSetpshared_Post_Status_EINVAL,
  CPthreadReqCondattrSetpshared_Post_Status_NA
} CPthreadReqCondattrSetpshared_Post_Status;

typedef enum {
  CPthreadReqCondattrSetpshared_Post_AttrObj_Set,
  CPthreadReqCondattrSetpshared_Post_AttrObj_Nop,
  CPthreadReqCondattrSetpshared_Post_AttrObj_NA
} CPthreadReqCondattrSetpshared_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_Pshared_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqCondattrSetpshared_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/condattr-setpshared test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_condattr_t *attr;

  /**
   * @brief This member specifies the `pshared` parameter value.
   */
  int pshared;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the attribute object.
   */
  pthread_condattr_t attr_obj;

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
    CPthreadReqCondattrSetpshared_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondattrSetpshared_Context;

static CPthreadReqCondattrSetpshared_Context
  CPthreadReqCondattrSetpshared_Instance;

static const char *const CPthreadReqCondattrSetpshared_PreDesc_Attr[] =
  { "Valid", "Null", "NA" };

static const char *const CPthreadReqCondattrSetpshared_PreDesc_Pshared[] =
  { "Shared", "Private", "Invalid", "NA" };

static const char *const *const CPthreadReqCondattrSetpshared_PreDesc[] = {
  CPthreadReqCondattrSetpshared_PreDesc_Attr,
  CPthreadReqCondattrSetpshared_PreDesc_Pshared,
  NULL
};

static void CPthreadReqCondattrSetpshared_Pre_Attr_Prepare(
  CPthreadReqCondattrSetpshared_Context *ctx,
  CPthreadReqCondattrSetpshared_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetpshared_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized condition
       * variable attributes object.
       */
      ctx->attr = &ctx->attr_obj;
      T_eq_int( pthread_condattr_init( &ctx->attr_obj ), 0 );
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrSetpshared_Pre_Pshared_Prepare(
  CPthreadReqCondattrSetpshared_Context    *ctx,
  CPthreadReqCondattrSetpshared_Pre_Pshared state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetpshared_Pre_Pshared_Shared: {
      /*
       * While the `pshared` parameter is PTHREAD_PROCESS_SHARED.
       */
      ctx->pshared = PTHREAD_PROCESS_SHARED;
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Pshared_Private: {
      /*
       * While the `pshared` parameter is PTHREAD_PROCESS_PRIVATE.
       */
      ctx->pshared = PTHREAD_PROCESS_PRIVATE;
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Pshared_Invalid: {
      /*
       * While the `pshared` parameter is neither PTHREAD_PROCESS_SHARED nor
       * PTHREAD_PROCESS_PRIVATE.
       */
      ctx->pshared = -1;
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Pshared_NA:
      break;
  }
}

static void CPthreadReqCondattrSetpshared_Post_Status_Check(
  CPthreadReqCondattrSetpshared_Context    *ctx,
  CPthreadReqCondattrSetpshared_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetpshared_Post_Status_Zero: {
      /*
       * The return value of pthread_condattr_setpshared() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_Status_EINVAL: {
      /*
       * The return value of pthread_condattr_setpshared() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrSetpshared_Post_AttrObj_Check(
  CPthreadReqCondattrSetpshared_Context     *ctx,
  CPthreadReqCondattrSetpshared_Post_AttrObj state
)
{
  int pshared;

  switch ( state ) {
    case CPthreadReqCondattrSetpshared_Post_AttrObj_Set: {
      /*
       * The process-shared attribute of the condition variable attributes
       * object referenced by `attr` shall be set to the value specified by
       * `pshared`.
       */
      pshared = -1;
      T_eq_int( pthread_condattr_getpshared( &ctx->attr_obj, &pshared ), 0 );
      T_eq_int( pshared, ctx->pshared );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_AttrObj_Nop: {
      /*
       * The process-shared attribute of the condition variable attributes
       * object referenced by `attr` shall not be modified.
       */
      pshared = -1;
      T_eq_int( pthread_condattr_getpshared( &ctx->attr_obj, &pshared ), 0 );
      T_eq_int( pshared, PTHREAD_PROCESS_PRIVATE );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqCondattrSetpshared_Action(
  CPthreadReqCondattrSetpshared_Context *ctx
)
{
  ctx->retval = pthread_condattr_setpshared( ctx->attr, ctx->pshared );
}

/* clang-format off */

static const CPthreadReqCondattrSetpshared_Entry
CPthreadReqCondattrSetpshared_Entries[] = {
  { 0, 0, 0, CPthreadReqCondattrSetpshared_Post_Status_EINVAL,
    CPthreadReqCondattrSetpshared_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqCondattrSetpshared_Post_Status_Zero,
    CPthreadReqCondattrSetpshared_Post_AttrObj_Set },
  { 0, 0, 0, CPthreadReqCondattrSetpshared_Post_Status_EINVAL,
    CPthreadReqCondattrSetpshared_Post_AttrObj_Nop }
};

static const uint8_t
CPthreadReqCondattrSetpshared_Map[] = {
  1, 1, 2, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondattrSetpshared_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqCondattrSetpshared_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondattrSetpshared_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondattrSetpshared_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondattrSetpshared_Scope,
  .initial_context = &CPthreadReqCondattrSetpshared_Instance
};

static inline CPthreadReqCondattrSetpshared_Entry
CPthreadReqCondattrSetpshared_PopEntry(
  CPthreadReqCondattrSetpshared_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondattrSetpshared_Entries
    [ CPthreadReqCondattrSetpshared_Map[ index ] ];
}

static void CPthreadReqCondattrSetpshared_TestVariant(
  CPthreadReqCondattrSetpshared_Context *ctx
)
{
  CPthreadReqCondattrSetpshared_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrSetpshared_Pre_Pshared_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondattrSetpshared_Action( ctx );
  CPthreadReqCondattrSetpshared_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrSetpshared_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqCondattrSetpshared( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondattrSetpshared,
  &CPthreadReqCondattrSetpshared_Fixture
)
{
  CPthreadReqCondattrSetpshared_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqCondattrSetpshared_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqCondattrSetpshared_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqCondattrSetpshared_Pre_Pshared_Shared;
      ctx->Map.pcs[ 1 ] < CPthreadReqCondattrSetpshared_Pre_Pshared_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCondattrSetpshared_PopEntry( ctx );
      CPthreadReqCondattrSetpshared_TestVariant( ctx );
    }
  }
}

/** @} */
