/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrInit
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
 * @defgroup CPthreadReqAttrInit spec:/c/pthread/req/attr-init
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrInit_Pre_Attr_Obj,
  CPthreadReqAttrInit_Pre_Attr_Null,
  CPthreadReqAttrInit_Pre_Attr_NA
} CPthreadReqAttrInit_Pre_Attr;

typedef enum {
  CPthreadReqAttrInit_Post_Status_Ok,
  CPthreadReqAttrInit_Post_Status_Inval,
  CPthreadReqAttrInit_Post_Status_NA
} CPthreadReqAttrInit_Post_Status;

typedef enum {
  CPthreadReqAttrInit_Post_Initialized_Yes,
  CPthreadReqAttrInit_Post_Initialized_NA
} CPthreadReqAttrInit_Post_Initialized;

typedef enum {
  CPthreadReqAttrInit_Post_StackAddr_Null,
  CPthreadReqAttrInit_Post_StackAddr_NA
} CPthreadReqAttrInit_Post_StackAddr;

typedef enum {
  CPthreadReqAttrInit_Post_StackSize_Zero,
  CPthreadReqAttrInit_Post_StackSize_NA
} CPthreadReqAttrInit_Post_StackSize;

typedef enum {
  CPthreadReqAttrInit_Post_ContentionScope_Process,
  CPthreadReqAttrInit_Post_ContentionScope_NA
} CPthreadReqAttrInit_Post_ContentionScope;

typedef enum {
  CPthreadReqAttrInit_Post_InheritSched_Inherit,
  CPthreadReqAttrInit_Post_InheritSched_NA
} CPthreadReqAttrInit_Post_InheritSched;

typedef enum {
  CPthreadReqAttrInit_Post_SchedPolicy_Fifo,
  CPthreadReqAttrInit_Post_SchedPolicy_NA
} CPthreadReqAttrInit_Post_SchedPolicy;

typedef enum {
  CPthreadReqAttrInit_Post_SchedParam_Default,
  CPthreadReqAttrInit_Post_SchedParam_NA
} CPthreadReqAttrInit_Post_SchedParam;

typedef enum {
  CPthreadReqAttrInit_Post_GuardSize_Zero,
  CPthreadReqAttrInit_Post_GuardSize_NA
} CPthreadReqAttrInit_Post_GuardSize;

typedef enum {
  CPthreadReqAttrInit_Post_DetachState_Joinable,
  CPthreadReqAttrInit_Post_DetachState_NA
} CPthreadReqAttrInit_Post_DetachState;

typedef enum {
  CPthreadReqAttrInit_Post_Affinity_All,
  CPthreadReqAttrInit_Post_Affinity_NA
} CPthreadReqAttrInit_Post_Affinity;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Initialized : 1;
  uint16_t Post_StackAddr : 1;
  uint16_t Post_StackSize : 1;
  uint16_t Post_ContentionScope : 1;
  uint16_t Post_InheritSched : 1;
  uint16_t Post_SchedPolicy : 1;
  uint16_t Post_SchedParam : 1;
  uint16_t Post_GuardSize : 1;
  uint16_t Post_DetachState : 1;
  uint16_t Post_Affinity : 1;
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
   * @brief This member contains the return value of the pthread_attr_init()
   *   call.
   */
  int status;

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

static CPthreadReqAttrInit_Context CPthreadReqAttrInit_Instance;

static const char *const CPthreadReqAttrInit_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrInit_PreDesc[] = {
  CPthreadReqAttrInit_PreDesc_Attr,
  NULL
};

