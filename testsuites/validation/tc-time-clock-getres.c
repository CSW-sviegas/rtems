/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CTimeReqClockGetres
 */

/*
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
#include <time.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CTimeReqClockGetres spec:/c/time/req/clock-getres
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CTimeReqClockGetres_Pre_ClockId_Monotonic,
  CTimeReqClockGetres_Pre_ClockId_Realtime,
  CTimeReqClockGetres_Pre_ClockId_ProcessCPUTimeId,
  CTimeReqClockGetres_Pre_ClockId_ThreadCPUTimeId,
  CTimeReqClockGetres_Pre_ClockId_Other,
  CTimeReqClockGetres_Pre_ClockId_NA
} CTimeReqClockGetres_Pre_ClockId;

typedef enum {
  CTimeReqClockGetres_Pre_Res_Valid,
  CTimeReqClockGetres_Pre_Res_Null,
  CTimeReqClockGetres_Pre_Res_NA
} CTimeReqClockGetres_Pre_Res;

typedef enum {
  CTimeReqClockGetres_Pre_TimecounterFreq_OneHz,
  CTimeReqClockGetres_Pre_TimecounterFreq_Low,
  CTimeReqClockGetres_Pre_TimecounterFreq_High,
  CTimeReqClockGetres_Pre_TimecounterFreq_NA
} CTimeReqClockGetres_Pre_TimecounterFreq;

typedef enum {
  CTimeReqClockGetres_Post_Status_Zero,
  CTimeReqClockGetres_Post_Status_MinusOne,
  CTimeReqClockGetres_Post_Status_NA
} CTimeReqClockGetres_Post_Status;

typedef enum {
  CTimeReqClockGetres_Post_Errno_Unchanged,
  CTimeReqClockGetres_Post_Errno_EINVAL,
  CTimeReqClockGetres_Post_Errno_NA
} CTimeReqClockGetres_Post_Errno;

typedef enum {
  CTimeReqClockGetres_Post_ResObj_Period,
  CTimeReqClockGetres_Post_ResObj_OneNanosecond,
  CTimeReqClockGetres_Post_ResObj_Unchanged,
  CTimeReqClockGetres_Post_ResObj_NA
} CTimeReqClockGetres_Post_ResObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_ClockId_NA : 1;
  uint16_t Pre_Res_NA : 1;
  uint16_t Pre_TimecounterFreq_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_ResObj : 2;
} CTimeReqClockGetres_Entry;

/**
 * @brief Test context for spec:/c/time/req/clock-getres test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the clock_getres() call.
   */
  int status;

  /**
   * @brief This member specifies the `clock_id` parameter value.
   */
  clockid_t clock_id;

  /**
   * @brief This member specifies the `res` parameter value.
   */
  struct timespec *res;

  /**
   * @brief This member provides the object referenced by the `res` parameter.
   */
  struct timespec res_obj;

  /**
   * @brief If this member is not zero, then the cleanup sets the timecounter
   *   frequency to its value.
   */
  uint64_t frequency;

  /**
   * @brief This member contains the timecounter frequency in Hz during the
   *   clock_getres() call.
   */
  uint64_t timecounter_frequency;

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
    CTimeReqClockGetres_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CTimeReqClockGetres_Context;

static CTimeReqClockGetres_Context CTimeReqClockGetres_Instance;

static const char *const CTimeReqClockGetres_PreDesc_ClockId[] = {
  "Monotonic",
  "Realtime",
  "ProcessCPUTimeId",
  "ThreadCPUTimeId",
  "Other",
  "NA"
};

static const char *const CTimeReqClockGetres_PreDesc_Res[] =
  { "Valid", "Null", "NA" };

static const char *const CTimeReqClockGetres_PreDesc_TimecounterFreq[] =
  { "OneHz", "Low", "High", "NA" };

static const char *const *const CTimeReqClockGetres_PreDesc[] = {
  CTimeReqClockGetres_PreDesc_ClockId,
  CTimeReqClockGetres_PreDesc_Res,
  CTimeReqClockGetres_PreDesc_TimecounterFreq,
  NULL
};

static void CTimeReqClockGetres_Pre_ClockId_Prepare(
  CTimeReqClockGetres_Context    *ctx,
  CTimeReqClockGetres_Pre_ClockId state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Pre_ClockId_Monotonic: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_MONOTONIC.
       */
      ctx->clock_id = CLOCK_MONOTONIC;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_Realtime: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_REALTIME.
       */
      ctx->clock_id = CLOCK_REALTIME;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_ProcessCPUTimeId: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_PROCESS_CPUTIME_ID.
       */
      ctx->clock_id = CLOCK_PROCESS_CPUTIME_ID;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_ThreadCPUTimeId: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_THREAD_CPUTIME_ID.
       */
      ctx->clock_id = CLOCK_THREAD_CPUTIME_ID;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_Other: {
      /*
       * While the `clock_id` parameter is not equal to CLOCK_MONOTONIC,
       * CLOCK_REALTIME, CLOCK_PROCESS_CPUTIME_ID, or CLOCK_THREAD_CPUTIME_ID.
       */
      ctx->clock_id = CLOCK_TAI;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_NA:
      break;
  }
}

