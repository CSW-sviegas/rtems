/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetstacksize
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
 * @defgroup CPthreadReqAttrGetstacksize spec:/c/pthread/req/attr-getstacksize
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetstacksize_Pre_Attr_Obj,
  CPthreadReqAttrGetstacksize_Pre_Attr_Null,
  CPthreadReqAttrGetstacksize_Pre_Attr_NA
} CPthreadReqAttrGetstacksize_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetstacksize_Pre_Initialized_Yes,
  CPthreadReqAttrGetstacksize_Pre_Initialized_No,
  CPthreadReqAttrGetstacksize_Pre_Initialized_NA
} CPthreadReqAttrGetstacksize_Pre_Initialized;

typedef enum {
  CPthreadReqAttrGetstacksize_Pre_StoredStackSize_Zero,
  CPthreadReqAttrGetstacksize_Pre_StoredStackSize_Positive,
  CPthreadReqAttrGetstacksize_Pre_StoredStackSize_NA
} CPthreadReqAttrGetstacksize_Pre_StoredStackSize;

typedef enum {
  CPthreadReqAttrGetstacksize_Pre_StackSize_Obj,
  CPthreadReqAttrGetstacksize_Pre_StackSize_Null,
  CPthreadReqAttrGetstacksize_Pre_StackSize_NA
} CPthreadReqAttrGetstacksize_Pre_StackSize;

typedef enum {
  CPthreadReqAttrGetstacksize_Post_Status_Ok,
  CPthreadReqAttrGetstacksize_Post_Status_Inval,
  CPthreadReqAttrGetstacksize_Post_Status_NA
} CPthreadReqAttrGetstacksize_Post_Status;

typedef enum {
  CPthreadReqAttrGetstacksize_Post_StackSize_Zero,
  CPthreadReqAttrGetstacksize_Post_StackSize_Positive,
  CPthreadReqAttrGetstacksize_Post_StackSize_Nop,
  CPthreadReqAttrGetstacksize_Post_StackSize_NA
} CPthreadReqAttrGetstacksize_Post_StackSize;

typedef enum {
  CPthreadReqAttrGetstacksize_Post_Attr_Nop,
  CPthreadReqAttrGetstacksize_Post_Attr_NA
} CPthreadReqAttrGetstacksize_Post_Attr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StoredStackSize_NA : 1;
  uint16_t Pre_StackSize_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_StackSize : 2;
  uint16_t Post_Attr : 1;
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
   * @brief This member contains the return value of the
   *   pthread_attr_getstacksize() call.
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

  /**
   * @brief This member contains the object referenced by the `stacksize`
   *   parameter.
   */
  size_t stacksize_obj;

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

static const char *const CPthreadReqAttrGetstacksize_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrGetstacksize_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const
  CPthreadReqAttrGetstacksize_PreDesc_StoredStackSize[] =
    { "Zero", "Positive", "NA" };

