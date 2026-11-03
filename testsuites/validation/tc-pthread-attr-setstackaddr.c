/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetstackaddr
 */

/*
 * Copyright (C) 2026 Critical Software S.A.
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
#include <stdint.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqAttrSetstackaddr spec:/c/pthread/req/attr-setstackaddr
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetstackaddr_Pre_Attr_Obj,
  CPthreadReqAttrSetstackaddr_Pre_Attr_Null,
  CPthreadReqAttrSetstackaddr_Pre_Attr_NA
} CPthreadReqAttrSetstackaddr_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetstackaddr_Pre_Initialized_Yes,
  CPthreadReqAttrSetstackaddr_Pre_Initialized_No,
  CPthreadReqAttrSetstackaddr_Pre_Initialized_NA
} CPthreadReqAttrSetstackaddr_Pre_Initialized;

typedef enum {
  CPthreadReqAttrSetstackaddr_Pre_StackAddr_Null,
  CPthreadReqAttrSetstackaddr_Pre_StackAddr_Area,
  CPthreadReqAttrSetstackaddr_Pre_StackAddr_NA
} CPthreadReqAttrSetstackaddr_Pre_StackAddr;

typedef enum {
  CPthreadReqAttrSetstackaddr_Post_Status_Ok,
  CPthreadReqAttrSetstackaddr_Post_Status_Inval,
  CPthreadReqAttrSetstackaddr_Post_Status_NA
} CPthreadReqAttrSetstackaddr_Post_Status;

typedef enum {
  CPthreadReqAttrSetstackaddr_Post_StackAddr_Null,
  CPthreadReqAttrSetstackaddr_Post_StackAddr_Area,
  CPthreadReqAttrSetstackaddr_Post_StackAddr_Nop,
  CPthreadReqAttrSetstackaddr_Post_StackAddr_NA
} CPthreadReqAttrSetstackaddr_Post_StackAddr;

typedef enum {
  CPthreadReqAttrSetstackaddr_Post_Other_Nop,
  CPthreadReqAttrSetstackaddr_Post_Other_NA
} CPthreadReqAttrSetstackaddr_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StackAddr_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_StackAddr : 2;
  uint16_t Post_Other : 1;
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
   * @brief This member contains the return value of the
   *   pthread_attr_setstackaddr() call.
   */
  int status;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains a copy of the thread attributes object before
   *   the call.
   */
  pthread_attr_t attr_before;

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

static const char *const CPthreadReqAttrSetstackaddr_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrSetstackaddr_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqAttrSetstackaddr_PreDesc_StackAddr[] =
  { "Null", "Area", "NA" };

static const char *const *const CPthreadReqAttrSetstackaddr_PreDesc[] = {
  CPthreadReqAttrSetstackaddr_PreDesc_Attr,
  CPthreadReqAttrSetstackaddr_PreDesc_Initialized,
  CPthreadReqAttrSetstackaddr_PreDesc_StackAddr,
  NULL
};

static char stack_area[ 256 ];

#define STACK_ADDR_POISON ( (void *) 1 )

