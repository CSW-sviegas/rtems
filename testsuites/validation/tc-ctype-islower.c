/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIslower
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
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CCtypeReqIslower spec:/c/ctype/req/islower
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
  CCtypeReqIslower_Pre_Form_Macro,
  CCtypeReqIslower_Pre_Form_Function,
  CCtypeReqIslower_Pre_Form_NA
} CCtypeReqIslower_Pre_Form;

typedef enum {
  CCtypeReqIslower_Pre_C_Lower,
  CCtypeReqIslower_Pre_C_Upper,
  CCtypeReqIslower_Pre_C_Other,
  CCtypeReqIslower_Pre_C_EOF,
  CCtypeReqIslower_Pre_C_NA
} CCtypeReqIslower_Pre_C;

typedef enum {
  CCtypeReqIslower_Post_Result_NonZero,
  CCtypeReqIslower_Post_Result_Zero,
  CCtypeReqIslower_Post_Result_NA
} CCtypeReqIslower_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Form_NA : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIslower_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/islower test case.
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
    CCtypeReqIslower_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIslower_Context;

static CCtypeReqIslower_Context CCtypeReqIslower_Instance;

static const char *const CCtypeReqIslower_PreDesc_Form[] =
  { "Macro", "Function", "NA" };

static const char *const CCtypeReqIslower_PreDesc_C[] =
  { "Lower", "Upper", "Other", "EOF", "NA" };

static const char *const *const CCtypeReqIslower_PreDesc[] =
  { CCtypeReqIslower_PreDesc_Form, CCtypeReqIslower_PreDesc_C, NULL };

typedef CCtypeReqIslower_Context Context;

static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static const char lower[] = "abcdefghijklmnopqrstuvwxyz";

static void Add( Context *ctx, int c )
{
  ctx->values[ ctx->count ] = c;
  ++ctx->count;
}

static void AddSet( Context *ctx, const char *set )
{
  while ( *set != '\0' ) {
    Add( ctx, (unsigned char) *set );
    ++set;
  }
}

static bool InSet( const char *set, int c )
{
  return c != '\0' && strchr( set, c ) != NULL;
}

static bool IsNamed( int c )
{
  return InSet( lower, c ) || InSet( upper, c );
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

static void CCtypeReqIslower_Pre_Form_Prepare(
  CCtypeReqIslower_Context *ctx,
  CCtypeReqIslower_Pre_Form state
)
{
  switch ( state ) {
    case CCtypeReqIslower_Pre_Form_Macro: {
      /*
       * While the call of islower() uses the macro definition of ctype.h.
       */
      ctx->macro = true;
      break;
    }

    case CCtypeReqIslower_Pre_Form_Function: {
      /*
       * While the call of islower() suppresses the macro definition of
       * ctype.h.
       */
      ctx->macro = false;
      break;
    }

    case CCtypeReqIslower_Pre_Form_NA:
      break;
  }
}

static void CCtypeReqIslower_Pre_C_Prepare(
  CCtypeReqIslower_Context *ctx,
  CCtypeReqIslower_Pre_C    state
)
{
  switch ( state ) {
    case CCtypeReqIslower_Pre_C_Lower: {
      /*
       * While the value of the `c` parameter is a lowercase letter.
       */
      AddSet( ctx, lower );
      break;
    }

    case CCtypeReqIslower_Pre_C_Upper: {
      /*
       * While the value of the `c` parameter is an uppercase letter.
       */
      AddSet( ctx, upper );
      break;
    }

    case CCtypeReqIslower_Pre_C_Other: {
      /*
       * While the value of the `c` parameter is a value of the type `unsigned
       * char`, while the value is neither a lowercase letter nor an uppercase
       * letter.
       */
      AddOther( ctx );
      break;
    }

    case CCtypeReqIslower_Pre_C_EOF: {
      /*
       * While the value of the `c` parameter is equal to EOF.
       */
      Add( ctx, EOF );
      break;
    }

    case CCtypeReqIslower_Pre_C_NA:
      break;
  }
}

static void CCtypeReqIslower_Post_Result_Check(
  CCtypeReqIslower_Context    *ctx,
  CCtypeReqIslower_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIslower_Post_Result_NonZero: {
      /*
       * The return value of islower() shall be non-zero.
       */
      CheckNonZero( ctx );
      break;
    }

    case CCtypeReqIslower_Post_Result_Zero: {
      /*
       * The return value of islower() shall be equal to zero.
       */
      CheckZero( ctx );
      break;
    }

    case CCtypeReqIslower_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIslower_Prepare( CCtypeReqIslower_Context *ctx )
{
  ctx->count = 0;
}

static void CCtypeReqIslower_Action( CCtypeReqIslower_Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->count; ++i ) {
    if ( ctx->macro ) {
      ctx->results[ i ] = islower( ctx->values[ i ] );
    } else {
      ctx->results[ i ] = (islower) ( ctx->values[ i ] );
    }
  }
}

/* clang-format off */

static const CCtypeReqIslower_Entry
CCtypeReqIslower_Entries[] = {
  { 0, 0, 0, CCtypeReqIslower_Post_Result_Zero },
  { 0, 0, 0, CCtypeReqIslower_Post_Result_NonZero }
};

static const uint8_t
CCtypeReqIslower_Map[] = {
  1, 0, 0, 0, 1, 0, 0, 0
};

/* clang-format on */

static size_t CCtypeReqIslower_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIslower_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIslower_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIslower_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIslower_Scope,
  .initial_context = &CCtypeReqIslower_Instance
};

static inline CCtypeReqIslower_Entry CCtypeReqIslower_PopEntry(
  CCtypeReqIslower_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIslower_Entries[ CCtypeReqIslower_Map[ index ] ];
}

static void CCtypeReqIslower_TestVariant( CCtypeReqIslower_Context *ctx )
{
  CCtypeReqIslower_Pre_Form_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIslower_Pre_C_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CCtypeReqIslower_Action( ctx );
  CCtypeReqIslower_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIslower( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIslower, &CCtypeReqIslower_Fixture )
{
  CCtypeReqIslower_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIslower_Pre_Form_Macro;
    ctx->Map.pcs[ 0 ] < CCtypeReqIslower_Pre_Form_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CCtypeReqIslower_Pre_C_Lower;
      ctx->Map.pcs[ 1 ] < CCtypeReqIslower_Pre_C_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CCtypeReqIslower_PopEntry( ctx );
      CCtypeReqIslower_Prepare( ctx );
      CCtypeReqIslower_TestVariant( ctx );
    }
  }
}

/** @} */
