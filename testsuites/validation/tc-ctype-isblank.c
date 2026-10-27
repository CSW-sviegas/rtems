/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsblank
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
#include <stdio.h>

#include <rtems/test.h>

/**
 * @defgroup CCtypeReqIsblank spec:/c/ctype/req/isblank
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * Each state of the character dimension provides a set of parameter values.
 * The action calls the function once for each value of the set. The set of the
 * other values contains each value of the type `unsigned char` which no other
 * state of the dimension holds. The test case is compiled without the builtin
 * functions of the compiler, so a call which suppresses the macro definition
 * reaches the function of the library.
 *
 * @{
 */

typedef enum {
  CCtypeReqIsblank_Pre_Form_Macro,
  CCtypeReqIsblank_Pre_Form_Function,
  CCtypeReqIsblank_Pre_Form_NA
} CCtypeReqIsblank_Pre_Form;

typedef enum {
  CCtypeReqIsblank_Pre_C_Space,
  CCtypeReqIsblank_Pre_C_Tab,
  CCtypeReqIsblank_Pre_C_Other,
  CCtypeReqIsblank_Pre_C_EOF,
  CCtypeReqIsblank_Pre_C_NA
} CCtypeReqIsblank_Pre_C;

typedef enum {
  CCtypeReqIsblank_Post_Result_NonZero,
  CCtypeReqIsblank_Post_Result_Zero,
  CCtypeReqIsblank_Post_Result_NA
} CCtypeReqIsblank_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Form_NA : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsblank_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isblank test case.
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
    CCtypeReqIsblank_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsblank_Context;

static CCtypeReqIsblank_Context CCtypeReqIsblank_Instance;

static const char *const CCtypeReqIsblank_PreDesc_Form[] =
  { "Macro", "Function", "NA" };

static const char *const CCtypeReqIsblank_PreDesc_C[] =
  { "Space", "Tab", "Other", "EOF", "NA" };

static const char *const *const CCtypeReqIsblank_PreDesc[] =
  { CCtypeReqIsblank_PreDesc_Form, CCtypeReqIsblank_PreDesc_C, NULL };

typedef CCtypeReqIsblank_Context Context;

static void Add( Context *ctx, int c )
{
  ctx->values[ ctx->count ] = c;
  ++ctx->count;
}

static bool IsNamed( int c )
{
  return c == ' ' || c == '\t';
}

static void AddOther( Context *ctx )
{
  int c;

  for ( c = 0; c <= UCHAR_MAX; ++c ) {
    if ( !IsNamed( c ) ) {
      Add( ctx, c );
    }
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

static void CCtypeReqIsblank_Pre_Form_Prepare(
  CCtypeReqIsblank_Context *ctx,
  CCtypeReqIsblank_Pre_Form state
)
{
  switch ( state ) {
    case CCtypeReqIsblank_Pre_Form_Macro: {
      /*
       * While the call of isblank() uses the macro definition of ctype.h.
       */
      ctx->macro = true;
      break;
    }

    case CCtypeReqIsblank_Pre_Form_Function: {
      /*
       * While the call of isblank() suppresses the macro definition of
       * ctype.h.
       */
      ctx->macro = false;
      break;
    }

    case CCtypeReqIsblank_Pre_Form_NA:
      break;
  }
}

static void CCtypeReqIsblank_Pre_C_Prepare(
  CCtypeReqIsblank_Context *ctx,
  CCtypeReqIsblank_Pre_C    state
)
{
  switch ( state ) {
    case CCtypeReqIsblank_Pre_C_Space: {
      /*
       * While the value of the `c` parameter is the space character.
       */
      Add( ctx, ' ' );
      break;
    }

    case CCtypeReqIsblank_Pre_C_Tab: {
      /*
       * While the value of the `c` parameter is the horizontal tab character.
       */
      Add( ctx, '\t' );
      break;
    }

    case CCtypeReqIsblank_Pre_C_Other: {
      /*
       * While the value of the `c` parameter is a value of the type `unsigned
       * char`, while the value is neither the space character nor the
       * horizontal tab character.
       */
      AddOther( ctx );
      break;
    }

    case CCtypeReqIsblank_Pre_C_EOF: {
      /*
       * While the value of the `c` parameter is equal to EOF.
       */
      Add( ctx, EOF );
      break;
    }

    case CCtypeReqIsblank_Pre_C_NA:
      break;
  }
}

static void CCtypeReqIsblank_Post_Result_Check(
  CCtypeReqIsblank_Context    *ctx,
  CCtypeReqIsblank_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsblank_Post_Result_NonZero: {
      /*
       * The return value of isblank() shall be non-zero.
       */
      CheckNonZero( ctx );
      break;
    }

    case CCtypeReqIsblank_Post_Result_Zero: {
      /*
       * The return value of isblank() shall be equal to zero.
       */
      CheckZero( ctx );
      break;
    }

    case CCtypeReqIsblank_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsblank_Prepare( CCtypeReqIsblank_Context *ctx )
{
  ctx->count = 0;
}

static void CCtypeReqIsblank_Action( CCtypeReqIsblank_Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->count; ++i ) {
    if ( ctx->macro ) {
      ctx->results[ i ] = isblank( ctx->values[ i ] );
    } else {
      ctx->results[ i ] = (isblank) ( ctx->values[ i ] );
    }
  }
}

/* clang-format off */

static const CCtypeReqIsblank_Entry
CCtypeReqIsblank_Entries[] = {
  { 0, 0, 0, CCtypeReqIsblank_Post_Result_NonZero },
  { 0, 0, 0, CCtypeReqIsblank_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsblank_Map[] = {
  0, 0, 1, 1, 0, 0, 1, 1
};

/* clang-format on */

static size_t CCtypeReqIsblank_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsblank_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsblank_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsblank_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsblank_Scope,
  .initial_context = &CCtypeReqIsblank_Instance
};

static inline CCtypeReqIsblank_Entry CCtypeReqIsblank_PopEntry(
  CCtypeReqIsblank_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsblank_Entries[ CCtypeReqIsblank_Map[ index ] ];
}

static void CCtypeReqIsblank_TestVariant( CCtypeReqIsblank_Context *ctx )
{
  CCtypeReqIsblank_Pre_Form_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsblank_Pre_C_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CCtypeReqIsblank_Action( ctx );
  CCtypeReqIsblank_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsblank( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsblank, &CCtypeReqIsblank_Fixture )
{
  CCtypeReqIsblank_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsblank_Pre_Form_Macro;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsblank_Pre_Form_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CCtypeReqIsblank_Pre_C_Space;
      ctx->Map.pcs[ 1 ] < CCtypeReqIsblank_Pre_C_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CCtypeReqIsblank_PopEntry( ctx );
      CCtypeReqIsblank_Prepare( ctx );
      CCtypeReqIsblank_TestVariant( ctx );
    }
  }
}

/** @} */