static void CPthreadReqAttrSetstackaddr_Pre_Attr_Prepare(
  CPthreadReqAttrSetstackaddr_Context *ctx,
  CPthreadReqAttrSetstackaddr_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Pre_Initialized_Prepare(
  CPthreadReqAttrSetstackaddr_Context        *ctx,
  CPthreadReqAttrSetstackaddr_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      ctx->attr_obj.stackaddr = STACK_ADDR_POISON;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.stackaddr = STACK_ADDR_POISON;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Pre_StackAddr_Prepare(
  CPthreadReqAttrSetstackaddr_Context      *ctx,
  CPthreadReqAttrSetstackaddr_Pre_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Pre_StackAddr_Null: {
      /*
       * While the `stackaddr` parameter is NULL.
       */
      ctx->stackaddr = NULL;
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_StackAddr_Area: {
      /*
       * While the `stackaddr` parameter is not NULL.
       */
      ctx->stackaddr = stack_area;
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
    case CPthreadReqAttrSetstackaddr_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_setstackaddr() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_setstackaddr() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Post_StackAddr_Check(
  CPthreadReqAttrSetstackaddr_Context       *ctx,
  CPthreadReqAttrSetstackaddr_Post_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Post_StackAddr_Null: {
      /*
       * The thread stack address attribute of the thread attributes object
       * referenced by the `attr` parameter shall be NULL.
       */
      T_null( ctx->attr_obj.stackaddr );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_StackAddr_Area: {
      /*
       * The thread stack address attribute of the thread attributes object
       * referenced by the `attr` parameter shall be the value of the
       * `stackaddr` parameter.
       */
      T_eq_ptr( ctx->attr_obj.stackaddr, stack_area );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_StackAddr_Nop: {
      /*
       * The thread stack address attribute of the thread attributes object
       * referenced by the `attr` parameter shall not be modified.
       */
      T_eq_ptr( ctx->attr_obj.stackaddr, ctx->attr_before.stackaddr );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_StackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Post_Other_Check(
  CPthreadReqAttrSetstackaddr_Context   *ctx,
  CPthreadReqAttrSetstackaddr_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Post_Other_Nop: {
      /*
       * The content of the thread attributes object referenced by the `attr`
       * parameter, other than its thread stack address attribute, shall not be
       * modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      expected.stackaddr = ctx->attr_obj.stackaddr;
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Action(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_setstackaddr( ctx->attr, ctx->stackaddr );
}

/* clang-format off */

static const CPthreadReqAttrSetstackaddr_Entry
CPthreadReqAttrSetstackaddr_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrSetstackaddr_Post_Status_Inval,
    CPthreadReqAttrSetstackaddr_Post_StackAddr_Nop,
    CPthreadReqAttrSetstackaddr_Post_Other_Nop },
  { 0, 0, 1, 0, CPthreadReqAttrSetstackaddr_Post_Status_Inval,
    CPthreadReqAttrSetstackaddr_Post_StackAddr_NA,
    CPthreadReqAttrSetstackaddr_Post_Other_NA },
  { 1, 0, 0, 0, CPthreadReqAttrSetstackaddr_Post_Status_NA,
    CPthreadReqAttrSetstackaddr_Post_StackAddr_NA,
    CPthreadReqAttrSetstackaddr_Post_Other_NA },
  { 0, 0, 0, 0, CPthreadReqAttrSetstackaddr_Post_Status_Ok,
    CPthreadReqAttrSetstackaddr_Post_StackAddr_Null,
    CPthreadReqAttrSetstackaddr_Post_Other_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrSetstackaddr_Post_Status_Ok,
    CPthreadReqAttrSetstackaddr_Post_StackAddr_Area,
    CPthreadReqAttrSetstackaddr_Post_Other_Nop }
};

static const uint8_t
CPthreadReqAttrSetstackaddr_Map[] = {
  3, 4, 0, 0, 1, 1, 2, 2
};

/* clang-format on */

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
  return CPthreadReqAttrSetstackaddr_Entries
    [ CPthreadReqAttrSetstackaddr_Map[ index ] ];
}

static void CPthreadReqAttrSetstackaddr_SetPreConditionStates(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetstackaddr_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void CPthreadReqAttrSetstackaddr_TestVariant(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  CPthreadReqAttrSetstackaddr_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetstackaddr_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetstackaddr_Pre_StackAddr_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqAttrSetstackaddr_Action( ctx );
  CPthreadReqAttrSetstackaddr_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetstackaddr_Post_StackAddr_Check(
    ctx,
    ctx->Map.entry.Post_StackAddr
  );
  CPthreadReqAttrSetstackaddr_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
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
    ctx->Map.pci[ 0 ] = CPthreadReqAttrSetstackaddr_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrSetstackaddr_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrSetstackaddr_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrSetstackaddr_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqAttrSetstackaddr_Pre_StackAddr_Null;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrSetstackaddr_Pre_StackAddr_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrSetstackaddr_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CPthreadReqAttrSetstackaddr_SetPreConditionStates( ctx );
        CPthreadReqAttrSetstackaddr_TestVariant( ctx );
      }
    }
  }
}

/** @} */
