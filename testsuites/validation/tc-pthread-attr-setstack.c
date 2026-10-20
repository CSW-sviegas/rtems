/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetstack
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
 * @defgroup CPthreadReqAttrSetstack spec:/c/pthread/req/attr-setstack
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetstack_Pre_Attr_Valid,
  CPthreadReqAttrSetstack_Pre_Attr_Null,
  CPthreadReqAttrSetstack_Pre_Attr_Invalid,
  CPthreadReqAttrSetstack_Pre_Attr_NA
} CPthreadReqAttrSetstack_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetstack_Pre_StackAddr_Valid,
  CPthreadReqAttrSetstack_Pre_StackAddr_NA
} CPthreadReqAttrSetstack_Pre_StackAddr;

typedef enum {
  CPthreadReqAttrSetstack_Pre_StackSize_Valid,
  CPthreadReqAttrSetstack_Pre_StackSize_NA
} CPthreadReqAttrSetstack_Pre_StackSize;

typedef enum {
  CPthreadReqAttrSetstack_Post_Status_Zero,
  CPthreadReqAttrSetstack_Post_Status_EINVAL,
  CPthreadReqAttrSetstack_Post_Status_NA
} CPthreadReqAttrSetstack_Post_Status;

typedef enum {
  CPthreadReqAttrSetstack_Post_StackAddrObj_Set,
  CPthreadReqAttrSetstack_Post_StackAddrObj_Nop,
  CPthreadReqAttrSetstack_Post_StackAddrObj_NA
} CPthreadReqAttrSetstack_Post_StackAddrObj;

typedef enum {
  CPthreadReqAttrSetstack_Post_StackSizeObj_Set,
  CPthreadReqAttrSetstack_Post_StackSizeObj_Nop,
  CPthreadReqAttrSetstack_Post_StackSizeObj_NA
} CPthreadReqAttrSetstack_Post_StackSizeObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_StackAddr_NA : 1;
  uint16_t Pre_StackSize_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_StackAddrObj : 2;
  uint16_t Post_StackSizeObj : 2;
} CPthreadReqAttrSetstack_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setstack test case.
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
   * @brief This member specifies the `stacksize` parameter value.
   */
  size_t stacksize;

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
    CPthreadReqAttrSetstack_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetstack_Context;

static CPthreadReqAttrSetstack_Context
  CPthreadReqAttrSetstack_Instance;

static const char * const CPthreadReqAttrSetstack_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqAttrSetstack_PreDesc_StackAddr[] = {
  "Valid",
  "NA"
};

static const char * const CPthreadReqAttrSetstack_PreDesc_StackSize[] = {
  "Valid",
  "NA"
};

static const char * const * const CPthreadReqAttrSetstack_PreDesc[] = {
  CPthreadReqAttrSetstack_PreDesc_Attr,
  CPthreadReqAttrSetstack_PreDesc_StackAddr,
  CPthreadReqAttrSetstack_PreDesc_StackSize,
  NULL
};

#define STACK_ADDR_VALUE ( (void *) 0x1000 )

#define STALE_STACK_ADDR ( (void *) 0x2000 )

#define STACK_SIZE_VALUE ( (size_t) 0x4000 )

#define STALE_STACK_SIZE ( (size_t) -1 )

static void CPthreadReqAttrSetstack_Pre_Attr_Prepare(
  CPthreadReqAttrSetstack_Context *ctx,
  CPthreadReqAttrSetstack_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstack_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references an initialized thread attributes
       * object.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_attr_init( &ctx->attr_obj );
      ctx->attr_obj.stackaddr = STALE_STACK_ADDR;
      ctx->attr_obj.stacksize = STALE_STACK_SIZE;
      break;
    }

    case CPthreadReqAttrSetstack_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetstack_Pre_Attr_Invalid: {
      /*
       * While the thread attributes object referenced by the `attr` parameter
       * is not initialized.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.stackaddr = STALE_STACK_ADDR;
      ctx->attr_obj.stacksize = STALE_STACK_SIZE;
      break;
    }

    case CPthreadReqAttrSetstack_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetstack_Pre_StackAddr_Prepare(
  CPthreadReqAttrSetstack_Context      *ctx,
  CPthreadReqAttrSetstack_Pre_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstack_Pre_StackAddr_Valid: {
      /*
       * While the `stackaddr` parameter specifies a stack address.
       */
      ctx->stackaddr = STACK_ADDR_VALUE;
      break;
    }

    case CPthreadReqAttrSetstack_Pre_StackAddr_NA:
      break;
  }
}

static void CPthreadReqAttrSetstack_Pre_StackSize_Prepare(
  CPthreadReqAttrSetstack_Context      *ctx,
  CPthreadReqAttrSetstack_Pre_StackSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstack_Pre_StackSize_Valid: {
      /*
       * While the `stacksize` parameter specifies a stack size.
       */
      ctx->stacksize = STACK_SIZE_VALUE;
      break;
    }

    case CPthreadReqAttrSetstack_Pre_StackSize_NA:
      break;
  }
}

