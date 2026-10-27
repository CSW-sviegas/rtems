/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsgraph
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
 * @defgroup CCtypeReqIsgraph spec:/c/ctype/req/isgraph
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
  CCtypeReqIsgraph_Pre_Form_Macro,
  CCtypeReqIsgraph_Pre_Form_Function,
  CCtypeReqIsgraph_Pre_Form_NA
} CCtypeReqIsgraph_Pre_Form;

typedef enum {
  CCtypeReqIsgraph_Pre_C_Upper,
  CCtypeReqIsgraph_Pre_C_Lower,
  CCtypeReqIsgraph_Pre_C_Digit,
  CCtypeReqIsgraph_Pre_C_Punct,
  CCtypeReqIsgraph_Pre_C_Space,
  CCtypeReqIsgraph_Pre_C_Other,
  CCtypeReqIsgraph_Pre_C_EOF,
  CCtypeReqIsgraph_Pre_C_NA
} CCtypeReqIsgraph_Pre_C;

typedef enum {
  CCtypeReqIsgraph_Post_Result_NonZero,
  CCtypeReqIsgraph_Post_Result_Zero,
  CCtypeReqIsgraph_Post_Result_NA
} CCtypeReqIsgraph_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Form_NA : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsgraph_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isgraph test case.
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
    CCtypeReqIsgraph_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsgraph_Context;

static CCtypeReqIsgraph_Context CCtypeReqIsgraph_Instance;

static const char *const CCtypeReqIsgraph_PreDesc_Form[] =
  { "Macro", "Function", "NA" };

static const char *const CCtypeReqIsgraph_PreDesc_C[] =
  { "Upper", "Lower", "Digit", "Punct", "Space", "Other", "EOF", "NA" };

static const char *const *const CCtypeReqIsgraph_PreDesc[] =
  { CCtypeReqIsgraph_PreDesc_Form, CCtypeReqIsgraph_PreDesc_C, NULL };

typedef CCtypeReqIsgraph_Context Context;

static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static const char lower[] = "abcdefghijklmnopqrstuvwxyz";

static const char digit[] = "0123456789";

static const char punct[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

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
  return InSet( upper, c ) || InSet( lower, c ) || InSet( digit, c ) ||
         InSet( punct, c ) || c == ' ';
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

static void CCtypeReqIsgraph_Pre_Form_Prepare(
  CCtypeReqIsgraph_Context *ctx,
  CCtypeReqIsgraph_Pre_Form state
)
{
  switch ( state ) {
    case CCtypeReqIsgraph_Pre_Form_Macro: {
      /*
       * While the call of isgraph() uses the macro definition of ctype.h.
       */
      ctx->macro = true;
      break;
    }

    case CCtypeReqIsgraph_Pre_Form_Function: {
      /*
       * While the call of isgraph() suppresses the macro definition of
       * ctype.h.
       */
      ctx->macro = false;
      break;
    }

    case CCtypeReqIsgraph_Pre_Form_NA:
      break;
  }
}

static void CCtypeReqIsgraph_Pre_C_Prepare(
  CCtypeReqIsgraph_Context *ctx,
  CCtypeReqIsgraph_Pre_C    state
)
{
  switch ( state ) {
    case CCtypeReqIsgraph_Pre_C_Upper: {
      /*
       * While the value of the `c` parameter is an uppercase letter.
       */
      AddSet( ctx, upper );
      break;
    }

    case CCtypeReqIsgraph_Pre_C_Lower: {
      /*
       * While the value of the `c` parameter is a lowercase letter.
       */
      AddSet( ctx, lower );
      break;
    }

    case CCtypeReqIsgraph_Pre_C_Digit: {
      /*
       * While the value of the `c` parameter is a digit.
       */
      AddSet( ctx, digit );
      break;
    }

    case CCtypeReqIsgraph_Pre_C_Punct: {
      /*
       * While the value of the `c` parameter is a punctuation character.
       */
      AddSet( ctx, punct );
      break;
    }

    case CCtypeReqIsgraph_Pre_C_Space: {
      /*
       * While the value of the `c` parameter is the space character.
       */
      Add( ctx, ' ' );
      break;
    }

    case CCtypeReqIsgraph_Pre_C_Other: {
      /*
       * While the value of the `c` parameter is a value of the type `unsigned
       * char`, while the value is neither an uppercase letter, a lowercase
       * letter, a digit, a punctuation character nor the space character.
       */
      AddOther( ctx );
      break;
    }

    case CCtypeReqIsgraph_Pre_C_EOF: {
      /*
       * While the value of the `c` parameter is equal to EOF.
       */
      Add( ctx, EOF );
      break;
    }

    case CCtypeReqIsgraph_Pre_C_NA:
      break;
  }
}

static void CCtypeReqIsgraph_Post_Result_Check(
  CCtypeReqIsgraph_Context    *ctx,
  CCtypeReqIsgraph_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsgraph_Post_Result_NonZero: {
      /*
       * The return value of isgraph() shall be non-zero.
       */
      CheckNonZero( ctx );
      break;
    }

    case CCtypeReqIsgraph_Post_Result_Zero: {
      /*
       * The return value of isgraph() shall be equal to zero.
       */
      CheckZero( ctx );
      break;
    }

    case CCtypeReqIsgraph_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsgraph_Prepare( CCtypeReqIsgraph_Context *ctx )
{
  ctx->count = 0;
}

static void CCtypeReqIsgraph_Action( CCtypeReqIsgraph_Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->count; ++i ) {
    if ( ctx->macro ) {
      ctx->results[ i ] = isgraph( ctx->values[ i ] );
    } else {
      ctx->results[ i ] = (isgraph) ( ctx->values[ i ] );
    }
  }
}

/* clang-format off */

static const CCtypeReqIsgraph_Entry
CCtypeReqIsgraph_Entries[] = {
  { 0, 0, 0, CCtypeReqIsgraph_Post_Result_NonZero },
  { 0, 0, 0, CCtypeReqIsgraph_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsgraph_Map[] = {
  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1
};

/* clang-format on */

static size_t CCtypeReqIsgraph_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsgraph_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsgraph_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsgraph_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsgraph_Scope,
  .initial_context = &CCtypeReqIsgraph_Instance
};

static inline CCtypeReqIsgraph_Entry CCtypeReqIsgraph_PopEntry(
  CCtypeReqIsgraph_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsgraph_Entries[ CCtypeReqIsgraph_Map[ index ] ];
}

static void CCtypeReqIsgraph_TestVariant( CCtypeReqIsgraph_Context *ctx )
{
  CCtypeReqIsgraph_Pre_Form_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsgraph_Pre_C_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CCtypeReqIsgraph_Action( ctx );
  CCtypeReqIsgraph_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsgraph( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsgraph, &CCtypeReqIsgraph_Fixture )
{
  CCtypeReqIsgraph_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsgraph_Pre_Form_Macro;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsgraph_Pre_Form_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CCtypeReqIsgraph_Pre_C_Upper;
      ctx->Map.pcs[ 1 ] < CCtypeReqIsgraph_Pre_C_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CCtypeReqIsgraph_PopEntry( ctx );
      CCtypeReqIsgraph_Prepare( ctx );
      CCtypeReqIsgraph_TestVariant( ctx );
    }
  }
}

/** @} */
