/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CTimeReqClockGettime
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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
#include <rtems.h>
#include <time.h>
#include <rtems/cpuuse.h>
#include <rtems/cpuuseimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CTimeReqClockGettime spec:/c/time/req/clock-gettime
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * The software timecounter of the test suite advances at each read. The
 * prepare restores the counter after each of its reads, so the directive call
 * reads the same counter value. The expected time of each clock follows from
 * the values which the prepare reads.
 *
 * @{
 */

typedef enum {
  CTimeReqClockGettime_Pre_ClockId_Monotonic,
  CTimeReqClockGettime_Pre_ClockId_Realtime,
  CTimeReqClockGettime_Pre_ClockId_ProcessCPUTimeId,
  CTimeReqClockGettime_Pre_ClockId_ThreadCPUTimeId,
  CTimeReqClockGettime_Pre_ClockId_Other,
  CTimeReqClockGettime_Pre_ClockId_NA
} CTimeReqClockGettime_Pre_ClockId;

typedef enum {
  CTimeReqClockGettime_Pre_Tp_Valid,
  CTimeReqClockGettime_Pre_Tp_Null,
  CTimeReqClockGettime_Pre_Tp_NA
} CTimeReqClockGettime_Pre_Tp;

typedef enum {
  CTimeReqClockGettime_Post_Status_Zero,
  CTimeReqClockGettime_Post_Status_MinusOne,
  CTimeReqClockGettime_Post_Status_NA
} CTimeReqClockGettime_Post_Status;

typedef enum {
  CTimeReqClockGettime_Post_Errno_Unchanged,
  CTimeReqClockGettime_Post_Errno_EINVAL,
  CTimeReqClockGettime_Post_Errno_NA
} CTimeReqClockGettime_Post_Errno;

typedef enum {
  CTimeReqClockGettime_Post_Tp_Monotonic,
  CTimeReqClockGettime_Post_Tp_Realtime,
  CTimeReqClockGettime_Post_Tp_ProcessCPUTimeId,
  CTimeReqClockGettime_Post_Tp_ThreadCPUTimeId,
  CTimeReqClockGettime_Post_Tp_Unchanged,
  CTimeReqClockGettime_Post_Tp_NA
} CTimeReqClockGettime_Post_Tp;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_ClockId_NA : 1;
  uint16_t Pre_Tp_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Tp : 3;
} CTimeReqClockGettime_Entry;

/**
 * @brief Test context for spec:/c/time/req/clock-gettime test case.
 */
