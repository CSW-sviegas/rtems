/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqIsShared
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
 * @defgroup RtemsIntrReqIsShared spec:/rtems/intr/req/is-shared
 *
 * @ingroup TestsuitesValidationIntr
 *
 * The test stores the result in an object of type int. A result other than
 * zero or one fails the comparison with true and false.
 *
 * @{
 */

typedef enum {
  RtemsIntrReqIsShared_Pre_Sharing_Unique,
  RtemsIntrReqIsShared_Pre_Sharing_Shared,
  RtemsIntrReqIsShared_Pre_Sharing_NA
} RtemsIntrReqIsShared_Pre_Sharing;

typedef enum {
  RtemsIntrReqIsShared_Pre_Replace_Yes,
  RtemsIntrReqIsShared_Pre_Replace_No,
  RtemsIntrReqIsShared_Pre_Replace_NA
} RtemsIntrReqIsShared_Pre_Replace;

typedef enum {
  RtemsIntrReqIsShared_Pre_Enable_Enable,
  RtemsIntrReqIsShared_Pre_Enable_NoEnable,
  RtemsIntrReqIsShared_Pre_Enable_NA
} RtemsIntrReqIsShared_Pre_Enable;

typedef enum {
  RtemsIntrReqIsShared_Post_Result_True,
  RtemsIntrReqIsShared_Post_Result_False,
  RtemsIntrReqIsShared_Post_Result_NA
} RtemsIntrReqIsShared_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Sharing_NA : 1;
  uint8_t Pre_Replace_NA : 1;
  uint8_t Pre_Enable_NA : 1;
  uint8_t Post_Result : 2;
} RtemsIntrReqIsShared_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/is-shared test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `_options` parameter value.
   */
  rtems_option options;

  /**
   * @brief This member contains the result of RTEMS_INTERRUPT_IS_SHARED().
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
    RtemsIntrReqIsShared_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqIsShared_Context;

static RtemsIntrReqIsShared_Context RtemsIntrReqIsShared_Instance;

static const char *const RtemsIntrReqIsShared_PreDesc_Sharing[] =
  { "Unique", "Shared", "NA" };

static const char *const RtemsIntrReqIsShared_PreDesc_Replace[] =
  { "Yes", "No", "NA" };

static const char *const RtemsIntrReqIsShared_PreDesc_Enable[] =
  { "Enable", "NoEnable", "NA" };

static const char *const *const RtemsIntrReqIsShared_PreDesc[] = {
  RtemsIntrReqIsShared_PreDesc_Sharing,
  RtemsIntrReqIsShared_PreDesc_Replace,
  RtemsIntrReqIsShared_PreDesc_Enable,
  NULL
};

static void RtemsIntrReqIsShared_Pre_Sharing_Prepare(
  RtemsIntrReqIsShared_Context    *ctx,
  RtemsIntrReqIsShared_Pre_Sharing state
)
{
  switch ( state ) {
    case RtemsIntrReqIsShared_Pre_Sharing_Unique: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_UNIQUE option
       * set.
       */
      ctx->options |= RTEMS_INTERRUPT_UNIQUE;
      break;
    }

    case RtemsIntrReqIsShared_Pre_Sharing_Shared: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_UNIQUE option
       * cleared.
       */
      ctx->options |= RTEMS_INTERRUPT_SHARED;
      break;
    }

    case RtemsIntrReqIsShared_Pre_Sharing_NA:
      break;
  }
}

static void RtemsIntrReqIsShared_Pre_Replace_Prepare(
  RtemsIntrReqIsShared_Context    *ctx,
  RtemsIntrReqIsShared_Pre_Replace state
)
{
  switch ( state ) {
    case RtemsIntrReqIsShared_Pre_Replace_Yes: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_REPLACE option
       * set.
       */
      ctx->options |= RTEMS_INTERRUPT_REPLACE;
      break;
    }

    case RtemsIntrReqIsShared_Pre_Replace_No: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_REPLACE option
       * cleared.
       */
      /* Nothing to do */
      break;
    }

    case RtemsIntrReqIsShared_Pre_Replace_NA:
      break;
  }
}

