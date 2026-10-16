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
#include <time.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondattrGetclock spec:/c/pthread/req/condattr-getclock
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrGetclock_Pre_Attr_Valid,
  CPthreadReqCondattrGetclock_Pre_Attr_Null,
  CPthreadReqCondattrGetclock_Pre_Attr_NA
} CPthreadReqCondattrGetclock_Pre_Attr;

typedef enum {
  CPthreadReqCondattrGetclock_Pre_ClockId_Valid,
  CPthreadReqCondattrGetclock_Pre_ClockId_Null,
  CPthreadReqCondattrGetclock_Pre_ClockId_NA
} CPthreadReqCondattrGetclock_Pre_ClockId;

typedef enum {
  CPthreadReqCondattrGetclock_Post_Status_Zero,
  CPthreadReqCondattrGetclock_Post_Status_EINVAL,
  CPthreadReqCondattrGetclock_Post_Status_NA
} CPthreadReqCondattrGetclock_Post_Status;

typedef enum {
  CPthreadReqCondattrGetclock_Post_ClockObj_Set,
  CPthreadReqCondattrGetclock_Post_ClockObj_Nop,
  CPthreadReqCondattrGetclock_Post_ClockObj_NA
} CPthreadReqCondattrGetclock_Post_ClockObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_ClockId_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_ClockObj : 2;
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
  clockid_t *clock_id;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the attribute object.
   */
  pthread_condattr_t attr_obj;

  /**
   * @brief This member contains the clock value.
   */
  clockid_t clock_obj;

  /**
   * @brief This member contains the clock output object.
   */
  clockid_t clock_value;

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

static const char * const CPthreadReqCondattrGetclock_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CPthreadReqCondattrGetclock_PreDesc_ClockId[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqCondattrGetclock_PreDesc[] = {
  CPthreadReqCondattrGetclock_PreDesc_Attr,
  CPthreadReqCondattrGetclock_PreDesc_ClockId,
  NULL
};

static void CPthreadReqCondattrGetclock_Pre_Attr_Prepare(
  CPthreadReqCondattrGetclock_Context *ctx,
  CPthreadReqCondattrGetclock_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Pre_Attr_Valid: {
      /*
       * While the condition variable attributes object referenced by `attr` is
       * valid.
       */
      ctx->attr = &ctx->attr_obj;
      T_eq_int( pthread_condattr_init( &ctx->attr_obj ), 0 );
      T_eq_int( pthread_condattr_setclock( &ctx->attr_obj, ctx->clock_obj ), 0 );
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

static void CPthreadReqCondattrGetclock_Pre_ClockId_Prepare(
  CPthreadReqCondattrGetclock_Context    *ctx,
  CPthreadReqCondattrGetclock_Pre_ClockId state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Pre_ClockId_Valid: {
      /*
       * While the `clock_id` parameter references an object of type clockid_t.
       */
      ctx->clock_value = -1;
      ctx->clock_id = &ctx->clock_value;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_ClockId_Null: {
      /*
       * While the `clock_id` parameter is NULL.
       */
      ctx->clock_id = NULL;
      break;
    }

    case CPthreadReqCondattrGetclock_Pre_ClockId_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Post_Status_Check(
  CPthreadReqCondattrGetclock_Context    *ctx,
  CPthreadReqCondattrGetclock_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Post_Status_Zero: {
      /*
       * The return value of pthread_condattr_getclock() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_Status_EINVAL: {
      /*
       * The return value of pthread_condattr_getclock() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Post_ClockObj_Check(
  CPthreadReqCondattrGetclock_Context      *ctx,
  CPthreadReqCondattrGetclock_Post_ClockObj state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetclock_Post_ClockObj_Set: {
      /*
       * The value of the object referenced by the `clock_id` parameter shall
       * be set to the condition variable clock attribute of the condition
       * variable attributes object referenced by the `attr` parameter.
       */
      T_eq_int( ctx->clock_value, ctx->clock_obj );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_ClockObj_Nop: {
      /*
       * The value of the object referenced by the `clock_id` parameter shall
       * not be modified.
       */
      T_eq_int( ctx->clock_value, -1 );
      break;
    }

    case CPthreadReqCondattrGetclock_Post_ClockObj_NA:
      break;
  }
}

static void CPthreadReqCondattrGetclock_Prepare(
  CPthreadReqCondattrGetclock_Context *ctx
)
{
  ctx->clock_obj = CLOCK_MONOTONIC;
}

static void CPthreadReqCondattrGetclock_Action(
  CPthreadReqCondattrGetclock_Context *ctx
)
{
  ctx->retval = pthread_condattr_getclock( ctx->attr, ctx->clock_id );
}

static const CPthreadReqCondattrGetclock_Entry
CPthreadReqCondattrGetclock_Entries[] = {
  { 0, 0, 0, CPthreadReqCondattrGetclock_Post_Status_EINVAL,
    CPthreadReqCondattrGetclock_Post_ClockObj_NA },
  { 0, 0, 0, CPthreadReqCondattrGetclock_Post_Status_Zero,
    CPthreadReqCondattrGetclock_Post_ClockObj_Set },
  { 0, 0, 0, CPthreadReqCondattrGetclock_Post_Status_EINVAL,
    CPthreadReqCondattrGetclock_Post_ClockObj_Nop }
};

static const uint8_t
CPthreadReqCondattrGetclock_Map[] = {
  1, 0, 2, 0
};

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
  return CPthreadReqCondattrGetclock_Entries[
    CPthreadReqCondattrGetclock_Map[ index ]
  ];
}

static void CPthreadReqCondattrGetclock_TestVariant(
  CPthreadReqCondattrGetclock_Context *ctx
)
{
  CPthreadReqCondattrGetclock_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrGetclock_Pre_ClockId_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondattrGetclock_Action( ctx );
  CPthreadReqCondattrGetclock_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrGetclock_Post_ClockObj_Check(
    ctx,
    ctx->Map.entry.Post_ClockObj
  );
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
    ctx->Map.pcs[ 0 ] = CPthreadReqCondattrGetclock_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqCondattrGetclock_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqCondattrGetclock_Pre_ClockId_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqCondattrGetclock_Pre_ClockId_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCondattrGetclock_PopEntry( ctx );
      CPthreadReqCondattrGetclock_Prepare( ctx );
      CPthreadReqCondattrGetclock_TestVariant( ctx );
    }
  }
}

/** @} */
