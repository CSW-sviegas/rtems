/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetschedparam
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
 * @defgroup CPthreadReqAttrSetschedparam \
 *   spec:/c/pthread/req/attr-setschedparam
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetschedparam_Pre_Attr_Valid,
  CPthreadReqAttrSetschedparam_Pre_Attr_Null,
  CPthreadReqAttrSetschedparam_Pre_Attr_Invalid,
  CPthreadReqAttrSetschedparam_Pre_Attr_NA
} CPthreadReqAttrSetschedparam_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetschedparam_Pre_Param_Valid,
  CPthreadReqAttrSetschedparam_Pre_Param_Null,
  CPthreadReqAttrSetschedparam_Pre_Param_NA
} CPthreadReqAttrSetschedparam_Pre_Param;

typedef enum {
  CPthreadReqAttrSetschedparam_Post_Status_Zero,
  CPthreadReqAttrSetschedparam_Post_Status_EINVAL,
  CPthreadReqAttrSetschedparam_Post_Status_NA
} CPthreadReqAttrSetschedparam_Post_Status;

typedef enum {
  CPthreadReqAttrSetschedparam_Post_AttrObj_Set,
  CPthreadReqAttrSetschedparam_Post_AttrObj_Nop,
  CPthreadReqAttrSetschedparam_Post_AttrObj_NA
} CPthreadReqAttrSetschedparam_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_Param_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqAttrSetschedparam_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setschedparam test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `param` parameter value.
   */
  const struct sched_param *param;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_setschedparam() call.
   */
  int retval;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains the scheduling parameter object.
   */
  struct sched_param param_obj;

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
    CPthreadReqAttrSetschedparam_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetschedparam_Context;

static CPthreadReqAttrSetschedparam_Context
  CPthreadReqAttrSetschedparam_Instance;