typedef struct {
  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the CLOCK_REALTIME value before the
   *   clock_gettime() call.
   */
  struct timespec now_realtime;

  /**
   * @brief This member contains the CLOCK_MONOTONIC value before the
   *   clock_gettime() call.
   */
  struct timespec now_monotonic;

  /**
   * @brief This member contains the expected time of the
   *   CLOCK_PROCESS_CPUTIME_ID clock.
   */
  struct timespec process_time;

  /**
   * @brief This member contains the expected time of the
   *   CLOCK_THREAD_CPUTIME_ID clock.
   */
  struct timespec thread_time;

  /**
   * @brief This member provides the object referenced by the `tp` parameter.
   */
  struct timespec tp_obj;

  /**
   * @brief This member contains the return value of the clock_gettime() call.
   */
  int status;

  /**
   * @brief This member specifies the `clock_id` parameter value.
   */
  clockid_t clock_id;

  /**
   * @brief This member specifies the `tp` parameter value.
   */
  struct timespec *tp;

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
    CTimeReqClockGettime_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CTimeReqClockGettime_Context;

static CTimeReqClockGettime_Context CTimeReqClockGettime_Instance;

static const char *const CTimeReqClockGettime_PreDesc_ClockId[] = {
  "Monotonic",
  "Realtime",
  "ProcessCPUTimeId",
  "ThreadCPUTimeId",
  "Other",
  "NA"
};

static const char *const CTimeReqClockGettime_PreDesc_Tp[] =
  { "Valid", "Null", "NA" };

static const char *const *const CTimeReqClockGettime_PreDesc[] = {
  CTimeReqClockGettime_PreDesc_ClockId,
  CTimeReqClockGettime_PreDesc_Tp,
  NULL
};

static void Worker( rtems_task_argument arg )
{
  (void) arg;
  while ( true ) {
    Yield();
  }
}

static void CTimeReqClockGettime_Pre_ClockId_Prepare(
  CTimeReqClockGettime_Context    *ctx,
  CTimeReqClockGettime_Pre_ClockId state
)
{
  switch ( state ) {
    case CTimeReqClockGettime_Pre_ClockId_Monotonic: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_MONOTONIC.
       */
      ctx->clock_id = CLOCK_MONOTONIC;
      break;
    }

    case CTimeReqClockGettime_Pre_ClockId_Realtime: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_REALTIME.
       */
      ctx->clock_id = CLOCK_REALTIME;
      break;
    }

    case CTimeReqClockGettime_Pre_ClockId_ProcessCPUTimeId: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_PROCESS_CPUTIME_ID.
       */
      ctx->clock_id = CLOCK_PROCESS_CPUTIME_ID;
      break;
    }

    case CTimeReqClockGettime_Pre_ClockId_ThreadCPUTimeId: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_THREAD_CPUTIME_ID.
       */
      ctx->clock_id = CLOCK_THREAD_CPUTIME_ID;
      break;
    }

    case CTimeReqClockGettime_Pre_ClockId_Other: {
      /*
       * While the `clock_id` parameter is not equal to CLOCK_MONOTONIC,
       * CLOCK_REALTIME, CLOCK_PROCESS_CPUTIME_ID, or CLOCK_THREAD_CPUTIME_ID.
       */
      ctx->clock_id = CLOCK_TAI;
      break;
    }

    case CTimeReqClockGettime_Pre_ClockId_NA:
      break;
  }
}

static void CTimeReqClockGettime_Pre_Tp_Prepare(
  CTimeReqClockGettime_Context *ctx,
  CTimeReqClockGettime_Pre_Tp   state
)
{
  switch ( state ) {
    case CTimeReqClockGettime_Pre_Tp_Valid: {
      /*
       * While the `tp` parameter references an object of type struct timespec.
       */
      ctx->tp = &ctx->tp_obj;
      break;
    }

    case CTimeReqClockGettime_Pre_Tp_Null: {
      /*
       * While the `tp` parameter is equal to NULL.
       */
      ctx->tp = NULL;
      break;
    }

    case CTimeReqClockGettime_Pre_Tp_NA:
      break;
  }
}

static void CTimeReqClockGettime_Post_Status_Check(
  CTimeReqClockGettime_Context    *ctx,
  CTimeReqClockGettime_Post_Status state
)
{
  switch ( state ) {
    case CTimeReqClockGettime_Post_Status_Zero: {
      /*
       * The return value of clock_gettime() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CTimeReqClockGettime_Post_Status_MinusOne: {
      /*
       * The return value of clock_gettime() shall be equal to minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CTimeReqClockGettime_Post_Status_NA:
      break;
  }
}

static void CTimeReqClockGettime_Post_Errno_Check(
  CTimeReqClockGettime_Post_Errno state
)
{
  switch ( state ) {
    case CTimeReqClockGettime_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the directive call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CTimeReqClockGettime_Post_Errno_EINVAL: {
      /*
       * The errno shall be equal to EINVAL.
       */
      T_eq_int( errno, EINVAL );
      break;
    }

    case CTimeReqClockGettime_Post_Errno_NA:
      break;
  }
}

