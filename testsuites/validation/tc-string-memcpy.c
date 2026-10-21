/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqMemcpy
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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
#include <rtems.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqMemcpy spec:/c/string/req/memcpy
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * The source and the destination memory areas lie in two buffers. The test
 * fills both buffers with different patterns of distinct bytes before each
 * call. The source buffer stays unchanged, so the check compares against it.
 * The long size runs the loop of four blocks twice.
 *
 * @{
 */

typedef enum {
  CStringReqMemcpy_Pre_N_Zero,
  CStringReqMemcpy_Pre_N_One,
  CStringReqMemcpy_Pre_N_Short,
  CStringReqMemcpy_Pre_N_BelowWord,
  CStringReqMemcpy_Pre_N_Word,
  CStringReqMemcpy_Pre_N_AboveWord,
  CStringReqMemcpy_Pre_N_Medium,
  CStringReqMemcpy_Pre_N_BelowBlock,
  CStringReqMemcpy_Pre_N_Block,
  CStringReqMemcpy_Pre_N_AboveBlock,
  CStringReqMemcpy_Pre_N_Long,
  CStringReqMemcpy_Pre_N_NA
} CStringReqMemcpy_Pre_N;

typedef enum {
  CStringReqMemcpy_Pre_DestAlignment_Aligned,
  CStringReqMemcpy_Pre_DestAlignment_AboveAligned,
  CStringReqMemcpy_Pre_DestAlignment_Between,
  CStringReqMemcpy_Pre_DestAlignment_BelowAligned,
  CStringReqMemcpy_Pre_DestAlignment_NA
} CStringReqMemcpy_Pre_DestAlignment;

typedef enum {
  CStringReqMemcpy_Pre_SrcAlignment_Aligned,
  CStringReqMemcpy_Pre_SrcAlignment_AboveAligned,
  CStringReqMemcpy_Pre_SrcAlignment_Between,
  CStringReqMemcpy_Pre_SrcAlignment_BelowAligned,
  CStringReqMemcpy_Pre_SrcAlignment_NA
} CStringReqMemcpy_Pre_SrcAlignment;

typedef enum {
  CStringReqMemcpy_Post_Result_Dest,
  CStringReqMemcpy_Post_Result_NA
} CStringReqMemcpy_Post_Result;

typedef enum {
  CStringReqMemcpy_Post_Dest_Copy,
  CStringReqMemcpy_Post_Dest_NA
} CStringReqMemcpy_Post_Dest;

typedef enum {
  CStringReqMemcpy_Post_Outside_Nop,
  CStringReqMemcpy_Post_Outside_NA
} CStringReqMemcpy_Post_Outside;

typedef enum {
  CStringReqMemcpy_Post_Src_Nop,
  CStringReqMemcpy_Post_Src_NA
} CStringReqMemcpy_Post_Src;

typedef enum {
  CStringReqMemcpy_Post_Errno_Nop,
  CStringReqMemcpy_Post_Errno_NA
} CStringReqMemcpy_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_N_NA : 1;
  uint16_t Pre_DestAlignment_NA : 1;
  uint16_t Pre_SrcAlignment_NA : 1;
  uint16_t Post_Result : 1;
  uint16_t Post_Dest : 1;
  uint16_t Post_Outside : 1;
  uint16_t Post_Src : 1;
  uint16_t Post_Errno : 1;
} CStringReqMemcpy_Entry;

/**
 * @brief Test context for spec:/c/string/req/memcpy test case.
 */
