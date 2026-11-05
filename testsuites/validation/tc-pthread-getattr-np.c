/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqGetattrNp
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
#include <sched.h>
#include <string.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqGetattrNp spec:/c/pthread/req/getattr-np
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqGetattrNp_Pre_Attr_Obj,
  CPthreadReqGetattrNp_Pre_Attr_Null,
  CPthreadReqGetattrNp_Pre_Attr_NA
} CPthreadReqGetattrNp_Pre_Attr;

typedef enum {
  CPthreadReqGetattrNp_Pre_Thread_Task,
  CPthreadReqGetattrNp_Pre_Thread_Invalid,
  CPthreadReqGetattrNp_Pre_Thread_NA
} CPthreadReqGetattrNp_Pre_Thread;

typedef enum {
  CPthreadReqGetattrNp_Pre_DetachState_Joinable,
  CPthreadReqGetattrNp_Pre_DetachState_Detached,
  CPthreadReqGetattrNp_Pre_DetachState_NA
} CPthreadReqGetattrNp_Pre_DetachState;

typedef enum {
  CPthreadReqGetattrNp_Pre_Timeslice_No,
  CPthreadReqGetattrNp_Pre_Timeslice_Yes,
  CPthreadReqGetattrNp_Pre_Timeslice_NA
} CPthreadReqGetattrNp_Pre_Timeslice;

typedef enum {
  CPthreadReqGetattrNp_Pre_Priority_High,
  CPthreadReqGetattrNp_Pre_Priority_Low,
  CPthreadReqGetattrNp_Pre_Priority_NA
} CPthreadReqGetattrNp_Pre_Priority;

typedef enum {
  CPthreadReqGetattrNp_Pre_CurrentPriority_Real,
  CPthreadReqGetattrNp_Pre_CurrentPriority_Raised,
  CPthreadReqGetattrNp_Pre_CurrentPriority_NA
} CPthreadReqGetattrNp_Pre_CurrentPriority;

typedef enum {
  CPthreadReqGetattrNp_Pre_Stack_Small,
  CPthreadReqGetattrNp_Pre_Stack_Large,
  CPthreadReqGetattrNp_Pre_Stack_NA
} CPthreadReqGetattrNp_Pre_Stack;

typedef enum {
  CPthreadReqGetattrNp_Pre_Affinity_All,
  CPthreadReqGetattrNp_Pre_Affinity_One,
  CPthreadReqGetattrNp_Pre_Affinity_NA
} CPthreadReqGetattrNp_Pre_Affinity;

typedef enum {
  CPthreadReqGetattrNp_Post_Status_Ok,
  CPthreadReqGetattrNp_Post_Status_Inval,
  CPthreadReqGetattrNp_Post_Status_Srch,
  CPthreadReqGetattrNp_Post_Status_NA
} CPthreadReqGetattrNp_Post_Status;

typedef enum {
  CPthreadReqGetattrNp_Post_Object_Zero,
  CPthreadReqGetattrNp_Post_Object_NA
} CPthreadReqGetattrNp_Post_Object;

typedef enum {
  CPthreadReqGetattrNp_Post_Initialized_Yes,
  CPthreadReqGetattrNp_Post_Initialized_NA
} CPthreadReqGetattrNp_Post_Initialized;

typedef enum {
  CPthreadReqGetattrNp_Post_StackAddr_Small,
  CPthreadReqGetattrNp_Post_StackAddr_Large,
  CPthreadReqGetattrNp_Post_StackAddr_NA
} CPthreadReqGetattrNp_Post_StackAddr;

typedef enum {
  CPthreadReqGetattrNp_Post_StackSize_Small,
  CPthreadReqGetattrNp_Post_StackSize_Large,
  CPthreadReqGetattrNp_Post_StackSize_NA
} CPthreadReqGetattrNp_Post_StackSize;

typedef enum {
  CPthreadReqGetattrNp_Post_DetachState_Joinable,
  CPthreadReqGetattrNp_Post_DetachState_Detached,
  CPthreadReqGetattrNp_Post_DetachState_NA
} CPthreadReqGetattrNp_Post_DetachState;

typedef enum {
  CPthreadReqGetattrNp_Post_InheritSched_Explicit,
  CPthreadReqGetattrNp_Post_InheritSched_NA
} CPthreadReqGetattrNp_Post_InheritSched;

typedef enum {
  CPthreadReqGetattrNp_Post_Policy_Fifo,
  CPthreadReqGetattrNp_Post_Policy_Other,
  CPthreadReqGetattrNp_Post_Policy_NA
} CPthreadReqGetattrNp_Post_Policy;

