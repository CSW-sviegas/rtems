/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetstack
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
 * @defgroup CPthreadReqAttrGetstack spec:/c/pthread/req/attr-getstack
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetstack_Pre_Attr_Obj,
  CPthreadReqAttrGetstack_Pre_Attr_Null,
  CPthreadReqAttrGetstack_Pre_Attr_NA
} CPthreadReqAttrGetstack_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetstack_Pre_Initialized_Yes,
  CPthreadReqAttrGetstack_Pre_Initialized_No,
  CPthreadReqAttrGetstack_Pre_Initialized_NA
} CPthreadReqAttrGetstack_Pre_Initialized;

typedef enum {
  CPthreadReqAttrGetstack_Pre_StoredStackAddr_Null,
  CPthreadReqAttrGetstack_Pre_StoredStackAddr_Area,
  CPthreadReqAttrGetstack_Pre_StoredStackAddr_NA
} CPthreadReqAttrGetstack_Pre_StoredStackAddr;

typedef enum {
  CPthreadReqAttrGetstack_Pre_StoredStackSize_Zero,
  CPthreadReqAttrGetstack_Pre_StoredStackSize_Positive,
  CPthreadReqAttrGetstack_Pre_StoredStackSize_NA
} CPthreadReqAttrGetstack_Pre_StoredStackSize;

typedef enum {
  CPthreadReqAttrGetstack_Pre_StackAddr_Obj,
  CPthreadReqAttrGetstack_Pre_StackAddr_Null,
  CPthreadReqAttrGetstack_Pre_StackAddr_NA
} CPthreadReqAttrGetstack_Pre_StackAddr;

typedef enum {
  CPthreadReqAttrGetstack_Pre_StackSize_Obj,
  CPthreadReqAttrGetstack_Pre_StackSize_Null,
  CPthreadReqAttrGetstack_Pre_StackSize_NA
} CPthreadReqAttrGetstack_Pre_StackSize;

typedef enum {
  CPthreadReqAttrGetstack_Post_Status_Ok,
  CPthreadReqAttrGetstack_Post_Status_Inval,
  CPthreadReqAttrGetstack_Post_Status_NA
} CPthreadReqAttrGetstack_Post_Status;

typedef enum {
  CPthreadReqAttrGetstack_Post_StackAddr_Null,
  CPthreadReqAttrGetstack_Post_StackAddr_Area,
  CPthreadReqAttrGetstack_Post_StackAddr_Nop,
  CPthreadReqAttrGetstack_Post_StackAddr_NA
} CPthreadReqAttrGetstack_Post_StackAddr;

typedef enum {
  CPthreadReqAttrGetstack_Post_StackSize_Zero,
  CPthreadReqAttrGetstack_Post_StackSize_Positive,
  CPthreadReqAttrGetstack_Post_StackSize_Nop,
  CPthreadReqAttrGetstack_Post_StackSize_NA
} CPthreadReqAttrGetstack_Post_StackSize;

typedef enum {
  CPthreadReqAttrGetstack_Post_Attr_Nop,
  CPthreadReqAttrGetstack_Post_Attr_NA
} CPthreadReqAttrGetstack_Post_Attr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_StoredStackAddr_NA : 1;
  uint16_t Pre_StoredStackSize_NA : 1;
  uint16_t Pre_StackAddr_NA : 1;
  uint16_t Pre_StackSize_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_StackAddr : 2;
  uint16_t Post_StackSize : 2;
  uint16_t Post_Attr : 1;
} CPthreadReqAttrGetstack_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-getstack test case.
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
   * @brief This member specifies the `stacksize` parameter value.
   */
  size_t *stacksize;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_getstack() call.
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

  /**
   * @brief This member contains the object referenced by the `stacksize`
   *   parameter.
   */
  size_t stacksize_obj;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    CPthreadReqAttrGetstack_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetstack_Context;

static CPthreadReqAttrGetstack_Context CPthreadReqAttrGetstack_Instance;

static const char *const CPthreadReqAttrGetstack_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrGetstack_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqAttrGetstack_PreDesc_StoredStackAddr[] =
  { "Null", "Area", "NA" };

