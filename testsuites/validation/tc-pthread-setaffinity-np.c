/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSetaffinityNp
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
#include <rtems/test-scheduler.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSetaffinityNp spec:/c/pthread/req/setaffinity-np
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqSetaffinityNp_Pre_Thread_NoObj,
  CPthreadReqSetaffinityNp_Pre_Thread_Valid,
  CPthreadReqSetaffinityNp_Pre_Thread_NA
} CPthreadReqSetaffinityNp_Pre_Thread;

typedef enum {
  CPthreadReqSetaffinityNp_Pre_CPUSetSize_Askew,
  CPthreadReqSetaffinityNp_Pre_CPUSetSize_Normal,
  CPthreadReqSetaffinityNp_Pre_CPUSetSize_Huge,
  CPthreadReqSetaffinityNp_Pre_CPUSetSize_NA
} CPthreadReqSetaffinityNp_Pre_CPUSetSize;

typedef enum {
  CPthreadReqSetaffinityNp_Pre_CPUSetOnline_Supported,
  CPthreadReqSetaffinityNp_Pre_CPUSetOnline_Unsupported,
  CPthreadReqSetaffinityNp_Pre_CPUSetOnline_NA
} CPthreadReqSetaffinityNp_Pre_CPUSetOnline;

typedef enum {
  CPthreadReqSetaffinityNp_Pre_CPUSetHuge_NotZero,
  CPthreadReqSetaffinityNp_Pre_CPUSetHuge_Zero,
  CPthreadReqSetaffinityNp_Pre_CPUSetHuge_NA
} CPthreadReqSetaffinityNp_Pre_CPUSetHuge;

typedef enum {
  CPthreadReqSetaffinityNp_Pre_CPUSet_Valid,
  CPthreadReqSetaffinityNp_Pre_CPUSet_Null,
  CPthreadReqSetaffinityNp_Pre_CPUSet_NA
} CPthreadReqSetaffinityNp_Pre_CPUSet;

typedef enum {
  CPthreadReqSetaffinityNp_Post_Status_Zero,
  CPthreadReqSetaffinityNp_Post_Status_EFAULT,
  CPthreadReqSetaffinityNp_Post_Status_ESRCH,
  CPthreadReqSetaffinityNp_Post_Status_EINVAL,
  CPthreadReqSetaffinityNp_Post_Status_NA
} CPthreadReqSetaffinityNp_Post_Status;

typedef enum {
  CPthreadReqSetaffinityNp_Post_SetAffinity_Set,
  CPthreadReqSetaffinityNp_Post_SetAffinity_Nop,
  CPthreadReqSetaffinityNp_Post_SetAffinity_NA
} CPthreadReqSetaffinityNp_Post_SetAffinity;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Thread_NA : 1;
  uint16_t Pre_CPUSetSize_NA : 1;
  uint16_t Pre_CPUSetOnline_NA : 1;
  uint16_t Pre_CPUSetHuge_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_SetAffinity : 2;
} CPthreadReqSetaffinityNp_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/setaffinity-np test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_2 scheduler_log;

  /**
   * @brief This member provides the object referenced by the `cpuset`
   *   parameter.
   */
  cpu_set_t cpuset_obj[ 2 ];

  /**
   * @brief This member contains the return value of the
   *   pthread_setaffinity_np() call.
   */
  int status;

  /**
   * @brief This member specifies the `thread` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member specifies the `cpusetsize` parameter value.
   */
  size_t cpusetsize;

  /**
   * @brief This member specifies the `cpuset` parameter value.
   */
  cpu_set_t *cpuset;

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
    CPthreadReqSetaffinityNp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSetaffinityNp_Context;

static CPthreadReqSetaffinityNp_Context
  CPthreadReqSetaffinityNp_Instance;

static const char * const CPthreadReqSetaffinityNp_PreDesc_Thread[] = {
  "NoObj",
  "Valid",
  "NA"
};

static const char * const CPthreadReqSetaffinityNp_PreDesc_CPUSetSize[] = {
  "Askew",
  "Normal",
  "Huge",
  "NA"
};

static const char * const CPthreadReqSetaffinityNp_PreDesc_CPUSetOnline[] = {
  "Supported",
  "Unsupported",
  "NA"
};

