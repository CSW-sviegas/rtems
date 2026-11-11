/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondTimedwait
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
#include <rtems.h>
#include <string.h>
#include <time.h>
#include <rtems/posix/condimpl.h>
#include <rtems/score/percpu.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondTimedwait spec:/c/pthread/req/cond-timedwait
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondTimedwait_Pre_Cond_Obj,
  CPthreadReqCondTimedwait_Pre_Cond_Null,
  CPthreadReqCondTimedwait_Pre_Cond_NA
} CPthreadReqCondTimedwait_Pre_Cond;

typedef enum {
  CPthreadReqCondTimedwait_Pre_CondInit_Yes,
  CPthreadReqCondTimedwait_Pre_CondInit_Zeroed,
  CPthreadReqCondTimedwait_Pre_CondInit_No,
  CPthreadReqCondTimedwait_Pre_CondInit_NA
} CPthreadReqCondTimedwait_Pre_CondInit;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Clock_Realtime,
  CPthreadReqCondTimedwait_Pre_Clock_Monotonic,
  CPthreadReqCondTimedwait_Pre_Clock_NA
} CPthreadReqCondTimedwait_Pre_Clock;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Binding_None,
  CPthreadReqCondTimedwait_Pre_Binding_Same,
  CPthreadReqCondTimedwait_Pre_Binding_Other,
  CPthreadReqCondTimedwait_Pre_Binding_NA
} CPthreadReqCondTimedwait_Pre_Binding;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Mutex_Obj,
  CPthreadReqCondTimedwait_Pre_Mutex_Null,
  CPthreadReqCondTimedwait_Pre_Mutex_NA
} CPthreadReqCondTimedwait_Pre_Mutex;

typedef enum {
  CPthreadReqCondTimedwait_Pre_MutexInit_Yes,
  CPthreadReqCondTimedwait_Pre_MutexInit_No,
  CPthreadReqCondTimedwait_Pre_MutexInit_NA
} CPthreadReqCondTimedwait_Pre_MutexInit;

typedef enum {
  CPthreadReqCondTimedwait_Pre_MutexOwner_Caller,
  CPthreadReqCondTimedwait_Pre_MutexOwner_Other,
  CPthreadReqCondTimedwait_Pre_MutexOwner_Nobody,
  CPthreadReqCondTimedwait_Pre_MutexOwner_NA
} CPthreadReqCondTimedwait_Pre_MutexOwner;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Abstime_Obj,
  CPthreadReqCondTimedwait_Pre_Abstime_Null,
  CPthreadReqCondTimedwait_Pre_Abstime_NA
} CPthreadReqCondTimedwait_Pre_Abstime;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Time_Future,
  CPthreadReqCondTimedwait_Pre_Time_Past,
  CPthreadReqCondTimedwait_Pre_Time_NegSec,
  CPthreadReqCondTimedwait_Pre_Time_NegNsec,
  CPthreadReqCondTimedwait_Pre_Time_BigNsec,
  CPthreadReqCondTimedwait_Pre_Time_NA
} CPthreadReqCondTimedwait_Pre_Time;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Wake_Broadcast,
  CPthreadReqCondTimedwait_Pre_Wake_Timeout,
  CPthreadReqCondTimedwait_Pre_Wake_MutexDestroyed,
  CPthreadReqCondTimedwait_Pre_Wake_NA
} CPthreadReqCondTimedwait_Pre_Wake;

typedef enum {
  CPthreadReqCondTimedwait_Post_Status_Ok,
  CPthreadReqCondTimedwait_Post_Status_Inval,
  CPthreadReqCondTimedwait_Post_Status_Perm,
  CPthreadReqCondTimedwait_Post_Status_TimedOut,
  CPthreadReqCondTimedwait_Post_Status_NA
} CPthreadReqCondTimedwait_Post_Status;

typedef enum {
  CPthreadReqCondTimedwait_Post_Init_Default,
  CPthreadReqCondTimedwait_Post_Init_Nop,
  CPthreadReqCondTimedwait_Post_Init_NA
} CPthreadReqCondTimedwait_Post_Init;

typedef enum {
  CPthreadReqCondTimedwait_Post_Binding_Mutex,
  CPthreadReqCondTimedwait_Post_Binding_None,
  CPthreadReqCondTimedwait_Post_Binding_Nop,
  CPthreadReqCondTimedwait_Post_Binding_NA
} CPthreadReqCondTimedwait_Post_Binding;

typedef enum {
  CPthreadReqCondTimedwait_Post_Owner_Caller,
  CPthreadReqCondTimedwait_Post_Owner_Other,
  CPthreadReqCondTimedwait_Post_Owner_Nobody,
  CPthreadReqCondTimedwait_Post_Owner_NA
} CPthreadReqCondTimedwait_Post_Owner;