static const char *const CPthreadReqAttrGetstack_PreDesc_StoredStackSize[] =
  { "Zero", "Positive", "NA" };

static const char *const CPthreadReqAttrGetstack_PreDesc_StackAddr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrGetstack_PreDesc_StackSize[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrGetstack_PreDesc[] = {
  CPthreadReqAttrGetstack_PreDesc_Attr,
  CPthreadReqAttrGetstack_PreDesc_Initialized,
  CPthreadReqAttrGetstack_PreDesc_StoredStackAddr,
  CPthreadReqAttrGetstack_PreDesc_StoredStackSize,
  CPthreadReqAttrGetstack_PreDesc_StackAddr,
  CPthreadReqAttrGetstack_PreDesc_StackSize,
  NULL
};

static char stack_area[ 256 ];

#define STACK_ADDR_POISON ( (void *) 1 )

#define STACK_SIZE 8192

static void CPthreadReqAttrGetstack_Pre_Attr_Prepare(
  CPthreadReqAttrGetstack_Context *ctx,
  CPthreadReqAttrGetstack_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Pre_Initialized_Prepare(
  CPthreadReqAttrGetstack_Context        *ctx,
  CPthreadReqAttrGetstack_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstack_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstack_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Pre_StoredStackAddr_Prepare(
  CPthreadReqAttrGetstack_Context            *ctx,
  CPthreadReqAttrGetstack_Pre_StoredStackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Pre_StoredStackAddr_Null: {
      /*
       * While the thread stack address attribute of the object referenced by
       * the `attr` parameter is NULL.
       */
      ctx->attr_obj.stackaddr = NULL;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_StoredStackAddr_Area: {
      /*
       * While the thread stack address attribute of the object referenced by
       * the `attr` parameter is not NULL.
       */
      ctx->attr_obj.stackaddr = stack_area;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_StoredStackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Pre_StoredStackSize_Prepare(
  CPthreadReqAttrGetstack_Context            *ctx,
  CPthreadReqAttrGetstack_Pre_StoredStackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Pre_StoredStackSize_Zero: {
      /*
       * While the thread stack size attribute of the object referenced by the
       * `attr` parameter is zero.
       */
      ctx->attr_obj.stacksize = 0;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_StoredStackSize_Positive: {
      /*
       * While the thread stack size attribute of the object referenced by the
       * `attr` parameter is greater than zero.
       */
      ctx->attr_obj.stacksize = STACK_SIZE;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_StoredStackSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Pre_StackAddr_Prepare(
  CPthreadReqAttrGetstack_Context      *ctx,
  CPthreadReqAttrGetstack_Pre_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Pre_StackAddr_Obj: {
      /*
       * While the `stackaddr` parameter references an object.
       */
      ctx->stackaddr_obj = STACK_ADDR_POISON;
      ctx->stackaddr = &ctx->stackaddr_obj;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_StackAddr_Null: {
      /*
       * While the `stackaddr` parameter is NULL.
       */
      ctx->stackaddr = NULL;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_StackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Pre_StackSize_Prepare(
  CPthreadReqAttrGetstack_Context      *ctx,
  CPthreadReqAttrGetstack_Pre_StackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Pre_StackSize_Obj: {
      /*
       * While the `stacksize` parameter references an object.
       */
      ctx->stacksize_obj = SIZE_MAX;
      ctx->stacksize = &ctx->stacksize_obj;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_StackSize_Null: {
      /*
       * While the `stacksize` parameter is NULL.
       */
      ctx->stacksize = NULL;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_StackSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Post_Status_Check(
  CPthreadReqAttrGetstack_Context    *ctx,
  CPthreadReqAttrGetstack_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_getstack() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrGetstack_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_getstack() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrGetstack_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Post_StackAddr_Check(
  CPthreadReqAttrGetstack_Context       *ctx,
  CPthreadReqAttrGetstack_Post_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Post_StackAddr_Null: {
      /*
       * The object referenced by the `stackaddr` parameter shall be NULL.
       */
      T_null( ctx->stackaddr_obj );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackAddr_Area: {
      /*
       * The object referenced by the `stackaddr` parameter shall be the thread
       * stack address attribute of the object referenced by the `attr`
       * parameter.
       */
      T_eq_ptr( ctx->stackaddr_obj, stack_area );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackAddr_Nop: {
      /*
       * The object referenced by the `stackaddr` parameter shall not be
       * modified.
       */
      T_eq_ptr( ctx->stackaddr_obj, STACK_ADDR_POISON );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Post_StackSize_Check(
  CPthreadReqAttrGetstack_Context       *ctx,
  CPthreadReqAttrGetstack_Post_StackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Post_StackSize_Zero: {
      /*
       * The object referenced by the `stacksize` parameter shall be zero.
       */
      T_eq_sz( ctx->stacksize_obj, 0 );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackSize_Positive: {
      /*
       * The object referenced by the `stacksize` parameter shall be the thread
       * stack size attribute of the object referenced by the `attr` parameter.
       */
      T_eq_sz( ctx->stacksize_obj, STACK_SIZE );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackSize_Nop: {
      /*
       * The object referenced by the `stacksize` parameter shall not be
       * modified.
       */
      T_eq_sz( ctx->stacksize_obj, SIZE_MAX );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackSize_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Post_Attr_Check(
  CPthreadReqAttrGetstack_Context  *ctx,
  CPthreadReqAttrGetstack_Post_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Post_Attr_Nop: {
      /*
       * The object referenced by the `attr` parameter shall not be modified.
       */
      T_eq_mem( &ctx->attr_obj, &ctx->attr_before, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstack_Post_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Action(
  CPthreadReqAttrGetstack_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_getstack(
    ctx->attr,
    ctx->stackaddr,
    ctx->stacksize
  );
}

/* clang-format off */

static const CPthreadReqAttrGetstack_Entry
CPthreadReqAttrGetstack_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_NA,
    CPthreadReqAttrGetstack_Post_StackAddr_NA,
    CPthreadReqAttrGetstack_Post_StackSize_NA,
    CPthreadReqAttrGetstack_Post_Attr_NA },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Inval,
    CPthreadReqAttrGetstack_Post_StackAddr_Nop,
    CPthreadReqAttrGetstack_Post_StackSize_NA,
    CPthreadReqAttrGetstack_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Inval,
    CPthreadReqAttrGetstack_Post_StackAddr_NA,
    CPthreadReqAttrGetstack_Post_StackSize_Nop,
    CPthreadReqAttrGetstack_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Inval,
    CPthreadReqAttrGetstack_Post_StackAddr_NA,
    CPthreadReqAttrGetstack_Post_StackSize_NA,
    CPthreadReqAttrGetstack_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Inval,
    CPthreadReqAttrGetstack_Post_StackAddr_Nop,
    CPthreadReqAttrGetstack_Post_StackSize_Nop,
    CPthreadReqAttrGetstack_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Ok,
    CPthreadReqAttrGetstack_Post_StackAddr_Null,
    CPthreadReqAttrGetstack_Post_StackSize_Zero,
    CPthreadReqAttrGetstack_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Ok,
    CPthreadReqAttrGetstack_Post_StackAddr_Null,
    CPthreadReqAttrGetstack_Post_StackSize_Positive,
    CPthreadReqAttrGetstack_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Ok,
    CPthreadReqAttrGetstack_Post_StackAddr_Area,
    CPthreadReqAttrGetstack_Post_StackSize_Zero,
    CPthreadReqAttrGetstack_Post_Attr_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Ok,
    CPthreadReqAttrGetstack_Post_StackAddr_Area,
    CPthreadReqAttrGetstack_Post_StackSize_Positive,
    CPthreadReqAttrGetstack_Post_Attr_Nop },
  { 0, 0, 1, 1, 1, 0, 0, CPthreadReqAttrGetstack_Post_Status_Inval,
    CPthreadReqAttrGetstack_Post_StackAddr_Nop,
    CPthreadReqAttrGetstack_Post_StackSize_Nop,
    CPthreadReqAttrGetstack_Post_Attr_NA },
  { 0, 0, 1, 1, 1, 0, 0, CPthreadReqAttrGetstack_Post_Status_Inval,
    CPthreadReqAttrGetstack_Post_StackAddr_Nop,
    CPthreadReqAttrGetstack_Post_StackSize_NA,
    CPthreadReqAttrGetstack_Post_Attr_NA },
  { 0, 0, 1, 1, 1, 0, 0, CPthreadReqAttrGetstack_Post_Status_Inval,
    CPthreadReqAttrGetstack_Post_StackAddr_NA,
    CPthreadReqAttrGetstack_Post_StackSize_Nop,
    CPthreadReqAttrGetstack_Post_Attr_NA },
  { 0, 0, 1, 1, 1, 0, 0, CPthreadReqAttrGetstack_Post_Status_Inval,
    CPthreadReqAttrGetstack_Post_StackAddr_NA,
    CPthreadReqAttrGetstack_Post_StackSize_NA,
    CPthreadReqAttrGetstack_Post_Attr_NA }
};

static const uint8_t
CPthreadReqAttrGetstack_Map[] = {
  5, 1, 2, 3, 6, 1, 2, 3, 7, 1, 2, 3, 8, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1,
  2, 3, 4, 1, 2, 3, 9, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqAttrGetstack_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqAttrGetstack_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrGetstack_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrGetstack_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrGetstack_Scope,
  .initial_context = &CPthreadReqAttrGetstack_Instance
};

static inline CPthreadReqAttrGetstack_Entry CPthreadReqAttrGetstack_PopEntry(
  CPthreadReqAttrGetstack_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrGetstack_Entries
    [ CPthreadReqAttrGetstack_Map[ index ] ];
}

static void CPthreadReqAttrGetstack_SetPreConditionStates(
  CPthreadReqAttrGetstack_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetstack_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredStackAddr_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqAttrGetstack_Pre_StoredStackAddr_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_StoredStackSize_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqAttrGetstack_Pre_StoredStackSize_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
}

static void CPthreadReqAttrGetstack_TestVariant(
  CPthreadReqAttrGetstack_Context *ctx
)
{
  CPthreadReqAttrGetstack_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetstack_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrGetstack_Pre_StoredStackAddr_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrGetstack_Pre_StoredStackSize_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  CPthreadReqAttrGetstack_Pre_StackAddr_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqAttrGetstack_Pre_StackSize_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqAttrGetstack_Action( ctx );
  CPthreadReqAttrGetstack_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrGetstack_Post_StackAddr_Check(
    ctx,
    ctx->Map.entry.Post_StackAddr
  );
  CPthreadReqAttrGetstack_Post_StackSize_Check(
    ctx,
    ctx->Map.entry.Post_StackSize
  );
  CPthreadReqAttrGetstack_Post_Attr_Check( ctx, ctx->Map.entry.Post_Attr );
}

/**
 * @fn void T_case_body_CPthreadReqAttrGetstack( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrGetstack,
  &CPthreadReqAttrGetstack_Fixture
)
{
  CPthreadReqAttrGetstack_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrGetstack_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrGetstack_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrGetstack_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrGetstack_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqAttrGetstack_Pre_StoredStackAddr_Null;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrGetstack_Pre_StoredStackAddr_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqAttrGetstack_Pre_StoredStackSize_Zero;
          ctx->Map.pci[ 3 ] < CPthreadReqAttrGetstack_Pre_StoredStackSize_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqAttrGetstack_Pre_StackAddr_Obj;
            ctx->Map.pci[ 4 ] < CPthreadReqAttrGetstack_Pre_StackAddr_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CPthreadReqAttrGetstack_Pre_StackSize_Obj;
              ctx->Map.pci[ 5 ] < CPthreadReqAttrGetstack_Pre_StackSize_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = CPthreadReqAttrGetstack_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              CPthreadReqAttrGetstack_SetPreConditionStates( ctx );
              CPthreadReqAttrGetstack_TestVariant( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
