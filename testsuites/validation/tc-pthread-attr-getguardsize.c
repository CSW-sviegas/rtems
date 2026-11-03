/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetguardsize
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
 * @defgroup CPthreadReqAttrGetguardsize spec:/c/pthread/req/attr-getguardsize
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetguardsize_Pre_Attr_Obj,
  CPthreadReqAttrGetguardsize_Pre_Attr_Null,
  CPthreadReqAttrGetguardsize_Pre_Attr_NA
} CPthreadReqAttrGetguardsize_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetguardsize_Pre_Initialized_Yes,
  CPthreadReqAttrGetguardsize_Pre_Initialized_No,
  CPthreadReqAttrGetguardsize_Pre_Initialized_NA
} CPthreadReqAttrGetguardsize_Pre_Initialized;

typedef enum {
  CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_Zero,
  CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_Positive,
  CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_NA
} CPthreadReqAttrGetguardsize_Pre_StoredGuardSize;

typedef enum {
  CPthreadReqAttrGetguardsize_Pre_GuardSize_Obj,
  CPthreadReqAttrGetguardsize_Pre_GuardSize_Null,
  CPthreadReqAttrGetguardsize_Pre_GuardSize_NA
} CPthreadReqAttrGetguardsize_Pre_GuardSize;

typedef enum {
  CPthreadReqAttrGetguardsize_Post_Status_Ok,
  CPthreadReqAttrGetguardsize_Post_Status_Inval,
  CPthreadReqAttrGetguardsize_Post_Status_NA
} CPthreadReqAttrGetguardsize_Post_Status;

typedef enum {
  CPthreadReqAttrGetguardsize_Post_GuardSize_Zero,
  CPthreadReqAttrGetguardsize_Post_GuardSize_Positive,
  CPthreadReqAttrGetguardsize_Post_GuardSize_Nop,
  CPthreadReqAttrGetguardsize_Post_GuardSize_NA
} CPthreadReqAttrGetguardsize_Post_GuardSize;

typedef enum {
  CPthreadReqAttrGetguardsize_Post_Attr_Nop,
  CPthreadReqAttrGetguardsize_Post_Attr_NA
} CPthreadReqAttrGetguardsize_Post_Attr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StoredGuardSize_NA : 1;
  uint16_t Pre_GuardSize_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_GuardSize : 2;
  uint16_t Post_Attr : 1;
} CPthreadReqAttrGetguardsize_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-getguardsize test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_attr_t *attr;

  /**
   * @brief This member specifies the `guardsize` parameter value.
   */
  size_t *guardsize;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_getguardsize() call.
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
   * @brief This member contains the object referenced by the `guardsize`
   *   parameter.
   */
  size_t guardsize_obj;

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
    CPthreadReqAttrGetguardsize_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetguardsize_Context;

static CPthreadReqAttrGetguardsize_Context
  CPthreadReqAttrGetguardsize_Instance;

static const char *const CPthreadReqAttrGetguardsize_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrGetguardsize_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const
  CPthreadReqAttrGetguardsize_PreDesc_StoredGuardSize[] =
    { "Zero", "Positive", "NA" };

