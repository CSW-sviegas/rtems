/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqIsUnique
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
 * @defgroup RtemsIntrReqIsUnique spec:/rtems/intr/req/is-unique
 *
 * @ingroup TestsuitesValidationIntr
 *
 * The test stores the result in an object of type int. A result other than
 * zero or one fails the comparison with true and false.
 *
 * @{
 */

typedef enum {
  RtemsIntrReqIsUnique_Pre_Sharing_Unique,
  RtemsIntrReqIsUnique_Pre_Sharing_Shared,
  RtemsIntrReqIsUnique_Pre_Sharing_NA
} RtemsIntrReqIsUnique_Pre_Sharing;

typedef enum {
  RtemsIntrReqIsUnique_Pre_Replace_Yes,
  RtemsIntrReqIsUnique_Pre_Replace_No,
  RtemsIntrReqIsUnique_Pre_Replace_NA
} RtemsIntrReqIsUnique_Pre_Replace;

typedef enum {
  RtemsIntrReqIsUnique_Pre_Enable_Enable,
  RtemsIntrReqIsUnique_Pre_Enable_NoEnable,
  RtemsIntrReqIsUnique_Pre_Enable_NA
} RtemsIntrReqIsUnique_Pre_Enable;

typedef enum {
  RtemsIntrReqIsUnique_Post_Result_True,
  RtemsIntrReqIsUnique_Post_Result_False,
  RtemsIntrReqIsUnique_Post_Result_NA
} RtemsIntrReqIsUnique_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Sharing_NA : 1;
  uint8_t Pre_Replace_NA : 1;
  uint8_t Pre_Enable_NA : 1;
  uint8_t Post_Result : 2;
} RtemsIntrReqIsUnique_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/is-unique test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `_options` parameter value.
   */
  rtems_option options;

  /**
   * @brief This member contains the result of RTEMS_INTERRUPT_IS_UNIQUE().
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
    RtemsIntrReqIsUnique_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqIsUnique_Context;

static RtemsIntrReqIsUnique_Context RtemsIntrReqIsUnique_Instance;

static const char *const RtemsIntrReqIsUnique_PreDesc_Sharing[] =
  { "Unique", "Shared", "NA" };

static const char *const RtemsIntrReqIsUnique_PreDesc_Replace[] =
  { "Yes", "No", "NA" };

static const char *const RtemsIntrReqIsUnique_PreDesc_Enable[] =
  { "Enable", "NoEnable", "NA" };

static const char *const *const RtemsIntrReqIsUnique_PreDesc[] = {
  RtemsIntrReqIsUnique_PreDesc_Sharing,
  RtemsIntrReqIsUnique_PreDesc_Replace,
  RtemsIntrReqIsUnique_PreDesc_Enable,
  NULL
};

static void RtemsIntrReqIsUnique_Pre_Sharing_Prepare(
  RtemsIntrReqIsUnique_Context    *ctx,
  RtemsIntrReqIsUnique_Pre_Sharing state
)
{
  switch ( state ) {
    case RtemsIntrReqIsUnique_Pre_Sharing_Unique: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_UNIQUE option
       * set.
       */
      ctx->options |= RTEMS_INTERRUPT_UNIQUE;
      break;
    }

    case RtemsIntrReqIsUnique_Pre_Sharing_Shared: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_UNIQUE option
       * cleared.
       */
      ctx->options |= RTEMS_INTERRUPT_SHARED;
      break;
    }

    case RtemsIntrReqIsUnique_Pre_Sharing_NA:
      break;
  }
}

static void RtemsIntrReqIsUnique_Pre_Replace_Prepare(
  RtemsIntrReqIsUnique_Context    *ctx,
  RtemsIntrReqIsUnique_Pre_Replace state
)
{
  switch ( state ) {
    case RtemsIntrReqIsUnique_Pre_Replace_Yes: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_REPLACE option
       * set.
       */
      ctx->options |= RTEMS_INTERRUPT_REPLACE;
      break;
    }

    case RtemsIntrReqIsUnique_Pre_Replace_No: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_REPLACE option
       * cleared.
       */
      /* Nothing to do */
      break;
    }

    case RtemsIntrReqIsUnique_Pre_Replace_NA:
      break;
  }
}

