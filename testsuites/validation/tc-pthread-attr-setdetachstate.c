/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetdetachstate
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
 * @defgroup CPthreadReqAttrSetdetachstate \
 *   spec:/c/pthread/req/attr-setdetachstate
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetdetachstate_Pre_Attr_Obj,
  CPthreadReqAttrSetdetachstate_Pre_Attr_Null,
  CPthreadReqAttrSetdetachstate_Pre_Attr_NA
} CPthreadReqAttrSetdetachstate_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetdetachstate_Pre_Initialized_Yes,
  CPthreadReqAttrSetdetachstate_Pre_Initialized_No,
  CPthreadReqAttrSetdetachstate_Pre_Initialized_NA
} CPthreadReqAttrSetdetachstate_Pre_Initialized;

typedef enum {
  CPthreadReqAttrSetdetachstate_Pre_DetachState_Detached,
  CPthreadReqAttrSetdetachstate_Pre_DetachState_Joinable,
  CPthreadReqAttrSetdetachstate_Pre_DetachState_Invalid,
  CPthreadReqAttrSetdetachstate_Pre_DetachState_NA
} CPthreadReqAttrSetdetachstate_Pre_DetachState;

typedef enum {
  CPthreadReqAttrSetdetachstate_Post_Status_Ok,
  CPthreadReqAttrSetdetachstate_Post_Status_Inval,
  CPthreadReqAttrSetdetachstate_Post_Status_NA
} CPthreadReqAttrSetdetachstate_Post_Status;

typedef enum {
  CPthreadReqAttrSetdetachstate_Post_DetachState_Detached,
  CPthreadReqAttrSetdetachstate_Post_DetachState_Joinable,
  CPthreadReqAttrSetdetachstate_Post_DetachState_Nop,
  CPthreadReqAttrSetdetachstate_Post_DetachState_NA
} CPthreadReqAttrSetdetachstate_Post_DetachState;

typedef enum {
  CPthreadReqAttrSetdetachstate_Post_Other_Nop,
  CPthreadReqAttrSetdetachstate_Post_Other_NA
} CPthreadReqAttrSetdetachstate_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_DetachState_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_DetachState : 2;
  uint16_t Post_Other : 1;
} CPthreadReqAttrSetdetachstate_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setdetachstate test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `detachstate` parameter value.
   */
  int detachstate;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_setdetachstate() call.
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
    CPthreadReqAttrSetdetachstate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetdetachstate_Context;

static CPthreadReqAttrSetdetachstate_Context
  CPthreadReqAttrSetdetachstate_Instance;

static const char *const CPthreadReqAttrSetdetachstate_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrSetdetachstate_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqAttrSetdetachstate_PreDesc_DetachState[] =
  { "Detached", "Joinable", "Invalid", "NA" };

static const char *const *const CPthreadReqAttrSetdetachstate_PreDesc[] = {
  CPthreadReqAttrSetdetachstate_PreDesc_Attr,
  CPthreadReqAttrSetdetachstate_PreDesc_Initialized,
  CPthreadReqAttrSetdetachstate_PreDesc_DetachState,
  NULL
};

#define DETACH_STATE_INVALID 123

