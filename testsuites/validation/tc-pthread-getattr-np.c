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
#include <sched.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqGetattrNp spec:/c/pthread/req/getattr-np
 *
 * @{
 */

typedef enum {
  CPthreadReqGetattrNp_Pre_Attr_Valid,
  CPthreadReqGetattrNp_Pre_Attr_Null,
  CPthreadReqGetattrNp_Pre_Attr_NA
} CPthreadReqGetattrNp_Pre_Attr;

typedef enum {
  CPthreadReqGetattrNp_Pre_Thread_Valid,
  CPthreadReqGetattrNp_Pre_Thread_NoObj,
  CPthreadReqGetattrNp_Pre_Thread_NA
} CPthreadReqGetattrNp_Pre_Thread;

typedef enum {
  CPthreadReqGetattrNp_Pre_WorkerConfig_Explicit,
  CPthreadReqGetattrNp_Pre_WorkerConfig_Inherited,
  CPthreadReqGetattrNp_Pre_WorkerConfig_NA
} CPthreadReqGetattrNp_Pre_WorkerConfig;

typedef enum {
  CPthreadReqGetattrNp_Pre_WorkerState_Initial,
  CPthreadReqGetattrNp_Pre_WorkerState_Modified,
  CPthreadReqGetattrNp_Pre_WorkerState_NA
} CPthreadReqGetattrNp_Pre_WorkerState;

typedef enum {
  CPthreadReqGetattrNp_Post_Status_Zero,
  CPthreadReqGetattrNp_Post_Status_EINVAL,
  CPthreadReqGetattrNp_Post_Status_ESRCH,
  CPthreadReqGetattrNp_Post_Status_NA
} CPthreadReqGetattrNp_Post_Status;

typedef enum {
  CPthreadReqGetattrNp_Post_FixedFieldsObj_Zero,
  CPthreadReqGetattrNp_Post_FixedFieldsObj_Set,
  CPthreadReqGetattrNp_Post_FixedFieldsObj_NA
} CPthreadReqGetattrNp_Post_FixedFieldsObj;

typedef enum {
  CPthreadReqGetattrNp_Post_WorkerConfigObj_Zero,
  CPthreadReqGetattrNp_Post_WorkerConfigObj_SetExplicit,
  CPthreadReqGetattrNp_Post_WorkerConfigObj_SetInherited,
  CPthreadReqGetattrNp_Post_WorkerConfigObj_NA
} CPthreadReqGetattrNp_Post_WorkerConfigObj;

typedef enum {
  CPthreadReqGetattrNp_Post_SchedulingObj_Zero,
  CPthreadReqGetattrNp_Post_SchedulingObj_SetInitial,
  CPthreadReqGetattrNp_Post_SchedulingObj_SetModified,
  CPthreadReqGetattrNp_Post_SchedulingObj_SetInherited,
  CPthreadReqGetattrNp_Post_SchedulingObj_NA
} CPthreadReqGetattrNp_Post_SchedulingObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Thread_NA : 1;
  uint16_t Pre_WorkerConfig_NA : 1;
  uint16_t Pre_WorkerState_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_FixedFieldsObj : 2;
  uint16_t Post_WorkerConfigObj : 2;
  uint16_t Post_SchedulingObj : 3;
} CPthreadReqGetattrNp_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/getattr-np test case.
 */