static const char * const CPthreadReqSetaffinityNp_PreDesc_CPUSetHuge[] = {
  "NotZero",
  "Zero",
  "NA"
};

static const char * const CPthreadReqSetaffinityNp_PreDesc_CPUSet[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqSetaffinityNp_PreDesc[] = {
  CPthreadReqSetaffinityNp_PreDesc_Thread,
  CPthreadReqSetaffinityNp_PreDesc_CPUSetSize,
  CPthreadReqSetaffinityNp_PreDesc_CPUSetOnline,
  CPthreadReqSetaffinityNp_PreDesc_CPUSetHuge,
  CPthreadReqSetaffinityNp_PreDesc_CPUSet,
  NULL
};

#define INVALID_THREAD_ID ( (pthread_t) 0xffffffff )

static void CPthreadReqSetaffinityNp_Pre_Thread_Prepare(
  CPthreadReqSetaffinityNp_Context   *ctx,
  CPthreadReqSetaffinityNp_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqSetaffinityNp_Pre_Thread_NoObj: {
      /*
       * While the `thread` parameter is not associated with a thread.
       */
      ctx->thread = INVALID_THREAD_ID;
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_Thread_Valid: {
      /*
       * While the `thread` parameter is associated with a thread.
       */
      ctx->thread = pthread_self();
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqSetaffinityNp_Pre_CPUSetSize_Prepare(
  CPthreadReqSetaffinityNp_Context       *ctx,
  CPthreadReqSetaffinityNp_Pre_CPUSetSize state
)
{
  switch ( state ) {
    case CPthreadReqSetaffinityNp_Pre_CPUSetSize_Askew: {
      /*
       * While the `cpusetsize` parameter is not an integral multiple of the
       * size of long.
       */
      ctx->cpusetsize = SIZE_MAX;
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSetSize_Normal: {
      /*
       * While the `cpusetsize` parameter is an integral multiple of the size
       * of long, while the `cpusetsize` parameter is less than or equal to the
       * maximum processor set size storable in the system.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj[ 0 ] );
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSetSize_Huge: {
      /*
       * While the `cpusetsize` parameter is an integral multiple of the size
       * of long, while the `cpusetsize` parameter is greater than the maximum
       * processor set size storable in the system.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj );
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSetSize_NA:
      break;
  }
}

static void CPthreadReqSetaffinityNp_Pre_CPUSetOnline_Prepare(
  CPthreadReqSetaffinityNp_Context         *ctx,
  CPthreadReqSetaffinityNp_Pre_CPUSetOnline state
)
{
  switch ( state ) {
    case CPthreadReqSetaffinityNp_Pre_CPUSetOnline_Supported: {
      /*
       * While the intersection of the processor set specified by the
       * `cpusetsize` and `cpuset` parameters and the set of online processors
       * represents an affinity set supported by the home scheduler of the
       * thread specified by the `thread` parameter at some point during the
       * pthread_setaffinity_np() call.
       */
      /* Already prepared */
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSetOnline_Unsupported: {
      /*
       * While the intersection of the processor set specified by the
       * `cpusetsize` and `cpuset` parameters and the set of online processors
       * represents an affinity set not supported by the home scheduler of the
       * thread specified by the `thread` parameter at some point during the
       * pthread_setaffinity_np() call.
       */
      CPU_CLR( 0, &ctx->cpuset_obj[ 0 ] );
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSetOnline_NA:
      break;
  }
}

static void CPthreadReqSetaffinityNp_Pre_CPUSetHuge_Prepare(
  CPthreadReqSetaffinityNp_Context       *ctx,
  CPthreadReqSetaffinityNp_Pre_CPUSetHuge state
)
{
  switch ( state ) {
    case CPthreadReqSetaffinityNp_Pre_CPUSetHuge_NotZero: {
      /*
       * While the processor set specified by the `cpusetsize` and `cpuset`
       * parameters contains at least one processor which is not storable in a
       * processor set supported by the system.
       */
      /* Already prepared */
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSetHuge_Zero: {
      /*
       * While the processor set specified by the `cpusetsize` and `cpuset`
       * parameters contains no processor which is not storable in a processor
       * set supported by the system.
       */
      CPU_ZERO( &ctx->cpuset_obj[ 1 ] );
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSetHuge_NA:
      break;
  }
}

static void CPthreadReqSetaffinityNp_Pre_CPUSet_Prepare(
  CPthreadReqSetaffinityNp_Context   *ctx,
  CPthreadReqSetaffinityNp_Pre_CPUSet state
)
{
  switch ( state ) {
    case CPthreadReqSetaffinityNp_Pre_CPUSet_Valid: {
      /*
       * While the `cpuset` parameter references an object of type cpu_set_t.
       */
      ctx->cpuset = &ctx->cpuset_obj[ 0 ];
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSet_Null: {
      /*
       * While the `cpuset` parameter is equal to NULL.
       */
      ctx->cpuset = NULL;
      break;
    }

    case CPthreadReqSetaffinityNp_Pre_CPUSet_NA:
      break;
  }
}

static void CPthreadReqSetaffinityNp_Post_Status_Check(
  CPthreadReqSetaffinityNp_Context    *ctx,
  CPthreadReqSetaffinityNp_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSetaffinityNp_Post_Status_Zero: {
      /*
       * The return value of pthread_setaffinity_np() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSetaffinityNp_Post_Status_EFAULT: {
      /*
       * The return value of pthread_setaffinity_np() shall be equal to EFAULT.
       */
      T_eq_int( ctx->status, EFAULT );
      break;
    }

    case CPthreadReqSetaffinityNp_Post_Status_ESRCH: {
      /*
       * The return value of pthread_setaffinity_np() shall be equal to ESRCH.
       */
      T_eq_int( ctx->status, ESRCH );
      break;
    }

    case CPthreadReqSetaffinityNp_Post_Status_EINVAL: {
      /*
       * The return value of pthread_setaffinity_np() shall be equal to EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqSetaffinityNp_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSetaffinityNp_Post_SetAffinity_Check(
  CPthreadReqSetaffinityNp_Context         *ctx,
  CPthreadReqSetaffinityNp_Post_SetAffinity state
)
{
  switch ( state ) {
    case CPthreadReqSetaffinityNp_Post_SetAffinity_Set: {
      /*
       * The processor affinity of the thread specified by the `thread`
       * parameter shall be set with respect to the home scheduler of the
       * thread at some point during the pthread_setaffinity_np() call.
       */
      #if defined( RTEMS_SMP )
        T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
        T_eq_int(
          ctx->scheduler_log.events[ 0 ].operation,
          T_SCHEDULER_SET_AFFINITY
        );
        T_eq_int(
          ctx->scheduler_log.events[ 0 ].set_affinity.status,
          STATUS_SUCCESSFUL
        );
      #else
        T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      #endif
      break;
    }

    case CPthreadReqSetaffinityNp_Post_SetAffinity_Nop: {
      /*
       * No processor affinity shall be modified by the
       * pthread_setaffinity_np() call.
       */
      #if defined( RTEMS_SMP )
        if ( ctx->scheduler_log.header.recorded == 1 ) {
          T_eq_int(
            ctx->scheduler_log.events[ 0 ].operation,
            T_SCHEDULER_SET_AFFINITY
          );
          T_eq_int(
            ctx->scheduler_log.events[ 0 ].set_affinity.status,
            STATUS_INVALID_NUMBER
          );
        } else {
          T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
        }
      #else
        T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      #endif
      break;
    }

    case CPthreadReqSetaffinityNp_Post_SetAffinity_NA:
      break;
  }
}

static void CPthreadReqSetaffinityNp_Prepare(
  CPthreadReqSetaffinityNp_Context *ctx
)
{
  CPU_FILL_S( sizeof( ctx->cpuset_obj ), &ctx->cpuset_obj[ 0 ] );
}

static void CPthreadReqSetaffinityNp_Action(
  CPthreadReqSetaffinityNp_Context *ctx
)
{
  T_scheduler_log *log;

  log = T_scheduler_record_2( &ctx->scheduler_log );
  T_null( log );

  ctx->status = pthread_setaffinity_np(
    ctx->thread,
    ctx->cpusetsize,
    ctx->cpuset
  );

  log = T_scheduler_record( NULL );
  T_eq_ptr( &log->header, &ctx->scheduler_log.header );
}

/* clang-format off */

static const CPthreadReqSetaffinityNp_Entry
CPthreadReqSetaffinityNp_Entries[] = {
  { 1, 0, 0, 0, 0, 0, CPthreadReqSetaffinityNp_Post_Status_NA,
    CPthreadReqSetaffinityNp_Post_SetAffinity_NA },
  { 0, 0, 0, 1, 1, 0, CPthreadReqSetaffinityNp_Post_Status_EFAULT,
    CPthreadReqSetaffinityNp_Post_SetAffinity_Nop },
  { 0, 0, 0, 1, 1, 0, CPthreadReqSetaffinityNp_Post_Status_ESRCH,
    CPthreadReqSetaffinityNp_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 1, 0, CPthreadReqSetaffinityNp_Post_Status_EINVAL,
    CPthreadReqSetaffinityNp_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 0, 0, CPthreadReqSetaffinityNp_Post_Status_Zero,
    CPthreadReqSetaffinityNp_Post_SetAffinity_Set },
  { 0, 0, 0, 0, 0, 0, CPthreadReqSetaffinityNp_Post_Status_EINVAL,
    CPthreadReqSetaffinityNp_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 1, 0, CPthreadReqSetaffinityNp_Post_Status_Zero,
    CPthreadReqSetaffinityNp_Post_SetAffinity_Set }
};

static const uint8_t
CPthreadReqSetaffinityNp_Map[] = {
  2, 1, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 3, 1,
  0, 0, 3, 0, 0, 0, 6, 1, 0, 0, 3, 0, 0, 0, 4, 1, 4, 0, 5, 0, 5, 0
};

/* clang-format on */

static size_t CPthreadReqSetaffinityNp_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSetaffinityNp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqSetaffinityNp_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqSetaffinityNp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqSetaffinityNp_Scope,
  .initial_context = &CPthreadReqSetaffinityNp_Instance
};

static inline CPthreadReqSetaffinityNp_Entry CPthreadReqSetaffinityNp_PopEntry(
  CPthreadReqSetaffinityNp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSetaffinityNp_Entries[
    CPthreadReqSetaffinityNp_Map[ index ]
  ];
}

static void CPthreadReqSetaffinityNp_SetPreConditionStates(
  CPthreadReqSetaffinityNp_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_CPUSetOnline_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqSetaffinityNp_Pre_CPUSetOnline_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_CPUSetHuge_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqSetaffinityNp_Pre_CPUSetHuge_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
}

static void CPthreadReqSetaffinityNp_TestVariant(
  CPthreadReqSetaffinityNp_Context *ctx
)
{
  CPthreadReqSetaffinityNp_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSetaffinityNp_Pre_CPUSetSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSetaffinityNp_Pre_CPUSetOnline_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqSetaffinityNp_Pre_CPUSetHuge_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqSetaffinityNp_Pre_CPUSet_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqSetaffinityNp_Action( ctx );
  CPthreadReqSetaffinityNp_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqSetaffinityNp_Post_SetAffinity_Check(
    ctx,
    ctx->Map.entry.Post_SetAffinity
  );
}

/**
 * @fn void T_case_body_CPthreadReqSetaffinityNp( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqSetaffinityNp,
  &CPthreadReqSetaffinityNp_Fixture
)
{
  CPthreadReqSetaffinityNp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqSetaffinityNp_Pre_Thread_NoObj;
    ctx->Map.pci[ 0 ] < CPthreadReqSetaffinityNp_Pre_Thread_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqSetaffinityNp_Pre_CPUSetSize_Askew;
      ctx->Map.pci[ 1 ] < CPthreadReqSetaffinityNp_Pre_CPUSetSize_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqSetaffinityNp_Pre_CPUSetOnline_Supported;
        ctx->Map.pci[ 2 ] < CPthreadReqSetaffinityNp_Pre_CPUSetOnline_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqSetaffinityNp_Pre_CPUSetHuge_NotZero;
          ctx->Map.pci[ 3 ] < CPthreadReqSetaffinityNp_Pre_CPUSetHuge_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqSetaffinityNp_Pre_CPUSet_Valid;
            ctx->Map.pci[ 4 ] < CPthreadReqSetaffinityNp_Pre_CPUSet_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = CPthreadReqSetaffinityNp_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            CPthreadReqSetaffinityNp_SetPreConditionStates( ctx );
            CPthreadReqSetaffinityNp_Prepare( ctx );
            CPthreadReqSetaffinityNp_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
