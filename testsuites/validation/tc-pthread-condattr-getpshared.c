/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrGetpshared
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
 * @defgroup CPthreadReqCondattrGetpshared \
 *   spec:/c/pthread/req/condattr-getpshared
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrGetpshared_Pre_Attr_Obj,
  CPthreadReqCondattrGetpshared_Pre_Attr_Null,
  CPthreadReqCondattrGetpshared_Pre_Attr_NA
} CPthreadReqCondattrGetpshared_Pre_Attr;

typedef enum {
  CPthreadReqCondattrGetpshared_Pre_Initialized_Yes,
  CPthreadReqCondattrGetpshared_Pre_Initialized_No,
  CPthreadReqCondattrGetpshared_Pre_Initialized_NA
} CPthreadReqCondattrGetpshared_Pre_Initialized;

typedef enum {
  CPthreadReqCondattrGetpshared_Pre_StoredPshared_Shared,
  CPthreadReqCondattrGetpshared_Pre_StoredPshared_Private,
  CPthreadReqCondattrGetpshared_Pre_StoredPshared_NA
} CPthreadReqCondattrGetpshared_Pre_StoredPshared;

typedef enum {
  CPthreadReqCondattrGetpshared_Post_Status_Ok,
  CPthreadReqCondattrGetpshared_Post_Status_Inval,
  CPthreadReqCondattrGetpshared_Post_Status_NA
} CPthreadReqCondattrGetpshared_Post_Status;

typedef enum {
  CPthreadReqCondattrGetpshared_Post_Pshared_Shared,
  CPthreadReqCondattrGetpshared_Post_Pshared_Private,
  CPthreadReqCondattrGetpshared_Post_Pshared_Nop,
  CPthreadReqCondattrGetpshared_Post_Pshared_NA
} CPthreadReqCondattrGetpshared_Post_Pshared;

typedef enum {
  CPthreadReqCondattrGetpshared_Post_Attr_Nop,
  CPthreadReqCondattrGetpshared_Post_Attr_NA
} CPthreadReqCondattrGetpshared_Post_Attr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StoredPshared_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Pshared : 2;
  uint16_t Post_Attr : 1;
} CPthreadReqCondattrGetpshared_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/condattr-getpshared test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_condattr_t *attr;

  /**
   * @brief This member contains the return value of the
   *   pthread_condattr_getpshared() call.
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

  /**
   * @brief This member contains the object referenced by the `pshared`
   *   parameter.
   */
  int pshared_obj;

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
    CPthreadReqCondattrGetpshared_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondattrGetpshared_Context;

static CPthreadReqCondattrGetpshared_Context
  CPthreadReqCondattrGetpshared_Instance;

static const char *const CPthreadReqCondattrGetpshared_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondattrGetpshared_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const
  CPthreadReqCondattrGetpshared_PreDesc_StoredPshared[] =
    { "Shared", "Private", "NA" };

static const char *const *const CPthreadReqCondattrGetpshared_PreDesc[] = {
  CPthreadReqCondattrGetpshared_PreDesc_Attr,
  CPthreadReqCondattrGetpshared_PreDesc_Initialized,
  CPthreadReqCondattrGetpshared_PreDesc_StoredPshared,
  NULL
};

#define PSHARED_POISON 123

