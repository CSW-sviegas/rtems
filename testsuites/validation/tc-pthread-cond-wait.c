/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondWait
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
#include <rtems/posix/condimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondWait spec:/c/pthread/req/cond-wait
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCondWait_Pre_Cond_Obj,
  CPthreadReqCondWait_Pre_Cond_Null,
  CPthreadReqCondWait_Pre_Cond_NA
} CPthreadReqCondWait_Pre_Cond;

typedef enum {
  CPthreadReqCondWait_Pre_CondInit_Yes,
  CPthreadReqCondWait_Pre_CondInit_Zeroed,
  CPthreadReqCondWait_Pre_CondInit_No,
  CPthreadReqCondWait_Pre_CondInit_NA
} CPthreadReqCondWait_Pre_CondInit;

typedef enum {
  CPthreadReqCondWait_Pre_Binding_None,
  CPthreadReqCondWait_Pre_Binding_Same,
  CPthreadReqCondWait_Pre_Binding_Other,
  CPthreadReqCondWait_Pre_Binding_NA
} CPthreadReqCondWait_Pre_Binding;

typedef enum {
  CPthreadReqCondWait_Pre_Mutex_Obj,
  CPthreadReqCondWait_Pre_Mutex_Null,
  CPthreadReqCondWait_Pre_Mutex_NA
} CPthreadReqCondWait_Pre_Mutex;

typedef enum {
  CPthreadReqCondWait_Pre_MutexInit_Yes,
  CPthreadReqCondWait_Pre_MutexInit_No,
  CPthreadReqCondWait_Pre_MutexInit_NA
} CPthreadReqCondWait_Pre_MutexInit;

typedef enum {
  CPthreadReqCondWait_Pre_MutexOwner_Caller,
  CPthreadReqCondWait_Pre_MutexOwner_Other,
  CPthreadReqCondWait_Pre_MutexOwner_Nobody,
  CPthreadReqCondWait_Pre_MutexOwner_NA
} CPthreadReqCondWait_Pre_MutexOwner;

typedef enum {
  CPthreadReqCondWait_Pre_Wake_Broadcast,
  CPthreadReqCondWait_Pre_Wake_MutexDestroyed,
  CPthreadReqCondWait_Pre_Wake_NA
} CPthreadReqCondWait_Pre_Wake;

typedef enum {
  CPthreadReqCondWait_Post_Status_Ok,
  CPthreadReqCondWait_Post_Status_Inval,
  CPthreadReqCondWait_Post_Status_Perm,
  CPthreadReqCondWait_Post_Status_NA
} CPthreadReqCondWait_Post_Status;

typedef enum {
  CPthreadReqCondWait_Post_Init_Default,
  CPthreadReqCondWait_Post_Init_Nop,
  CPthreadReqCondWait_Post_Init_NA
} CPthreadReqCondWait_Post_Init;

typedef enum {
  CPthreadReqCondWait_Post_Binding_Mutex,
  CPthreadReqCondWait_Post_Binding_None,
  CPthreadReqCondWait_Post_Binding_Nop,
  CPthreadReqCondWait_Post_Binding_NA
} CPthreadReqCondWait_Post_Binding;

typedef enum {
  CPthreadReqCondWait_Post_Owner_Caller,
  CPthreadReqCondWait_Post_Owner_Other,
  CPthreadReqCondWait_Post_Owner_Nobody,
  CPthreadReqCondWait_Post_Owner_NA
} CPthreadReqCondWait_Post_Owner;

typedef enum {
  CPthreadReqCondWait_Post_Blocking_Yes,
  CPthreadReqCondWait_Post_Blocking_No,
  CPthreadReqCondWait_Post_Blocking_NA
} CPthreadReqCondWait_Post_Blocking;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Cond_NA : 1;
  uint32_t Pre_CondInit_NA : 1;
  uint32_t Pre_Binding_NA : 1;
  uint32_t Pre_Mutex_NA : 1;
  uint32_t Pre_MutexInit_NA : 1;
  uint32_t Pre_MutexOwner_NA : 1;
  uint32_t Pre_Wake_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Init : 2;
  uint32_t Post_Binding : 2;
  uint32_t Post_Owner : 2;
  uint32_t Post_Blocking : 2;
} CPthreadReqCondWait_Entry;