typedef enum {
  CPthreadReqGetattrNp_Post_Priority_High,
  CPthreadReqGetattrNp_Post_Priority_Low,
  CPthreadReqGetattrNp_Post_Priority_NA
} CPthreadReqGetattrNp_Post_Priority;

typedef enum {
  CPthreadReqGetattrNp_Post_Scope_Process,
  CPthreadReqGetattrNp_Post_Scope_NA
} CPthreadReqGetattrNp_Post_Scope;

typedef enum {
  CPthreadReqGetattrNp_Post_GuardSize_Zero,
  CPthreadReqGetattrNp_Post_GuardSize_NA
} CPthreadReqGetattrNp_Post_GuardSize;

typedef enum {
  CPthreadReqGetattrNp_Post_Affinity_All,
  CPthreadReqGetattrNp_Post_Affinity_One,
  CPthreadReqGetattrNp_Post_Affinity_NA
} CPthreadReqGetattrNp_Post_Affinity;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Attr_NA : 1;
  uint32_t Pre_Thread_NA : 1;
  uint32_t Pre_DetachState_NA : 1;
  uint32_t Pre_Timeslice_NA : 1;
  uint32_t Pre_Priority_NA : 1;
  uint32_t Pre_CurrentPriority_NA : 1;
  uint32_t Pre_Stack_NA : 1;
  uint32_t Pre_Affinity_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Object : 1;
  uint32_t Post_Initialized : 1;
  uint32_t Post_StackAddr : 2;
  uint32_t Post_StackSize : 2;
  uint32_t Post_DetachState : 2;
  uint32_t Post_InheritSched : 1;
  uint32_t Post_Policy : 2;
  uint32_t Post_Priority : 2;
  uint32_t Post_Scope : 1;
  uint32_t Post_GuardSize : 1;
  uint32_t Post_Affinity : 2;
} CPthreadReqGetattrNp_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/getattr-np test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `thread` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member contains the return value of the pthread_getattr_np()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member is true, if the `thread` parameter shall be associated
   *   with a task.
   */
  bool thread_is_task;

  /**
   * @brief This member is true, if the task shall be a detached thread.
   */
  bool detached;

  /**
   * @brief This member is true, if the task processor affinity of the task
   *   shall contain only the processor with index zero.
   */
  bool one_processor;

  /**
   * @brief This member specifies the initial modes of the task.
   */
  rtems_mode modes;

  /**
   * @brief This member specifies the initial priority of the task.
   */
  rtems_task_priority priority;

  /**
   * @brief This member specifies the task storage area of the task.
   */
  void *storage;

  /**
   * @brief This member specifies the size of the task storage area of the
   *   task.
   */
  size_t storage_size;

  /**
   * @brief This member contains the identifier of the task.
   */
  rtems_id task_id;

  /**
   * @brief This member contains the maximum priority of the home scheduler of
   *   the task.
   */
  rtems_task_priority maximum_priority;

  /**
   * @brief This member is true, if the task shall own the mutex.
   */
  bool raise_priority;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the identifier of the mutex with the
   *   RTEMS_PRIORITY_CEILING locking protocol.
   */
  rtems_id mutex_id;

  /**
   * @brief This member contains the address of a local variable of the task.
   */
  uintptr_t stack_object;

  /**
   * @brief This member contains the address of a thread-local object of the
   *   task.
   */
  uintptr_t tls_object;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 8 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 8 ];

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
    CPthreadReqGetattrNp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqGetattrNp_Context;

static CPthreadReqGetattrNp_Context CPthreadReqGetattrNp_Instance;

static const char *const CPthreadReqGetattrNp_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqGetattrNp_PreDesc_Thread[] =
  { "Task", "Invalid", "NA" };

static const char *const CPthreadReqGetattrNp_PreDesc_DetachState[] =
  { "Joinable", "Detached", "NA" };

static const char *const CPthreadReqGetattrNp_PreDesc_Timeslice[] =
  { "No", "Yes", "NA" };

static const char *const CPthreadReqGetattrNp_PreDesc_Priority[] =
  { "High", "Low", "NA" };

static const char *const CPthreadReqGetattrNp_PreDesc_CurrentPriority[] =
  { "Real", "Raised", "NA" };

static const char *const CPthreadReqGetattrNp_PreDesc_Stack[] =
  { "Small", "Large", "NA" };

