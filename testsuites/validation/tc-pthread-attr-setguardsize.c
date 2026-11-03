/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetguardsize
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
#include <stdint.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqAttrSetguardsize spec:/c/pthread/req/attr-setguardsize
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetguardsize_Pre_Attr_Obj,
  CPthreadReqAttrSetguardsize_Pre_Attr_Null,
  CPthreadReqAttrSetguardsize_Pre_Attr_NA
} CPthreadReqAttrSetguardsize_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetguardsize_Pre_Initialized_Yes,
  CPthreadReqAttrSetguardsize_Pre_Initialized_No,
  CPthreadReqAttrSetguardsize_Pre_Initialized_NA
} CPthreadReqAttrSetguardsize_Pre_Initialized;

typedef enum {
  CPthreadReqAttrSetguardsize_Pre_GuardSize_Zero,
  CPthreadReqAttrSetguardsize_Pre_GuardSize_Positive,
  CPthreadReqAttrSetguardsize_Pre_GuardSize_NA
} CPthreadReqAttrSetguardsize_Pre_GuardSize;

typedef enum {
  CPthreadReqAttrSetguardsize_Post_Status_Ok,
  CPthreadReqAttrSetguardsize_Post_Status_Inval,
  CPthreadReqAttrSetguardsize_Post_Status_NA
} CPthreadReqAttrSetguardsize_Post_Status;

typedef enum {
  CPthreadReqAttrSetguardsize_Post_GuardSize_Zero,
  CPthreadReqAttrSetguardsize_Post_GuardSize_Positive,
  CPthreadReqAttrSetguardsize_Post_GuardSize_Nop,
  CPthreadReqAttrSetguardsize_Post_GuardSize_NA
} CPthreadReqAttrSetguardsize_Post_GuardSize;

typedef enum {
  CPthreadReqAttrSetguardsize_Post_Other_Nop,
  CPthreadReqAttrSetguardsize_Post_Other_NA
} CPthreadReqAttrSetguardsize_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_GuardSize_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_GuardSize : 2;
  uint16_t Post_Other : 1;
} CPthreadReqAttrSetguardsize_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setguardsize test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `guardsize` parameter value.
   */
  size_t guardsize;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_setguardsize() call.
   */
  int status;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains a copy of the thread attributes object before
   *   the call.
   */
  pthread_attr_t attr_before;

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
    CPthreadReqAttrSetguardsize_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetguardsize_Context;

static CPthreadReqAttrSetguardsize_Context
  CPthreadReqAttrSetguardsize_Instance;

static const char *const CPthreadReqAttrSetguardsize_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrSetguardsize_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqAttrSetguardsize_PreDesc_GuardSize[] =
  { "Zero", "Positive", "NA" };

static const char *const *const CPthreadReqAttrSetguardsize_PreDesc[] = {
  CPthreadReqAttrSetguardsize_PreDesc_Attr,
  CPthreadReqAttrSetguardsize_PreDesc_Initialized,
  CPthreadReqAttrSetguardsize_PreDesc_GuardSize,
  NULL
};

#define GUARD_SIZE 4096

