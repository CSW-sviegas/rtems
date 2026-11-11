/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondInit
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <rtems/posix/condimpl.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondInit spec:/c/pthread/req/cond-init
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondInit_Pre_Cond_Obj,
  CPthreadReqCondInit_Pre_Cond_Null,
  CPthreadReqCondInit_Pre_Cond_NA
} CPthreadReqCondInit_Pre_Cond;

typedef enum {
  CPthreadReqCondInit_Pre_Attr_Obj,
  CPthreadReqCondInit_Pre_Attr_Null,
  CPthreadReqCondInit_Pre_Attr_NA
} CPthreadReqCondInit_Pre_Attr;

typedef enum {
  CPthreadReqCondInit_Pre_Initialized_Yes,
  CPthreadReqCondInit_Pre_Initialized_No,
  CPthreadReqCondInit_Pre_Initialized_NA
} CPthreadReqCondInit_Pre_Initialized;

typedef enum {
  CPthreadReqCondInit_Pre_Clock_Realtime,
  CPthreadReqCondInit_Pre_Clock_Monotonic,
  CPthreadReqCondInit_Pre_Clock_NA
} CPthreadReqCondInit_Pre_Clock;

typedef enum {
  CPthreadReqCondInit_Pre_Pshared_Private,
  CPthreadReqCondInit_Pre_Pshared_Shared,
  CPthreadReqCondInit_Pre_Pshared_Invalid,
  CPthreadReqCondInit_Pre_Pshared_NA
} CPthreadReqCondInit_Pre_Pshared;

typedef enum {
  CPthreadReqCondInit_Post_Status_Ok,
  CPthreadReqCondInit_Post_Status_Inval,
  CPthreadReqCondInit_Post_Status_NA
} CPthreadReqCondInit_Post_Status;

typedef enum {
  CPthreadReqCondInit_Post_State_Realtime,
  CPthreadReqCondInit_Post_State_Monotonic,
  CPthreadReqCondInit_Post_State_Nop,
  CPthreadReqCondInit_Post_State_NA
} CPthreadReqCondInit_Post_State;

typedef enum {
  CPthreadReqCondInit_Post_Binding_None,
  CPthreadReqCondInit_Post_Binding_Nop,
  CPthreadReqCondInit_Post_Binding_NA
} CPthreadReqCondInit_Post_Binding;

typedef enum {
  CPthreadReqCondInit_Post_Waiters_None,
  CPthreadReqCondInit_Post_Waiters_Nop,
  CPthreadReqCondInit_Post_Waiters_NA
} CPthreadReqCondInit_Post_Waiters;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Cond_NA : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_Clock_NA : 1;
  uint16_t Pre_Pshared_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_State : 2;
  uint16_t Post_Binding : 2;
  uint16_t Post_Waiters : 2;
} CPthreadReqCondInit_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/cond-init test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `cond` parameter value.
   */
  pthread_cond_t *cond;

  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_condattr_t *attr;

  /**
   * @brief This member contains the return value of the pthread_cond_init()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the condition variable object.
   */
  pthread_cond_t cond_obj;

  /**
   * @brief This member contains a copy of the condition variable object before
   *   the call.
   */
  pthread_cond_t cond_before;

  /**
   * @brief This member contains the condition variable attributes object.
   */
  pthread_condattr_t attr_obj;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 5 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    CPthreadReqCondInit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondInit_Context;

static CPthreadReqCondInit_Context CPthreadReqCondInit_Instance;

