/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSetcancelstate
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
 * @defgroup CPthreadReqSetcancelstate spec:/c/pthread/req/setcancelstate
 *
 * @{
 */

typedef enum {
  CPthreadReqSetcancelstate_Pre_Context_Thread,
  CPthreadReqSetcancelstate_Pre_Context_Interrupt,
  CPthreadReqSetcancelstate_Pre_Context_NA
} CPthreadReqSetcancelstate_Pre_Context;

typedef enum {
  CPthreadReqSetcancelstate_Pre_NewState_Enabled,
  CPthreadReqSetcancelstate_Pre_NewState_Disabled,
  CPthreadReqSetcancelstate_Pre_NewState_Invalid,
  CPthreadReqSetcancelstate_Pre_NewState_NA
} CPthreadReqSetcancelstate_Pre_NewState;

typedef enum {
  CPthreadReqSetcancelstate_Pre_OldState_Valid,
  CPthreadReqSetcancelstate_Pre_OldState_Null,
  CPthreadReqSetcancelstate_Pre_OldState_NA
} CPthreadReqSetcancelstate_Pre_OldState;

typedef enum {
  CPthreadReqSetcancelstate_Pre_Previous_Enabled,
  CPthreadReqSetcancelstate_Pre_Previous_Disabled,
  CPthreadReqSetcancelstate_Pre_Previous_NA
} CPthreadReqSetcancelstate_Pre_Previous;

typedef enum {
  CPthreadReqSetcancelstate_Post_Status_Zero,
  CPthreadReqSetcancelstate_Post_Status_EINVAL,
  CPthreadReqSetcancelstate_Post_Status_EPROTO,
  CPthreadReqSetcancelstate_Post_Status_NA
} CPthreadReqSetcancelstate_Post_Status;

typedef enum {
  CPthreadReqSetcancelstate_Post_OldState_Set,
  CPthreadReqSetcancelstate_Post_OldState_Nop,
  CPthreadReqSetcancelstate_Post_OldState_NA
} CPthreadReqSetcancelstate_Post_OldState;

typedef enum {
  CPthreadReqSetcancelstate_Post_State_Enabled,
  CPthreadReqSetcancelstate_Post_State_Disabled,
  CPthreadReqSetcancelstate_Post_State_Nop,
  CPthreadReqSetcancelstate_Post_State_NA
} CPthreadReqSetcancelstate_Post_State;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Context_NA : 1;
  uint16_t Pre_NewState_NA : 1;
  uint16_t Pre_OldState_NA : 1;
  uint16_t Pre_Previous_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_OldState : 2;
  uint16_t Post_State : 2;
} CPthreadReqSetcancelstate_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/setcancelstate test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `state` parameter value.
   */
  int state;

  /**
   * @brief This member specifies the `oldstate` parameter value.
   */
  int *oldstate;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the object referenced by the `oldstate`
   *   parameter in case it is not NULL.
   */
  int oldstate_value;

  /**
   * @brief This member specifies the thread cancel state attribute of the
   *   calling thread immediately before the pthread_setcancelstate() call.
   */
  int previous;

  /**
   * @brief If this member is true, then the pthread_setcancelstate() call
   *   shall be made from within interrupt context.
   */
  bool in_isr;

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
    CPthreadReqSetcancelstate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSetcancelstate_Context;

static CPthreadReqSetcancelstate_Context
  CPthreadReqSetcancelstate_Instance;

static const char * const CPthreadReqSetcancelstate_PreDesc_Context[] = {
  "Thread",
  "Interrupt",
  "NA"
};

