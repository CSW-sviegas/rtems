/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrSetclock
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
#include <time.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondattrSetclock spec:/c/pthread/req/condattr-setclock
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrSetclock_Pre_Attr_Valid,
  CPthreadReqCondattrSetclock_Pre_Attr_Null,
  CPthreadReqCondattrSetclock_Pre_Attr_NA
} CPthreadReqCondattrSetclock_Pre_Attr;

typedef enum {
  CPthreadReqCondattrSetclock_Pre_Clock_Realtime,
  CPthreadReqCondattrSetclock_Pre_Clock_Monotonic,
  CPthreadReqCondattrSetclock_Pre_Clock_Invalid,
  CPthreadReqCondattrSetclock_Pre_Clock_NA
} CPthreadReqCondattrSetclock_Pre_Clock;

typedef enum {
  CPthreadReqCondattrSetclock_Post_Status_Zero,
  CPthreadReqCondattrSetclock_Post_Status_EINVAL,
  CPthreadReqCondattrSetclock_Post_Status_NA
} CPthreadReqCondattrSetclock_Post_Status;

typedef enum {
  CPthreadReqCondattrSetclock_Post_AttrObj_Set,
  CPthreadReqCondattrSetclock_Post_AttrObj_Nop,
  CPthreadReqCondattrSetclock_Post_AttrObj_NA
} CPthreadReqCondattrSetclock_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_Clock_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqCondattrSetclock_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/condattr-setclock test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_condattr_t *attr;

  /**
   * @brief This member specifies the `clock_id` parameter value.
   */
  clockid_t clock_id;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the condition variable attributes object.
   */
  pthread_condattr_t attr_obj;

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
    CPthreadReqCondattrSetclock_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondattrSetclock_Context;

static CPthreadReqCondattrSetclock_Context
  CPthreadReqCondattrSetclock_Instance;

static const char *const CPthreadReqCondattrSetclock_PreDesc_Attr[] =
  { "Valid", "Null", "NA" };

static const char *const CPthreadReqCondattrSetclock_PreDesc_Clock[] =
  { "Realtime", "Monotonic", "Invalid", "NA" };

static const char *const *const CPthreadReqCondattrSetclock_PreDesc[] = {
  CPthreadReqCondattrSetclock_PreDesc_Attr,
  CPthreadReqCondattrSetclock_PreDesc_Clock,
  NULL
};

static void CPthreadReqCondattrSetclock_Pre_Attr_Prepare(
  CPthreadReqCondattrSetclock_Context *ctx,
  CPthreadReqCondattrSetclock_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetclock_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized condition
       * variable attributes object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_condattr_init( &ctx->attr_obj );
      ctx->attr_obj.clock = -1;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrSetclock_Pre_Clock_Prepare(
  CPthreadReqCondattrSetclock_Context  *ctx,
  CPthreadReqCondattrSetclock_Pre_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetclock_Pre_Clock_Realtime: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_REALTIME.
       */
      ctx->clock_id = CLOCK_REALTIME;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Clock_Monotonic: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_MONOTONIC.
       */
      ctx->clock_id = CLOCK_MONOTONIC;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Clock_Invalid: {
      /*
       * While the `clock_id` parameter is neither CLOCK_REALTIME nor
       * CLOCK_MONOTONIC.
       */
      ctx->clock_id = CLOCK_PROCESS_CPUTIME_ID;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Clock_NA:
      break;
  }
}

static void CPthreadReqCondattrSetclock_Post_Status_Check(
  CPthreadReqCondattrSetclock_Context    *ctx,
  CPthreadReqCondattrSetclock_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetclock_Post_Status_Zero: {
      /*
       * The return value of pthread_condattr_setclock() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_Status_EINVAL: {
      /*
       * The return value of pthread_condattr_setclock() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrSetclock_Post_AttrObj_Check(
  CPthreadReqCondattrSetclock_Context     *ctx,
  CPthreadReqCondattrSetclock_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetclock_Post_AttrObj_Set: {
      /*
       * The condition variable clock attribute of the condition variable
       * attributes object referenced by the `attr` parameter shall be set to
       * the clock specified by the `clock_id` parameter.
       */
      clockid_t clock_id;
      int       eno;

      eno = pthread_condattr_getclock( &ctx->attr_obj, &clock_id );
      T_eno_success( eno );
      T_eq_int( clock_id, ctx->clock_id );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_AttrObj_Nop: {
      /*
       * The condition variable attributes object referenced by the `attr`
       * parameter shall not be modified.
       */
      T_eq_int( ctx->attr_obj.clock, -1 );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqCondattrSetclock_Action(
  CPthreadReqCondattrSetclock_Context *ctx
)
{
  ctx->retval = pthread_condattr_setclock( ctx->attr, ctx->clock_id );
}

/* clang-format off */

static const CPthreadReqCondattrSetclock_Entry
CPthreadReqCondattrSetclock_Entries[] = {
  { 0, 0, 0, CPthreadReqCondattrSetclock_Post_Status_EINVAL,
    CPthreadReqCondattrSetclock_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqCondattrSetclock_Post_Status_Zero,
    CPthreadReqCondattrSetclock_Post_AttrObj_Set },
  { 0, 0, 0, CPthreadReqCondattrSetclock_Post_Status_EINVAL,
    CPthreadReqCondattrSetclock_Post_AttrObj_Nop }
};

static const uint8_t
CPthreadReqCondattrSetclock_Map[] = {
  1, 1, 2, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondattrSetclock_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqCondattrSetclock_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondattrSetclock_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondattrSetclock_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondattrSetclock_Scope,
  .initial_context = &CPthreadReqCondattrSetclock_Instance
};

static inline CPthreadReqCondattrSetclock_Entry
CPthreadReqCondattrSetclock_PopEntry(
  CPthreadReqCondattrSetclock_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondattrSetclock_Entries
    [ CPthreadReqCondattrSetclock_Map[ index ] ];
}

static void CPthreadReqCondattrSetclock_TestVariant(
  CPthreadReqCondattrSetclock_Context *ctx
)
{
  CPthreadReqCondattrSetclock_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrSetclock_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondattrSetclock_Action( ctx );
  CPthreadReqCondattrSetclock_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrSetclock_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqCondattrSetclock( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondattrSetclock,
  &CPthreadReqCondattrSetclock_Fixture
)
{
  CPthreadReqCondattrSetclock_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqCondattrSetclock_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqCondattrSetclock_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqCondattrSetclock_Pre_Clock_Realtime;
      ctx->Map.pcs[ 1 ] < CPthreadReqCondattrSetclock_Pre_Clock_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCondattrSetclock_PopEntry( ctx );
      CPthreadReqCondattrSetclock_TestVariant( ctx );
    }
  }
}

/** @} */