static const char *const CPthreadReqCondInit_PreDesc_Cond[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondInit_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondInit_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCondInit_PreDesc_Clock[] =
  { "Realtime", "Monotonic", "NA" };

static const char *const CPthreadReqCondInit_PreDesc_Pshared[] =
  { "Private", "Shared", "Invalid", "NA" };

static const char *const *const CPthreadReqCondInit_PreDesc[] = {
  CPthreadReqCondInit_PreDesc_Cond,
  CPthreadReqCondInit_PreDesc_Attr,
  CPthreadReqCondInit_PreDesc_Initialized,
  CPthreadReqCondInit_PreDesc_Clock,
  CPthreadReqCondInit_PreDesc_Pshared,
  NULL
};

#define PSHARED_INVALID 0x7fffffff

static unsigned long InitializedFlags( const pthread_cond_t *cond )
{
  return ( (uintptr_t) cond ^ POSIX_CONDITION_VARIABLES_MAGIC ) &
         ~POSIX_CONDITION_VARIABLES_FLAGS_MASK;
}

static void CPthreadReqCondInit_Pre_Cond_Prepare(
  CPthreadReqCondInit_Context *ctx,
  CPthreadReqCondInit_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Pre_Cond_Obj: {
      /*
       * While the `cond` parameter references an object of type
       * pthread_cond_t.
       */
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondInit_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondInit_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondInit_Pre_Attr_Prepare(
  CPthreadReqCondInit_Context *ctx,
  CPthreadReqCondInit_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_condattr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqCondInit_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondInit_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondInit_Pre_Initialized_Prepare(
  CPthreadReqCondInit_Context        *ctx,
  CPthreadReqCondInit_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * condition variable attributes object.
       */
      T_eno_success( pthread_condattr_init( &ctx->attr_obj ) );
      break;
    }

    case CPthreadReqCondInit_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized condition variable attributes object.
       */
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = 0;
      break;
    }

    case CPthreadReqCondInit_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqCondInit_Pre_Clock_Prepare(
  CPthreadReqCondInit_Context  *ctx,
  CPthreadReqCondInit_Pre_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Pre_Clock_Realtime: {
      /*
       * While the condition variable clock attribute of the object referenced
       * by the `attr` parameter is CLOCK_REALTIME.
       */
      ctx->attr_obj.clock = CLOCK_REALTIME;
      break;
    }

    case CPthreadReqCondInit_Pre_Clock_Monotonic: {
      /*
       * While the condition variable clock attribute of the object referenced
       * by the `attr` parameter is CLOCK_MONOTONIC.
       */
      ctx->attr_obj.clock = CLOCK_MONOTONIC;
      break;
    }

    case CPthreadReqCondInit_Pre_Clock_NA:
      break;
  }
}

static void CPthreadReqCondInit_Pre_Pshared_Prepare(
  CPthreadReqCondInit_Context    *ctx,
  CPthreadReqCondInit_Pre_Pshared state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Pre_Pshared_Private: {
      /*
       * While the process-shared attribute of the object referenced by the
       * `attr` parameter is PTHREAD_PROCESS_PRIVATE.
       */
      ctx->attr_obj.process_shared = PTHREAD_PROCESS_PRIVATE;
      break;
    }

    case CPthreadReqCondInit_Pre_Pshared_Shared: {
      /*
       * While the process-shared attribute of the object referenced by the
       * `attr` parameter is PTHREAD_PROCESS_SHARED.
       */
      ctx->attr_obj.process_shared = PTHREAD_PROCESS_SHARED;
      break;
    }

    case CPthreadReqCondInit_Pre_Pshared_Invalid: {
      /*
       * While the process-shared attribute of the object referenced by the
       * `attr` parameter is neither PTHREAD_PROCESS_PRIVATE nor
       * PTHREAD_PROCESS_SHARED.
       */
      ctx->attr_obj.process_shared = PSHARED_INVALID;
      break;
    }

    case CPthreadReqCondInit_Pre_Pshared_NA:
      break;
  }
}

static void CPthreadReqCondInit_Post_Status_Check(
  CPthreadReqCondInit_Context    *ctx,
  CPthreadReqCondInit_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Post_Status_Ok: {
      /*
       * The return value of pthread_cond_init() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondInit_Post_Status_Inval: {
      /*
       * The return value of pthread_cond_init() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondInit_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondInit_Post_State_Check(
  CPthreadReqCondInit_Context   *ctx,
  CPthreadReqCondInit_Post_State state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Post_State_Realtime: {
      /*
       * The object referenced by the `cond` parameter shall be an initialized
       * condition variable with the condition variable clock attribute
       * CLOCK_REALTIME.
       */
      T_eq_ulong( ctx->cond_obj._flags, InitializedFlags( &ctx->cond_obj ) );
      break;
    }

    case CPthreadReqCondInit_Post_State_Monotonic: {
      /*
       * The object referenced by the `cond` parameter shall be an initialized
       * condition variable with the condition variable clock attribute
       * CLOCK_MONOTONIC.
       */
      T_eq_ulong(
        ctx->cond_obj._flags,
        InitializedFlags( &ctx->cond_obj ) |
          POSIX_CONDITION_VARIABLES_CLOCK_MONOTONIC
      );
      break;
    }

    case CPthreadReqCondInit_Post_State_Nop: {
      /*
       * Whether the object referenced by the `cond` parameter is an
       * initialized condition variable and its condition variable clock
       * attribute shall not be modified.
       */
      T_eq_ulong( ctx->cond_obj._flags, ctx->cond_before._flags );
      break;
    }

    case CPthreadReqCondInit_Post_State_NA:
      break;
  }
}

