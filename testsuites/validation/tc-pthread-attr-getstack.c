/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetstack
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
 * @defgroup CPthreadReqAttrGetstack spec:/c/pthread/req/attr-getstack
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetstack_Pre_Attr_Valid,
  CPthreadReqAttrGetstack_Pre_Attr_Null,
  CPthreadReqAttrGetstack_Pre_Attr_Invalid,
  CPthreadReqAttrGetstack_Pre_Attr_NA
} CPthreadReqAttrGetstack_Pre_Attr;

typedef enum {
  CPthreadReqAttrGetstack_Pre_StackAddr_Valid,
  CPthreadReqAttrGetstack_Pre_StackAddr_Null,
  CPthreadReqAttrGetstack_Pre_StackAddr_NA
} CPthreadReqAttrGetstack_Pre_StackAddr;

typedef enum {
  CPthreadReqAttrGetstack_Pre_StackSize_Valid,
  CPthreadReqAttrGetstack_Pre_StackSize_Null,
  CPthreadReqAttrGetstack_Pre_StackSize_NA
} CPthreadReqAttrGetstack_Pre_StackSize;

typedef enum {
  CPthreadReqAttrGetstack_Post_Status_Zero,
  CPthreadReqAttrGetstack_Post_Status_EINVAL,
  CPthreadReqAttrGetstack_Post_Status_NA
} CPthreadReqAttrGetstack_Post_Status;

typedef enum {
  CPthreadReqAttrGetstack_Post_StackAddrObj_Set,
  CPthreadReqAttrGetstack_Post_StackAddrObj_Nop,
  CPthreadReqAttrGetstack_Post_StackAddrObj_NA
} CPthreadReqAttrGetstack_Post_StackAddrObj;

