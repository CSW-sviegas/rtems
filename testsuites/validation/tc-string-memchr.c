/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqMemchr
 */

/*
 * Copyright (C) 2025, 2026 embedded brains GmbH & Co. KG
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

#include <errno.h>
#include <limits.h>
#include <rtems.h>
#include <stdint.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqMemchr spec:/c/string/req/memchr
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * The memory area starts inside a buffer filled with a byte which differs from
 * the byte to locate. Where the memory area holds more than two bytes, the
 * second byte holds a second match. Otherwise the first byte holds it. The
 * second byte lies in the first block of an aligned memory area and starts the
 * first block of a memory area which starts one byte below an aligned address.
 * It lies before the first block of the other memory areas. A memory area
 * without a match has the byte to locate immediately before and after it. A
 * size beyond the object places the matches as the long size does.
 *
 * @{
 */

typedef enum {
  CStringReqMemchr_Pre_N_Zero,
  CStringReqMemchr_Pre_N_One,
  CStringReqMemchr_Pre_N_Short,
  CStringReqMemchr_Pre_N_BelowWord,
  CStringReqMemchr_Pre_N_Word,
  CStringReqMemchr_Pre_N_AboveWord,
  CStringReqMemchr_Pre_N_Long,
  CStringReqMemchr_Pre_N_NA
} CStringReqMemchr_Pre_N;

typedef enum {
  CStringReqMemchr_Pre_Bound_Inside,
  CStringReqMemchr_Pre_Bound_Beyond,
  CStringReqMemchr_Pre_Bound_NA
} CStringReqMemchr_Pre_Bound;

typedef enum {
  CStringReqMemchr_Pre_Alignment_Aligned,
  CStringReqMemchr_Pre_Alignment_AboveAligned,
  CStringReqMemchr_Pre_Alignment_Between,
  CStringReqMemchr_Pre_Alignment_BelowAligned,
  CStringReqMemchr_Pre_Alignment_NA
} CStringReqMemchr_Pre_Alignment;

typedef enum {
  CStringReqMemchr_Pre_C_InRange,
  CStringReqMemchr_Pre_C_Negative,
  CStringReqMemchr_Pre_C_Greater,
  CStringReqMemchr_Pre_C_NA
} CStringReqMemchr_Pre_C;

typedef enum {
  CStringReqMemchr_Pre_Match_None,
  CStringReqMemchr_Pre_Match_One,
  CStringReqMemchr_Pre_Match_Several,
  CStringReqMemchr_Pre_Match_NA
} CStringReqMemchr_Pre_Match;

typedef enum {
  CStringReqMemchr_Post_Result_First,
  CStringReqMemchr_Post_Result_Null,
  CStringReqMemchr_Post_Result_NA
} CStringReqMemchr_Post_Result;

typedef enum {
  CStringReqMemchr_Post_Area_Nop,
  CStringReqMemchr_Post_Area_NA
} CStringReqMemchr_Post_Area;

typedef enum {
  CStringReqMemchr_Post_Errno_Nop,
  CStringReqMemchr_Post_Errno_NA
} CStringReqMemchr_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_N_NA : 1;
  uint16_t Pre_Bound_NA : 1;
  uint16_t Pre_Alignment_NA : 1;
  uint16_t Pre_C_NA : 1;
  uint16_t Pre_Match_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_Area : 1;
  uint16_t Post_Errno : 1;
} CStringReqMemchr_Entry;

/**
 * @brief Test context for spec:/c/string/req/memchr test case.
 */
