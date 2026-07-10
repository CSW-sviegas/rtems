/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqStrcasecmp
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

#include <errno.h>
#include <string.h>
#include <strings.h>

#include <rtems/test.h>

/**
 * @defgroup CStringsReqStrcasecmp spec:/c/strings/req/strcasecmp
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStringsReqStrcasecmp_Pre_Stop_BothEnd,
  CStringsReqStrcasecmp_Pre_Stop_S1Ends,
  CStringsReqStrcasecmp_Pre_Stop_S2Ends,
  CStringsReqStrcasecmp_Pre_Stop_Less,
  CStringsReqStrcasecmp_Pre_Stop_Greater,
  CStringsReqStrcasecmp_Pre_Stop_NA
} CStringsReqStrcasecmp_Pre_Stop;

typedef enum {
  CStringsReqStrcasecmp_Pre_Letters_None,
  CStringsReqStrcasecmp_Pre_Letters_One,
  CStringsReqStrcasecmp_Pre_Letters_Both,
  CStringsReqStrcasecmp_Pre_Letters_NA
} CStringsReqStrcasecmp_Pre_Letters;

typedef enum {
  CStringsReqStrcasecmp_Pre_CaseDiffers_Yes,
  CStringsReqStrcasecmp_Pre_CaseDiffers_No,
  CStringsReqStrcasecmp_Pre_CaseDiffers_NA
} CStringsReqStrcasecmp_Pre_CaseDiffers;

typedef enum {
  CStringsReqStrcasecmp_Post_Result_Less,
  CStringsReqStrcasecmp_Post_Result_Equal,
  CStringsReqStrcasecmp_Post_Result_Greater,
  CStringsReqStrcasecmp_Post_Result_NA
} CStringsReqStrcasecmp_Post_Result;

typedef enum {
  CStringsReqStrcasecmp_Post_S1_Nop,
  CStringsReqStrcasecmp_Post_S1_NA
} CStringsReqStrcasecmp_Post_S1;

typedef enum {
  CStringsReqStrcasecmp_Post_S2_Nop,
  CStringsReqStrcasecmp_Post_S2_NA
} CStringsReqStrcasecmp_Post_S2;

typedef enum {
  CStringsReqStrcasecmp_Post_Errno_Nop,
  CStringsReqStrcasecmp_Post_Errno_NA
} CStringsReqStrcasecmp_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Stop_NA : 1;
  uint16_t Pre_Letters_NA : 1;
  uint16_t Pre_CaseDiffers_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_S1 : 1;
  uint16_t Post_S2 : 1;
  uint16_t Post_Errno : 1;
} CStringsReqStrcasecmp_Entry;

/**
 * @brief Test context for spec:/c/strings/req/strcasecmp test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `s1` parameter value.
   */
  const char *s1;

  /**
   * @brief This member specifies the `s2` parameter value.
   */
  const char *s2;

  /**
   * @brief This member contains the return value of the strcasecmp() call.
   */
  int retval;

  /**
   * @brief This member contains the value of errno after the strcasecmp()
   *   call.
   */
  int errno_value;

  /**
   * @brief This member specifies the stop index of the strings.
   */
  size_t stop;

  /**
   * @brief This member references the pairs of bytes at the stop index, one
   *   pair for each number of letters.
   */
  const unsigned char ( *pairs )[ 2 ];

  /**
   * @brief This member provides the string referenced by the `s1` parameter.
   */
  char s1_buf[ 8 ];

  /**
   * @brief This member provides the string referenced by the `s2` parameter.
   */
  char s2_buf[ 8 ];

  /**
   * @brief This member contains the content of the string referenced by the
   *   `s1` parameter before the strcasecmp() call.
   */
  char s1_before[ 8 ];

  /**
   * @brief This member contains the content of the string referenced by the
   *   `s2` parameter before the strcasecmp() call.
   */
  char s2_before[ 8 ];

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
    CStringsReqStrcasecmp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqStrcasecmp_Context;