static void RtemsIntrReqIsShared_Pre_Enable_Prepare(
  RtemsIntrReqIsShared_Context   *ctx,
  RtemsIntrReqIsShared_Pre_Enable state
)
{
  switch ( state ) {
    case RtemsIntrReqIsShared_Pre_Enable_Enable: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_NO_ENABLE
       * option cleared.
       */
      ctx->options |= RTEMS_INTERRUPT_ENABLE;
      break;
    }

    case RtemsIntrReqIsShared_Pre_Enable_NoEnable: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_NO_ENABLE
       * option set.
       */
      ctx->options |= RTEMS_INTERRUPT_NO_ENABLE;
      break;
    }

    case RtemsIntrReqIsShared_Pre_Enable_NA:
      break;
  }
}

static void RtemsIntrReqIsShared_Post_Result_Check(
  RtemsIntrReqIsShared_Context    *ctx,
  RtemsIntrReqIsShared_Post_Result state
)
{
  switch ( state ) {
    case RtemsIntrReqIsShared_Post_Result_True: {
      /*
       * The result of RTEMS_INTERRUPT_IS_SHARED() shall be true.
       */
      T_eq_int( ctx->result, true );
      break;
    }

    case RtemsIntrReqIsShared_Post_Result_False: {
      /*
       * The result of RTEMS_INTERRUPT_IS_SHARED() shall be false.
       */
      T_eq_int( ctx->result, false );
      break;
    }

    case RtemsIntrReqIsShared_Post_Result_NA:
      break;
  }
}

static void RtemsIntrReqIsShared_Prepare( RtemsIntrReqIsShared_Context *ctx )
{
  ctx->options = 0;
}

static void RtemsIntrReqIsShared_Action( RtemsIntrReqIsShared_Context *ctx )
{
  ctx->result = RTEMS_INTERRUPT_IS_SHARED( ctx->options );
}

/* clang-format off */

static const RtemsIntrReqIsShared_Entry
RtemsIntrReqIsShared_Entries[] = {
  { 0, 0, 0, 0, RtemsIntrReqIsShared_Post_Result_False },
  { 0, 0, 0, 0, RtemsIntrReqIsShared_Post_Result_True }
};

static const uint8_t
RtemsIntrReqIsShared_Map[] = {
  0, 0, 0, 0, 1, 1, 1, 1
};

/* clang-format on */

static size_t RtemsIntrReqIsShared_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqIsShared_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsIntrReqIsShared_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsIntrReqIsShared_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqIsShared_Scope,
  .initial_context = &RtemsIntrReqIsShared_Instance
};

static inline RtemsIntrReqIsShared_Entry RtemsIntrReqIsShared_PopEntry(
  RtemsIntrReqIsShared_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqIsShared_Entries[ RtemsIntrReqIsShared_Map[ index ] ];
}

static void RtemsIntrReqIsShared_TestVariant(
  RtemsIntrReqIsShared_Context *ctx
)
{
  RtemsIntrReqIsShared_Pre_Sharing_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqIsShared_Pre_Replace_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqIsShared_Pre_Enable_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqIsShared_Action( ctx );
  RtemsIntrReqIsShared_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_RtemsIntrReqIsShared( void )
 */
T_TEST_CASE_FIXTURE( RtemsIntrReqIsShared, &RtemsIntrReqIsShared_Fixture )
{
  RtemsIntrReqIsShared_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsIntrReqIsShared_Pre_Sharing_Unique;
    ctx->Map.pcs[ 0 ] < RtemsIntrReqIsShared_Pre_Sharing_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsIntrReqIsShared_Pre_Replace_Yes;
      ctx->Map.pcs[ 1 ] < RtemsIntrReqIsShared_Pre_Replace_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsIntrReqIsShared_Pre_Enable_Enable;
        ctx->Map.pcs[ 2 ] < RtemsIntrReqIsShared_Pre_Enable_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqIsShared_PopEntry( ctx );
        RtemsIntrReqIsShared_Prepare( ctx );
        RtemsIntrReqIsShared_TestVariant( ctx );
      }
    }
  }
}

/** @} */
