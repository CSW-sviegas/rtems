/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrGetclock
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
 * @defgroup CPthreadReqCondattrGetclock spec:/c/pthread/req/condattr-getclock
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrGetclock_Pre_Attr_Obj,
  CPthreadReqCondattrGetclock_Pre_Attr_Null,
  CPthreadReqCondattrGetclock_Pre_Attr_NA
} CPthreadReqCondattrGetclock_Pre_Attr;

typedef enum {
  CPthreadReqCondattrGetclock_Pre_Initialized_Yes,
  CPthreadReqCondattrGetclock_Pre_Initialized_No,
  CPthreadReqCondattrGetclock_Pre_Initialized_NA
} CPthreadReqCondattrGetclock_Pre_Initialized;

typedef enum {
  CPthreadReqCondattrGetclock_Pre_StoredClock_Realtime,
  CPthreadReqCondattrGetclock_Pre_StoredClock_Monotonic,
  CPthreadReqCondattrGetclock_Pre_StoredClock_NA
} CPthreadReqCondattrGetclock_Pre_StoredClock;

typedef enum {
  CPthreadReqCondattrGetclock_Pre_Clock_Obj,
  CPthreadReqCondattrGetclock_Pre_Clock_Null,
  CPthreadReqCondattrGetclock_Pre_Clock_NA
} CPthreadReqCondattrGetclock_Pre_Clock;

typedef enum {
  CPthreadReqCondattrGetclock_Post_Status_Ok,
  CPthreadReqCondattrGetclock_Post_Status_Inval,
  CPthreadReqCondattrGetclock_Post_Status_NA
} CPthreadReqCondattrGetclock_Post_Status;

typedef enum {
  CPthreadReqCondattrGetclock_Post_Clock_Realtime,
  CPthreadReqCondattrGetclock_Post_Clock_Monotonic,
  CPthreadReqCondattrGetclock_Post_Clock_Nop,
  CPthreadReqCondattrGetclock_Post_Clock_NA
} CPthreadReqCondattrGetclock_Post_Clock;

typedef enum {
  CPthreadReqCondattrGetclock_Post_Attr_Nop,
  CPthreadReqCondattrGetclock_Post_Attr_NA
} CPthreadReqCondattrGetclock_Post_Attr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StoredClock_NA : 1;
  uint16_t Pre_Clock_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Clock : 2;
  uint16_t Post_Attr : 1;
} CPthreadReqCondattrGetclock_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/condattr-getclock test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_condattr_t *attr;

  /**
   * @brief This member specifies the `clock_id` parameter value.
   */
  clockid_t *clock;

  /**
   * @brief This member contains the return value of the
   *   pthread_condattr_getclock() call.
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

  /**
   * @brief This member contains the object referenced by the `clock_id`
   *   parameter.
   */
  clockid_t clock_obj;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    CPthreadReqCondattrGetclock_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondattrGetclock_Context;

static CPthreadReqCondattrGetclock_Context
  CPthreadReqCondattrGetclock_Instance;

static const char *const CPthreadReqCondattrGetclock_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondattrGetclock_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCondattrGetclock_PreDesc_StoredClock[] =
  { "Realtime", "Monotonic", "NA" };

