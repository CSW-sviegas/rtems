/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetdetachstate
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
 * @defgroup CPthreadReqAttrSetdetachstate \
 *   spec:/c/pthread/req/attr-setdetachstate
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetdetachstate_Pre_Attr_Valid,
  CPthreadReqAttrSetdetachstate_Pre_Attr_Null,
  CPthreadReqAttrSetdetachstate_Pre_Attr_Invalid,
  CPthreadReqAttrSetdetachstate_Pre_Attr_NA
} CPthreadReqAttrSetdetachstate_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetdetachstate_Pre_DetachState_Detached,
  CPthreadReqAttrSetdetachstate_Pre_DetachState_Joinable,
  CPthreadReqAttrSetdetachstate_Pre_DetachState_Invalid,
  CPthreadReqAttrSetdetachstate_Pre_DetachState_NA
} CPthreadReqAttrSetdetachstate_Pre_DetachState;

typedef enum {
  CPthreadReqAttrSetdetachstate_Post_Status_Zero,
  CPthreadReqAttrSetdetachstate_Post_Status_EINVAL,
  CPthreadReqAttrSetdetachstate_Post_Status_NA
} CPthreadReqAttrSetdetachstate_Post_Status;

typedef enum {
  CPthreadReqAttrSetdetachstate_Post_AttrObj_Set,
  CPthreadReqAttrSetdetachstate_Post_AttrObj_Nop,
  CPthreadReqAttrSetdetachstate_Post_AttrObj_NA
} CPthreadReqAttrSetdetachstate_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_DetachState_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
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
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

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

static const char * const CPthreadReqAttrSetdetachstate_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrSetdetachstate_PreDesc_DetachState[] = {
  "Detached",
  "Joinable",
  "Invalid",
  "NA"
};

static const char * const * const CPthreadReqAttrSetdetachstate_PreDesc[] = {
  CPthreadReqAttrSetdetachstate_PreDesc_Attr,
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
    case CPthreadReqAttrSetdetachstate_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      ctx->attr_obj.detachstate = -1;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.detachstate = -1;
      break;
    }

    case CPthreadReqAttrSetdetachstate_Pre_Attr_NA:
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
    case CPthreadReqAttrSetdetachstate_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_setdetachstate() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_setdetachstate() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetdetachstate_Post_AttrObj_Check(
  CPthreadReqAttrSetdetachstate_Context     *ctx,
  CPthreadReqAttrSetdetachstate_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetdetachstate_Post_AttrObj_Set: {
      /*
       * The value of the thread detach state attribute of the thread
       * attributes object referenced by the `attr` parameter shall be set to
       * the state specified by the `detachstate` parameter.
       */
      int detachstate;
      int eno;

      eno = pthread_attr_getdetachstate( &ctx->attr_obj, &detachstate );
      T_eno_success( eno );
      T_eq_int( detachstate, ctx->detachstate );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_AttrObj_Nop: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * not be modified.
       */
      T_eq_int( ctx->attr_obj.detachstate, -1 );
      break;
    }

    case CPthreadReqAttrSetdetachstate_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetdetachstate_Action(
  CPthreadReqAttrSetdetachstate_Context *ctx
)
{
  ctx->retval = pthread_attr_setdetachstate( ctx->attr, ctx->detachstate );
}

static const CPthreadReqAttrSetdetachstate_Entry
CPthreadReqAttrSetdetachstate_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrSetdetachstate_Post_Status_EINVAL,
    CPthreadReqAttrSetdetachstate_Post_AttrObj_Nop },
  { 0, 0, 0, CPthreadReqAttrSetdetachstate_Post_Status_EINVAL,
    CPthreadReqAttrSetdetachstate_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqAttrSetdetachstate_Post_Status_Zero,
    CPthreadReqAttrSetdetachstate_Post_AttrObj_Set }
};

static const uint8_t
CPthreadReqAttrSetdetachstate_Map[] = {
  2, 2, 0, 1, 1, 1, 0, 0, 0
};

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
  return CPthreadReqAttrSetdetachstate_Entries[
    CPthreadReqAttrSetdetachstate_Map[ index ]
  ];
}

static void CPthreadReqAttrSetdetachstate_TestVariant(
  CPthreadReqAttrSetdetachstate_Context *ctx
)
{
  CPthreadReqAttrSetdetachstate_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetdetachstate_Pre_DetachState_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetdetachstate_Action( ctx );
  CPthreadReqAttrSetdetachstate_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetdetachstate_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
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
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetdetachstate_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetdetachstate_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetdetachstate_Pre_DetachState_Detached;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrSetdetachstate_Pre_DetachState_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrSetdetachstate_PopEntry( ctx );
      CPthreadReqAttrSetdetachstate_TestVariant( ctx );
    }
  }
}

/** @} */