static void CPthreadReqAttrSetguardsize_Pre_Attr_Prepare(
  CPthreadReqAttrSetguardsize_Context *ctx,
  CPthreadReqAttrSetguardsize_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Pre_Initialized_Prepare(
  CPthreadReqAttrSetguardsize_Context        *ctx,
  CPthreadReqAttrSetguardsize_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      ctx->attr_obj.guardsize = SIZE_MAX;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.guardsize = SIZE_MAX;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Pre_GuardSize_Prepare(
  CPthreadReqAttrSetguardsize_Context      *ctx,
  CPthreadReqAttrSetguardsize_Pre_GuardSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Pre_GuardSize_Zero: {
      /*
       * While the `guardsize` parameter is zero.
       */
      ctx->guardsize = 0;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_GuardSize_Positive: {
      /*
       * While the `guardsize` parameter is greater than zero.
       */
      ctx->guardsize = GUARD_SIZE;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_GuardSize_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Post_Status_Check(
  CPthreadReqAttrSetguardsize_Context    *ctx,
  CPthreadReqAttrSetguardsize_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_setguardsize() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_setguardsize() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Post_GuardSize_Check(
  CPthreadReqAttrSetguardsize_Context       *ctx,
  CPthreadReqAttrSetguardsize_Post_GuardSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Post_GuardSize_Zero: {
      /*
       * The thread guard size attribute of the thread attributes object
       * referenced by the `attr` parameter shall be zero.
       */
      T_eq_sz( ctx->attr_obj.guardsize, 0 );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_GuardSize_Positive: {
      /*
       * The thread guard size attribute of the thread attributes object
       * referenced by the `attr` parameter shall be the value of the
       * `guardsize` parameter.
       */
      T_eq_sz( ctx->attr_obj.guardsize, GUARD_SIZE );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_GuardSize_Nop: {
      /*
       * The thread guard size attribute of the thread attributes object
       * referenced by the `attr` parameter shall not be modified.
       */
      T_eq_sz( ctx->attr_obj.guardsize, ctx->attr_before.guardsize );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_GuardSize_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Post_Other_Check(
  CPthreadReqAttrSetguardsize_Context   *ctx,
  CPthreadReqAttrSetguardsize_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Post_Other_Nop: {
      /*
       * The content of the thread attributes object referenced by the `attr`
       * parameter, other than its thread guard size attribute, shall not be
       * modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.guardsize = ctx->attr_obj.guardsize;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Action(
  CPthreadReqAttrSetguardsize_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_setguardsize( ctx->attr, ctx->guardsize );
}

/* clang-format off */

static const CPthreadReqAttrSetguardsize_Entry
CPthreadReqAttrSetguardsize_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrSetguardsize_Post_Status_Inval,
    CPthreadReqAttrSetguardsize_Post_GuardSize_Nop,
    CPthreadReqAttrSetguardsize_Post_Other_Nop },
  { 0, 0, 1, 0, CPthreadReqAttrSetguardsize_Post_Status_Inval,
    CPthreadReqAttrSetguardsize_Post_GuardSize_NA,
    CPthreadReqAttrSetguardsize_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqAttrSetguardsize_Post_Status_NA,
    CPthreadReqAttrSetguardsize_Post_GuardSize_NA,
    CPthreadReqAttrSetguardsize_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqAttrSetguardsize_Post_Status_Ok,
    CPthreadReqAttrSetguardsize_Post_GuardSize_Zero,
    CPthreadReqAttrSetguardsize_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetguardsize_Post_Status_Ok,
    CPthreadReqAttrSetguardsize_Post_GuardSize_Positive,
    CPthreadReqAttrSetguardsize_Post_Other_Nop }
};

static const uint8_t
CPthreadReqAttrSetguardsize_Map[] = {
  3, 4, 0, 0, 1, 1, 2, 2
};

/* clang-format on */

static size_t CPthreadReqAttrSetguardsize_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetguardsize_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetguardsize_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetguardsize_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetguardsize_Scope,
  .initial_context = &CPthreadReqAttrSetguardsize_Instance
};

static inline CPthreadReqAttrSetguardsize_Entry
CPthreadReqAttrSetguardsize_PopEntry(
  CPthreadReqAttrSetguardsize_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetguardsize_Entries
    [ CPthreadReqAttrSetguardsize_Map[ index ] ];
}

static void CPthreadReqAttrSetguardsize_SetPreConditionStates(
  CPthreadReqAttrSetguardsize_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetguardsize_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqAttrSetguardsize_TestVariant(
  CPthreadReqAttrSetguardsize_Context *ctx
)
{
  CPthreadReqAttrSetguardsize_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetguardsize_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetguardsize_Pre_GuardSize_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqAttrSetguardsize_Action( ctx );
  CPthreadReqAttrSetguardsize_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetguardsize_Post_GuardSize_Check(
    ctx,
    ctx->Map.entry.Post_GuardSize
  );
  CPthreadReqAttrSetguardsize_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetguardsize( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetguardsize,
  &CPthreadReqAttrSetguardsize_Fixture
)
{
  CPthreadReqAttrSetguardsize_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrSetguardsize_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrSetguardsize_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrSetguardsize_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrSetguardsize_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqAttrSetguardsize_Pre_GuardSize_Zero;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrSetguardsize_Pre_GuardSize_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrSetguardsize_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqAttrSetguardsize_SetPreConditionStates( ctx );
        CPthreadReqAttrSetguardsize_TestVariant( ctx );
      }
    }
  }
}

/** @} */
