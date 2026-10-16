/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondInit
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
#include <string.h>
#include <time.h>
#include <rtems/posix/condimpl.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondInit spec:/c/pthread/req/cond-init
 *
 * @{
 */

typedef enum {
  CPthreadReqCondInit_Pre_Cond_Valid,
  CPthreadReqCondInit_Pre_Cond_Null,
  CPthreadReqCondInit_Pre_Cond_NA
} CPthreadReqCondInit_Pre_Cond;

typedef enum {
  CPthreadReqCondInit_Pre_Attr_Default,
  CPthreadReqCondInit_Pre_Attr_Valid,
  CPthreadReqCondInit_Pre_Attr_Monotonic,
  CPthreadReqCondInit_Pre_Attr_Uninitialized,
  CPthreadReqCondInit_Pre_Attr_InvalidPshared,
  CPthreadReqCondInit_Pre_Attr_NA
} CPthreadReqCondInit_Pre_Attr;

typedef enum {
  CPthreadReqCondInit_Post_Status_Zero,
  CPthreadReqCondInit_Post_Status_EINVAL,
  CPthreadReqCondInit_Post_Status_NA
} CPthreadReqCondInit_Post_Status;

typedef enum {
  CPthreadReqCondInit_Post_Clock_Realtime,
  CPthreadReqCondInit_Post_Clock_Monotonic,
  CPthreadReqCondInit_Post_Clock_NA
} CPthreadReqCondInit_Post_Clock;

typedef enum {
  CPthreadReqCondInit_Post_CondObj_Initialized,
  CPthreadReqCondInit_Post_CondObj_Nop,
  CPthreadReqCondInit_Post_CondObj_NA
} CPthreadReqCondInit_Post_CondObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Cond_NA : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Clock : 2;
  uint16_t Post_CondObj : 2;
} CPthreadReqCondInit_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/cond-init test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `cond` parameter value.
   */
  pthread_cond_t *cond;

  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_condattr_t *attr;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the condition variable object.
   */
  pthread_cond_t cond_obj;

  /**
   * @brief This member contains the reference copy of the condition variable
   *   object.
   */
  pthread_cond_t cond_ref;

  /**
   * @brief This member contains the attributes object.
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
    CPthreadReqCondInit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondInit_Context;

static CPthreadReqCondInit_Context
  CPthreadReqCondInit_Instance;

static const char * const CPthreadReqCondInit_PreDesc_Cond[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CPthreadReqCondInit_PreDesc_Attr[] = {
  "Default",
  "Valid",
  "Monotonic",
  "Uninitialized",
  "InvalidPshared",
  "NA"
};

static const char * const * const CPthreadReqCondInit_PreDesc[] = {
  CPthreadReqCondInit_PreDesc_Cond,
  CPthreadReqCondInit_PreDesc_Attr,
  NULL
};

static void CPthreadReqCondInit_Pre_Cond_Prepare(
  CPthreadReqCondInit_Context *ctx,
  CPthreadReqCondInit_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Pre_Cond_Valid: {
      /*
       * While the `cond` parameter references an object of type
       * pthread_cond_t.
       */
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondInit_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondInit_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondInit_Pre_Attr_Prepare(
  CPthreadReqCondInit_Context *ctx,
  CPthreadReqCondInit_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Pre_Attr_Default: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondInit_Pre_Attr_Valid: {
      /*
       * While the condition variable attributes object referenced by `attr` is
       * initialized.
       */
      T_eq_int( pthread_condattr_init( &ctx->attr_obj ), 0 );
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqCondInit_Pre_Attr_Monotonic: {
      /*
       * While the condition variable clock attribute of the condition variable
       * attributes object referenced by `attr` is CLOCK_MONOTONIC.
       */
      T_eq_int( pthread_condattr_init( &ctx->attr_obj ), 0 );
      T_eq_int( pthread_condattr_setclock( &ctx->attr_obj, CLOCK_MONOTONIC ), 0 );
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqCondInit_Pre_Attr_Uninitialized: {
      /*
       * While the condition variable attributes object referenced by `attr` is
       * not initialized.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqCondInit_Pre_Attr_InvalidPshared: {
      /*
       * While the process-shared attribute of the condition variable
       * attributes object referenced by `attr` is invalid.
       */
      T_eq_int( pthread_condattr_init( &ctx->attr_obj ), 0 );
      ctx->attr_obj.process_shared = -1;
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqCondInit_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondInit_Post_Status_Check(
  CPthreadReqCondInit_Context    *ctx,
  CPthreadReqCondInit_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Post_Status_Zero: {
      /*
       * The return value of pthread_cond_init() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondInit_Post_Status_EINVAL: {
      /*
       * The return value of pthread_cond_init() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondInit_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondInit_Post_Clock_Check(
  CPthreadReqCondInit_Context   *ctx,
  CPthreadReqCondInit_Post_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Post_Clock_Realtime: {
      /*
       * The condition variable clock attribute of the condition variable
       * object referenced by `cond` shall be CLOCK_REALTIME.
       */
      T_eq_int(
        _POSIX_Condition_variables_Get_clock( ctx->cond_obj._flags ),
        CLOCK_REALTIME
      );
      break;
    }

    case CPthreadReqCondInit_Post_Clock_Monotonic: {
      /*
       * The condition variable clock attribute of the condition variable
       * object referenced by `cond` shall be CLOCK_MONOTONIC.
       */
      T_eq_int(
        _POSIX_Condition_variables_Get_clock( ctx->cond_obj._flags ),
        CLOCK_MONOTONIC
      );
      break;
    }

    case CPthreadReqCondInit_Post_Clock_NA:
      break;
  }
}

static void CPthreadReqCondInit_Post_CondObj_Check(
  CPthreadReqCondInit_Context     *ctx,
  CPthreadReqCondInit_Post_CondObj state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Post_CondObj_Initialized: {
      /*
       * The condition variable object referenced by `cond` shall be
       * initialized.
       */
      T_eq_int( pthread_cond_destroy( &ctx->cond_obj ), 0 );
      break;
    }

    case CPthreadReqCondInit_Post_CondObj_Nop: {
      /*
       * The condition variable object referenced by `cond` shall not be
       * modified.
       */
      T_eq_int(
        memcmp( &ctx->cond_obj, &ctx->cond_ref, sizeof( ctx->cond_obj ) ),
        0
      );
      break;
    }

    case CPthreadReqCondInit_Post_CondObj_NA:
      break;
  }
}

static void CPthreadReqCondInit_Prepare( CPthreadReqCondInit_Context *ctx )
{
  memset( &ctx->cond_obj, 0xa5, sizeof( ctx->cond_obj ) );
  memcpy( &ctx->cond_ref, &ctx->cond_obj, sizeof( ctx->cond_ref ) );
}

static void CPthreadReqCondInit_Action( CPthreadReqCondInit_Context *ctx )
{
  ctx->retval = pthread_cond_init( ctx->cond, ctx->attr );
}

static const CPthreadReqCondInit_Entry
CPthreadReqCondInit_Entries[] = {
  { 0, 0, 0, CPthreadReqCondInit_Post_Status_EINVAL,
    CPthreadReqCondInit_Post_Clock_NA, CPthreadReqCondInit_Post_CondObj_NA },
  { 0, 0, 0, CPthreadReqCondInit_Post_Status_Zero,
    CPthreadReqCondInit_Post_Clock_Realtime,
    CPthreadReqCondInit_Post_CondObj_Initialized },
  { 0, 0, 0, CPthreadReqCondInit_Post_Status_EINVAL,
    CPthreadReqCondInit_Post_Clock_NA, CPthreadReqCondInit_Post_CondObj_Nop },
  { 0, 0, 0, CPthreadReqCondInit_Post_Status_Zero,
    CPthreadReqCondInit_Post_Clock_Monotonic,
    CPthreadReqCondInit_Post_CondObj_Initialized }
};

static const uint8_t
CPthreadReqCondInit_Map[] = {
  1, 1, 3, 2, 2, 0, 0, 0, 0, 0
};

static size_t CPthreadReqCondInit_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondInit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqCondInit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqCondInit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondInit_Scope,
  .initial_context = &CPthreadReqCondInit_Instance
};

static inline CPthreadReqCondInit_Entry CPthreadReqCondInit_PopEntry(
  CPthreadReqCondInit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondInit_Entries[
    CPthreadReqCondInit_Map[ index ]
  ];
}

static void CPthreadReqCondInit_TestVariant( CPthreadReqCondInit_Context *ctx )
{
  CPthreadReqCondInit_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondInit_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondInit_Action( ctx );
  CPthreadReqCondInit_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondInit_Post_Clock_Check( ctx, ctx->Map.entry.Post_Clock );
  CPthreadReqCondInit_Post_CondObj_Check( ctx, ctx->Map.entry.Post_CondObj );
}

/**
 * @fn void T_case_body_CPthreadReqCondInit( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqCondInit, &CPthreadReqCondInit_Fixture )
{
  CPthreadReqCondInit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqCondInit_Pre_Cond_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqCondInit_Pre_Cond_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqCondInit_Pre_Attr_Default;
      ctx->Map.pcs[ 1 ] < CPthreadReqCondInit_Pre_Attr_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqCondInit_PopEntry( ctx );
      CPthreadReqCondInit_Prepare( ctx );
      CPthreadReqCondInit_TestVariant( ctx );
    }
  }
}

/** @} */
