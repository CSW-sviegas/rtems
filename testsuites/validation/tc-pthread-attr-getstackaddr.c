/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetstackaddr
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
 * @defgroup CPthreadReqAttrGetstackaddr spec:/c/pthread/req/attr-getstackaddr
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetstackaddr_Pre_Attr_Obj,
  CPthreadReqAttrGetstackaddr_Pre_Attr_Null,
  CPthreadReqAttrGetstackaddr_Pre_Attr_NA
} CPthreadReqAttrGetstackaddr_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetstackaddr_Pre_Initialized_Yes,
  CPthreadReqAttrGetstackaddr_Pre_Initialized_No,
  CPthreadReqAttrGetstackaddr_Pre_Initialized_NA
} CPthreadReqAttrGetstackaddr_Pre_Initialized;

typedef enum {
  CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_Null,
  CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_Area,
  CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_NA
} CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr;

typedef enum {
  CPthreadReqAttrGetstackaddr_Pre_StackAddr_Obj,
  CPthreadReqAttrGetstackaddr_Pre_StackAddr_Null,
  CPthreadReqAttrGetstackaddr_Pre_StackAddr_NA
} CPthreadReqAttrGetstackaddr_Pre_StackAddr;

typedef enum {
  CPthreadReqAttrGetstackaddr_Post_Status_Ok,
  CPthreadReqAttrGetstackaddr_Post_Status_Inval,
  CPthreadReqAttrGetstackaddr_Post_Status_NA
} CPthreadReqAttrGetstackaddr_Post_Status;

typedef enum {
  CPthreadReqAttrGetstackaddr_Post_StackAddr_Null,
  CPthreadReqAttrGetstackaddr_Post_StackAddr_Area,
  CPthreadReqAttrGetstackaddr_Post_StackAddr_Nop,
  CPthreadReqAttrGetstackaddr_Post_StackAddr_NA
} CPthreadReqAttrGetstackaddr_Post_StackAddr;

typedef enum {
  CPthreadReqAttrGetstackaddr_Post_Attr_Nop,
  CPthreadReqAttrGetstackaddr_Post_Attr_NA
} CPthreadReqAttrGetstackaddr_Post_Attr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StoredStackAddr_NA : 1;
  uint16_t Pre_StackAddr_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_StackAddr : 2;
  uint16_t Post_Attr : 1;
} CPthreadReqAttrGetstackaddr_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-getstackaddr test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_attr_t *attr;

  /**
   * @brief This member specifies the `stackaddr` parameter value.
   */
  void **stackaddr;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_getstackaddr() call.
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

  /**
   * @brief This member contains the object referenced by the `stackaddr`
   *   parameter.
   */
  void *stackaddr_obj;

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
    CPthreadReqAttrGetstackaddr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetstackaddr_Context;

static CPthreadReqAttrGetstackaddr_Context
  CPthreadReqAttrGetstackaddr_Instance;

static const char *const CPthreadReqAttrGetstackaddr_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrGetstackaddr_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const
  CPthreadReqAttrGetstackaddr_PreDesc_StoredStackAddr[] =
    { "Null", "Area", "NA" };