typedef struct {
  /**
   * @brief This member provides the barrier used to keep the
   *   explicit-scheduled worker thread alive for the duration of the test
   *   case.
   */
  pthread_barrier_t barrier;

  /**
   * @brief This member provides the barrier used to keep the inherit-scheduled
   *   worker thread alive for the duration of the test case.
   */
  pthread_barrier_t barrier2;

  /**
   * @brief This member provides the explicit-scheduled, joinable worker
   *   thread.
   */
  pthread_t worker_id;

  /**
   * @brief This member provides the inherit-scheduled, detached worker thread.
   */
  pthread_t worker_id2;

  /**
   * @brief This member contains the scheduling policy inherited by the worker
   *   referenced by the worker_id2 member at thread creation.
   */
  int worker2_schedpolicy;

  /**
   * @brief This member contains the priority inherited by the worker
   *   referenced by the worker_id2 member at thread creation.
   */
  int worker2_priority;

  /**
   * @brief This member specifies the `thread` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains the return value.
   */
  int retval;

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
    CPthreadReqGetattrNp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqGetattrNp_Context;

static CPthreadReqGetattrNp_Context
  CPthreadReqGetattrNp_Instance;

static const char * const CPthreadReqGetattrNp_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CPthreadReqGetattrNp_PreDesc_Thread[] = {
  "Valid",
  "NoObj",
  "NA"
};

static const char * const CPthreadReqGetattrNp_PreDesc_WorkerConfig[] = {
  "Explicit",
  "Inherited",
  "NA"
};

static const char * const CPthreadReqGetattrNp_PreDesc_WorkerState[] = {
  "Initial",
  "Modified",
  "NA"
};

static const char * const * const CPthreadReqGetattrNp_PreDesc[] = {
  CPthreadReqGetattrNp_PreDesc_Attr,
  CPthreadReqGetattrNp_PreDesc_Thread,
  CPthreadReqGetattrNp_PreDesc_WorkerConfig,
  CPthreadReqGetattrNp_PreDesc_WorkerState,
  NULL
};

#define WORKER_GUARD_SIZE ( (size_t) 4096 )

#define INVALID_THREAD_ID ( (pthread_t) 0xffffffff )

static char Worker_stack[ 64 * 1024 ] __attribute__(( aligned( 16 ) ));

static char Worker2_stack[ 64 * 1024 ] __attribute__(( aligned( 16 ) ));

static void *Worker( void *arg )
{
  pthread_barrier_t *barrier;

  barrier = (pthread_barrier_t *) arg;
  pthread_barrier_wait( barrier );

  return NULL;
}

static void *WorkerDetached( void *arg )
{
  pthread_barrier_t *barrier;

  barrier = (pthread_barrier_t *) arg;
  pthread_barrier_wait( barrier );
  pthread_barrier_wait( barrier );

  return NULL;
}

static void CPthreadReqGetattrNp_Pre_Attr_Prepare(
  CPthreadReqGetattrNp_Context *ctx,
  CPthreadReqGetattrNp_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references a thread attributes object.
       */
      memset( &ctx->attr_obj, 0xff, sizeof( ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = false;
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
    case CPthreadReqGetattrNp_Pre_Thread_Valid: {
      /*
       * While the `thread` parameter is associated with a thread.
       */
      /* Set by the WorkerConfig pre-condition. */
      break;
    }

    case CPthreadReqGetattrNp_Pre_Thread_NoObj: {
      /*
       * While the `thread` parameter is not associated with a thread.
       */
      ctx->thread = INVALID_THREAD_ID;
      break;
    }

    case CPthreadReqGetattrNp_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_WorkerConfig_Prepare(
  CPthreadReqGetattrNp_Context         *ctx,
  CPthreadReqGetattrNp_Pre_WorkerConfig state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_WorkerConfig_Explicit: {
      /*
       * While the thread associated with the `thread` parameter was created
       * with the PTHREAD_EXPLICIT_SCHED thread inherit scheduler attribute and
       * the PTHREAD_CREATE_JOINABLE thread detach state attribute.
       */
      ctx->thread = ctx->worker_id;
      break;
    }

    case CPthreadReqGetattrNp_Pre_WorkerConfig_Inherited: {
      /*
       * While the thread associated with the `thread` parameter was created
       * with the PTHREAD_INHERIT_SCHED thread inherit scheduler attribute and
       * the PTHREAD_CREATE_DETACHED thread detach state attribute.
       */
      ctx->thread = ctx->worker_id2;
      break;
    }

    case CPthreadReqGetattrNp_Pre_WorkerConfig_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Pre_WorkerState_Prepare(
  CPthreadReqGetattrNp_Context        *ctx,
  CPthreadReqGetattrNp_Pre_WorkerState state
)
{
  int                eno;
  struct sched_param param;

  switch ( state ) {
    case CPthreadReqGetattrNp_Pre_WorkerState_Initial: {
      /*
       * While the thread associated with the `thread` parameter has the
       * scheduling policy and scheduling parameters specified at thread
       * creation.
       */
      /* Already established at thread creation. */
      break;
    }

    case CPthreadReqGetattrNp_Pre_WorkerState_Modified: {
      /*
       * While the scheduling policy and scheduling parameters of the thread
       * associated with the `thread` parameter have been changed after thread
       * creation.
       */
      param.sched_priority = sched_get_priority_max( SCHED_FIFO );
      eno = pthread_setschedparam( ctx->worker_id, SCHED_FIFO, &param );
      T_eno_success( eno );
      break;
    }

    case CPthreadReqGetattrNp_Pre_WorkerState_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_Status_Check(
  CPthreadReqGetattrNp_Context    *ctx,
  CPthreadReqGetattrNp_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_Status_Zero: {
      /*
       * The return value of pthread_getattr_np() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqGetattrNp_Post_Status_EINVAL: {
      /*
       * The return value of pthread_getattr_np() shall be equal to EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqGetattrNp_Post_Status_ESRCH: {
      /*
       * The return value of pthread_getattr_np() shall be equal to ESRCH.
       */
      T_eq_int( ctx->retval, ESRCH );
      break;
    }

    case CPthreadReqGetattrNp_Post_Status_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_FixedFieldsObj_Check(
  CPthreadReqGetattrNp_Context            *ctx,
  CPthreadReqGetattrNp_Post_FixedFieldsObj state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_FixedFieldsObj_Zero: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * be uninitialized, with the thread contention scope attribute, the
       * thread guard size attribute, the CPU-time clock allowance, the
       * affinity set reference, and the affinity set size cleared to zero.
       */
      T_eq_int( ctx->attr_obj.is_initialized, false );
      T_eq_int( ctx->attr_obj.contentionscope, 0 );
      T_eq_sz( ctx->attr_obj.guardsize, 0 );
      T_eq_int( ctx->attr_obj.cputime_clock_allowed, 0 );
      T_eq_ptr( ctx->attr_obj.affinityset, NULL );
      T_eq_sz( ctx->attr_obj.affinitysetsize, 0 );
      break;
    }

    case CPthreadReqGetattrNp_Post_FixedFieldsObj_Set: {
      /*
       * The thread attributes object referenced by the `attr` parameter shall
       * be initialized.
       *
       * The thread contention scope attribute of the attributes object shall
       * be PTHREAD_SCOPE_PROCESS.
       *
       * The thread guard size attribute of the attributes object shall be
       * zero.
       *
       * The CPU-time clock shall be allowed for the attributes object.
       *
       * The affinity set reference of the attributes object shall reference
       * the preallocated affinity set embedded in the attributes object.
       *
       * The affinity set size of the attributes object shall be set to the
       * size of the preallocated affinity set of the attributes object.
       *
       * The preallocated affinity set of the attributes object shall contain
       * the processor affinity of the thread associated with the `thread`
       * parameter.
       */
      T_eq_int( ctx->attr_obj.is_initialized, true );
      T_eq_int( ctx->attr_obj.contentionscope, PTHREAD_SCOPE_PROCESS );
      T_eq_sz( ctx->attr_obj.guardsize, 0 );
      T_eq_int( ctx->attr_obj.cputime_clock_allowed, 1 );
      T_eq_ptr(
        ctx->attr_obj.affinityset,
        &ctx->attr_obj.affinitysetpreallocated
      );
      T_eq_sz(
        ctx->attr_obj.affinitysetsize,
        sizeof( ctx->attr_obj.affinitysetpreallocated )
      );
      T_true( CPU_ISSET( 0, &ctx->attr_obj.affinitysetpreallocated ) );
      break;
    }

    case CPthreadReqGetattrNp_Post_FixedFieldsObj_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_WorkerConfigObj_Check(
  CPthreadReqGetattrNp_Context             *ctx,
  CPthreadReqGetattrNp_Post_WorkerConfigObj state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_WorkerConfigObj_Zero: {
      /*
       * The thread stack address attribute, the thread stack size attribute,
       * the thread inherit scheduler attribute, and the thread detach state
       * attribute of the thread attributes object referenced by the `attr`
       * parameter shall be cleared to zero.
       */
      T_eq_ptr( ctx->attr_obj.stackaddr, NULL );
      T_eq_sz( ctx->attr_obj.stacksize, 0 );
      T_eq_int( ctx->attr_obj.inheritsched, 0 );
      T_eq_int( ctx->attr_obj.detachstate, 0 );
      break;
    }

    case CPthreadReqGetattrNp_Post_WorkerConfigObj_SetExplicit: {
      /*
       * The value of the thread stack address attribute of the thread
       * attributes object referenced by the `attr` parameter shall be set to
       * the stack address of the thread associated with the `thread`
       * parameter.
       *
       * The value of the thread stack size attribute of the attributes object
       * shall be set to the stack size of the thread associated with the
       * `thread` parameter.
       *
       * The thread inherit scheduler attribute of the attributes object shall
       * be PTHREAD_EXPLICIT_SCHED.
       *
       * The thread detach state attribute of the attributes object shall be
       * PTHREAD_CREATE_JOINABLE.
       */
      void  *stackaddr;
      size_t stacksize;
      int    detachstate;
      int    eno;

      eno = pthread_attr_getstackaddr( &ctx->attr_obj, &stackaddr );
      T_eno_success( eno );
      T_eq_ptr( stackaddr, Worker_stack );

      eno = pthread_attr_getstacksize( &ctx->attr_obj, &stacksize );
      T_eno_success( eno );
      T_true( stacksize > 0 );
      T_true( stacksize <= (size_t) sizeof( Worker_stack ) );

      /*
       * The pthread_attr_getinheritsched() accessor is not part of the
       * qualified subset, so the member is checked directly.
       */
      T_eq_int( ctx->attr_obj.inheritsched, PTHREAD_EXPLICIT_SCHED );

      eno = pthread_attr_getdetachstate( &ctx->attr_obj, &detachstate );
      T_eno_success( eno );
      T_eq_int( detachstate, PTHREAD_CREATE_JOINABLE );
      break;
    }

    case CPthreadReqGetattrNp_Post_WorkerConfigObj_SetInherited: {
      /*
       * The value of the thread stack address attribute of the thread
       * attributes object referenced by the `attr` parameter shall be set to
       * the stack address of the thread associated with the `thread`
       * parameter.
       *
       * The value of the thread stack size attribute of the attributes object
       * shall be set to the stack size of the thread associated with the
       * `thread` parameter.
       *
       * The thread inherit scheduler attribute of the attributes object shall
       * be PTHREAD_INHERIT_SCHED.
       *
       * The thread detach state attribute of the attributes object shall be
       * PTHREAD_CREATE_DETACHED.
       */
      void  *stackaddr;
      size_t stacksize;
      int    detachstate;
      int    eno;

      eno = pthread_attr_getstackaddr( &ctx->attr_obj, &stackaddr );
      T_eno_success( eno );
      T_eq_ptr( stackaddr, Worker2_stack );

      eno = pthread_attr_getstacksize( &ctx->attr_obj, &stacksize );
      T_eno_success( eno );
      T_true( stacksize > 0 );
      T_true( stacksize <= (size_t) sizeof( Worker2_stack ) );

      /*
       * The pthread_attr_getinheritsched() accessor is not part of the
       * qualified subset, so the member is checked directly.
       */
      T_eq_int( ctx->attr_obj.inheritsched, PTHREAD_INHERIT_SCHED );

      eno = pthread_attr_getdetachstate( &ctx->attr_obj, &detachstate );
      T_eno_success( eno );
      T_eq_int( detachstate, PTHREAD_CREATE_DETACHED );
      break;
    }

    case CPthreadReqGetattrNp_Post_WorkerConfigObj_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Post_SchedulingObj_Check(
  CPthreadReqGetattrNp_Context           *ctx,
  CPthreadReqGetattrNp_Post_SchedulingObj state
)
{
  switch ( state ) {
    case CPthreadReqGetattrNp_Post_SchedulingObj_Zero: {
      /*
       * The scheduling policy and the scheduling priority of the thread
       * attributes object referenced by the `attr` parameter shall be cleared
       * to zero.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, 0 );
      T_eq_int( ctx->attr_obj.schedparam.sched_priority, 0 );
      break;
    }

    case CPthreadReqGetattrNp_Post_SchedulingObj_SetInitial: {
      /*
       * The scheduling policy and scheduling parameters of the thread
       * attributes object referenced by the `attr` parameter shall be set to
       * the scheduling policy and scheduling parameters specified for the
       * thread associated with the `thread` parameter at thread creation.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_RR );
      T_eq_int(
        ctx->attr_obj.schedparam.sched_priority,
        sched_get_priority_min( SCHED_RR ) + 2
      );
      break;
    }

    case CPthreadReqGetattrNp_Post_SchedulingObj_SetModified: {
      /*
       * The scheduling policy and scheduling parameters of the attributes
       * object shall be set to the scheduling policy and scheduling parameters
       * last set for the thread associated with the `thread` parameter after
       * thread creation, and not to the scheduling policy and scheduling
       * parameters specified at thread creation.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, SCHED_FIFO );
      T_eq_int(
        ctx->attr_obj.schedparam.sched_priority,
        sched_get_priority_max( SCHED_FIFO )
      );
      break;
    }

    case CPthreadReqGetattrNp_Post_SchedulingObj_SetInherited: {
      /*
       * The scheduling policy and scheduling parameters of the attributes
       * object shall be set to the scheduling policy and scheduling parameters
       * that were in effect for the calling thread at the time the thread
       * associated with the `thread` parameter was created.
       */
      T_eq_int( ctx->attr_obj.schedpolicy, ctx->worker2_schedpolicy );
      T_eq_int( ctx->attr_obj.schedparam.sched_priority, ctx->worker2_priority );
      break;
    }

    case CPthreadReqGetattrNp_Post_SchedulingObj_NA:
      break;
  }
}

static void CPthreadReqGetattrNp_Setup( CPthreadReqGetattrNp_Context *ctx )
{
  pthread_attr_t     worker_attr;
  struct sched_param param;
  int                policy;
  int                eno;

  eno = pthread_barrier_init( &ctx->barrier, NULL, 2 );
  T_eno_success( eno );

  eno = pthread_barrier_init( &ctx->barrier2, NULL, 2 );
  T_eno_success( eno );

  eno = pthread_attr_init( &worker_attr );
  T_eno_success( eno );

  eno = pthread_attr_setstack(
    &worker_attr,
    Worker_stack,
    sizeof( Worker_stack )
  );
  T_eno_success( eno );

  eno = pthread_attr_setguardsize( &worker_attr, WORKER_GUARD_SIZE );
  T_eno_success( eno );

  eno = pthread_attr_setinheritsched( &worker_attr, PTHREAD_EXPLICIT_SCHED );
  T_eno_success( eno );

  eno = pthread_attr_setschedpolicy( &worker_attr, SCHED_RR );
  T_eno_success( eno );

  param.sched_priority = sched_get_priority_min( SCHED_RR ) + 2;
  eno = pthread_attr_setschedparam( &worker_attr, &param );
  T_eno_success( eno );

  eno = pthread_create( &ctx->worker_id, &worker_attr, Worker, &ctx->barrier );
  T_eno_success( eno );

  eno = pthread_attr_destroy( &worker_attr );
  T_eno_success( eno );

  eno = pthread_getschedparam( pthread_self(), &policy, &param );
  T_eno_success( eno );
  ctx->worker2_schedpolicy = policy;
  ctx->worker2_priority = param.sched_priority;

  eno = pthread_attr_init( &worker_attr );
  T_eno_success( eno );

  eno = pthread_attr_setstack(
    &worker_attr,
    Worker2_stack,
    sizeof( Worker2_stack )
  );
  T_eno_success( eno );

  eno = pthread_attr_setguardsize( &worker_attr, WORKER_GUARD_SIZE );
  T_eno_success( eno );

  eno = pthread_attr_setinheritsched( &worker_attr, PTHREAD_INHERIT_SCHED );
  T_eno_success( eno );

  eno = pthread_attr_setdetachstate( &worker_attr, PTHREAD_CREATE_DETACHED );
  T_eno_success( eno );

  eno = pthread_create(
    &ctx->worker_id2,
    &worker_attr,
    WorkerDetached,
    &ctx->barrier2
  );
  T_eno_success( eno );

  eno = pthread_attr_destroy( &worker_attr );
  T_eno_success( eno );
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
  int eno;

  eno = pthread_barrier_wait( &ctx->barrier );
  T_true( eno == 0 || eno == PTHREAD_BARRIER_SERIAL_THREAD );

  eno = pthread_join( ctx->worker_id, NULL );
  T_eno_success( eno );

  eno = pthread_barrier_destroy( &ctx->barrier );
  T_eno_success( eno );

  eno = pthread_barrier_wait( &ctx->barrier2 );
  T_true( eno == 0 || eno == PTHREAD_BARRIER_SERIAL_THREAD );

  eno = pthread_barrier_wait( &ctx->barrier2 );
  T_true( eno == 0 || eno == PTHREAD_BARRIER_SERIAL_THREAD );

  eno = pthread_barrier_destroy( &ctx->barrier2 );
  T_eno_success( eno );
}

static void CPthreadReqGetattrNp_Teardown_Wrap( void *arg )
{
  CPthreadReqGetattrNp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqGetattrNp_Teardown( ctx );
}

static void CPthreadReqGetattrNp_Action( CPthreadReqGetattrNp_Context *ctx )
{
  ctx->retval = pthread_getattr_np( ctx->thread, ctx->attr );
}

static const CPthreadReqGetattrNp_Entry
CPthreadReqGetattrNp_Entries[] = {
  { 0, 0, 0, 1, 1, CPthreadReqGetattrNp_Post_Status_ESRCH,
    CPthreadReqGetattrNp_Post_FixedFieldsObj_Zero,
    CPthreadReqGetattrNp_Post_WorkerConfigObj_Zero,
    CPthreadReqGetattrNp_Post_SchedulingObj_Zero },
  { 0, 0, 0, 1, 1, CPthreadReqGetattrNp_Post_Status_EINVAL,
    CPthreadReqGetattrNp_Post_FixedFieldsObj_NA,
    CPthreadReqGetattrNp_Post_WorkerConfigObj_NA,
    CPthreadReqGetattrNp_Post_SchedulingObj_NA },
  { 0, 0, 0, 0, 1, CPthreadReqGetattrNp_Post_Status_Zero,
    CPthreadReqGetattrNp_Post_FixedFieldsObj_Set,
    CPthreadReqGetattrNp_Post_WorkerConfigObj_SetInherited,
    CPthreadReqGetattrNp_Post_SchedulingObj_SetInherited },
  { 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_EINVAL,
    CPthreadReqGetattrNp_Post_FixedFieldsObj_NA,
    CPthreadReqGetattrNp_Post_WorkerConfigObj_NA,
    CPthreadReqGetattrNp_Post_SchedulingObj_NA },
  { 0, 0, 0, 0, 1, CPthreadReqGetattrNp_Post_Status_EINVAL,
    CPthreadReqGetattrNp_Post_FixedFieldsObj_NA,
    CPthreadReqGetattrNp_Post_WorkerConfigObj_NA,
    CPthreadReqGetattrNp_Post_SchedulingObj_NA },
  { 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Zero,
    CPthreadReqGetattrNp_Post_FixedFieldsObj_Set,
    CPthreadReqGetattrNp_Post_WorkerConfigObj_SetExplicit,
    CPthreadReqGetattrNp_Post_SchedulingObj_SetInitial },
  { 0, 0, 0, 0, 0, CPthreadReqGetattrNp_Post_Status_Zero,
    CPthreadReqGetattrNp_Post_FixedFieldsObj_Set,
    CPthreadReqGetattrNp_Post_WorkerConfigObj_SetExplicit,
    CPthreadReqGetattrNp_Post_SchedulingObj_SetModified }
};

static const uint8_t
CPthreadReqGetattrNp_Map[] = {
  5, 6, 2, 2, 0, 0, 0, 0, 3, 3, 4, 4, 1, 1, 1, 1
};

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
  return CPthreadReqGetattrNp_Entries[
    CPthreadReqGetattrNp_Map[ index ]
  ];
}

static void CPthreadReqGetattrNp_SetPreConditionStates(
  CPthreadReqGetattrNp_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_WorkerConfig_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqGetattrNp_Pre_WorkerConfig_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_WorkerState_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqGetattrNp_Pre_WorkerState_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CPthreadReqGetattrNp_TestVariant(
  CPthreadReqGetattrNp_Context *ctx
)
{
  CPthreadReqGetattrNp_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqGetattrNp_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqGetattrNp_Pre_WorkerConfig_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqGetattrNp_Pre_WorkerState_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqGetattrNp_Action( ctx );
  CPthreadReqGetattrNp_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqGetattrNp_Post_FixedFieldsObj_Check(
    ctx,
    ctx->Map.entry.Post_FixedFieldsObj
  );
  CPthreadReqGetattrNp_Post_WorkerConfigObj_Check(
    ctx,
    ctx->Map.entry.Post_WorkerConfigObj
  );
  CPthreadReqGetattrNp_Post_SchedulingObj_Check(
    ctx,
    ctx->Map.entry.Post_SchedulingObj
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
    ctx->Map.pci[ 0 ] = CPthreadReqGetattrNp_Pre_Attr_Valid;
    ctx->Map.pci[ 0 ] < CPthreadReqGetattrNp_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqGetattrNp_Pre_Thread_Valid;
      ctx->Map.pci[ 1 ] < CPthreadReqGetattrNp_Pre_Thread_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqGetattrNp_Pre_WorkerConfig_Explicit;
        ctx->Map.pci[ 2 ] < CPthreadReqGetattrNp_Pre_WorkerConfig_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqGetattrNp_Pre_WorkerState_Initial;
          ctx->Map.pci[ 3 ] < CPthreadReqGetattrNp_Pre_WorkerState_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqGetattrNp_PopEntry( ctx );
          CPthreadReqGetattrNp_SetPreConditionStates( ctx );
          CPthreadReqGetattrNp_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