typedef enum { COND_INIT_YES, COND_INIT_ZEROED, COND_INIT_NO } CondInit;

typedef enum {
  MUTEX_OWNER_CALLER,
  MUTEX_OWNER_OTHER,
  MUTEX_OWNER_NOBODY
} MutexOwner;

typedef enum { WAKE_BROADCAST, WAKE_MUTEX_DESTROYED } Wake;

/**
 * @brief Test context for spec:/c/pthread/req/cond-wait test case.
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
   * @brief This member contains the return value of the pthread_cond_wait()
   *   call.
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
   * @brief This member specifies the initialization state of the condition
   *   variable object.
   */
  CondInit cond_init;

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

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 7 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 7 ];

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
    CPthreadReqCondWait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondWait_Context;

static CPthreadReqCondWait_Context CPthreadReqCondWait_Instance;

static const char *const CPthreadReqCondWait_PreDesc_Cond[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_CondInit[] =
  { "Yes", "Zeroed", "No", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_Binding[] =
  { "None", "Same", "Other", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_Mutex[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_MutexInit[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_MutexOwner[] =
  { "Caller", "Other", "Nobody", "NA" };

static const char *const CPthreadReqCondWait_PreDesc_Wake[] =
  { "Broadcast", "MutexDestroyed", "NA" };

static const char *const *const CPthreadReqCondWait_PreDesc[] = {
  CPthreadReqCondWait_PreDesc_Cond,
  CPthreadReqCondWait_PreDesc_CondInit,
  CPthreadReqCondWait_PreDesc_Binding,
  CPthreadReqCondWait_PreDesc_Mutex,
  CPthreadReqCondWait_PreDesc_MutexInit,
  CPthreadReqCondWait_PreDesc_MutexOwner,
  CPthreadReqCondWait_PreDesc_Wake,
  NULL
};

typedef CPthreadReqCondWait_Context Context;

#define EVENT_LOCK RTEMS_EVENT_0

#define EVENT_UNLOCK RTEMS_EVENT_1

#define COND_FLAGS_REALTIME( cond )                              \
  ( ( (uintptr_t) ( cond ) ^ POSIX_CONDITION_VARIABLES_MAGIC ) & \
    ~POSIX_CONDITION_VARIABLES_FLAGS_MASK )

static void PrepareCond( Context *ctx )
{
  int eno;

  switch ( ctx->cond_init ) {
    case COND_INIT_YES:
      eno = pthread_cond_init( &ctx->cond_obj, NULL );
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
  ctx->woken = true;

  if ( ctx->wake == WAKE_MUTEX_DESTROYED ) {
    eno = pthread_mutex_destroy( &ctx->mutex_obj );
    T_eq_int( eno, 0 );
  }

  eno = pthread_cond_broadcast( &ctx->cond_obj );
  T_eq_int( eno, 0 );

  SuspendSelf();
}

static void CPthreadReqCondWait_Pre_Cond_Prepare(
  CPthreadReqCondWait_Context *ctx,
  CPthreadReqCondWait_Pre_Cond state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Pre_Cond_Obj: {
      /*
       * While the `cond` parameter references an object of type
       * pthread_cond_t.
       */
      ctx->cond = &ctx->cond_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Cond_Null: {
      /*
       * While the `cond` parameter is NULL.
       */
      ctx->cond = NULL;
      break;
    }

    case CPthreadReqCondWait_Pre_Cond_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_CondInit_Prepare(
  CPthreadReqCondWait_Context     *ctx,
  CPthreadReqCondWait_Pre_CondInit state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Pre_CondInit_Yes: {
      /*
       * While the object referenced by the `cond` parameter is an initialized
       * condition variable.
       */
      ctx->cond_init = COND_INIT_YES;
      break;
    }

    case CPthreadReqCondWait_Pre_CondInit_Zeroed: {
      /*
       * While the object referenced by the `cond` parameter is filled with
       * zero bytes.
       */
      ctx->cond_init = COND_INIT_ZEROED;
      break;
    }

    case CPthreadReqCondWait_Pre_CondInit_No: {
      /*
       * While the object referenced by the `cond` parameter is neither an
       * initialized condition variable nor filled with zero bytes.
       */
      ctx->cond_init = COND_INIT_NO;
      break;
    }

    case CPthreadReqCondWait_Pre_CondInit_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_Binding_Prepare(
  CPthreadReqCondWait_Context    *ctx,
  CPthreadReqCondWait_Pre_Binding state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Pre_Binding_None: {
      /*
       * While the condition variable referenced by the `cond` parameter is
       * bound to no mutex.
       */
      ctx->binding = NULL;
      break;
    }

    case CPthreadReqCondWait_Pre_Binding_Same: {
      /*
       * While the condition variable referenced by the `cond` parameter is
       * bound to the mutex referenced by the `mutex` parameter.
       */
      ctx->binding = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Binding_Other: {
      /*
       * While the condition variable referenced by the `cond` parameter is
       * bound to a mutex other than the one referenced by the `mutex`
       * parameter.
       */
      ctx->binding = &ctx->other_mutex_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Binding_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_Mutex_Prepare(
  CPthreadReqCondWait_Context  *ctx,
  CPthreadReqCondWait_Pre_Mutex state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Pre_Mutex_Obj: {
      /*
       * While the `mutex` parameter references an object of type
       * pthread_mutex_t.
       */
      ctx->mutex = &ctx->mutex_obj;
      break;
    }

    case CPthreadReqCondWait_Pre_Mutex_Null: {
      /*
       * While the `mutex` parameter is NULL.
       */
      ctx->mutex = NULL;
      break;
    }

    case CPthreadReqCondWait_Pre_Mutex_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_MutexInit_Prepare(
  CPthreadReqCondWait_Context      *ctx,
  CPthreadReqCondWait_Pre_MutexInit state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Pre_MutexInit_Yes: {
      /*
       * While the object referenced by the `mutex` parameter is an initialized
       * mutex.
       */
      ctx->mutex_init = true;
      break;
    }

    case CPthreadReqCondWait_Pre_MutexInit_No: {
      /*
       * While the object referenced by the `mutex` parameter is neither an
       * initialized mutex nor filled with zero bytes.
       */
      ctx->mutex_init = false;
      break;
    }

    case CPthreadReqCondWait_Pre_MutexInit_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_MutexOwner_Prepare(
  CPthreadReqCondWait_Context       *ctx,
  CPthreadReqCondWait_Pre_MutexOwner state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Pre_MutexOwner_Caller: {
      /*
       * While the mutex referenced by the `mutex` parameter is owned by the
       * calling thread.
       */
      ctx->mutex_owner = MUTEX_OWNER_CALLER;
      break;
    }

    case CPthreadReqCondWait_Pre_MutexOwner_Other: {
      /*
       * While the mutex referenced by the `mutex` parameter is owned by a
       * thread other than the calling thread.
       */
      ctx->mutex_owner = MUTEX_OWNER_OTHER;
      break;
    }

    case CPthreadReqCondWait_Pre_MutexOwner_Nobody: {
      /*
       * While the mutex referenced by the `mutex` parameter is not owned by
       * any thread.
       */
      ctx->mutex_owner = MUTEX_OWNER_NOBODY;
      break;
    }

    case CPthreadReqCondWait_Pre_MutexOwner_NA:
      break;
  }
}

static void CPthreadReqCondWait_Pre_Wake_Prepare(
  CPthreadReqCondWait_Context *ctx,
  CPthreadReqCondWait_Pre_Wake state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Pre_Wake_Broadcast: {
      /*
       * While the condition variable is broadcast after the calling thread
       * blocked on it.
       */
      ctx->wake = WAKE_BROADCAST;
      break;
    }

    case CPthreadReqCondWait_Pre_Wake_MutexDestroyed: {
      /*
       * While the mutex referenced by the `mutex` parameter is destroyed and
       * then the condition variable is broadcast after the calling thread
       * blocked on the condition variable.
       */
      ctx->wake = WAKE_MUTEX_DESTROYED;
      break;
    }

    case CPthreadReqCondWait_Pre_Wake_NA:
      break;
  }
}

static void CPthreadReqCondWait_Post_Status_Check(
  CPthreadReqCondWait_Context    *ctx,
  CPthreadReqCondWait_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Post_Status_Ok: {
      /*
       * The return value of pthread_cond_wait() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCondWait_Post_Status_Inval: {
      /*
       * The return value of pthread_cond_wait() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqCondWait_Post_Status_Perm: {
      /*
       * The return value of pthread_cond_wait() shall be EPERM.
       */
      T_eq_int( ctx->status, EPERM );
      break;
    }

    case CPthreadReqCondWait_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondWait_Post_Init_Check(
  CPthreadReqCondWait_Context  *ctx,
  CPthreadReqCondWait_Post_Init state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Post_Init_Default: {
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

    case CPthreadReqCondWait_Post_Init_Nop: {
      /*
       * The initialization state and the condition variable clock attribute of
       * the object referenced by the `cond` parameter shall not be modified.
       */
      T_eq_ulong( ctx->cond_obj._flags, ctx->cond_before._flags );
      break;
    }

    case CPthreadReqCondWait_Post_Init_NA:
      break;
  }
}

static void CPthreadReqCondWait_Post_Binding_Check(
  CPthreadReqCondWait_Context     *ctx,
  CPthreadReqCondWait_Post_Binding state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Post_Binding_Mutex: {
      /*
       * The condition variable referenced by the `cond` parameter shall be
       * bound to the mutex referenced by the `mutex` parameter.
       */
      T_eq_ptr( ctx->cond_obj._mutex, &ctx->mutex_obj );
      break;
    }

    case CPthreadReqCondWait_Post_Binding_None: {
      /*
       * The condition variable referenced by the `cond` parameter shall be
       * bound to no mutex.
       */
      T_null( ctx->cond_obj._mutex );
      break;
    }

    case CPthreadReqCondWait_Post_Binding_Nop: {
      /*
       * The binding of the object referenced by the `cond` parameter to a
       * mutex shall not be modified.
       */
      T_eq_ptr( ctx->cond_obj._mutex, ctx->cond_before._mutex );
      break;
    }

    case CPthreadReqCondWait_Post_Binding_NA:
      break;
  }
}

static void CPthreadReqCondWait_Post_Owner_Check(
  CPthreadReqCondWait_Context   *ctx,
  CPthreadReqCondWait_Post_Owner state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Post_Owner_Caller: {
      /*
       * The mutex referenced by the `mutex` parameter shall be owned by the
       * calling thread.
       */
      T_eq_ptr( ctx->mutex_obj._Recursive._Mutex._Queue._owner, ctx->runner );
      break;
    }

    case CPthreadReqCondWait_Post_Owner_Other: {
      /*
       * The mutex referenced by the `mutex` parameter shall be owned by the
       * thread other than the calling thread which owned it before the call.
       */
      T_eq_ptr( ctx->mutex_obj._Recursive._Mutex._Queue._owner, ctx->owner );
      break;
    }

    case CPthreadReqCondWait_Post_Owner_Nobody: {
      /*
       * The mutex referenced by the `mutex` parameter shall not be owned by
       * any thread.
       */
      T_null( ctx->mutex_obj._Recursive._Mutex._Queue._owner );
      break;
    }

    case CPthreadReqCondWait_Post_Owner_NA:
      break;
  }
}

static void CPthreadReqCondWait_Post_Blocking_Check(
  CPthreadReqCondWait_Context      *ctx,
  CPthreadReqCondWait_Post_Blocking state
)
{
  switch ( state ) {
    case CPthreadReqCondWait_Post_Blocking_Yes: {
      /*
       * The calling thread shall block on the condition variable until the
       * condition variable is signaled or broadcast.
       */
      T_true( ctx->blocked );
      break;
    }

    case CPthreadReqCondWait_Post_Blocking_No: {
      /*
       * The calling thread shall not block on the condition variable.
       */
      T_false( ctx->blocked );
      break;
    }

    case CPthreadReqCondWait_Post_Blocking_NA:
      break;
  }
}

static void CPthreadReqCondWait_Setup( CPthreadReqCondWait_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  SetSelfAffinityOne( 0 );
  ctx->runner = GetThread( rtems_task_self() );
  ctx->owner_id = CreateTask( "OWNR", PRIO_HIGH );
  SetAffinityOne( ctx->owner_id, 0 );
  StartTask( ctx->owner_id, Owner, ctx );
  ctx->owner = GetThread( ctx->owner_id );
}

static void CPthreadReqCondWait_Setup_Wrap( void *arg )
{
  CPthreadReqCondWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondWait_Setup( ctx );
}

static void CPthreadReqCondWait_Teardown( CPthreadReqCondWait_Context *ctx )
{
  DeleteTask( ctx->owner_id );
  SetSelfAffinityAll();
  RestoreRunnerPriority();
}

static void CPthreadReqCondWait_Teardown_Wrap( void *arg )
{
  CPthreadReqCondWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCondWait_Teardown( ctx );
}

static void CPthreadReqCondWait_Prepare( CPthreadReqCondWait_Context *ctx )
{
  ctx->cond = NULL;
  ctx->mutex = NULL;
  ctx->status = -1;
  ctx->cond_init = COND_INIT_NO;
  ctx->binding = NULL;
  ctx->mutex_init = false;
  ctx->mutex_owner = MUTEX_OWNER_NOBODY;
  ctx->wake = WAKE_BROADCAST;
  ctx->waker_id = 0;
  ctx->woken = false;
  ctx->blocked = false;
}

static void CPthreadReqCondWait_Action( CPthreadReqCondWait_Context *ctx )
{
  PrepareCond( ctx );
  PrepareMutex( ctx );
  ctx->waker_id = CreateTask( "WAKE", PRIO_LOW );
  SetAffinityOne( ctx->waker_id, 0 );
  StartTask( ctx->waker_id, Waker, ctx );
  memcpy( &ctx->cond_before, &ctx->cond_obj, sizeof( ctx->cond_before ) );
  ctx->status = pthread_cond_wait( ctx->cond, ctx->mutex );
  ctx->blocked = ctx->woken;
}

static void CPthreadReqCondWait_Cleanup( CPthreadReqCondWait_Context *ctx )
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

static const CPthreadReqCondWait_Entry
CPthreadReqCondWait_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCondWait_Post_Status_NA,
    CPthreadReqCondWait_Post_Init_NA, CPthreadReqCondWait_Post_Binding_NA,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_NA },
  { 1, 0, 0, 0, 0, 1, 1, 1, CPthreadReqCondWait_Post_Status_NA,
    CPthreadReqCondWait_Post_Init_NA, CPthreadReqCondWait_Post_Binding_NA,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCondWait_Post_Status_Ok,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_NA,
    CPthreadReqCondWait_Post_Owner_Caller,
    CPthreadReqCondWait_Post_Blocking_Yes },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_NA,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_Yes },
  { 0, 0, 0, 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Perm,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Mutex,
    CPthreadReqCondWait_Post_Owner_Other, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Perm,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Mutex,
    CPthreadReqCondWait_Post_Owner_Nobody, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 0, 0, 0, 1, 1, CPthreadReqCondWait_Post_Status_Perm,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Mutex,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 0, 0, 1, 1, 1, CPthreadReqCondWait_Post_Status_Perm,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_None,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_Caller, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_Other, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 0, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_Nobody, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 0, 0, 0, 1, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 0, 0, 1, 1, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 0, 0, 0, CPthreadReqCondWait_Post_Status_Ok,
    CPthreadReqCondWait_Post_Init_Default, CPthreadReqCondWait_Post_Binding_NA,
    CPthreadReqCondWait_Post_Owner_Caller,
    CPthreadReqCondWait_Post_Blocking_Yes },
  { 0, 0, 0, 1, 0, 0, 0, 0, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Default, CPthreadReqCondWait_Post_Binding_NA,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_Yes },
  { 0, 0, 0, 1, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Perm,
    CPthreadReqCondWait_Post_Init_Default,
    CPthreadReqCondWait_Post_Binding_Mutex,
    CPthreadReqCondWait_Post_Owner_Other, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Perm,
    CPthreadReqCondWait_Post_Init_Default,
    CPthreadReqCondWait_Post_Binding_Mutex,
    CPthreadReqCondWait_Post_Owner_Nobody, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 0, 1, 1, CPthreadReqCondWait_Post_Status_Perm,
    CPthreadReqCondWait_Post_Init_Default,
    CPthreadReqCondWait_Post_Binding_Mutex, CPthreadReqCondWait_Post_Owner_NA,
    CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 1, 1, 1, CPthreadReqCondWait_Post_Status_Perm,
    CPthreadReqCondWait_Post_Init_Default,
    CPthreadReqCondWait_Post_Binding_None, CPthreadReqCondWait_Post_Owner_NA,
    CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_Caller, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_Other, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 0, 0, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_Nobody, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 0, 1, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 0, 1, 0, 1, 1, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_Nop, CPthreadReqCondWait_Post_Binding_Nop,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_No },
  { 0, 0, 1, 1, 1, 1, 1, 1, CPthreadReqCondWait_Post_Status_Inval,
    CPthreadReqCondWait_Post_Init_NA, CPthreadReqCondWait_Post_Binding_NA,
    CPthreadReqCondWait_Post_Owner_NA, CPthreadReqCondWait_Post_Blocking_No }
};

static const uint8_t
CPthreadReqCondWait_Map[] = {
  2, 3, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3,
  4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 0, 9, 0,
  10, 0, 11, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 14, 15, 0,
  16, 0, 17, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 20, 0, 21, 0, 22,
  0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqCondWait_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCondWait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqCondWait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqCondWait_Fixture = {
  .setup = CPthreadReqCondWait_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqCondWait_Teardown_Wrap,
  .scope = CPthreadReqCondWait_Scope,
  .initial_context = &CPthreadReqCondWait_Instance
};

static inline CPthreadReqCondWait_Entry CPthreadReqCondWait_PopEntry(
  CPthreadReqCondWait_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondWait_Entries[ CPthreadReqCondWait_Map[ index ] ];
}

static void CPthreadReqCondWait_SetPreConditionStates(
  CPthreadReqCondWait_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_CondInit_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCondWait_Pre_CondInit_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Binding_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCondWait_Pre_Binding_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Mutex_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqCondWait_Pre_Mutex_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_MutexInit_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqCondWait_Pre_MutexInit_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_MutexOwner_NA ) {
    ctx->Map.pcs[ 5 ] = CPthreadReqCondWait_Pre_MutexOwner_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Wake_NA ) {
    ctx->Map.pcs[ 6 ] = CPthreadReqCondWait_Pre_Wake_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }
}

static void CPthreadReqCondWait_TestVariant( CPthreadReqCondWait_Context *ctx )
{
  CPthreadReqCondWait_Pre_Cond_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondWait_Pre_CondInit_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCondWait_Pre_Binding_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCondWait_Pre_Mutex_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqCondWait_Pre_MutexInit_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqCondWait_Pre_MutexOwner_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqCondWait_Pre_Wake_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CPthreadReqCondWait_Action( ctx );
  CPthreadReqCondWait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCondWait_Post_Init_Check( ctx, ctx->Map.entry.Post_Init );
  CPthreadReqCondWait_Post_Binding_Check( ctx, ctx->Map.entry.Post_Binding );
  CPthreadReqCondWait_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  CPthreadReqCondWait_Post_Blocking_Check( ctx, ctx->Map.entry.Post_Blocking );
}

/**
 * @fn void T_case_body_CPthreadReqCondWait( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqCondWait, &CPthreadReqCondWait_Fixture )
{
  CPthreadReqCondWait_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCondWait_Pre_Cond_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqCondWait_Pre_Cond_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCondWait_Pre_CondInit_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCondWait_Pre_CondInit_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCondWait_Pre_Binding_None;
        ctx->Map.pci[ 2 ] < CPthreadReqCondWait_Pre_Binding_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqCondWait_Pre_Mutex_Obj;
          ctx->Map.pci[ 3 ] < CPthreadReqCondWait_Pre_Mutex_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqCondWait_Pre_MutexInit_Yes;
            ctx->Map.pci[ 4 ] < CPthreadReqCondWait_Pre_MutexInit_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CPthreadReqCondWait_Pre_MutexOwner_Caller;
              ctx->Map.pci[ 5 ] < CPthreadReqCondWait_Pre_MutexOwner_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CPthreadReqCondWait_Pre_Wake_Broadcast;
                ctx->Map.pci[ 6 ] < CPthreadReqCondWait_Pre_Wake_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                ctx->Map.entry = CPthreadReqCondWait_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                CPthreadReqCondWait_SetPreConditionStates( ctx );
                CPthreadReqCondWait_Prepare( ctx );
                CPthreadReqCondWait_TestVariant( ctx );
                CPthreadReqCondWait_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