typedef enum {
  CPthreadReqAttrGetstack_Post_StackSizeObj_Set,
  CPthreadReqAttrGetstack_Post_StackSizeObj_Nop,
  CPthreadReqAttrGetstack_Post_StackSizeObj_NA
} CPthreadReqAttrGetstack_Post_StackSizeObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_StackAddr_NA : 1;
  uint16_t Pre_StackSize_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_StackAddrObj : 2;
  uint16_t Post_StackSizeObj : 2;
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
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains the stack address output object.
   */
  void *stackaddr_value;

  /**
   * @brief This member contains the stack size output object.
   */
  size_t stacksize_value;

  struct {
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
    CPthreadReqAttrGetstack_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetstack_Context;

static CPthreadReqAttrGetstack_Context
  CPthreadReqAttrGetstack_Instance;

static const char * const CPthreadReqAttrGetstack_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrGetstack_PreDesc_StackAddr[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CPthreadReqAttrGetstack_PreDesc_StackSize[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqAttrGetstack_PreDesc[] = {
  CPthreadReqAttrGetstack_PreDesc_Attr,
  CPthreadReqAttrGetstack_PreDesc_StackAddr,
  CPthreadReqAttrGetstack_PreDesc_StackSize,
  NULL
};

#define STACK_ADDR_VALUE ( (void *) 0x1000 )

#define STALE_STACK_ADDR ( (void *) 0x2000 )

#define STACK_SIZE_VALUE ( (size_t) 0x4000 )

#define STALE_STACK_SIZE ( (size_t) -1 )

static void CPthreadReqAttrGetstack_Pre_Attr_Prepare(
  CPthreadReqAttrGetstack_Context *ctx,
  CPthreadReqAttrGetstack_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      pthread_attr_setstack( &ctx->attr_obj, STACK_ADDR_VALUE, STACK_SIZE_VALUE );
      break;
    }

    case CPthreadReqAttrGetstack_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrGetstack_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqAttrGetstack_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Pre_StackAddr_Prepare(
  CPthreadReqAttrGetstack_Context      *ctx,
  CPthreadReqAttrGetstack_Pre_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Pre_StackAddr_Valid: {
      /*
       * While the `stackaddr` parameter references an object of type `void *`.
       */
      ctx->stackaddr_value = STALE_STACK_ADDR;
      ctx->stackaddr = &ctx->stackaddr_value;
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
    case CPthreadReqAttrGetstack_Pre_StackSize_Valid: {
      /*
       * While the `stacksize` parameter references an object of type size_t.
       */
      ctx->stacksize_value = STALE_STACK_SIZE;
      ctx->stacksize = &ctx->stacksize_value;
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
    case CPthreadReqAttrGetstack_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_getstack() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrGetstack_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_getstack() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrGetstack_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Post_StackAddrObj_Check(
  CPthreadReqAttrGetstack_Context          *ctx,
  CPthreadReqAttrGetstack_Post_StackAddrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Post_StackAddrObj_Set: {
      /*
       * The value of the object referenced by the `stackaddr` parameter shall
       * be set to the thread stack address attribute of the thread attributes
       * object referenced by the `attr` parameter.
       */
      T_eq_ptr( ctx->stackaddr_value, STACK_ADDR_VALUE );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackAddrObj_Nop: {
      /*
       * The object referenced by the `stackaddr` parameter shall not be
       * modified.
       */
      T_eq_ptr( ctx->stackaddr_value, STALE_STACK_ADDR );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackAddrObj_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Post_StackSizeObj_Check(
  CPthreadReqAttrGetstack_Context          *ctx,
  CPthreadReqAttrGetstack_Post_StackSizeObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetstack_Post_StackSizeObj_Set: {
      /*
       * The value of the object referenced by the `stacksize` parameter shall
       * be set to the thread stack size attribute of the thread attributes
       * object referenced by the `attr` parameter.
       */
      T_eq_sz( ctx->stacksize_value, STACK_SIZE_VALUE );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackSizeObj_Nop: {
      /*
       * The object referenced by the `stacksize` parameter shall not be
       * modified.
       */
      T_eq_sz( ctx->stacksize_value, STALE_STACK_SIZE );
      break;
    }

    case CPthreadReqAttrGetstack_Post_StackSizeObj_NA:
      break;
  }
}

static void CPthreadReqAttrGetstack_Action(
  CPthreadReqAttrGetstack_Context *ctx
)
{
  ctx->retval = pthread_attr_getstack(
    ctx->attr,
    ctx->stackaddr,
    ctx->stacksize
  );
}

static const CPthreadReqAttrGetstack_Entry
CPthreadReqAttrGetstack_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_EINVAL,
    CPthreadReqAttrGetstack_Post_StackAddrObj_Nop,
    CPthreadReqAttrGetstack_Post_StackSizeObj_NA },
  { 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_EINVAL,
    CPthreadReqAttrGetstack_Post_StackAddrObj_NA,
    CPthreadReqAttrGetstack_Post_StackSizeObj_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_EINVAL,
    CPthreadReqAttrGetstack_Post_StackAddrObj_NA,
    CPthreadReqAttrGetstack_Post_StackSizeObj_NA },
  { 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_EINVAL,
    CPthreadReqAttrGetstack_Post_StackAddrObj_Nop,
    CPthreadReqAttrGetstack_Post_StackSizeObj_Nop },
  { 0, 0, 0, 0, CPthreadReqAttrGetstack_Post_Status_Zero,
    CPthreadReqAttrGetstack_Post_StackAddrObj_Set,
    CPthreadReqAttrGetstack_Post_StackSizeObj_Set }
};

static const uint8_t
CPthreadReqAttrGetstack_Map[] = {
  4, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2
};

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
  return CPthreadReqAttrGetstack_Entries[
    CPthreadReqAttrGetstack_Map[ index ]
  ];
}

static void CPthreadReqAttrGetstack_TestVariant(
  CPthreadReqAttrGetstack_Context *ctx
)
{
  CPthreadReqAttrGetstack_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetstack_Pre_StackAddr_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrGetstack_Pre_StackSize_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqAttrGetstack_Action( ctx );
  CPthreadReqAttrGetstack_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrGetstack_Post_StackAddrObj_Check(
    ctx,
    ctx->Map.entry.Post_StackAddrObj
  );
  CPthreadReqAttrGetstack_Post_StackSizeObj_Check(
    ctx,
    ctx->Map.entry.Post_StackSizeObj
  );
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
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrGetstack_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrGetstack_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrGetstack_Pre_StackAddr_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrGetstack_Pre_StackAddr_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CPthreadReqAttrGetstack_Pre_StackSize_Valid;
        ctx->Map.pcs[ 2 ] < CPthreadReqAttrGetstack_Pre_StackSize_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrGetstack_PopEntry( ctx );
        CPthreadReqAttrGetstack_TestVariant( ctx );
      }
    }
  }
}

/** @} */