typedef enum {
  CPthreadReqCondTimedwait_Post_Blocking_Yes,
  CPthreadReqCondTimedwait_Post_Blocking_No,
  CPthreadReqCondTimedwait_Post_Blocking_NA
} CPthreadReqCondTimedwait_Post_Blocking;

typedef enum {
  CPthreadReqCondTimedwait_Post_Clock_Realtime,
  CPthreadReqCondTimedwait_Post_Clock_Monotonic,
  CPthreadReqCondTimedwait_Post_Clock_NA
} CPthreadReqCondTimedwait_Post_Clock;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Cond_NA : 1;
  uint32_t Pre_CondInit_NA : 1;
  uint32_t Pre_Clock_NA : 1;
  uint32_t Pre_Binding_NA : 1;
  uint32_t Pre_Mutex_NA : 1;
  uint32_t Pre_MutexInit_NA : 1;
  uint32_t Pre_MutexOwner_NA : 1;
  uint32_t Pre_Abstime_NA : 1;
  uint32_t Pre_Time_NA : 1;
  uint32_t Pre_Wake_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Init : 2;
  uint32_t Post_Binding : 2;
  uint32_t Post_Owner : 2;
  uint32_t Post_Blocking : 2;
  uint32_t Post_Clock : 2;
} CPthreadReqCondTimedwait_Entry;

typedef enum { COND_INIT_YES, COND_INIT_ZEROED, COND_INIT_NO } CondInit;

typedef enum {
  MUTEX_OWNER_CALLER,
  MUTEX_OWNER_OTHER,
  MUTEX_OWNER_NOBODY
} MutexOwner;

typedef enum { WAKE_BROADCAST, WAKE_TIMEOUT, WAKE_MUTEX_DESTROYED } Wake;

/**
 * @brief Test context for spec:/c/pthread/req/cond-timedwait test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `cond` parameter value.
   */
  pthread_cond_t *cond;

  /**
   * @brief This member specifies the `mutex` parameter value.
   */
  pthread_mutex_t *mutex;

  /**
   * @brief This member specifies the `abstime` parameter value.
   */
  const struct timespec *abstime;

  /**
   * @brief This member contains the return value of the
   *   pthread_cond_timedwait() call.
   */
  int status;

  /**
   * @brief This member provides the condition variable object.
   */
  pthread_cond_t cond_obj;

  /**
   * @brief This member contains a copy of the condition variable object before
   *   the call.
   */
  pthread_cond_t cond_before;

  /**
   * @brief This member provides the mutex object.
   */
  pthread_mutex_t mutex_obj;

  /**
   * @brief This member provides the object of the mutex other than the one
   *   referenced by the `mutex` parameter.
   */
  pthread_mutex_t other_mutex_obj;

  /**
   * @brief This member provides the time point object.
   */
  struct timespec abstime_obj;

  /**
   * @brief This member specifies the initialization state of the condition
   *   variable object.
   */
  CondInit cond_init;

  /**
   * @brief This member specifies the clock attribute of the condition
   *   variable.
   */
  clockid_t clock;

  /**
   * @brief This member specifies the mutex to which the condition variable is
   *   bound.
   */
  pthread_mutex_t *binding;

  /**
   * @brief This member is true, if the mutex object is initialized.
   */
  bool mutex_init;

  /**
   * @brief This member specifies the owner of the mutex.
   */
  MutexOwner mutex_owner;

  /**
   * @brief This member specifies how the wait ends.
   */
  Wake wake;

  /**
   * @brief This member references the TCB of the runner task.
   */
  Thread_Control *runner;

  /**
   * @brief This member contains the identifier of the owner task.
   */
  rtems_id owner_id;

  /**
   * @brief This member references the TCB of the owner task.
   */
  Thread_Control *owner;

  /**
   * @brief This member is true, if the owner task owns the mutex.
   */
  bool owner_locked;

  /**
   * @brief This member contains the identifier of the waker task.
   */
  rtems_id waker_id;

  /**
   * @brief This member is true, if the waker task found the runner task
   *   blocked on the condition variable.
   */
  bool woken;

  /**
   * @brief This member is true, if the waker task found the runner task
   *   blocked on the condition variable before the call returned.
   */
  bool blocked;

  /**
   * @brief This member references the watchdog header of the timeout of the
   *   runner task while it was blocked on the condition variable.
   */
  Watchdog_Header *timer_header;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 10 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 10 ];

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
    CPthreadReqCondTimedwait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondTimedwait_Context;