static void RtemsIntrReqIsUnique_Pre_Enable_Prepare(
  RtemsIntrReqIsUnique_Context   *ctx,
  RtemsIntrReqIsUnique_Pre_Enable state
)
{
  switch ( state ) {
    case RtemsIntrReqIsUnique_Pre_Enable_Enable: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_NO_ENABLE
       * option cleared.
       */
      ctx->options |= RTEMS_INTERRUPT_ENABLE;
      break;
    }

    case RtemsIntrReqIsUnique_Pre_Enable_NoEnable: {
      /*
       * While the `_options` parameter has the RTEMS_INTERRUPT_NO_ENABLE
       * option set.
       */
      ctx->options |= RTEMS_INTERRUPT_NO_ENABLE;
      break;
    }

    case RtemsIntrReqIsUnique_Pre_Enable_NA:
      break;
  }
}

static void RtemsIntrReqIsUnique_Post_Result_Check(
  RtemsIntrReqIsUnique_Context    *ctx,
  RtemsIntrReqIsUnique_Post_Result state
)
{
  switch ( state ) {
    case RtemsIntrReqIsUnique_Post_Result_True: {
      /*
       * The result of RTEMS_INTERRUPT_IS_UNIQUE() shall be true.
       */
      T_eq_int( ctx->result, true );
      break;
    }

    case RtemsIntrReqIsUnique_Post_Result_False: {
      /*
       * The result of RTEMS_INTERRUPT_IS_UNIQUE() shall be false.
       */
      T_eq_int( ctx->result, false );
      break;
    }

    case RtemsIntrReqIsUnique_Post_Result_NA:
      break;
  }
}

static void RtemsIntrReqIsUnique_Prepare( RtemsIntrReqIsUnique_Context *ctx )
{
  ctx->options = 0;
}

static void RtemsIntrReqIsUnique_Action( RtemsIntrReqIsUnique_Context *ctx )
{
  ctx->result = RTEMS_INTERRUPT_IS_UNIQUE( ctx->options );
}

/* clang-format off */

static const RtemsIntrReqIsUnique_Entry
RtemsIntrReqIsUnique_Entries[] = {
  { 0, 0, 0, 0, RtemsIntrReqIsUnique_Post_Result_True },
  { 0, 0, 0, 0, RtemsIntrReqIsUnique_Post_Result_False }
};

static const uint8_t
RtemsIntrReqIsUnique_Map[] = {
  0, 0, 0, 0, 1, 1, 1, 1
};

/* clang-format on */

static size_t RtemsIntrReqIsUnique_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqIsUnique_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsIntrReqIsUnique_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsIntrReqIsUnique_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqIsUnique_Scope,
  .initial_context = &RtemsIntrReqIsUnique_Instance
};

static inline RtemsIntrReqIsUnique_Entry RtemsIntrReqIsUnique_PopEntry(
  RtemsIntrReqIsUnique_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqIsUnique_Entries[ RtemsIntrReqIsUnique_Map[ index ] ];
}

static void RtemsIntrReqIsUnique_TestVariant(
  RtemsIntrReqIsUnique_Context *ctx
)
{
  RtemsIntrReqIsUnique_Pre_Sharing_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqIsUnique_Pre_Replace_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqIsUnique_Pre_Enable_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqIsUnique_Action( ctx );
  RtemsIntrReqIsUnique_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_RtemsIntrReqIsUnique( void )
 */
T_TEST_CASE_FIXTURE( RtemsIntrReqIsUnique, &RtemsIntrReqIsUnique_Fixture )
{
  RtemsIntrReqIsUnique_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsIntrReqIsUnique_Pre_Sharing_Unique;
    ctx->Map.pcs[ 0 ] < RtemsIntrReqIsUnique_Pre_Sharing_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsIntrReqIsUnique_Pre_Replace_Yes;
      ctx->Map.pcs[ 1 ] < RtemsIntrReqIsUnique_Pre_Replace_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsIntrReqIsUnique_Pre_Enable_Enable;
        ctx->Map.pcs[ 2 ] < RtemsIntrReqIsUnique_Pre_Enable_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqIsUnique_PopEntry( ctx );
        RtemsIntrReqIsUnique_Prepare( ctx );
        RtemsIntrReqIsUnique_TestVariant( ctx );
      }
    }
  }
}

/** @} */
