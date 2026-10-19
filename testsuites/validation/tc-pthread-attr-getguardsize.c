/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetguardsize
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
 * @defgroup CPthreadReqAttrGetguardsize spec:/c/pthread/req/attr-getguardsize
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetguardsize_Pre_Attr_Valid,
  CPthreadReqAttrGetguardsize_Pre_Attr_Null,
  CPthreadReqAttrGetguardsize_Pre_Attr_Invalid,
  CPthreadReqAttrGetguardsize_Pre_Attr_NA
} CPthreadReqAttrGetguardsize_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetguardsize_Pre_GuardSize_Valid,
  CPthreadReqAttrGetguardsize_Pre_GuardSize_Null,
  CPthreadReqAttrGetguardsize_Pre_GuardSize_NA
} CPthreadReqAttrGetguardsize_Pre_GuardSize;

typedef enum {
  CPthreadReqAttrGetguardsize_Post_Status_Zero,
  CPthreadReqAttrGetguardsize_Post_Status_EINVAL,
  CPthreadReqAttrGetguardsize_Post_Status_NA
} CPthreadReqAttrGetguardsize_Post_Status;

typedef enum {
  CPthreadReqAttrGetguardsize_Post_GuardSizeObj_Set,
  CPthreadReqAttrGetguardsize_Post_GuardSizeObj_Nop,
  CPthreadReqAttrGetguardsize_Post_GuardSizeObj_NA
} CPthreadReqAttrGetguardsize_Post_GuardSizeObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_GuardSize_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_GuardSizeObj : 2;
} CPthreadReqAttrGetguardsize_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-getguardsize test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_attr_t *attr;

  /**
   * @brief This member specifies the `guardsize` parameter value.
   */
  size_t *guardsize;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains the guard size output object.
   */
  size_t guardsize_value;

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
    CPthreadReqAttrGetguardsize_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetguardsize_Context;

static CPthreadReqAttrGetguardsize_Context
  CPthreadReqAttrGetguardsize_Instance;

static const char * const CPthreadReqAttrGetguardsize_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrGetguardsize_PreDesc_GuardSize[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqAttrGetguardsize_PreDesc[] = {
  CPthreadReqAttrGetguardsize_PreDesc_Attr,
  CPthreadReqAttrGetguardsize_PreDesc_GuardSize,
  NULL
};

#define GUARD_SIZE_VALUE ( (size_t) 4096 )

#define STALE_GUARD_SIZE ( (size_t) -1 )

static void CPthreadReqAttrGetguardsize_Pre_Attr_Prepare(
  CPthreadReqAttrGetguardsize_Context *ctx,
  CPthreadReqAttrGetguardsize_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      pthread_attr_setguardsize( &ctx->attr_obj, GUARD_SIZE_VALUE );
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Pre_GuardSize_Prepare(
  CPthreadReqAttrGetguardsize_Context      *ctx,
  CPthreadReqAttrGetguardsize_Pre_GuardSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Pre_GuardSize_Valid: {
      /*
       * While the `guardsize` parameter references an object of type size_t.
       */
      ctx->guardsize_value = STALE_GUARD_SIZE;
      ctx->guardsize = &ctx->guardsize_value;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_GuardSize_Null: {
      /*
       * While the `guardsize` parameter is NULL.
       */
      ctx->guardsize = NULL;
      break;
    }

    case CPthreadReqAttrGetguardsize_Pre_GuardSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Post_Status_Check(
  CPthreadReqAttrGetguardsize_Context    *ctx,
  CPthreadReqAttrGetguardsize_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_getguardsize() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_getguardsize() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Post_GuardSizeObj_Check(
  CPthreadReqAttrGetguardsize_Context          *ctx,
  CPthreadReqAttrGetguardsize_Post_GuardSizeObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetguardsize_Post_GuardSizeObj_Set: {
      /*
       * The value of the object referenced by the `guardsize` parameter shall
       * be set to the thread guard size attribute of the thread attributes
       * object referenced by the `attr` parameter.
       */
      T_eq_sz( ctx->guardsize_value, GUARD_SIZE_VALUE );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_GuardSizeObj_Nop: {
      /*
       * The object referenced by the `guardsize` parameter shall not be
       * modified.
       */
      T_eq_sz( ctx->guardsize_value, STALE_GUARD_SIZE );
      break;
    }

    case CPthreadReqAttrGetguardsize_Post_GuardSizeObj_NA:
      break;
  }
}

static void CPthreadReqAttrGetguardsize_Action(
  CPthreadReqAttrGetguardsize_Context *ctx
)
{
  ctx->retval = pthread_attr_getguardsize( ctx->attr, ctx->guardsize );
}

static const CPthreadReqAttrGetguardsize_Entry
CPthreadReqAttrGetguardsize_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrGetguardsize_Post_Status_EINVAL,
    CPthreadReqAttrGetguardsize_Post_GuardSizeObj_NA },
  { 0, 0, 0, CPthreadReqAttrGetguardsize_Post_Status_EINVAL,
    CPthreadReqAttrGetguardsize_Post_GuardSizeObj_Nop },
  { 0, 0, 0, CPthreadReqAttrGetguardsize_Post_Status_Zero,
    CPthreadReqAttrGetguardsize_Post_GuardSizeObj_Set }
};

static const uint8_t
CPthreadReqAttrGetguardsize_Map[] = {
  2, 0, 1, 0, 1, 0
};

static size_t CPthreadReqAttrGetguardsize_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrGetguardsize_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrGetguardsize_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrGetguardsize_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrGetguardsize_Scope,
  .initial_context = &CPthreadReqAttrGetguardsize_Instance
};

static inline CPthreadReqAttrGetguardsize_Entry
CPthreadReqAttrGetguardsize_PopEntry(
  CPthreadReqAttrGetguardsize_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrGetguardsize_Entries[
    CPthreadReqAttrGetguardsize_Map[ index ]
  ];
}

static void CPthreadReqAttrGetguardsize_TestVariant(
  CPthreadReqAttrGetguardsize_Context *ctx
)
{
  CPthreadReqAttrGetguardsize_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetguardsize_Pre_GuardSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrGetguardsize_Action( ctx );
  CPthreadReqAttrGetguardsize_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrGetguardsize_Post_GuardSizeObj_Check(
    ctx,
    ctx->Map.entry.Post_GuardSizeObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrGetguardsize( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrGetguardsize,
  &CPthreadReqAttrGetguardsize_Fixture
)
{
  CPthreadReqAttrGetguardsize_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrGetguardsize_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrGetguardsize_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetguardsize_Pre_GuardSize_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrGetguardsize_Pre_GuardSize_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrGetguardsize_PopEntry( ctx );
      CPthreadReqAttrGetguardsize_TestVariant( ctx );
    }
  }
}

/** @} */
