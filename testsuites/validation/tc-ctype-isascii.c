/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsascii
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

#include <ctype.h>
#include <limits.h>

#include <rtems/test.h>

/**
 * @defgroup CCtypeReqIsascii spec:/c/ctype/req/isascii
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * Each state of the value dimension provides a set of parameter values. The
 * action calls the function once for each value of the set. The sets hold the
 * bounds of the states and the limits of the type `int`. The test case is
 * compiled without the builtin functions of the compiler, so a call which
 * suppresses the macro definition reaches the function of the library.
 *
 * @{
 */

typedef enum {
  CCtypeReqIsascii_Pre_Form_Macro,
  CCtypeReqIsascii_Pre_Form_Function,
  CCtypeReqIsascii_Pre_Form_NA
} CCtypeReqIsascii_Pre_Form;

typedef enum {
  CCtypeReqIsascii_Pre_C_Negative,
  CCtypeReqIsascii_Pre_C_Ascii,
  CCtypeReqIsascii_Pre_C_Above,
  CCtypeReqIsascii_Pre_C_NA
} CCtypeReqIsascii_Pre_C;

typedef enum {
  CCtypeReqIsascii_Post_Result_NonZero,
  CCtypeReqIsascii_Post_Result_Zero,
  CCtypeReqIsascii_Post_Result_NA
} CCtypeReqIsascii_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Form_NA : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsascii_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isascii test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the call uses the macro definition,
   *   otherwise the call reaches the function.
   */
  bool macro;

  /**
   * @brief This member contains the `c` parameter values.
   */
  int values[ UCHAR_MAX + 2 ];

  /**
   * @brief This member contains the return value of each call.
   */
  int results[ UCHAR_MAX + 2 ];

  /**
   * @brief This member contains the count of parameter values.
   */
  size_t count;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    CCtypeReqIsascii_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsascii_Context;

static CCtypeReqIsascii_Context CCtypeReqIsascii_Instance;

static const char *const CCtypeReqIsascii_PreDesc_Form[] =
  { "Macro", "Function", "NA" };

static const char *const CCtypeReqIsascii_PreDesc_C[] =
  { "Negative", "Ascii", "Above", "NA" };

static const char *const *const CCtypeReqIsascii_PreDesc[] =
  { CCtypeReqIsascii_PreDesc_Form, CCtypeReqIsascii_PreDesc_C, NULL };

typedef CCtypeReqIsascii_Context Context;

static void Add( Context *ctx, int c )
{
  ctx->values[ ctx->count ] = c;
  ++ctx->count;
}

static void AddRange( Context *ctx, int first, int last )
{
  int c;

  for ( c = first; c <= last; ++c ) {
    Add( ctx, c );
  }
}

static void CheckNonZero( const Context *ctx )
{
  size_t i;

  T_gt_sz( ctx->count, 0 );

  for ( i = 0; i < ctx->count; ++i ) {
    T_quiet_ne_int( ctx->results[ i ], 0 );
  }
}

static void CheckZero( const Context *ctx )
{
  size_t i;

  T_gt_sz( ctx->count, 0 );

  for ( i = 0; i < ctx->count; ++i ) {
    T_quiet_eq_int( ctx->results[ i ], 0 );
  }
}

static void CCtypeReqIsascii_Pre_Form_Prepare(
  CCtypeReqIsascii_Context *ctx,
  CCtypeReqIsascii_Pre_Form state
)
{
  switch ( state ) {
    case CCtypeReqIsascii_Pre_Form_Macro: {
      /*
       * While the call of isascii() uses the macro definition of ctype.h.
       */
      ctx->macro = true;
      break;
    }

    case CCtypeReqIsascii_Pre_Form_Function: {
      /*
       * While the call of isascii() suppresses the macro definition of
       * ctype.h.
       */
      ctx->macro = false;
      break;
    }

    case CCtypeReqIsascii_Pre_Form_NA:
      break;
  }
}

static void CCtypeReqIsascii_Pre_C_Prepare(
  CCtypeReqIsascii_Context *ctx,
  CCtypeReqIsascii_Pre_C    state
)
{
  switch ( state ) {
    case CCtypeReqIsascii_Pre_C_Negative: {
      /*
       * While the value of the `c` parameter is less than zero.
       */
      Add( ctx, INT_MIN );
      Add( ctx, -2 );
      Add( ctx, -1 );
      break;
    }

    case CCtypeReqIsascii_Pre_C_Ascii: {
      /*
       * While the value of the `c` parameter is greater than or equal to zero
       * and less than or equal to 127.
       */
      AddRange( ctx, 0, 127 );
      break;
    }

    case CCtypeReqIsascii_Pre_C_Above: {
      /*
       * While the value of the `c` parameter is greater than 127.
       */
      AddRange( ctx, 128, UCHAR_MAX + 1 );
      Add( ctx, INT_MAX );
      break;
    }

    case CCtypeReqIsascii_Pre_C_NA:
      break;
  }
}

static void CCtypeReqIsascii_Post_Result_Check(
  CCtypeReqIsascii_Context    *ctx,
  CCtypeReqIsascii_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsascii_Post_Result_NonZero: {
      /*
       * The return value of isascii() shall be non-zero.
       */
      CheckNonZero( ctx );
      break;
    }

    case CCtypeReqIsascii_Post_Result_Zero: {
      /*
       * The return value of isascii() shall be equal to zero.
       */
      CheckZero( ctx );
      break;
    }

    case CCtypeReqIsascii_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsascii_Prepare( CCtypeReqIsascii_Context *ctx )
{
  ctx->count = 0;
}

static void CCtypeReqIsascii_Action( CCtypeReqIsascii_Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->count; ++i ) {
    if ( ctx->macro ) {
      ctx->results[ i ] = isascii( ctx->values[ i ] );
    } else {
      ctx->results[ i ] = (isascii) ( ctx->values[ i ] );
    }
  }
}

/* clang-format off */

static const CCtypeReqIsascii_Entry
CCtypeReqIsascii_Entries[] = {
  { 0, 0, 0, CCtypeReqIsascii_Post_Result_Zero },
  { 0, 0, 0, CCtypeReqIsascii_Post_Result_NonZero }
};

static const uint8_t
CCtypeReqIsascii_Map[] = {
  0, 1, 0, 0, 1, 0
};

/* clang-format on */

static size_t CCtypeReqIsascii_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsascii_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsascii_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsascii_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsascii_Scope,
  .initial_context = &CCtypeReqIsascii_Instance
};

static inline CCtypeReqIsascii_Entry CCtypeReqIsascii_PopEntry(
  CCtypeReqIsascii_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsascii_Entries[ CCtypeReqIsascii_Map[ index ] ];
}

static void CCtypeReqIsascii_TestVariant( CCtypeReqIsascii_Context *ctx )
{
  CCtypeReqIsascii_Pre_Form_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsascii_Pre_C_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CCtypeReqIsascii_Action( ctx );
  CCtypeReqIsascii_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsascii( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsascii, &CCtypeReqIsascii_Fixture )
{
  CCtypeReqIsascii_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsascii_Pre_Form_Macro;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsascii_Pre_Form_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CCtypeReqIsascii_Pre_C_Negative;
      ctx->Map.pcs[ 1 ] < CCtypeReqIsascii_Pre_C_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CCtypeReqIsascii_PopEntry( ctx );
      CCtypeReqIsascii_Prepare( ctx );
      CCtypeReqIsascii_TestVariant( ctx );
    }
  }
}

/** @} */
