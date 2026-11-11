/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrSetpshared
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
 * @defgroup CPthreadReqCondattrSetpshared \
 *   spec:/c/pthread/req/condattr-setpshared
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrSetpshared_Pre_Attr_Obj,
  CPthreadReqCondattrSetpshared_Pre_Attr_Null,
  CPthreadReqCondattrSetpshared_Pre_Attr_NA
} CPthreadReqCondattrSetpshared_Pre_Attr;

typedef enum {
  CPthreadReqCondattrSetpshared_Pre_Initialized_Yes,
  CPthreadReqCondattrSetpshared_Pre_Initialized_No,
  CPthreadReqCondattrSetpshared_Pre_Initialized_NA
} CPthreadReqCondattrSetpshared_Pre_Initialized;

typedef enum {
  CPthreadReqCondattrSetpshared_Pre_Pshared_Shared,
  CPthreadReqCondattrSetpshared_Pre_Pshared_Private,
  CPthreadReqCondattrSetpshared_Pre_Pshared_Invalid,
  CPthreadReqCondattrSetpshared_Pre_Pshared_NA
} CPthreadReqCondattrSetpshared_Pre_Pshared;

typedef enum {
  CPthreadReqCondattrSetpshared_Post_Status_Ok,
  CPthreadReqCondattrSetpshared_Post_Status_Inval,
  CPthreadReqCondattrSetpshared_Post_Status_NA
} CPthreadReqCondattrSetpshared_Post_Status;

typedef enum {
  CPthreadReqCondattrSetpshared_Post_Pshared_Shared,
  CPthreadReqCondattrSetpshared_Post_Pshared_Private,
  CPthreadReqCondattrSetpshared_Post_Pshared_Nop,
  CPthreadReqCondattrSetpshared_Post_Pshared_NA
} CPthreadReqCondattrSetpshared_Post_Pshared;

typedef enum {
  CPthreadReqCondattrSetpshared_Post_Other_Nop,
  CPthreadReqCondattrSetpshared_Post_Other_NA
} CPthreadReqCondattrSetpshared_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_Pshared_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Pshared : 2;
  uint16_t Post_Other : 1;
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
   * @brief This member contains the return value of the
   *   pthread_condattr_setpshared() call.
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
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondattrSetpshared_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCondattrSetpshared_PreDesc_Pshared[] =
  { "Shared", "Private", "Invalid", "NA" };

static const char *const *const CPthreadReqCondattrSetpshared_PreDesc[] = {
  CPthreadReqCondattrSetpshared_PreDesc_Attr,
  CPthreadReqCondattrSetpshared_PreDesc_Initialized,
  CPthreadReqCondattrSetpshared_PreDesc_Pshared,
  NULL
};

#define PSHARED_POISON 123

#define PSHARED_INVALID 0x7fffffff