static const char * const CPthreadReqAttrSetschedparam_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrSetschedparam_PreDesc_Param[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqAttrSetschedparam_PreDesc[] = {
  CPthreadReqAttrSetschedparam_PreDesc_Attr,
  CPthreadReqAttrSetschedparam_PreDesc_Param,
  NULL
};

#define PARAM_PRIORITY 3

#define PARAM_LOW_PRIORITY 4

static void CheckSchedParam(
  const struct sched_param *actual,
  const struct sched_param *expected
)
{
  T_eq_int( actual->sched_priority, expected->sched_priority );
#if defined( _POSIX_SPORADIC_SERVER ) || \
  defined( _POSIX_THREAD_SPORADIC_SERVER )
  T_eq_int(
    actual->sched_ss_low_priority,
    expected->sched_ss_low_priority
  );
  T_eq_i64(
    (int64_t) actual->sched_ss_repl_period.tv_sec,
    (int64_t) expected->sched_ss_repl_period.tv_sec
  );
  T_eq_long(
    actual->sched_ss_repl_period.tv_nsec,
    expected->sched_ss_repl_period.tv_nsec
  );
  T_eq_i64(
    (int64_t) actual->sched_ss_init_budget.tv_sec,
    (int64_t) expected->sched_ss_init_budget.tv_sec
  );
  T_eq_long(
    actual->sched_ss_init_budget.tv_nsec,
    expected->sched_ss_init_budget.tv_nsec
  );
  T_eq_int( actual->sched_ss_max_repl, expected->sched_ss_max_repl );
#endif
}

static void CPthreadReqAttrSetschedparam_Pre_Attr_Prepare(
  CPthreadReqAttrSetschedparam_Context *ctx,
  CPthreadReqAttrSetschedparam_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      memset(
        &ctx->attr_obj.schedparam,
        0xff,
        sizeof( ctx->attr_obj.schedparam )
      );
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      memset(
        &ctx->attr_obj.schedparam,
        0xff,
        sizeof( ctx->attr_obj.schedparam )
      );
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Pre_Param_Prepare(
  CPthreadReqAttrSetschedparam_Context  *ctx,
  CPthreadReqAttrSetschedparam_Pre_Param state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Pre_Param_Valid: {
      /*
       * While the `param` parameter references a scheduling parameter object.
       */
      memset( &ctx->param_obj, 0, sizeof( ctx->param_obj ) );
      ctx->param_obj.sched_priority = PARAM_PRIORITY;
      #if defined( _POSIX_SPORADIC_SERVER ) || \
        defined( _POSIX_THREAD_SPORADIC_SERVER )
      ctx->param_obj.sched_ss_low_priority = PARAM_LOW_PRIORITY;
      ctx->param_obj.sched_ss_repl_period.tv_sec = 1;
      ctx->param_obj.sched_ss_repl_period.tv_nsec = 2;
      ctx->param_obj.sched_ss_init_budget.tv_sec = 3;
      ctx->param_obj.sched_ss_init_budget.tv_nsec = 4;
      ctx->param_obj.sched_ss_max_repl = 5;
      #endif
      ctx->param = &ctx->param_obj;
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Param_Null: {
      /*
       * While the `param` parameter is NULL.
       */
      ctx->param = NULL;
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Param_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Post_Status_Check(
  CPthreadReqAttrSetschedparam_Context    *ctx,
  CPthreadReqAttrSetschedparam_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_setschedparam() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_setschedparam() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Post_AttrObj_Check(
  CPthreadReqAttrSetschedparam_Context     *ctx,
  CPthreadReqAttrSetschedparam_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Post_AttrObj_Set: {
      /*
       * The value of the scheduling parameter of the thread attributes object
       * referenced by the `attr` parameter shall be set to the parameters
       * object referenced by the `param` parameter.
       */
      CheckSchedParam( &ctx->attr_obj.schedparam, ctx->param );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_AttrObj_Nop: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * not be modified.
       */
      struct sched_param param;

      memset( &param, 0xff, sizeof( param ) );
      CheckSchedParam( &ctx->attr_obj.schedparam, &param );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Action(
  CPthreadReqAttrSetschedparam_Context *ctx
)
{
  ctx->retval = pthread_attr_setschedparam( ctx->attr, ctx->param );
}

/* clang-format off */

static const CPthreadReqAttrSetschedparam_Entry
CPthreadReqAttrSetschedparam_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrSetschedparam_Post_Status_EINVAL,
    CPthreadReqAttrSetschedparam_Post_AttrObj_Nop },
  { 0, 0, 0, CPthreadReqAttrSetschedparam_Post_Status_EINVAL,
    CPthreadReqAttrSetschedparam_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqAttrSetschedparam_Post_Status_Zero,
    CPthreadReqAttrSetschedparam_Post_AttrObj_Set }
};

static const uint8_t
CPthreadReqAttrSetschedparam_Map[] = {
  2, 0, 1, 1, 0, 0
};

/* clang-format on */

static size_t CPthreadReqAttrSetschedparam_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetschedparam_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetschedparam_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetschedparam_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetschedparam_Scope,
  .initial_context = &CPthreadReqAttrSetschedparam_Instance
};

static inline CPthreadReqAttrSetschedparam_Entry
CPthreadReqAttrSetschedparam_PopEntry(
  CPthreadReqAttrSetschedparam_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetschedparam_Entries[
    CPthreadReqAttrSetschedparam_Map[ index ]
  ];
}

static void CPthreadReqAttrSetschedparam_TestVariant(
  CPthreadReqAttrSetschedparam_Context *ctx
)
{
  CPthreadReqAttrSetschedparam_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetschedparam_Pre_Param_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrSetschedparam_Action( ctx );
  CPthreadReqAttrSetschedparam_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetschedparam_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetschedparam( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetschedparam,
  &CPthreadReqAttrSetschedparam_Fixture
)
{
  CPthreadReqAttrSetschedparam_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetschedparam_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetschedparam_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetschedparam_Pre_Param_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrSetschedparam_Pre_Param_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrSetschedparam_PopEntry( ctx );
      CPthreadReqAttrSetschedparam_TestVariant( ctx );
    }
  }
}

/** @} */
