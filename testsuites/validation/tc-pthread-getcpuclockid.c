/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqGetcpuclockid
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

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqGetcpuclockid spec:/c/pthread/req/getcpuclockid
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqGetcpuclockid_Pre_Parameters_Arbitrary,
  CPthreadReqGetcpuclockid_Pre_Parameters_NA
} CPthreadReqGetcpuclockid_Pre_Parameters;

typedef enum {
  CPthreadReqGetcpuclockid_Post_Status_MinusOne,
  CPthreadReqGetcpuclockid_Post_Status_NA
} CPthreadReqGetcpuclockid_Post_Status;

typedef enum {
  CPthreadReqGetcpuclockid_Post_Errno_ENOSYS,
  CPthreadReqGetcpuclockid_Post_Errno_NA
} CPthreadReqGetcpuclockid_Post_Errno;

typedef enum {
  CPthreadReqGetcpuclockid_Post_ClockId_Nop,
  CPthreadReqGetcpuclockid_Post_ClockId_NA
} CPthreadReqGetcpuclockid_Post_ClockId;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Parameters_NA : 1;
  uint8_t Post_Status : 1;
  uint8_t Post_Errno : 1;
  uint8_t Post_ClockId : 1;
} CPthreadReqGetcpuclockid_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/getcpuclockid test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `thread` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member specifies the `clock_id` parameter value.
   */
  clockid_t *clock_id;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the object referenced by the `clock_id`
   *   parameter.
   */
  clockid_t clock_id_obj;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    CPthreadReqGetcpuclockid_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqGetcpuclockid_Context;

static CPthreadReqGetcpuclockid_Context
  CPthreadReqGetcpuclockid_Instance;

static const char * const CPthreadReqGetcpuclockid_PreDesc_Parameters[] = {
  "Arbitrary",
  "NA"
};

static const char * const * const CPthreadReqGetcpuclockid_PreDesc[] = {
  CPthreadReqGetcpuclockid_PreDesc_Parameters,
  NULL
};

static void CPthreadReqGetcpuclockid_Pre_Parameters_Prepare(
  CPthreadReqGetcpuclockid_Context       *ctx,
  CPthreadReqGetcpuclockid_Pre_Parameters state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Pre_Parameters_Arbitrary: {
      /*
       * While the `thread` parameter has an arbitrary value, while the
       * `clock_id` parameter references an object of type clockid_t.
       */
      ctx->thread = pthread_self();
      ctx->clock_id_obj = -1;
      ctx->clock_id = &ctx->clock_id_obj;
      break;
    }

    case CPthreadReqGetcpuclockid_Pre_Parameters_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Post_Status_Check(
  CPthreadReqGetcpuclockid_Context    *ctx,
  CPthreadReqGetcpuclockid_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Post_Status_MinusOne: {
      /*
       * The return value of pthread_getcpuclockid() shall be equal to minus
       * one.
       */
      T_eq_int( ctx->retval, -1 );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_Status_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Post_Errno_Check(
  CPthreadReqGetcpuclockid_Post_Errno state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Post_Errno_ENOSYS: {
      /*
       * The errno shall be set to ENOSYS.
       */
      T_eq_int( errno, ENOSYS );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_Errno_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Post_ClockId_Check(
  CPthreadReqGetcpuclockid_Context     *ctx,
  CPthreadReqGetcpuclockid_Post_ClockId state
)
{
  switch ( state ) {
    case CPthreadReqGetcpuclockid_Post_ClockId_Nop: {
      /*
       * The value of the object referenced by the `clock_id` parameter shall
       * not be modified.
       */
      T_eq_int( ctx->clock_id_obj, -1 );
      break;
    }

    case CPthreadReqGetcpuclockid_Post_ClockId_NA:
      break;
  }
}

static void CPthreadReqGetcpuclockid_Prepare( void )
{
  errno = -1;
}

static void CPthreadReqGetcpuclockid_Action(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  ctx->retval = pthread_getcpuclockid( ctx->thread, ctx->clock_id );
}

/* clang-format off */

static const CPthreadReqGetcpuclockid_Entry
CPthreadReqGetcpuclockid_Entries[] = {
  { 0, 0, CPthreadReqGetcpuclockid_Post_Status_MinusOne,
    CPthreadReqGetcpuclockid_Post_Errno_ENOSYS,
    CPthreadReqGetcpuclockid_Post_ClockId_Nop }
};

static const uint8_t
CPthreadReqGetcpuclockid_Map[] = {
  0
};

/* clang-format on */

static size_t CPthreadReqGetcpuclockid_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqGetcpuclockid_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqGetcpuclockid_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqGetcpuclockid_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqGetcpuclockid_Scope,
  .initial_context = &CPthreadReqGetcpuclockid_Instance
};

static inline CPthreadReqGetcpuclockid_Entry CPthreadReqGetcpuclockid_PopEntry(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqGetcpuclockid_Entries[
    CPthreadReqGetcpuclockid_Map[ index ]
  ];
}

static void CPthreadReqGetcpuclockid_TestVariant(
  CPthreadReqGetcpuclockid_Context *ctx
)
{
  CPthreadReqGetcpuclockid_Pre_Parameters_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqGetcpuclockid_Action( ctx );
  CPthreadReqGetcpuclockid_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqGetcpuclockid_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CPthreadReqGetcpuclockid_Post_ClockId_Check(
    ctx,
    ctx->Map.entry.Post_ClockId
  );
}

/**
 * @fn void T_case_body_CPthreadReqGetcpuclockid( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqGetcpuclockid,
  &CPthreadReqGetcpuclockid_Fixture
)
{
  CPthreadReqGetcpuclockid_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqGetcpuclockid_Pre_Parameters_Arbitrary;
    ctx->Map.pcs[ 0 ] < CPthreadReqGetcpuclockid_Pre_Parameters_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqGetcpuclockid_PopEntry( ctx );
    CPthreadReqGetcpuclockid_Prepare();
    CPthreadReqGetcpuclockid_TestVariant( ctx );
  }
}

/** @} */