static const char *const CPthreadReqAttrGetstackaddr_PreDesc_StackAddr[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrGetstackaddr_PreDesc[] = {
  CPthreadReqAttrGetstackaddr_PreDesc_Attr,
  CPthreadReqAttrGetstackaddr_PreDesc_Initialized,
  CPthreadReqAttrGetstackaddr_PreDesc_StoredStackAddr,
  CPthreadReqAttrGetstackaddr_PreDesc_StackAddr,
  NULL
};

static char stack_area[ 256 ];

#define STACK_ADDR_POISON ( (void *) 1 )

static void CPthreadReqAttrGetstackaddr_Pre_Attr_Prepare(
  CPthreadReqAttrGetstackaddr_Context *ctx,
  CPthreadReqAttrGetstackaddr_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstackaddr_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrGetstackaddr_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetstackaddr_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstackaddr_Pre_Initialized_Prepare(
  CPthreadReqAttrGetstackaddr_Context        *ctx,
  CPthreadReqAttrGetstackaddr_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstackaddr_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstackaddr_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstackaddr_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_Prepare(
  CPthreadReqAttrGetstackaddr_Context            *ctx,
  CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_Null: {
      /*
       * While the thread stack address attribute of the object referenced by
       * the `attr` parameter is NULL.
       */
      ctx->attr_obj.stackaddr = NULL;
      break;
    }

    case CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_Area: {
      /*
       * While the thread stack address attribute of the object referenced by
       * the `attr` parameter is not NULL.
       */
      ctx->attr_obj.stackaddr = stack_area;
      break;
    }

    case CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstackaddr_Pre_StackAddr_Prepare(
  CPthreadReqAttrGetstackaddr_Context      *ctx,
  CPthreadReqAttrGetstackaddr_Pre_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstackaddr_Pre_StackAddr_Obj: {
      /*
       * While the `stackaddr` parameter references an object.
       */
      ctx->stackaddr_obj = STACK_ADDR_POISON;
      ctx->stackaddr = &ctx->stackaddr_obj;
      break;
    }

    case CPthreadReqAttrGetstackaddr_Pre_StackAddr_Null: {
      /*
       * While the `stackaddr` parameter is NULL.
       */
      ctx->stackaddr = NULL;
      break;
    }

    case CPthreadReqAttrGetstackaddr_Pre_StackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstackaddr_Post_Status_Check(
  CPthreadReqAttrGetstackaddr_Context    *ctx,
  CPthreadReqAttrGetstackaddr_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstackaddr_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_getstackaddr() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrGetstackaddr_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_getstackaddr() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrGetstackaddr_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetstackaddr_Post_StackAddr_Check(
  CPthreadReqAttrGetstackaddr_Context       *ctx,
  CPthreadReqAttrGetstackaddr_Post_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstackaddr_Post_StackAddr_Null: {
      /*
       * The object referenced by the `stackaddr` parameter shall be NULL.
       */
      T_null( ctx->stackaddr_obj );
      break;
    }

    case CPthreadReqAttrGetstackaddr_Post_StackAddr_Area: {
      /*
       * The object referenced by the `stackaddr` parameter shall be the thread
       * stack address attribute of the object referenced by the `attr`
       * parameter.
       */
      T_eq_ptr( ctx->stackaddr_obj, stack_area );
      break;
    }

    case CPthreadReqAttrGetstackaddr_Post_StackAddr_Nop: {
      /*
       * The object referenced by the `stackaddr` parameter shall not be
       * modified.
       */
      T_eq_ptr( ctx->stackaddr_obj, STACK_ADDR_POISON );
      break;
    }

    case CPthreadReqAttrGetstackaddr_Post_StackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstackaddr_Post_Attr_Check(
  CPthreadReqAttrGetstackaddr_Context  *ctx,
  CPthreadReqAttrGetstackaddr_Post_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstackaddr_Post_Attr_Nop: {
      /*
       * The object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_mem( &ctx->attr_obj, &ctx->attr_before, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstackaddr_Post_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstackaddr_Action(
  CPthreadReqAttrGetstackaddr_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_getstackaddr( ctx->attr, ctx->stackaddr );
}

/* clang-format off */

static const CPthreadReqAttrGetstackaddr_Entry
CPthreadReqAttrGetstackaddr_Entries[] = {
  { 1, 0, 0, 0, 0, CPthreadReqAttrGetstackaddr_Post_Status_NA,
    CPthreadReqAttrGetstackaddr_Post_StackAddr_NA,
    CPthreadReqAttrGetstackaddr_Post_Attr_NA },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetstackaddr_Post_Status_Inval,
    CPthreadReqAttrGetstackaddr_Post_StackAddr_NA,
    CPthreadReqAttrGetstackaddr_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetstackaddr_Post_Status_Inval,
    CPthreadReqAttrGetstackaddr_Post_StackAddr_Nop,
    CPthreadReqAttrGetstackaddr_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetstackaddr_Post_Status_Ok,
    CPthreadReqAttrGetstackaddr_Post_StackAddr_Null,
    CPthreadReqAttrGetstackaddr_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqAttrGetstackaddr_Post_Status_Ok,
    CPthreadReqAttrGetstackaddr_Post_StackAddr_Area,
    CPthreadReqAttrGetstackaddr_Post_Attr_Nop },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetstackaddr_Post_Status_Inval,
    CPthreadReqAttrGetstackaddr_Post_StackAddr_Nop,
    CPthreadReqAttrGetstackaddr_Post_Attr_NA },
  { 0, 0, 1, 1, 0, CPthreadReqAttrGetstackaddr_Post_Status_Inval,
    CPthreadReqAttrGetstackaddr_Post_StackAddr_NA,
    CPthreadReqAttrGetstackaddr_Post_Attr_NA }
};

static const uint8_t
CPthreadReqAttrGetstackaddr_Map[] = {
  3, 1, 4, 1, 2, 1, 2, 1, 5, 6, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqAttrGetstackaddr_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrGetstackaddr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrGetstackaddr_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrGetstackaddr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrGetstackaddr_Scope,
  .initial_context = &CPthreadReqAttrGetstackaddr_Instance
};

static inline CPthreadReqAttrGetstackaddr_Entry
CPthreadReqAttrGetstackaddr_PopEntry(
  CPthreadReqAttrGetstackaddr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrGetstackaddr_Entries
    [ CPthreadReqAttrGetstackaddr_Map[ index ] ];
}

static void CPthreadReqAttrGetstackaddr_SetPreConditionStates(
  CPthreadReqAttrGetstackaddr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetstackaddr_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredStackAddr_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void CPthreadReqAttrGetstackaddr_TestVariant(
  CPthreadReqAttrGetstackaddr_Context *ctx
)
{
  CPthreadReqAttrGetstackaddr_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetstackaddr_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrGetstackaddr_Pre_StackAddr_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqAttrGetstackaddr_Action( ctx );
  CPthreadReqAttrGetstackaddr_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrGetstackaddr_Post_StackAddr_Check(
    ctx,
    ctx->Map.entry.Post_StackAddr
  );
  CPthreadReqAttrGetstackaddr_Post_Attr_Check( ctx, ctx->Map.entry.Post_Attr );
}

/**
 * @fn void T_case_body_CPthreadReqAttrGetstackaddr( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrGetstackaddr,
  &CPthreadReqAttrGetstackaddr_Fixture
)
{
  CPthreadReqAttrGetstackaddr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrGetstackaddr_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrGetstackaddr_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrGetstackaddr_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrGetstackaddr_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_Null;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrGetstackaddr_Pre_StoredStackAddr_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqAttrGetstackaddr_Pre_StackAddr_Obj;
          ctx->Map.pci[ 3 ] < CPthreadReqAttrGetstackaddr_Pre_StackAddr_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqAttrGetstackaddr_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CPthreadReqAttrGetstackaddr_SetPreConditionStates( ctx );
          CPthreadReqAttrGetstackaddr_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