typedef struct {
  /**
   * @brief This member provides the memory which holds the destination memory
   *   area.
   */
  unsigned char dest_buf[ 16 * sizeof( long ) ] RTEMS_ALIGNED(
    sizeof( long )
  );

  /**
   * @brief This member contains the content of the destination memory before
   *   the call.
   */
  unsigned char before[ 16 * sizeof( long ) ];

  /**
   * @brief This member provides the memory which holds the source memory area.
   */
  unsigned char src_buf[ 16 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member contains the content of the memory which holds the
   *   source memory area before the memcpy() call.
   */
  unsigned char src_before[ 16 * sizeof( long ) ];

  /**
   * @brief This member specifies the offset of the destination memory area
   *   from an aligned address.
   */
  size_t dest_offset;

  /**
   * @brief This member specifies the offset of the source memory area from an
   *   aligned address.
   */
  size_t src_offset;

  /**
   * @brief This member specifies the `dest` parameter value.
   */
  void *dest;

  /**
   * @brief This member specifies the `src` parameter value.
   */
  const void *src;

  /**
   * @brief This member specifies the `n` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value of the memcpy() call.
   */
  void *retval;

  /**
   * @brief This member contains the value of errno after the memcpy() call.
   */
  int errno_value;

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
    CStringReqMemcpy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqMemcpy_Context;

static CStringReqMemcpy_Context CStringReqMemcpy_Instance;

static const char *const CStringReqMemcpy_PreDesc_N[] = {
  "Zero",
  "One",
  "Short",
  "BelowWord",
  "Word",
  "AboveWord",
  "Medium",
  "BelowBlock",
  "Block",
  "AboveBlock",
  "Long",
  "NA"
};

static const char *const CStringReqMemcpy_PreDesc_DestAlignment[] =
  { "Aligned", "AboveAligned", "Between", "BelowAligned", "NA" };

static const char *const CStringReqMemcpy_PreDesc_SrcAlignment[] =
  { "Aligned", "AboveAligned", "Between", "BelowAligned", "NA" };

static const char *const *const CStringReqMemcpy_PreDesc[] = {
  CStringReqMemcpy_PreDesc_N,
  CStringReqMemcpy_PreDesc_DestAlignment,
  CStringReqMemcpy_PreDesc_SrcAlignment,
  NULL
};

typedef CStringReqMemcpy_Context Context;

static void Arrange( Context *ctx )
{
  size_t i;

  for ( i = 0; i < sizeof( ctx->dest_buf ); ++i ) {
    ctx->dest_buf[ i ] = (unsigned char) ( i + 1 );
    ctx->before[ i ] = ctx->dest_buf[ i ];
    ctx->src_buf[ i ] = (unsigned char) ( i + 0x81 );
    ctx->src_before[ i ] = ctx->src_buf[ i ];
  }

  ctx->dest = &ctx->dest_buf[ 2 * sizeof( long ) + ctx->dest_offset ];
  ctx->src = &ctx->src_buf[ 2 * sizeof( long ) + ctx->src_offset ];
}

static void CheckOutside( const Context *ctx )
{
  size_t i;
  size_t begin;

  begin = (size_t) ( (unsigned char *) ctx->dest - ctx->dest_buf );

  for ( i = 0; i < sizeof( ctx->dest_buf ); ++i ) {
    if ( i < begin || i >= begin + ctx->n ) {
      T_quiet_eq_u8( ctx->dest_buf[ i ], ctx->before[ i ] );
    }
  }
}

static void CheckSource( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < sizeof( ctx->src_buf ); ++i ) {
    T_quiet_eq_u8( ctx->src_buf[ i ], ctx->src_before[ i ] );
  }
}

static void CStringReqMemcpy_Pre_N_Prepare(
  CStringReqMemcpy_Context *ctx,
  CStringReqMemcpy_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqMemcpy_Pre_N_Zero: {
      /*
       * While the size specified by `n` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqMemcpy_Pre_N_One: {
      /*
       * While the size specified by `n` is equal to one.
       */
      ctx->n = 1;
      break;
    }

    case CStringReqMemcpy_Pre_N_Short: {
      /*
       * While the size specified by `n` is greater than one, while the size
       * specified by `n` is less than the size of type long minus one.
       */
      ctx->n = 2;
      break;
    }

    case CStringReqMemcpy_Pre_N_BelowWord: {
      /*
       * While the size specified by `n` is equal to the size of type long
       * minus one.
       */
      ctx->n = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemcpy_Pre_N_Word: {
      /*
       * While the size specified by `n` is equal to the size of type long.
       */
      ctx->n = sizeof( long );
      break;
    }

    case CStringReqMemcpy_Pre_N_AboveWord: {
      /*
       * While the size specified by `n` is equal to the size of type long plus
       * one.
       */
      ctx->n = sizeof( long ) + 1;
      break;
    }

    case CStringReqMemcpy_Pre_N_Medium: {
      /*
       * While the size specified by `n` is greater than the size of type long
       * plus one, while the size specified by `n` is less than four times the
       * size of type long minus one.
       */
      ctx->n = sizeof( long ) + 2;
      break;
    }

    case CStringReqMemcpy_Pre_N_BelowBlock: {
      /*
       * While the size specified by `n` is equal to four times the size of
       * type long minus one.
       */
      ctx->n = 4 * sizeof( long ) - 1;
      break;
    }

    case CStringReqMemcpy_Pre_N_Block: {
      /*
       * While the size specified by `n` is equal to four times the size of
       * type long.
       */
      ctx->n = 4 * sizeof( long );
      break;
    }

    case CStringReqMemcpy_Pre_N_AboveBlock: {
      /*
       * While the size specified by `n` is equal to four times the size of
       * type long plus one.
       */
      ctx->n = 4 * sizeof( long ) + 1;
      break;
    }

    case CStringReqMemcpy_Pre_N_Long: {
      /*
       * While the size specified by `n` is greater than four times the size of
       * type long plus one.
       */
      ctx->n = 9 * sizeof( long ) + 1;
      break;
    }

    case CStringReqMemcpy_Pre_N_NA:
      break;
  }
}

static void CStringReqMemcpy_Pre_DestAlignment_Prepare(
  CStringReqMemcpy_Context          *ctx,
  CStringReqMemcpy_Pre_DestAlignment state
)
{
  switch ( state ) {
    case CStringReqMemcpy_Pre_DestAlignment_Aligned: {
      /*
       * While the memory area referenced by the `dest` parameter starts at an
       * address which is aligned to the size of type long.
       */
      ctx->dest_offset = 0;
      break;
    }

    case CStringReqMemcpy_Pre_DestAlignment_AboveAligned: {
      /*
       * While the memory area referenced by the `dest` parameter starts one
       * byte above an address which is aligned to the size of type long.
       */
      ctx->dest_offset = 1;
      break;
    }

    case CStringReqMemcpy_Pre_DestAlignment_Between: {
      /*
       * While the memory area referenced by the `dest` parameter starts more
       * than one byte above an address which is aligned to the size of type
       * long, while the memory area referenced by the `dest` parameter starts
       * more than one byte below the next address which is aligned to the size
       * of type long.
       */
      ctx->dest_offset = 2;
      break;
    }

    case CStringReqMemcpy_Pre_DestAlignment_BelowAligned: {
      /*
       * While the memory area referenced by the `dest` parameter starts one
       * byte below an address which is aligned to the size of type long.
       */
      ctx->dest_offset = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemcpy_Pre_DestAlignment_NA:
      break;
  }
}

static void CStringReqMemcpy_Pre_SrcAlignment_Prepare(
  CStringReqMemcpy_Context         *ctx,
  CStringReqMemcpy_Pre_SrcAlignment state
)
{
  switch ( state ) {
    case CStringReqMemcpy_Pre_SrcAlignment_Aligned: {
      /*
       * While the memory area referenced by the `src` parameter starts at an
       * address which is aligned to the size of type long.
       */
      ctx->src_offset = 0;
      break;
    }

    case CStringReqMemcpy_Pre_SrcAlignment_AboveAligned: {
      /*
       * While the memory area referenced by the `src` parameter starts one
       * byte above an address which is aligned to the size of type long.
       */
      ctx->src_offset = 1;
      break;
    }

    case CStringReqMemcpy_Pre_SrcAlignment_Between: {
      /*
       * While the memory area referenced by the `src` parameter starts more
       * than one byte above an address which is aligned to the size of type
       * long, while the memory area referenced by the `src` parameter starts
       * more than one byte below the next address which is aligned to the size
       * of type long.
       */
      ctx->src_offset = 2;
      break;
    }

    case CStringReqMemcpy_Pre_SrcAlignment_BelowAligned: {
      /*
       * While the memory area referenced by the `src` parameter starts one
       * byte below an address which is aligned to the size of type long.
       */
      ctx->src_offset = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemcpy_Pre_SrcAlignment_NA:
      break;
  }
}

static void CStringReqMemcpy_Post_Result_Check(
  CStringReqMemcpy_Context    *ctx,
  CStringReqMemcpy_Post_Result state
)
{
  switch ( state ) {
    case CStringReqMemcpy_Post_Result_Dest: {
      /*
       * The return value of memcpy() shall be the value of the `dest`
       * parameter.
       */
      T_eq_ptr( ctx->retval, ctx->dest );
      break;
    }

    case CStringReqMemcpy_Post_Result_NA:
      break;
  }
}

static void CStringReqMemcpy_Post_Dest_Check(
  CStringReqMemcpy_Context  *ctx,
  CStringReqMemcpy_Post_Dest state
)
{
  switch ( state ) {
    case CStringReqMemcpy_Post_Dest_Copy: {
      /*
       * The first `n` bytes of the memory area referenced by the `src`
       * parameter shall be copied to the first `n` bytes of the memory area
       * referenced by the `dest` parameter.
       */
      T_eq_mem( ctx->dest, ctx->src, ctx->n );
      break;
    }

    case CStringReqMemcpy_Post_Dest_NA:
      break;
  }
}

static void CStringReqMemcpy_Post_Outside_Check(
  CStringReqMemcpy_Context     *ctx,
  CStringReqMemcpy_Post_Outside state
)
{
  switch ( state ) {
    case CStringReqMemcpy_Post_Outside_Nop: {
      /*
       * The bytes outside the first `n` bytes of the memory area referenced by
       * the `dest` parameter shall not be modified.
       */
      CheckOutside( ctx );
      break;
    }

    case CStringReqMemcpy_Post_Outside_NA:
      break;
  }
}

static void CStringReqMemcpy_Post_Src_Check(
  CStringReqMemcpy_Context *ctx,
  CStringReqMemcpy_Post_Src state
)
{
  switch ( state ) {
    case CStringReqMemcpy_Post_Src_Nop: {
      /*
       * The bytes of the memory area referenced by the `src` parameter shall
       * not be modified.
       */
      CheckSource( ctx );
      break;
    }

    case CStringReqMemcpy_Post_Src_NA:
      break;
  }
}

static void CStringReqMemcpy_Post_Errno_Check(
  CStringReqMemcpy_Context   *ctx,
  CStringReqMemcpy_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqMemcpy_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqMemcpy_Post_Errno_NA:
      break;
  }
}

static void CStringReqMemcpy_Prepare( CStringReqMemcpy_Context *ctx )
{
  ctx->n = 0;
  ctx->dest_offset = 0;
  ctx->src_offset = 0;
  ctx->retval = NULL;
}

static void CStringReqMemcpy_Action( CStringReqMemcpy_Context *ctx )
{
  Arrange( ctx );
  errno = -1;
  ctx->retval = memcpy( ctx->dest, ctx->src, ctx->n );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqMemcpy_Entry
CStringReqMemcpy_Entries[] = {
  { 0, 0, 0, 0, CStringReqMemcpy_Post_Result_Dest,
    CStringReqMemcpy_Post_Dest_Copy, CStringReqMemcpy_Post_Outside_Nop,
    CStringReqMemcpy_Post_Src_Nop, CStringReqMemcpy_Post_Errno_Nop },
  { 0, 0, 0, 0, CStringReqMemcpy_Post_Result_Dest,
    CStringReqMemcpy_Post_Dest_NA, CStringReqMemcpy_Post_Outside_Nop,
    CStringReqMemcpy_Post_Src_Nop, CStringReqMemcpy_Post_Errno_Nop }
};

static const uint8_t
CStringReqMemcpy_Map[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CStringReqMemcpy_Scope( void *arg, char *buf, size_t n )
{
  CStringReqMemcpy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqMemcpy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqMemcpy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqMemcpy_Scope,
  .initial_context = &CStringReqMemcpy_Instance
};

static inline CStringReqMemcpy_Entry CStringReqMemcpy_PopEntry(
  CStringReqMemcpy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqMemcpy_Entries[ CStringReqMemcpy_Map[ index ] ];
}

static void CStringReqMemcpy_TestVariant( CStringReqMemcpy_Context *ctx )
{
  CStringReqMemcpy_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqMemcpy_Pre_DestAlignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqMemcpy_Pre_SrcAlignment_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqMemcpy_Action( ctx );
  CStringReqMemcpy_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqMemcpy_Post_Dest_Check( ctx, ctx->Map.entry.Post_Dest );
  CStringReqMemcpy_Post_Outside_Check( ctx, ctx->Map.entry.Post_Outside );
  CStringReqMemcpy_Post_Src_Check( ctx, ctx->Map.entry.Post_Src );
  CStringReqMemcpy_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqMemcpy( void )
 */
T_TEST_CASE_FIXTURE( CStringReqMemcpy, &CStringReqMemcpy_Fixture )
{
  CStringReqMemcpy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringReqMemcpy_Pre_N_Zero;
    ctx->Map.pcs[ 0 ] < CStringReqMemcpy_Pre_N_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqMemcpy_Pre_DestAlignment_Aligned;
      ctx->Map.pcs[ 1 ] < CStringReqMemcpy_Pre_DestAlignment_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringReqMemcpy_Pre_SrcAlignment_Aligned;
        ctx->Map.pcs[ 2 ] < CStringReqMemcpy_Pre_SrcAlignment_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CStringReqMemcpy_PopEntry( ctx );
        CStringReqMemcpy_Prepare( ctx );
        CStringReqMemcpy_TestVariant( ctx );
      }
    }
  }
}

/** @} */