static void CPthreadReqCondInit_Post_Binding_Check(
  CPthreadReqCondInit_Context     *ctx,
  CPthreadReqCondInit_Post_Binding state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Post_Binding_None: {
      /*
       * The condition variable referenced by the `cond` parameter shall be
       * bound to no mutex.
       */
      T_null( ctx->cond_obj._mutex );
      break;
    }

    case CPthreadReqCondInit_Post_Binding_Nop: {
      /*
       * The binding of the object referenced by the `cond` parameter to a
       * mutex shall not be modified.
       */
      T_eq_ptr( ctx->cond_obj._mutex, ctx->cond_before._mutex );
      break;
    }

    case CPthreadReqCondInit_Post_Binding_NA:
      break;
  }
}

static void CPthreadReqCondInit_Post_Waiters_Check(
  CPthreadReqCondInit_Context     *ctx,
  CPthreadReqCondInit_Post_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqCondInit_Post_Waiters_None: {
      /*
       * No thread shall be blocked on the condition variable referenced by the
       * `cond` parameter.
       */
      T_null( ctx->cond_obj._Queue._heads );
      break;
    }

    case CPthreadReqCondInit_Post_Waiters_Nop: {
      /*
       * The record of the threads blocked on the object referenced by the
       * `cond` parameter shall not be modified.
       */
      T_eq_ptr( ctx->cond_obj._Queue._heads, ctx->cond_before._Queue._heads );
      break;
    }

    case CPthreadReqCondInit_Post_Waiters_NA:
      break;
  }
}

static void CPthreadReqCondInit_Prepare( CPthreadReqCondInit_Context *ctx )
{
  memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
}

static void CPthreadReqCondInit_Action( CPthreadReqCondInit_Context *ctx )
{
  memcpy( &ctx->cond_before, &ctx->cond_obj, sizeof( ctx->cond_before ) );
  ctx->status = pthread_cond_init( ctx->cond, ctx->attr );
}

/* clang-format off */

