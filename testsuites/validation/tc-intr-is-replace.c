/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqIsReplace
 */

/*
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

#include <rtems.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqIsReplace spec:/rtems/intr/req/is-replace
 *
 * @ingroup TestsuitesValidationIntr
 *
 * The test stores the result in an object of type int. A result other than
 * zero or one fails the comparison with true and false.
 *
 * @{
 */

typedef enum {
  RtemsIntrReqIsReplace_Pre_Sharing_Unique,
  RtemsIntrReqIsReplace_Pre_Sharing_Shared,
  RtemsIntrReqIsReplace_Pre_Sharing_NA
} RtemsIntrReqIsReplace_Pre_Sharing;

typedef enum {
  RtemsIntrReqIsReplace_Pre_Replace_Yes,
  RtemsIntrReqIsReplace_Pre_Replace_No,
  RtemsIntrReqIsReplace_Pre_Replace_NA
} RtemsIntrReqIsReplace_Pre_Replace;

typedef enum {
  RtemsIntrReqIsReplace_Pre_Enable_Enable,
  RtemsIntrReqIsReplace_Pre_Enable_NoEnable,
  RtemsIntrReqIsReplace_Pre_Enable_NA
} RtemsIntrReqIsReplace_Pre_Enable;

typedef enum {
  RtemsIntrReqIsReplace_Post_Result_True,
  RtemsIntrReqIsReplace_Post_Result_False,
  RtemsIntrReqIsReplace_Post_Result_NA
} RtemsIntrReqIsReplace_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Sharing_NA : 1;
  uint8_t Pre_Replace_NA : 1;
  uint8_t Pre_Enable_NA : 1;
  uint8_t Post_Result : 2;
} RtemsIntrReqIsReplace_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/is-replace test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `_options` parameter value.
   */
  rtems_option options;

  /**
   * @brief This member contains the result of RTEMS_INTERRUPT_IS_REPLACE().
   */
  int result;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    RtemsIntrReqIsReplace_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqIsReplace_Context;

static RtemsIntrReqIsReplace_Context RtemsIntrReqIsReplace_Instance;

static const char *const RtemsIntrReqIsReplace_PreDesc_Sharing[] =
  { "Unique", "Shared", "NA" };

static const char *const RtemsIntrReqIsReplace_PreDesc_Replace[] =
  { "Yes", "No", "NA" };

static const char *const RtemsIntrReqIsReplace_PreDesc_Enable[] =
  { "Enable", "NoEnable", "NA" };

static const char *const *const RtemsIntrReqIsReplace_PreDesc[] = {
  RtemsIntrReqIsReplace_PreDesc_Sharing,
  RtemsIntrReqIsReplace_PreDesc_Replace,
  RtemsIntrReqIsReplace_PreDesc_Enable,
  NULL
};

static void RtemsIntrReqIsReplace_Pre_Sharing_Prepare(
  RtemsIntrReqIsReplace_Context    *ctx,
  RtemsIntrReqIsReplace_Pre_Sharing state
)
{
  switch ( state ) {
    case RtemsIntrReqIsReplace_Pre_Sharing_Unique: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_UNIQUE option
       * set.
       */
      ctx->options |= RTEMS_INTERRUPT_UNIQUE;
      break;
    }

    case RtemsIntrReqIsReplace_Pre_Sharing_Shared: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_UNIQUE option
       * cleared.
       */
      ctx->options |= RTEMS_INTERRUPT_SHARED;
      break;
    }

    case RtemsIntrReqIsReplace_Pre_Sharing_NA:
      break;
  }
}

static void RtemsIntrReqIsReplace_Pre_Replace_Prepare(
  RtemsIntrReqIsReplace_Context    *ctx,
  RtemsIntrReqIsReplace_Pre_Replace state
)
{
  switch ( state ) {
    case RtemsIntrReqIsReplace_Pre_Replace_Yes: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_REPLACE option
       * set.
       */
      ctx->options |= RTEMS_INTERRUPT_REPLACE;
      break;
    }

    case RtemsIntrReqIsReplace_Pre_Replace_No: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_REPLACE option
       * cleared.
       */
      /* Nothing to do */
      break;
    }

    case RtemsIntrReqIsReplace_Pre_Replace_NA:
      break;
  }
}

