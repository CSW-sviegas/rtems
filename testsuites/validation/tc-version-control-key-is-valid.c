/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsVersionReqControlKeyIsValid
 */

/*
 * Copyright (C) 2025, 2026 embedded brains GmbH & Co. KG
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

#include <rtems/version.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsVersionReqControlKeyIsValid \
 *   spec:/rtems/version/req/control-key-is-valid
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsVersionReqControlKeyIsValid_Pre_Key_Empty,
  RtemsVersionReqControlKeyIsValid_Pre_Key_NonEmpty,
  RtemsVersionReqControlKeyIsValid_Pre_Key_NA
} RtemsVersionReqControlKeyIsValid_Pre_Key;

typedef enum {
  RtemsVersionReqControlKeyIsValid_Post_Result_True,
  RtemsVersionReqControlKeyIsValid_Post_Result_False,
  RtemsVersionReqControlKeyIsValid_Post_Result_NA
} RtemsVersionReqControlKeyIsValid_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Key_NA : 1;
  uint8_t Post_Result : 2;
} RtemsVersionReqControlKeyIsValid_Entry;

/**
 * @brief Test context for spec:/rtems/version/req/control-key-is-valid test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the
   *   rtems_version_control_key_is_valid() call.
   */
  bool result;

  /**
   * @brief This member specifies the `key` parameter value.
   */
  const char *key;

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
    RtemsVersionReqControlKeyIsValid_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsVersionReqControlKeyIsValid_Context;

static RtemsVersionReqControlKeyIsValid_Context
  RtemsVersionReqControlKeyIsValid_Instance;

static const char *const RtemsVersionReqControlKeyIsValid_PreDesc_Key[] =
  { "Empty", "NonEmpty", "NA" };

static const char *const *const RtemsVersionReqControlKeyIsValid_PreDesc[] = {
  RtemsVersionReqControlKeyIsValid_PreDesc_Key,
  NULL
};

static void RtemsVersionReqControlKeyIsValid_Pre_Key_Prepare(
  RtemsVersionReqControlKeyIsValid_Context *ctx,
  RtemsVersionReqControlKeyIsValid_Pre_Key  state
)
{
  switch ( state ) {
    case RtemsVersionReqControlKeyIsValid_Pre_Key_Empty: {
      /*
       * While the `key` parameter references an empty string.
       */
      ctx->key = "";
      break;
    }

    case RtemsVersionReqControlKeyIsValid_Pre_Key_NonEmpty: {
      /*
       * While the `key` parameter references a string which is not empty.
       */
      ctx->key = "x";
      break;
    }

    case RtemsVersionReqControlKeyIsValid_Pre_Key_NA:
      break;
  }
}

static void RtemsVersionReqControlKeyIsValid_Post_Result_Check(
  RtemsVersionReqControlKeyIsValid_Context    *ctx,
  RtemsVersionReqControlKeyIsValid_Post_Result state
)
{
  switch ( state ) {
    case RtemsVersionReqControlKeyIsValid_Post_Result_True: {
      /*
       * The return value of rtems_version_control_key_is_valid() shall be
       * true.
       */
      T_true( ctx->result );
      break;
    }

    case RtemsVersionReqControlKeyIsValid_Post_Result_False: {
      /*
       * The return value of rtems_version_control_key_is_valid() shall be
       * false.
       */
      T_false( ctx->result );
      break;
    }

    case RtemsVersionReqControlKeyIsValid_Post_Result_NA:
      break;
  }
}

static void RtemsVersionReqControlKeyIsValid_Action(
  RtemsVersionReqControlKeyIsValid_Context *ctx
)
{
  ctx->result = rtems_version_control_key_is_valid( ctx->key );
}

/* clang-format off */

static const RtemsVersionReqControlKeyIsValid_Entry
RtemsVersionReqControlKeyIsValid_Entries[] = {
  { 0, 0, RtemsVersionReqControlKeyIsValid_Post_Result_False },
  { 0, 0, RtemsVersionReqControlKeyIsValid_Post_Result_True }
};

static const uint8_t
RtemsVersionReqControlKeyIsValid_Map[] = {
  0, 1
};

/* clang-format on */

static size_t RtemsVersionReqControlKeyIsValid_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsVersionReqControlKeyIsValid_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsVersionReqControlKeyIsValid_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsVersionReqControlKeyIsValid_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsVersionReqControlKeyIsValid_Scope,
  .initial_context = &RtemsVersionReqControlKeyIsValid_Instance
};

static inline RtemsVersionReqControlKeyIsValid_Entry
RtemsVersionReqControlKeyIsValid_PopEntry(
  RtemsVersionReqControlKeyIsValid_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsVersionReqControlKeyIsValid_Entries
    [ RtemsVersionReqControlKeyIsValid_Map[ index ] ];
}

static void RtemsVersionReqControlKeyIsValid_TestVariant(
  RtemsVersionReqControlKeyIsValid_Context *ctx
)
{
  RtemsVersionReqControlKeyIsValid_Pre_Key_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsVersionReqControlKeyIsValid_Action( ctx );
  RtemsVersionReqControlKeyIsValid_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_RtemsVersionReqControlKeyIsValid( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsVersionReqControlKeyIsValid,
  &RtemsVersionReqControlKeyIsValid_Fixture
)
{
  RtemsVersionReqControlKeyIsValid_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsVersionReqControlKeyIsValid_Pre_Key_Empty;
    ctx->Map.pcs[ 0 ] < RtemsVersionReqControlKeyIsValid_Pre_Key_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsVersionReqControlKeyIsValid_PopEntry( ctx );
    RtemsVersionReqControlKeyIsValid_TestVariant( ctx );
  }
}

/** @} */
