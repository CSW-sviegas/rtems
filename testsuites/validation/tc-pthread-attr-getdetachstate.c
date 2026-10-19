/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetdetachstate
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqAttrGetdetachstate \
 *   spec:/c/pthread/req/attr-getdetachstate
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetdetachstate_Pre_Attr_Valid,
  CPthreadReqAttrGetdetachstate_Pre_Attr_Null,
  CPthreadReqAttrGetdetachstate_Pre_Attr_Invalid,
  CPthreadReqAttrGetdetachstate_Pre_Attr_NA
} CPthreadReqAttrGetdetachstate_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetdetachstate_Pre_DetachState_Valid,
  CPthreadReqAttrGetdetachstate_Pre_DetachState_Null,
  CPthreadReqAttrGetdetachstate_Pre_DetachState_NA
} CPthreadReqAttrGetdetachstate_Pre_DetachState;

typedef enum {
  CPthreadReqAttrGetdetachstate_Post_Status_Zero,
  CPthreadReqAttrGetdetachstate_Post_Status_EINVAL,
  CPthreadReqAttrGetdetachstate_Post_Status_NA
} CPthreadReqAttrGetdetachstate_Post_Status;

typedef enum {
  CPthreadReqAttrGetdetachstate_Post_DetachStateObj_Set,
  CPthreadReqAttrGetdetachstate_Post_DetachStateObj_Nop,
  CPthreadReqAttrGetdetachstate_Post_DetachStateObj_NA
} CPthreadReqAttrGetdetachstate_Post_DetachStateObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_DetachState_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_DetachStateObj : 2;
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
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains the detach state output object.
   */
  int detachstate_value;

  struct {
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

static const char * const CPthreadReqAttrGetdetachstate_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrGetdetachstate_PreDesc_DetachState[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqAttrGetdetachstate_PreDesc[] = {
  CPthreadReqAttrGetdetachstate_PreDesc_Attr,
  CPthreadReqAttrGetdetachstate_PreDesc_DetachState,
  NULL
};

#define DETACH_STATE_VALUE PTHREAD_CREATE_DETACHED

static void CPthreadReqAttrGetdetachstate_Pre_Attr_Prepare(
  CPthreadReqAttrGetdetachstate_Context *ctx,
  CPthreadReqAttrGetdetachstate_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      pthread_attr_setdetachstate( &ctx->attr_obj, DETACH_STATE_VALUE );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Pre_DetachState_Prepare(
  CPthreadReqAttrGetdetachstate_Context        *ctx,
  CPthreadReqAttrGetdetachstate_Pre_DetachState state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Pre_DetachState_Valid: {
      /*
       * While the `detachstate` parameter references an object of type `int`.
       */
      ctx->detachstate_value = -1;
      ctx->detachstate = &ctx->detachstate_value;
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
    case CPthreadReqAttrGetdetachstate_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_getdetachstate() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_getdetachstate() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Post_DetachStateObj_Check(
  CPthreadReqAttrGetdetachstate_Context            *ctx,
  CPthreadReqAttrGetdetachstate_Post_DetachStateObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetdetachstate_Post_DetachStateObj_Set: {
      /*
       * The value of the object referenced by the `detachstate` parameter
       * shall be set to the thread detach state attribute of the thread
       * attributes object referenced by the `attr` parameter.
       */
      T_eq_int( ctx->detachstate_value, DETACH_STATE_VALUE );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_DetachStateObj_Nop: {
      /*
       * The object referenced by the `detachstate` parameter shall not be
       * modified.
       */
      T_eq_int( ctx->detachstate_value, -1 );
      break;
    }

    case CPthreadReqAttrGetdetachstate_Post_DetachStateObj_NA:
      break;
  }
}

static void CPthreadReqAttrGetdetachstate_Action(
  CPthreadReqAttrGetdetachstate_Context *ctx
)
{
  ctx->retval = pthread_attr_getdetachstate( ctx->attr, ctx->detachstate );
}

static const CPthreadReqAttrGetdetachstate_Entry
CPthreadReqAttrGetdetachstate_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrGetdetachstate_Post_Status_EINVAL,
    CPthreadReqAttrGetdetachstate_Post_DetachStateObj_NA },
  { 0, 0, 0, CPthreadReqAttrGetdetachstate_Post_Status_EINVAL,
    CPthreadReqAttrGetdetachstate_Post_DetachStateObj_Nop },
  { 0, 0, 0, CPthreadReqAttrGetdetachstate_Post_Status_Zero,
    CPthreadReqAttrGetdetachstate_Post_DetachStateObj_Set }
};

static const uint8_t
CPthreadReqAttrGetdetachstate_Map[] = {
  2, 0, 1, 0, 1, 0
};

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
  return CPthreadReqAttrGetdetachstate_Entries[
    CPthreadReqAttrGetdetachstate_Map[ index ]
  ];
}

static void CPthreadReqAttrGetdetachstate_TestVariant(
  CPthreadReqAttrGetdetachstate_Context *ctx
)
{
  CPthreadReqAttrGetdetachstate_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetdetachstate_Pre_DetachState_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrGetdetachstate_Action( ctx );
  CPthreadReqAttrGetdetachstate_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrGetdetachstate_Post_DetachStateObj_Check(
    ctx,
    ctx->Map.entry.Post_DetachStateObj
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
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrGetdetachstate_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrGetdetachstate_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetdetachstate_Pre_DetachState_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrGetdetachstate_Pre_DetachState_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrGetdetachstate_PopEntry( ctx );
      CPthreadReqAttrGetdetachstate_TestVariant( ctx );
    }
  }
}

/** @} */
