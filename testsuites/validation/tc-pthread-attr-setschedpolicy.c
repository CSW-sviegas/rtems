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
#include <sched.h>
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
  CPthreadReqAttrSetschedpolicy_Pre_Attr_Obj,
  CPthreadReqAttrSetschedpolicy_Pre_Attr_Null,
  CPthreadReqAttrSetschedpolicy_Pre_Attr_NA
} CPthreadReqAttrSetschedpolicy_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetschedpolicy_Pre_Initialized_Yes,
  CPthreadReqAttrSetschedpolicy_Pre_Initialized_No,
  CPthreadReqAttrSetschedpolicy_Pre_Initialized_NA
} CPthreadReqAttrSetschedpolicy_Pre_Initialized;

typedef enum {
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Other,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Fifo,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_RoundRobin,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Sporadic,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Invalid,
  CPthreadReqAttrSetschedpolicy_Pre_Policy_NA
} CPthreadReqAttrSetschedpolicy_Pre_Policy;

typedef enum {
  CPthreadReqAttrSetschedpolicy_Post_Status_Ok,
  CPthreadReqAttrSetschedpolicy_Post_Status_Inval,
  CPthreadReqAttrSetschedpolicy_Post_Status_NotSup,
  CPthreadReqAttrSetschedpolicy_Post_Status_NA
} CPthreadReqAttrSetschedpolicy_Post_Status;

typedef enum {
  CPthreadReqAttrSetschedpolicy_Post_Policy_Other,
  CPthreadReqAttrSetschedpolicy_Post_Policy_Fifo,
  CPthreadReqAttrSetschedpolicy_Post_Policy_RoundRobin,
  CPthreadReqAttrSetschedpolicy_Post_Policy_Sporadic,
  CPthreadReqAttrSetschedpolicy_Post_Policy_Nop,
  CPthreadReqAttrSetschedpolicy_Post_Policy_NA
} CPthreadReqAttrSetschedpolicy_Post_Policy;

typedef enum {
  CPthreadReqAttrSetschedpolicy_Post_Other_Nop,
  CPthreadReqAttrSetschedpolicy_Post_Other_NA
} CPthreadReqAttrSetschedpolicy_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_Policy_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Policy : 3;
  uint16_t Post_Other : 1;
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

static const char *const CPthreadReqAttrSetschedpolicy_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrSetschedpolicy_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqAttrSetschedpolicy_PreDesc_Policy[] =
  { "Other", "Fifo", "RoundRobin", "Sporadic", "Invalid", "NA" };

