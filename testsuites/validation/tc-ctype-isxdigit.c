/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsxdigit
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
 * @defgroup CCtypeReqIsxdigit spec:/c/ctype/req/isxdigit
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
  CCtypeReqIsxdigit_Pre_Form_Macro,
  CCtypeReqIsxdigit_Pre_Form_Function,
  CCtypeReqIsxdigit_Pre_Form_NA
} CCtypeReqIsxdigit_Pre_Form;

typedef enum {
  CCtypeReqIsxdigit_Pre_C_Digit,
  CCtypeReqIsxdigit_Pre_C_HexLetter,
  CCtypeReqIsxdigit_Pre_C_Other,
  CCtypeReqIsxdigit_Pre_C_EOF,
  CCtypeReqIsxdigit_Pre_C_NA
} CCtypeReqIsxdigit_Pre_C;

typedef enum {
  CCtypeReqIsxdigit_Post_Result_NonZero,
  CCtypeReqIsxdigit_Post_Result_Zero,
  CCtypeReqIsxdigit_Post_Result_NA
} CCtypeReqIsxdigit_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Form_NA : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsxdigit_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isxdigit test case.
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
    CCtypeReqIsxdigit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsxdigit_Context;

static CCtypeReqIsxdigit_Context CCtypeReqIsxdigit_Instance;

static const char *const CCtypeReqIsxdigit_PreDesc_Form[] =
  { "Macro", "Function", "NA" };

static const char *const CCtypeReqIsxdigit_PreDesc_C[] =
  { "Digit", "HexLetter", "Other", "EOF", "NA" };

static const char *const *const CCtypeReqIsxdigit_PreDesc[] =
  { CCtypeReqIsxdigit_PreDesc_Form, CCtypeReqIsxdigit_PreDesc_C, NULL };

typedef CCtypeReqIsxdigit_Context Context;

static const char digit[] = "0123456789";

static const char hexletter[] = "ABCDEFabcdef";

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
  return InSet( digit, c ) || InSet( hexletter, c );
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

static void CCtypeReqIsxdigit_Pre_Form_Prepare(
  CCtypeReqIsxdigit_Context *ctx,
  CCtypeReqIsxdigit_Pre_Form state
)
{
  switch ( state ) {
    case CCtypeReqIsxdigit_Pre_Form_Macro: {
      /*
       * While the call of isxdigit() uses the macro definition of ctype.h.
       */
      ctx->macro = true;
      break;
    }

    case CCtypeReqIsxdigit_Pre_Form_Function: {
      /*
       * While the call of isxdigit() suppresses the macro definition of
       * ctype.h.
       */
      ctx->macro = false;
      break;
    }

    case CCtypeReqIsxdigit_Pre_Form_NA:
      break;
  }
}

static void CCtypeReqIsxdigit_Pre_C_Prepare(
  CCtypeReqIsxdigit_Context *ctx,
  CCtypeReqIsxdigit_Pre_C    state
)
{
  switch ( state ) {
    case CCtypeReqIsxdigit_Pre_C_Digit: {
      /*
       * While the value of the `c` parameter is a digit.
       */
      AddSet( ctx, digit );
      break;
    }

    case CCtypeReqIsxdigit_Pre_C_HexLetter: {
      /*
       * While the value of the `c` parameter is a hexadecimal letter.
       */
      AddSet( ctx, hexletter );
      break;
    }

    case CCtypeReqIsxdigit_Pre_C_Other: {
      /*
       * While the value of the `c` parameter is a value of the type `unsigned
       * char`, while the value is neither a digit nor a hexadecimal letter.
       */
      AddOther( ctx );
      break;
    }

    case CCtypeReqIsxdigit_Pre_C_EOF: {
      /*
       * While the value of the `c` parameter is equal to EOF.
       */
      Add( ctx, EOF );
      break;
    }

    case CCtypeReqIsxdigit_Pre_C_NA:
      break;
  }
}

static void CCtypeReqIsxdigit_Post_Result_Check(
  CCtypeReqIsxdigit_Context    *ctx,
  CCtypeReqIsxdigit_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsxdigit_Post_Result_NonZero: {
      /*
       * The return value of isxdigit() shall be non-zero.
       */
      CheckNonZero( ctx );
      break;
    }

    case CCtypeReqIsxdigit_Post_Result_Zero: {
      /*
       * The return value of isxdigit() shall be equal to zero.
       */
      CheckZero( ctx );
      break;
    }

    case CCtypeReqIsxdigit_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsxdigit_Prepare( CCtypeReqIsxdigit_Context *ctx )
{
  ctx->count = 0;
}

static void CCtypeReqIsxdigit_Action( CCtypeReqIsxdigit_Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->count; ++i ) {
    if ( ctx->macro ) {
      ctx->results[ i ] = isxdigit( ctx->values[ i ] );
    } else {
      ctx->results[ i ] = (isxdigit) ( ctx->values[ i ] );
    }
  }
}

/* clang-format off */

static const CCtypeReqIsxdigit_Entry
CCtypeReqIsxdigit_Entries[] = {
  { 0, 0, 0, CCtypeReqIsxdigit_Post_Result_NonZero },
  { 0, 0, 0, CCtypeReqIsxdigit_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsxdigit_Map[] = {
  0, 0, 1, 1, 0, 0, 1, 1
};

/* clang-format on */

static size_t CCtypeReqIsxdigit_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsxdigit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsxdigit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsxdigit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsxdigit_Scope,
  .initial_context = &CCtypeReqIsxdigit_Instance
};

static inline CCtypeReqIsxdigit_Entry CCtypeReqIsxdigit_PopEntry(
  CCtypeReqIsxdigit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsxdigit_Entries[ CCtypeReqIsxdigit_Map[ index ] ];
}

static void CCtypeReqIsxdigit_TestVariant( CCtypeReqIsxdigit_Context *ctx )
{
  CCtypeReqIsxdigit_Pre_Form_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsxdigit_Pre_C_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CCtypeReqIsxdigit_Action( ctx );
  CCtypeReqIsxdigit_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsxdigit( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsxdigit, &CCtypeReqIsxdigit_Fixture )
{
  CCtypeReqIsxdigit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsxdigit_Pre_Form_Macro;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsxdigit_Pre_Form_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CCtypeReqIsxdigit_Pre_C_Digit;
      ctx->Map.pcs[ 1 ] < CCtypeReqIsxdigit_Pre_C_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CCtypeReqIsxdigit_PopEntry( ctx );
      CCtypeReqIsxdigit_Prepare( ctx );
      CCtypeReqIsxdigit_TestVariant( ctx );
    }
  }
}

/** @} */