typedef struct {
  /**
   * @brief This member provides the memory which holds the memory area.
   */
  unsigned char buf[ 16 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member contains the content of the memory which holds the
   *   memory area before the memchr() call.
   */
  unsigned char before[ 16 * sizeof( long ) ];

  /**
   * @brief This member specifies the `s` parameter value.
   */
  unsigned char *s;

  /**
   * @brief This member specifies the `c` parameter value.
   */
  int c;

  /**
   * @brief This member specifies the `n` parameter value.
   */
  size_t n;

  /**
   * @brief This member specifies the count of bytes of the memory area which
   *   hold the arranged content.
   */
  size_t len;

  /**
   * @brief This member contains the expected return value.
   */
  const unsigned char *expected;

  /**
   * @brief This member contains the return value of the memchr() call.
   */
  void *retval;

  /**
   * @brief This member contains the value of errno after the memchr() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    CStringReqMemchr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqMemchr_Context;

static CStringReqMemchr_Context CStringReqMemchr_Instance;

static const char *const CStringReqMemchr_PreDesc_N[] =
  { "Zero", "One", "Short", "BelowWord", "Word", "AboveWord", "Long", "NA" };

static const char *const CStringReqMemchr_PreDesc_Bound[] =
  { "Inside", "Beyond", "NA" };

static const char *const CStringReqMemchr_PreDesc_Alignment[] =
  { "Aligned", "AboveAligned", "Between", "BelowAligned", "NA" };

static const char *const CStringReqMemchr_PreDesc_C[] =
  { "InRange", "Negative", "Greater", "NA" };

static const char *const CStringReqMemchr_PreDesc_Match[] =
  { "None", "One", "Several", "NA" };

static const char *const *const CStringReqMemchr_PreDesc[] = {
  CStringReqMemchr_PreDesc_N,
  CStringReqMemchr_PreDesc_Bound,
  CStringReqMemchr_PreDesc_Alignment,
  CStringReqMemchr_PreDesc_C,
  CStringReqMemchr_PreDesc_Match,
  NULL
};

#define FILL 0x11

#define TARGET 0xa5

static void CStringReqMemchr_Pre_N_Prepare(
  CStringReqMemchr_Context *ctx,
  CStringReqMemchr_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_N_Zero: {
      /*
       * While the size specified by `n` is equal to zero.
       */
      ctx->n = 0;
      ctx->len = ctx->n;
      break;
    }

    case CStringReqMemchr_Pre_N_One: {
      /*
       * While the size specified by `n` is equal to one.
       */
      ctx->n = 1;
      ctx->len = ctx->n;
      break;
    }

    case CStringReqMemchr_Pre_N_Short: {
      /*
       * While the size specified by `n` is greater than one, while the size
       * specified by `n` is less than the size of type long minus one.
       */
      ctx->n = 2;
      ctx->len = ctx->n;
      break;
    }

    case CStringReqMemchr_Pre_N_BelowWord: {
      /*
       * While the size specified by `n` is equal to the size of type long
       * minus one.
       */
      ctx->n = sizeof( long ) - 1;
      ctx->len = ctx->n;
      break;
    }

    case CStringReqMemchr_Pre_N_Word: {
      /*
       * While the size specified by `n` is equal to the size of type long.
       */
      ctx->n = sizeof( long );
      ctx->len = ctx->n;
      break;
    }

    case CStringReqMemchr_Pre_N_AboveWord: {
      /*
       * While the size specified by `n` is equal to the size of type long plus
       * one.
       */
      ctx->n = sizeof( long ) + 1;
      ctx->len = ctx->n;
      break;
    }

    case CStringReqMemchr_Pre_N_Long: {
      /*
       * While the size specified by `n` is greater than the size of type long
       * plus one.
       */
      ctx->n = 5 * sizeof( long ) + 1;
      ctx->len = ctx->n;
      break;
    }

    case CStringReqMemchr_Pre_N_NA:
      break;
  }
}

static void CStringReqMemchr_Pre_Bound_Prepare(
  CStringReqMemchr_Context  *ctx,
  CStringReqMemchr_Pre_Bound state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_Bound_Inside: {
      /*
       * While the size specified by `n` is less than or equal to the count of
       * bytes from the address specified by the `s` parameter to the end of
       * the object which contains it.
       */
      /* The N pre-condition sets the size. */
      break;
    }

    case CStringReqMemchr_Pre_Bound_Beyond: {
      /*
       * While the size specified by `n` is greater than the count of bytes
       * from the address specified by the `s` parameter to the end of the
       * object which contains it.
       */
      ctx->n = SIZE_MAX;
      break;
    }

    case CStringReqMemchr_Pre_Bound_NA:
      break;
  }
}

