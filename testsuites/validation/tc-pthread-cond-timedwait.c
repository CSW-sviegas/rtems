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
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondTimedwait spec:/c/pthread/req/cond-timedwait
 *
 * @{
 */

typedef enum {
  CPthreadReqCondTimedwait_Pre_Cond_Valid,
  CPthreadReqCondTimedwait_Pre_Cond_Zeroed,
  CPthreadReqCondTimedwait_Pre_Cond_Invalid,
  CPthreadReqCondTimedwait_Pre_Cond_Null,
  CPthreadReqCondTimedwait_Pre_Cond_NA
} CPthreadReqCondTimedwait_Pre_Cond;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Mutex_Owned,
  CPthreadReqCondTimedwait_Pre_Mutex_NotOwned,
  CPthreadReqCondTimedwait_Pre_Mutex_NA
} CPthreadReqCondTimedwait_Pre_Mutex;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Abstime_Future,
  CPthreadReqCondTimedwait_Pre_Abstime_Past,
  CPthreadReqCondTimedwait_Pre_Abstime_Invalid,
  CPthreadReqCondTimedwait_Pre_Abstime_Null,
  CPthreadReqCondTimedwait_Pre_Abstime_NA
} CPthreadReqCondTimedwait_Pre_Abstime;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Binding_Free,
  CPthreadReqCondTimedwait_Pre_Binding_Same,
  CPthreadReqCondTimedwait_Pre_Binding_Other,
  CPthreadReqCondTimedwait_Pre_Binding_NA
} CPthreadReqCondTimedwait_Pre_Binding;

typedef enum {
  CPthreadReqCondTimedwait_Pre_Wake_Broadcast,
  CPthreadReqCondTimedwait_Pre_Wake_Timeout,
  CPthreadReqCondTimedwait_Pre_Wake_Destroyed,
  CPthreadReqCondTimedwait_Pre_Wake_NA
} CPthreadReqCondTimedwait_Pre_Wake;

typedef enum {
  CPthreadReqCondTimedwait_Post_Status_Zero,
  CPthreadReqCondTimedwait_Post_Status_EINVAL,
  CPthreadReqCondTimedwait_Post_Status_ETIMEDOUT,
  CPthreadReqCondTimedwait_Post_Status_EPERM,
  CPthreadReqCondTimedwait_Post_Status_NA
} CPthreadReqCondTimedwait_Post_Status;

typedef enum {
  CPthreadReqCondTimedwait_Post_Owner_Caller,
  CPthreadReqCondTimedwait_Post_Owner_Nobody,
  CPthreadReqCondTimedwait_Post_Owner_NA
} CPthreadReqCondTimedwait_Post_Owner;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Cond_NA : 1;
  uint16_t Pre_Mutex_NA : 1;
  uint16_t Pre_Abstime_NA : 1;
  uint16_t Pre_Binding_NA : 1;
  uint16_t Pre_Wake_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Owner : 2;
} CPthreadReqCondTimedwait_Entry;

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
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the condition variable object.
   */
  pthread_cond_t cond_obj;

  /**
   * @brief This member contains the mutex object.
   */
  pthread_mutex_t mutex_obj;

  /**
   * @brief This member contains the mutex object used by the worker.
   */
  pthread_mutex_t other_mutex_obj;

  /**
   * @brief This member contains the time point.
   */
  struct timespec abstime_obj;

  /**
   * @brief This member contains the worker identifier or zero.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the identifier of the waiting thread or zero.
   */
  rtems_id waiter_id;

  /**
   * @brief This member contains the request for the worker.
   */
  int worker_request;

  /**
   * @brief This member indicates if the mutex object is owned by the calling
   *   thread before the action.
   */
  bool mutex_owned;

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
    CPthreadReqCondTimedwait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondTimedwait_Context;

static CPthreadReqCondTimedwait_Context
  CPthreadReqCondTimedwait_Instance;

static const char * const CPthreadReqCondTimedwait_PreDesc_Cond[] = {
  "Valid",
  "Zeroed",
  "Invalid",
  "Null",
  "NA"
};

static const char * const CPthreadReqCondTimedwait_PreDesc_Mutex[] = {
  "Owned",
  "NotOwned",
  "NA"
};

static const char * const CPthreadReqCondTimedwait_PreDesc_Abstime[] = {
  "Future",
  "Past",
  "Invalid",
  "Null",
  "NA"
};

static const char * const CPthreadReqCondTimedwait_PreDesc_Binding[] = {
  "Free",
  "Same",
  "Other",
  "NA"
};