static const char *const CPthreadReqAttrGetstacksize_PreDesc_StackSize[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrGetstacksize_PreDesc[] = {
  CPthreadReqAttrGetstacksize_PreDesc_Attr,
  CPthreadReqAttrGetstacksize_PreDesc_Initialized,
  CPthreadReqAttrGetstacksize_PreDesc_StoredStackSize,
  CPthreadReqAttrGetstacksize_PreDesc_StackSize,
  NULL
};

#define STACK_SIZE 8192

static void CPthreadReqAttrGetstacksize_Pre_Attr_Prepare(
  CPthreadReqAttrGetstacksize_Context *ctx,
  CPthreadReqAttrGetstacksize_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Pre_Initialized_Prepare(
  CPthreadReqAttrGetstacksize_Context        *ctx,
  CPthreadReqAttrGetstacksize_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Pre_StoredStackSize_Prepare(
  CPthreadReqAttrGetstacksize_Context            *ctx,
  CPthreadReqAttrGetstacksize_Pre_StoredStackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Pre_StoredStackSize_Zero: {
      /*
       * While the thread stack size attribute of the object referenced by the
       * `attr` parameter is zero.
       */
      ctx->attr_obj.stacksize = 0;
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_StoredStackSize_Positive: {
      /*
       * While the thread stack size attribute of the object referenced by the
       * `attr` parameter is greater than zero.
       */
      ctx->attr_obj.stacksize = STACK_SIZE;
      break;
    }

    case CPthreadReqAttrGetstacksize_Pre_StoredStackSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Pre_StackSize_Prepare(
  CPthreadReqAttrGetstacksize_Context      *ctx,
  CPthreadReqAttrGetstacksize_Pre_StackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Pre_StackSize_Obj: {
      /*
       * While the `stacksize` parameter references an object.
       */
      ctx->stacksize_obj = SIZE_MAX;
      ctx->stacksize = &ctx->stacksize_obj;
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
    case CPthreadReqAttrGetstacksize_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_getstacksize() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_getstacksize() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Post_StackSize_Check(
  CPthreadReqAttrGetstacksize_Context       *ctx,
  CPthreadReqAttrGetstacksize_Post_StackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Post_StackSize_Zero: {
      /*
       * The object referenced by the `stacksize` parameter shall be zero.
       */
      T_eq_sz( ctx->stacksize_obj, 0 );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_StackSize_Positive: {
      /*
       * The object referenced by the `stacksize` parameter shall be the thread
       * stack size attribute of the object referenced by the `attr` parameter.
       */
      T_eq_sz( ctx->stacksize_obj, STACK_SIZE );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_StackSize_Nop: {
      /*
       * The object referenced by the `stacksize` parameter shall not be
       * modified.
       */
      T_eq_sz( ctx->stacksize_obj, SIZE_MAX );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_StackSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Post_Attr_Check(
  CPthreadReqAttrGetstacksize_Context  *ctx,
  CPthreadReqAttrGetstacksize_Post_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstacksize_Post_Attr_Nop: {
      /*
       * The object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_mem( &ctx->attr_obj, &ctx->attr_before, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstacksize_Post_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstacksize_Action(
  CPthreadReqAttrGetstacksize_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_getstacksize( ctx->attr, ctx->stacksize );
}

/* clang-format off */

static const CPthreadReqAttrGetstacksize_Entry
CPthreadReqAttrGetstacksize_Entries[] = {
  { 1, 0, 0, 0, 0, CPthreadReqAttrGetstacksize_Post_Status_NA,
    CPthreadReqAttrGetstacksize_Post_StackSize_NA,
    CPthreadReqAttrGetstacksize_Post_Attr_NA },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetstacksize_Post_Status_Inval,
    CPthreadReqAttrGetstacksize_Post_StackSize_NA,
    CPthreadReqAttrGetstacksize_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetstacksize_Post_Status_Inval,
    CPthreadReqAttrGetstacksize_Post_StackSize_Nop,
    CPthreadReqAttrGetstacksize_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetstacksize_Post_Status_Ok,
    CPthreadReqAttrGetstacksize_Post_StackSize_Zero,
    CPthreadReqAttrGetstacksize_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetstacksize_Post_Status_Ok,
    CPthreadReqAttrGetstacksize_Post_StackSize_Positive,
    CPthreadReqAttrGetstacksize_Post_Attr_Nop },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetstacksize_Post_Status_Inval,
    CPthreadReqAttrGetstacksize_Post_StackSize_Nop,
    CPthreadReqAttrGetstacksize_Post_Attr_NA },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetstacksize_Post_Status_Inval,
    CPthreadReqAttrGetstacksize_Post_StackSize_NA,
    CPthreadReqAttrGetstacksize_Post_Attr_NA }
};

static const uint8_t
CPthreadReqAttrGetstacksize_Map[] = {
  3, 1, 4, 1, 2, 1, 2, 1, 5, 6, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

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
  return CPthreadReqAttrGetstacksize_Entries
    [ CPthreadReqAttrGetstacksize_Map[ index ] ];
}

static void CPthreadReqAttrGetstacksize_SetPreConditionStates(
  CPthreadReqAttrGetstacksize_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetstacksize_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredStackSize_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqAttrGetstacksize_Pre_StoredStackSize_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void CPthreadReqAttrGetstacksize_TestVariant(
  CPthreadReqAttrGetstacksize_Context *ctx
)
{
  CPthreadReqAttrGetstacksize_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetstacksize_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrGetstacksize_Pre_StoredStackSize_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrGetstacksize_Pre_StackSize_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqAttrGetstacksize_Action( ctx );
  CPthreadReqAttrGetstacksize_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrGetstacksize_Post_StackSize_Check(
    ctx,
    ctx->Map.entry.Post_StackSize
  );
  CPthreadReqAttrGetstacksize_Post_Attr_Check( ctx, ctx->Map.entry.Post_Attr );
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
    ctx->Map.pci[ 0 ] = CPthreadReqAttrGetstacksize_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrGetstacksize_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrGetstacksize_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrGetstacksize_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqAttrGetstacksize_Pre_StoredStackSize_Zero;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrGetstacksize_Pre_StoredStackSize_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqAttrGetstacksize_Pre_StackSize_Obj;
          ctx->Map.pci[ 3 ] < CPthreadReqAttrGetstacksize_Pre_StackSize_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqAttrGetstacksize_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CPthreadReqAttrGetstacksize_SetPreConditionStates( ctx );
          CPthreadReqAttrGetstacksize_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
