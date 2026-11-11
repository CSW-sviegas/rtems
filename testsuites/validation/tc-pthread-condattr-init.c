/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrInit
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
#include <time.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondattrInit spec:/c/pthread/req/condattr-init
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrInit_Pre_Attr_Obj,
  CPthreadReqCondattrInit_Pre_Attr_Null,
  CPthreadReqCondattrInit_Pre_Attr_NA
} CPthreadReqCondattrInit_Pre_Attr;

typedef enum {
  CPthreadReqCondattrInit_Post_Status_Ok,
  CPthreadReqCondattrInit_Post_Status_Inval,
  CPthreadReqCondattrInit_Post_Status_NA
} CPthreadReqCondattrInit_Post_Status;

typedef enum {
  CPthreadReqCondattrInit_Post_Initialized_Yes,
  CPthreadReqCondattrInit_Post_Initialized_NA
} CPthreadReqCondattrInit_Post_Initialized;

typedef enum {
  CPthreadReqCondattrInit_Post_Clock_Realtime,
  CPthreadReqCondattrInit_Post_Clock_NA
} CPthreadReqCondattrInit_Post_Clock;

typedef enum {
  CPthreadReqCondattrInit_Post_ProcessShared_Private,
  CPthreadReqCondattrInit_Post_ProcessShared_NA
} CPthreadReqCondattrInit_Post_ProcessShared;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Initialized : 1;
  uint8_t Post_Clock : 1;
  uint8_t Post_ProcessShared : 1;
} CPthreadReqCondattrInit_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/condattr-init test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_condattr_t *attr;

  /**
   * @brief This member contains the return value of the
   *   pthread_condattr_init() call.
   */
  int status;

  /**
   * @brief This member contains the condition variable attributes object.
   */
  pthread_condattr_t attr_obj;

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
    CPthreadReqCondattrInit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondattrInit_Context;

static CPthreadReqCondattrInit_Context CPthreadReqCondattrInit_Instance;

static const char *const CPthreadReqCondattrInit_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqCondattrInit_PreDesc[] = {
  CPthreadReqCondattrInit_PreDesc_Attr,
  NULL
};

static void CPthreadReqCondattrInit_Pre_Attr_Prepare(
  CPthreadReqCondattrInit_Context *ctx,
  CPthreadReqCondattrInit_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrInit_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_condattr_t.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = 0;
      break;
    }

    case CPthreadReqCondattrInit_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondattrInit_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrInit_Post_Status_Check(
  CPthreadReqCondattrInit_Context    *ctx,
  CPthreadReqCondattrInit_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrInit_Post_Status_Ok: {
      /*
       * The return value of pthread_condattr_init() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondattrInit_Post_Status_Inval: {
      /*
       * The return value of pthread_condattr_init() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondattrInit_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrInit_Post_Initialized_Check(
  CPthreadReqCondattrInit_Context         *ctx,
  CPthreadReqCondattrInit_Post_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqCondattrInit_Post_Initialized_Yes: {
      /*
       * The object referenced by the `attr` parameter shall be an initialized
       * condition variable attributes object.
       */
      T_true( ctx->attr_obj.is_initialized );
      break;
    }

    case CPthreadReqCondattrInit_Post_Initialized_NA:
      break;
  }
}

static void CPthreadReqCondattrInit_Post_Clock_Check(
  CPthreadReqCondattrInit_Context   *ctx,
  CPthreadReqCondattrInit_Post_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondattrInit_Post_Clock_Realtime: {
      /*
       * The condition variable clock attribute of the condition variable
       * attributes object referenced by the `attr` parameter shall be
       * CLOCK_REALTIME.
       */
      T_eq_ulong( ctx->attr_obj.clock, CLOCK_REALTIME );
      break;
    }

    case CPthreadReqCondattrInit_Post_Clock_NA:
      break;
  }
}

static void CPthreadReqCondattrInit_Post_ProcessShared_Check(
  CPthreadReqCondattrInit_Context           *ctx,
  CPthreadReqCondattrInit_Post_ProcessShared state
)
{
  switch ( state ) {
    case CPthreadReqCondattrInit_Post_ProcessShared_Private: {
      /*
       * The process-shared attribute of the condition variable attributes
       * object referenced by the `attr` parameter shall be
       * PTHREAD_PROCESS_PRIVATE.
       */
      T_eq_int( ctx->attr_obj.process_shared, PTHREAD_PROCESS_PRIVATE );
      break;
    }

    case CPthreadReqCondattrInit_Post_ProcessShared_NA:
      break;
  }
}

static void CPthreadReqCondattrInit_Action(
  CPthreadReqCondattrInit_Context *ctx
)
{
  ctx->status = pthread_condattr_init( ctx->attr );
}

/* clang-format off */

static const CPthreadReqCondattrInit_Entry
CPthreadReqCondattrInit_Entries[] = {
  { 0, 0, CPthreadReqCondattrInit_Post_Status_Ok,
    CPthreadReqCondattrInit_Post_Initialized_Yes,
    CPthreadReqCondattrInit_Post_Clock_Realtime,
    CPthreadReqCondattrInit_Post_ProcessShared_Private },
  { 0, 0, CPthreadReqCondattrInit_Post_Status_Inval,
    CPthreadReqCondattrInit_Post_Initialized_NA,
    CPthreadReqCondattrInit_Post_Clock_NA,
    CPthreadReqCondattrInit_Post_ProcessShared_NA }
};

static const uint8_t
CPthreadReqCondattrInit_Map[] = {
  0, 1
};

/* clang-format on */

static size_t CPthreadReqCondattrInit_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondattrInit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondattrInit_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondattrInit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondattrInit_Scope,
  .initial_context = &CPthreadReqCondattrInit_Instance
};

static inline CPthreadReqCondattrInit_Entry CPthreadReqCondattrInit_PopEntry(
  CPthreadReqCondattrInit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondattrInit_Entries
    [ CPthreadReqCondattrInit_Map[ index ] ];
}

static void CPthreadReqCondattrInit_TestVariant(
  CPthreadReqCondattrInit_Context *ctx
)
{
  CPthreadReqCondattrInit_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrInit_Action( ctx );
  CPthreadReqCondattrInit_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondattrInit_Post_Initialized_Check(
    ctx,
    ctx->Map.entry.Post_Initialized
  );
  CPthreadReqCondattrInit_Post_Clock_Check( ctx, ctx->Map.entry.Post_Clock );
  CPthreadReqCondattrInit_Post_ProcessShared_Check(
    ctx,
    ctx->Map.entry.Post_ProcessShared
  );
}

/**
 * @fn void T_case_body_CPthreadReqCondattrInit( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondattrInit,
  &CPthreadReqCondattrInit_Fixture
)
{
  CPthreadReqCondattrInit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqCondattrInit_Pre_Attr_Obj;
    ctx->Map.pcs[ 0 ] < CPthreadReqCondattrInit_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqCondattrInit_PopEntry( ctx );
    CPthreadReqCondattrInit_TestVariant( ctx );
  }
}

/** @} */