static void CStringReqMemchr_Pre_Alignment_Prepare(
  CStringReqMemchr_Context      *ctx,
  CStringReqMemchr_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_Alignment_Aligned: {
      /*
       * While the memory area referenced by the `s` parameter starts at an
       * address which is aligned to the size of type long.
       */
      ctx->s = &ctx->buf[ sizeof( long ) ];
      break;
    }

    case CStringReqMemchr_Pre_Alignment_AboveAligned: {
      /*
       * While the memory area referenced by the `s` parameter starts one byte
       * above an address which is aligned to the size of type long.
       */
      ctx->s = &ctx->buf[ sizeof( long ) + 1 ];
      break;
    }

    case CStringReqMemchr_Pre_Alignment_Between: {
      /*
       * While the memory area referenced by the `s` parameter starts more than
       * one byte above an address which is aligned to the size of type long,
       * while the memory area referenced by the `s` parameter starts more than
       * one byte below the next address which is aligned to the size of type
       * long.
       */
      ctx->s = &ctx->buf[ sizeof( long ) + 2 ];
      break;
    }

    case CStringReqMemchr_Pre_Alignment_BelowAligned: {
      /*
       * While the memory area referenced by the `s` parameter starts one byte
       * below an address which is aligned to the size of type long.
       */
      ctx->s = &ctx->buf[ sizeof( long ) + sizeof( long ) - 1 ];
      break;
    }

    case CStringReqMemchr_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqMemchr_Pre_C_Prepare(
  CStringReqMemchr_Context *ctx,
  CStringReqMemchr_Pre_C    state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_C_InRange: {
      /*
       * While the value of the `c` parameter is in the range of the type
       * `unsigned char`.
       */
      ctx->c = TARGET;
      break;
    }

    case CStringReqMemchr_Pre_C_Negative: {
      /*
       * While the value of the `c` parameter is less than zero.
       */
      ctx->c = TARGET - ( UCHAR_MAX + 1 );
      break;
    }

    case CStringReqMemchr_Pre_C_Greater: {
      /*
       * While the value of the `c` parameter is greater than the maximum value
       * of the type `unsigned char`.
       */
      ctx->c = TARGET + ( UCHAR_MAX + 1 );
      break;
    }

    case CStringReqMemchr_Pre_C_NA:
      break;
  }
}

static void CStringReqMemchr_Pre_Match_Prepare(
  CStringReqMemchr_Context  *ctx,
  CStringReqMemchr_Pre_Match state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_Match_None: {
      /*
       * While no byte which lies in the first `n` bytes of the memory area
       * referenced by the `s` parameter and in the object which contains it is
       * equal to the value of the `c` parameter converted to `unsigned char`.
       */
      ctx->s[ -1 ] = TARGET;
      ctx->s[ ctx->len ] = TARGET;
      break;
    }

    case CStringReqMemchr_Pre_Match_One: {
      /*
       * While exactly one byte which lies in the first `n` bytes of the memory
       * area referenced by the `s` parameter and in the object which contains
       * it is equal to the value of the `c` parameter converted to `unsigned
       * char`.
       */
      ctx->s[ ctx->len - 1 ] = TARGET;
      ctx->expected = &ctx->s[ ctx->len - 1 ];
      break;
    }

    case CStringReqMemchr_Pre_Match_Several: {
      /*
       * While more than one byte which lies in the first `n` bytes of the
       * memory area referenced by the `s` parameter and in the object which
       * contains it is equal to the value of the `c` parameter converted to
       * `unsigned char`.
       */
      if ( ctx->len > 2 ) {
        ctx->s[ 1 ] = TARGET;
        ctx->expected = &ctx->s[ 1 ];
      } else {
        ctx->s[ 0 ] = TARGET;
        ctx->expected = &ctx->s[ 0 ];
      }

      ctx->s[ ctx->len - 1 ] = TARGET;
      break;
    }

    case CStringReqMemchr_Pre_Match_NA:
      break;
  }
}

static void CStringReqMemchr_Post_Result_Check(
  CStringReqMemchr_Context    *ctx,
  CStringReqMemchr_Post_Result state
)
{
  switch ( state ) {
    case CStringReqMemchr_Post_Result_First: {
      /*
       * The return value of memchr() shall be the address of the first byte
       * which lies in the first `n` bytes of the memory area referenced by the
       * `s` parameter and in the object which contains it and which is equal
       * to the value of the `c` parameter converted to `unsigned char`.
       */
      T_eq_ptr( ctx->retval, ctx->expected );
      break;
    }

    case CStringReqMemchr_Post_Result_Null: {
      /*
       * The return value of memchr() shall be NULL.
       */
      T_null( ctx->retval );
      break;
    }

    case CStringReqMemchr_Post_Result_NA:
      break;
  }
}

static void CStringReqMemchr_Post_Area_Check(
  CStringReqMemchr_Context  *ctx,
  CStringReqMemchr_Post_Area state
)
{
  switch ( state ) {
    case CStringReqMemchr_Post_Area_Nop: {
      /*
       * The bytes of the memory area referenced by the `s` parameter shall not
       * be modified.
       */
      T_eq_mem( ctx->buf, ctx->before, sizeof( ctx->buf ) );
      break;
    }

    case CStringReqMemchr_Post_Area_NA:
      break;
  }
}

