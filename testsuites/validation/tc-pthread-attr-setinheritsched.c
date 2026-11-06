/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetinheritsched
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
 * @defgroup CPthreadReqAttrSetinheritsched \
 *   spec:/c/pthread/req/attr-setinheritsched
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetinheritsched_Pre_Attr_Obj,
  CPthreadReqAttrSetinheritsched_Pre_Attr_Null,
  CPthreadReqAttrSetinheritsched_Pre_Attr_NA
} CPthreadReqAttrSetinheritsched_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetinheritsched_Pre_Initialized_Yes,
  CPthreadReqAttrSetinheritsched_Pre_Initialized_No,
  CPthreadReqAttrSetinheritsched_Pre_Initialized_NA
} CPthreadReqAttrSetinheritsched_Pre_Initialized;

typedef enum {
  CPthreadReqAttrSetinheritsched_Pre_InheritSched_Inherit,
  CPthreadReqAttrSetinheritsched_Pre_InheritSched_Explicit,
  CPthreadReqAttrSetinheritsched_Pre_InheritSched_Invalid,
  CPthreadReqAttrSetinheritsched_Pre_InheritSched_NA
} CPthreadReqAttrSetinheritsched_Pre_InheritSched;

typedef enum {
  CPthreadReqAttrSetinheritsched_Post_Status_Ok,
  CPthreadReqAttrSetinheritsched_Post_Status_Inval,
  CPthreadReqAttrSetinheritsched_Post_Status_NotSup,
  CPthreadReqAttrSetinheritsched_Post_Status_NA
} CPthreadReqAttrSetinheritsched_Post_Status;

typedef enum {
  CPthreadReqAttrSetinheritsched_Post_InheritSched_Inherit,
  CPthreadReqAttrSetinheritsched_Post_InheritSched_Explicit,
  CPthreadReqAttrSetinheritsched_Post_InheritSched_Nop,
  CPthreadReqAttrSetinheritsched_Post_InheritSched_NA
} CPthreadReqAttrSetinheritsched_Post_InheritSched;

typedef enum {
  CPthreadReqAttrSetinheritsched_Post_Other_Nop,
  CPthreadReqAttrSetinheritsched_Post_Other_NA
} CPthreadReqAttrSetinheritsched_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_InheritSched_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_InheritSched : 2;
  uint16_t Post_Other : 1;
} CPthreadReqAttrSetinheritsched_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setinheritsched test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `inheritsched` parameter value.
   */
  int inheritsched;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_setinheritsched() call.
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
    CPthreadReqAttrSetinheritsched_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetinheritsched_Context;

static CPthreadReqAttrSetinheritsched_Context
  CPthreadReqAttrSetinheritsched_Instance;

static const char *const CPthreadReqAttrSetinheritsched_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrSetinheritsched_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const
  CPthreadReqAttrSetinheritsched_PreDesc_InheritSched[] =
    { "Inherit", "Explicit", "Invalid", "NA" };

static const char *const *const CPthreadReqAttrSetinheritsched_PreDesc[] = {
  CPthreadReqAttrSetinheritsched_PreDesc_Attr,
  CPthreadReqAttrSetinheritsched_PreDesc_Initialized,
  CPthreadReqAttrSetinheritsched_PreDesc_InheritSched,
  NULL
};

#define INHERIT_SCHED_INVALID 123

