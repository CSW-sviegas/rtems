/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIscntrl
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
 * @defgroup CCtypeReqIscntrl spec:/c/ctype/req/iscntrl
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
  CCtypeReqIscntrl_Pre_Form_Macro,
  CCtypeReqIscntrl_Pre_Form_Function,
  CCtypeReqIscntrl_Pre_Form_NA
} CCtypeReqIscntrl_Pre_Form;

typedef enum {
  CCtypeReqIscntrl_Pre_C_Control,
  CCtypeReqIscntrl_Pre_C_Other,
  CCtypeReqIscntrl_Pre_C_EOF,
  CCtypeReqIscntrl_Pre_C_NA
} CCtypeReqIscntrl_Pre_C;

typedef enum {
  CCtypeReqIscntrl_Post_Result_NonZero,
  CCtypeReqIscntrl_Post_Result_Zero,
  CCtypeReqIscntrl_Post_Result_NA
} CCtypeReqIscntrl_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Form_NA : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIscntrl_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/iscntrl test case.
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
    CCtypeReqIscntrl_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIscntrl_Context;

static CCtypeReqIscntrl_Context CCtypeReqIscntrl_Instance;

static const char *const CCtypeReqIscntrl_PreDesc_Form[] =
  { "Macro", "Function", "NA" };

static const char *const CCtypeReqIscntrl_PreDesc_C[] =
  { "Control", "Other", "EOF", "NA" };

static const char *const *const CCtypeReqIscntrl_PreDesc[] =
  { CCtypeReqIscntrl_PreDesc_Form, CCtypeReqIscntrl_PreDesc_C, NULL };

typedef CCtypeReqIscntrl_Context Context;

static void Add( Context *ctx, int c )
{
  ctx->values[ ctx->count ] = c;
  ++ctx->count;
}

static bool IsControl( int c )
{
  return ( c >= 0 && c <= 0x1f ) || c == 0x7f;
}

static void AddControl( Context *ctx )
{
  int c;

  for ( c = 0; c <= UCHAR_MAX; ++c ) {
    if ( IsControl( c ) ) {
      Add( ctx, c );
    }
  }
}

static bool IsNamed( int c )
{
  return IsControl( c );
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

static void CCtypeReqIscntrl_Pre_Form_Prepare(
  CCtypeReqIscntrl_Context *ctx,
  CCtypeReqIscntrl_Pre_Form state
)
{
  switch ( state ) {
    case CCtypeReqIscntrl_Pre_Form_Macro: {
      /*
       * While the call of iscntrl() uses the macro definition of ctype.h.
       */
      ctx->macro = true;
      break;
    }

    case CCtypeReqIscntrl_Pre_Form_Function: {
      /*
       * While the call of iscntrl() suppresses the macro definition of
       * ctype.h.
       */
      ctx->macro = false;
      break;
    }

    case CCtypeReqIscntrl_Pre_Form_NA:
      break;
  }
}

static void CCtypeReqIscntrl_Pre_C_Prepare(
  CCtypeReqIscntrl_Context *ctx,
  CCtypeReqIscntrl_Pre_C    state
)
{
  switch ( state ) {
    case CCtypeReqIscntrl_Pre_C_Control: {
      /*
       * While the value of the `c` parameter is a control character.
       */
      AddControl( ctx );
      break;
    }

    case CCtypeReqIscntrl_Pre_C_Other: {
      /*
       * While the value of the `c` parameter is a value of the type `unsigned
       * char`, while the value is not a control character.
       */
      AddOther( ctx );
      break;
    }

    case CCtypeReqIscntrl_Pre_C_EOF: {
      /*
       * While the value of the `c` parameter is equal to EOF.
       */
      Add( ctx, EOF );
      break;
    }

    case CCtypeReqIscntrl_Pre_C_NA:
      break;
  }
}

static void CCtypeReqIscntrl_Post_Result_Check(
  CCtypeReqIscntrl_Context    *ctx,
  CCtypeReqIscntrl_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIscntrl_Post_Result_NonZero: {
      /*
       * The return value of iscntrl() shall be non-zero.
       */
      CheckNonZero( ctx );
      break;
    }

    case CCtypeReqIscntrl_Post_Result_Zero: {
      /*
       * The return value of iscntrl() shall be equal to zero.
       */
      CheckZero( ctx );
      break;
    }

    case CCtypeReqIscntrl_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIscntrl_Prepare( CCtypeReqIscntrl_Context *ctx )
{
  ctx->count = 0;
}

static void CCtypeReqIscntrl_Action( CCtypeReqIscntrl_Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->count; ++i ) {
    if ( ctx->macro ) {
      ctx->results[ i ] = iscntrl( ctx->values[ i ] );
    } else {
      ctx->results[ i ] = (iscntrl) ( ctx->values[ i ] );
    }
  }
}

/* clang-format off */

static const CCtypeReqIscntrl_Entry
CCtypeReqIscntrl_Entries[] = {
  { 0, 0, 0, CCtypeReqIscntrl_Post_Result_Zero },
  { 0, 0, 0, CCtypeReqIscntrl_Post_Result_NonZero }
};

static const uint8_t
CCtypeReqIscntrl_Map[] = {
  1, 0, 0, 1, 0, 0
};

/* clang-format on */

static size_t CCtypeReqIscntrl_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIscntrl_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIscntrl_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIscntrl_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIscntrl_Scope,
  .initial_context = &CCtypeReqIscntrl_Instance
};

static inline CCtypeReqIscntrl_Entry CCtypeReqIscntrl_PopEntry(
  CCtypeReqIscntrl_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIscntrl_Entries[ CCtypeReqIscntrl_Map[ index ] ];
}

static void CCtypeReqIscntrl_TestVariant( CCtypeReqIscntrl_Context *ctx )
{
  CCtypeReqIscntrl_Pre_Form_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIscntrl_Pre_C_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CCtypeReqIscntrl_Action( ctx );
  CCtypeReqIscntrl_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIscntrl( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIscntrl, &CCtypeReqIscntrl_Fixture )
{
  CCtypeReqIscntrl_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIscntrl_Pre_Form_Macro;
    ctx->Map.pcs[ 0 ] < CCtypeReqIscntrl_Pre_Form_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CCtypeReqIscntrl_Pre_C_Control;
      ctx->Map.pcs[ 1 ] < CCtypeReqIscntrl_Pre_C_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CCtypeReqIscntrl_PopEntry( ctx );
      CCtypeReqIscntrl_Prepare( ctx );
      CCtypeReqIscntrl_TestVariant( ctx );
    }
  }
}

/** @} */
