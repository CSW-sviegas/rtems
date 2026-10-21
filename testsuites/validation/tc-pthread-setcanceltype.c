/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSetcanceltype
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
#include <limits.h>
#include <pthread.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSetcanceltype spec:/c/pthread/req/setcanceltype
 *
 * @{
 */

typedef enum {
  CPthreadReqSetcanceltype_Pre_Context_Thread,
  CPthreadReqSetcanceltype_Pre_Context_Interrupt,
  CPthreadReqSetcanceltype_Pre_Context_NA
} CPthreadReqSetcanceltype_Pre_Context;

typedef enum {
  CPthreadReqSetcanceltype_Pre_Type_Deferred,
  CPthreadReqSetcanceltype_Pre_Type_Async,
  CPthreadReqSetcanceltype_Pre_Type_Invalid,
  CPthreadReqSetcanceltype_Pre_Type_NA
} CPthreadReqSetcanceltype_Pre_Type;

typedef enum {
  CPthreadReqSetcanceltype_Pre_OldType_Valid,
  CPthreadReqSetcanceltype_Pre_OldType_Null,
  CPthreadReqSetcanceltype_Pre_OldType_NA
} CPthreadReqSetcanceltype_Pre_OldType;

typedef enum {
  CPthreadReqSetcanceltype_Pre_Previous_Deferred,
  CPthreadReqSetcanceltype_Pre_Previous_Async,
  CPthreadReqSetcanceltype_Pre_Previous_NA
} CPthreadReqSetcanceltype_Pre_Previous;

typedef enum {
  CPthreadReqSetcanceltype_Post_Status_Zero,
  CPthreadReqSetcanceltype_Post_Status_EPROTO,
  CPthreadReqSetcanceltype_Post_Status_EINVAL,
  CPthreadReqSetcanceltype_Post_Status_NA
} CPthreadReqSetcanceltype_Post_Status;

typedef enum {
  CPthreadReqSetcanceltype_Post_Type_Deferred,
  CPthreadReqSetcanceltype_Post_Type_Async,
  CPthreadReqSetcanceltype_Post_Type_Nop,
  CPthreadReqSetcanceltype_Post_Type_NA
} CPthreadReqSetcanceltype_Post_Type;

typedef enum {
  CPthreadReqSetcanceltype_Post_OldType_Deferred,
  CPthreadReqSetcanceltype_Post_OldType_Async,
  CPthreadReqSetcanceltype_Post_OldType_Nop,
  CPthreadReqSetcanceltype_Post_OldType_NA
} CPthreadReqSetcanceltype_Post_OldType;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Context_NA : 1;
  uint16_t Pre_Type_NA : 1;
  uint16_t Pre_OldType_NA : 1;
  uint16_t Pre_Previous_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Type : 2;
  uint16_t Post_OldType : 2;
} CPthreadReqSetcanceltype_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/setcanceltype test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `type` parameter value.
   */
  int type;

  /**
   * @brief This member specifies the `oldtype` parameter value.
   */
  int *oldtype;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the object referenced by the `oldtype`
   *   parameter in case it is not NULL.
   */
  int old_type;

  /**
   * @brief This member specifies the thread cancel type attribute of the
   *   calling thread immediately before the pthread_setcanceltype() call.
   */
  int previous_type;

  /**
   * @brief If this member is true, then the pthread_setcanceltype() call shall
   *   be made from within interrupt context.
   */
  bool in_isr;

  /**
   * @brief This member contains the thread cancel type attribute of the
   *   calling thread at the begin of the test case.
   */
  int initial_type;

  struct {
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
    CPthreadReqSetcanceltype_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSetcanceltype_Context;

static CPthreadReqSetcanceltype_Context
  CPthreadReqSetcanceltype_Instance;

static const char * const CPthreadReqSetcanceltype_PreDesc_Context[] = {
  "Thread",
  "Interrupt",
  "NA"
};

static const char * const CPthreadReqSetcanceltype_PreDesc_Type[] = {
  "Deferred",
  "Async",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqSetcanceltype_PreDesc_OldType[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CPthreadReqSetcanceltype_PreDesc_Previous[] = {
  "Deferred",
  "Async",
  "NA"
};

static const char * const * const CPthreadReqSetcanceltype_PreDesc[] = {
  CPthreadReqSetcanceltype_PreDesc_Context,
  CPthreadReqSetcanceltype_PreDesc_Type,
  CPthreadReqSetcanceltype_PreDesc_OldType,
  CPthreadReqSetcanceltype_PreDesc_Previous,
  NULL
};

typedef CPthreadReqSetcanceltype_Context Context;

#define INVALID_TYPE -1

static int GetCancelType( void )
{
  int type;

  (void) pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, &type );
  (void) pthread_setcanceltype( type, NULL );

  return type;
}

static void Call( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->retval = pthread_setcanceltype( ctx->type, ctx->oldtype );
}

static void CPthreadReqSetcanceltype_Pre_Context_Prepare(
  CPthreadReqSetcanceltype_Context    *ctx,
  CPthreadReqSetcanceltype_Pre_Context state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_Context_Thread: {
      /*
       * While the pthread_setcanceltype() call is made from within thread
       * context.
       */
      ctx->in_isr = false;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Context_Interrupt: {
      /*
       * While the pthread_setcanceltype() call is made from within interrupt
       * context.
       */
      ctx->in_isr = true;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Context_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_Type_Prepare(
  CPthreadReqSetcanceltype_Context *ctx,
  CPthreadReqSetcanceltype_Pre_Type state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_Type_Deferred: {
      /*
       * While the `type` parameter is PTHREAD_CANCEL_DEFERRED.
       */
      ctx->type = PTHREAD_CANCEL_DEFERRED;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Type_Async: {
      /*
       * While the `type` parameter is PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Type_Invalid: {
      /*
       * While the `type` parameter is neither PTHREAD_CANCEL_DEFERRED nor
       * PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->type = INT_MAX;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Type_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_OldType_Prepare(
  CPthreadReqSetcanceltype_Context    *ctx,
  CPthreadReqSetcanceltype_Pre_OldType state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_OldType_Valid: {
      /*
       * While the `oldtype` parameter references an object of type `int`.
       */
      ctx->oldtype = &ctx->old_type;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_OldType_Null: {
      /*
       * While the `oldtype` parameter is NULL.
       */
      ctx->oldtype = NULL;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_OldType_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_Previous_Prepare(
  CPthreadReqSetcanceltype_Context     *ctx,
  CPthreadReqSetcanceltype_Pre_Previous state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_Previous_Deferred: {
      /*
       * While the thread cancel type attribute of the calling thread is
       * PTHREAD_CANCEL_DEFERRED immediately before the pthread_setcanceltype()
       * call.
       */
      ctx->previous_type = PTHREAD_CANCEL_DEFERRED;
      (void) pthread_setcanceltype( ctx->previous_type, NULL );
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Previous_Async: {
      /*
       * While the thread cancel type attribute of the calling thread is
       * PTHREAD_CANCEL_ASYNCHRONOUS immediately before the
       * pthread_setcanceltype() call.
       */
      ctx->previous_type = PTHREAD_CANCEL_ASYNCHRONOUS;
      (void) pthread_setcanceltype( ctx->previous_type, NULL );
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Previous_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_Status_Check(
  CPthreadReqSetcanceltype_Context    *ctx,
  CPthreadReqSetcanceltype_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_Status_Zero: {
      /*
       * The return value of pthread_setcanceltype() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Status_EPROTO: {
      /*
       * The return value of pthread_setcanceltype() shall be equal to EPROTO.
       */
      T_eq_int( ctx->retval, EPROTO );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Status_EINVAL: {
      /*
       * The return value of pthread_setcanceltype() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_Type_Check(
  CPthreadReqSetcanceltype_Context  *ctx,
  CPthreadReqSetcanceltype_Post_Type state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_Type_Deferred: {
      /*
       * The thread cancel type attribute of the calling thread shall be
       * PTHREAD_CANCEL_DEFERRED after the return of the
       * pthread_setcanceltype() call.
       */
      T_eq_int( GetCancelType(), PTHREAD_CANCEL_DEFERRED );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Type_Async: {
      /*
       * The thread cancel type attribute of the calling thread shall be
       * PTHREAD_CANCEL_ASYNCHRONOUS after the return of the
       * pthread_setcanceltype() call.
       */
      T_eq_int( GetCancelType(), PTHREAD_CANCEL_ASYNCHRONOUS );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Type_Nop: {
      /*
       * The thread cancel type attribute of the calling thread shall not be
       * changed by the pthread_setcanceltype() call.
       */
      T_eq_int( GetCancelType(), ctx->previous_type );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Type_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_OldType_Check(
  CPthreadReqSetcanceltype_Context     *ctx,
  CPthreadReqSetcanceltype_Post_OldType state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_OldType_Deferred: {
      /*
       * The value of the object referenced by the `oldtype` parameter shall be
       * set to PTHREAD_CANCEL_DEFERRED.
       */
      T_eq_int( ctx->old_type, PTHREAD_CANCEL_DEFERRED );
      break;
    }

    case CPthreadReqSetcanceltype_Post_OldType_Async: {
      /*
       * The value of the object referenced by the `oldtype` parameter shall be
       * set to PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      T_eq_int( ctx->old_type, PTHREAD_CANCEL_ASYNCHRONOUS );
      break;
    }

    case CPthreadReqSetcanceltype_Post_OldType_Nop: {
      /*
       * Objects referenced by the `oldtype` parameter in past calls to
       * pthread_setcanceltype() shall not be accessed by the
       * pthread_setcanceltype() call.
       */
      T_eq_int( ctx->old_type, INVALID_TYPE );
      break;
    }

    case CPthreadReqSetcanceltype_Post_OldType_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Setup(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  (void) pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, &ctx->initial_type );
}

static void CPthreadReqSetcanceltype_Setup_Wrap( void *arg )
{
  CPthreadReqSetcanceltype_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSetcanceltype_Setup( ctx );
}

static void CPthreadReqSetcanceltype_Teardown(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  (void) pthread_setcanceltype( ctx->initial_type, NULL );
}

static void CPthreadReqSetcanceltype_Teardown_Wrap( void *arg )
{
  CPthreadReqSetcanceltype_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSetcanceltype_Teardown( ctx );
}

static void CPthreadReqSetcanceltype_Prepare(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  ctx->old_type = INVALID_TYPE;
}

static void CPthreadReqSetcanceltype_Action(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  if ( ctx->in_isr ) {
    CallWithinISR( Call, ctx );
  } else {
    ctx->retval = pthread_setcanceltype( ctx->type, ctx->oldtype );
  }
}

static const CPthreadReqSetcanceltype_Entry
CPthreadReqSetcanceltype_Entries[] = {
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_EPROTO,
    CPthreadReqSetcanceltype_Post_Type_Nop,
    CPthreadReqSetcanceltype_Post_OldType_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_EPROTO,
    CPthreadReqSetcanceltype_Post_Type_Nop,
    CPthreadReqSetcanceltype_Post_OldType_NA },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Zero,
    CPthreadReqSetcanceltype_Post_Type_Deferred,
    CPthreadReqSetcanceltype_Post_OldType_NA },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Zero,
    CPthreadReqSetcanceltype_Post_Type_Async,
    CPthreadReqSetcanceltype_Post_OldType_NA },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_EINVAL,
    CPthreadReqSetcanceltype_Post_Type_Nop,
    CPthreadReqSetcanceltype_Post_OldType_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_EINVAL,
    CPthreadReqSetcanceltype_Post_Type_Nop,
    CPthreadReqSetcanceltype_Post_OldType_NA },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Zero,
    CPthreadReqSetcanceltype_Post_Type_Deferred,
    CPthreadReqSetcanceltype_Post_OldType_Deferred },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Zero,
    CPthreadReqSetcanceltype_Post_Type_Deferred,
    CPthreadReqSetcanceltype_Post_OldType_Async },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Zero,
    CPthreadReqSetcanceltype_Post_Type_Async,
    CPthreadReqSetcanceltype_Post_OldType_Deferred },
  { 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Zero,
    CPthreadReqSetcanceltype_Post_Type_Async,
    CPthreadReqSetcanceltype_Post_OldType_Async }
};

static const uint8_t
CPthreadReqSetcanceltype_Map[] = {
  6, 7, 2, 2, 8, 9, 3, 3, 4, 4, 5, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1
};

static size_t CPthreadReqSetcanceltype_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSetcanceltype_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqSetcanceltype_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqSetcanceltype_Fixture = {
  .setup = CPthreadReqSetcanceltype_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqSetcanceltype_Teardown_Wrap,
  .scope = CPthreadReqSetcanceltype_Scope,
  .initial_context = &CPthreadReqSetcanceltype_Instance
};

static inline CPthreadReqSetcanceltype_Entry CPthreadReqSetcanceltype_PopEntry(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSetcanceltype_Entries[
    CPthreadReqSetcanceltype_Map[ index ]
  ];
}

static void CPthreadReqSetcanceltype_TestVariant(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  CPthreadReqSetcanceltype_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSetcanceltype_Pre_Type_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSetcanceltype_Pre_OldType_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqSetcanceltype_Pre_Previous_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqSetcanceltype_Action( ctx );
  CPthreadReqSetcanceltype_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqSetcanceltype_Post_Type_Check( ctx, ctx->Map.entry.Post_Type );
  CPthreadReqSetcanceltype_Post_OldType_Check(
    ctx,
    ctx->Map.entry.Post_OldType
  );
}

/**
 * @fn void T_case_body_CPthreadReqSetcanceltype( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqSetcanceltype,
  &CPthreadReqSetcanceltype_Fixture
)
{
  CPthreadReqSetcanceltype_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSetcanceltype_Pre_Context_Thread;
    ctx->Map.pcs[ 0 ] < CPthreadReqSetcanceltype_Pre_Context_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqSetcanceltype_Pre_Type_Deferred;
      ctx->Map.pcs[ 1 ] < CPthreadReqSetcanceltype_Pre_Type_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CPthreadReqSetcanceltype_Pre_OldType_Valid;
        ctx->Map.pcs[ 2 ] < CPthreadReqSetcanceltype_Pre_OldType_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CPthreadReqSetcanceltype_Pre_Previous_Deferred;
          ctx->Map.pcs[ 3 ] < CPthreadReqSetcanceltype_Pre_Previous_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqSetcanceltype_PopEntry( ctx );
          CPthreadReqSetcanceltype_Prepare( ctx );
          CPthreadReqSetcanceltype_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