static const char *const CPthreadReqAttrGetguardsize_PreDesc_GuardSize[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrGetguardsize_PreDesc[] = {
  CPthreadReqAttrGetguardsize_PreDesc_Attr,
  CPthreadReqAttrGetguardsize_PreDesc_Initialized,
  CPthreadReqAttrGetguardsize_PreDesc_StoredGuardSize,
  CPthreadReqAttrGetguardsize_PreDesc_GuardSize,
  NULL
};

#define GUARD_SIZE 4096

static void CPthreadReqAttrGetguardsize_Pre_Attr_Prepare(
  CPthreadReqAttrGetguardsize_Context *ctx,
  CPthreadReqAttrGetguardsize_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Pre_Initialized_Prepare(
  CPthreadReqAttrGetguardsize_Context        *ctx,
  CPthreadReqAttrGetguardsize_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_Prepare(
  CPthreadReqAttrGetguardsize_Context            *ctx,
  CPthreadReqAttrGetguardsize_Pre_StoredGuardSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_Zero: {
      /*
       * While the thread guard size attribute of the object referenced by the
       * `attr` parameter is zero.
       */
      ctx->attr_obj.guardsize = 0;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_Positive: {
      /*
       * While the thread guard size attribute of the object referenced by the
       * `attr` parameter is greater than zero.
       */
      ctx->attr_obj.guardsize = GUARD_SIZE;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Pre_GuardSize_Prepare(
  CPthreadReqAttrGetguardsize_Context      *ctx,
  CPthreadReqAttrGetguardsize_Pre_GuardSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Pre_GuardSize_Obj: {
      /*
       * While the `guardsize` parameter references an object.
       */
      ctx->guardsize_obj = SIZE_MAX;
      ctx->guardsize = &ctx->guardsize_obj;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_GuardSize_Null: {
      /*
       * While the `guardsize` parameter is NULL.
       */
      ctx->guardsize = NULL;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_GuardSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Post_Status_Check(
  CPthreadReqAttrGetguardsize_Context    *ctx,
  CPthreadReqAttrGetguardsize_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_getguardsize() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_getguardsize() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Post_GuardSize_Check(
  CPthreadReqAttrGetguardsize_Context       *ctx,
  CPthreadReqAttrGetguardsize_Post_GuardSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Post_GuardSize_Zero: {
      /*
       * The object referenced by the `guardsize` parameter shall be zero.
       */
      T_eq_sz( ctx->guardsize_obj, 0 );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_GuardSize_Positive: {
      /*
       * The object referenced by the `guardsize` parameter shall be the thread
       * guard size attribute of the object referenced by the `attr` parameter.
       */
      T_eq_sz( ctx->guardsize_obj, GUARD_SIZE );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_GuardSize_Nop: {
      /*
       * The object referenced by the `guardsize` parameter shall not be
       * modified.
       */
      T_eq_sz( ctx->guardsize_obj, SIZE_MAX );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_GuardSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Post_Attr_Check(
  CPthreadReqAttrGetguardsize_Context  *ctx,
  CPthreadReqAttrGetguardsize_Post_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Post_Attr_Nop: {
      /*
       * The object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_mem( &ctx->attr_obj, &ctx->attr_before, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Action(
  CPthreadReqAttrGetguardsize_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_getguardsize( ctx->attr, ctx->guardsize );
}

/* clang-format off */

static const CPthreadReqAttrGetguardsize_Entry
CPthreadReqAttrGetguardsize_Entries[] = {
  { 1, 0, 0, 0, 0, CPthreadReqAttrGetguardsize_Post_Status_NA,
    CPthreadReqAttrGetguardsize_Post_GuardSize_NA,
    CPthreadReqAttrGetguardsize_Post_Attr_NA },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetguardsize_Post_Status_Inval,
    CPthreadReqAttrGetguardsize_Post_GuardSize_NA,
    CPthreadReqAttrGetguardsize_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetguardsize_Post_Status_Inval,
    CPthreadReqAttrGetguardsize_Post_GuardSize_Nop,
    CPthreadReqAttrGetguardsize_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetguardsize_Post_Status_Ok,
    CPthreadReqAttrGetguardsize_Post_GuardSize_Zero,
    CPthreadReqAttrGetguardsize_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetguardsize_Post_Status_Ok,
    CPthreadReqAttrGetguardsize_Post_GuardSize_Positive,
    CPthreadReqAttrGetguardsize_Post_Attr_Nop },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetguardsize_Post_Status_Inval,
    CPthreadReqAttrGetguardsize_Post_GuardSize_Nop,
    CPthreadReqAttrGetguardsize_Post_Attr_NA },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetguardsize_Post_Status_Inval,
    CPthreadReqAttrGetguardsize_Post_GuardSize_NA,
    CPthreadReqAttrGetguardsize_Post_Attr_NA }
};

static const uint8_t
CPthreadReqAttrGetguardsize_Map[] = {
  3, 1, 4, 1, 2, 1, 2, 1, 5, 6, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqAttrGetguardsize_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrGetguardsize_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrGetguardsize_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrGetguardsize_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrGetguardsize_Scope,
  .initial_context = &CPthreadReqAttrGetguardsize_Instance
};

static inline CPthreadReqAttrGetguardsize_Entry
CPthreadReqAttrGetguardsize_PopEntry(
  CPthreadReqAttrGetguardsize_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrGetguardsize_Entries
    [ CPthreadReqAttrGetguardsize_Map[ index ] ];
}

static void CPthreadReqAttrGetguardsize_SetPreConditionStates(
  CPthreadReqAttrGetguardsize_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetguardsize_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredGuardSize_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void CPthreadReqAttrGetguardsize_TestVariant(
  CPthreadReqAttrGetguardsize_Context *ctx
)
{
  CPthreadReqAttrGetguardsize_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetguardsize_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrGetguardsize_Pre_GuardSize_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqAttrGetguardsize_Action( ctx );
  CPthreadReqAttrGetguardsize_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrGetguardsize_Post_GuardSize_Check(
    ctx,
    ctx->Map.entry.Post_GuardSize
  );
  CPthreadReqAttrGetguardsize_Post_Attr_Check( ctx, ctx->Map.entry.Post_Attr );
}

/**
 * @fn void T_case_body_CPthreadReqAttrGetguardsize( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrGetguardsize,
  &CPthreadReqAttrGetguardsize_Fixture
)
{
  CPthreadReqAttrGetguardsize_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrGetguardsize_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrGetguardsize_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrGetguardsize_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrGetguardsize_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_Zero;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrGetguardsize_Pre_StoredGuardSize_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqAttrGetguardsize_Pre_GuardSize_Obj;
          ctx->Map.pci[ 3 ] < CPthreadReqAttrGetguardsize_Pre_GuardSize_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqAttrGetguardsize_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CPthreadReqAttrGetguardsize_SetPreConditionStates( ctx );
          CPthreadReqAttrGetguardsize_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