static const char * const CPthreadReqCondTimedwait_PreDesc_Wake[] = {
  "Broadcast",
  "Timeout",
  "Destroyed",
  "NA"
};

static const char * const * const CPthreadReqCondTimedwait_PreDesc[] = {
  CPthreadReqCondTimedwait_PreDesc_Cond,
  CPthreadReqCondTimedwait_PreDesc_Mutex,
  CPthreadReqCondTimedwait_PreDesc_Abstime,
  CPthreadReqCondTimedwait_PreDesc_Binding,
  CPthreadReqCondTimedwait_PreDesc_Wake,
  NULL
};

typedef enum {
  WORKER_WAIT_OTHER,
  WORKER_WAIT_SAME,
  WORKER_BROADCAST,
  WORKER_TIMEOUT,
  WORKER_DESTROY
} WorkerRequest;

typedef CPthreadReqCondTimedwait_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;
  int      eno;

  ctx = (Context *) arg;

  switch ( ctx->worker_request ) {
    case WORKER_WAIT_OTHER:
      eno = pthread_mutex_lock( &ctx->other_mutex_obj );
      T_eno_success( eno );
      (void) pthread_cond_wait( &ctx->cond_obj, &ctx->other_mutex_obj );
      (void) pthread_mutex_unlock( &ctx->other_mutex_obj );
      break;
    case WORKER_WAIT_SAME:
      eno = pthread_mutex_lock( &ctx->mutex_obj );
      T_eno_success( eno );
      (void) pthread_cond_wait( &ctx->cond_obj, &ctx->mutex_obj );
      (void) pthread_mutex_unlock( &ctx->mutex_obj );
      break;
    case WORKER_BROADCAST:
      eno = pthread_mutex_lock( &ctx->mutex_obj );
      T_eno_success( eno );
      eno = pthread_cond_broadcast( &ctx->cond_obj );
      T_eno_success( eno );
      eno = pthread_mutex_unlock( &ctx->mutex_obj );
      T_eno_success( eno );
      break;
    case WORKER_TIMEOUT:
      FinalClockTick();
      break;
    case WORKER_DESTROY:
      eno = pthread_mutex_destroy( &ctx->mutex_obj );
      T_eno_success( eno );
      eno = pthread_cond_broadcast( &ctx->cond_obj );
      T_eno_success( eno );
      break;
  }

  SuspendSelf();
}

static rtems_id StartWorker( Context *ctx, int request, Priority prio )
{
  rtems_id id;

  ctx->worker_request = request;
  id = CreateTask( "WORK", prio );
  SetAffinityOne( id, 0 );
  StartTask( id, Worker, ctx );

  return id;
}

