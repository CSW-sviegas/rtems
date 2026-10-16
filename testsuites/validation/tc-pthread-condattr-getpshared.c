/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCondattrGetpshared
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

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCondattrGetpshared \
 *   spec:/c/pthread/req/condattr-getpshared
 *
 * @{
 */

typedef enum {
  CPthreadReqCondattrGetpshared_Pre_Attr_Valid,
  CPthreadReqCondattrGetpshared_Pre_Attr_Null,
  CPthreadReqCondattrGetpshared_Pre_Attr_NA
} CPthreadReqCondattrGetpshared_Pre_Attr;

typedef enum {
  CPthreadReqCondattrGetpshared_Post_Status_Zero,
  CPthreadReqCondattrGetpshared_Post_Status_EINVAL,
  CPthreadReqCondattrGetpshared_Post_Status_NA
} CPthreadReqCondattrGetpshared_Post_Status;

typedef enum {
  CPthreadReqCondattrGetpshared_Post_PsharedObj_Set,
  CPthreadReqCondattrGetpshared_Post_PsharedObj_Nop,
  CPthreadReqCondattrGetpshared_Post_PsharedObj_NA
} CPthreadReqCondattrGetpshared_Post_PsharedObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_PsharedObj : 2;
} CPthreadReqCondattrGetpshared_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/condattr-getpshared test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_condattr_t *attr;

  /**
   * @brief This member specifies the `pshared` parameter value.
   */
  int *pshared;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the attribute object.
   */
  pthread_condattr_t attr_obj;

  /**
   * @brief This member contains the pshared value.
   */
  int pshared_obj;

  /**
   * @brief This member contains the pshared output object.
   */
  int pshared_value;

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
    CPthreadReqCondattrGetpshared_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCondattrGetpshared_Context;

static CPthreadReqCondattrGetpshared_Context
  CPthreadReqCondattrGetpshared_Instance;

static const char * const CPthreadReqCondattrGetpshared_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqCondattrGetpshared_PreDesc[] = {
  CPthreadReqCondattrGetpshared_PreDesc_Attr,
  NULL
};

static void CPthreadReqCondattrGetpshared_Pre_Attr_Prepare(
  CPthreadReqCondattrGetpshared_Context *ctx,
  CPthreadReqCondattrGetpshared_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Pre_Attr_Valid: {
      /*
       * While the `attr` parameter references a valid condition variable
       * attributes object.
       */
      ctx->attr = &ctx->attr_obj;
      T_eq_int( pthread_condattr_init( &ctx->attr_obj ), 0 );
      T_eq_int(
        pthread_condattr_setpshared( &ctx->attr_obj, ctx->pshared_obj ),
        0
      );
      break;
    }

    case CPthreadReqCondattrGetpshared_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqCondattrGetpshared_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Post_Status_Check(
  CPthreadReqCondattrGetpshared_Context    *ctx,
  CPthreadReqCondattrGetpshared_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Post_Status_Zero: {
      /*
       * The return value of pthread_condattr_getpshared() shall be equal to
       * zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_Status_EINVAL: {
      /*
       * The return value of pthread_condattr_getpshared() shall be equal to
       * EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Post_PsharedObj_Check(
  CPthreadReqCondattrGetpshared_Context        *ctx,
  CPthreadReqCondattrGetpshared_Post_PsharedObj state
)
{
  switch ( state ) {
    case CPthreadReqCondattrGetpshared_Post_PsharedObj_Set: {
      /*
       * The value of the object referenced by the `pshared` parameter shall be
       * set to the process-shared attribute of the condition variable
       * attributes object referenced by the `attr` parameter.
       */
      T_eq_int( ctx->pshared_value, ctx->pshared_obj );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_PsharedObj_Nop: {
      /*
       * The value of the object referenced by the `pshared` parameter shall
       * not be modified.
       */
      T_eq_int( ctx->pshared_value, -1 );
      break;
    }

    case CPthreadReqCondattrGetpshared_Post_PsharedObj_NA:
      break;
  }
}

static void CPthreadReqCondattrGetpshared_Prepare(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  ctx->pshared_obj = PTHREAD_PROCESS_SHARED;
  ctx->pshared_value = -1;
  ctx->pshared = &ctx->pshared_value;
}

static void CPthreadReqCondattrGetpshared_Action(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  ctx->retval = pthread_condattr_getpshared( ctx->attr, ctx->pshared );
}

static const CPthreadReqCondattrGetpshared_Entry
CPthreadReqCondattrGetpshared_Entries[] = {
  { 0, 0, CPthreadReqCondattrGetpshared_Post_Status_Zero,
    CPthreadReqCondattrGetpshared_Post_PsharedObj_Set },
  { 0, 0, CPthreadReqCondattrGetpshared_Post_Status_EINVAL,
    CPthreadReqCondattrGetpshared_Post_PsharedObj_Nop }
};

static const uint8_t
CPthreadReqCondattrGetpshared_Map[] = {
  0, 1
};

static size_t CPthreadReqCondattrGetpshared_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqCondattrGetpshared_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqCondattrGetpshared_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqCondattrGetpshared_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqCondattrGetpshared_Scope,
  .initial_context = &CPthreadReqCondattrGetpshared_Instance
};

static inline CPthreadReqCondattrGetpshared_Entry
CPthreadReqCondattrGetpshared_PopEntry(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCondattrGetpshared_Entries[
    CPthreadReqCondattrGetpshared_Map[ index ]
  ];
}

static void CPthreadReqCondattrGetpshared_TestVariant(
  CPthreadReqCondattrGetpshared_Context *ctx
)
{
  CPthreadReqCondattrGetpshared_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCondattrGetpshared_Action( ctx );
  CPthreadReqCondattrGetpshared_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqCondattrGetpshared_Post_PsharedObj_Check(
    ctx,
    ctx->Map.entry.Post_PsharedObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqCondattrGetpshared( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqCondattrGetpshared,
  &CPthreadReqCondattrGetpshared_Fixture
)
{
  CPthreadReqCondattrGetpshared_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqCondattrGetpshared_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqCondattrGetpshared_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqCondattrGetpshared_PopEntry( ctx );
    CPthreadReqCondattrGetpshared_Prepare( ctx );
    CPthreadReqCondattrGetpshared_TestVariant( ctx );
  }
}

/** @} */
