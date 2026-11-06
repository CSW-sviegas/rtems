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
#include <sched.h>
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
  CPthreadReqAttrSetschedparam_Pre_Attr_Obj,
  CPthreadReqAttrSetschedparam_Pre_Attr_Null,
  CPthreadReqAttrSetschedparam_Pre_Attr_NA
} CPthreadReqAttrSetschedparam_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetschedparam_Pre_Initialized_Yes,
  CPthreadReqAttrSetschedparam_Pre_Initialized_No,
  CPthreadReqAttrSetschedparam_Pre_Initialized_NA
} CPthreadReqAttrSetschedparam_Pre_Initialized;

typedef enum {
  CPthreadReqAttrSetschedparam_Pre_Param_Obj,
  CPthreadReqAttrSetschedparam_Pre_Param_Null,
  CPthreadReqAttrSetschedparam_Pre_Param_NA
} CPthreadReqAttrSetschedparam_Pre_Param;

typedef enum {
  CPthreadReqAttrSetschedparam_Post_Status_Ok,
  CPthreadReqAttrSetschedparam_Post_Status_Inval,
  CPthreadReqAttrSetschedparam_Post_Status_NA
} CPthreadReqAttrSetschedparam_Post_Status;

typedef enum {
  CPthreadReqAttrSetschedparam_Post_SchedParam_Param,
  CPthreadReqAttrSetschedparam_Post_SchedParam_Nop,
  CPthreadReqAttrSetschedparam_Post_SchedParam_NA
} CPthreadReqAttrSetschedparam_Post_SchedParam;

typedef enum {
  CPthreadReqAttrSetschedparam_Post_Other_Nop,
  CPthreadReqAttrSetschedparam_Post_Other_NA
} CPthreadReqAttrSetschedparam_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_Param_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_SchedParam : 2;
  uint16_t Post_Other : 1;
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
   * @brief This member contains the scheduling parameter object.
   */
  struct sched_param param_obj;

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

