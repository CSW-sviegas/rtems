/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetscope
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
 * @defgroup CPthreadReqAttrGetscope spec:/c/pthread/req/attr-getscope
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetscope_Pre_Attr_Obj,
  CPthreadReqAttrGetscope_Pre_Attr_Null,
  CPthreadReqAttrGetscope_Pre_Attr_NA
} CPthreadReqAttrGetscope_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetscope_Pre_Initialized_Yes,
  CPthreadReqAttrGetscope_Pre_Initialized_No,
  CPthreadReqAttrGetscope_Pre_Initialized_NA
} CPthreadReqAttrGetscope_Pre_Initialized;

typedef enum {
  CPthreadReqAttrGetscope_Pre_StoredContentionScope_Process,
  CPthreadReqAttrGetscope_Pre_StoredContentionScope_System,
  CPthreadReqAttrGetscope_Pre_StoredContentionScope_NA
} CPthreadReqAttrGetscope_Pre_StoredContentionScope;

typedef enum {
  CPthreadReqAttrGetscope_Pre_ContentionScope_Obj,
  CPthreadReqAttrGetscope_Pre_ContentionScope_Null,
  CPthreadReqAttrGetscope_Pre_ContentionScope_NA
} CPthreadReqAttrGetscope_Pre_ContentionScope;

typedef enum {
  CPthreadReqAttrGetscope_Post_Status_Ok,
  CPthreadReqAttrGetscope_Post_Status_Inval,
  CPthreadReqAttrGetscope_Post_Status_NA
} CPthreadReqAttrGetscope_Post_Status;

typedef enum {
  CPthreadReqAttrGetscope_Post_ContentionScope_Process,
  CPthreadReqAttrGetscope_Post_ContentionScope_System,
  CPthreadReqAttrGetscope_Post_ContentionScope_Nop,
  CPthreadReqAttrGetscope_Post_ContentionScope_NA
} CPthreadReqAttrGetscope_Post_ContentionScope;

typedef enum {
  CPthreadReqAttrGetscope_Post_Attr_Nop,
  CPthreadReqAttrGetscope_Post_Attr_NA
} CPthreadReqAttrGetscope_Post_Attr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StoredContentionScope_NA : 1;
  uint16_t Pre_ContentionScope_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_ContentionScope : 2;
  uint16_t Post_Attr : 1;
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
   * @brief This member contains the return value of the
   *   pthread_attr_getscope() call.
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
   * @brief This member contains the object referenced by the `contentionscope`
   *   parameter.
   */
  int contentionscope_obj;

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
    CPthreadReqAttrGetscope_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetscope_Context;

static CPthreadReqAttrGetscope_Context CPthreadReqAttrGetscope_Instance;

static const char *const CPthreadReqAttrGetscope_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrGetscope_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const
  CPthreadReqAttrGetscope_PreDesc_StoredContentionScope[] =
    { "Process", "System", "NA" };