static void CPthreadReqAttrInit_Pre_Attr_Prepare(
  CPthreadReqAttrInit_Context *ctx,
  CPthreadReqAttrInit_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
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
    case CPthreadReqAttrInit_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_init() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrInit_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_init() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrInit_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_Initialized_Check(
  CPthreadReqAttrInit_Context         *ctx,
  CPthreadReqAttrInit_Post_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_Initialized_Yes: {
      /*
       * The object referenced by the `attr` parameter shall be an initialized
       * thread attributes object.
       */
      T_true( ctx->attr_obj.is_initialized );
      break;
    }

    case CPthreadReqAttrInit_Post_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_StackAddr_Check(
  CPthreadReqAttrInit_Context       *ctx,
  CPthreadReqAttrInit_Post_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_StackAddr_Null: {
      /*
       * The thread stack address attribute of the thread attributes object
       * referenced by the `attr` parameter shall be NULL.
       */
      T_null( ctx->attr_obj.stackaddr );
      break;
    }

    case CPthreadReqAttrInit_Post_StackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_StackSize_Check(
  CPthreadReqAttrInit_Context       *ctx,
  CPthreadReqAttrInit_Post_StackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_StackSize_Zero: {
      /*
       * The thread stack size attribute of the thread attributes object
       * referenced by the `attr` parameter shall be zero.
       */
      T_eq_sz( ctx->attr_obj.stacksize, 0 );
      break;
    }

    case CPthreadReqAttrInit_Post_StackSize_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_ContentionScope_Check(
  CPthreadReqAttrInit_Context             *ctx,
  CPthreadReqAttrInit_Post_ContentionScope state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_ContentionScope_Process: {
      /*
       * The thread contention scope attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_SCOPE_PROCESS.
       */
      T_eq_int( ctx->attr_obj.contentionscope, PTHREAD_SCOPE_PROCESS );
      break;
    }

    case CPthreadReqAttrInit_Post_ContentionScope_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_InheritSched_Check(
  CPthreadReqAttrInit_Context          *ctx,
  CPthreadReqAttrInit_Post_InheritSched state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_InheritSched_Inherit: {
      /*
       * The thread inherit scheduler attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_INHERIT_SCHED.
       */
      T_eq_int( ctx->attr_obj.inheritsched, PTHREAD_INHERIT_SCHED );
      break;
    }

    case CPthreadReqAttrInit_Post_InheritSched_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_SchedPolicy_Check(
  CPthreadReqAttrInit_Context         *ctx,
  CPthreadReqAttrInit_Post_SchedPolicy state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_SchedPolicy_Fifo: {
      /*
       * The scheduling policy of the thread attributes object referenced by
       * the `attr` parameter shall be SCHED_FIFO.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_FIFO );
      break;
    }

    case CPthreadReqAttrInit_Post_SchedPolicy_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_SchedParam_Check(
  CPthreadReqAttrInit_Context        *ctx,
  CPthreadReqAttrInit_Post_SchedParam state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_SchedParam_Default: {
      /*
       * The scheduling parameters of the thread attributes object referenced
       * by the `attr` parameter shall specify a priority of two and zero for
       * each other parameter.
       */
      T_eq_int( ctx->attr_obj.schedparam.sched_priority, 2 );
      T_eq_int( ctx->attr_obj.schedparam.sched_ss_low_priority, 0 );
      T_eq_i64( ctx->attr_obj.schedparam.sched_ss_repl_period.tv_sec, 0 );
      T_eq_long( ctx->attr_obj.schedparam.sched_ss_repl_period.tv_nsec, 0 );
      T_eq_i64( ctx->attr_obj.schedparam.sched_ss_init_budget.tv_sec, 0 );
      T_eq_long( ctx->attr_obj.schedparam.sched_ss_init_budget.tv_nsec, 0 );
      T_eq_int( ctx->attr_obj.schedparam.sched_ss_max_repl, 0 );
      break;
    }

    case CPthreadReqAttrInit_Post_SchedParam_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_GuardSize_Check(
  CPthreadReqAttrInit_Context       *ctx,
  CPthreadReqAttrInit_Post_GuardSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_GuardSize_Zero: {
      /*
       * The thread guard size attribute of the thread attributes object
       * referenced by the `attr` parameter shall be zero.
       */
      T_eq_sz( ctx->attr_obj.guardsize, 0 );
      break;
    }

    case CPthreadReqAttrInit_Post_GuardSize_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_DetachState_Check(
  CPthreadReqAttrInit_Context         *ctx,
  CPthreadReqAttrInit_Post_DetachState state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_DetachState_Joinable: {
      /*
       * The thread detach state attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_CREATE_JOINABLE.
       */
      T_eq_int( ctx->attr_obj.detachstate, PTHREAD_CREATE_JOINABLE );
      break;
    }

    case CPthreadReqAttrInit_Post_DetachState_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Post_Affinity_Check(
  CPthreadReqAttrInit_Context      *ctx,
  CPthreadReqAttrInit_Post_Affinity state
)
{
  switch ( state ) {
    case CPthreadReqAttrInit_Post_Affinity_All: {
      /*
       * The processor affinity set of the thread attributes object referenced
       * by the `attr` parameter shall be a cpu_set_t object which contains
       * every processor.
       */
      T_eq_ptr(
        ctx->attr_obj.affinityset,
        &ctx->attr_obj.affinitysetpreallocated
      );
      T_eq_sz(
        ctx->attr_obj.affinitysetsize,
        sizeof( ctx->attr_obj.affinitysetpreallocated )
      );
      T_true( CPU_ISFULLSET( ctx->attr_obj.affinityset ) );
      break;
    }

    case CPthreadReqAttrInit_Post_Affinity_NA:
      break;
  }
}

static void CPthreadReqAttrInit_Action( CPthreadReqAttrInit_Context *ctx )
{
  ctx->status = pthread_attr_init( ctx->attr );
}

/* clang-format off */

static const CPthreadReqAttrInit_Entry
CPthreadReqAttrInit_Entries[] = {
  { 0, 0, CPthreadReqAttrInit_Post_Status_Ok,
    CPthreadReqAttrInit_Post_Initialized_Yes,
    CPthreadReqAttrInit_Post_StackAddr_Null,
    CPthreadReqAttrInit_Post_StackSize_Zero,
    CPthreadReqAttrInit_Post_ContentionScope_Process,
    CPthreadReqAttrInit_Post_InheritSched_Inherit,
    CPthreadReqAttrInit_Post_SchedPolicy_Fifo,
    CPthreadReqAttrInit_Post_SchedParam_Default,
    CPthreadReqAttrInit_Post_GuardSize_Zero,
    CPthreadReqAttrInit_Post_DetachState_Joinable,
    CPthreadReqAttrInit_Post_Affinity_All },
  { 0, 0, CPthreadReqAttrInit_Post_Status_Inval,
    CPthreadReqAttrInit_Post_Initialized_NA,
    CPthreadReqAttrInit_Post_StackAddr_NA,
    CPthreadReqAttrInit_Post_StackSize_NA,
    CPthreadReqAttrInit_Post_ContentionScope_NA,
    CPthreadReqAttrInit_Post_InheritSched_NA,
    CPthreadReqAttrInit_Post_SchedPolicy_NA,
    CPthreadReqAttrInit_Post_SchedParam_NA,
    CPthreadReqAttrInit_Post_GuardSize_NA,
    CPthreadReqAttrInit_Post_DetachState_NA,
    CPthreadReqAttrInit_Post_Affinity_NA }
};

static const uint8_t
CPthreadReqAttrInit_Map[] = {
  0, 1
};

/* clang-format on */

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
  return CPthreadReqAttrInit_Entries[ CPthreadReqAttrInit_Map[ index ] ];
}

static void CPthreadReqAttrInit_TestVariant( CPthreadReqAttrInit_Context *ctx )
{
  CPthreadReqAttrInit_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrInit_Action( ctx );
  CPthreadReqAttrInit_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrInit_Post_Initialized_Check(
    ctx,
    ctx->Map.entry.Post_Initialized
  );
  CPthreadReqAttrInit_Post_StackAddr_Check(
    ctx,
    ctx->Map.entry.Post_StackAddr
  );
  CPthreadReqAttrInit_Post_StackSize_Check(
    ctx,
    ctx->Map.entry.Post_StackSize
  );
  CPthreadReqAttrInit_Post_ContentionScope_Check(
    ctx,
    ctx->Map.entry.Post_ContentionScope
  );
  CPthreadReqAttrInit_Post_InheritSched_Check(
    ctx,
    ctx->Map.entry.Post_InheritSched
  );
  CPthreadReqAttrInit_Post_SchedPolicy_Check(
    ctx,
    ctx->Map.entry.Post_SchedPolicy
  );
  CPthreadReqAttrInit_Post_SchedParam_Check(
    ctx,
    ctx->Map.entry.Post_SchedParam
  );
  CPthreadReqAttrInit_Post_GuardSize_Check(
    ctx,
    ctx->Map.entry.Post_GuardSize
  );
  CPthreadReqAttrInit_Post_DetachState_Check(
    ctx,
    ctx->Map.entry.Post_DetachState
  );
  CPthreadReqAttrInit_Post_Affinity_Check( ctx, ctx->Map.entry.Post_Affinity );
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
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrInit_Pre_Attr_Obj;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrInit_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqAttrInit_PopEntry( ctx );
    CPthreadReqAttrInit_TestVariant( ctx );
  }
}

/** @} */
