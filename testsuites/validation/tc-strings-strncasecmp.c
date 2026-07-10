/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqStrncasecmp
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
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include <rtems/test.h>

/**
 * @defgroup CStringsReqStrncasecmp spec:/c/strings/req/strncasecmp
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStringsReqStrncasecmp_Pre_N_Zero,
  CStringsReqStrncasecmp_Pre_N_One,
  CStringsReqStrncasecmp_Pre_N_Between,
  CStringsReqStrncasecmp_Pre_N_Max,
  CStringsReqStrncasecmp_Pre_N_NA
} CStringsReqStrncasecmp_Pre_N;

typedef enum {
  CStringsReqStrncasecmp_Pre_Index_Before,
  CStringsReqStrncasecmp_Pre_Index_Last,
  CStringsReqStrncasecmp_Pre_Index_At,
  CStringsReqStrncasecmp_Pre_Index_Next,
  CStringsReqStrncasecmp_Pre_Index_Beyond,
  CStringsReqStrncasecmp_Pre_Index_NA
} CStringsReqStrncasecmp_Pre_Index;

typedef enum {
  CStringsReqStrncasecmp_Pre_Stop_BothEnd,
  CStringsReqStrncasecmp_Pre_Stop_S1Ends,
  CStringsReqStrncasecmp_Pre_Stop_S2Ends,
  CStringsReqStrncasecmp_Pre_Stop_Less,
  CStringsReqStrncasecmp_Pre_Stop_Greater,
  CStringsReqStrncasecmp_Pre_Stop_NA
} CStringsReqStrncasecmp_Pre_Stop;

typedef enum {
  CStringsReqStrncasecmp_Pre_Letters_None,
  CStringsReqStrncasecmp_Pre_Letters_One,
  CStringsReqStrncasecmp_Pre_Letters_Both,
  CStringsReqStrncasecmp_Pre_Letters_NA
} CStringsReqStrncasecmp_Pre_Letters;

typedef enum {
  CStringsReqStrncasecmp_Pre_CaseDiffers_Yes,
  CStringsReqStrncasecmp_Pre_CaseDiffers_No,
  CStringsReqStrncasecmp_Pre_CaseDiffers_NA
} CStringsReqStrncasecmp_Pre_CaseDiffers;

typedef enum {
  CStringsReqStrncasecmp_Post_Result_Less,
  CStringsReqStrncasecmp_Post_Result_Equal,
  CStringsReqStrncasecmp_Post_Result_Greater,
  CStringsReqStrncasecmp_Post_Result_NA
} CStringsReqStrncasecmp_Post_Result;

typedef enum {
  CStringsReqStrncasecmp_Post_S1_Nop,
  CStringsReqStrncasecmp_Post_S1_NA
} CStringsReqStrncasecmp_Post_S1;

typedef enum {
  CStringsReqStrncasecmp_Post_S2_Nop,
  CStringsReqStrncasecmp_Post_S2_NA
} CStringsReqStrncasecmp_Post_S2;

typedef enum {
  CStringsReqStrncasecmp_Post_Errno_Nop,
  CStringsReqStrncasecmp_Post_Errno_NA
} CStringsReqStrncasecmp_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_N_NA : 1;
  uint16_t Pre_Index_NA : 1;
  uint16_t Pre_Stop_NA : 1;
  uint16_t Pre_Letters_NA : 1;
  uint16_t Pre_CaseDiffers_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_S1 : 1;
  uint16_t Post_S2 : 1;
  uint16_t Post_Errno : 1;
} CStringsReqStrncasecmp_Entry;

/**
 * @brief Test context for spec:/c/strings/req/strncasecmp test case.
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
   * @brief This member specifies the `n` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value of the strncasecmp() call.
   */
  int retval;

  /**
   * @brief This member contains the value of errno after the strncasecmp()
   *   call.
   */
  int errno_value;

  /**
   * @brief This member specifies the stop index of the arrays.
   */
  size_t stop;

  /**
   * @brief This member references the pairs of bytes at the stop index, one
   *   pair for each number of letters.
   */
  const unsigned char ( *pairs )[ 2 ];

  /**
   * @brief This member provides the array referenced by the `s1` parameter.
   */
  char s1_buf[ 8 ];

  /**
   * @brief This member provides the array referenced by the `s2` parameter.
   */
  char s2_buf[ 8 ];

  /**
   * @brief This member contains the content of the array referenced by the
   *   `s1` parameter before the strncasecmp() call.
   */
  char s1_before[ 8 ];

  /**
   * @brief This member contains the content of the array referenced by the
   *   `s2` parameter before the strncasecmp() call.
   */
  char s2_before[ 8 ];

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
    CStringsReqStrncasecmp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqStrncasecmp_Context;