static void CPthreadReqCondattrGetpshared_Pre_Attr_Prepare(
  CPthreadReqCondattrGetpshared_Context *ctx,
  CPthreadReqCondattrGetpshared_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_condattr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqCondattrGetpshared_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondattrGetpshared_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Pre_Initialized_Prepare(
  CPthreadReqCondattrGetpshared_Context        *ctx,
  CPthreadReqCondattrGetpshared_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * condition variable attributes object.
       */
      T_eno_success( pthread_condattr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqCondattrGetpshared_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized condition variable attributes object.
       */
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = 0;
      break;
    }

    case CPthreadReqCondattrGetpshared_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Pre_StoredPshared_Prepare(
  CPthreadReqCondattrGetpshared_Context          *ctx,
  CPthreadReqCondattrGetpshared_Pre_StoredPshared state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Pre_StoredPshared_Shared: {
      /*
       * While the process-shared attribute of the object referenced by the
       * `attr` parameter is PTHREAD_PROCESS_SHARED.
       */
      ctx->attr_obj.process_shared = PTHREAD_PROCESS_SHARED;
      break;
    }

    case CPthreadReqCondattrGetpshared_Pre_StoredPshared_Private: {
      /*
       * While the process-shared attribute of the object referenced by the
       * `attr` parameter is PTHREAD_PROCESS_PRIVATE.
       */
      ctx->attr_obj.process_shared = PTHREAD_PROCESS_PRIVATE;
      break;
    }

    case CPthreadReqCondattrGetpshared_Pre_StoredPshared_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Post_Status_Check(
  CPthreadReqCondattrGetpshared_Context    *ctx,
  CPthreadReqCondattrGetpshared_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Post_Status_Ok: {
      /*
       * The return value of pthread_condattr_getpshared() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_Status_Inval: {
      /*
       * The return value of pthread_condattr_getpshared() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Post_Pshared_Check(
  CPthreadReqCondattrGetpshared_Context     *ctx,
  CPthreadReqCondattrGetpshared_Post_Pshared state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Post_Pshared_Shared: {
      /*
       * The object referenced by the `pshared` parameter shall be
       * PTHREAD_PROCESS_SHARED.
       */
      T_eq_int( ctx->pshared_obj, PTHREAD_PROCESS_SHARED );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_Pshared_Private: {
      /*
       * The object referenced by the `pshared` parameter shall be
       * PTHREAD_PROCESS_PRIVATE.
       */
      T_eq_int( ctx->pshared_obj, PTHREAD_PROCESS_PRIVATE );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_Pshared_Nop: {
      /*
       * The object referenced by the `pshared` parameter shall not be
       * modified.
       */
      T_eq_int( ctx->pshared_obj, PSHARED_POISON );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_Pshared_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Post_Attr_Check(
  CPthreadReqCondattrGetpshared_Context  *ctx,
  CPthreadReqCondattrGetpshared_Post_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Post_Attr_Nop: {
      /*
       * The object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_mem( &ctx->attr_obj, &ctx->attr_before, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Prepare(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  ctx->pshared_obj = PSHARED_POISON;
}

static void CPthreadReqCondattrGetpshared_Action(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_condattr_getpshared( ctx->attr, &ctx->pshared_obj );
}

/* clang-format off */

static const CPthreadReqCondattrGetpshared_Entry
CPthreadReqCondattrGetpshared_Entries[] = {
  { 1, 0, 0, 0, CPthreadReqCondattrGetpshared_Post_Status_NA,
    CPthreadReqCondattrGetpshared_Post_Pshared_NA,
    CPthreadReqCondattrGetpshared_Post_Attr_NA },
  { 0, 0, 0, 0, CPthreadReqCondattrGetpshared_Post_Status_Ok,
    CPthreadReqCondattrGetpshared_Post_Pshared_Shared,
    CPthreadReqCondattrGetpshared_Post_Attr_Nop },
  { 0, 0, 0, 0, CPthreadReqCondattrGetpshared_Post_Status_Ok,
    CPthreadReqCondattrGetpshared_Post_Pshared_Private,
    CPthreadReqCondattrGetpshared_Post_Attr_Nop },
  { 0, 0, 1, 1, CPthreadReqCondattrGetpshared_Post_Status_Inval,
    CPthreadReqCondattrGetpshared_Post_Pshared_Nop,
    CPthreadReqCondattrGetpshared_Post_Attr_NA }
};

static const uint8_t
CPthreadReqCondattrGetpshared_Map[] = {
  1, 2, 1, 2, 3, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondattrGetpshared_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqCondattrGetpshared_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondattrGetpshared_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondattrGetpshared_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondattrGetpshared_Scope,
  .initial_context = &CPthreadReqCondattrGetpshared_Instance
};

static inline CPthreadReqCondattrGetpshared_Entry
CPthreadReqCondattrGetpshared_PopEntry(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondattrGetpshared_Entries
    [ CPthreadReqCondattrGetpshared_Map[ index ] ];
}

static void CPthreadReqCondattrGetpshared_SetPreConditionStates(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondattrGetpshared_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredPshared_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondattrGetpshared_Pre_StoredPshared_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CPthreadReqCondattrGetpshared_TestVariant(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  CPthreadReqCondattrGetpshared_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrGetpshared_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqCondattrGetpshared_Pre_StoredPshared_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqCondattrGetpshared_Action( ctx );
  CPthreadReqCondattrGetpshared_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrGetpshared_Post_Pshared_Check(
    ctx,
    ctx->Map.entry.Post_Pshared
  );
  CPthreadReqCondattrGetpshared_Post_Attr_Check(
    ctx,
    ctx->Map.entry.Post_Attr
  );
}

/**
 * @fn void T_case_body_CPthreadReqCondattrGetpshared( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondattrGetpshared,
  &CPthreadReqCondattrGetpshared_Fixture
)
{
  CPthreadReqCondattrGetpshared_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondattrGetpshared_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondattrGetpshared_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondattrGetpshared_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCondattrGetpshared_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqCondattrGetpshared_Pre_StoredPshared_Shared;
        ctx->Map.pci[ 2 ] < CPthreadReqCondattrGetpshared_Pre_StoredPshared_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqCondattrGetpshared_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqCondattrGetpshared_SetPreConditionStates( ctx );
        CPthreadReqCondattrGetpshared_Prepare( ctx );
        CPthreadReqCondattrGetpshared_TestVariant( ctx );
      }
    }
  }
}

/** @} */
