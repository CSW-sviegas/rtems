/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetinheritsched
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

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqAttrSetinheritsched \
 *   spec:/c/pthread/req/attr-setinheritsched
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetinheritsched_Pre_Attr_Valid,
  CPthreadReqAttrSetinheritsched_Pre_Attr_Null,
  CPthreadReqAttrSetinheritsched_Pre_Attr_Invalid,
  CPthreadReqAttrSetinheritsched_Pre_Attr_NA
} CPthreadReqAttrSetinheritsched_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Inherit,
  CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Explicit,
  CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Invalid,
  CPthreadReqAttrSetinheritsched_Pre_Inheritsched_NA
} CPthreadReqAttrSetinheritsched_Pre_Inheritsched;

typedef enum {
  CPthreadReqAttrSetinheritsched_Post_Status_Zero,
  CPthreadReqAttrSetinheritsched_Post_Status_EINVAL,
  CPthreadReqAttrSetinheritsched_Post_Status_ENOTSUP,
  CPthreadReqAttrSetinheritsched_Post_Status_NA
} CPthreadReqAttrSetinheritsched_Post_Status;

typedef enum {
  CPthreadReqAttrSetinheritsched_Post_AttrObj_Set,
  CPthreadReqAttrSetinheritsched_Post_AttrObj_Nop,
  CPthreadReqAttrSetinheritsched_Post_AttrObj_NA
} CPthreadReqAttrSetinheritsched_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_Inheritsched_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqAttrSetinheritsched_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setinheritsched test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `inheritsched` parameter value.
   */
  int inheritsched;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_setinheritsched() call.
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
    CPthreadReqAttrSetinheritsched_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetinheritsched_Context;

static CPthreadReqAttrSetinheritsched_Context
  CPthreadReqAttrSetinheritsched_Instance;

static const char * const CPthreadReqAttrSetinheritsched_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrSetinheritsched_PreDesc_Inheritsched[] = {
  "Inherit",
  "Explicit",
  "Invalid",
  "NA"
};

static const char * const * const CPthreadReqAttrSetinheritsched_PreDesc[] = {
  CPthreadReqAttrSetinheritsched_PreDesc_Attr,
  CPthreadReqAttrSetinheritsched_PreDesc_Inheritsched,
  NULL
};

#define INHERITSCHED_INVALID 123

static void CPthreadReqAttrSetinheritsched_Pre_Attr_Prepare(
  CPthreadReqAttrSetinheritsched_Context *ctx,
  CPthreadReqAttrSetinheritsched_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      ctx->attr_obj.inheritsched = -1;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.inheritsched = -1;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Prepare(
  CPthreadReqAttrSetinheritsched_Context         *ctx,
  CPthreadReqAttrSetinheritsched_Pre_Inheritsched state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Inherit: {
      /*
       * While the `inheritsched` parameter is equal to PTHREAD_INHERIT_SCHED.
       */
      ctx->inheritsched = PTHREAD_INHERIT_SCHED;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Explicit: {
      /*
       * While the `inheritsched` parameter is equal to PTHREAD_EXPLICIT_SCHED.
       */
      ctx->inheritsched = PTHREAD_EXPLICIT_SCHED;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Invalid: {
      /*
       * While the `inheritsched` parameter is not a valid thread inherit
       * scheduler attribute.
       */
      ctx->inheritsched = INHERITSCHED_INVALID;
      break;
    }

    case CPthreadReqAttrSetinheritsched_Pre_Inheritsched_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Post_Status_Check(
  CPthreadReqAttrSetinheritsched_Context    *ctx,
  CPthreadReqAttrSetinheritsched_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_setinheritsched() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_setinheritsched() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_Status_ENOTSUP: {
      /*
       * The return value of pthread_attr_setinheritsched() shall be equal to
       * ENOTSUP.
       */
      T_eq_int( ctx->retval, ENOTSUP );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Post_AttrObj_Check(
  CPthreadReqAttrSetinheritsched_Context     *ctx,
  CPthreadReqAttrSetinheritsched_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetinheritsched_Post_AttrObj_Set: {
      /*
       * The value of the thread inherit scheduler attribute of the thread
       * attributes object referenced by the `attr` parameter shall be set to
       * the indicator specified by the `inheritsched` parameter.
       */
      int inheritsched;
      int eno;

      eno = pthread_attr_getinheritsched( &ctx->attr_obj, &inheritsched );
      T_eno_success( eno );
      T_eq_int( inheritsched, ctx->inheritsched );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_AttrObj_Nop: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * not be modified.
       */
      T_eq_int( ctx->attr_obj.inheritsched, -1 );
      break;
    }

    case CPthreadReqAttrSetinheritsched_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetinheritsched_Action(
  CPthreadReqAttrSetinheritsched_Context *ctx
)
{
  ctx->retval = pthread_attr_setinheritsched( ctx->attr, ctx->inheritsched );
}

static const CPthreadReqAttrSetinheritsched_Entry
CPthreadReqAttrSetinheritsched_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_EINVAL,
    CPthreadReqAttrSetinheritsched_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_EINVAL,
    CPthreadReqAttrSetinheritsched_Post_AttrObj_Nop },
  { 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_Zero,
    CPthreadReqAttrSetinheritsched_Post_AttrObj_Set },
  { 0, 0, 0, CPthreadReqAttrSetinheritsched_Post_Status_ENOTSUP,
    CPthreadReqAttrSetinheritsched_Post_AttrObj_Nop }
};

static const uint8_t
CPthreadReqAttrSetinheritsched_Map[] = {
  2, 2, 3, 0, 0, 0, 1, 1, 1
};

static size_t CPthreadReqAttrSetinheritsched_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetinheritsched_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetinheritsched_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetinheritsched_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetinheritsched_Scope,
  .initial_context = &CPthreadReqAttrSetinheritsched_Instance
};

static inline CPthreadReqAttrSetinheritsched_Entry
CPthreadReqAttrSetinheritsched_PopEntry(
  CPthreadReqAttrSetinheritsched_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetinheritsched_Entries[
    CPthreadReqAttrSetinheritsched_Map[ index ]
  ];
}

static void CPthreadReqAttrSetinheritsched_TestVariant(
  CPthreadReqAttrSetinheritsched_Context *ctx
)
{
  CPthreadReqAttrSetinheritsched_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetinheritsched_Action( ctx );
  CPthreadReqAttrSetinheritsched_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetinheritsched_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetinheritsched( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetinheritsched,
  &CPthreadReqAttrSetinheritsched_Fixture
)
{
  CPthreadReqAttrSetinheritsched_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetinheritsched_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetinheritsched_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetinheritsched_Pre_Inheritsched_Inherit;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrSetinheritsched_Pre_Inheritsched_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrSetinheritsched_PopEntry( ctx );
      CPthreadReqAttrSetinheritsched_TestVariant( ctx );
    }
  }
}

/** @} */
