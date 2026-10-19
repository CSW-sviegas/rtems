/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetstacksize
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
 * @defgroup CPthreadReqAttrGetstacksize spec:/c/pthread/req/attr-getstacksize
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetstacksize_Pre_Attr_Valid,
  CPthreadReqAttrGetstacksize_Pre_Attr_Null,
  CPthreadReqAttrGetstacksize_Pre_Attr_Invalid,
  CPthreadReqAttrGetstacksize_Pre_Attr_NA
} CPthreadReqAttrGetstacksize_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetstacksize_Pre_StackSize_Valid,
  CPthreadReqAttrGetstacksize_Pre_StackSize_Null,
  CPthreadReqAttrGetstacksize_Pre_StackSize_NA
} CPthreadReqAttrGetstacksize_Pre_StackSize;

typedef enum {
  CPthreadReqAttrGetstacksize_Post_Status_Zero,
  CPthreadReqAttrGetstacksize_Post_Status_EINVAL,
  CPthreadReqAttrGetstacksize_Post_Status_NA
} CPthreadReqAttrGetstacksize_Post_Status;

typedef enum {
  CPthreadReqAttrGetstacksize_Post_StackSizeObj_Set,
  CPthreadReqAttrGetstacksize_Post_StackSizeObj_Nop,
  CPthreadReqAttrGetstacksize_Post_StackSizeObj_NA
} CPthreadReqAttrGetstacksize_Post_StackSizeObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_StackSize_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_StackSizeObj : 2;
} CPthreadReqAttrGetstacksize_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-getstacksize test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_attr_t *attr;

  /**
   * @brief This member specifies the `stacksize` parameter value.
   */
  size_t *stacksize;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains the stack size output object.
   */
  size_t stacksize_value;

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
    CPthreadReqAttrGetstacksize_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetstacksize_Context;

static CPthreadReqAttrGetstacksize_Context
  CPthreadReqAttrGetstacksize_Instance;

static const char * const CPthreadReqAttrGetstacksize_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrGetstacksize_PreDesc_StackSize[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqAttrGetstacksize_PreDesc[] = {
  CPthreadReqAttrGetstacksize_PreDesc_Attr,
  CPthreadReqAttrGetstacksize_PreDesc_StackSize,
  NULL
};

#define STACK_SIZE_VALUE ( (size_t) 65536 )

#define STALE_STACK_SIZE ( (size_t) -1 )

static void CPthreadReqAttrGetstacksize_Pre_Attr_Prepare(
  CPthreadReqAttrGetstacksize_Context *ctx,
  CPthreadReqAttrGetstacksize_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      pthread_attr_setstacksize( &ctx->attr_obj, STACK_SIZE_VALUE );
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Pre_StackSize_Prepare(
  CPthreadReqAttrGetstacksize_Context      *ctx,
  CPthreadReqAttrGetstacksize_Pre_StackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Pre_StackSize_Valid: {
      /*
       * While the `stacksize` parameter references an object of type size_t.
       */
      ctx->stacksize_value = STALE_STACK_SIZE;
      ctx->stacksize = &ctx->stacksize_value;
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_StackSize_Null: {
      /*
       * While the `stacksize` parameter is NULL.
       */
      ctx->stacksize = NULL;
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_StackSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Post_Status_Check(
  CPthreadReqAttrGetstacksize_Context    *ctx,
  CPthreadReqAttrGetstacksize_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_getstacksize() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_getstacksize() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Post_StackSizeObj_Check(
  CPthreadReqAttrGetstacksize_Context          *ctx,
  CPthreadReqAttrGetstacksize_Post_StackSizeObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Post_StackSizeObj_Set: {
      /*
       * The value of the object referenced by the `stacksize` parameter shall
       * be set to the thread stack size attribute of the thread attributes
       * object referenced by the `attr` parameter.
       */
      T_eq_sz( ctx->stacksize_value, STACK_SIZE_VALUE );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_StackSizeObj_Nop: {
      /*
       * The object referenced by the `stacksize` parameter shall not be
       * modified.
       */
      T_eq_sz( ctx->stacksize_value, STALE_STACK_SIZE );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_StackSizeObj_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Action(
  CPthreadReqAttrGetstacksize_Context *ctx
)
{
  ctx->retval = pthread_attr_getstacksize( ctx->attr, ctx->stacksize );
}

static const CPthreadReqAttrGetstacksize_Entry
CPthreadReqAttrGetstacksize_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrGetstacksize_Post_Status_EINVAL,
    CPthreadReqAttrGetstacksize_Post_StackSizeObj_NA },
  { 0, 0, 0, CPthreadReqAttrGetstacksize_Post_Status_EINVAL,
    CPthreadReqAttrGetstacksize_Post_StackSizeObj_Nop },
  { 0, 0, 0, CPthreadReqAttrGetstacksize_Post_Status_Zero,
    CPthreadReqAttrGetstacksize_Post_StackSizeObj_Set }
};

static const uint8_t
CPthreadReqAttrGetstacksize_Map[] = {
  2, 0, 1, 0, 1, 0
};

static size_t CPthreadReqAttrGetstacksize_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrGetstacksize_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrGetstacksize_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrGetstacksize_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrGetstacksize_Scope,
  .initial_context = &CPthreadReqAttrGetstacksize_Instance
};

static inline CPthreadReqAttrGetstacksize_Entry
CPthreadReqAttrGetstacksize_PopEntry(
  CPthreadReqAttrGetstacksize_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrGetstacksize_Entries[
    CPthreadReqAttrGetstacksize_Map[ index ]
  ];
}

static void CPthreadReqAttrGetstacksize_TestVariant(
  CPthreadReqAttrGetstacksize_Context *ctx
)
{
  CPthreadReqAttrGetstacksize_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetstacksize_Pre_StackSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrGetstacksize_Action( ctx );
  CPthreadReqAttrGetstacksize_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrGetstacksize_Post_StackSizeObj_Check(
    ctx,
    ctx->Map.entry.Post_StackSizeObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrGetstacksize( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrGetstacksize,
  &CPthreadReqAttrGetstacksize_Fixture
)
{
  CPthreadReqAttrGetstacksize_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrGetstacksize_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrGetstacksize_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetstacksize_Pre_StackSize_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrGetstacksize_Pre_StackSize_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrGetstacksize_PopEntry( ctx );
      CPthreadReqAttrGetstacksize_TestVariant( ctx );
    }
  }
}

/** @} */
