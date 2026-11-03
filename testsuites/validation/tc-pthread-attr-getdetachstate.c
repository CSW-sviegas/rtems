/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetdetachstate
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
 * @defgroup CPthreadReqAttrGetdetachstate \
 *   spec:/c/pthread/req/attr-getdetachstate
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetdetachstate_Pre_Attr_Obj,
  CPthreadReqAttrGetdetachstate_Pre_Attr_Null,
  CPthreadReqAttrGetdetachstate_Pre_Attr_NA
} CPthreadReqAttrGetdetachstate_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetdetachstate_Pre_Initialized_Yes,
  CPthreadReqAttrGetdetachstate_Pre_Initialized_No,
  CPthreadReqAttrGetdetachstate_Pre_Initialized_NA
} CPthreadReqAttrGetdetachstate_Pre_Initialized;

typedef enum {
  CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_Detached,
  CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_Joinable,
  CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_NA
} CPthreadReqAttrGetdetachstate_Pre_StoredDetachState;

typedef enum {
  CPthreadReqAttrGetdetachstate_Pre_DetachState_Obj,
  CPthreadReqAttrGetdetachstate_Pre_DetachState_Null,
  CPthreadReqAttrGetdetachstate_Pre_DetachState_NA
} CPthreadReqAttrGetdetachstate_Pre_DetachState;

typedef enum {
  CPthreadReqAttrGetdetachstate_Post_Status_Ok,
  CPthreadReqAttrGetdetachstate_Post_Status_Inval,
  CPthreadReqAttrGetdetachstate_Post_Status_NA
} CPthreadReqAttrGetdetachstate_Post_Status;

typedef enum {
  CPthreadReqAttrGetdetachstate_Post_DetachState_Detached,
  CPthreadReqAttrGetdetachstate_Post_DetachState_Joinable,
  CPthreadReqAttrGetdetachstate_Post_DetachState_Nop,
  CPthreadReqAttrGetdetachstate_Post_DetachState_NA
} CPthreadReqAttrGetdetachstate_Post_DetachState;

typedef enum {
  CPthreadReqAttrGetdetachstate_Post_Attr_Nop,
  CPthreadReqAttrGetdetachstate_Post_Attr_NA
} CPthreadReqAttrGetdetachstate_Post_Attr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StoredDetachState_NA : 1;
  uint16_t Pre_DetachState_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_DetachState : 2;
  uint16_t Post_Attr : 1;
} CPthreadReqAttrGetdetachstate_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-getdetachstate test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_attr_t *attr;

  /**
   * @brief This member specifies the `detachstate` parameter value.
   */
  int *detachstate;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_getdetachstate() call.
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
   * @brief This member contains the object referenced by the `detachstate`
   *   parameter.
   */
  int detachstate_obj;

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
    CPthreadReqAttrGetdetachstate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetdetachstate_Context;

static CPthreadReqAttrGetdetachstate_Context
  CPthreadReqAttrGetdetachstate_Instance;

static const char *const CPthreadReqAttrGetdetachstate_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrGetdetachstate_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const
  CPthreadReqAttrGetdetachstate_PreDesc_StoredDetachState[] =
    { "Detached", "Joinable", "NA" };

