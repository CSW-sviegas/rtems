/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetscope
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
 * @defgroup CPthreadReqAttrSetscope spec:/c/pthread/req/attr-setscope
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetscope_Pre_Attr_Valid,
  CPthreadReqAttrSetscope_Pre_Attr_Null,
  CPthreadReqAttrSetscope_Pre_Attr_Invalid,
  CPthreadReqAttrSetscope_Pre_Attr_NA
} CPthreadReqAttrSetscope_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetscope_Pre_ContentionScope_Process,
  CPthreadReqAttrSetscope_Pre_ContentionScope_System,
  CPthreadReqAttrSetscope_Pre_ContentionScope_Invalid,
  CPthreadReqAttrSetscope_Pre_ContentionScope_NA
} CPthreadReqAttrSetscope_Pre_ContentionScope;

typedef enum {
  CPthreadReqAttrSetscope_Post_Status_Zero,
  CPthreadReqAttrSetscope_Post_Status_EINVAL,
  CPthreadReqAttrSetscope_Post_Status_ENOTSUP,
  CPthreadReqAttrSetscope_Post_Status_NA
} CPthreadReqAttrSetscope_Post_Status;

typedef enum {
  CPthreadReqAttrSetscope_Post_AttrObj_Set,
  CPthreadReqAttrSetscope_Post_AttrObj_Nop,
  CPthreadReqAttrSetscope_Post_AttrObj_NA
} CPthreadReqAttrSetscope_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_ContentionScope_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqAttrSetscope_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setscope test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `contentionscope` parameter value.
   */
  int contentionscope;

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
    CPthreadReqAttrSetscope_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetscope_Context;

static CPthreadReqAttrSetscope_Context
  CPthreadReqAttrSetscope_Instance;

static const char * const CPthreadReqAttrSetscope_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrSetscope_PreDesc_ContentionScope[] = {
  "Process",
  "System",
  "Invalid",
  "NA"
};

static const char * const * const CPthreadReqAttrSetscope_PreDesc[] = {
  CPthreadReqAttrSetscope_PreDesc_Attr,
  CPthreadReqAttrSetscope_PreDesc_ContentionScope,
  NULL
};

#define CONTENTIONSCOPE_INVALID 123

static void CPthreadReqAttrSetscope_Pre_Attr_Prepare(
  CPthreadReqAttrSetscope_Context *ctx,
  CPthreadReqAttrSetscope_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      ctx->attr_obj.contentionscope = -1;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.contentionscope = -1;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Pre_ContentionScope_Prepare(
  CPthreadReqAttrSetscope_Context            *ctx,
  CPthreadReqAttrSetscope_Pre_ContentionScope state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Pre_ContentionScope_Process: {
      /*
       * While the `contentionscope` parameter is equal to
       * PTHREAD_SCOPE_PROCESS.
       */
      ctx->contentionscope = PTHREAD_SCOPE_PROCESS;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_ContentionScope_System: {
      /*
       * While the `contentionscope` parameter is equal to
       * PTHREAD_SCOPE_SYSTEM.
       */
      ctx->contentionscope = PTHREAD_SCOPE_SYSTEM;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_ContentionScope_Invalid: {
      /*
       * While the `contentionscope` parameter is not a valid thread contention
       * scope attribute.
       */
      ctx->contentionscope = CONTENTIONSCOPE_INVALID;
      break;
    }

    case CPthreadReqAttrSetscope_Pre_ContentionScope_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Post_Status_Check(
  CPthreadReqAttrSetscope_Context    *ctx,
  CPthreadReqAttrSetscope_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_setscope() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrSetscope_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_setscope() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrSetscope_Post_Status_ENOTSUP: {
      /*
       * The return value of pthread_attr_setscope() shall be equal to ENOTSUP.
       */
      T_eq_int( ctx->retval, ENOTSUP );
      break;
    }

    case CPthreadReqAttrSetscope_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Post_AttrObj_Check(
  CPthreadReqAttrSetscope_Context     *ctx,
  CPthreadReqAttrSetscope_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetscope_Post_AttrObj_Set: {
      /*
       * The value of the thread contention scope attribute of the thread
       * attributes object referenced by the `attr` parameter shall be set to
       * the contention scope specified by the `contentionscope` parameter.
       */
      int contentionscope;
      int eno;

      eno = pthread_attr_getscope( &ctx->attr_obj, &contentionscope );
      T_eno_success( eno );
      T_eq_int( contentionscope, ctx->contentionscope );
      break;
    }

    case CPthreadReqAttrSetscope_Post_AttrObj_Nop: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * not be modified.
       */
      T_eq_int( ctx->attr_obj.contentionscope, -1 );
      break;
    }

    case CPthreadReqAttrSetscope_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetscope_Action(
  CPthreadReqAttrSetscope_Context *ctx
)
{
  ctx->retval = pthread_attr_setscope( ctx->attr, ctx->contentionscope );
}

static const CPthreadReqAttrSetscope_Entry
CPthreadReqAttrSetscope_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrSetscope_Post_Status_EINVAL,
    CPthreadReqAttrSetscope_Post_AttrObj_Nop },
  { 0, 0, 0, CPthreadReqAttrSetscope_Post_Status_EINVAL,
    CPthreadReqAttrSetscope_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqAttrSetscope_Post_Status_Zero,
    CPthreadReqAttrSetscope_Post_AttrObj_Set },
  { 0, 0, 0, CPthreadReqAttrSetscope_Post_Status_ENOTSUP,
    CPthreadReqAttrSetscope_Post_AttrObj_Nop }
};

static const uint8_t
CPthreadReqAttrSetscope_Map[] = {
  2, 3, 0, 1, 1, 1, 0, 0, 0
};

static size_t CPthreadReqAttrSetscope_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqAttrSetscope_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetscope_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetscope_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetscope_Scope,
  .initial_context = &CPthreadReqAttrSetscope_Instance
};

static inline CPthreadReqAttrSetscope_Entry CPthreadReqAttrSetscope_PopEntry(
  CPthreadReqAttrSetscope_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetscope_Entries[
    CPthreadReqAttrSetscope_Map[ index ]
  ];
}

static void CPthreadReqAttrSetscope_TestVariant(
  CPthreadReqAttrSetscope_Context *ctx
)
{
  CPthreadReqAttrSetscope_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetscope_Pre_ContentionScope_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetscope_Action( ctx );
  CPthreadReqAttrSetscope_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrSetscope_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetscope( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetscope,
  &CPthreadReqAttrSetscope_Fixture
)
{
  CPthreadReqAttrSetscope_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetscope_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetscope_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetscope_Pre_ContentionScope_Process;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrSetscope_Pre_ContentionScope_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrSetscope_PopEntry( ctx );
      CPthreadReqAttrSetscope_TestVariant( ctx );
    }
  }
}

/** @} */
