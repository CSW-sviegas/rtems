/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrSetclock
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <string.h>
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
  CPthreadReqCondattrSetclock_Pre_Attr_Obj,
  CPthreadReqCondattrSetclock_Pre_Attr_Null,
  CPthreadReqCondattrSetclock_Pre_Attr_NA
} CPthreadReqCondattrSetclock_Pre_Attr;

typedef enum {
  CPthreadReqCondattrSetclock_Pre_Initialized_Yes,
  CPthreadReqCondattrSetclock_Pre_Initialized_No,
  CPthreadReqCondattrSetclock_Pre_Initialized_NA
} CPthreadReqCondattrSetclock_Pre_Initialized;

typedef enum {
  CPthreadReqCondattrSetclock_Pre_Clock_Realtime,
  CPthreadReqCondattrSetclock_Pre_Clock_Monotonic,
  CPthreadReqCondattrSetclock_Pre_Clock_ProcessCpuTime,
  CPthreadReqCondattrSetclock_Pre_Clock_ThreadCpuTime,
  CPthreadReqCondattrSetclock_Pre_Clock_Invalid,
  CPthreadReqCondattrSetclock_Pre_Clock_NA
} CPthreadReqCondattrSetclock_Pre_Clock;

typedef enum {
  CPthreadReqCondattrSetclock_Post_Status_Ok,
  CPthreadReqCondattrSetclock_Post_Status_Inval,
  CPthreadReqCondattrSetclock_Post_Status_NA
} CPthreadReqCondattrSetclock_Post_Status;

typedef enum {
  CPthreadReqCondattrSetclock_Post_Clock_Realtime,
  CPthreadReqCondattrSetclock_Post_Clock_Monotonic,
  CPthreadReqCondattrSetclock_Post_Clock_Nop,
  CPthreadReqCondattrSetclock_Post_Clock_NA
} CPthreadReqCondattrSetclock_Post_Clock;

typedef enum {
  CPthreadReqCondattrSetclock_Post_Other_Nop,
  CPthreadReqCondattrSetclock_Post_Other_NA
} CPthreadReqCondattrSetclock_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_Clock_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Clock : 2;
  uint16_t Post_Other : 1;
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
  clockid_t clock;

  /**
   * @brief This member contains the return value of the
   *   pthread_condattr_setclock() call.
   */
  int status;

  /**
   * @brief This member contains the condition variable attributes object.
   */
  pthread_condattr_t attr_obj;

  /**
   * @brief This member contains a copy of the condition variable attributes
   *   object before the call.
   */
  pthread_condattr_t attr_before;

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
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondattrSetclock_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCondattrSetclock_PreDesc_Clock[] = {
  "Realtime",
  "Monotonic",
  "ProcessCpuTime",
  "ThreadCpuTime",
  "Invalid",
  "NA"
};

static const char *const *const CPthreadReqCondattrSetclock_PreDesc[] = {
  CPthreadReqCondattrSetclock_PreDesc_Attr,
  CPthreadReqCondattrSetclock_PreDesc_Initialized,
  CPthreadReqCondattrSetclock_PreDesc_Clock,
  NULL
};

#define CLOCK_POISON 123

#define CLOCK_INVALID ( (clockid_t) 0x7fffffff )