static const char *const *const CPthreadReqAttrSetschedpolicy_PreDesc[] = {
  CPthreadReqAttrSetschedpolicy_PreDesc_Attr,
  CPthreadReqAttrSetschedpolicy_PreDesc_Initialized,
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
    case CPthreadReqAttrSetschedpolicy_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedpolicy_Pre_Initialized_Prepare(
  CPthreadReqAttrSetschedpolicy_Context        *ctx,
  CPthreadReqAttrSetschedpolicy_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedpolicy_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      ctx->attr_obj.schedpolicy = -1;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.schedpolicy = -1;
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Pre_Initialized_NA:
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
    case CPthreadReqAttrSetschedpolicy_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_setschedpolicy() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_setschedpolicy() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Status_NotSup: {
      /*
       * The return value of pthread_attr_setschedpolicy() shall be ENOTSUP.
       */
      T_eq_int( ctx->status, ENOTSUP );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedpolicy_Post_Policy_Check(
  CPthreadReqAttrSetschedpolicy_Context    *ctx,
  CPthreadReqAttrSetschedpolicy_Post_Policy state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedpolicy_Post_Policy_Other: {
      /*
       * The scheduling policy of the thread attributes object referenced by
       * the `attr` parameter shall be SCHED_OTHER.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_OTHER );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Policy_Fifo: {
      /*
       * The scheduling policy of the thread attributes object referenced by
       * the `attr` parameter shall be SCHED_FIFO.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_FIFO );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Policy_RoundRobin: {
      /*
       * The scheduling policy of the thread attributes object referenced by
       * the `attr` parameter shall be SCHED_RR.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_RR );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Policy_Sporadic: {
      /*
       * The scheduling policy of the thread attributes object referenced by
       * the `attr` parameter shall be SCHED_SPORADIC.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_SPORADIC );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Policy_Nop: {
      /*
       * The scheduling policy of the thread attributes object referenced by
       * the `attr` parameter shall not be modified.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, ctx->attr_before.schedpolicy );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Policy_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedpolicy_Post_Other_Check(
  CPthreadReqAttrSetschedpolicy_Context   *ctx,
  CPthreadReqAttrSetschedpolicy_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedpolicy_Post_Other_Nop: {
      /*
       * The content of the thread attributes object referenced by the `attr`
       * parameter, other than its scheduling policy, shall not be modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.schedpolicy = ctx->attr_obj.schedpolicy;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrSetschedpolicy_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedpolicy_Action(
  CPthreadReqAttrSetschedpolicy_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_setschedpolicy( ctx->attr, ctx->policy );
}

/* clang-format off */

static const CPthreadReqAttrSetschedpolicy_Entry
CPthreadReqAttrSetschedpolicy_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_Inval,
    CPthreadReqAttrSetschedpolicy_Post_Policy_Nop,
    CPthreadReqAttrSetschedpolicy_Post_Other_Nop },
  { 0, 0, 1, 0, CPthreadReqAttrSetschedpolicy_Post_Status_Inval,
    CPthreadReqAttrSetschedpolicy_Post_Policy_NA,
    CPthreadReqAttrSetschedpolicy_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_NA,
    CPthreadReqAttrSetschedpolicy_Post_Policy_NA,
    CPthreadReqAttrSetschedpolicy_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_Ok,
    CPthreadReqAttrSetschedpolicy_Post_Policy_Other,
    CPthreadReqAttrSetschedpolicy_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_Ok,
    CPthreadReqAttrSetschedpolicy_Post_Policy_Fifo,
    CPthreadReqAttrSetschedpolicy_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_Ok,
    CPthreadReqAttrSetschedpolicy_Post_Policy_RoundRobin,
    CPthreadReqAttrSetschedpolicy_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_Ok,
    CPthreadReqAttrSetschedpolicy_Post_Policy_Sporadic,
    CPthreadReqAttrSetschedpolicy_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetschedpolicy_Post_Status_NotSup,
    CPthreadReqAttrSetschedpolicy_Post_Policy_Nop,
    CPthreadReqAttrSetschedpolicy_Post_Other_Nop }
};

static const uint8_t
CPthreadReqAttrSetschedpolicy_Map[] = {
  3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2
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
  return CPthreadReqAttrSetschedpolicy_Entries
    [ CPthreadReqAttrSetschedpolicy_Map[ index ] ];
}

static void CPthreadReqAttrSetschedpolicy_SetPreConditionStates(
  CPthreadReqAttrSetschedpolicy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetschedpolicy_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqAttrSetschedpolicy_TestVariant(
  CPthreadReqAttrSetschedpolicy_Context *ctx
)
{
  CPthreadReqAttrSetschedpolicy_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetschedpolicy_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetschedpolicy_Pre_Policy_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqAttrSetschedpolicy_Action( ctx );
  CPthreadReqAttrSetschedpolicy_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetschedpolicy_Post_Policy_Check(
    ctx,
    ctx->Map.entry.Post_Policy
  );
  CPthreadReqAttrSetschedpolicy_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
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
    ctx->Map.pci[ 0 ] = CPthreadReqAttrSetschedpolicy_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrSetschedpolicy_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrSetschedpolicy_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrSetschedpolicy_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqAttrSetschedpolicy_Pre_Policy_Other;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrSetschedpolicy_Pre_Policy_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrSetschedpolicy_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqAttrSetschedpolicy_SetPreConditionStates( ctx );
        CPthreadReqAttrSetschedpolicy_TestVariant( ctx );
      }
    }
  }
}

/** @} */