static const char *const CPthreadReqCondattrGetclock_PreDesc_Clock[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqCondattrGetclock_PreDesc[] = {
  CPthreadReqCondattrGetclock_PreDesc_Attr,
  CPthreadReqCondattrGetclock_PreDesc_Initialized,
  CPthreadReqCondattrGetclock_PreDesc_StoredClock,
  CPthreadReqCondattrGetclock_PreDesc_Clock,
  NULL
};

#define CLOCK_POISON ( (clockid_t) 123 )

static void CPthreadReqCondattrGetclock_Pre_Attr_Prepare(
  CPthreadReqCondattrGetclock_Context *ctx,
  CPthreadReqCondattrGetclock_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_condattr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Pre_Initialized_Prepare(
  CPthreadReqCondattrGetclock_Context        *ctx,
  CPthreadReqCondattrGetclock_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * condition variable attributes object.
       */
      T_eno_success( pthread_condattr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized condition variable attributes object.
       */
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = 0;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Pre_StoredClock_Prepare(
  CPthreadReqCondattrGetclock_Context        *ctx,
  CPthreadReqCondattrGetclock_Pre_StoredClock state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Pre_StoredClock_Realtime: {
      /*
       * While the condition variable clock attribute of the object referenced
       * by the `attr` parameter is CLOCK_REALTIME.
       */
      ctx->attr_obj.clock = CLOCK_REALTIME;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_StoredClock_Monotonic: {
      /*
       * While the condition variable clock attribute of the object referenced
       * by the `attr` parameter is CLOCK_MONOTONIC.
       */
      ctx->attr_obj.clock = CLOCK_MONOTONIC;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_StoredClock_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Pre_Clock_Prepare(
  CPthreadReqCondattrGetclock_Context  *ctx,
  CPthreadReqCondattrGetclock_Pre_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Pre_Clock_Obj: {
      /*
       * While the `clock_id` parameter references an object of type clockid_t.
       */
      ctx->clock_obj = CLOCK_POISON;
      ctx->clock = &ctx->clock_obj;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_Clock_Null: {
      /*
       * While the `clock_id` parameter is NULL.
       */
      ctx->clock = NULL;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_Clock_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Post_Status_Check(
  CPthreadReqCondattrGetclock_Context    *ctx,
  CPthreadReqCondattrGetclock_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Post_Status_Ok: {
      /*
       * The return value of pthread_condattr_getclock() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_Status_Inval: {
      /*
       * The return value of pthread_condattr_getclock() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Post_Clock_Check(
  CPthreadReqCondattrGetclock_Context   *ctx,
  CPthreadReqCondattrGetclock_Post_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Post_Clock_Realtime: {
      /*
       * The object referenced by the `clock_id` parameter shall be
       * CLOCK_REALTIME.
       */
      T_eq_ulong( ctx->clock_obj, CLOCK_REALTIME );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_Clock_Monotonic: {
      /*
       * The object referenced by the `clock_id` parameter shall be
       * CLOCK_MONOTONIC.
       */
      T_eq_ulong( ctx->clock_obj, CLOCK_MONOTONIC );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_Clock_Nop: {
      /*
       * The object referenced by the `clock_id` parameter shall not be
       * modified.
       */
      T_eq_ulong( ctx->clock_obj, CLOCK_POISON );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_Clock_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Post_Attr_Check(
  CPthreadReqCondattrGetclock_Context  *ctx,
  CPthreadReqCondattrGetclock_Post_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Post_Attr_Nop: {
      /*
       * The object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_mem( &ctx->attr_obj, &ctx->attr_before, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Action(
  CPthreadReqCondattrGetclock_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_condattr_getclock( ctx->attr, ctx->clock );
}

/* clang-format off */

static const CPthreadReqCondattrGetclock_Entry
CPthreadReqCondattrGetclock_Entries[] = {
  { 1, 0, 0, 0, 0, CPthreadReqCondattrGetclock_Post_Status_NA,
    CPthreadReqCondattrGetclock_Post_Clock_NA,
    CPthreadReqCondattrGetclock_Post_Attr_NA },
  { 0, 0, 0, 0, 0, CPthreadReqCondattrGetclock_Post_Status_Inval,
    CPthreadReqCondattrGetclock_Post_Clock_NA,
    CPthreadReqCondattrGetclock_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqCondattrGetclock_Post_Status_Ok,
    CPthreadReqCondattrGetclock_Post_Clock_Realtime,
    CPthreadReqCondattrGetclock_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqCondattrGetclock_Post_Status_Ok,
    CPthreadReqCondattrGetclock_Post_Clock_Monotonic,
    CPthreadReqCondattrGetclock_Post_Attr_Nop },
  { 0, 0, 1, 1, 0, CPthreadReqCondattrGetclock_Post_Status_Inval,
    CPthreadReqCondattrGetclock_Post_Clock_Nop,
    CPthreadReqCondattrGetclock_Post_Attr_NA },
  { 0, 0, 1, 1, 0, CPthreadReqCondattrGetclock_Post_Status_Inval,
    CPthreadReqCondattrGetclock_Post_Clock_NA,
    CPthreadReqCondattrGetclock_Post_Attr_NA }
};

static const uint8_t
CPthreadReqCondattrGetclock_Map[] = {
  2, 1, 3, 1, 2, 1, 3, 1, 4, 5, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondattrGetclock_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqCondattrGetclock_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondattrGetclock_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondattrGetclock_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondattrGetclock_Scope,
  .initial_context = &CPthreadReqCondattrGetclock_Instance
};

static inline CPthreadReqCondattrGetclock_Entry
CPthreadReqCondattrGetclock_PopEntry(
  CPthreadReqCondattrGetclock_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondattrGetclock_Entries
    [ CPthreadReqCondattrGetclock_Map[ index ] ];
}

static void CPthreadReqCondattrGetclock_SetPreConditionStates(
  CPthreadReqCondattrGetclock_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondattrGetclock_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredClock_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondattrGetclock_Pre_StoredClock_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void CPthreadReqCondattrGetclock_TestVariant(
  CPthreadReqCondattrGetclock_Context *ctx
)
{
  CPthreadReqCondattrGetclock_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrGetclock_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqCondattrGetclock_Pre_StoredClock_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqCondattrGetclock_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqCondattrGetclock_Action( ctx );
  CPthreadReqCondattrGetclock_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrGetclock_Post_Clock_Check(
    ctx,
    ctx->Map.entry.Post_Clock
  );
  CPthreadReqCondattrGetclock_Post_Attr_Check( ctx, ctx->Map.entry.Post_Attr );
}

/**
 * @fn void T_case_body_CPthreadReqCondattrGetclock( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondattrGetclock,
  &CPthreadReqCondattrGetclock_Fixture
)
{
  CPthreadReqCondattrGetclock_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondattrGetclock_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondattrGetclock_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondattrGetclock_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCondattrGetclock_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqCondattrGetclock_Pre_StoredClock_Realtime;
        ctx->Map.pci[ 2 ] < CPthreadReqCondattrGetclock_Pre_StoredClock_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqCondattrGetclock_Pre_Clock_Obj;
          ctx->Map.pci[ 3 ] < CPthreadReqCondattrGetclock_Pre_Clock_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqCondattrGetclock_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CPthreadReqCondattrGetclock_SetPreConditionStates( ctx );
          CPthreadReqCondattrGetclock_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