static void CTimeReqClockGetres_Pre_Res_Prepare(
  CTimeReqClockGetres_Context *ctx,
  CTimeReqClockGetres_Pre_Res  state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Pre_Res_Valid: {
      /*
       * While the `res` parameter references an object of type struct
       * timespec.
       */
      ctx->res = &ctx->res_obj;
      break;
    }

    case CTimeReqClockGetres_Pre_Res_Null: {
      /*
       * While the `res` parameter is equal to NULL.
       */
      ctx->res = NULL;
      break;
    }

    case CTimeReqClockGetres_Pre_Res_NA:
      break;
  }
}

static void CTimeReqClockGetres_Pre_TimecounterFreq_Prepare(
  CTimeReqClockGetres_Context            *ctx,
  CTimeReqClockGetres_Pre_TimecounterFreq state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Pre_TimecounterFreq_OneHz: {
      /*
       * While the frequency of the currently installed timecounter is equal to
       * 1 Hz.
       */
      ctx->timecounter_frequency = 1;
      ctx->frequency = SetTimecounterFrequency( ctx->timecounter_frequency );
      break;
    }

    case CTimeReqClockGetres_Pre_TimecounterFreq_Low: {
      /*
       * While the frequency of the currently installed timecounter is greater
       * than 1 Hz and at most 1000000000 Hz.
       */
      ctx->timecounter_frequency = UINT64_C( 1500000 );
      ctx->frequency = SetTimecounterFrequency( ctx->timecounter_frequency );
      break;
    }

    case CTimeReqClockGetres_Pre_TimecounterFreq_High: {
      /*
       * While the frequency of the currently installed timecounter is greater
       * than 1000000000 Hz.
       */
      ctx->timecounter_frequency = UINT64_C( 2000000000 );
      ctx->frequency = SetTimecounterFrequency( ctx->timecounter_frequency );
      break;
    }

    case CTimeReqClockGetres_Pre_TimecounterFreq_NA:
      break;
  }
}

static void CTimeReqClockGetres_Post_Status_Check(
  CTimeReqClockGetres_Context    *ctx,
  CTimeReqClockGetres_Post_Status state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Post_Status_Zero: {
      /*
       * The return value of clock_getres() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CTimeReqClockGetres_Post_Status_MinusOne: {
      /*
       * The return value of clock_getres() shall be equal to minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CTimeReqClockGetres_Post_Status_NA:
      break;
  }
}

static void CTimeReqClockGetres_Post_Errno_Check(
  CTimeReqClockGetres_Post_Errno state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the directive call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CTimeReqClockGetres_Post_Errno_EINVAL: {
      /*
       * The errno shall be equal to EINVAL.
       */
      T_eq_int( errno, EINVAL );
      break;
    }

    case CTimeReqClockGetres_Post_Errno_NA:
      break;
  }
}

static void CTimeReqClockGetres_Post_ResObj_Check(
  CTimeReqClockGetres_Context    *ctx,
  CTimeReqClockGetres_Post_ResObj state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Post_ResObj_Period: {
      /*
       * The object referenced by the `res` parameter shall be set to a time in
       * nanoseconds of 1000000000 divided by the frequency in Hz of the
       * currently installed timecounter at some time point during the
       * directive call, rounded down to an integer.
       */
      uint64_t ns;

      ns = UINT64_C( 1000000000 ) / ctx->timecounter_frequency;
      T_eq_i64( ctx->res_obj.tv_sec, (int64_t) ( ns / 1000000000 ) );
      T_eq_long( ctx->res_obj.tv_nsec, (long) ( ns % 1000000000 ) );
      break;
    }

    case CTimeReqClockGetres_Post_ResObj_OneNanosecond: {
      /*
       * The object referenced by the `res` parameter shall be set to a time of
       * one nanosecond.
       */
      T_eq_long( ctx->res_obj.tv_sec, 0 );
      T_eq_long( ctx->res_obj.tv_nsec, 1 );
      break;
    }

    case CTimeReqClockGetres_Post_ResObj_Unchanged: {
      /*
       * The object referenced by the `res` parameter shall not be changed by
       * the directive call.
       */
      T_eq_long( ctx->res_obj.tv_sec, -1 );
      T_eq_long( ctx->res_obj.tv_nsec, -1 );
      break;
    }

    case CTimeReqClockGetres_Post_ResObj_NA:
      break;
  }
}

