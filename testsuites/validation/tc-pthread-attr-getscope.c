/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetscope
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

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqAttrGetscope spec:/c/pthread/req/attr-getscope
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetscope_Pre_Attr_Valid,
  CPthreadReqAttrGetscope_Pre_Attr_Null,
  CPthreadReqAttrGetscope_Pre_Attr_Invalid,
  CPthreadReqAttrGetscope_Pre_Attr_NA
} CPthreadReqAttrGetscope_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetscope_Pre_ContentionScope_Valid,
  CPthreadReqAttrGetscope_Pre_ContentionScope_Null,
  CPthreadReqAttrGetscope_Pre_ContentionScope_NA
} CPthreadReqAttrGetscope_Pre_ContentionScope;

typedef enum {
  CPthreadReqAttrGetscope_Post_Status_Zero,
  CPthreadReqAttrGetscope_Post_Status_EINVAL,
  CPthreadReqAttrGetscope_Post_Status_NA
} CPthreadReqAttrGetscope_Post_Status;

typedef enum {
  CPthreadReqAttrGetscope_Post_ScopeObj_Set,
  CPthreadReqAttrGetscope_Post_ScopeObj_Nop,
  CPthreadReqAttrGetscope_Post_ScopeObj_NA
} CPthreadReqAttrGetscope_Post_ScopeObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_ContentionScope_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_ScopeObj : 2;
} CPthreadReqAttrGetscope_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-getscope test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_attr_t *attr;

  /**
   * @brief This member specifies the `contentionscope` parameter value.
   */
  int *contentionscope;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains the contention scope output object.
   */
  int contentionscope_value;

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
    CPthreadReqAttrGetscope_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetscope_Context;

static CPthreadReqAttrGetscope_Context
  CPthreadReqAttrGetscope_Instance;

static const char * const CPthreadReqAttrGetscope_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrGetscope_PreDesc_ContentionScope[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqAttrGetscope_PreDesc[] = {
  CPthreadReqAttrGetscope_PreDesc_Attr,
  CPthreadReqAttrGetscope_PreDesc_ContentionScope,
  NULL
};

/*
 * PTHREAD_SCOPE_PROCESS is both the only contention scope accepted by
 * pthread_attr_setscope() and the default contention scope.  The Set
 * post-condition therefore cannot distinguish a copied value from the
 * default value of an initialized attributes object.
 */
#define CONTENTION_SCOPE_VALUE PTHREAD_SCOPE_PROCESS

static void CPthreadReqAttrGetscope_Pre_Attr_Prepare(
  CPthreadReqAttrGetscope_Context *ctx,
  CPthreadReqAttrGetscope_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      pthread_attr_setscope( &ctx->attr_obj, CONTENTION_SCOPE_VALUE );
      break;
    }

    case CPthreadReqAttrGetscope_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetscope_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetscope_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Pre_ContentionScope_Prepare(
  CPthreadReqAttrGetscope_Context            *ctx,
  CPthreadReqAttrGetscope_Pre_ContentionScope state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Pre_ContentionScope_Valid: {
      /*
       * While the `contentionscope` parameter references an object of type
       * `int`.
       */
      ctx->contentionscope_value = -1;
      ctx->contentionscope = &ctx->contentionscope_value;
      break;
    }

    case CPthreadReqAttrGetscope_Pre_ContentionScope_Null: {
      /*
       * While the `contentionscope` parameter is NULL.
       */
      ctx->contentionscope = NULL;
      break;
    }

    case CPthreadReqAttrGetscope_Pre_ContentionScope_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Post_Status_Check(
  CPthreadReqAttrGetscope_Context    *ctx,
  CPthreadReqAttrGetscope_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_getscope() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrGetscope_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_getscope() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrGetscope_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Post_ScopeObj_Check(
  CPthreadReqAttrGetscope_Context      *ctx,
  CPthreadReqAttrGetscope_Post_ScopeObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Post_ScopeObj_Set: {
      /*
       * The value of the object referenced by the `contentionscope` parameter
       * shall be set to the thread contention scope attribute of the thread
       * attributes object referenced by the `attr` parameter.
       */
      T_eq_int( ctx->contentionscope_value, CONTENTION_SCOPE_VALUE );
      break;
    }

    case CPthreadReqAttrGetscope_Post_ScopeObj_Nop: {
      /*
       * The object referenced by the `contentionscope` parameter shall not be
       * modified.
       */
      T_eq_int( ctx->contentionscope_value, -1 );
      break;
    }

    case CPthreadReqAttrGetscope_Post_ScopeObj_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Action(
  CPthreadReqAttrGetscope_Context *ctx
)
{
  ctx->retval = pthread_attr_getscope( ctx->attr, ctx->contentionscope );
}

static const CPthreadReqAttrGetscope_Entry
CPthreadReqAttrGetscope_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrGetscope_Post_Status_EINVAL,
    CPthreadReqAttrGetscope_Post_ScopeObj_NA },
  { 0, 0, 0, CPthreadReqAttrGetscope_Post_Status_EINVAL,
    CPthreadReqAttrGetscope_Post_ScopeObj_Nop },
  { 0, 0, 0, CPthreadReqAttrGetscope_Post_Status_Zero,
    CPthreadReqAttrGetscope_Post_ScopeObj_Set }
};

static const uint8_t
CPthreadReqAttrGetscope_Map[] = {
  2, 0, 1, 0, 1, 0
};

static size_t CPthreadReqAttrGetscope_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqAttrGetscope_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrGetscope_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrGetscope_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrGetscope_Scope,
  .initial_context = &CPthreadReqAttrGetscope_Instance
};

static inline CPthreadReqAttrGetscope_Entry CPthreadReqAttrGetscope_PopEntry(
  CPthreadReqAttrGetscope_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrGetscope_Entries[
    CPthreadReqAttrGetscope_Map[ index ]
  ];
}

static void CPthreadReqAttrGetscope_TestVariant(
  CPthreadReqAttrGetscope_Context *ctx
)
{
  CPthreadReqAttrGetscope_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetscope_Pre_ContentionScope_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrGetscope_Action( ctx );
  CPthreadReqAttrGetscope_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrGetscope_Post_ScopeObj_Check(
    ctx,
    ctx->Map.entry.Post_ScopeObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrGetscope( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrGetscope,
  &CPthreadReqAttrGetscope_Fixture
)
{
  CPthreadReqAttrGetscope_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrGetscope_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrGetscope_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetscope_Pre_ContentionScope_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrGetscope_Pre_ContentionScope_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrGetscope_PopEntry( ctx );
      CPthreadReqAttrGetscope_TestVariant( ctx );
    }
  }
}

/** @} */