static CPthreadReqCondTimedwait_Context CPthreadReqCondTimedwait_Instance;

static const char *const CPthreadReqCondTimedwait_PreDesc_Cond[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_CondInit[] =
  { "Yes", "Zeroed", "No", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_Clock[] =
  { "Realtime", "Monotonic", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_Binding[] =
  { "None", "Same", "Other", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_Mutex[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_MutexInit[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_MutexOwner[] =
  { "Caller", "Other", "Nobody", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_Abstime[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_Time[] =
  { "Future", "Past", "NegSec", "NegNsec", "BigNsec", "NA" };

static const char *const CPthreadReqCondTimedwait_PreDesc_Wake[] =
  { "Broadcast", "Timeout", "MutexDestroyed", "NA" };

static const char *const *const CPthreadReqCondTimedwait_PreDesc[] = {
  CPthreadReqCondTimedwait_PreDesc_Cond,
  CPthreadReqCondTimedwait_PreDesc_CondInit,
  CPthreadReqCondTimedwait_PreDesc_Clock,
  CPthreadReqCondTimedwait_PreDesc_Binding,
  CPthreadReqCondTimedwait_PreDesc_Mutex,
  CPthreadReqCondTimedwait_PreDesc_MutexInit,
  CPthreadReqCondTimedwait_PreDesc_MutexOwner,
  CPthreadReqCondTimedwait_PreDesc_Abstime,
  CPthreadReqCondTimedwait_PreDesc_Time,
  CPthreadReqCondTimedwait_PreDesc_Wake,
  NULL
};

typedef CPthreadReqCondTimedwait_Context Context;

#define EVENT_LOCK RTEMS_EVENT_0

#define EVENT_UNLOCK RTEMS_EVENT_1

#define FAR_FUTURE ( (time_t) 0x400000000 )

#define COND_FLAGS_REALTIME( cond )                              \
  ( ( (uintptr_t) ( cond ) ^ POSIX_CONDITION_VARIABLES_MAGIC ) & \
    ~POSIX_CONDITION_VARIABLES_FLAGS_MASK )

static void PrepareCond( Context *ctx )
{
  pthread_condattr_t attr;
  int                eno;

  switch ( ctx->cond_init ) {
    case COND_INIT_YES:
      eno = pthread_condattr_init( &attr );
      T_eq_int( eno, 0 );
      eno = pthread_condattr_setclock( &attr, ctx->clock );
      T_eq_int( eno, 0 );
      eno = pthread_cond_init( &ctx->cond_obj, &attr );
      T_eq_int( eno, 0 );
      eno = pthread_condattr_destroy( &attr );
      T_eq_int( eno, 0 );
      ctx->cond_obj._mutex = ctx->binding;
      break;
    case COND_INIT_ZEROED:
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      break;
    default:
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
      break;
  }
}

static void PrepareMutex( Context *ctx )
{
  int eno;

  if ( !ctx->mutex_init ) {
    memset( &ctx->mutex_obj, 0xff, sizeof( ctx->mutex_obj ) );
    return;
  }

  eno = pthread_mutex_init( &ctx->mutex_obj, NULL );
  T_eq_int( eno, 0 );

  switch ( ctx->mutex_owner ) {
    case MUTEX_OWNER_CALLER:
      eno = pthread_mutex_lock( &ctx->mutex_obj );
      T_eq_int( eno, 0 );
      break;
    case MUTEX_OWNER_OTHER:
      SendEvents( ctx->owner_id, EVENT_LOCK );
      T_true( ctx->owner_locked );
      break;
    default:
      break;
  }
}

static void Owner( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_event_set events;
    int             eno;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_LOCK ) != 0 ) {
      eno = pthread_mutex_lock( &ctx->mutex_obj );
      T_eq_int( eno, 0 );
      ctx->owner_locked = true;
    }

    if ( ( events & EVENT_UNLOCK ) != 0 ) {
      eno = pthread_mutex_unlock( &ctx->mutex_obj );
      T_eq_int( eno, 0 );
      ctx->owner_locked = false;
    }
  }
}

static void Waker( rtems_task_argument arg )
{
  Context *ctx;
  int      eno;

  ctx = (Context *) arg;

  /*
   * The runner and this task share processor 0, and the runner has the
   * higher priority.  This task runs only while the runner is blocked.
   */
  T_ne_u32(
    ctx->runner->current_state & STATES_WAITING_FOR_CONDITION_VARIABLE,
    0
  );
  ctx->timer_header = ctx->runner->Timer.header;
  ctx->woken = true;

  if ( ctx->wake == WAKE_TIMEOUT ) {
    FinalClockTick();
  } else {
    if ( ctx->wake == WAKE_MUTEX_DESTROYED ) {
      eno = pthread_mutex_destroy( &ctx->mutex_obj );
      T_eq_int( eno, 0 );
    }

    eno = pthread_cond_broadcast( &ctx->cond_obj );
    T_eq_int( eno, 0 );
  }

  SuspendSelf();
}

static void CPthreadReqCondTimedwait_Pre_Cond_Prepare(
  CPthreadReqCondTimedwait_Context *ctx,
  CPthreadReqCondTimedwait_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Cond_Obj: {
      /*
       * While the `cond` parameter references an object of type
       * pthread_cond_t.
       */
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_CondInit_Prepare(
  CPthreadReqCondTimedwait_Context     *ctx,
  CPthreadReqCondTimedwait_Pre_CondInit state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_CondInit_Yes: {
      /*
       * While the object referenced by the `cond` parameter is an initialized
       * condition variable.
       */
      ctx->cond_init = COND_INIT_YES;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_CondInit_Zeroed: {
      /*
       * While the object referenced by the `cond` parameter is filled with
       * zero bytes.
       */
      ctx->cond_init = COND_INIT_ZEROED;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_CondInit_No: {
      /*
       * While the object referenced by the `cond` parameter is neither an
       * initialized condition variable nor filled with zero bytes.
       */
      ctx->cond_init = COND_INIT_NO;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_CondInit_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_Clock_Prepare(
  CPthreadReqCondTimedwait_Context  *ctx,
  CPthreadReqCondTimedwait_Pre_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Clock_Realtime: {
      /*
       * While the condition variable clock attribute of the condition variable
       * referenced by the `cond` parameter is CLOCK_REALTIME.
       */
      ctx->clock = CLOCK_REALTIME;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Clock_Monotonic: {
      /*
       * While the condition variable clock attribute of the condition variable
       * referenced by the `cond` parameter is CLOCK_MONOTONIC.
       */
      ctx->clock = CLOCK_MONOTONIC;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Clock_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_Binding_Prepare(
  CPthreadReqCondTimedwait_Context    *ctx,
  CPthreadReqCondTimedwait_Pre_Binding state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Binding_None: {
      /*
       * While the condition variable referenced by the `cond` parameter is
       * bound to no mutex.
       */
      ctx->binding = NULL;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Binding_Same: {
      /*
       * While the condition variable referenced by the `cond` parameter is
       * bound to the mutex referenced by the `mutex` parameter.
       */
      ctx->binding = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Binding_Other: {
      /*
       * While the condition variable referenced by the `cond` parameter is
       * bound to a mutex other than the one referenced by the `mutex`
       * parameter.
       */
      ctx->binding = &ctx->other_mutex_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Binding_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_Mutex_Prepare(
  CPthreadReqCondTimedwait_Context  *ctx,
  CPthreadReqCondTimedwait_Pre_Mutex state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Mutex_Obj: {
      /*
       * While the `mutex` parameter references an object of type
       * pthread_mutex_t.
       */
      ctx->mutex = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Mutex_Null: {
      /*
       * While the `mutex` parameter is NULL.
       */
      ctx->mutex = NULL;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Mutex_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_MutexInit_Prepare(
  CPthreadReqCondTimedwait_Context      *ctx,
  CPthreadReqCondTimedwait_Pre_MutexInit state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_MutexInit_Yes: {
      /*
       * While the object referenced by the `mutex` parameter is an initialized
       * mutex.
       */
      ctx->mutex_init = true;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_MutexInit_No: {
      /*
       * While the object referenced by the `mutex` parameter is neither an
       * initialized mutex nor filled with zero bytes.
       */
      ctx->mutex_init = false;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_MutexInit_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_MutexOwner_Prepare(
  CPthreadReqCondTimedwait_Context       *ctx,
  CPthreadReqCondTimedwait_Pre_MutexOwner state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_MutexOwner_Caller: {
      /*
       * While the mutex referenced by the `mutex` parameter is owned by the
       * calling thread.
       */
      ctx->mutex_owner = MUTEX_OWNER_CALLER;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_MutexOwner_Other: {
      /*
       * While the mutex referenced by the `mutex` parameter is owned by a
       * thread other than the calling thread.
       */
      ctx->mutex_owner = MUTEX_OWNER_OTHER;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_MutexOwner_Nobody: {
      /*
       * While the mutex referenced by the `mutex` parameter is not owned by
       * any thread.
       */
      ctx->mutex_owner = MUTEX_OWNER_NOBODY;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_MutexOwner_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_Abstime_Prepare(
  CPthreadReqCondTimedwait_Context    *ctx,
  CPthreadReqCondTimedwait_Pre_Abstime state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Abstime_Obj: {
      /*
       * While the `abstime` parameter references an object of type struct
       * timespec.
       */
      ctx->abstime = &ctx->abstime_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Abstime_Null: {
      /*
       * While the `abstime` parameter is NULL.
       */
      ctx->abstime = NULL;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Abstime_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_Time_Prepare(
  CPthreadReqCondTimedwait_Context *ctx,
  CPthreadReqCondTimedwait_Pre_Time state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Time_Future: {
      /*
       * While the `abstime` parameter references a time point in the future.
       */
      ctx->abstime_obj.tv_sec = FAR_FUTURE;
      ctx->abstime_obj.tv_nsec = 0;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Time_Past: {
      /*
       * While the `abstime` parameter references a time point in the past with
       * a seconds value of zero or more and a nanoseconds value of zero or
       * more and less than one billion.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = 0;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Time_NegSec: {
      /*
       * While the `abstime` parameter references a time point with a negative
       * seconds value and a nanoseconds value of zero or more and less than
       * one billion.
       */
      ctx->abstime_obj.tv_sec = -1;
      ctx->abstime_obj.tv_nsec = 0;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Time_NegNsec: {
      /*
       * While the `abstime` parameter references a time point with a negative
       * nanoseconds value.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = -1;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Time_BigNsec: {
      /*
       * While the `abstime` parameter references a time point with a
       * nanoseconds value greater than or equal to one billion.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = 1000000000;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Time_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_Wake_Prepare(
  CPthreadReqCondTimedwait_Context *ctx,
  CPthreadReqCondTimedwait_Pre_Wake state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Wake_Broadcast: {
      /*
       * While the condition variable is broadcast after the calling thread
       * blocked on it and before the timeout of the wait for the time point
       * referenced by the `abstime` parameter expires.
       */
      ctx->wake = WAKE_BROADCAST;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Wake_Timeout: {
      /*
       * While the timeout of the wait for the time point referenced by the
       * `abstime` parameter expires after the calling thread blocked on the
       * condition variable and before the condition variable is broadcast.
       */
      ctx->wake = WAKE_TIMEOUT;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Wake_MutexDestroyed: {
      /*
       * While the mutex referenced by the `mutex` parameter is destroyed and
       * then the condition variable is broadcast after the calling thread
       * blocked on the condition variable and before the time point referenced
       * by the `abstime` parameter is reached.
       */
      ctx->wake = WAKE_MUTEX_DESTROYED;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Wake_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Post_Status_Check(
  CPthreadReqCondTimedwait_Context    *ctx,
  CPthreadReqCondTimedwait_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Post_Status_Ok: {
      /*
       * The return value of pthread_cond_timedwait() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Status_Inval: {
      /*
       * The return value of pthread_cond_timedwait() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Status_Perm: {
      /*
       * The return value of pthread_cond_timedwait() shall be EPERM.
       */
      T_eq_int( ctx->status, EPERM );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Status_TimedOut: {
      /*
       * The return value of pthread_cond_timedwait() shall be ETIMEDOUT.
       */
      T_eq_int( ctx->status, ETIMEDOUT );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Post_Init_Check(
  CPthreadReqCondTimedwait_Context  *ctx,
  CPthreadReqCondTimedwait_Post_Init state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Post_Init_Default: {
      /*
       * The object referenced by the `cond` parameter shall be a condition
       * variable initialized with the condition variable clock attribute
       * CLOCK_REALTIME.
       */
      T_eq_ulong(
        ctx->cond_obj._flags,
        COND_FLAGS_REALTIME( &ctx->cond_obj )
      );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Init_Nop: {
      /*
       * The initialization state and the condition variable clock attribute of
       * the object referenced by the `cond` parameter shall not be modified.
       */
      T_eq_ulong( ctx->cond_obj._flags, ctx->cond_before._flags );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Init_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Post_Binding_Check(
  CPthreadReqCondTimedwait_Context     *ctx,
  CPthreadReqCondTimedwait_Post_Binding state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Post_Binding_Mutex: {
      /*
       * The condition variable referenced by the `cond` parameter shall be
       * bound to the mutex referenced by the `mutex` parameter.
       */
      T_eq_ptr( ctx->cond_obj._mutex, &ctx->mutex_obj );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Binding_None: {
      /*
       * The condition variable referenced by the `cond` parameter shall be
       * bound to no mutex.
       */
      T_null( ctx->cond_obj._mutex );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Binding_Nop: {
      /*
       * The binding of the object referenced by the `cond` parameter to a
       * mutex shall not be modified.
       */
      T_eq_ptr( ctx->cond_obj._mutex, ctx->cond_before._mutex );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Binding_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Post_Owner_Check(
  CPthreadReqCondTimedwait_Context   *ctx,
  CPthreadReqCondTimedwait_Post_Owner state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Post_Owner_Caller: {
      /*
       * The mutex referenced by the `mutex` parameter shall be owned by the
       * calling thread.
       */
      T_eq_ptr( ctx->mutex_obj._Recursive._Mutex._Queue._owner, ctx->runner );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Owner_Other: {
      /*
       * The mutex referenced by the `mutex` parameter shall be owned by the
       * thread other than the calling thread which owned it before the call.
       */
      T_eq_ptr( ctx->mutex_obj._Recursive._Mutex._Queue._owner, ctx->owner );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Owner_Nobody: {
      /*
       * The mutex referenced by the `mutex` parameter shall not be owned by
       * any thread.
       */
      T_null( ctx->mutex_obj._Recursive._Mutex._Queue._owner );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Owner_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Post_Blocking_Check(
  CPthreadReqCondTimedwait_Context      *ctx,
  CPthreadReqCondTimedwait_Post_Blocking state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Post_Blocking_Yes: {
      /*
       * The calling thread shall block on the condition variable until the
       * condition variable is signaled or broadcast, or until the timeout of
       * the wait for the time point referenced by the `abstime` parameter
       * expires.
       */
      T_true( ctx->blocked );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Blocking_No: {
      /*
       * The calling thread shall not block on the condition variable.
       */
      T_false( ctx->blocked );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Blocking_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Post_Clock_Check(
  CPthreadReqCondTimedwait_Context   *ctx,
  CPthreadReqCondTimedwait_Post_Clock state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Post_Clock_Realtime: {
      /*
       * The time point referenced by the `abstime` parameter shall be measured
       * against CLOCK_REALTIME.
       */
      T_eq_ptr(
        ctx->timer_header,
        &_Per_CPU_Get_by_index( 0 )
           ->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ]
      );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Clock_Monotonic: {
      /*
       * The time point referenced by the `abstime` parameter shall be measured
       * against CLOCK_MONOTONIC.
       */
      T_eq_ptr(
        ctx->timer_header,
        &_Per_CPU_Get_by_index( 0 )
           ->Watchdog.Header[ PER_CPU_WATCHDOG_MONOTONIC ]
      );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Clock_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Setup(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  SetSelfPriority( PRIO_NORMAL );
  SetSelfAffinityOne( 0 );
  ctx->runner = GetThread( rtems_task_self() );
  ctx->owner_id = CreateTask( "OWNR", PRIO_HIGH );
  SetAffinityOne( ctx->owner_id, 0 );
  StartTask( ctx->owner_id, Owner, ctx );
  ctx->owner = GetThread( ctx->owner_id );
}

static void CPthreadReqCondTimedwait_Setup_Wrap( void *arg )
{
  CPthreadReqCondTimedwait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondTimedwait_Setup( ctx );
}

static void CPthreadReqCondTimedwait_Teardown(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  DeleteTask( ctx->owner_id );
  SetSelfAffinityAll();
  RestoreRunnerPriority();
}

static void CPthreadReqCondTimedwait_Teardown_Wrap( void *arg )
{
  CPthreadReqCondTimedwait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondTimedwait_Teardown( ctx );
}

static void CPthreadReqCondTimedwait_Prepare(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  ctx->cond = NULL;
  ctx->mutex = NULL;
  ctx->abstime = NULL;
  ctx->status = -1;
  ctx->cond_init = COND_INIT_NO;
  ctx->clock = CLOCK_REALTIME;
  ctx->binding = NULL;
  ctx->mutex_init = false;
  ctx->mutex_owner = MUTEX_OWNER_NOBODY;
  ctx->wake = WAKE_BROADCAST;
  ctx->waker_id = 0;
  ctx->woken = false;
  ctx->blocked = false;
  ctx->timer_header = NULL;
}

static void CPthreadReqCondTimedwait_Action(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  PrepareCond( ctx );
  PrepareMutex( ctx );
  ctx->waker_id = CreateTask( "WAKE", PRIO_LOW );
  SetAffinityOne( ctx->waker_id, 0 );
  StartTask( ctx->waker_id, Waker, ctx );
  memcpy( &ctx->cond_before, &ctx->cond_obj, sizeof( ctx->cond_before ) );
  ctx->status = pthread_cond_timedwait( ctx->cond, ctx->mutex, ctx->abstime );
  ctx->blocked = ctx->woken;
}

static void CPthreadReqCondTimedwait_Cleanup(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  DeleteTask( ctx->waker_id );

  if ( ctx->owner_locked ) {
    SendEvents( ctx->owner_id, EVENT_UNLOCK );
  }

  (void) pthread_mutex_unlock( &ctx->mutex_obj );
  (void) pthread_mutex_destroy( &ctx->mutex_obj );
  (void) pthread_cond_destroy( &ctx->cond_obj );
}

/* clang-format off */

static const CPthreadReqCondTimedwait_Entry
CPthreadReqCondTimedwait_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCondTimedwait_Post_Status_NA,
    CPthreadReqCondTimedwait_Post_Init_NA,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_NA,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_NA,
    CPthreadReqCondTimedwait_Post_Init_NA,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_NA,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_Perm,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Other,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_Perm,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Nobody,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_Perm,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_Perm,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_None,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_Other,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_Nobody,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_TimedOut,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_Perm,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Other,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_Perm,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Nobody,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_Perm,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_Perm,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_None,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_Other,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_Nobody,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Nop,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCondTimedwait_Post_Status_Ok,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CPthreadReqCondTimedwait_Post_Status_TimedOut,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCondTimedwait_Post_Status_Ok,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Monotonic },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CPthreadReqCondTimedwait_Post_Status_TimedOut,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Monotonic },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Nop,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Monotonic },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_TimedOut,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_NA,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_No,
    CPthreadReqCondTimedwait_Post_Clock_NA },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, CPthreadReqCondTimedwait_Post_Status_Ok,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Realtime },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
    CPthreadReqCondTimedwait_Post_Status_TimedOut,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_Mutex,
    CPthreadReqCondTimedwait_Post_Owner_Caller,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Realtime },
  { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
    CPthreadReqCondTimedwait_Post_Status_Inval,
    CPthreadReqCondTimedwait_Post_Init_Default,
    CPthreadReqCondTimedwait_Post_Binding_NA,
    CPthreadReqCondTimedwait_Post_Owner_NA,
    CPthreadReqCondTimedwait_Post_Blocking_Yes,
    CPthreadReqCondTimedwait_Post_Clock_Realtime }
};

static const uint8_t
CPthreadReqCondTimedwait_Map[] = {
  23, 24, 25, 11, 0, 0, 11, 0, 0, 12, 0, 0, 12, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0,
  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 5, 0, 0, 5, 0, 0, 5, 0, 0, 5, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  23, 24, 25, 11, 0, 0, 11, 0, 0, 12, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0,
  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 6, 0, 0, 6, 0, 0, 6, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 7, 0, 0, 7, 0, 0, 7, 0, 0, 7, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 8, 0, 0, 8, 0, 0, 8, 0, 0, 8, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 9, 0, 0, 9, 0, 0, 9, 0, 0, 9, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0,
  0, 10, 0, 0, 10, 0, 0, 10, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26,
  27, 28, 11, 0, 0, 11, 0, 0, 12, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 4,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
  0, 0, 5, 0, 0, 5, 0, 0, 5, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 27,
  28, 11, 0, 0, 11, 0, 0, 12, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 6,
  0, 0, 6, 0, 0, 6, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  7, 0, 0, 7, 0, 0, 7, 0, 0, 7, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 8, 0, 0, 8, 0, 0, 8, 0, 0, 8, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 9, 0, 0, 9, 0, 0, 9, 0, 0, 9, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 10, 0,
  0, 10, 0, 0, 10, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33, 34, 29,
  0, 0, 29, 0, 0, 30, 0, 0, 30, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 13, 0, 0, 13, 0, 0, 13, 0, 0, 13, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 14, 0, 0, 14, 0, 0, 14, 0, 0, 14, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 15, 0, 0, 15, 0, 0, 15,
  0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 16, 0, 0, 16, 0, 0, 16, 0, 0, 16, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 17, 0, 0, 17, 0, 0, 17, 0, 0, 17, 0, 0,
  22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 18, 0, 0, 18, 0, 0,
  18, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0,
  19, 0, 0, 19, 0, 0, 19, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 20, 0, 0, 20, 0, 0, 20, 0, 0, 20, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 21, 0, 0, 21, 0,
  0, 21, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondTimedwait_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondTimedwait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondTimedwait_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondTimedwait_Fixture = {
  .setup = CPthreadReqCondTimedwait_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqCondTimedwait_Teardown_Wrap,
  .scope = CPthreadReqCondTimedwait_Scope,
  .initial_context = &CPthreadReqCondTimedwait_Instance
};

static inline CPthreadReqCondTimedwait_Entry CPthreadReqCondTimedwait_PopEntry(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondTimedwait_Entries
    [ CPthreadReqCondTimedwait_Map[ index ] ];
}

static void CPthreadReqCondTimedwait_SetPreConditionStates(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_CondInit_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondTimedwait_Pre_CondInit_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Clock_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondTimedwait_Pre_Clock_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Binding_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqCondTimedwait_Pre_Binding_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Mutex_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqCondTimedwait_Pre_Mutex_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_MutexInit_NA ) {
    ctx->Map.pcs[ 5 ] = CPthreadReqCondTimedwait_Pre_MutexInit_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_MutexOwner_NA ) {
    ctx->Map.pcs[ 6 ] = CPthreadReqCondTimedwait_Pre_MutexOwner_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];

  if ( ctx->Map.entry.Pre_Time_NA ) {
    ctx->Map.pcs[ 8 ] = CPthreadReqCondTimedwait_Pre_Time_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }

  if ( ctx->Map.entry.Pre_Wake_NA ) {
    ctx->Map.pcs[ 9 ] = CPthreadReqCondTimedwait_Pre_Wake_NA;
  } else {
    ctx->Map.pcs[ 9 ] = ctx->Map.pci[ 9 ];
  }
}

static void CPthreadReqCondTimedwait_TestVariant(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  CPthreadReqCondTimedwait_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondTimedwait_Pre_CondInit_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondTimedwait_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondTimedwait_Pre_Binding_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqCondTimedwait_Pre_Mutex_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqCondTimedwait_Pre_MutexInit_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqCondTimedwait_Pre_MutexOwner_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CPthreadReqCondTimedwait_Pre_Abstime_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CPthreadReqCondTimedwait_Pre_Time_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  CPthreadReqCondTimedwait_Pre_Wake_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  CPthreadReqCondTimedwait_Action( ctx );
  CPthreadReqCondTimedwait_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondTimedwait_Post_Init_Check( ctx, ctx->Map.entry.Post_Init );
  CPthreadReqCondTimedwait_Post_Binding_Check(
    ctx,
    ctx->Map.entry.Post_Binding
  );
  CPthreadReqCondTimedwait_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  CPthreadReqCondTimedwait_Post_Blocking_Check(
    ctx,
    ctx->Map.entry.Post_Blocking
  );
  CPthreadReqCondTimedwait_Post_Clock_Check( ctx, ctx->Map.entry.Post_Clock );
}

/**
 * @fn void T_case_body_CPthreadReqCondTimedwait( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondTimedwait,
  &CPthreadReqCondTimedwait_Fixture
)
{
  CPthreadReqCondTimedwait_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondTimedwait_Pre_Cond_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondTimedwait_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondTimedwait_Pre_CondInit_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCondTimedwait_Pre_CondInit_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondTimedwait_Pre_Clock_Realtime;
        ctx->Map.pci[ 2 ] < CPthreadReqCondTimedwait_Pre_Clock_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqCondTimedwait_Pre_Binding_None;
          ctx->Map.pci[ 3 ] < CPthreadReqCondTimedwait_Pre_Binding_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqCondTimedwait_Pre_Mutex_Obj;
            ctx->Map.pci[ 4 ] < CPthreadReqCondTimedwait_Pre_Mutex_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CPthreadReqCondTimedwait_Pre_MutexInit_Yes;
              ctx->Map.pci[ 5 ] < CPthreadReqCondTimedwait_Pre_MutexInit_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] =
                  CPthreadReqCondTimedwait_Pre_MutexOwner_Caller;
                ctx->Map.pci[ 6 ] < CPthreadReqCondTimedwait_Pre_MutexOwner_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CPthreadReqCondTimedwait_Pre_Abstime_Obj;
                  ctx->Map.pci[ 7 ] < CPthreadReqCondTimedwait_Pre_Abstime_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] =
                      CPthreadReqCondTimedwait_Pre_Time_Future;
                    ctx->Map.pci[ 8 ] < CPthreadReqCondTimedwait_Pre_Time_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    for (
                      ctx->Map.pci[ 9 ] =
                        CPthreadReqCondTimedwait_Pre_Wake_Broadcast;
                      ctx->Map.pci[ 9 ] < CPthreadReqCondTimedwait_Pre_Wake_NA;
                      ++ctx->Map.pci[ 9 ]
                    ) {
                      ctx->Map.entry = CPthreadReqCondTimedwait_PopEntry(
                        ctx
                      );

                      if ( ctx->Map.entry.Skip ) {
                        continue;
                      }

                      CPthreadReqCondTimedwait_SetPreConditionStates( ctx );
                      CPthreadReqCondTimedwait_Prepare( ctx );
                      CPthreadReqCondTimedwait_TestVariant( ctx );
                      CPthreadReqCondTimedwait_Cleanup( ctx );
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
