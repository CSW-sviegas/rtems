/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetguardsize
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
 * @defgroup CPthreadReqAttrSetguardsize spec:/c/pthread/req/attr-setguardsize
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetguardsize_Pre_Attr_Valid,
  CPthreadReqAttrSetguardsize_Pre_Attr_Null,
  CPthreadReqAttrSetguardsize_Pre_Attr_Invalid,
  CPthreadReqAttrSetguardsize_Pre_Attr_NA
} CPthreadReqAttrSetguardsize_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetguardsize_Pre_GuardSize_Valid,
  CPthreadReqAttrSetguardsize_Pre_GuardSize_NA
} CPthreadReqAttrSetguardsize_Pre_GuardSize;

typedef enum {
  CPthreadReqAttrSetguardsize_Post_Status_Zero,
  CPthreadReqAttrSetguardsize_Post_Status_EINVAL,
  CPthreadReqAttrSetguardsize_Post_Status_NA
} CPthreadReqAttrSetguardsize_Post_Status;

typedef enum {
  CPthreadReqAttrSetguardsize_Post_AttrObj_Set,
  CPthreadReqAttrSetguardsize_Post_AttrObj_Nop,
  CPthreadReqAttrSetguardsize_Post_AttrObj_NA
} CPthreadReqAttrSetguardsize_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_GuardSize_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqAttrSetguardsize_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setguardsize test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `guardsize` parameter value.
   */
  size_t guardsize;

  /**
   * @brief This member contains the return value.
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
    CPthreadReqAttrSetguardsize_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetguardsize_Context;

static CPthreadReqAttrSetguardsize_Context
  CPthreadReqAttrSetguardsize_Instance;

static const char * const CPthreadReqAttrSetguardsize_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrSetguardsize_PreDesc_GuardSize[] = {
  "Valid",
  "NA"
};

static const char * const * const CPthreadReqAttrSetguardsize_PreDesc[] = {
  CPthreadReqAttrSetguardsize_PreDesc_Attr,
  CPthreadReqAttrSetguardsize_PreDesc_GuardSize,
  NULL
};

#define GUARD_SIZE_VALUE ( (size_t) 4096 )

#define STALE_GUARD_SIZE ( (size_t) -1 )

static void CPthreadReqAttrSetguardsize_Pre_Attr_Prepare(
  CPthreadReqAttrSetguardsize_Context *ctx,
  CPthreadReqAttrSetguardsize_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      ctx->attr_obj.guardsize = STALE_GUARD_SIZE;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.guardsize = STALE_GUARD_SIZE;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Pre_GuardSize_Prepare(
  CPthreadReqAttrSetguardsize_Context      *ctx,
  CPthreadReqAttrSetguardsize_Pre_GuardSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Pre_GuardSize_Valid: {
      /*
       * While the `guardsize` parameter specifies a guard size.
       */
      ctx->guardsize = GUARD_SIZE_VALUE;
      break;
    }

    case CPthreadReqAttrSetguardsize_Pre_GuardSize_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Post_Status_Check(
  CPthreadReqAttrSetguardsize_Context    *ctx,
  CPthreadReqAttrSetguardsize_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_setguardsize() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_setguardsize() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Post_AttrObj_Check(
  CPthreadReqAttrSetguardsize_Context     *ctx,
  CPthreadReqAttrSetguardsize_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetguardsize_Post_AttrObj_Set: {
      /*
       * The value of the thread guard size attribute of the thread attributes
       * object referenced by the `attr` parameter shall be set to the size
       * specified by the `guardsize` parameter.
       */
      size_t guardsize;
      int    eno;

      eno = pthread_attr_getguardsize( &ctx->attr_obj, &guardsize );
      T_eno_success( eno );
      T_eq_sz( guardsize, GUARD_SIZE_VALUE );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_AttrObj_Nop: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * not be modified.
       */
      T_eq_sz( ctx->attr_obj.guardsize, STALE_GUARD_SIZE );
      break;
    }

    case CPthreadReqAttrSetguardsize_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetguardsize_Action(
  CPthreadReqAttrSetguardsize_Context *ctx
)
{
  ctx->retval = pthread_attr_setguardsize( ctx->attr, ctx->guardsize );
}

static const CPthreadReqAttrSetguardsize_Entry
CPthreadReqAttrSetguardsize_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrSetguardsize_Post_Status_Zero,
    CPthreadReqAttrSetguardsize_Post_AttrObj_Set },
  { 0, 0, 0, CPthreadReqAttrSetguardsize_Post_Status_EINVAL,
    CPthreadReqAttrSetguardsize_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqAttrSetguardsize_Post_Status_EINVAL,
    CPthreadReqAttrSetguardsize_Post_AttrObj_Nop }
};

static const uint8_t
CPthreadReqAttrSetguardsize_Map[] = {
  0, 1, 2
};

static size_t CPthreadReqAttrSetguardsize_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetguardsize_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetguardsize_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetguardsize_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetguardsize_Scope,
  .initial_context = &CPthreadReqAttrSetguardsize_Instance
};

static inline CPthreadReqAttrSetguardsize_Entry
CPthreadReqAttrSetguardsize_PopEntry(
  CPthreadReqAttrSetguardsize_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetguardsize_Entries[
    CPthreadReqAttrSetguardsize_Map[ index ]
  ];
}

static void CPthreadReqAttrSetguardsize_TestVariant(
  CPthreadReqAttrSetguardsize_Context *ctx
)
{
  CPthreadReqAttrSetguardsize_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetguardsize_Pre_GuardSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrSetguardsize_Action( ctx );
  CPthreadReqAttrSetguardsize_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetguardsize_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetguardsize( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetguardsize,
  &CPthreadReqAttrSetguardsize_Fixture
)
{
  CPthreadReqAttrSetguardsize_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetguardsize_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetguardsize_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetguardsize_Pre_GuardSize_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrSetguardsize_Pre_GuardSize_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrSetguardsize_PopEntry( ctx );
      CPthreadReqAttrSetguardsize_TestVariant( ctx );
    }
  }
}

/** @} */
