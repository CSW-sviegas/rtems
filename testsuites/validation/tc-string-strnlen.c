/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrnlen
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

#include <rtems/test.h>

/**
 * @defgroup CStringReqStrnlen spec:/c/string/req/strnlen
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStringReqStrnlen_Pre_Maxlen_Zero,
  CStringReqStrnlen_Pre_Maxlen_One,
  CStringReqStrnlen_Pre_Maxlen_Between,
  CStringReqStrnlen_Pre_Maxlen_Max,
  CStringReqStrnlen_Pre_Maxlen_NA
} CStringReqStrnlen_Pre_Maxlen;

typedef enum {
  CStringReqStrnlen_Pre_Nul_Before,
  CStringReqStrnlen_Pre_Nul_Last,
  CStringReqStrnlen_Pre_Nul_At,
  CStringReqStrnlen_Pre_Nul_Next,
  CStringReqStrnlen_Pre_Nul_Beyond,
  CStringReqStrnlen_Pre_Nul_NA
} CStringReqStrnlen_Pre_Nul;

typedef enum {
  CStringReqStrnlen_Post_Result_Length,
  CStringReqStrnlen_Post_Result_Maxlen,
  CStringReqStrnlen_Post_Result_NA
} CStringReqStrnlen_Post_Result;

typedef enum {
  CStringReqStrnlen_Post_Array_Nop,
  CStringReqStrnlen_Post_Array_NA
} CStringReqStrnlen_Post_Array;

typedef enum {
  CStringReqStrnlen_Post_Errno_Nop,
  CStringReqStrnlen_Post_Errno_NA
} CStringReqStrnlen_Post_Errno;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Maxlen_NA : 1;
  uint8_t Pre_Nul_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_Array : 1;
  uint8_t Post_Errno : 1;
} CStringReqStrnlen_Entry;

/**
 * @brief Test context for spec:/c/string/req/strnlen test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `s` parameter value.
   */
  const char *s;

  /**
   * @brief This member specifies the `maxlen` parameter value.
   */
  size_t maxlen;

  /**
   * @brief This member contains the return value of the strnlen() call.
   */
  size_t retval;

  /**
   * @brief This member contains the index of the first null byte of the array.
   */
  size_t length;

  /**
   * @brief This member provides the array referenced by the `s` parameter.
   */
  char buf[ 8 ];

  /**
   * @brief This member contains the content of the array before the strnlen()
   *   call.
   */
  char buf_before[ 8 ];

  /**
   * @brief This member contains the value of errno after the strnlen() call.
   */
  int errno_value;

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
    CStringReqStrnlen_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrnlen_Context;

static CStringReqStrnlen_Context CStringReqStrnlen_Instance;

static const char *const CStringReqStrnlen_PreDesc_Maxlen[] =
  { "Zero", "One", "Between", "Max", "NA" };

static const char *const CStringReqStrnlen_PreDesc_Nul[] =
  { "Before", "Last", "At", "Next", "Beyond", "NA" };

static const char *const *const CStringReqStrnlen_PreDesc[] =
  { CStringReqStrnlen_PreDesc_Maxlen, CStringReqStrnlen_PreDesc_Nul, NULL };

static void CStringReqStrnlen_Pre_Maxlen_Prepare(
  CStringReqStrnlen_Context   *ctx,
  CStringReqStrnlen_Pre_Maxlen state
)
{
  switch ( state ) {
    case CStringReqStrnlen_Pre_Maxlen_Zero: {
      /*
       * While the `maxlen` parameter is equal to zero.
       */
      ctx->maxlen = 0;
      break;
    }

    case CStringReqStrnlen_Pre_Maxlen_One: {
      /*
       * While the `maxlen` parameter is equal to one.
       */
      ctx->maxlen = 1;
      break;
    }

    case CStringReqStrnlen_Pre_Maxlen_Between: {
      /*
       * While the `maxlen` parameter is greater than one, while the `maxlen`
       * parameter is less than SIZE_MAX.
       */
      ctx->maxlen = 4;
      break;
    }

    case CStringReqStrnlen_Pre_Maxlen_Max: {
      /*
       * While the `maxlen` parameter is equal to SIZE_MAX.
       */
      ctx->maxlen = SIZE_MAX;
      break;
    }

    case CStringReqStrnlen_Pre_Maxlen_NA:
      break;
  }
}

static void CStringReqStrnlen_Pre_Nul_Prepare(
  CStringReqStrnlen_Context *ctx,
  CStringReqStrnlen_Pre_Nul  state
)
{
  switch ( state ) {
    case CStringReqStrnlen_Pre_Nul_Before: {
      /*
       * While the first null byte of the array referenced by the `s` parameter
       * has an index less than the value of the `maxlen` parameter minus one.
       */
      if ( ctx->maxlen == SIZE_MAX ) {
        ctx->length = 3;
      } else {
        ctx->length = ctx->maxlen - 2;
      }

      ctx->buf[ ctx->length ] = '\0';
      break;
    }

    case CStringReqStrnlen_Pre_Nul_Last: {
      /*
       * While the first null byte of the array referenced by the `s` parameter
       * has an index equal to the value of the `maxlen` parameter minus one.
       */
      ctx->length = ctx->maxlen - 1;
      ctx->buf[ ctx->length ] = '\0';
      break;
    }

    case CStringReqStrnlen_Pre_Nul_At: {
      /*
       * While the first null byte of the array referenced by the `s` parameter
       * has an index equal to the value of the `maxlen` parameter.
       */
      ctx->buf[ ctx->maxlen ] = '\0';
      break;
    }

    case CStringReqStrnlen_Pre_Nul_Next: {
      /*
       * While the first null byte of the array referenced by the `s` parameter
       * has an index equal to the value of the `maxlen` parameter plus one.
       */
      ctx->buf[ ctx->maxlen + 1 ] = '\0';
      break;
    }

    case CStringReqStrnlen_Pre_Nul_Beyond: {
      /*
       * While the array referenced by the `s` parameter contains no null byte
       * at an index less than or equal to the value of the `maxlen` parameter
       * plus one.
       */
      ctx->buf[ ctx->maxlen + 2 ] = '\0';
      break;
    }

    case CStringReqStrnlen_Pre_Nul_NA:
      break;
  }
}

