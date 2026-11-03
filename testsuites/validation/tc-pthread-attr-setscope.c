/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetscope
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
 * @defgroup CPthreadReqAttrSetscope spec:/c/pthread/req/attr-setscope
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetscope_Pre_Attr_Obj,
  CPthreadReqAttrSetscope_Pre_Attr_Null,
  CPthreadReqAttrSetscope_Pre_Attr_NA
} CPthreadReqAttrSetscope_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetscope_Pre_Initialized_Yes,
  CPthreadReqAttrSetscope_Pre_Initialized_No,
  CPthreadReqAttrSetscope_Pre_Initialized_NA
} CPthreadReqAttrSetscope_Pre_Initialized;

typedef enum {
  CPthreadReqAttrSetscope_Pre_ContentionScope_Process,
  CPthreadReqAttrSetscope_Pre_ContentionScope_System,
  CPthreadReqAttrSetscope_Pre_ContentionScope_Invalid,
  CPthreadReqAttrSetscope_Pre_ContentionScope_NA
} CPthreadReqAttrSetscope_Pre_ContentionScope;

typedef enum {
  CPthreadReqAttrSetscope_Post_Status_Ok,
  CPthreadReqAttrSetscope_Post_Status_Inval,
  CPthreadReqAttrSetscope_Post_Status_NotSup,
  CPthreadReqAttrSetscope_Post_Status_NA
} CPthreadReqAttrSetscope_Post_Status;

typedef enum {
  CPthreadReqAttrSetscope_Post_ContentionScope_Process,
  CPthreadReqAttrSetscope_Post_ContentionScope_Nop,
  CPthreadReqAttrSetscope_Post_ContentionScope_NA
} CPthreadReqAttrSetscope_Post_ContentionScope;

typedef enum {
  CPthreadReqAttrSetscope_Post_Other_Nop,
  CPthreadReqAttrSetscope_Post_Other_NA
} CPthreadReqAttrSetscope_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_ContentionScope_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_ContentionScope : 2;
  uint16_t Post_Other : 1;
} CPthreadReqAttrSetscope_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setscope test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `contentionscope` parameter value.
   */
  int contentionscope;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_setscope() call.
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
    CPthreadReqAttrSetscope_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetscope_Context;

static CPthreadReqAttrSetscope_Context CPthreadReqAttrSetscope_Instance;

static const char *const CPthreadReqAttrSetscope_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrSetscope_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqAttrSetscope_PreDesc_ContentionScope[] =
  { "Process", "System", "Invalid", "NA" };

static const char *const *const CPthreadReqAttrSetscope_PreDesc[] = {
  CPthreadReqAttrSetscope_PreDesc_Attr,
  CPthreadReqAttrSetscope_PreDesc_Initialized,
  CPthreadReqAttrSetscope_PreDesc_ContentionScope,
  NULL
};

#define CONTENTION_SCOPE_POISON -1

#define CONTENTION_SCOPE_INVALID 123

