/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrDestroy
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
 * @defgroup CPthreadReqAttrDestroy spec:/c/pthread/req/attr-destroy
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrDestroy_Pre_Attr_Valid,
  CPthreadReqAttrDestroy_Pre_Attr_Null,
  CPthreadReqAttrDestroy_Pre_Attr_Invalid,
  CPthreadReqAttrDestroy_Pre_Attr_NA
} CPthreadReqAttrDestroy_Pre_Attr;

typedef enum {
  CPthreadReqAttrDestroy_Post_Status_Zero,
  CPthreadReqAttrDestroy_Post_Status_EINVAL,
  CPthreadReqAttrDestroy_Post_Status_NA
} CPthreadReqAttrDestroy_Post_Status;

typedef enum {
  CPthreadReqAttrDestroy_Post_AttrObj_NotInitialized,
  CPthreadReqAttrDestroy_Post_AttrObj_Nop,
  CPthreadReqAttrDestroy_Post_AttrObj_NA
} CPthreadReqAttrDestroy_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqAttrDestroy_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-destroy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

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
    CPthreadReqAttrDestroy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrDestroy_Context;

static CPthreadReqAttrDestroy_Context
  CPthreadReqAttrDestroy_Instance;

static const char * const CPthreadReqAttrDestroy_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const * const CPthreadReqAttrDestroy_PreDesc[] = {
  CPthreadReqAttrDestroy_PreDesc_Attr,
  NULL
};

static void CPthreadReqAttrDestroy_Pre_Attr_Prepare(
  CPthreadReqAttrDestroy_Context *ctx,
  CPthreadReqAttrDestroy_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrDestroy_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      break;
    }

    case CPthreadReqAttrDestroy_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrDestroy_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrDestroy_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrDestroy_Post_Status_Check(
  CPthreadReqAttrDestroy_Context    *ctx,
  CPthreadReqAttrDestroy_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrDestroy_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_destroy() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrDestroy_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_destroy() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrDestroy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrDestroy_Post_AttrObj_Check(
  CPthreadReqAttrDestroy_Context     *ctx,
  CPthreadReqAttrDestroy_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrDestroy_Post_AttrObj_NotInitialized: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * be uninitialized after the return of the pthread_attr_destroy() call.
       */
      T_false( ctx->attr_obj.is_initialized );
      break;
    }

    case CPthreadReqAttrDestroy_Post_AttrObj_Nop: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * not be modified by the pthread_attr_destroy() call.
       */
      T_false( ctx->attr_obj.is_initialized );
      break;
    }

    case CPthreadReqAttrDestroy_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrDestroy_Action(
  CPthreadReqAttrDestroy_Context *ctx
)
{
  ctx->retval = pthread_attr_destroy( ctx->attr );
}

static const CPthreadReqAttrDestroy_Entry
CPthreadReqAttrDestroy_Entries[] = {
  { 0, 0, CPthreadReqAttrDestroy_Post_Status_Zero,
    CPthreadReqAttrDestroy_Post_AttrObj_NotInitialized },
  { 0, 0, CPthreadReqAttrDestroy_Post_Status_EINVAL,
    CPthreadReqAttrDestroy_Post_AttrObj_NA },
  { 0, 0, CPthreadReqAttrDestroy_Post_Status_EINVAL,
    CPthreadReqAttrDestroy_Post_AttrObj_Nop }
};

static const uint8_t
CPthreadReqAttrDestroy_Map[] = {
  0, 1, 2
};

static size_t CPthreadReqAttrDestroy_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqAttrDestroy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqAttrDestroy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqAttrDestroy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrDestroy_Scope,
  .initial_context = &CPthreadReqAttrDestroy_Instance
};

static inline CPthreadReqAttrDestroy_Entry CPthreadReqAttrDestroy_PopEntry(
  CPthreadReqAttrDestroy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrDestroy_Entries[
    CPthreadReqAttrDestroy_Map[ index ]
  ];
}

static void CPthreadReqAttrDestroy_TestVariant(
  CPthreadReqAttrDestroy_Context *ctx
)
{
  CPthreadReqAttrDestroy_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrDestroy_Action( ctx );
  CPthreadReqAttrDestroy_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrDestroy_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrDestroy( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqAttrDestroy, &CPthreadReqAttrDestroy_Fixture )
{
  CPthreadReqAttrDestroy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrDestroy_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrDestroy_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqAttrDestroy_PopEntry( ctx );
    CPthreadReqAttrDestroy_TestVariant( ctx );
  }
}

/** @} */