static CStringsReqStrcasecmp_Context CStringsReqStrcasecmp_Instance;

static const char *const CStringsReqStrcasecmp_PreDesc_Stop[] =
  { "BothEnd", "S1Ends", "S2Ends", "Less", "Greater", "NA" };

static const char *const CStringsReqStrcasecmp_PreDesc_Letters[] =
  { "None", "One", "Both", "NA" };

static const char *const CStringsReqStrcasecmp_PreDesc_CaseDiffers[] =
  { "Yes", "No", "NA" };

static const char *const *const CStringsReqStrcasecmp_PreDesc[] = {
  CStringsReqStrcasecmp_PreDesc_Stop,
  CStringsReqStrcasecmp_PreDesc_Letters,
  CStringsReqStrcasecmp_PreDesc_CaseDiffers,
  NULL
};

typedef CStringsReqStrcasecmp_Context Context;

static const unsigned char both_end_pairs[ 3 ][ 2 ] =
  { { '\0', '\0' }, { '\0', '\0' }, { '\0', '\0' } };

static const unsigned char s1_ends_pairs[ 3 ][ 2 ] =
  { { '\0', 0xff }, { '\0', 'a' }, { '\0', '\0' } };

static const unsigned char s2_ends_pairs[ 3 ][ 2 ] =
  { { 0xff, '\0' }, { 'a', '\0' }, { '\0', '\0' } };

static const unsigned char less_pairs[ 3 ][ 2 ] =
  { { '!', 0x80 }, { '_', 'A' }, { 'a', 'B' } };

static const unsigned char greater_pairs[ 3 ][ 2 ] =
  { { 0x80, '!' }, { 'A', '_' }, { 'B', 'a' } };

static void SetPair( Context *ctx, const unsigned char *pair )
{
  memcpy( &ctx->s1_buf[ ctx->stop ], &pair[ 0 ], 1 );
  memcpy( &ctx->s2_buf[ ctx->stop ], &pair[ 1 ], 1 );
  ctx->s1_buf[ ctx->stop + 1 ] = '\0';
  ctx->s2_buf[ ctx->stop + 1 ] = '\0';
}

