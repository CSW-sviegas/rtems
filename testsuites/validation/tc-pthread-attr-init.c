/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrInit
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
#include <sched.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqAttrInit spec:/c/pthread/req/attr-init
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrInit_Pre_Attr_Valid,
  CPthreadReqAttrInit_Pre_Attr_Null,
  CPthreadReqAttrInit_Pre_Attr_NA
} CPthreadReqAttrInit_Pre_Attr;

typedef enum {
  CPthreadReqAttrInit_Post_Status_Zero,
  CPthreadReqAttrInit_Post_Status_EINVAL,
  CPthreadReqAttrInit_Post_Status_NA
} CPthreadReqAttrInit_Post_Status;

typedef enum {
  CPthreadReqAttrInit_Post_AttrObj_Initialized,
  CPthreadReqAttrInit_Post_AttrObj_NA
} CPthreadReqAttrInit_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 1;
} CPthreadReqAttrInit_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-init test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

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
    CPthreadReqAttrInit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrInit_Context;

static CPthreadReqAttrInit_Context
  CPthreadReqAttrInit_Instance;

static const char * const CPthreadReqAttrInit_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqAttrInit_PreDesc[] = {
  CPthreadReqAttrInit_PreDesc_Attr,
  NULL
};

static void CPthreadReqAttrInit_Pre_Attr_Prepare(
  CPthreadReqAttrInit_Context *ctx,
  CPthreadReqAttrInit_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references a thread attributes object.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = false;
      break;
    }

    case CPthreadReqAttrInit_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrInit_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_Status_Check(
  CPthreadReqAttrInit_Context    *ctx,
  CPthreadReqAttrInit_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_init() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrInit_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_init() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrInit_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_AttrObj_Check(
  CPthreadReqAttrInit_Context     *ctx,
  CPthreadReqAttrInit_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_AttrObj_Initialized: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * be set to the default thread attributes after the return of the
       * pthread_attr_init() call.
       */
      T_true( ctx->attr_obj.is_initialized );
      T_eq_ptr( ctx->attr_obj.stackaddr, NULL );
      T_eq_sz( ctx->attr_obj.stacksize, 0 );
      T_eq_int( ctx->attr_obj.contentionscope, PTHREAD_SCOPE_PROCESS );
      T_eq_int( ctx->attr_obj.inheritsched, PTHREAD_INHERIT_SCHED );
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_FIFO );
      T_eq_int( ctx->attr_obj.schedparam.sched_priority, 2 );
      T_eq_sz( ctx->attr_obj.guardsize, 0 );
      T_eq_int( ctx->attr_obj.cputime_clock_allowed, 1 );
      T_eq_int( ctx->attr_obj.detachstate, PTHREAD_CREATE_JOINABLE );
      T_eq_sz(
        ctx->attr_obj.affinitysetsize,
        sizeof( ctx->attr_obj.affinitysetpreallocated )
      );
      T_eq_ptr(
        ctx->attr_obj.affinityset,
        &ctx->attr_obj.affinitysetpreallocated
      );
      T_true( CPU_ISSET( 0, &ctx->attr_obj.affinitysetpreallocated ) );
      break;
    }

    case CPthreadReqAttrInit_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Action( CPthreadReqAttrInit_Context *ctx )
{
  ctx->retval = pthread_attr_init( ctx->attr );
}

static const CPthreadReqAttrInit_Entry
CPthreadReqAttrInit_Entries[] = {
  { 0, 0, CPthreadReqAttrInit_Post_Status_Zero,
    CPthreadReqAttrInit_Post_AttrObj_Initialized },
  { 0, 0, CPthreadReqAttrInit_Post_Status_EINVAL,
    CPthreadReqAttrInit_Post_AttrObj_NA }
};

static const uint8_t
CPthreadReqAttrInit_Map[] = {
  0, 1
};

static size_t CPthreadReqAttrInit_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqAttrInit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqAttrInit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqAttrInit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrInit_Scope,
  .initial_context = &CPthreadReqAttrInit_Instance
};

static inline CPthreadReqAttrInit_Entry CPthreadReqAttrInit_PopEntry(
  CPthreadReqAttrInit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrInit_Entries[
    CPthreadReqAttrInit_Map[ index ]
  ];
}

static void CPthreadReqAttrInit_TestVariant( CPthreadReqAttrInit_Context *ctx )
{
  CPthreadReqAttrInit_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrInit_Action( ctx );
  CPthreadReqAttrInit_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrInit_Post_AttrObj_Check( ctx, ctx->Map.entry.Post_AttrObj );
}

/**
 * @fn void T_case_body_CPthreadReqAttrInit( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqAttrInit, &CPthreadReqAttrInit_Fixture )
{
  CPthreadReqAttrInit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrInit_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrInit_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqAttrInit_PopEntry( ctx );
    CPthreadReqAttrInit_TestVariant( ctx );
  }
}

/** @} */