static const char * const CPthreadReqSetcancelstate_PreDesc_NewState[] = {
  "Enabled",
  "Disabled",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqSetcancelstate_PreDesc_OldState[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CPthreadReqSetcancelstate_PreDesc_Previous[] = {
  "Enabled",
  "Disabled",
  "NA"
};

static const char * const * const CPthreadReqSetcancelstate_PreDesc[] = {
  CPthreadReqSetcancelstate_PreDesc_Context,
  CPthreadReqSetcancelstate_PreDesc_NewState,
  CPthreadReqSetcancelstate_PreDesc_OldState,
  CPthreadReqSetcancelstate_PreDesc_Previous,
  NULL
};

typedef CPthreadReqSetcancelstate_Context Context;

#define INVALID_STATE -1

static int GetCancelState( void )
{
  int state;

  (void) pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, &state );
  (void) pthread_setcancelstate( state, NULL );

  return state;
}

static void Call( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->retval = pthread_setcancelstate( ctx->state, ctx->oldstate );
}

static void CPthreadReqSetcancelstate_Pre_Context_Prepare(
  CPthreadReqSetcancelstate_Context    *ctx,
  CPthreadReqSetcancelstate_Pre_Context state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_Context_Thread: {
      /*
       * While the pthread_setcancelstate() call is made from within thread
       * context.
       */
      ctx->in_isr = false;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_Context_Interrupt: {
      /*
       * While the pthread_setcancelstate() call is made from within interrupt
       * context.
       */
      ctx->in_isr = true;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_Context_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_NewState_Prepare(
  CPthreadReqSetcancelstate_Context     *ctx,
  CPthreadReqSetcancelstate_Pre_NewState state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_NewState_Enabled: {
      /*
       * While the `state` parameter is PTHREAD_CANCEL_ENABLE.
       */
      ctx->state = PTHREAD_CANCEL_ENABLE;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_NewState_Disabled: {
      /*
       * While the `state` parameter is PTHREAD_CANCEL_DISABLE.
       */
      ctx->state = PTHREAD_CANCEL_DISABLE;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_NewState_Invalid: {
      /*
       * While the `state` parameter is neither PTHREAD_CANCEL_ENABLE nor
       * PTHREAD_CANCEL_DISABLE.
       */
      ctx->state = INT_MAX;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_NewState_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_OldState_Prepare(
  CPthreadReqSetcancelstate_Context     *ctx,
  CPthreadReqSetcancelstate_Pre_OldState state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_OldState_Valid: {
      /*
       * While the `oldstate` parameter references an object of type `int`.
       */
      ctx->oldstate = &ctx->oldstate_value;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_OldState_Null: {
      /*
       * While the `oldstate` parameter is NULL.
       */
      ctx->oldstate = NULL;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_OldState_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_Previous_Prepare(
  CPthreadReqSetcancelstate_Context     *ctx,
  CPthreadReqSetcancelstate_Pre_Previous state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_Previous_Enabled: {
      /*
       * While the thread cancel state attribute of the calling thread is
       * PTHREAD_CANCEL_ENABLE immediately before the pthread_setcancelstate()
       * call.
       */
      ctx->previous = PTHREAD_CANCEL_ENABLE;
      (void) pthread_setcancelstate( ctx->previous, NULL );
      break;
    }

    case CPthreadReqSetcancelstate_Pre_Previous_Disabled: {
      /*
       * While the thread cancel state attribute of the calling thread is
       * PTHREAD_CANCEL_DISABLE immediately before the pthread_setcancelstate()
       * call.
       */
      ctx->previous = PTHREAD_CANCEL_DISABLE;
      (void) pthread_setcancelstate( ctx->previous, NULL );
      break;
    }

    case CPthreadReqSetcancelstate_Pre_Previous_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_Status_Check(
  CPthreadReqSetcancelstate_Context    *ctx,
  CPthreadReqSetcancelstate_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_Status_Zero: {
      /*
       * The return value of pthread_setcancelstate() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Status_EINVAL: {
      /*
       * The return value of pthread_setcancelstate() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Status_EPROTO: {
      /*
       * The return value of pthread_setcancelstate() shall be equal to EPROTO.
       */
      T_eq_int( ctx->retval, EPROTO );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_OldState_Check(
  CPthreadReqSetcancelstate_Context      *ctx,
  CPthreadReqSetcancelstate_Post_OldState state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_OldState_Set: {
      /*
       * The value of the object referenced by the `oldstate` parameter shall
       * be set to the thread cancel state attribute of the calling thread
       * immediately before the pthread_setcancelstate() call.
       */
      T_eq_int( ctx->oldstate_value, ctx->previous );
      break;
    }

    case CPthreadReqSetcancelstate_Post_OldState_Nop: {
      /*
       * Objects referenced by the `oldstate` parameter in past calls to
       * pthread_setcancelstate() shall not be accessed by the
       * pthread_setcancelstate() call.
       */
      T_eq_int( ctx->oldstate_value, INVALID_STATE );
      break;
    }

    case CPthreadReqSetcancelstate_Post_OldState_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_State_Check(
  CPthreadReqSetcancelstate_Context   *ctx,
  CPthreadReqSetcancelstate_Post_State state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_State_Enabled: {
      /*
       * The thread cancel state attribute of the calling thread shall be
       * PTHREAD_CANCEL_ENABLE after the return of the pthread_setcancelstate()
       * call.
       */
      T_eq_int( GetCancelState(), PTHREAD_CANCEL_ENABLE );
      break;
    }

    case CPthreadReqSetcancelstate_Post_State_Disabled: {
      /*
       * The thread cancel state attribute of the calling thread shall be
       * PTHREAD_CANCEL_DISABLE after the return of the
       * pthread_setcancelstate() call.
       */
      T_eq_int( GetCancelState(), PTHREAD_CANCEL_DISABLE );
      break;
    }

    case CPthreadReqSetcancelstate_Post_State_Nop: {
      /*
       * The thread cancel state attribute of the calling thread shall not be
       * changed by the pthread_setcancelstate() call.
       */
      T_eq_int( GetCancelState(), ctx->previous );
      break;
    }

    case CPthreadReqSetcancelstate_Post_State_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Prepare(
  CPthreadReqSetcancelstate_Context *ctx
)
{
  ctx->oldstate_value = INVALID_STATE;
}

static void CPthreadReqSetcancelstate_Action(
  CPthreadReqSetcancelstate_Context *ctx
)
{
  if ( ctx->in_isr ) {
    CallWithinISR( Call, ctx );
  } else {
    ctx->retval = pthread_setcancelstate( ctx->state, ctx->oldstate );
  }
}

static void CPthreadReqSetcancelstate_Cleanup( void )
{
  (void) pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
}

static const CPthreadReqSetcancelstate_Entry
CPthreadReqSetcancelstate_Entries[] = {
  { 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_EPROTO,
    CPthreadReqSetcancelstate_Post_OldState_Nop,
    CPthreadReqSetcancelstate_Post_State_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_EPROTO,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_State_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Zero,
    CPthreadReqSetcancelstate_Post_OldState_Set,
    CPthreadReqSetcancelstate_Post_State_Enabled },
  { 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Zero,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_State_Enabled },
  { 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Zero,
    CPthreadReqSetcancelstate_Post_OldState_Set,
    CPthreadReqSetcancelstate_Post_State_Disabled },
  { 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Zero,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_State_Disabled },
  { 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_EINVAL,
    CPthreadReqSetcancelstate_Post_OldState_Nop,
    CPthreadReqSetcancelstate_Post_State_Nop },
  { 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_EINVAL,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_State_Nop }
};

static const uint8_t
CPthreadReqSetcancelstate_Map[] = {
  2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1
};

static size_t CPthreadReqSetcancelstate_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSetcancelstate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqSetcancelstate_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqSetcancelstate_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqSetcancelstate_Scope,
  .initial_context = &CPthreadReqSetcancelstate_Instance
};

static inline CPthreadReqSetcancelstate_Entry
CPthreadReqSetcancelstate_PopEntry( CPthreadReqSetcancelstate_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSetcancelstate_Entries[
    CPthreadReqSetcancelstate_Map[ index ]
  ];
}

static void CPthreadReqSetcancelstate_TestVariant(
  CPthreadReqSetcancelstate_Context *ctx
)
{
  CPthreadReqSetcancelstate_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSetcancelstate_Pre_NewState_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSetcancelstate_Pre_OldState_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqSetcancelstate_Pre_Previous_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqSetcancelstate_Action( ctx );
  CPthreadReqSetcancelstate_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqSetcancelstate_Post_OldState_Check(
    ctx,
    ctx->Map.entry.Post_OldState
  );
  CPthreadReqSetcancelstate_Post_State_Check( ctx, ctx->Map.entry.Post_State );
}

/**
 * @fn void T_case_body_CPthreadReqSetcancelstate( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqSetcancelstate,
  &CPthreadReqSetcancelstate_Fixture
)
{
  CPthreadReqSetcancelstate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSetcancelstate_Pre_Context_Thread;
    ctx->Map.pcs[ 0 ] < CPthreadReqSetcancelstate_Pre_Context_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqSetcancelstate_Pre_NewState_Enabled;
      ctx->Map.pcs[ 1 ] < CPthreadReqSetcancelstate_Pre_NewState_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CPthreadReqSetcancelstate_Pre_OldState_Valid;
        ctx->Map.pcs[ 2 ] < CPthreadReqSetcancelstate_Pre_OldState_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CPthreadReqSetcancelstate_Pre_Previous_Enabled;
          ctx->Map.pcs[ 3 ] < CPthreadReqSetcancelstate_Pre_Previous_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqSetcancelstate_PopEntry( ctx );
          CPthreadReqSetcancelstate_Prepare( ctx );
          CPthreadReqSetcancelstate_TestVariant( ctx );
          CPthreadReqSetcancelstate_Cleanup();
        }
      }
    }
  }
}

/** @} */