static void CStringReqMemchr_Post_Errno_Check(
  CStringReqMemchr_Context   *ctx,
  CStringReqMemchr_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqMemchr_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqMemchr_Post_Errno_NA:
      break;
  }
}

static void CStringReqMemchr_Prepare( CStringReqMemchr_Context *ctx )
{
  memset( ctx->buf, FILL, sizeof( ctx->buf ) );
  ctx->s = NULL;
  ctx->c = 0;
  ctx->n = 0;
  ctx->len = 0;
  ctx->expected = NULL;
  ctx->retval = NULL;
}

static void CStringReqMemchr_Action( CStringReqMemchr_Context *ctx )
{
  memcpy( ctx->before, ctx->buf, sizeof( ctx->buf ) );
  errno = -1;
  ctx->retval = memchr( ctx->s, ctx->c, ctx->n );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqMemchr_Entry
CStringReqMemchr_Entries[] = {
  { 0, 0, 0, 0, 0, 0, CStringReqMemchr_Post_Result_First,
    CStringReqMemchr_Post_Area_Nop, CStringReqMemchr_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 0, CStringReqMemchr_Post_Result_NA,
    CStringReqMemchr_Post_Area_NA, CStringReqMemchr_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 0, CStringReqMemchr_Post_Result_Null,
    CStringReqMemchr_Post_Area_Nop, CStringReqMemchr_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 0, CStringReqMemchr_Post_Result_NA,
    CStringReqMemchr_Post_Area_NA, CStringReqMemchr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, CStringReqMemchr_Post_Result_NA,
    CStringReqMemchr_Post_Area_NA, CStringReqMemchr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, CStringReqMemchr_Post_Result_NA,
    CStringReqMemchr_Post_Area_NA, CStringReqMemchr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, CStringReqMemchr_Post_Result_NA,
    CStringReqMemchr_Post_Area_NA, CStringReqMemchr_Post_Errno_NA }
};

static const uint8_t
CStringReqMemchr_Map[] = {
  2, 5, 5, 2, 5, 5, 2, 5, 5, 2, 5, 5, 2, 5, 5, 2, 5, 5, 2, 5, 5, 2, 5, 5, 2, 5,
  5, 2, 5, 5, 2, 5, 5, 2, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 0, 6, 2, 0, 6,
  2, 0, 6, 2, 0, 6, 2, 0, 6, 2, 0, 6, 2, 0, 6, 2, 0, 6, 2, 0, 6, 2, 0, 6, 2, 0,
  6, 2, 0, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0,
  2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0,
  2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0,
  2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 2,
  0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0,
  2, 0, 0, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2,
  0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0,
  3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0,
  0, 3, 0, 0, 3, 0, 0, 3, 0, 0
};

/* clang-format on */

static size_t CStringReqMemchr_Scope( void *arg, char *buf, size_t n )
{
  CStringReqMemchr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqMemchr_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqMemchr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqMemchr_Scope,
  .initial_context = &CStringReqMemchr_Instance
};

static inline CStringReqMemchr_Entry CStringReqMemchr_PopEntry(
  CStringReqMemchr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqMemchr_Entries[ CStringReqMemchr_Map[ index ] ];
}

static void CStringReqMemchr_TestVariant( CStringReqMemchr_Context *ctx )
{
  CStringReqMemchr_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqMemchr_Pre_Bound_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqMemchr_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqMemchr_Pre_C_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqMemchr_Pre_Match_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CStringReqMemchr_Action( ctx );
  CStringReqMemchr_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqMemchr_Post_Area_Check( ctx, ctx->Map.entry.Post_Area );
  CStringReqMemchr_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqMemchr( void )
 */
T_TEST_CASE_FIXTURE( CStringReqMemchr, &CStringReqMemchr_Fixture )
{
  CStringReqMemchr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringReqMemchr_Pre_N_Zero;
    ctx->Map.pcs[ 0 ] < CStringReqMemchr_Pre_N_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqMemchr_Pre_Bound_Inside;
      ctx->Map.pcs[ 1 ] < CStringReqMemchr_Pre_Bound_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringReqMemchr_Pre_Alignment_Aligned;
        ctx->Map.pcs[ 2 ] < CStringReqMemchr_Pre_Alignment_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CStringReqMemchr_Pre_C_InRange;
          ctx->Map.pcs[ 3 ] < CStringReqMemchr_Pre_C_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = CStringReqMemchr_Pre_Match_None;
            ctx->Map.pcs[ 4 ] < CStringReqMemchr_Pre_Match_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = CStringReqMemchr_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            CStringReqMemchr_Prepare( ctx );
            CStringReqMemchr_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
