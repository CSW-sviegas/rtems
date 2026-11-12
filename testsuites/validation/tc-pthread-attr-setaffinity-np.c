/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetaffinityNp
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
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqAttrSetaffinityNp \
 *   spec:/c/pthread/req/attr-setaffinity-np
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetaffinityNp_Pre_Attr_Obj,
  CPthreadReqAttrSetaffinityNp_Pre_Attr_Null,
  CPthreadReqAttrSetaffinityNp_Pre_Attr_NA
} CPthreadReqAttrSetaffinityNp_Pre_Attr;

typedef enum {
  CPthreadReqAttrSetaffinityNp_Pre_Initialized_Yes,
  CPthreadReqAttrSetaffinityNp_Pre_Initialized_No,
  CPthreadReqAttrSetaffinityNp_Pre_Initialized_NA
} CPthreadReqAttrSetaffinityNp_Pre_Initialized;

typedef enum {
  CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Valid,
  CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Small,
  CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Large,
  CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_NA
} CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize;

typedef enum {
  CPthreadReqAttrSetaffinityNp_Pre_CPUSet_Obj,
  CPthreadReqAttrSetaffinityNp_Pre_CPUSet_Null,
  CPthreadReqAttrSetaffinityNp_Pre_CPUSet_NA
} CPthreadReqAttrSetaffinityNp_Pre_CPUSet;

typedef enum {
  CPthreadReqAttrSetaffinityNp_Post_Status_Ok,
  CPthreadReqAttrSetaffinityNp_Post_Status_Inval,
  CPthreadReqAttrSetaffinityNp_Post_Status_NA
} CPthreadReqAttrSetaffinityNp_Post_Status;

typedef enum {
  CPthreadReqAttrSetaffinityNp_Post_Affinity_CPUSet,
  CPthreadReqAttrSetaffinityNp_Post_Affinity_Nop,
  CPthreadReqAttrSetaffinityNp_Post_Affinity_NA
} CPthreadReqAttrSetaffinityNp_Post_Affinity;

typedef enum {
  CPthreadReqAttrSetaffinityNp_Post_Other_Nop,
  CPthreadReqAttrSetaffinityNp_Post_Other_NA
} CPthreadReqAttrSetaffinityNp_Post_Other;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Attr_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_CPUSetSize_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Affinity : 2;
  uint16_t Post_Other : 1;
} CPthreadReqAttrSetaffinityNp_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setaffinity-np test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `cpusetsize` parameter value.
   */
  size_t cpusetsize;

  /**
   * @brief This member specifies the `cpuset` parameter value.
   */
  const cpu_set_t *cpuset;

  /**
   * @brief This member contains the return value of the
   *   pthread_attr_setaffinity_np() call.
   */
  int status;

  /**
   * @brief This member contains the thread attributes object.
   */
  pthread_attr_t attr_obj;

  /**
   * @brief This member contains a copy of the thread attributes object before
   *   the call.
   */
  pthread_attr_t attr_before;

  /**
   * @brief This member contains the processor set object.
   */
  cpu_set_t cpuset_obj;

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
    CPthreadReqAttrSetaffinityNp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetaffinityNp_Context;

static CPthreadReqAttrSetaffinityNp_Context
  CPthreadReqAttrSetaffinityNp_Instance;

static const char *const CPthreadReqAttrSetaffinityNp_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqAttrSetaffinityNp_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqAttrSetaffinityNp_PreDesc_CPUSetSize[] =
  { "Valid", "Small", "Large", "NA" };

static const char *const CPthreadReqAttrSetaffinityNp_PreDesc_CPUSet[] =
  { "Obj", "Null", "NA" };

static const char *const *const CPthreadReqAttrSetaffinityNp_PreDesc[] = {
  CPthreadReqAttrSetaffinityNp_PreDesc_Attr,
  CPthreadReqAttrSetaffinityNp_PreDesc_Initialized,
  CPthreadReqAttrSetaffinityNp_PreDesc_CPUSetSize,
  CPthreadReqAttrSetaffinityNp_PreDesc_CPUSet,
  NULL
};

