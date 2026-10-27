/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetschedpolicy
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
 * @defgroup CPthreadReqAttrSetschedpolicy \
 *   spec:/c/pthread/req/attr-setschedpolicy
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetschedpolicy_Pre_Attr_Valid,
  CPthreadReqAttrSetschedpolicy_Pre_Attr_Null,
  CPthreadReqAttrSetschedpolicy_Pre_Attr_Invalid,
  CPthreadReqAttrSetschedpolicy_Pre_Attr_NA
} CPthreadReqAttrSetschedpolicy_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Other,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Fifo,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_RoundRobin,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Sporadic,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Invalid,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_NA
} CPthreadReqAttrSetschedpolicy_Pre_Policy;

typedef enum {
  CPthreadReqAttrSetschedpolicy_Post_Status_Zero,
  CPthreadReqAttrSetschedpolicy_Post_Status_EINVAL,
  CPthreadReqAttrSetschedpolicy_Post_Status_ENOTSUP,
  CPthreadReqAttrSetschedpolicy_Post_Status_NA
} CPthreadReqAttrSetschedpolicy_Post_Status;

typedef enum {
  CPthreadReqAttrSetschedpolicy_Post_AttrObj_Set,
  CPthreadReqAttrSetschedpolicy_Post_AttrObj_Nop,
  CPthreadReqAttrSetschedpolicy_Post_AttrObj_NA
} CPthreadReqAttrSetschedpolicy_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_Policy_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqAttrSetschedpolicy_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setschedpolicy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `policy` parameter value.
   */
  int policy;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_setschedpolicy() call.
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
    CPthreadReqAttrSetschedpolicy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetschedpolicy_Context;

static CPthreadReqAttrSetschedpolicy_Context
  CPthreadReqAttrSetschedpolicy_Instance;

static const char * const CPthreadReqAttrSetschedpolicy_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrSetschedpolicy_PreDesc_Policy[] = {
  "Other",
  "Fifo",
  "RoundRobin",
  "Sporadic",
  "Invalid",
  "NA"
};

static const char * const * const CPthreadReqAttrSetschedpolicy_PreDesc[] = {
  CPthreadReqAttrSetschedpolicy_PreDesc_Attr,
  CPthreadReqAttrSetschedpolicy_PreDesc_Policy,
  NULL
};

#define POLICY_INVALID 123

static void CPthreadReqAttrSetschedpolicy_Pre_Attr_Prepare(
  CPthreadReqAttrSetschedpolicy_Context *ctx,
  CPthreadReqAttrSetschedpolicy_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedpolicy_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      ctx->attr_obj.schedpolicy = -1;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.schedpolicy = -1;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedpolicy_Pre_Policy_Prepare(
  CPthreadReqAttrSetschedpolicy_Context   *ctx,
  CPthreadReqAttrSetschedpolicy_Pre_Policy state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedpolicy_Pre_Policy_Other: {
      /*
       * While the `policy` parameter is equal to SCHED_OTHER.
       */
      ctx->policy = SCHED_OTHER;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Policy_Fifo: {
      /*
       * While the `policy` parameter is equal to SCHED_FIFO.
       */
      ctx->policy = SCHED_FIFO;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Policy_RoundRobin: {
      /*
       * While the `policy` parameter is equal to SCHED_RR.
       */
      ctx->policy = SCHED_RR;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Policy_Sporadic: {
      /*
       * While the `policy` parameter is equal to SCHED_SPORADIC.
       */
      ctx->policy = SCHED_SPORADIC;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Policy_Invalid: {
      /*
       * While the `policy` parameter is not a valid scheduling policy.
       */
      ctx->policy = POLICY_INVALID;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Policy_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedpolicy_Post_Status_Check(
  CPthreadReqAttrSetschedpolicy_Context    *ctx,
  CPthreadReqAttrSetschedpolicy_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedpolicy_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_setschedpolicy() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_setschedpolicy() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Status_ENOTSUP: {
      /*
       * The return value of pthread_attr_setschedpolicy() shall be equal to
       * ENOTSUP.
       */
      T_eq_int( ctx->retval, ENOTSUP );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedpolicy_Post_AttrObj_Check(
  CPthreadReqAttrSetschedpolicy_Context     *ctx,
  CPthreadReqAttrSetschedpolicy_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedpolicy_Post_AttrObj_Set: {
      /*
       * The value of the scheduling policy of the thread attributes object
       * referenced by the `attr` parameter shall be set to the policy
       * specified by the `policy` parameter.
       */
      int policy;
      int eno;

      eno = pthread_attr_getschedpolicy( &ctx->attr_obj, &policy );
      T_eno_success( eno );
      T_eq_int( policy, ctx->policy );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_AttrObj_Nop: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * not be modified.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, -1 );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedpolicy_Action(
  CPthreadReqAttrSetschedpolicy_Context *ctx
)
{
  ctx->retval = pthread_attr_setschedpolicy( ctx->attr, ctx->policy );
}

/* clang-format off */

static const CPthreadReqAttrSetschedpolicy_Entry
CPthreadReqAttrSetschedpolicy_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_EINVAL,
    CPthreadReqAttrSetschedpolicy_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_EINVAL,
    CPthreadReqAttrSetschedpolicy_Post_AttrObj_Nop },
  { 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_Zero,
    CPthreadReqAttrSetschedpolicy_Post_AttrObj_Set },
  { 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_ENOTSUP,
    CPthreadReqAttrSetschedpolicy_Post_AttrObj_Nop }
};

static const uint8_t
CPthreadReqAttrSetschedpolicy_Map[] = {
  2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t CPthreadReqAttrSetschedpolicy_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetschedpolicy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetschedpolicy_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetschedpolicy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetschedpolicy_Scope,
  .initial_context = &CPthreadReqAttrSetschedpolicy_Instance
};

static inline CPthreadReqAttrSetschedpolicy_Entry
CPthreadReqAttrSetschedpolicy_PopEntry(
  CPthreadReqAttrSetschedpolicy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetschedpolicy_Entries[
    CPthreadReqAttrSetschedpolicy_Map[ index ]
  ];
}

static void CPthreadReqAttrSetschedpolicy_TestVariant(
  CPthreadReqAttrSetschedpolicy_Context *ctx
)
{
  CPthreadReqAttrSetschedpolicy_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrSetschedpolicy_Action( ctx );
  CPthreadReqAttrSetschedpolicy_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetschedpolicy_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetschedpolicy( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetschedpolicy,
  &CPthreadReqAttrSetschedpolicy_Fixture
)
{
  CPthreadReqAttrSetschedpolicy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetschedpolicy_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetschedpolicy_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetschedpolicy_Pre_Policy_Other;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrSetschedpolicy_Pre_Policy_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrSetschedpolicy_PopEntry( ctx );
      CPthreadReqAttrSetschedpolicy_TestVariant( ctx );
    }
  }
}

/** @} */