static CStringsReqStrncasecmp_Context CStringsReqStrncasecmp_Instance;

static const char *const CStringsReqStrncasecmp_PreDesc_N[] =
  { "Zero", "One", "Between", "Max", "NA" };

static const char *const CStringsReqStrncasecmp_PreDesc_Index[] =
  { "Before", "Last", "At", "Next", "Beyond", "NA" };

static const char *const CStringsReqStrncasecmp_PreDesc_Stop[] =
  { "BothEnd", "S1Ends", "S2Ends", "Less", "Greater", "NA" };

static const char *const CStringsReqStrncasecmp_PreDesc_Letters[] =
  { "None", "One", "Both", "NA" };

static const char *const CStringsReqStrncasecmp_PreDesc_CaseDiffers[] =
  { "Yes", "No", "NA" };

static const char *const *const CStringsReqStrncasecmp_PreDesc[] = {
  CStringsReqStrncasecmp_PreDesc_N,
  CStringsReqStrncasecmp_PreDesc_Index,
  CStringsReqStrncasecmp_PreDesc_Stop,
  CStringsReqStrncasecmp_PreDesc_Letters,
  CStringsReqStrncasecmp_PreDesc_CaseDiffers,
  NULL
};

typedef CStringsReqStrncasecmp_Context Context;

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