static void CTimeReqClockGettime_Post_Tp_Check(
  CTimeReqClockGettime_Context *ctx,
  CTimeReqClockGettime_Post_Tp  state
)
{
  switch ( state ) {
    case CTimeReqClockGettime_Post_Tp_Monotonic: {
      /*
       * The object referenced by the `tp` parameter shall be set to the time
       * elapsed since some fixed time point in the past measured using the
       * CLOCK_MONOTONIC at some time point during the directive call.
       */
      T_eq_i64( ctx->tp_obj.tv_sec, ctx->now_monotonic.tv_sec );
      T_eq_long( ctx->tp_obj.tv_nsec, ctx->now_monotonic.tv_nsec );
      break;
    }

    case CTimeReqClockGettime_Post_Tp_Realtime: {
      /*
       * The object referenced by the `tp` parameter shall be set to the time
       * elapsed since the Unix epoch measured using the CLOCK_REALTIME at some
       * time point during the directive call.
       */
      T_eq_i64( ctx->tp_obj.tv_sec, ctx->now_realtime.tv_sec );
      T_eq_long( ctx->tp_obj.tv_nsec, ctx->now_realtime.tv_nsec );
      break;
    }

    case CTimeReqClockGettime_Post_Tp_ProcessCPUTimeId: {
      /*
       * The object referenced by the `tp` parameter shall be set to the time
       * elapsed since the last CPU usage reset point at some time point during
       * the directive call.
       */
      T_eq_i64( ctx->tp_obj.tv_sec, ctx->process_time.tv_sec );
      T_eq_long( ctx->tp_obj.tv_nsec, ctx->process_time.tv_nsec );
      break;
    }

    case CTimeReqClockGettime_Post_Tp_ThreadCPUTimeId: {
      /*
       * The object referenced by the `tp` parameter shall be set to the
       * execution time of the executing thread since the last CPU usage reset
       * point at some time point during the directive call.
       */
      T_eq_i64( ctx->tp_obj.tv_sec, ctx->thread_time.tv_sec );
      T_eq_long( ctx->tp_obj.tv_nsec, ctx->thread_time.tv_nsec );
      break;
    }

    case CTimeReqClockGettime_Post_Tp_Unchanged: {
      /*
       * The object referenced by the `tp` parameter shall not be changed by
       * the directive call.
       */
      T_eq_i64( ctx->tp_obj.tv_sec, -1 );
      T_eq_long( ctx->tp_obj.tv_nsec, -1 );
      break;
    }

    case CTimeReqClockGettime_Post_Tp_NA:
      break;
  }
}

static void CTimeReqClockGettime_Setup( CTimeReqClockGettime_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", GetSelfPriority() );
  StartTask( ctx->worker_id, Worker, 0 );
}

static void CTimeReqClockGettime_Setup_Wrap( void *arg )
{
  CTimeReqClockGettime_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CTimeReqClockGettime_Setup( ctx );
}

static void CTimeReqClockGettime_Teardown( CTimeReqClockGettime_Context *ctx )
{
  DeleteTask( ctx->worker_id );
}

static void CTimeReqClockGettime_Teardown_Wrap( void *arg )
{
  CTimeReqClockGettime_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CTimeReqClockGettime_Teardown( ctx );
}

static void CTimeReqClockGettime_Prepare( CTimeReqClockGettime_Context *ctx )
{
  uint32_t          counter;
  Timestamp_Control uptime;
  Timestamp_Control used;
  Timestamp_Control ran;
  Thread_Control   *executing;

  rtems_cpu_usage_reset();
  Yield();
  counter = GetTimecountCounter();

  rtems_clock_get_monotonic( &ctx->now_monotonic );
  SetTimecountCounter( counter );

  rtems_clock_get_realtime( &ctx->now_realtime );
  SetTimecountCounter( counter );

  _TOD_Get_uptime( &uptime );
  SetTimecountCounter( counter );

  _Timestamp_Subtract( &CPU_usage_Uptime_at_last_reset, &uptime, &used );
  _Timestamp_To_timespec( &used, &ctx->process_time );

  executing = _Thread_Get_executing();
  _Timestamp_Subtract(
    &executing->cpu_time_used_at_last_reset,
    &executing->cpu_time_used,
    &used
  );
  _Timestamp_Subtract(
    &_Thread_Get_CPU( executing )->cpu_usage_timestamp,
    &uptime,
    &ran
  );
  _Timestamp_Add_to( &used, &ran );
  _Timestamp_To_timespec( &used, &ctx->thread_time );

  errno = -1;
  ctx->status = -1;
  ctx->tp_obj.tv_sec = -1;
  ctx->tp_obj.tv_nsec = -1;
}