static void CPthreadReqAttrSetstack_Post_Status_Check(
  CPthreadReqAttrSetstack_Context    *ctx,
  CPthreadReqAttrSetstack_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstack_Post_Status_Zero: {
      /*
       * The return value of pthread_attr_setstack() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqAttrSetstack_Post_Status_EINVAL: {
      /*
       * The return value of pthread_attr_setstack() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqAttrSetstack_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetstack_Post_StackAddrObj_Check(
  CPthreadReqAttrSetstack_Context          *ctx,
  CPthreadReqAttrSetstack_Post_StackAddrObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstack_Post_StackAddrObj_Set: {
      /*
       * The value of the thread stack address attribute of the thread
       * attributes object referenced by the `attr` parameter shall be set to
       * the address specified by the `stackaddr` parameter.
       */
      void  *stackaddr;
      size_t stacksize;
      int    eno;

      eno = pthread_attr_getstack( &ctx->attr_obj, &stackaddr, &stacksize );
      T_eno_success( eno );
      T_eq_ptr( stackaddr, STACK_ADDR_VALUE );
      break;
    }

    case CPthreadReqAttrSetstack_Post_StackAddrObj_Nop: {
      /*
       * The thread stack address attribute of the thread attributes object
       * referenced by the `attr` parameter shall not be modified.
       */
      T_eq_ptr( ctx->attr_obj.stackaddr, STALE_STACK_ADDR );
      break;
    }

    case CPthreadReqAttrSetstack_Post_StackAddrObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetstack_Post_StackSizeObj_Check(
  CPthreadReqAttrSetstack_Context          *ctx,
  CPthreadReqAttrSetstack_Post_StackSizeObj state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstack_Post_StackSizeObj_Set: {
      /*
       * The value of the thread stack size attribute of the thread attributes
       * object referenced by the `attr` parameter shall be set to the size
       * specified by the `stacksize` parameter.
       */
      void  *stackaddr;
      size_t stacksize;
      int    eno;

      eno = pthread_attr_getstack( &ctx->attr_obj, &stackaddr, &stacksize );
      T_eno_success( eno );
      T_eq_sz( stacksize, STACK_SIZE_VALUE );
      break;
    }

    case CPthreadReqAttrSetstack_Post_StackSizeObj_Nop: {
      /*
       * The thread stack size attribute of the thread attributes object
       * referenced by the `attr` parameter shall not be modified.
       */
      T_eq_sz( ctx->attr_obj.stacksize, STALE_STACK_SIZE );
      break;
    }

    case CPthreadReqAttrSetstack_Post_StackSizeObj_NA:
      break;
  }
}

static void CPthreadReqAttrSetstack_Action(
  CPthreadReqAttrSetstack_Context *ctx
)
{
  ctx->retval = pthread_attr_setstack(
    ctx->attr,
    ctx->stackaddr,
    ctx->stacksize
  );
}

static const CPthreadReqAttrSetstack_Entry
CPthreadReqAttrSetstack_Entries[] = {
  { 0, 0, 0, 0, CPthreadReqAttrSetstack_Post_Status_Zero,
    CPthreadReqAttrSetstack_Post_StackAddrObj_Set,
    CPthreadReqAttrSetstack_Post_StackSizeObj_Set },
  { 0, 0, 0, 0, CPthreadReqAttrSetstack_Post_Status_EINVAL,
    CPthreadReqAttrSetstack_Post_StackAddrObj_NA,
    CPthreadReqAttrSetstack_Post_StackSizeObj_NA },
  { 0, 0, 0, 0, CPthreadReqAttrSetstack_Post_Status_EINVAL,
    CPthreadReqAttrSetstack_Post_StackAddrObj_Nop,
    CPthreadReqAttrSetstack_Post_StackSizeObj_Nop }
};

static const uint8_t
CPthreadReqAttrSetstack_Map[] = {
  0, 1, 2
};

static size_t CPthreadReqAttrSetstack_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqAttrSetstack_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetstack_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetstack_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetstack_Scope,
  .initial_context = &CPthreadReqAttrSetstack_Instance
};

static inline CPthreadReqAttrSetstack_Entry CPthreadReqAttrSetstack_PopEntry(
  CPthreadReqAttrSetstack_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetstack_Entries[
    CPthreadReqAttrSetstack_Map[ index ]
  ];
}

static void CPthreadReqAttrSetstack_TestVariant(
  CPthreadReqAttrSetstack_Context *ctx
)
{
  CPthreadReqAttrSetstack_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetstack_Pre_StackAddr_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqAttrSetstack_Pre_StackSize_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqAttrSetstack_Action( ctx );
  CPthreadReqAttrSetstack_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqAttrSetstack_Post_StackAddrObj_Check(
    ctx,
    ctx->Map.entry.Post_StackAddrObj
  );
  CPthreadReqAttrSetstack_Post_StackSizeObj_Check(
    ctx,
    ctx->Map.entry.Post_StackSizeObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetstack( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetstack,
  &CPthreadReqAttrSetstack_Fixture
)
{
  CPthreadReqAttrSetstack_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetstack_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetstack_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetstack_Pre_StackAddr_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqAttrSetstack_Pre_StackAddr_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CPthreadReqAttrSetstack_Pre_StackSize_Valid;
        ctx->Map.pcs[ 2 ] < CPthreadReqAttrSetstack_Pre_StackSize_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CPthreadReqAttrSetstack_PopEntry( ctx );
        CPthreadReqAttrSetstack_TestVariant( ctx );
      }
    }
  }
}

/** @} */