static const char *const CPthreadReqAttrSetschedparam_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrSetschedparam_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqAttrSetschedparam_PreDesc_Param[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrSetschedparam_PreDesc[] = {
  CPthreadReqAttrSetschedparam_PreDesc_Attr,
  CPthreadReqAttrSetschedparam_PreDesc_Initialized,
  CPthreadReqAttrSetschedparam_PreDesc_Param,
  NULL
};

#define PARAM_PRIORITY 3

#define PARAM_LOW_PRIORITY 2

#define PARAM_REPL_PERIOD_SEC 7

#define PARAM_REPL_PERIOD_NSEC 8

#define PARAM_INIT_BUDGET_SEC 5

#define PARAM_INIT_BUDGET_NSEC 6

#define PARAM_MAX_REPL 4

static void CPthreadReqAttrSetschedparam_Pre_Attr_Prepare(
  CPthreadReqAttrSetschedparam_Context *ctx,
  CPthreadReqAttrSetschedparam_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Pre_Initialized_Prepare(
  CPthreadReqAttrSetschedparam_Context        *ctx,
  CPthreadReqAttrSetschedparam_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      memset(
        &ctx->attr_obj.schedparam,
        0xff,
        sizeof( ctx->attr_obj.schedparam )
      );
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      memset(
        &ctx->attr_obj.schedparam,
        0xff,
        sizeof( ctx->attr_obj.schedparam )
      );
      break;
    }

    case CPthreadReqAttrSetschedparam_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Pre_Param_Prepare(
  CPthreadReqAttrSetschedparam_Context  *ctx,
  CPthreadReqAttrSetschedparam_Pre_Param state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Pre_Param_Obj: {
      /*
       * While the `param` parameter references an object of type struct
       * sched_param.
       */
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
    case CPthreadReqAttrSetschedparam_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_setschedparam() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_setschedparam() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Post_SchedParam_Check(
  CPthreadReqAttrSetschedparam_Context        *ctx,
  CPthreadReqAttrSetschedparam_Post_SchedParam state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Post_SchedParam_Param: {
      /*
       * The scheduling parameter of the thread attributes object referenced by
       * the `attr` parameter shall be equal to the object referenced by the
       * `param` parameter.
       */
      const struct sched_param *param;

      param = &ctx->attr_obj.schedparam;
      T_eq_int( param->sched_priority, PARAM_PRIORITY );
      T_eq_int( param->sched_ss_low_priority, PARAM_LOW_PRIORITY );
      T_eq_i64( param->sched_ss_repl_period.tv_sec, PARAM_REPL_PERIOD_SEC );
      T_eq_long( param->sched_ss_repl_period.tv_nsec, PARAM_REPL_PERIOD_NSEC );
      T_eq_i64( param->sched_ss_init_budget.tv_sec, PARAM_INIT_BUDGET_SEC );
      T_eq_long( param->sched_ss_init_budget.tv_nsec, PARAM_INIT_BUDGET_NSEC );
      T_eq_int( param->sched_ss_max_repl, PARAM_MAX_REPL );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_SchedParam_Nop: {
      /*
       * The scheduling parameter of the thread attributes object referenced by
       * the `attr` parameter shall not be modified.
       */
      T_eq_mem(
        &ctx->attr_obj.schedparam,
        &ctx->attr_before.schedparam,
        sizeof( ctx->attr_obj.schedparam )
      );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_SchedParam_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Post_Other_Check(
  CPthreadReqAttrSetschedparam_Context   *ctx,
  CPthreadReqAttrSetschedparam_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetschedparam_Post_Other_Nop: {
      /*
       * The content of the thread attributes object referenced by the `attr`
       * parameter, other than its scheduling parameter, shall not be modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      memcpy(
        &expected.schedparam,
        &ctx->attr_obj.schedparam,
        sizeof( expected.schedparam )
      );
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrSetschedparam_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrSetschedparam_Setup(
  CPthreadReqAttrSetschedparam_Context *ctx
)
{
  memset( &ctx->param_obj, 0, sizeof( ctx->param_obj ) );
  ctx->param_obj.sched_priority = PARAM_PRIORITY;
  ctx->param_obj.sched_ss_low_priority = PARAM_LOW_PRIORITY;
  ctx->param_obj.sched_ss_repl_period.tv_sec = PARAM_REPL_PERIOD_SEC;
  ctx->param_obj.sched_ss_repl_period.tv_nsec = PARAM_REPL_PERIOD_NSEC;
  ctx->param_obj.sched_ss_init_budget.tv_sec = PARAM_INIT_BUDGET_SEC;
  ctx->param_obj.sched_ss_init_budget.tv_nsec = PARAM_INIT_BUDGET_NSEC;
  ctx->param_obj.sched_ss_max_repl = PARAM_MAX_REPL;
}

static void CPthreadReqAttrSetschedparam_Setup_Wrap( void *arg )
{
  CPthreadReqAttrSetschedparam_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqAttrSetschedparam_Setup( ctx );
}

static void CPthreadReqAttrSetschedparam_Action(
  CPthreadReqAttrSetschedparam_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_setschedparam( ctx->attr, ctx->param );
}

/* clang-format off */

static const CPthreadReqAttrSetschedparam_Entry
CPthreadReqAttrSetschedparam_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrSetschedparam_Post_Status_Inval,
    CPthreadReqAttrSetschedparam_Post_SchedParam_Nop,
    CPthreadReqAttrSetschedparam_Post_Other_Nop },
  { 0, 0, 1, 0, CPthreadReqAttrSetschedparam_Post_Status_Inval,
    CPthreadReqAttrSetschedparam_Post_SchedParam_NA,
    CPthreadReqAttrSetschedparam_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqAttrSetschedparam_Post_Status_NA,
    CPthreadReqAttrSetschedparam_Post_SchedParam_NA,
    CPthreadReqAttrSetschedparam_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqAttrSetschedparam_Post_Status_Ok,
    CPthreadReqAttrSetschedparam_Post_SchedParam_Param,
    CPthreadReqAttrSetschedparam_Post_Other_Nop }
};

static const uint8_t
CPthreadReqAttrSetschedparam_Map[] = {
  3, 0, 0, 0, 1, 1, 2, 2
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
  .setup = CPthreadReqAttrSetschedparam_Setup_Wrap,
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
  return CPthreadReqAttrSetschedparam_Entries
    [ CPthreadReqAttrSetschedparam_Map[ index ] ];
}

static void CPthreadReqAttrSetschedparam_SetPreConditionStates(
  CPthreadReqAttrSetschedparam_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetschedparam_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqAttrSetschedparam_TestVariant(
  CPthreadReqAttrSetschedparam_Context *ctx
)
{
  CPthreadReqAttrSetschedparam_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetschedparam_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetschedparam_Pre_Param_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqAttrSetschedparam_Action( ctx );
  CPthreadReqAttrSetschedparam_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetschedparam_Post_SchedParam_Check(
    ctx,
    ctx->Map.entry.Post_SchedParam
  );
  CPthreadReqAttrSetschedparam_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
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
    ctx->Map.pci[ 0 ] = CPthreadReqAttrSetschedparam_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrSetschedparam_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrSetschedparam_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrSetschedparam_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqAttrSetschedparam_Pre_Param_Obj;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrSetschedparam_Pre_Param_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrSetschedparam_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqAttrSetschedparam_SetPreConditionStates( ctx );
        CPthreadReqAttrSetschedparam_TestVariant( ctx );
      }
    }
  }
}

/** @} */