static void CPthreadReqAttrSetdetachstate_Pre_Attr_Prepare(
  CPthreadReqAttrSetdetachstate_Context *ctx,
  CPthreadReqAttrSetdetachstate_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetdetachstate_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetdetachstate_Pre_Initialized_Prepare(
  CPthreadReqAttrSetdetachstate_Context        *ctx,
  CPthreadReqAttrSetdetachstate_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetdetachstate_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      ctx->attr_obj.detachstate = -1;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.detachstate = -1;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrSetdetachstate_Pre_DetachState_Prepare(
  CPthreadReqAttrSetdetachstate_Context        *ctx,
  CPthreadReqAttrSetdetachstate_Pre_DetachState state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetdetachstate_Pre_DetachState_Detached: {
      /*
       * While the `detachstate` parameter is equal to PTHREAD_CREATE_DETACHED.
       */
      ctx->detachstate = PTHREAD_CREATE_DETACHED;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_DetachState_Joinable: {
      /*
       * While the `detachstate` parameter is equal to PTHREAD_CREATE_JOINABLE.
       */
      ctx->detachstate = PTHREAD_CREATE_JOINABLE;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_DetachState_Invalid: {
      /*
       * While the `detachstate` parameter is not a valid thread detach state
       * attribute.
       */
      ctx->detachstate = DETACH_STATE_INVALID;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_DetachState_NA:
      break;
  }
}

static void CPthreadReqAttrSetdetachstate_Post_Status_Check(
  CPthreadReqAttrSetdetachstate_Context    *ctx,
  CPthreadReqAttrSetdetachstate_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetdetachstate_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_setdetachstate() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_setdetachstate() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetdetachstate_Post_DetachState_Check(
  CPthreadReqAttrSetdetachstate_Context         *ctx,
  CPthreadReqAttrSetdetachstate_Post_DetachState state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetdetachstate_Post_DetachState_Detached: {
      /*
       * The thread detach state attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_CREATE_DETACHED.
       */
      T_eq_int( ctx->attr_obj.detachstate, PTHREAD_CREATE_DETACHED );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_DetachState_Joinable: {
      /*
       * The thread detach state attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_CREATE_JOINABLE.
       */
      T_eq_int( ctx->attr_obj.detachstate, PTHREAD_CREATE_JOINABLE );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_DetachState_Nop: {
      /*
       * The thread detach state attribute of the thread attributes object
       * referenced by the `attr` parameter shall not be modified.
       */
      T_eq_int( ctx->attr_obj.detachstate, ctx->attr_before.detachstate );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_DetachState_NA:
      break;
  }
}

static void CPthreadReqAttrSetdetachstate_Post_Other_Check(
  CPthreadReqAttrSetdetachstate_Context   *ctx,
  CPthreadReqAttrSetdetachstate_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetdetachstate_Post_Other_Nop: {
      /*
       * The content of the thread attributes object referenced by the `attr`
       * parameter, other than its thread detach state attribute, shall not be
       * modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.detachstate = ctx->attr_obj.detachstate;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrSetdetachstate_Action(
  CPthreadReqAttrSetdetachstate_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_setdetachstate( ctx->attr, ctx->detachstate );
}

/* clang-format off */

static const CPthreadReqAttrSetdetachstate_Entry
CPthreadReqAttrSetdetachstate_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrSetdetachstate_Post_Status_Inval,
    CPthreadReqAttrSetdetachstate_Post_DetachState_Nop,
    CPthreadReqAttrSetdetachstate_Post_Other_Nop },
  { 0, 0, 1, 0, CPthreadReqAttrSetdetachstate_Post_Status_Inval,
    CPthreadReqAttrSetdetachstate_Post_DetachState_NA,
    CPthreadReqAttrSetdetachstate_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqAttrSetdetachstate_Post_Status_NA,
    CPthreadReqAttrSetdetachstate_Post_DetachState_NA,
    CPthreadReqAttrSetdetachstate_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqAttrSetdetachstate_Post_Status_Ok,
    CPthreadReqAttrSetdetachstate_Post_DetachState_Detached,
    CPthreadReqAttrSetdetachstate_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetdetachstate_Post_Status_Ok,
    CPthreadReqAttrSetdetachstate_Post_DetachState_Joinable,
    CPthreadReqAttrSetdetachstate_Post_Other_Nop }
};

static const uint8_t
CPthreadReqAttrSetdetachstate_Map[] = {
  3, 4, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2
};

/* clang-format on */

static size_t CPthreadReqAttrSetdetachstate_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetdetachstate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetdetachstate_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetdetachstate_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetdetachstate_Scope,
  .initial_context = &CPthreadReqAttrSetdetachstate_Instance
};

static inline CPthreadReqAttrSetdetachstate_Entry
CPthreadReqAttrSetdetachstate_PopEntry(
  CPthreadReqAttrSetdetachstate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetdetachstate_Entries
    [ CPthreadReqAttrSetdetachstate_Map[ index ] ];
}

static void CPthreadReqAttrSetdetachstate_SetPreConditionStates(
  CPthreadReqAttrSetdetachstate_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetdetachstate_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqAttrSetdetachstate_TestVariant(
  CPthreadReqAttrSetdetachstate_Context *ctx
)
{
  CPthreadReqAttrSetdetachstate_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetdetachstate_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetdetachstate_Pre_DetachState_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrSetdetachstate_Action( ctx );
  CPthreadReqAttrSetdetachstate_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetdetachstate_Post_DetachState_Check(
    ctx,
    ctx->Map.entry.Post_DetachState
  );
  CPthreadReqAttrSetdetachstate_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetdetachstate( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetdetachstate,
  &CPthreadReqAttrSetdetachstate_Fixture
)
{
  CPthreadReqAttrSetdetachstate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrSetdetachstate_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrSetdetachstate_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrSetdetachstate_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrSetdetachstate_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqAttrSetdetachstate_Pre_DetachState_Detached;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrSetdetachstate_Pre_DetachState_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrSetdetachstate_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqAttrSetdetachstate_SetPreConditionStates( ctx );
        CPthreadReqAttrSetdetachstate_TestVariant( ctx );
      }
    }
  }
}

/** @} */