static void CPthreadReqAttrSetscope_Pre_Attr_Prepare(
  CPthreadReqAttrSetscope_Context *ctx,
  CPthreadReqAttrSetscope_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Pre_Initialized_Prepare(
  CPthreadReqAttrSetscope_Context        *ctx,
  CPthreadReqAttrSetscope_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      ctx->attr_obj.contentionscope = CONTENTION_SCOPE_POISON;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.contentionscope = CONTENTION_SCOPE_POISON;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Pre_ContentionScope_Prepare(
  CPthreadReqAttrSetscope_Context            *ctx,
  CPthreadReqAttrSetscope_Pre_ContentionScope state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Pre_ContentionScope_Process: {
      /*
       * While the `contentionscope` parameter is equal to
       * PTHREAD_SCOPE_PROCESS.
       */
      ctx->contentionscope = PTHREAD_SCOPE_PROCESS;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_ContentionScope_System: {
      /*
       * While the `contentionscope` parameter is equal to
       * PTHREAD_SCOPE_SYSTEM.
       */
      ctx->contentionscope = PTHREAD_SCOPE_SYSTEM;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_ContentionScope_Invalid: {
      /*
       * While the `contentionscope` parameter is not a valid thread contention
       * scope attribute.
       */
      ctx->contentionscope = CONTENTION_SCOPE_INVALID;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_ContentionScope_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Post_Status_Check(
  CPthreadReqAttrSetscope_Context    *ctx,
  CPthreadReqAttrSetscope_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_setscope() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrSetscope_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_setscope() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrSetscope_Post_Status_NotSup: {
      /*
       * The return value of pthread_attr_setscope() shall be ENOTSUP.
       */
      T_eq_int( ctx->status, ENOTSUP );
      break;
    }

    case CPthreadReqAttrSetscope_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Post_ContentionScope_Check(
  CPthreadReqAttrSetscope_Context             *ctx,
  CPthreadReqAttrSetscope_Post_ContentionScope state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Post_ContentionScope_Process: {
      /*
       * The thread contention scope attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_SCOPE_PROCESS.
       */
      T_eq_int( ctx->attr_obj.contentionscope, PTHREAD_SCOPE_PROCESS );
      break;
    }

    case CPthreadReqAttrSetscope_Post_ContentionScope_Nop: {
      /*
       * The thread contention scope attribute of the thread attributes object
       * referenced by the `attr` parameter shall not be modified.
       */
      T_eq_int(
        ctx->attr_obj.contentionscope,
        ctx->attr_before.contentionscope
      );
      break;
    }

    case CPthreadReqAttrSetscope_Post_ContentionScope_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Post_Other_Check(
  CPthreadReqAttrSetscope_Context   *ctx,
  CPthreadReqAttrSetscope_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Post_Other_Nop: {
      /*
       * The content of the thread attributes object referenced by the `attr`
       * parameter, other than its thread contention scope attribute, shall not
       * be modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.contentionscope = ctx->attr_obj.contentionscope;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrSetscope_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Action(
  CPthreadReqAttrSetscope_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_setscope( ctx->attr, ctx->contentionscope );
}

/* clang-format off */

static const CPthreadReqAttrSetscope_Entry
CPthreadReqAttrSetscope_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrSetscope_Post_Status_Inval,
    CPthreadReqAttrSetscope_Post_ContentionScope_Nop,
    CPthreadReqAttrSetscope_Post_Other_Nop },
  { 0, 0, 1, 0, CPthreadReqAttrSetscope_Post_Status_Inval,
    CPthreadReqAttrSetscope_Post_ContentionScope_NA,
    CPthreadReqAttrSetscope_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqAttrSetscope_Post_Status_NA,
    CPthreadReqAttrSetscope_Post_ContentionScope_NA,
    CPthreadReqAttrSetscope_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqAttrSetscope_Post_Status_Ok,
    CPthreadReqAttrSetscope_Post_ContentionScope_Process,
    CPthreadReqAttrSetscope_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetscope_Post_Status_NotSup,
    CPthreadReqAttrSetscope_Post_ContentionScope_Nop,
    CPthreadReqAttrSetscope_Post_Other_Nop }
};

static const uint8_t
CPthreadReqAttrSetscope_Map[] = {
  3, 4, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2
};

/* clang-format on */

static size_t CPthreadReqAttrSetscope_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqAttrSetscope_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetscope_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetscope_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetscope_Scope,
  .initial_context = &CPthreadReqAttrSetscope_Instance
};

static inline CPthreadReqAttrSetscope_Entry CPthreadReqAttrSetscope_PopEntry(
  CPthreadReqAttrSetscope_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetscope_Entries
    [ CPthreadReqAttrSetscope_Map[ index ] ];
}

static void CPthreadReqAttrSetscope_SetPreConditionStates(
  CPthreadReqAttrSetscope_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetscope_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqAttrSetscope_TestVariant(
  CPthreadReqAttrSetscope_Context *ctx
)
{
  CPthreadReqAttrSetscope_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetscope_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrSetscope_Pre_ContentionScope_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrSetscope_Action( ctx );
  CPthreadReqAttrSetscope_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrSetscope_Post_ContentionScope_Check(
    ctx,
    ctx->Map.entry.Post_ContentionScope
  );
  CPthreadReqAttrSetscope_Post_Other_Check( ctx, ctx->Map.entry.Post_Other );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetscope( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetscope,
  &CPthreadReqAttrSetscope_Fixture
)
{
  CPthreadReqAttrSetscope_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrSetscope_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrSetscope_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrSetscope_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrSetscope_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqAttrSetscope_Pre_ContentionScope_Process;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrSetscope_Pre_ContentionScope_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrSetscope_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqAttrSetscope_SetPreConditionStates( ctx );
        CPthreadReqAttrSetscope_TestVariant( ctx );
      }
    }
  }
}

/** @} */
