/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqFfsll
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

#include <limits.h>
#include <strings.h>

#include <rtems/test.h>

/**
 * @defgroup CStringsReqFfsll spec:/c/strings/req/ffsll
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStringsReqFfsll_Pre_Value_MinusOne,
  CStringsReqFfsll_Pre_Value_Zero,
  CStringsReqFfsll_Pre_Value_One,
  CStringsReqFfsll_Pre_Value_Least,
  CStringsReqFfsll_Pre_Value_Middle,
  CStringsReqFfsll_Pre_Value_Most,
  CStringsReqFfsll_Pre_Value_NA
} CStringsReqFfsll_Pre_Value;

typedef enum {
  CStringsReqFfsll_Post_Result_Zero,
  CStringsReqFfsll_Post_Result_Index,
  CStringsReqFfsll_Post_Result_NA
} CStringsReqFfsll_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Value_NA : 1;
  uint8_t Post_Result : 2;
} CStringsReqFfsll_Entry;

/**
 * @brief Test context for spec:/c/strings/req/ffsll test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `i` parameter value.
   */
  long long i;

  /**
   * @brief This member contains the return value of the ffsll() call.
   */
  int result;

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
    CStringsReqFfsll_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqFfsll_Context;

static CStringsReqFfsll_Context CStringsReqFfsll_Instance;

static const char *const CStringsReqFfsll_PreDesc_Value[] =
  { "MinusOne", "Zero", "One", "Least", "Middle", "Most", "NA" };

static const char *const *const CStringsReqFfsll_PreDesc[] = {
  CStringsReqFfsll_PreDesc_Value,
  NULL
};

static void CStringsReqFfsll_Pre_Value_Prepare(
  CStringsReqFfsll_Context  *ctx,
  CStringsReqFfsll_Pre_Value state
)
{
  switch ( state ) {
    case CStringsReqFfsll_Pre_Value_MinusOne: {
      /*
       * While the value of the `i` parameter is equal to minus one.
       */
      ctx->i = -1LL;
      break;
    }

    case CStringsReqFfsll_Pre_Value_Zero: {
      /*
       * While the value of the `i` parameter is equal to zero.
       */
      ctx->i = 0;
      break;
    }

    case CStringsReqFfsll_Pre_Value_One: {
      /*
       * While the value of the `i` parameter is equal to one.
       */
      ctx->i = 1LL;
      break;
    }

    case CStringsReqFfsll_Pre_Value_Least: {
      /*
       * While the least significant bit of the value of the `i` parameter is
       * set, while the value of the `i` parameter is neither one nor minus
       * one.
       */
      ctx->i = 0x0008010000000001LL;
      break;
    }

    case CStringsReqFfsll_Pre_Value_Middle: {
      /*
       * While the least significant bit set in the value of the `i` parameter
       * is neither the least significant bit nor the most significant bit of
       * the value.
       */
      ctx->i = 0x0008010000000000LL;
      break;
    }

    case CStringsReqFfsll_Pre_Value_Most: {
      /*
       * While the most significant bit is the only bit set in the value of the
       * `i` parameter.
       */
      ctx->i = LLONG_MIN;
      break;
    }

    case CStringsReqFfsll_Pre_Value_NA:
      break;
  }
}

static void CStringsReqFfsll_Post_Result_Check(
  CStringsReqFfsll_Context    *ctx,
  CStringsReqFfsll_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqFfsll_Post_Result_Zero: {
      /*
       * The return value of ffsll() shall be equal to zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case CStringsReqFfsll_Post_Result_Index: {
      /*
       * The return value of ffsll() shall be the number of the least
       * significant bit set in the value of the `i` parameter, with the bits
       * of the value numbered from one at the least significant bit.
       */
      unsigned long long value;
      int                expected;

      value = (unsigned long long) ctx->i;
      expected = 1;

      while ( ( value & 1U ) == 0 ) {
        value >>= 1;
        ++expected;
      }

      T_eq_int( ctx->result, expected );
      break;
    }

    case CStringsReqFfsll_Post_Result_NA:
      break;
  }
}

static void CStringsReqFfsll_Action( CStringsReqFfsll_Context *ctx )
{
  ctx->result = ffsll( ctx->i );
}

/* clang-format off */

static const CStringsReqFfsll_Entry
CStringsReqFfsll_Entries[] = {
  { 0, 0, CStringsReqFfsll_Post_Result_Index },
  { 0, 0, CStringsReqFfsll_Post_Result_Zero }
};

static const uint8_t
CStringsReqFfsll_Map[] = {
  0, 1, 0, 0, 0, 0
};

/* clang-format on */

static size_t CStringsReqFfsll_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqFfsll_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringsReqFfsll_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringsReqFfsll_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqFfsll_Scope,
  .initial_context = &CStringsReqFfsll_Instance
};

static inline CStringsReqFfsll_Entry CStringsReqFfsll_PopEntry(
  CStringsReqFfsll_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqFfsll_Entries[ CStringsReqFfsll_Map[ index ] ];
}

static void CStringsReqFfsll_TestVariant( CStringsReqFfsll_Context *ctx )
{
  CStringsReqFfsll_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqFfsll_Action( ctx );
  CStringsReqFfsll_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringsReqFfsll( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqFfsll, &CStringsReqFfsll_Fixture )
{
  CStringsReqFfsll_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqFfsll_Pre_Value_MinusOne;
    ctx->Map.pcs[ 0 ] < CStringsReqFfsll_Pre_Value_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStringsReqFfsll_PopEntry( ctx );
    CStringsReqFfsll_TestVariant( ctx );
  }
}

/** @} */
