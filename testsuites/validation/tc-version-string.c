/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsVersionReqVersion
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

#include <version-vc-key.h>
#include <rtems/version.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsVersionReqVersion spec:/rtems/version/req/version
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsVersionReqVersion_Pre_Label_Empty,
  RtemsVersionReqVersion_Pre_Label_NonEmpty,
  RtemsVersionReqVersion_Pre_Label_NA
} RtemsVersionReqVersion_Pre_Label;

typedef enum {
  RtemsVersionReqVersion_Post_Version_Label,
  RtemsVersionReqVersion_Post_Version_NoLabel,
  RtemsVersionReqVersion_Post_Version_NA
} RtemsVersionReqVersion_Post_Version;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Label_NA : 1;
  uint8_t Post_Version : 2;
} RtemsVersionReqVersion_Entry;

/**
 * @brief Test context for spec:/rtems/version/req/version test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the rtems_version() call.
   */
  const char *version;

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
    RtemsVersionReqVersion_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsVersionReqVersion_Context;

static RtemsVersionReqVersion_Context RtemsVersionReqVersion_Instance;

static const char *const RtemsVersionReqVersion_PreDesc_Label[] =
  { "Empty", "NonEmpty", "NA" };

static const char *const *const RtemsVersionReqVersion_PreDesc[] = {
  RtemsVersionReqVersion_PreDesc_Label,
  NULL
};

#if !defined( RTEMS_VERSION_CONTROL_KEY )
#error "the release label of the build is empty"
#endif

static void RtemsVersionReqVersion_Pre_Label_Prepare(
  RtemsVersionReqVersion_Pre_Label state
)
{
  switch ( state ) {
    case RtemsVersionReqVersion_Pre_Label_Empty: {
      /*
       * While the release label is empty.
       */
      SetReleaseLabel( "" );
      break;
    }

    case RtemsVersionReqVersion_Pre_Label_NonEmpty: {
      /*
       * While the release label is not empty.
       */
      SetReleaseLabel( NULL );
      break;
    }

    case RtemsVersionReqVersion_Pre_Label_NA:
      break;
  }
}

static void RtemsVersionReqVersion_Post_Version_Check(
  RtemsVersionReqVersion_Context     *ctx,
  RtemsVersionReqVersion_Post_Version state
)
{
  switch ( state ) {
    case RtemsVersionReqVersion_Post_Version_Label: {
      /*
       * The return value of rtems_version() shall be the major version number,
       * the minor version number and the revision of the RTEMS version in
       * decimal digits, separated by `.` characters and followed by a `.`
       * character and the release label.
       */
      T_eq_str( ctx->version, "7.0.0." RTEMS_VERSION_CONTROL_KEY );
      break;
    }

    case RtemsVersionReqVersion_Post_Version_NoLabel: {
      /*
       * The return value of rtems_version() shall be the major version number,
       * the minor version number and the revision of the RTEMS version in
       * decimal digits, separated by `.` characters.
       */
      T_eq_str( ctx->version, "7.0.0" );
      break;
    }

    case RtemsVersionReqVersion_Post_Version_NA:
      break;
  }
}

static void RtemsVersionReqVersion_Action(
  RtemsVersionReqVersion_Context *ctx
)
{
  ctx->version = rtems_version();
}

static void RtemsVersionReqVersion_Cleanup( void )
{
  SetReleaseLabel( NULL );
}

/* clang-format off */

static const RtemsVersionReqVersion_Entry
RtemsVersionReqVersion_Entries[] = {
  { 0, 0, RtemsVersionReqVersion_Post_Version_NoLabel },
  { 0, 0, RtemsVersionReqVersion_Post_Version_Label }
};

static const uint8_t
RtemsVersionReqVersion_Map[] = {
  0, 1
};

/* clang-format on */

static size_t RtemsVersionReqVersion_Scope( void *arg, char *buf, size_t n )
{
  RtemsVersionReqVersion_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsVersionReqVersion_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsVersionReqVersion_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsVersionReqVersion_Scope,
  .initial_context = &RtemsVersionReqVersion_Instance
};

static inline RtemsVersionReqVersion_Entry RtemsVersionReqVersion_PopEntry(
  RtemsVersionReqVersion_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsVersionReqVersion_Entries[ RtemsVersionReqVersion_Map[ index ] ];
}

static void RtemsVersionReqVersion_TestVariant(
  RtemsVersionReqVersion_Context *ctx
)
{
  RtemsVersionReqVersion_Pre_Label_Prepare( ctx->Map.pcs[ 0 ] );
  RtemsVersionReqVersion_Action( ctx );
  RtemsVersionReqVersion_Post_Version_Check(
    ctx,
    ctx->Map.entry.Post_Version
  );
}

/**
 * @fn void T_case_body_RtemsVersionReqVersion( void )
 */
T_TEST_CASE_FIXTURE( RtemsVersionReqVersion, &RtemsVersionReqVersion_Fixture )
{
  RtemsVersionReqVersion_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsVersionReqVersion_Pre_Label_Empty;
    ctx->Map.pcs[ 0 ] < RtemsVersionReqVersion_Pre_Label_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsVersionReqVersion_PopEntry( ctx );
    RtemsVersionReqVersion_TestVariant( ctx );
    RtemsVersionReqVersion_Cleanup();
  }
}

/** @} */