static void CStringsReqStrcasecmp_Pre_Stop_Prepare(
  CStringsReqStrcasecmp_Context *ctx,
  CStringsReqStrcasecmp_Pre_Stop state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Pre_Stop_BothEnd: {
      /*
       * While both strings hold a null byte at the stop index.
       */
      ctx->pairs = both_end_pairs;
      break;
    }

    case CStringsReqStrcasecmp_Pre_Stop_S1Ends: {
      /*
       * While the string referenced by the `s1` parameter holds a null byte at
       * the stop index, while the string referenced by the `s2` parameter
       * holds no null byte at the stop index.
       */
      ctx->pairs = s1_ends_pairs;
      break;
    }

    case CStringsReqStrcasecmp_Pre_Stop_S2Ends: {
      /*
       * While the string referenced by the `s1` parameter holds no null byte
       * at the stop index, while the string referenced by the `s2` parameter
       * holds a null byte at the stop index.
       */
      ctx->pairs = s2_ends_pairs;
      break;
    }

    case CStringsReqStrcasecmp_Pre_Stop_Less: {
      /*
       * While no string holds a null byte at the stop index, while the byte of
       * the string referenced by the `s1` parameter at the stop index is less
       * than the byte of the string referenced by the `s2` parameter at the
       * stop index, after the conversion of each uppercase letter to the
       * corresponding lowercase letter, where both bytes are interpreted as
       * type `unsigned char`.
       */
      ctx->pairs = less_pairs;
      break;
    }

    case CStringsReqStrcasecmp_Pre_Stop_Greater: {
      /*
       * While no string holds a null byte at the stop index, while the byte of
       * the string referenced by the `s1` parameter at the stop index is
       * greater than the byte of the string referenced by the `s2` parameter
       * at the stop index, after the conversion of each uppercase letter to
       * the corresponding lowercase letter, where both bytes are interpreted
       * as type `unsigned char`.
       */
      ctx->pairs = greater_pairs;
      break;
    }

    case CStringsReqStrcasecmp_Pre_Stop_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Pre_Letters_Prepare(
  CStringsReqStrcasecmp_Context    *ctx,
  CStringsReqStrcasecmp_Pre_Letters state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Pre_Letters_None: {
      /*
       * While no byte of the strings at the stop index is an uppercase letter
       * or a lowercase letter.
       */
      SetPair( ctx, ctx->pairs[ 0 ] );
      break;
    }

    case CStringsReqStrcasecmp_Pre_Letters_One: {
      /*
       * While exactly one byte of the strings at the stop index is an
       * uppercase letter or a lowercase letter.
       */
      SetPair( ctx, ctx->pairs[ 1 ] );
      break;
    }

    case CStringsReqStrcasecmp_Pre_Letters_Both: {
      /*
       * While each byte of the strings at the stop index is an uppercase
       * letter or a lowercase letter.
       */
      SetPair( ctx, ctx->pairs[ 2 ] );
      break;
    }

    case CStringsReqStrcasecmp_Pre_Letters_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Pre_CaseDiffers_Prepare(
  CStringsReqStrcasecmp_Context        *ctx,
  CStringsReqStrcasecmp_Pre_CaseDiffers state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Pre_CaseDiffers_Yes: {
      /*
       * While the strings hold different bytes at an index less than the stop
       * index.
       */
      ctx->s1_buf[ 0 ] = 'X';
      ctx->s2_buf[ 1 ] = 'X';
      break;
    }

    case CStringsReqStrcasecmp_Pre_CaseDiffers_No: {
      /*
       * While the strings hold equal bytes at each index less than the stop
       * index.
       */
      /* The prepared strings hold equal bytes. */
      break;
    }

    case CStringsReqStrcasecmp_Pre_CaseDiffers_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Post_Result_Check(
  CStringsReqStrcasecmp_Context    *ctx,
  CStringsReqStrcasecmp_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Post_Result_Less: {
      /*
       * The return value of strcasecmp() shall be less than zero.
       */
      T_lt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrcasecmp_Post_Result_Equal: {
      /*
       * The return value of strcasecmp() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrcasecmp_Post_Result_Greater: {
      /*
       * The return value of strcasecmp() shall be greater than zero.
       */
      T_gt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrcasecmp_Post_Result_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Post_S1_Check(
  CStringsReqStrcasecmp_Context *ctx,
  CStringsReqStrcasecmp_Post_S1  state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Post_S1_Nop: {
      /*
       * The bytes of the string referenced by the `s1` parameter shall not be
       * modified.
       */
      T_eq_mem( ctx->s1_buf, ctx->s1_before, sizeof( ctx->s1_buf ) );
      break;
    }

    case CStringsReqStrcasecmp_Post_S1_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Post_S2_Check(
  CStringsReqStrcasecmp_Context *ctx,
  CStringsReqStrcasecmp_Post_S2  state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Post_S2_Nop: {
      /*
       * The bytes of the string referenced by the `s2` parameter shall not be
       * modified.
       */
      T_eq_mem( ctx->s2_buf, ctx->s2_before, sizeof( ctx->s2_buf ) );
      break;
    }

    case CStringsReqStrcasecmp_Post_S2_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Post_Errno_Check(
  CStringsReqStrcasecmp_Context   *ctx,
  CStringsReqStrcasecmp_Post_Errno state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringsReqStrcasecmp_Post_Errno_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Prepare( CStringsReqStrcasecmp_Context *ctx )
{
  memset( ctx->s1_buf, 'x', sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 'x', sizeof( ctx->s2_buf ) );
  ctx->s1 = ctx->s1_buf;
  ctx->s2 = ctx->s2_buf;
  ctx->stop = 3;
}

static void CStringsReqStrcasecmp_Action( CStringsReqStrcasecmp_Context *ctx )
{
  memcpy( ctx->s1_before, ctx->s1_buf, sizeof( ctx->s1_buf ) );
  memcpy( ctx->s2_before, ctx->s2_buf, sizeof( ctx->s2_buf ) );
  errno = -1;
  ctx->retval = strcasecmp( ctx->s1, ctx->s2 );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringsReqStrcasecmp_Entry
CStringsReqStrcasecmp_Entries[] = {
  { 0, 0, 0, 0, CStringsReqStrcasecmp_Post_Result_Less,
    CStringsReqStrcasecmp_Post_S1_Nop, CStringsReqStrcasecmp_Post_S2_Nop,
    CStringsReqStrcasecmp_Post_Errno_Nop },
  { 0, 0, 0, 0, CStringsReqStrcasecmp_Post_Result_Greater,
    CStringsReqStrcasecmp_Post_S1_Nop, CStringsReqStrcasecmp_Post_S2_Nop,
    CStringsReqStrcasecmp_Post_Errno_Nop },
  { 1, 0, 0, 0, CStringsReqStrcasecmp_Post_Result_NA,
    CStringsReqStrcasecmp_Post_S1_NA, CStringsReqStrcasecmp_Post_S2_NA,
    CStringsReqStrcasecmp_Post_Errno_NA },
  { 0, 0, 0, 0, CStringsReqStrcasecmp_Post_Result_Equal,
    CStringsReqStrcasecmp_Post_S1_Nop, CStringsReqStrcasecmp_Post_S2_Nop,
    CStringsReqStrcasecmp_Post_Errno_Nop }
};

static const uint8_t
CStringsReqStrcasecmp_Map[] = {
  3, 3, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 1, 1
};

/* clang-format on */

static size_t CStringsReqStrcasecmp_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqStrcasecmp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringsReqStrcasecmp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringsReqStrcasecmp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqStrcasecmp_Scope,
  .initial_context = &CStringsReqStrcasecmp_Instance
};

static inline CStringsReqStrcasecmp_Entry CStringsReqStrcasecmp_PopEntry(
  CStringsReqStrcasecmp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqStrcasecmp_Entries[ CStringsReqStrcasecmp_Map[ index ] ];
}

static void CStringsReqStrcasecmp_TestVariant(
  CStringsReqStrcasecmp_Context *ctx
)
{
  CStringsReqStrcasecmp_Pre_Stop_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqStrcasecmp_Pre_Letters_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringsReqStrcasecmp_Pre_CaseDiffers_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringsReqStrcasecmp_Action( ctx );
  CStringsReqStrcasecmp_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringsReqStrcasecmp_Post_S1_Check( ctx, ctx->Map.entry.Post_S1 );
  CStringsReqStrcasecmp_Post_S2_Check( ctx, ctx->Map.entry.Post_S2 );
  CStringsReqStrcasecmp_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringsReqStrcasecmp( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqStrcasecmp, &CStringsReqStrcasecmp_Fixture )
{
  CStringsReqStrcasecmp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqStrcasecmp_Pre_Stop_BothEnd;
    ctx->Map.pcs[ 0 ] < CStringsReqStrcasecmp_Pre_Stop_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringsReqStrcasecmp_Pre_Letters_None;
      ctx->Map.pcs[ 1 ] < CStringsReqStrcasecmp_Pre_Letters_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringsReqStrcasecmp_Pre_CaseDiffers_Yes;
        ctx->Map.pcs[ 2 ] < CStringsReqStrcasecmp_Pre_CaseDiffers_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CStringsReqStrcasecmp_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CStringsReqStrcasecmp_Prepare( ctx );
        CStringsReqStrcasecmp_TestVariant( ctx );
      }
    }
  }
}

/** @} */