static void CPthreadReqAttrSetaffinityNp_Pre_Attr_Prepare(
  CPthreadReqAttrSetaffinityNp_Context *ctx,
  CPthreadReqAttrSetaffinityNp_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetaffinityNp_Pre_Attr_Obj: {
      /*
       * While the `attr` parameter references an object of type
       * pthread_attr_t.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqAttrSetaffinityNp_Pre_Initialized_Prepare(
  CPthreadReqAttrSetaffinityNp_Context        *ctx,
  CPthreadReqAttrSetaffinityNp_Pre_Initialized state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetaffinityNp_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `attr` parameter is an initialized
       * thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      CPU_ZERO( &ctx->attr_obj.affinitysetpreallocated );
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_Initialized_No: {
      /*
       * While the object referenced by the `attr` parameter is not an
       * initialized thread attributes object.
       */
      T_eno_success( pthread_attr_init( &ctx->attr_obj ) );
      ctx->attr_obj.is_initialized = false;
      CPU_ZERO( &ctx->attr_obj.affinitysetpreallocated );
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_Initialized_NA:
      break;
  }
}

static void CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Prepare(
  CPthreadReqAttrSetaffinityNp_Context       *ctx,
  CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Valid: {
      /*
       * While the `cpusetsize` parameter is equal to the size of an object of
       * type cpu_set_t.
       */
      ctx->cpusetsize = sizeof( cpu_set_t );
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Small: {
      /*
       * While the `cpusetsize` parameter is less than the size of an object of
       * type cpu_set_t.
       */
      ctx->cpusetsize = sizeof( cpu_set_t ) - 1;
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Large: {
      /*
       * While the `cpusetsize` parameter is greater than the size of an object
       * of type cpu_set_t.
       */
      ctx->cpusetsize = sizeof( cpu_set_t ) + 1;
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_NA:
      break;
  }
}

static void CPthreadReqAttrSetaffinityNp_Pre_CPUSet_Prepare(
  CPthreadReqAttrSetaffinityNp_Context   *ctx,
  CPthreadReqAttrSetaffinityNp_Pre_CPUSet state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetaffinityNp_Pre_CPUSet_Obj: {
      /*
       * While the `cpuset` parameter references an object of type cpu_set_t.
       */
      ctx->cpuset = &ctx->cpuset_obj;
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_CPUSet_Null: {
      /*
       * While the `cpuset` parameter is NULL.
       */
      ctx->cpuset = NULL;
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Pre_CPUSet_NA:
      break;
  }
}

static void CPthreadReqAttrSetaffinityNp_Post_Status_Check(
  CPthreadReqAttrSetaffinityNp_Context    *ctx,
  CPthreadReqAttrSetaffinityNp_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetaffinityNp_Post_Status_Ok: {
      /*
       * The return value of pthread_attr_setaffinity_np() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Post_Status_Inval: {
      /*
       * The return value of pthread_attr_setaffinity_np() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetaffinityNp_Post_Affinity_Check(
  CPthreadReqAttrSetaffinityNp_Context      *ctx,
  CPthreadReqAttrSetaffinityNp_Post_Affinity state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetaffinityNp_Post_Affinity_CPUSet: {
      /*
       * The processor affinity set of the thread attributes object referenced
       * by the `attr` parameter shall be equal to the processor set referenced
       * by the `cpuset` parameter.
       */
      T_eq_mem(
        &ctx->attr_obj.affinitysetpreallocated,
        &ctx->cpuset_obj,
        sizeof( ctx->cpuset_obj )
      );
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Post_Affinity_Nop: {
      /*
       * The processor affinity set of the thread attributes object referenced
       * by the `attr` parameter shall not be modified.
       */
      T_eq_mem(
        &ctx->attr_obj.affinitysetpreallocated,
        &ctx->attr_before.affinitysetpreallocated,
        sizeof( ctx->attr_obj.affinitysetpreallocated )
      );
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Post_Affinity_NA:
      break;
  }
}

static void CPthreadReqAttrSetaffinityNp_Post_Other_Check(
  CPthreadReqAttrSetaffinityNp_Context   *ctx,
  CPthreadReqAttrSetaffinityNp_Post_Other state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetaffinityNp_Post_Other_Nop: {
      /*
       * The content of the thread attributes object referenced by the `attr`
       * parameter, other than its processor affinity set, shall not be
       * modified.
       */
      pthread_attr_t expected;

      memcpy( &expected, &ctx->attr_before, sizeof( expected ) );
      memcpy(
        &expected.affinitysetpreallocated,
        &ctx->attr_obj.affinitysetpreallocated,
        sizeof( expected.affinitysetpreallocated )
      );
      T_eq_mem( &ctx->attr_obj, &expected, sizeof( expected ) );
      break;
    }

    case CPthreadReqAttrSetaffinityNp_Post_Other_NA:
      break;
  }
}

static void CPthreadReqAttrSetaffinityNp_Setup(
  CPthreadReqAttrSetaffinityNp_Context *ctx
)
{
  CPU_ZERO( &ctx->cpuset_obj );
  CPU_SET( 0, &ctx->cpuset_obj );
  CPU_SET( CPU_SETSIZE - 1, &ctx->cpuset_obj );
}

static void CPthreadReqAttrSetaffinityNp_Setup_Wrap( void *arg )
{
  CPthreadReqAttrSetaffinityNp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqAttrSetaffinityNp_Setup( ctx );
}

static void CPthreadReqAttrSetaffinityNp_Action(
  CPthreadReqAttrSetaffinityNp_Context *ctx
)
{
  memcpy( &ctx->attr_before, &ctx->attr_obj, sizeof( ctx->attr_before ) );
  ctx->status = pthread_attr_setaffinity_np(
    ctx->attr,
    ctx->cpusetsize,
    ctx->cpuset
  );
}

/* clang-format off */

static const CPthreadReqAttrSetaffinityNp_Entry
CPthreadReqAttrSetaffinityNp_Entries[] = {
  { 0, 0, 0, 0, 0, CPthreadReqAttrSetaffinityNp_Post_Status_Inval,
    CPthreadReqAttrSetaffinityNp_Post_Affinity_Nop,
    CPthreadReqAttrSetaffinityNp_Post_Other_Nop },
  { 0, 0, 1, 0, 0, CPthreadReqAttrSetaffinityNp_Post_Status_Inval,
    CPthreadReqAttrSetaffinityNp_Post_Affinity_NA,
    CPthreadReqAttrSetaffinityNp_Post_Other_NA },
  { 1, 0, 0, 0, 0, CPthreadReqAttrSetaffinityNp_Post_Status_NA,
    CPthreadReqAttrSetaffinityNp_Post_Affinity_NA,
    CPthreadReqAttrSetaffinityNp_Post_Other_NA },
  { 0, 0, 0, 0, 0, CPthreadReqAttrSetaffinityNp_Post_Status_Ok,
    CPthreadReqAttrSetaffinityNp_Post_Affinity_CPUSet,
    CPthreadReqAttrSetaffinityNp_Post_Other_Nop }
};

static const uint8_t
CPthreadReqAttrSetaffinityNp_Map[] = {
  3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2
};

/* clang-format on */

static size_t CPthreadReqAttrSetaffinityNp_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetaffinityNp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetaffinityNp_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetaffinityNp_Fixture = {
  .setup = CPthreadReqAttrSetaffinityNp_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetaffinityNp_Scope,
  .initial_context = &CPthreadReqAttrSetaffinityNp_Instance
};

static inline CPthreadReqAttrSetaffinityNp_Entry
CPthreadReqAttrSetaffinityNp_PopEntry(
  CPthreadReqAttrSetaffinityNp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetaffinityNp_Entries
    [ CPthreadReqAttrSetaffinityNp_Map[ index ] ];
}

static void CPthreadReqAttrSetaffinityNp_SetPreConditionStates(
  CPthreadReqAttrSetaffinityNp_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqAttrSetaffinityNp_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void CPthreadReqAttrSetaffinityNp_TestVariant(
  CPthreadReqAttrSetaffinityNp_Context *ctx
)
{
  CPthreadReqAttrSetaffinityNp_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetaffinityNp_Pre_Initialized_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  CPthreadReqAttrSetaffinityNp_Pre_CPUSet_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqAttrSetaffinityNp_Action( ctx );
  CPthreadReqAttrSetaffinityNp_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqAttrSetaffinityNp_Post_Affinity_Check(
    ctx,
    ctx->Map.entry.Post_Affinity
  );
  CPthreadReqAttrSetaffinityNp_Post_Other_Check(
    ctx,
    ctx->Map.entry.Post_Other
  );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetaffinityNp( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetaffinityNp,
  &CPthreadReqAttrSetaffinityNp_Fixture
)
{
  CPthreadReqAttrSetaffinityNp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqAttrSetaffinityNp_Pre_Attr_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqAttrSetaffinityNp_Pre_Attr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqAttrSetaffinityNp_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqAttrSetaffinityNp_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_Valid;
        ctx->Map.pci[ 2 ] < CPthreadReqAttrSetaffinityNp_Pre_CPUSetSize_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqAttrSetaffinityNp_Pre_CPUSet_Obj;
          ctx->Map.pci[ 3 ] < CPthreadReqAttrSetaffinityNp_Pre_CPUSet_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqAttrSetaffinityNp_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CPthreadReqAttrSetaffinityNp_SetPreConditionStates( ctx );
          CPthreadReqAttrSetaffinityNp_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
