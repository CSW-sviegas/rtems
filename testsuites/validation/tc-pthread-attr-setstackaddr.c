/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetstackaddr
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
 * @defgroup CPthreadReqAttrSetstackaddr spec:/c/pthread/req/attr-setstackaddr
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetstackaddr_Pre_Attr_Valid,
  CPthreadReqAttrSetstackaddr_Pre_Attr_Null,
  CPthreadReqAttrSetstackaddr_Pre_Attr_Invalid,
  CPthreadReqAttrSetstackaddr_Pre_Attr_NA
} CPthreadReqAttrSetstackaddr_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetstackaddr_Pre_StackAddr_Valid,
  CPthreadReqAttrSetstackaddr_Pre_StackAddr_NA
} CPthreadReqAttrSetstackaddr_Pre_StackAddr;

typedef enum {
  CPthreadReqAttrSetstackaddr_Post_Status_Zero,
  CPthreadReqAttrSetstackaddr_Post_Status_EINVAL,
  CPthreadReqAttrSetstackaddr_Post_Status_NA
} CPthreadReqAttrSetstackaddr_Post_Status;

typedef enum {
  CPthreadReqAttrSetstackaddr_Post_AttrObj_Set,
  CPthreadReqAttrSetstackaddr_Post_AttrObj_Nop,
  CPthreadReqAttrSetstackaddr_Post_AttrObj_NA
} CPthreadReqAttrSetstackaddr_Post_AttrObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_StackAddr_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_AttrObj : 2;
} CPthreadReqAttrSetstackaddr_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setstackaddr test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `stackaddr` parameter value.
   */
  void *stackaddr;

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
    CPthreadReqAttrSetstackaddr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetstackaddr_Context;

static CPthreadReqAttrSetstackaddr_Context
  CPthreadReqAttrSetstackaddr_Instance;

static const char * const CPthreadReqAttrSetstackaddr_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrSetstackaddr_PreDesc_StackAddr[] = {
  "Valid",
  "NA"
};

static const char * const * const CPthreadReqAttrSetstackaddr_PreDesc[] = {
  CPthreadReqAttrSetstackaddr_PreDesc_Attr,
  CPthreadReqAttrSetstackaddr_PreDesc_StackAddr,
  NULL
};

#define STACK_ADDR_VALUE ( (void *) 0x1000 )

#define STALE_STACK_ADDR ( (void *) 0x2000 )

static void CPthreadReqAttrSetstackaddr_Pre_Attr_Prepare(
  CPthreadReqAttrSetstackaddr_Context *ctx,
  CPthreadReqAttrSetstackaddr_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      ctx->attr_obj.stackaddr = STALE_STACK_ADDR;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.stackaddr = STALE_STACK_ADDR;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Pre_StackAddr_Prepare(
  CPthreadReqAttrSetstackaddr_Context      *ctx,
  CPthreadReqAttrSetstackaddr_Pre_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Pre_StackAddr_Valid: {
      /*
       * While the `stackaddr` parameter specifies a stack address.
       */
      ctx->stackaddr = STACK_ADDR_VALUE;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_StackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Post_Status_Check(
  CPthreadReqAttrSetstackaddr_Context    *ctx,
  CPthreadReqAttrSetstackaddr_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_setstackaddr() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_setstackaddr() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Post_AttrObj_Check(
  CPthreadReqAttrSetstackaddr_Context     *ctx,
  CPthreadReqAttrSetstackaddr_Post_AttrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Post_AttrObj_Set: {
      /*
       * The value of the thread stack address attribute of the thread
       * attributes object referenced by the `attr` parameter shall be set to
       * the address specified by the `stackaddr` parameter.
       */
      void *stackaddr;
      int   eno;

      eno = pthread_attr_getstackaddr( &ctx->attr_obj, &stackaddr );
      T_eno_success( eno );
      T_eq_ptr( stackaddr, STACK_ADDR_VALUE );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_AttrObj_Nop: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * not be modified.
       */
      T_eq_ptr( ctx->attr_obj.stackaddr, STALE_STACK_ADDR );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_AttrObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Action(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  ctx->retval = pthread_attr_setstackaddr( ctx->attr, ctx->stackaddr );
}

static const CPthreadReqAttrSetstackaddr_Entry
CPthreadReqAttrSetstackaddr_Entries[] = {
  { 0, 0, 0, CPthreadReqAttrSetstackaddr_Post_Status_Zero,
    CPthreadReqAttrSetstackaddr_Post_AttrObj_Set },
  { 0, 0, 0, CPthreadReqAttrSetstackaddr_Post_Status_EINVAL,
    CPthreadReqAttrSetstackaddr_Post_AttrObj_NA },
  { 0, 0, 0, CPthreadReqAttrSetstackaddr_Post_Status_EINVAL,
    CPthreadReqAttrSetstackaddr_Post_AttrObj_Nop }
};

static const uint8_t
CPthreadReqAttrSetstackaddr_Map[] = {
  0, 1, 2
};

static size_t CPthreadReqAttrSetstackaddr_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetstackaddr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetstackaddr_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetstackaddr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetstackaddr_Scope,
  .initial_context = &CPthreadReqAttrSetstackaddr_Instance
};

static inline CPthreadReqAttrSetstackaddr_Entry
CPthreadReqAttrSetstackaddr_PopEntry(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetstackaddr_Entries[
    CPthreadReqAttrSetstackaddr_Map[ index ]
  ];
}

static void CPthreadReqAttrSetstackaddr_TestVariant(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  CPthreadReqAttrSetstackaddr_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetstackaddr_Pre_StackAddr_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrSetstackaddr_Action( ctx );
  CPthreadReqAttrSetstackaddr_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetstackaddr_Post_AttrObj_Check(
    ctx,
    ctx->Map.entry.Post_AttrObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetstackaddr( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetstackaddr,
  &CPthreadReqAttrSetstackaddr_Fixture
)
{
  CPthreadReqAttrSetstackaddr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetstackaddr_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetstackaddr_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetstackaddr_Pre_StackAddr_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrSetstackaddr_Pre_StackAddr_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqAttrSetstackaddr_PopEntry( ctx );
      CPthreadReqAttrSetstackaddr_TestVariant( ctx );
    }
  }
}

/** @} */