static void CPthreadReqAttrSetinheritsched_Pre_Attr_Prepare(
  CPthreadReqAttrSetinheritsched_Context *ctx,
  CPthreadReqAttrSetinheritsched_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Pre_Initialized_Prepare(
  CPthreadReqAttrSetinheritsched_Context        *ctx,
  CPthreadReqAttrSetinheritsched_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      ctx->attr_obj.inheritsched = -1;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.inheritsched = -1;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Pre_InheritSched_Prepare(
  CPthreadReqAttrSetinheritsched_Context         *ctx,
  CPthreadReqAttrSetinheritsched_Pre_InheritSched state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Pre_InheritSched_Inherit: {
      /*
       * While the `inheritsched` parameter is equal to PTHREAD_INHERIT_SCHED.
       */
      ctx->inheritsched = PTHREAD_INHERIT_SCHED;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_InheritSched_Explicit: {
      /*
       * While the `inheritsched` parameter is equal to PTHREAD_EXPLICIT_SCHED.
       */
      ctx->inheritsched = PTHREAD_EXPLICIT_SCHED;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_InheritSched_Invalid: {
      /*
       * While the `inheritsched` parameter is not a valid thread inherit
       * scheduler attribute.
       */
      ctx->inheritsched = INHERIT_SCHED_INVALID;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_InheritSched_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Post_Status_Check(
  CPthreadReqAttrSetinheritsched_Context    *ctx,
  CPthreadReqAttrSetinheritsched_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_setinheritsched() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_setinheritsched() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_Status_NotSup: {
      /*
       * The return value of pthread_attr_setinheritsched() shall be ENOTSUP.
       */
      T_eq_int( ctx->status, ENOTSUP );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Post_InheritSched_Check(
  CPthreadReqAttrSetinheritsched_Context          *ctx,
  CPthreadReqAttrSetinheritsched_Post_InheritSched state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Post_InheritSched_Inherit: {
      /*
       * The thread inherit scheduler attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_INHERIT_SCHED.
       */
      T_eq_int( ctx->attr_obj.inheritsched, PTHREAD_INHERIT_SCHED );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_InheritSched_Explicit: {
      /*
       * The thread inherit scheduler attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_EXPLICIT_SCHED.
       */
      T_eq_int( ctx->attr_obj.inheritsched, PTHREAD_EXPLICIT_SCHED );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_InheritSched_Nop: {
      /*
       * The thread inherit scheduler attribute of the thread attributes object
       * referenced by the `attr` parameter shall not be modified.
       */
      T_eq_int( ctx->attr_obj.inheritsched, ctx->attr_before.inheritsched );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_InheritSched_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Post_Other_Check(
  CPthreadReqAttrSetinheritsched_Context   *ctx,
  CPthreadReqAttrSetinheritsched_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Post_Other_Nop: {
      /*
       * The content of the thread attributes object referenced by the `attr`
       * parameter, other than its thread inherit scheduler attribute, shall
       * not be modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.inheritsched = ctx->attr_obj.inheritsched;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Action(
  CPthreadReqAttrSetinheritsched_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_setinheritsched( ctx->attr, ctx->inheritsched );
}

/* clang-format off */

static const CPthreadReqAttrSetinheritsched_Entry
CPthreadReqAttrSetinheritsched_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_Inval,
    CPthreadReqAttrSetinheritsched_Post_InheritSched_Nop,
    CPthreadReqAttrSetinheritsched_Post_Other_Nop },
  { 0, 0, 1, 0, CPthreadReqAttrSetinheritsched_Post_Status_Inval,
    CPthreadReqAttrSetinheritsched_Post_InheritSched_NA,
    CPthreadReqAttrSetinheritsched_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_NA,
    CPthreadReqAttrSetinheritsched_Post_InheritSched_NA,
    CPthreadReqAttrSetinheritsched_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_Ok,
    CPthreadReqAttrSetinheritsched_Post_InheritSched_Inherit,
    CPthreadReqAttrSetinheritsched_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_Ok,
    CPthreadReqAttrSetinheritsched_Post_InheritSched_Explicit,
    CPthreadReqAttrSetinheritsched_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_NotSup,
    CPthreadReqAttrSetinheritsched_Post_InheritSched_Nop,
    CPthreadReqAttrSetinheritsched_Post_Other_Nop }
};

static const uint8_t
CPthreadReqAttrSetinheritsched_Map[] = {
  3, 4, 5, 0, 0, 0, 1, 1, 1, 2, 2, 2
};

/* clang-format on */

static size_t CPthreadReqAttrSetinheritsched_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetinheritsched_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetinheritsched_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetinheritsched_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetinheritsched_Scope,
  .initial_context = &CPthreadReqAttrSetinheritsched_Instance
};

static inline CPthreadReqAttrSetinheritsched_Entry
CPthreadReqAttrSetinheritsched_PopEntry(
  CPthreadReqAttrSetinheritsched_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetinheritsched_Entries
    [ CPthreadReqAttrSetinheritsched_Map[ index ] ];
}

static void CPthreadReqAttrSetinheritsched_SetPreConditionStates(
  CPthreadReqAttrSetinheritsched_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetinheritsched_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqAttrSetinheritsched_TestVariant(
  CPthreadReqAttrSetinheritsched_Context *ctx
)
{
  CPthreadReqAttrSetinheritsched_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetinheritsched_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetinheritsched_Pre_InheritSched_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrSetinheritsched_Action( ctx );
  CPthreadReqAttrSetinheritsched_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetinheritsched_Post_InheritSched_Check(
    ctx,
    ctx->Map.entry.Post_InheritSched
  );
  CPthreadReqAttrSetinheritsched_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetinheritsched( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetinheritsched,
  &CPthreadReqAttrSetinheritsched_Fixture
)
{
  CPthreadReqAttrSetinheritsched_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrSetinheritsched_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrSetinheritsched_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrSetinheritsched_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrSetinheritsched_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqAttrSetinheritsched_Pre_InheritSched_Inherit;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrSetinheritsched_Pre_InheritSched_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrSetinheritsched_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqAttrSetinheritsched_SetPreConditionStates( ctx );
        CPthreadReqAttrSetinheritsched_TestVariant( ctx );
      }
    }
  }
}

/** @} */