static void CPthreadReqCondTimedwait_Pre_Cond_Prepare(
  CPthreadReqCondTimedwait_Context *ctx,
  CPthreadReqCondTimedwait_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Cond_Valid: {
      /*
       * While the `cond` parameter references an initialized condition
       * variable object.
       */
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Cond_Zeroed: {
      /*
       * While the `cond` parameter references a condition variable object
       * which is filled with zero bytes.
       */
      memset( &ctx->cond_obj, 0, sizeof( ctx->cond_obj ) );
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Cond_Invalid: {
      /*
       * While the `cond` parameter references an invalid condition variable
       * object.
       */
      memset( &ctx->cond_obj, 0xff, sizeof( ctx->cond_obj ) );
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

static void CPthreadReqCondTimedwait_Pre_Mutex_Prepare(
  CPthreadReqCondTimedwait_Context  *ctx,
  CPthreadReqCondTimedwait_Pre_Mutex state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Mutex_Owned: {
      /*
       * While the mutex object referenced by `mutex` is owned by the calling
       * thread.
       */
      /* Already prepared */
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Mutex_NotOwned: {
      /*
       * While the mutex object referenced by `mutex` is not owned by any
       * thread.
       */
      eno = pthread_mutex_unlock( ctx->mutex );
      T_eno_success( eno );
      ctx->mutex_owned = false;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Mutex_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Pre_Abstime_Prepare(
  CPthreadReqCondTimedwait_Context    *ctx,
  CPthreadReqCondTimedwait_Pre_Abstime state
)
{
  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Abstime_Future: {
      /*
       * While the `abstime` parameter references a time point which is in the
       * future.
       */
      clock_gettime( CLOCK_REALTIME, &ctx->abstime_obj );
      ctx->abstime_obj.tv_sec += 1000;
      ctx->abstime = &ctx->abstime_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Abstime_Past: {
      /*
       * While the `abstime` parameter references a time point which is in the
       * past.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = 0;
      ctx->abstime = &ctx->abstime_obj;
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Abstime_Invalid: {
      /*
       * While the `abstime` parameter references a time point with a
       * nanoseconds value which is out of range.
       */
      ctx->abstime_obj.tv_sec = 1;
      ctx->abstime_obj.tv_nsec = 1000000000;
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

static void CPthreadReqCondTimedwait_Pre_Binding_Prepare(
  CPthreadReqCondTimedwait_Context    *ctx,
  CPthreadReqCondTimedwait_Pre_Binding state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqCondTimedwait_Pre_Binding_Free: {
      /*
       * While no thread waits on the condition variable object.
       */
      /* Nothing to do */
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Binding_Same: {
      /*
       * While a thread waits on the condition variable object using the mutex
       * object referenced by `mutex`.
       */
      if ( ctx->mutex_owned ) {
        eno = pthread_mutex_unlock( ctx->mutex );
        T_eno_success( eno );
      }

      ctx->waiter_id = StartWorker( ctx, WORKER_WAIT_SAME, PRIO_HIGH );

      if ( ctx->mutex_owned ) {
        eno = pthread_mutex_lock( ctx->mutex );
        T_eno_success( eno );
      }
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Binding_Other: {
      /*
       * While a thread waits on the condition variable object using a mutex
       * object different from the one referenced by `mutex`.
       */
      ctx->waiter_id = StartWorker( ctx, WORKER_WAIT_OTHER, PRIO_HIGH );
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Binding_NA:
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
       * While the condition variable object is broadcast before the time point
       * is reached.
       */
      ctx->worker_id = StartWorker( ctx, WORKER_BROADCAST, PRIO_LOW );
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Wake_Timeout: {
      /*
       * While the time point is reached before the condition variable object
       * is broadcast.
       */
      ctx->worker_id = StartWorker( ctx, WORKER_TIMEOUT, PRIO_LOW );
      break;
    }

    case CPthreadReqCondTimedwait_Pre_Wake_Destroyed: {
      /*
       * While the mutex object referenced by `mutex` is destroyed and the
       * condition variable object is broadcast before the time point is
       * reached.
       */
      ctx->worker_id = StartWorker( ctx, WORKER_DESTROY, PRIO_LOW );
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
    case CPthreadReqCondTimedwait_Post_Status_Zero: {
      /*
       * The return value of pthread_cond_timedwait() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Status_EINVAL: {
      /*
       * The return value of pthread_cond_timedwait() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Status_ETIMEDOUT: {
      /*
       * The return value of pthread_cond_timedwait() shall be equal to
       * ETIMEDOUT.
       */
      T_eq_int( ctx->retval, ETIMEDOUT );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Status_EPERM: {
      /*
       * The return value of pthread_cond_timedwait() shall be equal to EPERM.
       */
      T_eq_int( ctx->retval, EPERM );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Post_Owner_Check(
  CPthreadReqCondTimedwait_Context   *ctx,
  CPthreadReqCondTimedwait_Post_Owner state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqCondTimedwait_Post_Owner_Caller: {
      /*
       * The mutex object referenced by `mutex` shall be owned by the calling
       * thread after the return.
       */
      eno = pthread_mutex_unlock( ctx->mutex );
      T_eno_success( eno );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Owner_Nobody: {
      /*
       * The mutex object referenced by `mutex` shall not be owned by the
       * calling thread.
       */
      eno = pthread_mutex_unlock( ctx->mutex );
      T_eq_int( eno, EPERM );
      break;
    }

    case CPthreadReqCondTimedwait_Post_Owner_NA:
      break;
  }
}

static void CPthreadReqCondTimedwait_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
  SetSelfAffinityOne( 0 );
}

static void CPthreadReqCondTimedwait_Setup_Wrap( void *arg )
{
  CPthreadReqCondTimedwait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondTimedwait_Setup();
}

static void CPthreadReqCondTimedwait_Teardown( void )
{
  RestoreRunnerPriority();
  SetSelfAffinityAll();
}

static void CPthreadReqCondTimedwait_Teardown_Wrap( void *arg )
{
  CPthreadReqCondTimedwait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondTimedwait_Teardown();
}

static void CPthreadReqCondTimedwait_Prepare(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  int eno;

  ctx->worker_id = 0;
  ctx->waiter_id = 0;
  ctx->cond = &ctx->cond_obj;
  ctx->mutex = &ctx->mutex_obj;
  ctx->abstime = &ctx->abstime_obj;
  ctx->mutex_owned = true;

  eno = pthread_cond_init( &ctx->cond_obj, NULL );
  T_eno_success( eno );
  eno = pthread_mutex_init( &ctx->mutex_obj, NULL );
  T_eno_success( eno );
  eno = pthread_mutex_init( &ctx->other_mutex_obj, NULL );
  T_eno_success( eno );
  eno = pthread_mutex_lock( &ctx->mutex_obj );
  T_eno_success( eno );
}

static void CPthreadReqCondTimedwait_Action(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  ctx->retval = pthread_cond_timedwait( ctx->cond, ctx->mutex, ctx->abstime );
}

static void CPthreadReqCondTimedwait_Cleanup(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  if ( ctx->worker_id != 0 ) {
    DeleteTask( ctx->worker_id );
  }

  if ( ctx->waiter_id != 0 ) {
    DeleteTask( ctx->waiter_id );
  }

  (void) pthread_mutex_unlock( &ctx->mutex_obj );
  (void) pthread_mutex_destroy( &ctx->mutex_obj );
  (void) pthread_mutex_destroy( &ctx->other_mutex_obj );
  (void) pthread_cond_destroy( &ctx->cond_obj );
}

static const CPthreadReqCondTimedwait_Entry
CPthreadReqCondTimedwait_Entries[] = {
  { 0, 0, 1, 0, 1, 1, CPthreadReqCondTimedwait_Post_Status_EINVAL,
    CPthreadReqCondTimedwait_Post_Owner_Caller },
  { 0, 0, 0, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_EINVAL,
    CPthreadReqCondTimedwait_Post_Owner_Caller },
  { 0, 0, 0, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_ETIMEDOUT,
    CPthreadReqCondTimedwait_Post_Owner_Caller },
  { 0, 0, 0, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_EINVAL,
    CPthreadReqCondTimedwait_Post_Owner_Nobody },
  { 0, 0, 0, 0, 0, 1, CPthreadReqCondTimedwait_Post_Status_EPERM,
    CPthreadReqCondTimedwait_Post_Owner_Nobody },
  { 0, 0, 0, 0, 0, 0, CPthreadReqCondTimedwait_Post_Status_Zero,
    CPthreadReqCondTimedwait_Post_Owner_Caller },
  { 0, 0, 0, 0, 0, 0, CPthreadReqCondTimedwait_Post_Status_ETIMEDOUT,
    CPthreadReqCondTimedwait_Post_Owner_Caller },
  { 0, 0, 0, 0, 0, 0, CPthreadReqCondTimedwait_Post_Status_EINVAL,
    CPthreadReqCondTimedwait_Post_Owner_NA }
};

static const uint8_t
CPthreadReqCondTimedwait_Map[] = {
  5, 6, 7, 5, 6, 7, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 3,
  3, 3, 1, 1, 1, 1, 1, 1, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 7, 5, 6, 7,
  1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 3, 3, 3, 1, 1, 1, 1,
  1, 1, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0
};

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
  return CPthreadReqCondTimedwait_Entries[
    CPthreadReqCondTimedwait_Map[ index ]
  ];
}

static void CPthreadReqCondTimedwait_SetPreConditionStates(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Mutex_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondTimedwait_Pre_Mutex_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Binding_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqCondTimedwait_Pre_Binding_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Wake_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqCondTimedwait_Pre_Wake_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }
}

static void CPthreadReqCondTimedwait_TestVariant(
  CPthreadReqCondTimedwait_Context *ctx
)
{
  CPthreadReqCondTimedwait_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondTimedwait_Pre_Mutex_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondTimedwait_Pre_Abstime_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondTimedwait_Pre_Binding_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqCondTimedwait_Pre_Wake_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqCondTimedwait_Action( ctx );
  CPthreadReqCondTimedwait_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondTimedwait_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
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
    ctx->Map.pci[ 0 ] = CPthreadReqCondTimedwait_Pre_Cond_Valid;
    ctx->Map.pci[ 0 ] < CPthreadReqCondTimedwait_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondTimedwait_Pre_Mutex_Owned;
      ctx->Map.pci[ 1 ] < CPthreadReqCondTimedwait_Pre_Mutex_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondTimedwait_Pre_Abstime_Future;
        ctx->Map.pci[ 2 ] < CPthreadReqCondTimedwait_Pre_Abstime_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqCondTimedwait_Pre_Binding_Free;
          ctx->Map.pci[ 3 ] < CPthreadReqCondTimedwait_Pre_Binding_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqCondTimedwait_Pre_Wake_Broadcast;
            ctx->Map.pci[ 4 ] < CPthreadReqCondTimedwait_Pre_Wake_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = CPthreadReqCondTimedwait_PopEntry( ctx );
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

/** @} */