static const CPthreadReqCondInit_Entry
CPthreadReqCondInit_Entries[] = {
  { 1, 0, 0, 0, 0, 0, CPthreadReqCondInit_Post_Status_NA,
    CPthreadReqCondInit_Post_State_NA, CPthreadReqCondInit_Post_Binding_NA,
    CPthreadReqCondInit_Post_Waiters_NA },
  { 0, 0, 0, 0, 0, 0, CPthreadReqCondInit_Post_Status_Inval,
    CPthreadReqCondInit_Post_State_NA, CPthreadReqCondInit_Post_Binding_NA,
    CPthreadReqCondInit_Post_Waiters_NA },
  { 0, 0, 0, 0, 0, 0, CPthreadReqCondInit_Post_Status_Inval,
    CPthreadReqCondInit_Post_State_Nop, CPthreadReqCondInit_Post_Binding_Nop,
    CPthreadReqCondInit_Post_Waiters_Nop },
  { 0, 0, 0, 0, 0, 0, CPthreadReqCondInit_Post_Status_Ok,
    CPthreadReqCondInit_Post_State_Realtime,
    CPthreadReqCondInit_Post_Binding_None,
    CPthreadReqCondInit_Post_Waiters_None },
  { 0, 0, 0, 0, 0, 0, CPthreadReqCondInit_Post_Status_Ok,
    CPthreadReqCondInit_Post_State_Monotonic,
    CPthreadReqCondInit_Post_Binding_None,
    CPthreadReqCondInit_Post_Waiters_None },
  { 0, 0, 0, 1, 1, 1, CPthreadReqCondInit_Post_Status_Ok,
    CPthreadReqCondInit_Post_State_Realtime,
    CPthreadReqCondInit_Post_Binding_None,
    CPthreadReqCondInit_Post_Waiters_None },
  { 0, 0, 0, 1, 1, 1, CPthreadReqCondInit_Post_Status_Inval,
    CPthreadReqCondInit_Post_State_NA, CPthreadReqCondInit_Post_Binding_NA,
    CPthreadReqCondInit_Post_Waiters_NA }
};

static const uint8_t
CPthreadReqCondInit_Map[] = {
  3, 3, 2, 4, 4, 2, 2, 2, 2, 2, 2, 2, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondInit_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondInit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqCondInit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqCondInit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondInit_Scope,
  .initial_context = &CPthreadReqCondInit_Instance
};

static inline CPthreadReqCondInit_Entry CPthreadReqCondInit_PopEntry(
  CPthreadReqCondInit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondInit_Entries[ CPthreadReqCondInit_Map[ index ] ];
}

static void CPthreadReqCondInit_SetPreConditionStates(
  CPthreadReqCondInit_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondInit_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Clock_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqCondInit_Pre_Clock_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Pshared_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqCondInit_Pre_Pshared_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }
}

static void CPthreadReqCondInit_TestVariant( CPthreadReqCondInit_Context *ctx )
{
  CPthreadReqCondInit_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondInit_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondInit_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondInit_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqCondInit_Pre_Pshared_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqCondInit_Action( ctx );
  CPthreadReqCondInit_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondInit_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  CPthreadReqCondInit_Post_Binding_Check( ctx, ctx->Map.entry.Post_Binding );
  CPthreadReqCondInit_Post_Waiters_Check( ctx, ctx->Map.entry.Post_Waiters );
}

/**
 * @fn void T_case_body_CPthreadReqCondInit( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqCondInit, &CPthreadReqCondInit_Fixture )
{
  CPthreadReqCondInit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondInit_Pre_Cond_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondInit_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondInit_Pre_Attr_Obj;
      ctx->Map.pci[ 1 ] < CPthreadReqCondInit_Pre_Attr_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondInit_Pre_Initialized_Yes;
        ctx->Map.pci[ 2 ] < CPthreadReqCondInit_Pre_Initialized_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqCondInit_Pre_Clock_Realtime;
          ctx->Map.pci[ 3 ] < CPthreadReqCondInit_Pre_Clock_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqCondInit_Pre_Pshared_Private;
            ctx->Map.pci[ 4 ] < CPthreadReqCondInit_Pre_Pshared_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = CPthreadReqCondInit_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            CPthreadReqCondInit_SetPreConditionStates( ctx );
            CPthreadReqCondInit_Prepare( ctx );
            CPthreadReqCondInit_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
