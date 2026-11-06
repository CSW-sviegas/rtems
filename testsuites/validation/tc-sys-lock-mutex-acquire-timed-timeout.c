/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibReqSysLockMutexAcquireTimedTimeout
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
#include <rtems.h>
#include <string.h>
#include <time.h>
#include <sys/lock.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup NewlibReqSysLockMutexAcquireTimedTimeout \
 *   spec:/newlib/req/sys-lock-mutex-acquire-timed-timeout
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_None,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_Other,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_NA
} NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner;

typedef enum {
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_Valid,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_Invalid,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_NA
} NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec;

typedef enum {
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_Negative,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_FarFuture,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_Future,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_PastOrNow,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_NA
} NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec;

typedef enum {
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_Yes,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_No,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_NA
} NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender;

typedef enum {
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_Zero,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_ETIMEDOUT,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_EINVAL,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_NA
} NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status;

typedef enum {
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Caller,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Other,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_NA
} NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner;

typedef enum {
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Inactive,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Realtime,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_NA
} NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer;

typedef enum {
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Last,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Absolute,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_NA
} NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Owner_NA : 1;
  uint16_t Pre_AbstimeNSec_NA : 1;
  uint16_t Pre_AbstimeSec_NA : 1;
  uint16_t Pre_Surrender_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Owner : 2;
  uint16_t Post_Timer : 2;
  uint16_t Post_Expire : 2;
} NewlibReqSysLockMutexAcquireTimedTimeout_Entry;

typedef enum { OWNER_NONE, OWNER_CALLER, OWNER_OTHER } Owner;

/**
 * @brief Test context for
 *   spec:/newlib/req/sys-lock-mutex-acquire-timed-timeout test case.
 */
typedef struct {
  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member references the TCB of the worker task.
   */
  const rtems_tcb *worker_tcb;

  /**
   * @brief This member references the TCB of the runner task.
   */
  const rtems_tcb *runner_tcb;

  /**
   * @brief This member provides the mutex.
   */
  struct _Mutex_Control mutex;

  /**
   * @brief This member specifies the owner of the mutex before the
   *   _Mutex_Acquire_timed() call.
   */
  Owner owner;

  /**
   * @brief This member specifies if the owner surrenders the mutex while the
   *   worker waits for it.
   */
  bool surrender;

  /**
   * @brief This member provides the object referenced by the timeout
   *   parameter.
   */
  struct timespec abstime;

  /**
   * @brief This member contains the timer information of the worker task.
   */
  TaskTimerInfo timer_info;

  /**
   * @brief This member contains the return value of the _Mutex_Acquire_timed()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the owner of the mutex after the return of the
   *   _Mutex_Acquire_timed() call.
   */
  const rtems_tcb *owner_after;

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
    NewlibReqSysLockMutexAcquireTimedTimeout_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} NewlibReqSysLockMutexAcquireTimedTimeout_Context;

static NewlibReqSysLockMutexAcquireTimedTimeout_Context
  NewlibReqSysLockMutexAcquireTimedTimeout_Instance;

static const char *const
  NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc_Owner[] =
    { "None", "Other", "NA" };

static const char *const
  NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc_AbstimeNSec[] =
    { "Valid", "Invalid", "NA" };

static const char *const
  NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc_AbstimeSec[] =
    { "Negative", "FarFuture", "Future", "PastOrNow", "NA" };

static const char *const
  NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc_Surrender[] =
    { "Yes", "No", "NA" };

static const char *const *const
  NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc[] = {
    NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc_Owner,
    NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc_AbstimeNSec,
    NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc_AbstimeSec,
    NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc_Surrender,
    NULL
};

