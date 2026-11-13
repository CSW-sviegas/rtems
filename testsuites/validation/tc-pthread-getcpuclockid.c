/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqGetcpuclockid
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
#include <rtems.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqGetcpuclockid spec:/c/pthread/req/getcpuclockid
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqGetcpuclockid_Pre_Thread_Executing,
  CPthreadReqGetcpuclockid_Pre_Thread_Other,
  CPthreadReqGetcpuclockid_Pre_Thread_Invalid,
  CPthreadReqGetcpuclockid_Pre_Thread_NA
} CPthreadReqGetcpuclockid_Pre_Thread;

typedef enum {
  CPthreadReqGetcpuclockid_Pre_ClockId_Obj,
  CPthreadReqGetcpuclockid_Pre_ClockId_Null,
  CPthreadReqGetcpuclockid_Pre_ClockId_NA
} CPthreadReqGetcpuclockid_Pre_ClockId;

typedef enum {
  CPthreadReqGetcpuclockid_Post_Status_Zero,
  CPthreadReqGetcpuclockid_Post_Status_EINVAL,
  CPthreadReqGetcpuclockid_Post_Status_ESRCH,
  CPthreadReqGetcpuclockid_Post_Status_NA
} CPthreadReqGetcpuclockid_Post_Status;

typedef enum {
  CPthreadReqGetcpuclockid_Post_Errno_Unchanged,
  CPthreadReqGetcpuclockid_Post_Errno_NA
} CPthreadReqGetcpuclockid_Post_Errno;

typedef enum {
  CPthreadReqGetcpuclockid_Post_ClockIdObj_Executing,
  CPthreadReqGetcpuclockid_Post_ClockIdObj_Other,
  CPthreadReqGetcpuclockid_Post_ClockIdObj_Nop,
  CPthreadReqGetcpuclockid_Post_ClockIdObj_NA
} CPthreadReqGetcpuclockid_Post_ClockIdObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Thread_NA : 1;
  uint8_t Pre_ClockId_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Errno : 1;
  uint8_t Post_ClockIdObj : 2;
} CPthreadReqGetcpuclockid_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/getcpuclockid test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the
   *   pthread_getcpuclockid() call.
   */
  int status;

  /**
   * @brief This member specifies the `thread` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member specifies the `clock_id` parameter value.
   */
  clockid_t *clock_id;

  /**
   * @brief This member provides the object referenced by the `clock_id`
   *   parameter.
   */
  clockid_t clock_id_obj;

  /**
   * @brief This member contains the identifier of a thread other than the
   *   executing thread.
   */
  rtems_id other_id;

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
    CPthreadReqGetcpuclockid_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqGetcpuclockid_Context;

static CPthreadReqGetcpuclockid_Context CPthreadReqGetcpuclockid_Instance;

static const char *const CPthreadReqGetcpuclockid_PreDesc_Thread[] =
  { "Executing", "Other", "Invalid", "NA" };