static void CTimeReqClockGetres_Prepare( CTimeReqClockGetres_Context *ctx )
{
  errno = -1;
  ctx->status = -1;
  ctx->res_obj.tv_sec = -1;
  ctx->res_obj.tv_nsec = -1;
  ctx->frequency = 0;
  ctx->timecounter_frequency = 0;
}

static void CTimeReqClockGetres_Action( CTimeReqClockGetres_Context *ctx )
{
  ctx->status = clock_getres( ctx->clock_id, ctx->res );
}

static void CTimeReqClockGetres_Cleanup( CTimeReqClockGetres_Context *ctx )
{
  if ( ctx->frequency != 0 ) {
    (void) SetTimecounterFrequency( ctx->frequency );
  }
}

/* clang-format off */

static const CTimeReqClockGetres_Entry
CTimeReqClockGetres_Entries[] = {
  { 1, 0, 0, 0, CTimeReqClockGetres_Post_Status_NA,
    CTimeReqClockGetres_Post_Errno_NA, CTimeReqClockGetres_Post_ResObj_NA },
  { 0, 0, 0, 0, CTimeReqClockGetres_Post_Status_Zero,
    CTimeReqClockGetres_Post_Errno_Unchanged,
    CTimeReqClockGetres_Post_ResObj_Period },
  { 0, 0, 0, 0, CTimeReqClockGetres_Post_Status_Zero,
    CTimeReqClockGetres_Post_Errno_Unchanged,
    CTimeReqClockGetres_Post_ResObj_OneNanosecond },
  { 0, 0, 0, 1, CTimeReqClockGetres_Post_Status_Zero,
    CTimeReqClockGetres_Post_Errno_Unchanged,
    CTimeReqClockGetres_Post_ResObj_NA },
  { 0, 0, 0, 1, CTimeReqClockGetres_Post_Status_MinusOne,
    CTimeReqClockGetres_Post_Errno_EINVAL,
    CTimeReqClockGetres_Post_ResObj_Unchanged },
  { 0, 0, 0, 1, CTimeReqClockGetres_Post_Status_MinusOne,
    CTimeReqClockGetres_Post_Errno_EINVAL, CTimeReqClockGetres_Post_ResObj_NA }
};

static const uint8_t
CTimeReqClockGetres_Map[] = {
  1, 1, 2, 3, 0, 0, 1, 1, 2, 3, 0, 0, 1, 1, 2, 3, 0, 0, 1, 1, 2, 3, 0, 0, 4, 0,
  0, 5, 0, 0
};

/* clang-format on */

static size_t CTimeReqClockGetres_Scope( void *arg, char *buf, size_t n )
{
  CTimeReqClockGetres_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CTimeReqClockGetres_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CTimeReqClockGetres_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CTimeReqClockGetres_Scope,
  .initial_context = &CTimeReqClockGetres_Instance
};

static inline CTimeReqClockGetres_Entry CTimeReqClockGetres_PopEntry(
  CTimeReqClockGetres_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CTimeReqClockGetres_Entries[ CTimeReqClockGetres_Map[ index ] ];
}

static void CTimeReqClockGetres_SetPreConditionStates(
  CTimeReqClockGetres_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_TimecounterFreq_NA ) {
    ctx->Map.pcs[ 2 ] = CTimeReqClockGetres_Pre_TimecounterFreq_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CTimeReqClockGetres_TestVariant( CTimeReqClockGetres_Context *ctx )
{
  CTimeReqClockGetres_Pre_ClockId_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CTimeReqClockGetres_Pre_Res_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CTimeReqClockGetres_Pre_TimecounterFreq_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CTimeReqClockGetres_Action( ctx );
  CTimeReqClockGetres_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CTimeReqClockGetres_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CTimeReqClockGetres_Post_ResObj_Check( ctx, ctx->Map.entry.Post_ResObj );
}

/**
 * @fn void T_case_body_CTimeReqClockGetres( void )
 */
T_TEST_CASE_FIXTURE( CTimeReqClockGetres, &CTimeReqClockGetres_Fixture )
{
  CTimeReqClockGetres_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CTimeReqClockGetres_Pre_ClockId_Monotonic;
    ctx->Map.pci[ 0 ] < CTimeReqClockGetres_Pre_ClockId_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CTimeReqClockGetres_Pre_Res_Valid;
      ctx->Map.pci[ 1 ] < CTimeReqClockGetres_Pre_Res_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CTimeReqClockGetres_Pre_TimecounterFreq_OneHz;
        ctx->Map.pci[ 2 ] < CTimeReqClockGetres_Pre_TimecounterFreq_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CTimeReqClockGetres_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CTimeReqClockGetres_SetPreConditionStates( ctx );
        CTimeReqClockGetres_Prepare( ctx );
        CTimeReqClockGetres_TestVariant( ctx );
        CTimeReqClockGetres_Cleanup( ctx );
      }
    }
  }
}

/** @} */
