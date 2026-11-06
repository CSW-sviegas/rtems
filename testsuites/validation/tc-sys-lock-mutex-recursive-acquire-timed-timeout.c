/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibReqSysLockMutexRecursiveAcquireTimedTimeout
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
 * @defgroup NewlibReqSysLockMutexRecursiveAcquireTimedTimeout \
 *   spec:/newlib/req/sys-lock-mutex-recursive-acquire-timed-timeout
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_None,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_Caller,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_Other,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_NA
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner;

typedef enum {
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_Valid,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_Invalid,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_NA
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec;

typedef enum {
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_Negative,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_FarFuture,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_Future,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_PastOrNow,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_NA
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec;

typedef enum {
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_Yes,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_No,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_NA
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender;

typedef enum {
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_Zero,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_ETIMEDOUT,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_EINVAL,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_NA
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status;

typedef enum {
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Caller,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Other,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_NA
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner;

typedef enum {
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Inactive,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Realtime,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_NA
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer;

typedef enum {
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Last,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Absolute,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_NA
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire;

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
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Entry;

typedef enum { OWNER_NONE, OWNER_CALLER, OWNER_OTHER } Owner;

/**
 * @brief Test context for
 *   spec:/newlib/req/sys-lock-mutex-recursive-acquire-timed-timeout test case.
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
  struct _Mutex_recursive_Control mutex;

  /**
   * @brief This member specifies the owner of the mutex before the
   *   _Mutex_recursive_Acquire_timed() call.
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
   * @brief This member contains the return value of the
   *   _Mutex_recursive_Acquire_timed() call.
   */
  int status;

  /**
   * @brief This member contains the owner of the mutex after the return of the
   *   _Mutex_recursive_Acquire_timed() call.
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
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context;

static NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Instance;

static const char *const
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc_Owner[] =
    { "None", "Caller", "Other", "NA" };

static const char *const
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc_AbstimeNSec[] =
    { "Valid", "Invalid", "NA" };

static const char *const
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc_AbstimeSec[] =
    { "Negative", "FarFuture", "Future", "PastOrNow", "NA" };

static const char *const
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc_Surrender[] =
    { "Yes", "No", "NA" };

static const char *const *const
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc[] = {
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc_Owner,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc_AbstimeNSec,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc_AbstimeSec,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc_Surrender,
    NULL
};

typedef NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    SuspendSelf();

    if ( ctx->owner == OWNER_CALLER ) {
      _Mutex_recursive_Acquire( &ctx->mutex );
    }

    ctx->status = _Mutex_recursive_Acquire_timed( &ctx->mutex, &ctx->abstime );
    ctx->owner_after = ctx->mutex._Mutex._Queue._owner;

    if ( ctx->status == 0 ) {
      _Mutex_recursive_Release( &ctx->mutex );
    }

    if ( ctx->owner == OWNER_CALLER ) {
      _Mutex_recursive_Release( &ctx->mutex );
    }
  }
}

static void
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_Prepare(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context  *ctx,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_None: {
      /*
       * While the mutex has no owner.
       */
      ctx->owner = OWNER_NONE;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_Caller: {
      /*
       * While the calling task is the owner of the mutex.
       */
      ctx->owner = OWNER_CALLER;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_Other: {
      /*
       * While a task other than the calling task is the owner of the mutex.
       */
      ctx->owner = OWNER_OTHER;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_NA:
      break;
  }
}

static void
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_Prepare(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context        *ctx,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_Valid: {
      /*
       * While the `tv_nsec` member of the object referenced by the timeout
       * parameter is a valid nanoseconds value.
       */
      ctx->abstime.tv_nsec = 999999999;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_Invalid: {
      /*
       * While the `tv_nsec` member of the object referenced by the timeout
       * parameter is an invalid nanoseconds value.
       */
      ctx->abstime.tv_nsec = -1;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_NA:
      break;
  }
}

static void
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_Prepare(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context       *ctx,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_Negative: {
      /*
       * While the `tv_sec` member of the object referenced by the timeout
       * parameter is negative.
       */
      ctx->abstime.tv_sec = -238479;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_FarFuture: {
      /*
       * While the `tv_sec` member of the object referenced by the timeout
       * parameter specifies a time point which is past the implementation
       * limit.
       */
      ctx->abstime.tv_sec = INT64_MAX;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_Future: {
      /*
       * While the `tv_sec` member of the object referenced by the timeout
       * parameter specifies a time point which is after the current time of
       * the CLOCK_REALTIME and is within the implementation limits.
       */
      ctx->abstime.tv_sec = 1621322302;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_PastOrNow: {
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

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_NA:
      break;
  }
}

static void
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_Prepare(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context      *ctx,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_Yes: {
      /*
       * While the owner of the mutex surrenders the mutex before the time
       * point specified by the timeout parameter.
       */
      ctx->surrender = true;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_No: {
      /*
       * While the owner of the mutex does not surrender the mutex before the
       * time point specified by the timeout parameter.
       */
      ctx->surrender = false;
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_NA:
      break;
  }
}

static void
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_Check(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context    *ctx,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_Zero: {
      /*
       * The return value of _Mutex_recursive_Acquire_timed() shall be equal to
       * zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_ETIMEDOUT: {
      /*
       * The return value of _Mutex_recursive_Acquire_timed() shall be equal to
       * ETIMEDOUT.
       */
      T_eq_int( ctx->status, ETIMEDOUT );
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_EINVAL: {
      /*
       * The return value of _Mutex_recursive_Acquire_timed() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_NA:
      break;
  }
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Check(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context   *ctx,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Caller: {
      /*
       * The calling task shall be the owner of the mutex after the return of
       * the _Mutex_recursive_Acquire_timed() call.
       */
      T_eq_ptr( ctx->owner_after, ctx->worker_tcb );
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Other: {
      /*
       * The owner of the mutex before the _Mutex_recursive_Acquire_timed()
       * call shall be the owner of the mutex after the return of the
       * _Mutex_recursive_Acquire_timed() call.
       */
      T_eq_ptr( ctx->owner_after, ctx->runner_tcb );
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_NA:
      break;
  }
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Check(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context   *ctx,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Realtime: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_REALTIME.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_REALTIME );
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_NA:
      break;
  }
}

static void
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Check(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context    *ctx,
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire state
)
{
  switch ( state ) {
    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Last: {
      /*
       * The timer of the calling task shall expire at the last valid time
       * point of the CLOCK_REALTIME.
       */
      T_eq_u64( ctx->timer_info.expire_ticks, 0xffffffffffffffff );
      break;
    }

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Absolute: {
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

    case NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_NA:
      break;
  }
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Setup(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx
)
{
  rtems_time_of_day now = { 1988, 1, 1, 0, 0, 0, 0 };

  T_rsc_success( rtems_clock_set( &now ) );
  _Mutex_recursive_Initialize( &ctx->mutex );
  SetSelfPriority( PRIO_NORMAL );
  ctx->runner_tcb = GetExecuting();
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  ctx->worker_tcb = GetThread( ctx->worker_id );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Setup_Wrap(
  void *arg
)
{
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Setup( ctx );
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Teardown(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  _Mutex_recursive_Destroy( &ctx->mutex );
  RestoreRunnerPriority();
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Teardown_Wrap(
  void *arg
)
{
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Teardown( ctx );
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Prepare(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx
)
{
  ctx->status = -1;
  ctx->owner_after = NULL;
  ctx->surrender = false;
  ctx->abstime.tv_sec = -1;
  ctx->abstime.tv_nsec = -1;
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Action(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx
)
{
  if ( ctx->owner == OWNER_OTHER ) {
    _Mutex_recursive_Acquire( &ctx->mutex );
  }

  ResumeTask( ctx->worker_id );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );

  if ( ctx->owner == OWNER_OTHER ) {
    if ( ctx->surrender ) {
      _Mutex_recursive_Release( &ctx->mutex );
    } else {
      FinalClockTick();
      _Mutex_recursive_Release( &ctx->mutex );
    }
  }
}

/* clang-format off */

static const NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Entry
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Entries[] = {
  { 1, 0, 0, 0, 0,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_NA,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_NA,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_NA,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_NA },
  { 0, 0, 0, 0, 1,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_Zero,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Caller,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Inactive,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_NA },
  { 0, 0, 0, 0, 1,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_EINVAL,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Other,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Inactive,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_NA },
  { 0, 0, 0, 0, 1,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_ETIMEDOUT,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Other,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Inactive,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_NA },
  { 0, 0, 0, 0, 0,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_Zero,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Caller,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Realtime,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Last },
  { 0, 0, 0, 0, 0,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_ETIMEDOUT,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Other,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Realtime,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Last },
  { 0, 0, 0, 0, 0,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_Zero,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Caller,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Realtime,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Absolute },
  { 0, 0, 0, 0, 0,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_ETIMEDOUT,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Other,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Realtime,
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Absolute }
};

static const uint8_t
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Map[] = {
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 3, 0, 4, 5, 6, 7, 3, 0, 2, 0, 2, 0, 2, 0, 2, 0
};

/* clang-format on */

static size_t NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Fixture = {
  .setup = NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Setup_Wrap,
  .stop = NULL,
  .teardown = NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Teardown_Wrap,
  .scope = NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Scope,
  .initial_context =
    &NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Instance
};

static inline NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Entry
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PopEntry(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Entries
    [ NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Map[ index ] ];
}

static void
NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_SetPreConditionStates(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Surrender_NA ) {
    ctx->Map.pcs[ 3 ] =
      NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_TestVariant(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx
)
{
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Action( ctx );
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Owner_Check(
    ctx,
    ctx->Map.entry.Post_Owner
  );
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Timer_Check(
    ctx,
    ctx->Map.entry.Post_Timer
  );
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Post_Expire_Check(
    ctx,
    ctx->Map.entry.Post_Expire
  );
}

/**
 * @fn void T_case_body_NewlibReqSysLockMutexRecursiveAcquireTimedTimeout( void )
 */
T_TEST_CASE_FIXTURE(
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout,
  &NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Fixture
)
{
  NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] =
      NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_None;
    ctx->Map.pci[ 0 ] <
    NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Owner_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] =
        NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_Valid;
      ctx->Map.pci[ 1 ] <
      NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeNSec_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] =
          NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_Negative;
        ctx->Map.pci[ 2 ] <
        NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_AbstimeSec_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] =
            NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_Yes;
          ctx->Map.pci[ 3 ] <
          NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Pre_Surrender_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry =
            NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_SetPreConditionStates(
            ctx
          );
          NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_Prepare( ctx );
          NewlibReqSysLockMutexRecursiveAcquireTimedTimeout_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