static const char *const CPthreadReqGetcpuclockid_PreDesc_ClockId[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqGetcpuclockid_PreDesc[] = {
  CPthreadReqGetcpuclockid_PreDesc_Thread,
  CPthreadReqGetcpuclockid_PreDesc_ClockId,
  NULL
};

static void CPthreadReqGetcpuclockid_Pre_Thread_Prepare(
  CPthreadReqGetcpuclockid_Context   *ctx,
  CPthreadReqGetcpuclockid_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Pre_Thread_Executing: {
      /*
       * While the `thread` parameter is associated with the executing task.
       */
      ctx->thread = pthread_self();
      break;
    }

    case CPthreadReqGetcpuclockid_Pre_Thread_Other: {
      /*
       * While the `thread` parameter is associated with a thread other than
       * the executing task.
       */
      ctx->thread = ctx->other_id;
      break;
    }

    case CPthreadReqGetcpuclockid_Pre_Thread_Invalid: {
      /*
       * While the `thread` parameter is not associated with a thread.
       */
      ctx->thread = INVALID_ID;
      break;
    }

    case CPthreadReqGetcpuclockid_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Pre_ClockId_Prepare(
  CPthreadReqGetcpuclockid_Context    *ctx,
  CPthreadReqGetcpuclockid_Pre_ClockId state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Pre_ClockId_Obj: {
      /*
       * While the `clock_id` parameter references an object of type clockid_t.
       */
      ctx->clock_id = &ctx->clock_id_obj;
      break;
    }

    case CPthreadReqGetcpuclockid_Pre_ClockId_Null: {
      /*
       * While the `clock_id` parameter is equal to NULL.
       */
      ctx->clock_id = NULL;
      break;
    }

    case CPthreadReqGetcpuclockid_Pre_ClockId_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Post_Status_Check(
  CPthreadReqGetcpuclockid_Context    *ctx,
  CPthreadReqGetcpuclockid_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Post_Status_Zero: {
      /*
       * The return value of pthread_getcpuclockid() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_Status_EINVAL: {
      /*
       * The return value of pthread_getcpuclockid() shall be equal to EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_Status_ESRCH: {
      /*
       * The return value of pthread_getcpuclockid() shall be equal to ESRCH.
       */
      T_eq_int( ctx->status, ESRCH );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_Status_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Post_Errno_Check(
  CPthreadReqGetcpuclockid_Post_Errno state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the directive call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_Errno_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Post_ClockIdObj_Check(
  CPthreadReqGetcpuclockid_Context        *ctx,
  CPthreadReqGetcpuclockid_Post_ClockIdObj state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Post_ClockIdObj_Executing: {
      /*
       * The object referenced by the `clock_id` parameter shall be set to the
       * thread identifier of the executing task.
       */
      T_eq_int( ctx->clock_id_obj, (clockid_t) pthread_self() );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_ClockIdObj_Other: {
      /*
       * The object referenced by the `clock_id` parameter shall be set to the
       * thread identifier of the thread other than the executing task.
       */
      T_eq_int( ctx->clock_id_obj, (clockid_t) ctx->other_id );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_ClockIdObj_Nop: {
      /*
       * The object referenced by the `clock_id` parameter shall not be
       * modified.
       */
      T_eq_int( ctx->clock_id_obj, -1 );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_ClockIdObj_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Setup(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  ctx->other_id = CreateTask( "OTHR", PRIO_NORMAL );
}

static void CPthreadReqGetcpuclockid_Setup_Wrap( void *arg )
{
  CPthreadReqGetcpuclockid_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqGetcpuclockid_Setup( ctx );
}

static void CPthreadReqGetcpuclockid_Teardown(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  DeleteTask( ctx->other_id );
}

static void CPthreadReqGetcpuclockid_Teardown_Wrap( void *arg )
{
  CPthreadReqGetcpuclockid_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqGetcpuclockid_Teardown( ctx );
}

static void CPthreadReqGetcpuclockid_Prepare(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  errno = -1;
  ctx->status = 0;
  ctx->clock_id_obj = -1;
}

static void CPthreadReqGetcpuclockid_Action(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  ctx->status = pthread_getcpuclockid( ctx->thread, ctx->clock_id );
}

/* clang-format off */

static const CPthreadReqGetcpuclockid_Entry
CPthreadReqGetcpuclockid_Entries[] = {
  { 0, 0, 0, CPthreadReqGetcpuclockid_Post_Status_EINVAL,
    CPthreadReqGetcpuclockid_Post_Errno_Unchanged,
    CPthreadReqGetcpuclockid_Post_ClockIdObj_NA },
  { 0, 0, 0, CPthreadReqGetcpuclockid_Post_Status_Zero,
    CPthreadReqGetcpuclockid_Post_Errno_Unchanged,
    CPthreadReqGetcpuclockid_Post_ClockIdObj_Executing },
  { 0, 0, 0, CPthreadReqGetcpuclockid_Post_Status_Zero,
    CPthreadReqGetcpuclockid_Post_Errno_Unchanged,
    CPthreadReqGetcpuclockid_Post_ClockIdObj_Other },
  { 0, 0, 0, CPthreadReqGetcpuclockid_Post_Status_ESRCH,
    CPthreadReqGetcpuclockid_Post_Errno_Unchanged,
    CPthreadReqGetcpuclockid_Post_ClockIdObj_Nop }
};

static const uint8_t
CPthreadReqGetcpuclockid_Map[] = {
  1, 0, 2, 0, 3, 0
};

/* clang-format on */

static size_t CPthreadReqGetcpuclockid_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqGetcpuclockid_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqGetcpuclockid_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqGetcpuclockid_Fixture = {
  .setup = CPthreadReqGetcpuclockid_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqGetcpuclockid_Teardown_Wrap,
  .scope = CPthreadReqGetcpuclockid_Scope,
  .initial_context = &CPthreadReqGetcpuclockid_Instance
};

static inline CPthreadReqGetcpuclockid_Entry CPthreadReqGetcpuclockid_PopEntry(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqGetcpuclockid_Entries
    [ CPthreadReqGetcpuclockid_Map[ index ] ];
}

static void CPthreadReqGetcpuclockid_TestVariant(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  CPthreadReqGetcpuclockid_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqGetcpuclockid_Pre_ClockId_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqGetcpuclockid_Action( ctx );
  CPthreadReqGetcpuclockid_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqGetcpuclockid_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CPthreadReqGetcpuclockid_Post_ClockIdObj_Check(
    ctx,
    ctx->Map.entry.Post_ClockIdObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqGetcpuclockid( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqGetcpuclockid,
  &CPthreadReqGetcpuclockid_Fixture
)
{
  CPthreadReqGetcpuclockid_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqGetcpuclockid_Pre_Thread_Executing;
    ctx->Map.pcs[ 0 ] < CPthreadReqGetcpuclockid_Pre_Thread_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqGetcpuclockid_Pre_ClockId_Obj;
      ctx->Map.pcs[ 1 ] < CPthreadReqGetcpuclockid_Pre_ClockId_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqGetcpuclockid_PopEntry( ctx );
      CPthreadReqGetcpuclockid_Prepare( ctx );
      CPthreadReqGetcpuclockid_TestVariant( ctx );
    }
  }
}

/** @} */