static void CStringReqStrnlen_Post_Result_Check(
  CStringReqStrnlen_Context    *ctx,
  CStringReqStrnlen_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrnlen_Post_Result_Length: {
      /*
       * The return value of strnlen() shall be equal to the number of bytes
       * which precede the first null byte of the array referenced by the `s`
       * parameter.
       */
      T_eq_sz( ctx->retval, ctx->length );
      break;
    }

    case CStringReqStrnlen_Post_Result_Maxlen: {
      /*
       * The return value of strnlen() shall be equal to the value of the
       * `maxlen` parameter.
       */
      T_eq_sz( ctx->retval, ctx->maxlen );
      break;
    }

    case CStringReqStrnlen_Post_Result_NA:
      break;
  }
}

static void CStringReqStrnlen_Post_Array_Check(
  CStringReqStrnlen_Context   *ctx,
  CStringReqStrnlen_Post_Array state
)
{
  switch ( state ) {
    case CStringReqStrnlen_Post_Array_Nop: {
      /*
       * The bytes of the array referenced by the `s` parameter shall not be
       * modified.
       */
      T_eq_mem( ctx->buf, ctx->buf_before, sizeof( ctx->buf ) );
      break;
    }

    case CStringReqStrnlen_Post_Array_NA:
      break;
  }
}

static void CStringReqStrnlen_Post_Errno_Check(
  CStringReqStrnlen_Context   *ctx,
  CStringReqStrnlen_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqStrnlen_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqStrnlen_Post_Errno_NA:
      break;
  }
}

static void CStringReqStrnlen_Prepare( CStringReqStrnlen_Context *ctx )
{
  memset( ctx->buf, 'x', sizeof( ctx->buf ) );
  ctx->s = ctx->buf;
}

static void CStringReqStrnlen_Action( CStringReqStrnlen_Context *ctx )
{
  memcpy( ctx->buf_before, ctx->buf, sizeof( ctx->buf ) );
  errno = -1;
  ctx->retval = strnlen( ctx->s, ctx->maxlen );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqStrnlen_Entry
CStringReqStrnlen_Entries[] = {
  { 0, 0, 0, CStringReqStrnlen_Post_Result_Maxlen,
    CStringReqStrnlen_Post_Array_Nop, CStringReqStrnlen_Post_Errno_Nop },
  { 0, 0, 0, CStringReqStrnlen_Post_Result_Length,
    CStringReqStrnlen_Post_Array_Nop, CStringReqStrnlen_Post_Errno_Nop },
  { 1, 0, 0, CStringReqStrnlen_Post_Result_NA, CStringReqStrnlen_Post_Array_NA,
    CStringReqStrnlen_Post_Errno_NA },
  { 1, 0, 0, CStringReqStrnlen_Post_Result_NA, CStringReqStrnlen_Post_Array_NA,
    CStringReqStrnlen_Post_Errno_NA }
};

static const uint8_t
CStringReqStrnlen_Map[] = {
  3, 3, 0, 0, 0, 3, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 2, 2, 2, 2
};

/* clang-format on */

static size_t CStringReqStrnlen_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrnlen_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrnlen_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrnlen_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrnlen_Scope,
  .initial_context = &CStringReqStrnlen_Instance
};

static inline CStringReqStrnlen_Entry CStringReqStrnlen_PopEntry(
  CStringReqStrnlen_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrnlen_Entries[ CStringReqStrnlen_Map[ index ] ];
}

static void CStringReqStrnlen_TestVariant( CStringReqStrnlen_Context *ctx )
{
  CStringReqStrnlen_Pre_Maxlen_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrnlen_Pre_Nul_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrnlen_Action( ctx );
  CStringReqStrnlen_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqStrnlen_Post_Array_Check( ctx, ctx->Map.entry.Post_Array );
  CStringReqStrnlen_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqStrnlen( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrnlen, &CStringReqStrnlen_Fixture )
{
  CStringReqStrnlen_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringReqStrnlen_Pre_Maxlen_Zero;
    ctx->Map.pcs[ 0 ] < CStringReqStrnlen_Pre_Maxlen_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqStrnlen_Pre_Nul_Before;
      ctx->Map.pcs[ 1 ] < CStringReqStrnlen_Pre_Nul_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CStringReqStrnlen_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      CStringReqStrnlen_Prepare( ctx );
      CStringReqStrnlen_TestVariant( ctx );
    }
  }
}

/** @} */