static void CPthreadReqCondattrSetclock_Pre_Attr_Prepare(
  CPthreadReqCondattrSetclock_Context *ctx,
  CPthreadReqCondattrSetclock_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetclock_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_condattr_t.
       */
      ctx->attr = &ctx->attr_obj;
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

static void CPthreadReqCondattrSetclock_Pre_Initialized_Prepare(
  CPthreadReqCondattrSetclock_Context        *ctx,
  CPthreadReqCondattrSetclock_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetclock_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * condition variable attributes object.
       */
      T_eno_success( pthread_condattr_init( &ctx->attr_obj ) );
      ctx->attr_obj.clock = CLOCK_POISON;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized condition variable attributes object.
       */
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = 0;
      ctx->attr_obj.clock = CLOCK_POISON;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Initialized_NA:
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
      ctx->clock = CLOCK_REALTIME;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Clock_Monotonic: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_MONOTONIC.
       */
      ctx->clock = CLOCK_MONOTONIC;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Clock_ProcessCpuTime: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_PROCESS_CPUTIME_ID.
       */
      ctx->clock = CLOCK_PROCESS_CPUTIME_ID;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Clock_ThreadCpuTime: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_THREAD_CPUTIME_ID.
       */
      ctx->clock = CLOCK_THREAD_CPUTIME_ID;
      break;
    }

    case CPthreadReqCondattrSetclock_Pre_Clock_Invalid: {
      /*
       * While the `clock_id` parameter is neither CLOCK_REALTIME, nor
       * CLOCK_MONOTONIC, nor CLOCK_PROCESS_CPUTIME_ID, nor
       * CLOCK_THREAD_CPUTIME_ID.
       */
      ctx->clock = CLOCK_INVALID;
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
    case CPthreadReqCondattrSetclock_Post_Status_Ok: {
      /*
       * The return value of pthread_condattr_setclock() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_Status_Inval: {
      /*
       * The return value of pthread_condattr_setclock() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrSetclock_Post_Clock_Check(
  CPthreadReqCondattrSetclock_Context   *ctx,
  CPthreadReqCondattrSetclock_Post_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetclock_Post_Clock_Realtime: {
      /*
       * The condition variable clock attribute of the condition variable
       * attributes object referenced by the `attr` parameter shall be
       * CLOCK_REALTIME.
       */
      T_eq_ulong( ctx->attr_obj.clock, CLOCK_REALTIME );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_Clock_Monotonic: {
      /*
       * The condition variable clock attribute of the condition variable
       * attributes object referenced by the `attr` parameter shall be
       * CLOCK_MONOTONIC.
       */
      T_eq_ulong( ctx->attr_obj.clock, CLOCK_MONOTONIC );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_Clock_Nop: {
      /*
       * The condition variable clock attribute of the condition variable
       * attributes object referenced by the `attr` parameter shall not be
       * modified.
       */
      T_eq_ulong( ctx->attr_obj.clock, ctx->attr_before.clock );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_Clock_NA:
      break;
  }
}

static void CPthreadReqCondattrSetclock_Post_Other_Check(
  CPthreadReqCondattrSetclock_Context   *ctx,
  CPthreadReqCondattrSetclock_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqCondattrSetclock_Post_Other_Nop: {
      /*
       * The content of the condition variable attributes object referenced by
       * the `attr` parameter, other than its condition variable clock
       * attribute, shall not be modified.
       */
      pthread_condattr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.clock = ctx->attr_obj.clock;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqCondattrSetclock_Post_Other_NA:
      break;
  }
}

static void CPthreadReqCondattrSetclock_Action(
  CPthreadReqCondattrSetclock_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_condattr_setclock( ctx->attr, ctx->clock );
}

/* clang-format off */

static const CPthreadReqCondattrSetclock_Entry
CPthreadReqCondattrSetclock_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqCondattrSetclock_Post_Status_Inval,
    CPthreadReqCondattrSetclock_Post_Clock_Nop,
    CPthreadReqCondattrSetclock_Post_Other_Nop },
  { 0, 0, 1, 0, CPthreadReqCondattrSetclock_Post_Status_Inval,
    CPthreadReqCondattrSetclock_Post_Clock_NA,
    CPthreadReqCondattrSetclock_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqCondattrSetclock_Post_Status_NA,
    CPthreadReqCondattrSetclock_Post_Clock_NA,
    CPthreadReqCondattrSetclock_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqCondattrSetclock_Post_Status_Ok,
    CPthreadReqCondattrSetclock_Post_Clock_Realtime,
    CPthreadReqCondattrSetclock_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqCondattrSetclock_Post_Status_Ok,
    CPthreadReqCondattrSetclock_Post_Clock_Monotonic,
    CPthreadReqCondattrSetclock_Post_Other_Nop }
};

static const uint8_t
CPthreadReqCondattrSetclock_Map[] = {
  3, 4, 0, 0, 0, 3, 4, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2
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

static void CPthreadReqCondattrSetclock_SetPreConditionStates(
  CPthreadReqCondattrSetclock_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondattrSetclock_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqCondattrSetclock_TestVariant(
  CPthreadReqCondattrSetclock_Context *ctx
)
{
  CPthreadReqCondattrSetclock_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrSetclock_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqCondattrSetclock_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondattrSetclock_Action( ctx );
  CPthreadReqCondattrSetclock_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrSetclock_Post_Clock_Check(
    ctx,
    ctx->Map.entry.Post_Clock
  );
  CPthreadReqCondattrSetclock_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
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
    ctx->Map.pci[ 0 ] = CPthreadReqCondattrSetclock_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondattrSetclock_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondattrSetclock_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCondattrSetclock_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondattrSetclock_Pre_Clock_Realtime;
        ctx->Map.pci[ 2 ] < CPthreadReqCondattrSetclock_Pre_Clock_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqCondattrSetclock_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqCondattrSetclock_SetPreConditionStates( ctx );
        CPthreadReqCondattrSetclock_TestVariant( ctx );
      }
    }
  }
}

/** @} */