static void CStringsReqStrncasecmp_Pre_N_Prepare(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Pre_N    state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_N_Zero: {
      /*
       * While the `n` parameter is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_One: {
      /*
       * While the `n` parameter is equal to one.
       */
      ctx->n = 1;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_Between: {
      /*
       * While the `n` parameter is greater than one, while the `n` parameter
       * is less than SIZE_MAX.
       */
      ctx->n = 4;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_Max: {
      /*
       * While the `n` parameter is equal to SIZE_MAX.
       */
      ctx->n = SIZE_MAX;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_Index_Prepare(
  CStringsReqStrncasecmp_Context  *ctx,
  CStringsReqStrncasecmp_Pre_Index state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_Index_Before: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is less than the value of the `n` parameter
       * minus one.
       */
      if ( ctx->n == SIZE_MAX ) {
        ctx->stop = 3;
      } else {
        ctx->stop = ctx->n - 2;
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_Last: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is equal to the value of the `n` parameter
       * minus one.
       */
      ctx->stop = ctx->n - 1;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_At: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is equal to the value of the `n` parameter.
       */
      ctx->stop = ctx->n;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_Next: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is equal to the value of the `n` parameter plus
       * one.
       */
      ctx->stop = ctx->n + 1;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_Beyond: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is greater than the value of the `n` parameter
       * plus one.
       */
      ctx->stop = ctx->n + 2;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_Stop_Prepare(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Pre_Stop state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_Stop_BothEnd: {
      /*
       * While both arrays hold a null byte at the stop index.
       */
      ctx->pairs = both_end_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_S1Ends: {
      /*
       * While the array referenced by the `s1` parameter holds a null byte at
       * the stop index, while the array referenced by the `s2` parameter holds
       * no null byte at the stop index.
       */
      ctx->pairs = s1_ends_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_S2Ends: {
      /*
       * While the array referenced by the `s1` parameter holds no null byte at
       * the stop index, while the array referenced by the `s2` parameter holds
       * a null byte at the stop index.
       */
      ctx->pairs = s2_ends_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_Less: {
      /*
       * While no array holds a null byte at the stop index, while the byte of
       * the array referenced by the `s1` parameter at the stop index is less
       * than the byte of the array referenced by the `s2` parameter at the
       * stop index, after the conversion of each uppercase letter to the
       * corresponding lowercase letter, where both bytes are interpreted as
       * type `unsigned char`.
       */
      ctx->pairs = less_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_Greater: {
      /*
       * While no array holds a null byte at the stop index, while the byte of
       * the array referenced by the `s1` parameter at the stop index is
       * greater than the byte of the array referenced by the `s2` parameter at
       * the stop index, after the conversion of each uppercase letter to the
       * corresponding lowercase letter, where both bytes are interpreted as
       * type `unsigned char`.
       */
      ctx->pairs = greater_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_Letters_Prepare(
  CStringsReqStrncasecmp_Context    *ctx,
  CStringsReqStrncasecmp_Pre_Letters state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_Letters_None: {
      /*
       * While no byte of the arrays at the stop index is an uppercase letter
       * or a lowercase letter.
       */
      SetPair( ctx, ctx->pairs[ 0 ] );
      break;
    }

    case CStringsReqStrncasecmp_Pre_Letters_One: {
      /*
       * While exactly one byte of the arrays at the stop index is an uppercase
       * letter or a lowercase letter.
       */
      SetPair( ctx, ctx->pairs[ 1 ] );
      break;
    }

    case CStringsReqStrncasecmp_Pre_Letters_Both: {
      /*
       * While each byte of the arrays at the stop index is an uppercase letter
       * or a lowercase letter.
       */
      SetPair( ctx, ctx->pairs[ 2 ] );
      break;
    }

    case CStringsReqStrncasecmp_Pre_Letters_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_CaseDiffers_Prepare(
  CStringsReqStrncasecmp_Context        *ctx,
  CStringsReqStrncasecmp_Pre_CaseDiffers state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_CaseDiffers_Yes: {
      /*
       * While the arrays hold different bytes at an index less than the stop
       * index.
       */
      ctx->s1_buf[ 0 ] = 'X';

      if ( ctx->stop > 1 ) {
        ctx->s2_buf[ 1 ] = 'X';
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_CaseDiffers_No: {
      /*
       * While the arrays hold equal bytes at each index less than the stop
       * index.
       */
      /* The prepared arrays hold equal bytes. */
      break;
    }

    case CStringsReqStrncasecmp_Pre_CaseDiffers_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_Result_Check(
  CStringsReqStrncasecmp_Context    *ctx,
  CStringsReqStrncasecmp_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_Result_Less: {
      /*
       * The return value of strncasecmp() shall be less than zero.
       */
      T_lt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_Equal: {
      /*
       * The return value of strncasecmp() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_Greater: {
      /*
       * The return value of strncasecmp() shall be greater than zero.
       */
      T_gt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_S1_Check(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Post_S1  state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_S1_Nop: {
      /*
       * The bytes of the array referenced by the `s1` parameter shall not be
       * modified.
       */
      T_eq_mem( ctx->s1_buf, ctx->s1_before, sizeof( ctx->s1_buf ) );
      break;
    }

    case CStringsReqStrncasecmp_Post_S1_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_S2_Check(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Post_S2  state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_S2_Nop: {
      /*
       * The bytes of the array referenced by the `s2` parameter shall not be
       * modified.
       */
      T_eq_mem( ctx->s2_buf, ctx->s2_before, sizeof( ctx->s2_buf ) );
      break;
    }

    case CStringsReqStrncasecmp_Post_S2_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_Errno_Check(
  CStringsReqStrncasecmp_Context   *ctx,
  CStringsReqStrncasecmp_Post_Errno state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Errno_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Prepare(
  CStringsReqStrncasecmp_Context *ctx
)
{
  memset( ctx->s1_buf, 'x', sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 'x', sizeof( ctx->s2_buf ) );
  ctx->s1 = ctx->s1_buf;
  ctx->s2 = ctx->s2_buf;
}

static void CStringsReqStrncasecmp_Action(
  CStringsReqStrncasecmp_Context *ctx
)
{
  memcpy( ctx->s1_before, ctx->s1_buf, sizeof( ctx->s1_buf ) );
  memcpy( ctx->s2_before, ctx->s2_buf, sizeof( ctx->s2_buf ) );
  errno = -1;
  ctx->retval = strncasecmp( ctx->s1, ctx->s2, ctx->n );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringsReqStrncasecmp_Entry
CStringsReqStrncasecmp_Entries[] = {
  { 0, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Equal,
    CStringsReqStrncasecmp_Post_S1_Nop, CStringsReqStrncasecmp_Post_S2_Nop,
    CStringsReqStrncasecmp_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_NA,
    CStringsReqStrncasecmp_Post_S1_NA, CStringsReqStrncasecmp_Post_S2_NA,
    CStringsReqStrncasecmp_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_NA,
    CStringsReqStrncasecmp_Post_S1_NA, CStringsReqStrncasecmp_Post_S2_NA,
    CStringsReqStrncasecmp_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_NA,
    CStringsReqStrncasecmp_Post_S1_NA, CStringsReqStrncasecmp_Post_S2_NA,
    CStringsReqStrncasecmp_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Less,
    CStringsReqStrncasecmp_Post_S1_Nop, CStringsReqStrncasecmp_Post_S2_Nop,
    CStringsReqStrncasecmp_Post_Errno_Nop },
  { 0, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Greater,
    CStringsReqStrncasecmp_Post_S1_Nop, CStringsReqStrncasecmp_Post_S2_Nop,
    CStringsReqStrncasecmp_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_NA,
    CStringsReqStrncasecmp_Post_S1_NA, CStringsReqStrncasecmp_Post_S2_NA,
    CStringsReqStrncasecmp_Post_Errno_NA }
};

static const uint8_t
CStringsReqStrncasecmp_Map[] = {
  3, 3, 1, 1, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 6, 0, 1, 1, 1, 1, 6, 0, 6, 0, 1, 1, 6, 0, 6, 0, 1, 1,
  6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0,
  0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 1, 1, 1,
  3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 0,
  1, 1, 1, 1, 6, 4, 6, 4, 1, 1, 6, 5, 6, 5, 1, 1, 6, 4, 6, 4, 6, 4, 6, 5, 6, 5,
  6, 5, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 4, 4, 4, 4, 1, 1,
  5, 5, 5, 5, 1, 1, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 0, 0, 1, 1, 1, 1, 4, 4,
  4, 4, 1, 1, 5, 5, 5, 5, 1, 1, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 4, 4, 4, 4, 1, 1, 5, 5, 5, 5, 1, 1,
  4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2,
  2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2,
  1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
  2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2
};

/* clang-format on */

static size_t CStringsReqStrncasecmp_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqStrncasecmp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringsReqStrncasecmp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringsReqStrncasecmp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqStrncasecmp_Scope,
  .initial_context = &CStringsReqStrncasecmp_Instance
};

static inline CStringsReqStrncasecmp_Entry CStringsReqStrncasecmp_PopEntry(
  CStringsReqStrncasecmp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqStrncasecmp_Entries[ CStringsReqStrncasecmp_Map[ index ] ];
}

static void CStringsReqStrncasecmp_TestVariant(
  CStringsReqStrncasecmp_Context *ctx
)
{
  CStringsReqStrncasecmp_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqStrncasecmp_Pre_Index_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringsReqStrncasecmp_Pre_Stop_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringsReqStrncasecmp_Pre_Letters_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringsReqStrncasecmp_Pre_CaseDiffers_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CStringsReqStrncasecmp_Action( ctx );
  CStringsReqStrncasecmp_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringsReqStrncasecmp_Post_S1_Check( ctx, ctx->Map.entry.Post_S1 );
  CStringsReqStrncasecmp_Post_S2_Check( ctx, ctx->Map.entry.Post_S2 );
  CStringsReqStrncasecmp_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringsReqStrncasecmp( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqStrncasecmp, &CStringsReqStrncasecmp_Fixture )
{
  CStringsReqStrncasecmp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqStrncasecmp_Pre_N_Zero;
    ctx->Map.pcs[ 0 ] < CStringsReqStrncasecmp_Pre_N_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringsReqStrncasecmp_Pre_Index_Before;
      ctx->Map.pcs[ 1 ] < CStringsReqStrncasecmp_Pre_Index_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringsReqStrncasecmp_Pre_Stop_BothEnd;
        ctx->Map.pcs[ 2 ] < CStringsReqStrncasecmp_Pre_Stop_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CStringsReqStrncasecmp_Pre_Letters_None;
          ctx->Map.pcs[ 3 ] < CStringsReqStrncasecmp_Pre_Letters_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = CStringsReqStrncasecmp_Pre_CaseDiffers_Yes;
            ctx->Map.pcs[ 4 ] < CStringsReqStrncasecmp_Pre_CaseDiffers_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = CStringsReqStrncasecmp_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            CStringsReqStrncasecmp_Prepare( ctx );
            CStringsReqStrncasecmp_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