static const char *const CPthreadReqAttrGetscope_PreDesc_ContentionScope[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrGetscope_PreDesc[] = {
  CPthreadReqAttrGetscope_PreDesc_Attr,
  CPthreadReqAttrGetscope_PreDesc_Initialized,
  CPthreadReqAttrGetscope_PreDesc_StoredContentionScope,
  CPthreadReqAttrGetscope_PreDesc_ContentionScope,
  NULL
};

#define CONTENTION_SCOPE_POISON -1

static void CPthreadReqAttrGetscope_Pre_Attr_Prepare(
  CPthreadReqAttrGetscope_Context *ctx,
  CPthreadReqAttrGetscope_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrGetscope_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetscope_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Pre_Initialized_Prepare(
  CPthreadReqAttrGetscope_Context        *ctx,
  CPthreadReqAttrGetscope_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetscope_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetscope_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Pre_StoredContentionScope_Prepare(
  CPthreadReqAttrGetscope_Context                  *ctx,
  CPthreadReqAttrGetscope_Pre_StoredContentionScope state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Pre_StoredContentionScope_Process: {
      /*
       * While the thread contention scope attribute of the object referenced
       * by the `attr` parameter is PTHREAD_SCOPE_PROCESS.
       */
      ctx->attr_obj.contentionscope = PTHREAD_SCOPE_PROCESS;
      break;
    }

    case CPthreadReqAttrGetscope_Pre_StoredContentionScope_System: {
      /*
       * While the thread contention scope attribute of the object referenced
       * by the `attr` parameter is PTHREAD_SCOPE_SYSTEM.
       */
      ctx->attr_obj.contentionscope = PTHREAD_SCOPE_SYSTEM;
      break;
    }

    case CPthreadReqAttrGetscope_Pre_StoredContentionScope_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Pre_ContentionScope_Prepare(
  CPthreadReqAttrGetscope_Context            *ctx,
  CPthreadReqAttrGetscope_Pre_ContentionScope state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Pre_ContentionScope_Obj: {
      /*
       * While the `contentionscope` parameter references an object.
       */
      ctx->contentionscope_obj = CONTENTION_SCOPE_POISON;
      ctx->contentionscope = &ctx->contentionscope_obj;
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
    case CPthreadReqAttrGetscope_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_getscope() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrGetscope_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_getscope() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrGetscope_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Post_ContentionScope_Check(
  CPthreadReqAttrGetscope_Context             *ctx,
  CPthreadReqAttrGetscope_Post_ContentionScope state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Post_ContentionScope_Process: {
      /*
       * The object referenced by the `contentionscope` parameter shall be
       * PTHREAD_SCOPE_PROCESS.
       */
      T_eq_int( ctx->contentionscope_obj, PTHREAD_SCOPE_PROCESS );
      break;
    }

    case CPthreadReqAttrGetscope_Post_ContentionScope_System: {
      /*
       * The object referenced by the `contentionscope` parameter shall be
       * PTHREAD_SCOPE_SYSTEM.
       */
      T_eq_int( ctx->contentionscope_obj, PTHREAD_SCOPE_SYSTEM );
      break;
    }

    case CPthreadReqAttrGetscope_Post_ContentionScope_Nop: {
      /*
       * The object referenced by the `contentionscope` parameter shall not be
       * modified.
       */
      T_eq_int( ctx->contentionscope_obj, CONTENTION_SCOPE_POISON );
      break;
    }

    case CPthreadReqAttrGetscope_Post_ContentionScope_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Post_Attr_Check(
  CPthreadReqAttrGetscope_Context  *ctx,
  CPthreadReqAttrGetscope_Post_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetscope_Post_Attr_Nop: {
      /*
       * The object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_mem( &ctx->attr_obj, &ctx->attr_before, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetscope_Post_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetscope_Action(
  CPthreadReqAttrGetscope_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_getscope( ctx->attr, ctx->contentionscope );
}

/* clang-format off */

static const CPthreadReqAttrGetscope_Entry
CPthreadReqAttrGetscope_Entries[] = {
  { 1, 0, 0, 0, 0, CPthreadReqAttrGetscope_Post_Status_NA,
    CPthreadReqAttrGetscope_Post_ContentionScope_NA,
    CPthreadReqAttrGetscope_Post_Attr_NA },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetscope_Post_Status_Inval,
    CPthreadReqAttrGetscope_Post_ContentionScope_NA,
    CPthreadReqAttrGetscope_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetscope_Post_Status_Inval,
    CPthreadReqAttrGetscope_Post_ContentionScope_Nop,
    CPthreadReqAttrGetscope_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetscope_Post_Status_Ok,
    CPthreadReqAttrGetscope_Post_ContentionScope_Process,
    CPthreadReqAttrGetscope_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetscope_Post_Status_Ok,
    CPthreadReqAttrGetscope_Post_ContentionScope_System,
    CPthreadReqAttrGetscope_Post_Attr_Nop },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetscope_Post_Status_Inval,
    CPthreadReqAttrGetscope_Post_ContentionScope_Nop,
    CPthreadReqAttrGetscope_Post_Attr_NA },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetscope_Post_Status_Inval,
    CPthreadReqAttrGetscope_Post_ContentionScope_NA,
    CPthreadReqAttrGetscope_Post_Attr_NA }
};

static const uint8_t
CPthreadReqAttrGetscope_Map[] = {
  3, 1, 4, 1, 2, 1, 2, 1, 5, 6, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

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
  return CPthreadReqAttrGetscope_Entries
    [ CPthreadReqAttrGetscope_Map[ index ] ];
}

static void CPthreadReqAttrGetscope_SetPreConditionStates(
  CPthreadReqAttrGetscope_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetscope_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredContentionScope_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqAttrGetscope_Pre_StoredContentionScope_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void CPthreadReqAttrGetscope_TestVariant(
  CPthreadReqAttrGetscope_Context *ctx
)
{
  CPthreadReqAttrGetscope_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetscope_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrGetscope_Pre_StoredContentionScope_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrGetscope_Pre_ContentionScope_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  CPthreadReqAttrGetscope_Action( ctx );
  CPthreadReqAttrGetscope_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrGetscope_Post_ContentionScope_Check(
    ctx,
    ctx->Map.entry.Post_ContentionScope
  );
  CPthreadReqAttrGetscope_Post_Attr_Check( ctx, ctx->Map.entry.Post_Attr );
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
    ctx->Map.pci[ 0 ] = CPthreadReqAttrGetscope_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrGetscope_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrGetscope_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrGetscope_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqAttrGetscope_Pre_StoredContentionScope_Process;
        ctx->Map.pci[ 2 ] <
        CPthreadReqAttrGetscope_Pre_StoredContentionScope_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqAttrGetscope_Pre_ContentionScope_Obj;
          ctx->Map.pci[ 3 ] < CPthreadReqAttrGetscope_Pre_ContentionScope_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqAttrGetscope_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CPthreadReqAttrGetscope_SetPreConditionStates( ctx );
          CPthreadReqAttrGetscope_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