typedef NewlibReqSysLockMutexAcquireTimedTimeout_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    SuspendSelf();

    ctx->status = _Mutex_Acquire_timed( &ctx->mutex, &ctx->abstime );
    ctx->owner_after = ctx->mutex._Queue._owner;

    if ( ctx->status == 0 ) {
      _Mutex_Release( &ctx->mutex );
    }
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_Prepare(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context  *ctx,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_None: {
      /*
       * While the mutex has no owner.
       */
      ctx->owner = OWNER_NONE;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_Other: {
      /*
       * While a task other than the calling task is the owner of the mutex.
       */
      ctx->owner = OWNER_OTHER;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_NA:
      break;
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_Prepare(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context        *ctx,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_Valid: {
      /*
       * While the `tv_nsec` member of the object referenced by the timeout
       * parameter is a valid nanoseconds value.
       */
      ctx->abstime.tv_nsec = 999999999;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_Invalid: {
      /*
       * While the `tv_nsec` member of the object referenced by the timeout
       * parameter is an invalid nanoseconds value.
       */
      ctx->abstime.tv_nsec = -1;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_NA:
      break;
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_Prepare(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context       *ctx,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_Negative: {
      /*
       * While the `tv_sec` member of the object referenced by the timeout
       * parameter is negative.
       */
      ctx->abstime.tv_sec = -238479;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_FarFuture: {
      /*
       * While the `tv_sec` member of the object referenced by the timeout
       * parameter specifies a time point which is past the implementation
       * limit.
       */
      ctx->abstime.tv_sec = INT64_MAX;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_Future: {
      /*
       * While the `tv_sec` member of the object referenced by the timeout
       * parameter specifies a time point which is after the current time of
       * the CLOCK_REALTIME and is within the implementation limits.
       */
      ctx->abstime.tv_sec = 1621322302;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_PastOrNow: {
      /*
       * While the `tv_sec` member of the object referenced by the timeout
       * parameter is non-negative and specifies a time point which is before
       * or at the current time of the CLOCK_REALTIME.
       */
      ctx->abstime.tv_sec = 0;

      if ( ctx->abstime.tv_nsec == 999999999 ) {
        ctx->abstime.tv_nsec = 0;
      }
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_NA:
      break;
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_Prepare(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context      *ctx,
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_Yes: {
      /*
       * While the owner of the mutex surrenders the mutex before the time
       * point specified by the timeout parameter.
       */
      ctx->surrender = true;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_No: {
      /*
       * While the owner of the mutex does not surrender the mutex before the
       * time point specified by the timeout parameter.
       */
      ctx->surrender = false;
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_NA:
      break;
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_Check(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context    *ctx,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_Zero: {
      /*
       * The return value of _Mutex_Acquire_timed() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_ETIMEDOUT: {
      /*
       * The return value of _Mutex_Acquire_timed() shall be equal to
       * ETIMEDOUT.
       */
      T_eq_int( ctx->status, ETIMEDOUT );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_EINVAL: {
      /*
       * The return value of _Mutex_Acquire_timed() shall be equal to EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_NA:
      break;
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Check(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context   *ctx,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Caller: {
      /*
       * The calling task shall be the owner of the mutex after the return of
       * the _Mutex_Acquire_timed() call.
       */
      T_eq_ptr( ctx->owner_after, ctx->worker_tcb );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Other: {
      /*
       * The owner of the mutex before the _Mutex_Acquire_timed() call shall be
       * the owner of the mutex after the return of the _Mutex_Acquire_timed()
       * call.
       */
      T_eq_ptr( ctx->owner_after, ctx->runner_tcb );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_NA:
      break;
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Check(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context   *ctx,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Realtime: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_REALTIME.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_REALTIME );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_NA:
      break;
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Check(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context    *ctx,
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Last: {
      /*
       * The timer of the calling task shall expire at the last valid time
       * point of the CLOCK_REALTIME.
       */
      T_eq_u64( ctx->timer_info.expire_ticks, 0xffffffffffffffff );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Absolute: {
      /*
       * The timer of the calling task shall expire at the time point specified
       * by the timeout parameter.
       */
      T_eq_i64( ctx->timer_info.expire_timespec.tv_sec, ctx->abstime.tv_sec );
      T_eq_long(
        ctx->timer_info.expire_timespec.tv_nsec,
        ctx->abstime.tv_nsec
      );
      break;
    }

    case NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_NA:
      break;
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Setup(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx
)
{
  rtems_time_of_day now = { 1988, 1, 1, 0, 0, 0, 0 };

  T_rsc_success( rtems_clock_set( &now ) );
  _Mutex_Initialize( &ctx->mutex );
  SetSelfPriority( PRIO_NORMAL );
  ctx->runner_tcb = GetExecuting();
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  ctx->worker_tcb = GetThread( ctx->worker_id );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Setup_Wrap( void *arg )
{
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  NewlibReqSysLockMutexAcquireTimedTimeout_Setup( ctx );
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Teardown(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  _Mutex_Destroy( &ctx->mutex );
  RestoreRunnerPriority();
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Teardown_Wrap( void *arg )
{
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  NewlibReqSysLockMutexAcquireTimedTimeout_Teardown( ctx );
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Prepare(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx
)
{
  ctx->status = -1;
  ctx->owner_after = NULL;
  ctx->surrender = false;
  ctx->abstime.tv_sec = -1;
  ctx->abstime.tv_nsec = -1;
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_Action(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx
)
{
  if ( ctx->owner == OWNER_OTHER ) {
    _Mutex_Acquire( &ctx->mutex );
  }

  ResumeTask( ctx->worker_id );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );

  if ( ctx->owner == OWNER_OTHER ) {
    if ( ctx->surrender ) {
      _Mutex_Release( &ctx->mutex );
    } else {
      FinalClockTick();
      _Mutex_Release( &ctx->mutex );
    }
  }
}

/* clang-format off */

static const NewlibReqSysLockMutexAcquireTimedTimeout_Entry
NewlibReqSysLockMutexAcquireTimedTimeout_Entries[] = {
  { 1, 0, 0, 0, 0, NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_NA,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_NA,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_NA,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_NA },
  { 0, 0, 0, 0, 1, NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_Zero,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Caller,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Inactive,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_NA },
  { 0, 0, 0, 0, 1, NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_EINVAL,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Other,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Inactive,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_NA },
  { 0, 0, 0, 0, 1,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_ETIMEDOUT,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Other,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Inactive,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_NA },
  { 0, 0, 0, 0, 0, NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_Zero,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Caller,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Realtime,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Last },
  { 0, 0, 0, 0, 0,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_ETIMEDOUT,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Other,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Realtime,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Last },
  { 0, 0, 0, 0, 0, NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_Zero,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Caller,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Realtime,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Absolute },
  { 0, 0, 0, 0, 0,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_ETIMEDOUT,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Other,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Realtime,
    NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Absolute }
};

static const uint8_t
NewlibReqSysLockMutexAcquireTimedTimeout_Map[] = {
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 0, 4, 5, 6, 7, 3, 0, 2, 0,
  2, 0, 2, 0, 2, 0
};

/* clang-format on */

static size_t NewlibReqSysLockMutexAcquireTimedTimeout_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      NewlibReqSysLockMutexAcquireTimedTimeout_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture NewlibReqSysLockMutexAcquireTimedTimeout_Fixture = {
  .setup = NewlibReqSysLockMutexAcquireTimedTimeout_Setup_Wrap,
  .stop = NULL,
  .teardown = NewlibReqSysLockMutexAcquireTimedTimeout_Teardown_Wrap,
  .scope = NewlibReqSysLockMutexAcquireTimedTimeout_Scope,
  .initial_context = &NewlibReqSysLockMutexAcquireTimedTimeout_Instance
};

static inline NewlibReqSysLockMutexAcquireTimedTimeout_Entry
NewlibReqSysLockMutexAcquireTimedTimeout_PopEntry(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return NewlibReqSysLockMutexAcquireTimedTimeout_Entries
    [ NewlibReqSysLockMutexAcquireTimedTimeout_Map[ index ] ];
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_SetPreConditionStates(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Surrender_NA ) {
    ctx->Map.pcs[ 3 ] =
      NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void NewlibReqSysLockMutexAcquireTimedTimeout_TestVariant(
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx
)
{
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  NewlibReqSysLockMutexAcquireTimedTimeout_Action( ctx );
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Owner_Check(
    ctx,
    ctx->Map.entry.Post_Owner
  );
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Timer_Check(
    ctx,
    ctx->Map.entry.Post_Timer
  );
  NewlibReqSysLockMutexAcquireTimedTimeout_Post_Expire_Check(
    ctx,
    ctx->Map.entry.Post_Expire
  );
}

/**
 * @fn void T_case_body_NewlibReqSysLockMutexAcquireTimedTimeout( void )
 */
T_TEST_CASE_FIXTURE(
  NewlibReqSysLockMutexAcquireTimedTimeout,
  &NewlibReqSysLockMutexAcquireTimedTimeout_Fixture
)
{
  NewlibReqSysLockMutexAcquireTimedTimeout_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] =
      NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_None;
    ctx->Map.pci[ 0 ] < NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Owner_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] =
        NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_Valid;
      ctx->Map.pci[ 1 ] <
      NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeNSec_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_Negative;
        ctx->Map.pci[ 2 ] <
        NewlibReqSysLockMutexAcquireTimedTimeout_Pre_AbstimeSec_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] =
            NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_Yes;
          ctx->Map.pci[ 3 ] <
          NewlibReqSysLockMutexAcquireTimedTimeout_Pre_Surrender_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = NewlibReqSysLockMutexAcquireTimedTimeout_PopEntry(
            ctx
          );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          NewlibReqSysLockMutexAcquireTimedTimeout_SetPreConditionStates(
            ctx
          );
          NewlibReqSysLockMutexAcquireTimedTimeout_Prepare( ctx );
          NewlibReqSysLockMutexAcquireTimedTimeout_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