static void CTimeReqClockGettime_Action( CTimeReqClockGettime_Context *ctx )
{
  ctx->status = clock_gettime( ctx->clock_id, ctx->tp );
}

/* clang-format off */

static const CTimeReqClockGettime_Entry
CTimeReqClockGettime_Entries[] = {
  { 0, 0, 0, CTimeReqClockGettime_Post_Status_MinusOne,
    CTimeReqClockGettime_Post_Errno_EINVAL, CTimeReqClockGettime_Post_Tp_NA },
  { 0, 0, 0, CTimeReqClockGettime_Post_Status_Zero,
    CTimeReqClockGettime_Post_Errno_Unchanged,
    CTimeReqClockGettime_Post_Tp_Monotonic },
  { 0, 0, 0, CTimeReqClockGettime_Post_Status_Zero,
    CTimeReqClockGettime_Post_Errno_Unchanged,
    CTimeReqClockGettime_Post_Tp_Realtime },
  { 0, 0, 0, CTimeReqClockGettime_Post_Status_Zero,
    CTimeReqClockGettime_Post_Errno_Unchanged,
    CTimeReqClockGettime_Post_Tp_ProcessCPUTimeId },
  { 0, 0, 0, CTimeReqClockGettime_Post_Status_Zero,
    CTimeReqClockGettime_Post_Errno_Unchanged,
    CTimeReqClockGettime_Post_Tp_ThreadCPUTimeId },
  { 0, 0, 0, CTimeReqClockGettime_Post_Status_MinusOne,
    CTimeReqClockGettime_Post_Errno_EINVAL,
    CTimeReqClockGettime_Post_Tp_Unchanged }
};

static const uint8_t
CTimeReqClockGettime_Map[] = {
  1, 0, 2, 0, 3, 0, 4, 0, 5, 0
};

/* clang-format on */

static size_t CTimeReqClockGettime_Scope( void *arg, char *buf, size_t n )
{
  CTimeReqClockGettime_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CTimeReqClockGettime_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CTimeReqClockGettime_Fixture = {
  .setup = CTimeReqClockGettime_Setup_Wrap,
  .stop = NULL,
  .teardown = CTimeReqClockGettime_Teardown_Wrap,
  .scope = CTimeReqClockGettime_Scope,
  .initial_context = &CTimeReqClockGettime_Instance
};

static inline CTimeReqClockGettime_Entry CTimeReqClockGettime_PopEntry(
  CTimeReqClockGettime_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CTimeReqClockGettime_Entries[ CTimeReqClockGettime_Map[ index ] ];
}

static void CTimeReqClockGettime_TestVariant(
  CTimeReqClockGettime_Context *ctx
)
{
  CTimeReqClockGettime_Pre_ClockId_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CTimeReqClockGettime_Pre_Tp_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CTimeReqClockGettime_Action( ctx );
  CTimeReqClockGettime_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CTimeReqClockGettime_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CTimeReqClockGettime_Post_Tp_Check( ctx, ctx->Map.entry.Post_Tp );
}

/**
 * @fn void T_case_body_CTimeReqClockGettime( void )
 */
T_TEST_CASE_FIXTURE( CTimeReqClockGettime, &CTimeReqClockGettime_Fixture )
{
  CTimeReqClockGettime_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CTimeReqClockGettime_Pre_ClockId_Monotonic;
    ctx->Map.pcs[ 0 ] < CTimeReqClockGettime_Pre_ClockId_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CTimeReqClockGettime_Pre_Tp_Valid;
      ctx->Map.pcs[ 1 ] < CTimeReqClockGettime_Pre_Tp_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CTimeReqClockGettime_PopEntry( ctx );
      CTimeReqClockGettime_Prepare( ctx );
      CTimeReqClockGettime_TestVariant( ctx );
    }
  }
}

/** @} */