static const char *const CPthreadReqGetattrNp_PreDesc_Affinity[] =
  { "All", "One", "NA" };

static const char *const *const CPthreadReqGetattrNp_PreDesc[] = {
  CPthreadReqGetattrNp_PreDesc_Attr,
  CPthreadReqGetattrNp_PreDesc_Thread,
  CPthreadReqGetattrNp_PreDesc_DetachState,
  CPthreadReqGetattrNp_PreDesc_Timeslice,
  CPthreadReqGetattrNp_PreDesc_Priority,
  CPthreadReqGetattrNp_PreDesc_CurrentPriority,
  CPthreadReqGetattrNp_PreDesc_Stack,
  CPthreadReqGetattrNp_PreDesc_Affinity,
  NULL
};

#define TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define STORAGE_SIZE_SMALL                             \
  RTEMS_ALIGN_UP(                                      \
    RTEMS_TASK_STORAGE_SIZE(                           \
      TEST_MAXIMUM_TLS_SIZE + TEST_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES                                  \
    ),                                                 \
    RTEMS_TASK_STORAGE_ALIGNMENT                       \
  )

#define STORAGE_SIZE_LARGE ( STORAGE_SIZE_SMALL + 1024 )

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT )
static char storage_small[ STORAGE_SIZE_SMALL ];

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT )
static char storage_large[ STORAGE_SIZE_LARGE ];

typedef CPthreadReqGetattrNp_Context Context;

static _Thread_local volatile int tls_object;

static void Worker( rtems_task_argument arg )
{
  Context     *ctx;
  volatile int stack_object;

  ctx = (Context *) arg;
  ctx->stack_object = (uintptr_t) &stack_object;
  ctx->tls_object = (uintptr_t) &tls_object;

  if ( ctx->raise_priority ) {
    ObtainMutex( ctx->mutex_id );
  }

  SendEvents( ctx->runner_id, RTEMS_EVENT_0 );
  (void) ReceiveAnyEvents();

  if ( ctx->raise_priority ) {
    ReleaseMutex( ctx->mutex_id );
  }

  SendEvents( ctx->runner_id, RTEMS_EVENT_0 );
  SuspendSelf();
}

static void CheckAffinity(
  const pthread_attr_t *attr,
  const cpu_set_t      *expected
)
{
  T_assert_eq_ptr( attr->affinityset, &attr->affinitysetpreallocated );
  T_eq_sz( attr->affinitysetsize, sizeof( attr->affinitysetpreallocated ) );
  T_true( CPU_EQUAL( attr->affinityset, expected ) );
}

static void CheckStackSize(
  const Context *ctx,
  const char    *storage,
  size_t         storage_size
)
{
  uintptr_t begin;
  uintptr_t end;

  begin = (uintptr_t) ctx->attr_obj.stackaddr;
  end = begin + ctx->attr_obj.stacksize;
  T_ge_uptr( ctx->stack_object, begin );
  T_lt_uptr( ctx->stack_object, end );
  T_ge_uptr( ctx->tls_object, end );
  T_lt_uptr( ctx->tls_object, (uintptr_t) storage + storage_size );
}