static const char *const CPthreadReqAttrGetdetachstate_PreDesc_DetachState[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrGetdetachstate_PreDesc[] = {
  CPthreadReqAttrGetdetachstate_PreDesc_Attr,
  CPthreadReqAttrGetdetachstate_PreDesc_Initialized,
  CPthreadReqAttrGetdetachstate_PreDesc_StoredDetachState,
  CPthreadReqAttrGetdetachstate_PreDesc_DetachState,
  NULL
};

#define DETACH_STATE_POISON -1

static void CPthreadReqAttrGetdetachstate_Pre_Attr_Prepare(
  CPthreadReqAttrGetdetachstate_Context *ctx,
  CPthreadReqAttrGetdetachstate_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Pre_Initialized_Prepare(
  CPthreadReqAttrGetdetachstate_Context        *ctx,
  CPthreadReqAttrGetdetachstate_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_Prepare(
  CPthreadReqAttrGetdetachstate_Context              *ctx,
  CPthreadReqAttrGetdetachstate_Pre_StoredDetachState state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_Detached: {
      /*
       * While the thread detach state attribute of the object referenced by
       * the `attr` parameter is PTHREAD_CREATE_DETACHED.
       */
      ctx->attr_obj.detachstate = PTHREAD_CREATE_DETACHED;
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_Joinable: {
      /*
       * While the thread detach state attribute of the object referenced by
       * the `attr` parameter is PTHREAD_CREATE_JOINABLE.
       */
      ctx->attr_obj.detachstate = PTHREAD_CREATE_JOINABLE;
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Pre_DetachState_Prepare(
  CPthreadReqAttrGetdetachstate_Context        *ctx,
  CPthreadReqAttrGetdetachstate_Pre_DetachState state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Pre_DetachState_Obj: {
      /*
       * While the `detachstate` parameter references an object.
       */
      ctx->detachstate_obj = DETACH_STATE_POISON;
      ctx->detachstate = &ctx->detachstate_obj;
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_DetachState_Null: {
      /*
       * While the `detachstate` parameter is NULL.
       */
      ctx->detachstate = NULL;
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_DetachState_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Post_Status_Check(
  CPthreadReqAttrGetdetachstate_Context    *ctx,
  CPthreadReqAttrGetdetachstate_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_getdetachstate() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_getdetachstate() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Post_DetachState_Check(
  CPthreadReqAttrGetdetachstate_Context         *ctx,
  CPthreadReqAttrGetdetachstate_Post_DetachState state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Post_DetachState_Detached: {
      /*
       * The object referenced by the `detachstate` parameter shall be
       * PTHREAD_CREATE_DETACHED.
       */
      T_eq_int( ctx->detachstate_obj, PTHREAD_CREATE_DETACHED );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_DetachState_Joinable: {
      /*
       * The object referenced by the `detachstate` parameter shall be
       * PTHREAD_CREATE_JOINABLE.
       */
      T_eq_int( ctx->detachstate_obj, PTHREAD_CREATE_JOINABLE );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_DetachState_Nop: {
      /*
       * The object referenced by the `detachstate` parameter shall not be
       * modified.
       */
      T_eq_int( ctx->detachstate_obj, DETACH_STATE_POISON );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_DetachState_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Post_Attr_Check(
  CPthreadReqAttrGetdetachstate_Context  *ctx,
  CPthreadReqAttrGetdetachstate_Post_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Post_Attr_Nop: {
      /*
       * The object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_mem( &ctx->attr_obj, &ctx->attr_before, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Action(
  CPthreadReqAttrGetdetachstate_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_getdetachstate( ctx->attr, ctx->detachstate );
}

/* clang-format off */

static const CPthreadReqAttrGetdetachstate_Entry
CPthreadReqAttrGetdetachstate_Entries[] = {
  { 1, 0, 0, 0, 0, CPthreadReqAttrGetdetachstate_Post_Status_NA,
    CPthreadReqAttrGetdetachstate_Post_DetachState_NA,
    CPthreadReqAttrGetdetachstate_Post_Attr_NA },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetdetachstate_Post_Status_Inval,
    CPthreadReqAttrGetdetachstate_Post_DetachState_NA,
    CPthreadReqAttrGetdetachstate_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetdetachstate_Post_Status_Inval,
    CPthreadReqAttrGetdetachstate_Post_DetachState_Nop,
    CPthreadReqAttrGetdetachstate_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetdetachstate_Post_Status_Ok,
    CPthreadReqAttrGetdetachstate_Post_DetachState_Detached,
    CPthreadReqAttrGetdetachstate_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetdetachstate_Post_Status_Ok,
    CPthreadReqAttrGetdetachstate_Post_DetachState_Joinable,
    CPthreadReqAttrGetdetachstate_Post_Attr_Nop },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetdetachstate_Post_Status_Inval,
    CPthreadReqAttrGetdetachstate_Post_DetachState_Nop,
    CPthreadReqAttrGetdetachstate_Post_Attr_NA },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetdetachstate_Post_Status_Inval,
    CPthreadReqAttrGetdetachstate_Post_DetachState_NA,
    CPthreadReqAttrGetdetachstate_Post_Attr_NA }
};

static const uint8_t
CPthreadReqAttrGetdetachstate_Map[] = {
  3, 1, 4, 1, 2, 1, 2, 1, 5, 6, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqAttrGetdetachstate_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrGetdetachstate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrGetdetachstate_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrGetdetachstate_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrGetdetachstate_Scope,
  .initial_context = &CPthreadReqAttrGetdetachstate_Instance
};

static inline CPthreadReqAttrGetdetachstate_Entry
CPthreadReqAttrGetdetachstate_PopEntry(
  CPthreadReqAttrGetdetachstate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrGetdetachstate_Entries
    [ CPthreadReqAttrGetdetachstate_Map[ index ] ];
}

static void CPthreadReqAttrGetdetachstate_SetPreConditionStates(
  CPthreadReqAttrGetdetachstate_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetdetachstate_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredDetachState_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void CPthreadReqAttrGetdetachstate_TestVariant(
  CPthreadReqAttrGetdetachstate_Context *ctx
)
{
  CPthreadReqAttrGetdetachstate_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetdetachstate_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrGetdetachstate_Pre_DetachState_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  CPthreadReqAttrGetdetachstate_Action( ctx );
  CPthreadReqAttrGetdetachstate_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrGetdetachstate_Post_DetachState_Check(
    ctx,
    ctx->Map.entry.Post_DetachState
  );
  CPthreadReqAttrGetdetachstate_Post_Attr_Check(
    ctx,
    ctx->Map.entry.Post_Attr
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrGetdetachstate( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrGetdetachstate,
  &CPthreadReqAttrGetdetachstate_Fixture
)
{
  CPthreadReqAttrGetdetachstate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrGetdetachstate_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrGetdetachstate_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrGetdetachstate_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrGetdetachstate_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_Detached;
        ctx->Map.pci[ 2 ] <
        CPthreadReqAttrGetdetachstate_Pre_StoredDetachState_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] =
            CPthreadReqAttrGetdetachstate_Pre_DetachState_Obj;
          ctx->Map.pci[ 3 ] < CPthreadReqAttrGetdetachstate_Pre_DetachState_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqAttrGetdetachstate_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CPthreadReqAttrGetdetachstate_SetPreConditionStates( ctx );
          CPthreadReqAttrGetdetachstate_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