static void RtemsIntrReqIsReplace_Pre_Enable_Prepare(
  RtemsIntrReqIsReplace_Context   *ctx,
  RtemsIntrReqIsReplace_Pre_Enable state
)
{
  switch ( state ) {
    case RtemsIntrReqIsReplace_Pre_Enable_Enable: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_NO_ENABLE
       * option cleared.
       */
      ctx->options |= RTEMS_INTERRUPT_ENABLE;
      break;
    }

    case RtemsIntrReqIsReplace_Pre_Enable_NoEnable: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_NO_ENABLE
       * option set.
       */
      ctx->options |= RTEMS_INTERRUPT_NO_ENABLE;
      break;
    }

    case RtemsIntrReqIsReplace_Pre_Enable_NA:
      break;
  }
}

static void RtemsIntrReqIsReplace_Post_Result_Check(
  RtemsIntrReqIsReplace_Context    *ctx,
  RtemsIntrReqIsReplace_Post_Result state
)
{
  switch ( state ) {
    case RtemsIntrReqIsReplace_Post_Result_True: {
      /*
       * The result of RTEMS_INTERRUPT_IS_REPLACE() shall be true.
       */
      T_eq_int( ctx->result, true );
      break;
    }

    case RtemsIntrReqIsReplace_Post_Result_False: {
      /*
       * The result of RTEMS_INTERRUPT_IS_REPLACE() shall be false.
       */
      T_eq_int( ctx->result, false );
      break;
    }

    case RtemsIntrReqIsReplace_Post_Result_NA:
      break;
  }
}

static void RtemsIntrReqIsReplace_Prepare( RtemsIntrReqIsReplace_Context *ctx )
{
  ctx->options = 0;
}

static void RtemsIntrReqIsReplace_Action( RtemsIntrReqIsReplace_Context *ctx )
{
  ctx->result = RTEMS_INTERRUPT_IS_REPLACE( ctx->options );
}

/* clang-format off */

static const RtemsIntrReqIsReplace_Entry
RtemsIntrReqIsReplace_Entries[] = {
  { 0, 0, 0, 0, RtemsIntrReqIsReplace_Post_Result_True },
  { 0, 0, 0, 0, RtemsIntrReqIsReplace_Post_Result_False }
};

static const uint8_t
RtemsIntrReqIsReplace_Map[] = {
  0, 0, 1, 1, 0, 0, 1, 1
};

/* clang-format on */

static size_t RtemsIntrReqIsReplace_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqIsReplace_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsIntrReqIsReplace_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsIntrReqIsReplace_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqIsReplace_Scope,
  .initial_context = &RtemsIntrReqIsReplace_Instance
};

static inline RtemsIntrReqIsReplace_Entry RtemsIntrReqIsReplace_PopEntry(
  RtemsIntrReqIsReplace_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqIsReplace_Entries[ RtemsIntrReqIsReplace_Map[ index ] ];
}

static void RtemsIntrReqIsReplace_TestVariant(
  RtemsIntrReqIsReplace_Context *ctx
)
{
  RtemsIntrReqIsReplace_Pre_Sharing_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqIsReplace_Pre_Replace_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqIsReplace_Pre_Enable_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqIsReplace_Action( ctx );
  RtemsIntrReqIsReplace_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_RtemsIntrReqIsReplace( void )
 */
T_TEST_CASE_FIXTURE( RtemsIntrReqIsReplace, &RtemsIntrReqIsReplace_Fixture )
{
  RtemsIntrReqIsReplace_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsIntrReqIsReplace_Pre_Sharing_Unique;
    ctx->Map.pcs[ 0 ] < RtemsIntrReqIsReplace_Pre_Sharing_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsIntrReqIsReplace_Pre_Replace_Yes;
      ctx->Map.pcs[ 1 ] < RtemsIntrReqIsReplace_Pre_Replace_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsIntrReqIsReplace_Pre_Enable_Enable;
        ctx->Map.pcs[ 2 ] < RtemsIntrReqIsReplace_Pre_Enable_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqIsReplace_PopEntry( ctx );
        RtemsIntrReqIsReplace_Prepare( ctx );
        RtemsIntrReqIsReplace_TestVariant( ctx );
      }
    }
  }
}

/** @} */