static void CPthreadReqGetattrNp_Pre_Attr_Prepare(
  CPthreadReqGetattrNp_Context *ctx,
  CPthreadReqGetattrNp_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_Thread_Prepare(
  CPthreadReqGetattrNp_Context   *ctx,
  CPthreadReqGetattrNp_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_Thread_Task: {
      /*
       * While the `thread` parameter is associated with a thread.
       */
      ctx->thread_is_task = true;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Thread_Invalid: {
      /*
       * While the `thread` parameter is associated with no thread.
       */
      ctx->thread_is_task = false;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_DetachState_Prepare(
  CPthreadReqGetattrNp_Context        *ctx,
  CPthreadReqGetattrNp_Pre_DetachState state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_DetachState_Joinable: {
      /*
       * While the thread associated with the `thread` parameter is a joinable
       * thread.
       */
      ctx->detached = false;
      break;
    }

    case CPthreadReqGetattrNp_Pre_DetachState_Detached: {
      /*
       * While the thread associated with the `thread` parameter is a detached
       * thread.
       */
      ctx->detached = true;
      break;
    }

    case CPthreadReqGetattrNp_Pre_DetachState_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_Timeslice_Prepare(
  CPthreadReqGetattrNp_Context      *ctx,
  CPthreadReqGetattrNp_Pre_Timeslice state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_Timeslice_No: {
      /*
       * While the thread associated with the `thread` parameter is a started
       * task with the RTEMS_NO_TIMESLICE mode.
       */
      ctx->modes = RTEMS_NO_TIMESLICE;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Timeslice_Yes: {
      /*
       * While the thread associated with the `thread` parameter is a started
       * task with the RTEMS_TIMESLICE mode.
       */
      ctx->modes = RTEMS_TIMESLICE;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Timeslice_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_Priority_Prepare(
  CPthreadReqGetattrNp_Context     *ctx,
  CPthreadReqGetattrNp_Pre_Priority state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_Priority_High: {
      /*
       * While the real priority of the thread associated with the `thread`
       * parameter is the high priority.
       */
      ctx->priority = PRIO_HIGH;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Priority_Low: {
      /*
       * While the real priority of the thread associated with the `thread`
       * parameter is the low priority, which is lower than the high priority.
       */
      ctx->priority = PRIO_LOW;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Priority_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_CurrentPriority_Prepare(
  CPthreadReqGetattrNp_Context            *ctx,
  CPthreadReqGetattrNp_Pre_CurrentPriority state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_CurrentPriority_Real: {
      /*
       * While the current priority of the thread associated with the `thread`
       * parameter is equal to its real priority.
       */
      ctx->raise_priority = false;
      break;
    }

    case CPthreadReqGetattrNp_Pre_CurrentPriority_Raised: {
      /*
       * While the thread associated with the `thread` parameter owns a mutex
       * with the RTEMS_PRIORITY_CEILING locking protocol and a ceiling
       * priority which is higher than the real priority of the thread, so that
       * its current priority is higher than its real priority.
       */
      ctx->raise_priority = true;
      break;
    }

    case CPthreadReqGetattrNp_Pre_CurrentPriority_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_Stack_Prepare(
  CPthreadReqGetattrNp_Context  *ctx,
  CPthreadReqGetattrNp_Pre_Stack state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_Stack_Small: {
      /*
       * While the thread associated with the `thread` parameter is a task with
       * a task storage area of the size which RTEMS_TASK_STORAGE_SIZE()
       * returns for the maximum thread-local storage size and the minimum
       * stack size of the test suite, aligned up to
       * RTEMS_TASK_STORAGE_ALIGNMENT.
       */
      ctx->storage = storage_small;
      ctx->storage_size = sizeof( storage_small );
      break;
    }

    case CPthreadReqGetattrNp_Pre_Stack_Large: {
      /*
       * While the thread associated with the `thread` parameter is a task with
       * a task storage area of the size which RTEMS_TASK_STORAGE_SIZE()
       * returns for the maximum thread-local storage size and the minimum
       * stack size of the test suite, aligned up to
       * RTEMS_TASK_STORAGE_ALIGNMENT, plus 1024 bytes.
       */
      ctx->storage = storage_large;
      ctx->storage_size = sizeof( storage_large );
      break;
    }

    case CPthreadReqGetattrNp_Pre_Stack_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_Affinity_Prepare(
  CPthreadReqGetattrNp_Context     *ctx,
  CPthreadReqGetattrNp_Pre_Affinity state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_Affinity_All: {
      /*
       * While the task processor affinity of the thread associated with the
       * `thread` parameter contains each processor of the system.
       */
      ctx->one_processor = false;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Affinity_One: {
      /*
       * While the task processor affinity of the thread associated with the
       * `thread` parameter contains only the processor with index zero.
       */
      ctx->one_processor = true;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Affinity_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_Status_Check(
  CPthreadReqGetattrNp_Context    *ctx,
  CPthreadReqGetattrNp_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_Status_Ok: {
      /*
       * The return value of pthread_getattr_np() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqGetattrNp_Post_Status_Inval: {
      /*
       * The return value of pthread_getattr_np() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqGetattrNp_Post_Status_Srch: {
      /*
       * The return value of pthread_getattr_np() shall be ESRCH.
       */
      T_eq_int( ctx->status, ESRCH );
      break;
    }

    case CPthreadReqGetattrNp_Post_Status_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_Object_Check(
  CPthreadReqGetattrNp_Context    *ctx,
  CPthreadReqGetattrNp_Post_Object state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_Object_Zero: {
      /*
       * Each byte of the object referenced by the `attr` parameter shall be
       * zero.
       */
      pthread_attr_t zero;

      memset( &zero, 0, sizeof( zero ) );
      T_eq_mem( &ctx->attr_obj, &zero, sizeof( zero ) );
      break;
    }

    case CPthreadReqGetattrNp_Post_Object_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_Initialized_Check(
  CPthreadReqGetattrNp_Context         *ctx,
  CPthreadReqGetattrNp_Post_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_Initialized_Yes: {
      /*
       * The object referenced by the `attr` parameter shall be an initialized
       * thread attributes object.
       */
      T_eq_int( ctx->attr_obj.is_initialized, 1 );
      break;
    }

    case CPthreadReqGetattrNp_Post_Initialized_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_StackAddr_Check(
  CPthreadReqGetattrNp_Context       *ctx,
  CPthreadReqGetattrNp_Post_StackAddr state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_StackAddr_Small: {
      /*
       * The thread stack address attribute of the thread attributes object
       * referenced by the `attr` parameter shall be the begin of the stack of
       * the thread.
       */
      T_eq_ptr( ctx->attr_obj.stackaddr, storage_small );
      break;
    }

    case CPthreadReqGetattrNp_Post_StackAddr_Large: {
      /*
       * The thread stack address attribute of the thread attributes object
       * referenced by the `attr` parameter shall be the begin of the stack of
       * the thread.
       */
      T_eq_ptr( ctx->attr_obj.stackaddr, storage_large );
      break;
    }

    case CPthreadReqGetattrNp_Post_StackAddr_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_StackSize_Check(
  CPthreadReqGetattrNp_Context       *ctx,
  CPthreadReqGetattrNp_Post_StackSize state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_StackSize_Small: {
      /*
       * The thread stack size attribute of the thread attributes object
       * referenced by the `attr` parameter shall be the size of the stack of
       * the thread.
       */
      CheckStackSize( ctx, storage_small, sizeof( storage_small ) );
      break;
    }

    case CPthreadReqGetattrNp_Post_StackSize_Large: {
      /*
       * The thread stack size attribute of the thread attributes object
       * referenced by the `attr` parameter shall be the size of the stack of
       * the thread.
       */
      CheckStackSize( ctx, storage_large, sizeof( storage_large ) );
      break;
    }

    case CPthreadReqGetattrNp_Post_StackSize_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_DetachState_Check(
  CPthreadReqGetattrNp_Context         *ctx,
  CPthreadReqGetattrNp_Post_DetachState state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_DetachState_Joinable: {
      /*
       * The thread detach state attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_CREATE_JOINABLE.
       */
      T_eq_int( ctx->attr_obj.detachstate, PTHREAD_CREATE_JOINABLE );
      break;
    }

    case CPthreadReqGetattrNp_Post_DetachState_Detached: {
      /*
       * The thread detach state attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_CREATE_DETACHED.
       */
      T_eq_int( ctx->attr_obj.detachstate, PTHREAD_CREATE_DETACHED );
      break;
    }

    case CPthreadReqGetattrNp_Post_DetachState_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_InheritSched_Check(
  CPthreadReqGetattrNp_Context          *ctx,
  CPthreadReqGetattrNp_Post_InheritSched state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_InheritSched_Explicit: {
      /*
       * The thread inherit scheduler attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_EXPLICIT_SCHED.
       */
      T_eq_int( ctx->attr_obj.inheritsched, PTHREAD_EXPLICIT_SCHED );
      break;
    }

    case CPthreadReqGetattrNp_Post_InheritSched_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_Policy_Check(
  CPthreadReqGetattrNp_Context    *ctx,
  CPthreadReqGetattrNp_Post_Policy state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_Policy_Fifo: {
      /*
       * The scheduling policy of the thread attributes object referenced by
       * the `attr` parameter shall be SCHED_FIFO.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_FIFO );
      break;
    }

    case CPthreadReqGetattrNp_Post_Policy_Other: {
      /*
       * The scheduling policy of the thread attributes object referenced by
       * the `attr` parameter shall be SCHED_OTHER.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_OTHER );
      break;
    }

    case CPthreadReqGetattrNp_Post_Policy_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_Priority_Check(
  CPthreadReqGetattrNp_Context      *ctx,
  CPthreadReqGetattrNp_Post_Priority state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_Priority_High: {
      /*
       * The priority of the scheduling parameters of the thread attributes
       * object referenced by the `attr` parameter shall be the maximum
       * priority of the home scheduler of the thread minus the high real
       * priority.
       */
      T_eq_int(
        ctx->attr_obj.schedparam.sched_priority,
        (int) ( ctx->maximum_priority - PRIO_HIGH )
      );
      break;
    }

    case CPthreadReqGetattrNp_Post_Priority_Low: {
      /*
       * The priority of the scheduling parameters of the thread attributes
       * object referenced by the `attr` parameter shall be the maximum
       * priority of the home scheduler of the thread minus the low real
       * priority.
       */
      T_eq_int(
        ctx->attr_obj.schedparam.sched_priority,
        (int) ( ctx->maximum_priority - PRIO_LOW )
      );
      break;
    }

    case CPthreadReqGetattrNp_Post_Priority_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_Scope_Check(
  CPthreadReqGetattrNp_Context   *ctx,
  CPthreadReqGetattrNp_Post_Scope state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_Scope_Process: {
      /*
       * The thread contention scope attribute of the thread attributes object
       * referenced by the `attr` parameter shall be PTHREAD_SCOPE_PROCESS.
       */
      T_eq_int( ctx->attr_obj.contentionscope, PTHREAD_SCOPE_PROCESS );
      break;
    }

    case CPthreadReqGetattrNp_Post_Scope_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_GuardSize_Check(
  CPthreadReqGetattrNp_Context       *ctx,
  CPthreadReqGetattrNp_Post_GuardSize state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_GuardSize_Zero: {
      /*
       * The thread guard size attribute of the thread attributes object
       * referenced by the `attr` parameter shall be zero.
       */
      T_eq_sz( ctx->attr_obj.guardsize, 0 );
      break;
    }

    case CPthreadReqGetattrNp_Post_GuardSize_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_Affinity_Check(
  CPthreadReqGetattrNp_Context      *ctx,
  CPthreadReqGetattrNp_Post_Affinity state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_Affinity_All: {
      /*
       * The processor affinity set of the thread attributes object referenced
       * by the `attr` parameter shall be a cpu_set_t object which contains
       * each processor of the system and no other processor.
       */
      cpu_set_t expected;
      uint32_t  cpu_max;
      uint32_t  cpu_index;

      CPU_ZERO( &expected );
      cpu_max = rtems_scheduler_get_processor_maximum();

      for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
        CPU_SET( (int) cpu_index, &expected );
      }

      CheckAffinity( &ctx->attr_obj, &expected );
      break;
    }

    case CPthreadReqGetattrNp_Post_Affinity_One: {
      /*
       * The processor affinity set of the thread attributes object referenced
       * by the `attr` parameter shall be a cpu_set_t object which contains
       * only the processor with index zero.
       */
      cpu_set_t expected;

      CPU_ZERO( &expected );
      CPU_SET( 0, &expected );
      CheckAffinity( &ctx->attr_obj, &expected );
      break;
    }

    case CPthreadReqGetattrNp_Post_Affinity_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Setup( CPthreadReqGetattrNp_Context *ctx )
{
  ctx->runner_id = rtems_task_self();
  SetSelfPriority( PRIO_NORMAL );
  T_rsc_success( rtems_scheduler_get_maximum_priority(
    GetSelfScheduler(),
    &ctx->maximum_priority
  ) );
  T_rsc_success( rtems_semaphore_create(
    rtems_build_name( 'M', 'U', 'T', 'X' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    PRIO_VERY_HIGH,
    &ctx->mutex_id
  ) );
}

static void CPthreadReqGetattrNp_Setup_Wrap( void *arg )
{
  CPthreadReqGetattrNp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqGetattrNp_Setup( ctx );
}

static void CPthreadReqGetattrNp_Teardown( CPthreadReqGetattrNp_Context *ctx )
{
  DeleteMutex( ctx->mutex_id );
  RestoreRunnerPriority();
}

static void CPthreadReqGetattrNp_Teardown_Wrap( void *arg )
{
  CPthreadReqGetattrNp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqGetattrNp_Teardown( ctx );
}

static void CPthreadReqGetattrNp_Prepare( CPthreadReqGetattrNp_Context *ctx )
{
  ctx->detached = false;
  ctx->raise_priority = false;
  ctx->one_processor = false;
  ctx->modes = RTEMS_NO_TIMESLICE;
  ctx->priority = PRIO_HIGH;
  ctx->storage = storage_small;
  ctx->storage_size = sizeof( storage_small );
  ctx->task_id = 0;
  ctx->status = -1;
}

static void CPthreadReqGetattrNp_Action( CPthreadReqGetattrNp_Context *ctx )
{
  if ( ctx->thread_is_task ) {
    rtems_task_config config;

    memset( &config, 0, sizeof( config ) );
    config.name = rtems_build_name( 'A', 'T', 'T', 'R' );
    config.initial_priority = PRIO_HIGH;
    config.storage_area = ctx->storage;
    config.storage_size = ctx->storage_size;
    config.maximum_thread_local_storage_size = TEST_MAXIMUM_TLS_SIZE;
    config.initial_modes = ctx->modes;
    config.attributes = TASK_ATTRIBUTES;
    T_rsc_success( rtems_task_construct( &config, &ctx->task_id ) );

    if ( ctx->one_processor ) {
      cpu_set_t affinity;

      CPU_ZERO( &affinity );
      CPU_SET( 0, &affinity );
      T_rsc_success(
        rtems_task_set_affinity( ctx->task_id, sizeof( affinity ), &affinity )
      );
    }

    StartTask( ctx->task_id, Worker, ctx );
    (void) ReceiveAnyEvents();
    SetPriority( ctx->task_id, ctx->priority );
    ctx->thread = (pthread_t) ctx->task_id;

    if ( ctx->detached ) {
      T_eno_success( pthread_detach( ctx->thread ) );
    }
  } else {
    ctx->thread = INVALID_ID;
  }

  ctx->status = pthread_getattr_np( ctx->thread, ctx->attr );
}

static void CPthreadReqGetattrNp_Cleanup( CPthreadReqGetattrNp_Context *ctx )
{
  if ( ctx->task_id != 0 ) {
    SendEvents( ctx->task_id, RTEMS_EVENT_0 );
    (void) ReceiveAnyEvents();
    DeleteTask( ctx->task_id );
  }
}

/* clang-format off */

static const CPthreadReqGetattrNp_Entry
CPthreadReqGetattrNp_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Joinable,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Fifo,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_High,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Small,
    CPthreadReqGetattrNp_Post_StackSize_Small,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_All },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Ok,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_Yes,
    CPthreadReqGetattrNp_Post_StackAddr_Large,
    CPthreadReqGetattrNp_Post_StackSize_Large,
    CPthreadReqGetattrNp_Post_DetachState_Detached,
    CPthreadReqGetattrNp_Post_InheritSched_Explicit,
    CPthreadReqGetattrNp_Post_Policy_Other,
    CPthreadReqGetattrNp_Post_Priority_Low,
    CPthreadReqGetattrNp_Post_Scope_Process,
    CPthreadReqGetattrNp_Post_GuardSize_Zero,
    CPthreadReqGetattrNp_Post_Affinity_One },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_NA,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
#endif
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, CPthreadReqGetattrNp_Post_Status_Inval,
    CPthreadReqGetattrNp_Post_Object_NA,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA },
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, CPthreadReqGetattrNp_Post_Status_Srch,
    CPthreadReqGetattrNp_Post_Object_Zero,
    CPthreadReqGetattrNp_Post_Initialized_NA,
    CPthreadReqGetattrNp_Post_StackAddr_NA,
    CPthreadReqGetattrNp_Post_StackSize_NA,
    CPthreadReqGetattrNp_Post_DetachState_NA,
    CPthreadReqGetattrNp_Post_InheritSched_NA,
    CPthreadReqGetattrNp_Post_Policy_NA, CPthreadReqGetattrNp_Post_Priority_NA,
    CPthreadReqGetattrNp_Post_Scope_NA, CPthreadReqGetattrNp_Post_GuardSize_NA,
    CPthreadReqGetattrNp_Post_Affinity_NA }
};

static const uint8_t
CPthreadReqGetattrNp_Map[] = {
  1, 2, 3, 4, 1, 2, 3, 4, 5, 6, 7, 8, 5, 6, 7, 8, 9, 10, 11, 12, 9, 10, 11, 12,
  13, 14, 15, 16, 13, 14, 15, 16, 17, 18, 19, 20, 17, 18, 19, 20, 21, 22, 23,
  24, 21, 22, 23, 24, 25, 26, 27, 28, 25, 26, 27, 28, 29, 30, 31, 32, 29, 30,
  31, 32, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqGetattrNp_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqGetattrNp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqGetattrNp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqGetattrNp_Fixture = {
  .setup = CPthreadReqGetattrNp_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqGetattrNp_Teardown_Wrap,
  .scope = CPthreadReqGetattrNp_Scope,
  .initial_context = &CPthreadReqGetattrNp_Instance
};

static inline CPthreadReqGetattrNp_Entry CPthreadReqGetattrNp_PopEntry(
  CPthreadReqGetattrNp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqGetattrNp_Entries[ CPthreadReqGetattrNp_Map[ index ] ];
}

static void CPthreadReqGetattrNp_SetPreConditionStates(
  CPthreadReqGetattrNp_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_DetachState_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqGetattrNp_Pre_DetachState_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Timeslice_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqGetattrNp_Pre_Timeslice_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Priority_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqGetattrNp_Pre_Priority_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_CurrentPriority_NA ) {
    ctx->Map.pcs[ 5 ] = CPthreadReqGetattrNp_Pre_CurrentPriority_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Stack_NA ) {
    ctx->Map.pcs[ 6 ] = CPthreadReqGetattrNp_Pre_Stack_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_Affinity_NA ) {
    ctx->Map.pcs[ 7 ] = CPthreadReqGetattrNp_Pre_Affinity_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }
}

static void CPthreadReqGetattrNp_TestVariant(
  CPthreadReqGetattrNp_Context *ctx
)
{
  CPthreadReqGetattrNp_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqGetattrNp_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqGetattrNp_Pre_DetachState_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqGetattrNp_Pre_Timeslice_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqGetattrNp_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqGetattrNp_Pre_CurrentPriority_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqGetattrNp_Pre_Stack_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CPthreadReqGetattrNp_Pre_Affinity_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CPthreadReqGetattrNp_Action( ctx );
  CPthreadReqGetattrNp_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqGetattrNp_Post_Object_Check( ctx, ctx->Map.entry.Post_Object );
  CPthreadReqGetattrNp_Post_Initialized_Check(
    ctx,
    ctx->Map.entry.Post_Initialized
  );
  CPthreadReqGetattrNp_Post_StackAddr_Check(
    ctx,
    ctx->Map.entry.Post_StackAddr
  );
  CPthreadReqGetattrNp_Post_StackSize_Check(
    ctx,
    ctx->Map.entry.Post_StackSize
  );
  CPthreadReqGetattrNp_Post_DetachState_Check(
    ctx,
    ctx->Map.entry.Post_DetachState
  );
  CPthreadReqGetattrNp_Post_InheritSched_Check(
    ctx,
    ctx->Map.entry.Post_InheritSched
  );
  CPthreadReqGetattrNp_Post_Policy_Check( ctx, ctx->Map.entry.Post_Policy );
  CPthreadReqGetattrNp_Post_Priority_Check(
    ctx,
    ctx->Map.entry.Post_Priority
  );
  CPthreadReqGetattrNp_Post_Scope_Check( ctx, ctx->Map.entry.Post_Scope );
  CPthreadReqGetattrNp_Post_GuardSize_Check(
    ctx,
    ctx->Map.entry.Post_GuardSize
  );
  CPthreadReqGetattrNp_Post_Affinity_Check(
    ctx,
    ctx->Map.entry.Post_Affinity
  );
}

/**
 * @fn void T_case_body_CPthreadReqGetattrNp( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqGetattrNp, &CPthreadReqGetattrNp_Fixture )
{
  CPthreadReqGetattrNp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqGetattrNp_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqGetattrNp_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqGetattrNp_Pre_Thread_Task;
      ctx->Map.pci[ 1 ] < CPthreadReqGetattrNp_Pre_Thread_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqGetattrNp_Pre_DetachState_Joinable;
        ctx->Map.pci[ 2 ] < CPthreadReqGetattrNp_Pre_DetachState_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqGetattrNp_Pre_Timeslice_No;
          ctx->Map.pci[ 3 ] < CPthreadReqGetattrNp_Pre_Timeslice_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqGetattrNp_Pre_Priority_High;
            ctx->Map.pci[ 4 ] < CPthreadReqGetattrNp_Pre_Priority_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] =
                CPthreadReqGetattrNp_Pre_CurrentPriority_Real;
              ctx->Map.pci[ 5 ] < CPthreadReqGetattrNp_Pre_CurrentPriority_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CPthreadReqGetattrNp_Pre_Stack_Small;
                ctx->Map.pci[ 6 ] < CPthreadReqGetattrNp_Pre_Stack_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CPthreadReqGetattrNp_Pre_Affinity_All;
                  ctx->Map.pci[ 7 ] < CPthreadReqGetattrNp_Pre_Affinity_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  ctx->Map.entry = CPthreadReqGetattrNp_PopEntry( ctx );

                  if ( ctx->Map.entry.Skip ) {
                    continue;
                  }

                  CPthreadReqGetattrNp_SetPreConditionStates( ctx );
                  CPthreadReqGetattrNp_Prepare( ctx );
                  CPthreadReqGetattrNp_TestVariant( ctx );
                  CPthreadReqGetattrNp_Cleanup( ctx );
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
