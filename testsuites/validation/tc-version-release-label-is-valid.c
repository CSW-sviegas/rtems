/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsVersionReqReleaseLabelIsValid
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsVersionReqReleaseLabelIsValid \
 *   spec:/rtems/version/req/release-label-is-valid
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsVersionReqReleaseLabelIsValid_Pre_Label_Empty,
  RtemsVersionReqReleaseLabelIsValid_Pre_Label_NonEmpty,
  RtemsVersionReqReleaseLabelIsValid_Pre_Label_NA
} RtemsVersionReqReleaseLabelIsValid_Pre_Label;

typedef enum {
  RtemsVersionReqReleaseLabelIsValid_Post_Result_True,
  RtemsVersionReqReleaseLabelIsValid_Post_Result_False,
  RtemsVersionReqReleaseLabelIsValid_Post_Result_NA
} RtemsVersionReqReleaseLabelIsValid_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Label_NA : 1;
  uint8_t Post_Result : 2;
} RtemsVersionReqReleaseLabelIsValid_Entry;

/**
 * @brief Test context for spec:/rtems/version/req/release-label-is-valid test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the
   *   rtems_version_release_label_is_valid() call.
   */
  bool result;

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
    RtemsVersionReqReleaseLabelIsValid_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsVersionReqReleaseLabelIsValid_Context;

static RtemsVersionReqReleaseLabelIsValid_Context
  RtemsVersionReqReleaseLabelIsValid_Instance;

static const char *const RtemsVersionReqReleaseLabelIsValid_PreDesc_Label[] =
  { "Empty", "NonEmpty", "NA" };

static const char *const *const RtemsVersionReqReleaseLabelIsValid_PreDesc[] =
  { RtemsVersionReqReleaseLabelIsValid_PreDesc_Label, NULL };

static void RtemsVersionReqReleaseLabelIsValid_Pre_Label_Prepare(
  RtemsVersionReqReleaseLabelIsValid_Pre_Label state
)
{
  switch ( state ) {
    case RtemsVersionReqReleaseLabelIsValid_Pre_Label_Empty: {
      /*
       * While the release label is empty.
       */
      SetReleaseLabel( "" );
      break;
    }

    case RtemsVersionReqReleaseLabelIsValid_Pre_Label_NonEmpty: {
      /*
       * While the release label is not empty.
       */
      SetReleaseLabel( "x" );
      break;
    }

    case RtemsVersionReqReleaseLabelIsValid_Pre_Label_NA:
      break;
  }
}

static void RtemsVersionReqReleaseLabelIsValid_Post_Result_Check(
  RtemsVersionReqReleaseLabelIsValid_Context    *ctx,
  RtemsVersionReqReleaseLabelIsValid_Post_Result state
)
{
  switch ( state ) {
    case RtemsVersionReqReleaseLabelIsValid_Post_Result_True: {
      /*
       * The return value of rtems_version_release_label_is_valid() shall be
       * true.
       */
      T_true( ctx->result );
      break;
    }

    case RtemsVersionReqReleaseLabelIsValid_Post_Result_False: {
      /*
       * The return value of rtems_version_release_label_is_valid() shall be
       * false.
       */
      T_false( ctx->result );
      break;
    }

    case RtemsVersionReqReleaseLabelIsValid_Post_Result_NA:
      break;
  }
}

static void RtemsVersionReqReleaseLabelIsValid_Action(
  RtemsVersionReqReleaseLabelIsValid_Context *ctx
)
{
  ctx->result = rtems_version_release_label_is_valid();
}

static void RtemsVersionReqReleaseLabelIsValid_Cleanup( void )
{
  SetReleaseLabel( NULL );
}

/* clang-format off */

static const RtemsVersionReqReleaseLabelIsValid_Entry
RtemsVersionReqReleaseLabelIsValid_Entries[] = {
  { 0, 0, RtemsVersionReqReleaseLabelIsValid_Post_Result_False },
  { 0, 0, RtemsVersionReqReleaseLabelIsValid_Post_Result_True }
};

static const uint8_t
RtemsVersionReqReleaseLabelIsValid_Map[] = {
  0, 1
};

/* clang-format on */

static size_t RtemsVersionReqReleaseLabelIsValid_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsVersionReqReleaseLabelIsValid_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsVersionReqReleaseLabelIsValid_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsVersionReqReleaseLabelIsValid_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsVersionReqReleaseLabelIsValid_Scope,
  .initial_context = &RtemsVersionReqReleaseLabelIsValid_Instance
};

static inline RtemsVersionReqReleaseLabelIsValid_Entry
RtemsVersionReqReleaseLabelIsValid_PopEntry(
  RtemsVersionReqReleaseLabelIsValid_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsVersionReqReleaseLabelIsValid_Entries
    [ RtemsVersionReqReleaseLabelIsValid_Map[ index ] ];
}

static void RtemsVersionReqReleaseLabelIsValid_TestVariant(
  RtemsVersionReqReleaseLabelIsValid_Context *ctx
)
{
  RtemsVersionReqReleaseLabelIsValid_Pre_Label_Prepare( ctx->Map.pcs[ 0 ] );
  RtemsVersionReqReleaseLabelIsValid_Action( ctx );
  RtemsVersionReqReleaseLabelIsValid_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_RtemsVersionReqReleaseLabelIsValid( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsVersionReqReleaseLabelIsValid,
  &RtemsVersionReqReleaseLabelIsValid_Fixture
)
{
  RtemsVersionReqReleaseLabelIsValid_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsVersionReqReleaseLabelIsValid_Pre_Label_Empty;
    ctx->Map.pcs[ 0 ] < RtemsVersionReqReleaseLabelIsValid_Pre_Label_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsVersionReqReleaseLabelIsValid_PopEntry( ctx );
    RtemsVersionReqReleaseLabelIsValid_TestVariant( ctx );
    RtemsVersionReqReleaseLabelIsValid_Cleanup();
  }
}

/** @} */
