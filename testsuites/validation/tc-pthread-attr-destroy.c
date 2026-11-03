/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrDestroy
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
 * @defgroup CPthreadReqAttrDestroy spec:/c/pthread/req/attr-destroy
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrDestroy_Pre_Attr_Obj,
  CPthreadReqAttrDestroy_Pre_Attr_Null,
  CPthreadReqAttrDestroy_Pre_Attr_NA
} CPthreadReqAttrDestroy_Pre_Attr;

typedef enum {
  CPthreadReqAttrDestroy_Pre_Initialized_Yes,
  CPthreadReqAttrDestroy_Pre_Initialized_No,
  CPthreadReqAttrDestroy_Pre_Initialized_NA
} CPthreadReqAttrDestroy_Pre_Initialized;

typedef enum {
  CPthreadReqAttrDestroy_Post_Status_Ok,
  CPthreadReqAttrDestroy_Post_Status_Inval,
  CPthreadReqAttrDestroy_Post_Status_NA
} CPthreadReqAttrDestroy_Post_Status;

typedef enum {
  CPthreadReqAttrDestroy_Post_Initialized_No,
  CPthreadReqAttrDestroy_Post_Initialized_Nop,
  CPthreadReqAttrDestroy_Post_Initialized_NA
} CPthreadReqAttrDestroy_Post_Initialized;

typedef enum {
  CPthreadReqAttrDestroy_Post_Other_Nop,
  CPthreadReqAttrDestroy_Post_Other_NA
} CPthreadReqAttrDestroy_Post_Other;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_Initialized_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Initialized : 2;
  uint8_t Post_Other : 1;
} CPthreadReqAttrDestroy_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-destroy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member contains the return value of the pthread_attr_destroy()
   *   call.
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
    CPthreadReqAttrDestroy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrDestroy_Context;

static CPthreadReqAttrDestroy_Context CPthreadReqAttrDestroy_Instance;

static const char *const CPthreadReqAttrDestroy_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrDestroy_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqAttrDestroy_PreDesc[] = {
  CPthreadReqAttrDestroy_PreDesc_Attr,
  CPthreadReqAttrDestroy_PreDesc_Initialized,
  NULL
};

static void CPthreadReqAttrDestroy_Pre_Attr_Prepare(
  CPthreadReqAttrDestroy_Context *ctx,
  CPthreadReqAttrDestroy_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrDestroy_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrDestroy_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrDestroy_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrDestroy_Pre_Initialized_Prepare(
  CPthreadReqAttrDestroy_Context        *ctx,
  CPthreadReqAttrDestroy_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrDestroy_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrDestroy_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrDestroy_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrDestroy_Post_Status_Check(
  CPthreadReqAttrDestroy_Context    *ctx,
  CPthreadReqAttrDestroy_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrDestroy_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_destroy() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrDestroy_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_destroy() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrDestroy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrDestroy_Post_Initialized_Check(
  CPthreadReqAttrDestroy_Context         *ctx,
  CPthreadReqAttrDestroy_Post_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrDestroy_Post_Initialized_No: {
      /*
       * The object referenced by the `attr` parameter shall not be an
       * initialized thread attributes object.
       */
      T_false( ctx->attr_obj.is_initialized );
      break;
    }

    case CPthreadReqAttrDestroy_Post_Initialized_Nop: {
      /*
       * Whether the object referenced by the `attr` parameter is an
       * initialized thread attributes object shall not be modified.
       */
      T_eq_int(
        ctx->attr_obj.is_initialized,
        ctx->attr_before.is_initialized
      );
      break;
    }

    case CPthreadReqAttrDestroy_Post_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrDestroy_Post_Other_Check(
  CPthreadReqAttrDestroy_Context   *ctx,
  CPthreadReqAttrDestroy_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrDestroy_Post_Other_Nop: {
      /*
       * The content of the object referenced by the `attr` parameter, other
       * than whether it is an initialized thread attributes object, shall not
       * be modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.is_initialized = ctx->attr_obj.is_initialized;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrDestroy_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrDestroy_Action(
  CPthreadReqAttrDestroy_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_destroy( ctx->attr );
}

/* clang-format off */

static const CPthreadReqAttrDestroy_Entry
CPthreadReqAttrDestroy_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrDestroy_Post_Status_Ok,
    CPthreadReqAttrDestroy_Post_Initialized_No,
    CPthreadReqAttrDestroy_Post_Other_Nop },
  { 0, 0, 0, CPthreadReqAttrDestroy_Post_Status_Inval,
    CPthreadReqAttrDestroy_Post_Initialized_Nop,
    CPthreadReqAttrDestroy_Post_Other_Nop },
  { 0, 0, 1, CPthreadReqAttrDestroy_Post_Status_Inval,
    CPthreadReqAttrDestroy_Post_Initialized_NA,
    CPthreadReqAttrDestroy_Post_Other_NA },
  { 1, 0, 0, CPthreadReqAttrDestroy_Post_Status_NA,
    CPthreadReqAttrDestroy_Post_Initialized_NA,
    CPthreadReqAttrDestroy_Post_Other_NA }
};

static const uint8_t
CPthreadReqAttrDestroy_Map[] = {
  0, 1, 2, 3
};

/* clang-format on */

static size_t CPthreadReqAttrDestroy_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqAttrDestroy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqAttrDestroy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqAttrDestroy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrDestroy_Scope,
  .initial_context = &CPthreadReqAttrDestroy_Instance
};

static inline CPthreadReqAttrDestroy_Entry CPthreadReqAttrDestroy_PopEntry(
  CPthreadReqAttrDestroy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrDestroy_Entries[ CPthreadReqAttrDestroy_Map[ index ] ];
}

static void CPthreadReqAttrDestroy_SetPreConditionStates(
  CPthreadReqAttrDestroy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrDestroy_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CPthreadReqAttrDestroy_TestVariant(
  CPthreadReqAttrDestroy_Context *ctx
)
{
  CPthreadReqAttrDestroy_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrDestroy_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrDestroy_Action( ctx );
  CPthreadReqAttrDestroy_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrDestroy_Post_Initialized_Check(
    ctx,
    ctx->Map.entry.Post_Initialized
  );
  CPthreadReqAttrDestroy_Post_Other_Check( ctx, ctx->Map.entry.Post_Other );
}

/**
 * @fn void T_case_body_CPthreadReqAttrDestroy( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqAttrDestroy, &CPthreadReqAttrDestroy_Fixture )
{
  CPthreadReqAttrDestroy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrDestroy_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrDestroy_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrDestroy_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrDestroy_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrDestroy_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      CPthreadReqAttrDestroy_SetPreConditionStates( ctx );
      CPthreadReqAttrDestroy_TestVariant( ctx );
    }
  }
}

/** @} */