static void CPthreadReqCondattrSetpshared_Pre_Attr_Prepare(
  CPthreadReqCondattrSetpshared_Context *ctx,
  CPthreadReqCondattrSetpshared_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetpshared_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_condattr_t.
       */
      ctx->attr = &ctx->attr_obj;
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

static void CPthreadReqCondattrSetpshared_Pre_Initialized_Prepare(
  CPthreadReqCondattrSetpshared_Context        *ctx,
  CPthreadReqCondattrSetpshared_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetpshared_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * condition variable attributes object.
       */
      T_eno_success( pthread_condattr_init( &ctx->attr_obj ) );
      ctx->attr_obj.process_shared = PSHARED_POISON;
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized condition variable attributes object.
       */
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = 0;
      ctx->attr_obj.process_shared = PSHARED_POISON;
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Initialized_NA:
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
       * While the `pshared` parameter is equal to PTHREAD_PROCESS_SHARED.
       */
      ctx->pshared = PTHREAD_PROCESS_SHARED;
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Pshared_Private: {
      /*
       * While the `pshared` parameter is equal to PTHREAD_PROCESS_PRIVATE.
       */
      ctx->pshared = PTHREAD_PROCESS_PRIVATE;
      break;
    }

    case CPthreadReqCondattrSetpshared_Pre_Pshared_Invalid: {
      /*
       * While the `pshared` parameter is neither PTHREAD_PROCESS_SHARED nor
       * PTHREAD_PROCESS_PRIVATE.
       */
      ctx->pshared = PSHARED_INVALID;
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
    case CPthreadReqCondattrSetpshared_Post_Status_Ok: {
      /*
       * The return value of pthread_condattr_setpshared() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_Status_Inval: {
      /*
       * The return value of pthread_condattr_setpshared() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrSetpshared_Post_Pshared_Check(
  CPthreadReqCondattrSetpshared_Context     *ctx,
  CPthreadReqCondattrSetpshared_Post_Pshared state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetpshared_Post_Pshared_Shared: {
      /*
       * The process-shared attribute of the condition variable attributes
       * object referenced by the `attr` parameter shall be
       * PTHREAD_PROCESS_SHARED.
       */
      T_eq_int( ctx->attr_obj.process_shared, PTHREAD_PROCESS_SHARED );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_Pshared_Private: {
      /*
       * The process-shared attribute of the condition variable attributes
       * object referenced by the `attr` parameter shall be
       * PTHREAD_PROCESS_PRIVATE.
       */
      T_eq_int( ctx->attr_obj.process_shared, PTHREAD_PROCESS_PRIVATE );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_Pshared_Nop: {
      /*
       * The process-shared attribute of the condition variable attributes
       * object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_int( ctx->attr_obj.process_shared, PSHARED_POISON );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_Pshared_NA:
      break;
  }
}

static void CPthreadReqCondattrSetpshared_Post_Other_Check(
  CPthreadReqCondattrSetpshared_Context   *ctx,
  CPthreadReqCondattrSetpshared_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetpshared_Post_Other_Nop: {
      /*
       * The content of the condition variable attributes object referenced by
       * the `attr` parameter, other than its process-shared attribute, shall
       * not be modified.
       */
      pthread_condattr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.process_shared = ctx->attr_obj.process_shared;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqCondattrSetpshared_Post_Other_NA:
      break;
  }
}

static void CPthreadReqCondattrSetpshared_Action(
  CPthreadReqCondattrSetpshared_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_condattr_setpshared( ctx->attr, ctx->pshared );
}

/* clang-format off */

static const CPthreadReqCondattrSetpshared_Entry
CPthreadReqCondattrSetpshared_Entries[] = {
  { 0, 0, 1, 0, CPthreadReqCondattrSetpshared_Post_Status_Inval,
    CPthreadReqCondattrSetpshared_Post_Pshared_NA,
    CPthreadReqCondattrSetpshared_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqCondattrSetpshared_Post_Status_NA,
    CPthreadReqCondattrSetpshared_Post_Pshared_NA,
    CPthreadReqCondattrSetpshared_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqCondattrSetpshared_Post_Status_Ok,
    CPthreadReqCondattrSetpshared_Post_Pshared_Shared,
    CPthreadReqCondattrSetpshared_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqCondattrSetpshared_Post_Status_Ok,
    CPthreadReqCondattrSetpshared_Post_Pshared_Private,
    CPthreadReqCondattrSetpshared_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqCondattrSetpshared_Post_Status_Inval,
    CPthreadReqCondattrSetpshared_Post_Pshared_Nop,
    CPthreadReqCondattrSetpshared_Post_Other_Nop }
};

static const uint8_t
CPthreadReqCondattrSetpshared_Map[] = {
  2, 3, 4, 2, 3, 4, 0, 0, 0, 1, 1, 1
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

static void CPthreadReqCondattrSetpshared_SetPreConditionStates(
  CPthreadReqCondattrSetpshared_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondattrSetpshared_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqCondattrSetpshared_TestVariant(
  CPthreadReqCondattrSetpshared_Context *ctx
)
{
  CPthreadReqCondattrSetpshared_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrSetpshared_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqCondattrSetpshared_Pre_Pshared_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondattrSetpshared_Action( ctx );
  CPthreadReqCondattrSetpshared_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrSetpshared_Post_Pshared_Check(
    ctx,
    ctx->Map.entry.Post_Pshared
  );
  CPthreadReqCondattrSetpshared_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
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
    ctx->Map.pci[ 0 ] = CPthreadReqCondattrSetpshared_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondattrSetpshared_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondattrSetpshared_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCondattrSetpshared_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondattrSetpshared_Pre_Pshared_Shared;
        ctx->Map.pci[ 2 ] < CPthreadReqCondattrSetpshared_Pre_Pshared_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqCondattrSetpshared_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqCondattrSetpshared_SetPreConditionStates( ctx );
        CPthreadReqCondattrSetpshared_TestVariant( ctx );
      }
    }
  }
}

/** @} */
