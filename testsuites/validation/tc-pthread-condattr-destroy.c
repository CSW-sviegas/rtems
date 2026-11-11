/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrDestroy
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
  CPthreadReqCondattrDestroy_Pre_Attr_Obj,
  CPthreadReqCondattrDestroy_Pre_Attr_Null,
  CPthreadReqCondattrDestroy_Pre_Attr_NA
} CPthreadReqCondattrDestroy_Pre_Attr;

typedef enum {
  CPthreadReqCondattrDestroy_Pre_Initialized_Yes,
  CPthreadReqCondattrDestroy_Pre_Initialized_No,
  CPthreadReqCondattrDestroy_Pre_Initialized_NA
} CPthreadReqCondattrDestroy_Pre_Initialized;

typedef enum {
  CPthreadReqCondattrDestroy_Post_Status_Ok,
  CPthreadReqCondattrDestroy_Post_Status_Inval,
  CPthreadReqCondattrDestroy_Post_Status_NA
} CPthreadReqCondattrDestroy_Post_Status;

typedef enum {
  CPthreadReqCondattrDestroy_Post_Initialized_No,
  CPthreadReqCondattrDestroy_Post_Initialized_Nop,
  CPthreadReqCondattrDestroy_Post_Initialized_NA
} CPthreadReqCondattrDestroy_Post_Initialized;

typedef enum {
  CPthreadReqCondattrDestroy_Post_Other_Nop,
  CPthreadReqCondattrDestroy_Post_Other_NA
} CPthreadReqCondattrDestroy_Post_Other;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_Initialized_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Initialized : 2;
  uint8_t Post_Other : 1;
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
   * @brief This member contains the return value of the
   *   pthread_condattr_destroy() call.
   */
  int status;

  /**
   * @brief This member contains the condition variable attributes object.
   */
  pthread_condattr_t attr_obj;

  /**
   * @brief This member contains a copy of the condition variable attributes
   *   object before the call.
   */
  pthread_condattr_t attr_before;

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
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondattrDestroy_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqCondattrDestroy_PreDesc[] = {
  CPthreadReqCondattrDestroy_PreDesc_Attr,
  CPthreadReqCondattrDestroy_PreDesc_Initialized,
  NULL
};

static void CPthreadReqCondattrDestroy_Pre_Attr_Prepare(
  CPthreadReqCondattrDestroy_Context *ctx,
  CPthreadReqCondattrDestroy_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrDestroy_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_condattr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqCondattrDestroy_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondattrDestroy_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrDestroy_Pre_Initialized_Prepare(
  CPthreadReqCondattrDestroy_Context        *ctx,
  CPthreadReqCondattrDestroy_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqCondattrDestroy_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * condition variable attributes object.
       */
      T_eno_success( pthread_condattr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqCondattrDestroy_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized condition variable attributes object.
       */
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = 0;
      break;
    }

    case CPthreadReqCondattrDestroy_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqCondattrDestroy_Post_Status_Check(
  CPthreadReqCondattrDestroy_Context    *ctx,
  CPthreadReqCondattrDestroy_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrDestroy_Post_Status_Ok: {
      /*
       * The return value of pthread_condattr_destroy() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_Status_Inval: {
      /*
       * The return value of pthread_condattr_destroy() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrDestroy_Post_Initialized_Check(
  CPthreadReqCondattrDestroy_Context         *ctx,
  CPthreadReqCondattrDestroy_Post_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqCondattrDestroy_Post_Initialized_No: {
      /*
       * The object referenced by the `attr` parameter shall not be an
       * initialized condition variable attributes object.
       */
      T_false( ctx->attr_obj.is_initialized );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_Initialized_Nop: {
      /*
       * Whether the object referenced by the `attr` parameter is an
       * initialized condition variable attributes object shall not be
       * modified.
       */
      T_eq_int(
        ctx->attr_obj.is_initialized,
        ctx->attr_before.is_initialized
      );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_Initialized_NA:
      break;
  }
}

static void CPthreadReqCondattrDestroy_Post_Other_Check(
  CPthreadReqCondattrDestroy_Context   *ctx,
  CPthreadReqCondattrDestroy_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqCondattrDestroy_Post_Other_Nop: {
      /*
       * The content of the object referenced by the `attr` parameter, other
       * than whether it is an initialized condition variable attributes
       * object, shall not be modified.
       */
      pthread_condattr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.is_initialized = ctx->attr_obj.is_initialized;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqCondattrDestroy_Post_Other_NA:
      break;
  }
}

static void CPthreadReqCondattrDestroy_Action(
  CPthreadReqCondattrDestroy_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_condattr_destroy( ctx->attr );
}

/* clang-format off */

static const CPthreadReqCondattrDestroy_Entry
CPthreadReqCondattrDestroy_Entries[] = {
  { 0, 0, 0, CPthreadReqCondattrDestroy_Post_Status_Ok,
    CPthreadReqCondattrDestroy_Post_Initialized_No,
    CPthreadReqCondattrDestroy_Post_Other_Nop },
  { 0, 0, 0, CPthreadReqCondattrDestroy_Post_Status_Inval,
    CPthreadReqCondattrDestroy_Post_Initialized_Nop,
    CPthreadReqCondattrDestroy_Post_Other_Nop },
  { 0, 0, 1, CPthreadReqCondattrDestroy_Post_Status_Inval,
    CPthreadReqCondattrDestroy_Post_Initialized_NA,
    CPthreadReqCondattrDestroy_Post_Other_NA },
  { 1, 0, 0, CPthreadReqCondattrDestroy_Post_Status_NA,
    CPthreadReqCondattrDestroy_Post_Initialized_NA,
    CPthreadReqCondattrDestroy_Post_Other_NA }
};

static const uint8_t
CPthreadReqCondattrDestroy_Map[] = {
  0, 1, 2, 3
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

static void CPthreadReqCondattrDestroy_SetPreConditionStates(
  CPthreadReqCondattrDestroy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondattrDestroy_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CPthreadReqCondattrDestroy_TestVariant(
  CPthreadReqCondattrDestroy_Context *ctx
)
{
  CPthreadReqCondattrDestroy_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrDestroy_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondattrDestroy_Action( ctx );
  CPthreadReqCondattrDestroy_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrDestroy_Post_Initialized_Check(
    ctx,
    ctx->Map.entry.Post_Initialized
  );
  CPthreadReqCondattrDestroy_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
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
    ctx->Map.pci[ 0 ] = CPthreadReqCondattrDestroy_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondattrDestroy_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondattrDestroy_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCondattrDestroy_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCondattrDestroy_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      CPthreadReqCondattrDestroy_SetPreConditionStates( ctx );
      CPthreadReqCondattrDestroy_TestVariant( ctx );
    }
  }
}

/** @} */
