/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqMemmove
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
#include <rtems.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqMemmove spec:/c/string/req/memmove
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * The memory areas lie in one buffer, so the offsets of the two areas arrange
 * their relative position. Two overlapping areas differ by the least distance
 * which the alignment of their start addresses permits. A memory area of the
 * state Between starts two bytes above an aligned address. It starts two bytes
 * below an aligned address if the other memory area starts one byte below an
 * aligned address. The least distance of each pair of alignment states then
 * gives the same overlapping sizes for each size of type long. Memory areas
 * which start one byte below an aligned address and have a size of one more
 * than a multiple of the size of type long end at aligned addresses, so the
 * copy towards lower addresses runs its block loop. The long size runs the
 * loop of four blocks twice. The test fills the buffer with a pattern of
 * distinct bytes before each call and records the source bytes with a loop of
 * its own.
 *
 * @{
 */

typedef enum {
  CStringReqMemmove_Pre_N_Zero,
  CStringReqMemmove_Pre_N_One,
  CStringReqMemmove_Pre_N_Short,
  CStringReqMemmove_Pre_N_BelowWord,
  CStringReqMemmove_Pre_N_Word,
  CStringReqMemmove_Pre_N_AboveWord,
  CStringReqMemmove_Pre_N_Medium,
  CStringReqMemmove_Pre_N_BelowBlock,
  CStringReqMemmove_Pre_N_Block,
  CStringReqMemmove_Pre_N_AboveBlock,
  CStringReqMemmove_Pre_N_Long,
  CStringReqMemmove_Pre_N_NA
} CStringReqMemmove_Pre_N;

typedef enum {
  CStringReqMemmove_Pre_DestAlignment_Aligned,
  CStringReqMemmove_Pre_DestAlignment_AboveAligned,
  CStringReqMemmove_Pre_DestAlignment_Between,
  CStringReqMemmove_Pre_DestAlignment_BelowAligned,
  CStringReqMemmove_Pre_DestAlignment_NA
} CStringReqMemmove_Pre_DestAlignment;

typedef enum {
  CStringReqMemmove_Pre_SrcAlignment_Aligned,
  CStringReqMemmove_Pre_SrcAlignment_AboveAligned,
  CStringReqMemmove_Pre_SrcAlignment_Between,
  CStringReqMemmove_Pre_SrcAlignment_BelowAligned,
  CStringReqMemmove_Pre_SrcAlignment_NA
} CStringReqMemmove_Pre_SrcAlignment;

typedef enum {
  CStringReqMemmove_Pre_Overlap_DisjointBelow,
  CStringReqMemmove_Pre_Overlap_DisjointAbove,
  CStringReqMemmove_Pre_Overlap_Same,
  CStringReqMemmove_Pre_Overlap_DestBelow,
  CStringReqMemmove_Pre_Overlap_DestAbove,
  CStringReqMemmove_Pre_Overlap_NA
} CStringReqMemmove_Pre_Overlap;

typedef enum {
  CStringReqMemmove_Post_Result_Dest,
  CStringReqMemmove_Post_Result_NA
} CStringReqMemmove_Post_Result;

typedef enum {
  CStringReqMemmove_Post_Dest_Copy,
  CStringReqMemmove_Post_Dest_NA
} CStringReqMemmove_Post_Dest;

typedef enum {
  CStringReqMemmove_Post_Outside_Nop,
  CStringReqMemmove_Post_Outside_NA
} CStringReqMemmove_Post_Outside;

typedef enum {
  CStringReqMemmove_Post_Errno_Nop,
  CStringReqMemmove_Post_Errno_NA
} CStringReqMemmove_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_N_NA : 1;
  uint16_t Pre_DestAlignment_NA : 1;
  uint16_t Pre_SrcAlignment_NA : 1;
  uint16_t Pre_Overlap_NA : 1;
  uint16_t Post_Result : 1;
  uint16_t Post_Dest : 1;
  uint16_t Post_Outside : 1;
  uint16_t Post_Errno : 1;
} CStringReqMemmove_Entry;

/**
 * @brief Test context for spec:/c/string/req/memmove test case.
 */
typedef struct {
  /**
   * @brief This member provides the memory which holds both memory areas.
   */
  unsigned char buf[ 24 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member contains the content of the memory before the call.
   */
  unsigned char before[ 24 * sizeof( long ) ];

  /**
   * @brief This member contains the bytes of the source memory area before the
   *   call.
   */
  unsigned char copy[ 24 * sizeof( long ) ];

  /**
   * @brief This member specifies the offset of the destination memory area
   *   from the next lower aligned address.
   */
  size_t dest_misalign;

  /**
   * @brief This member specifies the offset of the source memory area from the
   *   next lower aligned address.
   */
  size_t src_misalign;

  /**
   * @brief This member specifies the offset of the destination memory area.
   */
  size_t dest_offset;

  /**
   * @brief This member specifies the offset of the source memory area.
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
   * @brief This member contains the return value of the memmove() call.
   */
  void *retval;

  /**
   * @brief This member contains the value of errno after the memmove() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    CStringReqMemmove_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqMemmove_Context;

static CStringReqMemmove_Context CStringReqMemmove_Instance;

static const char *const CStringReqMemmove_PreDesc_N[] = {
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

static const char *const CStringReqMemmove_PreDesc_DestAlignment[] =
  { "Aligned", "AboveAligned", "Between", "BelowAligned", "NA" };

static const char *const CStringReqMemmove_PreDesc_SrcAlignment[] =
  { "Aligned", "AboveAligned", "Between", "BelowAligned", "NA" };

static const char *const CStringReqMemmove_PreDesc_Overlap[] =
  { "DisjointBelow", "DisjointAbove", "Same", "DestBelow", "DestAbove", "NA" };

static const char *const *const CStringReqMemmove_PreDesc[] = {
  CStringReqMemmove_PreDesc_N,
  CStringReqMemmove_PreDesc_DestAlignment,
  CStringReqMemmove_PreDesc_SrcAlignment,
  CStringReqMemmove_PreDesc_Overlap,
  NULL
};

typedef CStringReqMemmove_Context Context;

static size_t Distance( size_t upper_misalign, size_t lower_misalign )
{
  size_t distance;

  distance = ( upper_misalign - lower_misalign ) % sizeof( long );

  if ( distance == 0 ) {
    distance = sizeof( long );
  }

  return distance;
}

static size_t Resolve( size_t misalign, size_t other_misalign )
{
  if ( misalign == 2 && other_misalign == sizeof( long ) - 1 ) {
    return sizeof( long ) - 2;
  }

  return misalign;
}

static void Arrange( Context *ctx )
{
  size_t i;

  for ( i = 0; i < sizeof( ctx->buf ); ++i ) {
    ctx->buf[ i ] = (unsigned char) ( i + 1 );
    ctx->before[ i ] = ctx->buf[ i ];
  }

  ctx->dest = &ctx->buf[ ctx->dest_offset ];
  ctx->src = &ctx->buf[ ctx->src_offset ];

  for ( i = 0; i < ctx->n; ++i ) {
    ctx->copy[ i ] = ctx->buf[ ctx->src_offset + i ];
  }
}

static void CheckOutside( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < sizeof( ctx->buf ); ++i ) {
    if ( i < ctx->dest_offset || i >= ctx->dest_offset + ctx->n ) {
      T_quiet_eq_u8( ctx->buf[ i ], ctx->before[ i ] );
    }
  }
}

static void CStringReqMemmove_Pre_N_Prepare(
  CStringReqMemmove_Context *ctx,
  CStringReqMemmove_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqMemmove_Pre_N_Zero: {
      /*
       * While the size specified by `n` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqMemmove_Pre_N_One: {
      /*
       * While the size specified by `n` is equal to one.
       */
      ctx->n = 1;
      break;
    }

    case CStringReqMemmove_Pre_N_Short: {
      /*
       * While the size specified by `n` is greater than one, while the size
       * specified by `n` is less than the size of type long minus one.
       */
      ctx->n = 2;
      break;
    }

    case CStringReqMemmove_Pre_N_BelowWord: {
      /*
       * While the size specified by `n` is equal to the size of type long
       * minus one.
       */
      ctx->n = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemmove_Pre_N_Word: {
      /*
       * While the size specified by `n` is equal to the size of type long.
       */
      ctx->n = sizeof( long );
      break;
    }

    case CStringReqMemmove_Pre_N_AboveWord: {
      /*
       * While the size specified by `n` is equal to the size of type long plus
       * one.
       */
      ctx->n = sizeof( long ) + 1;
      break;
    }

    case CStringReqMemmove_Pre_N_Medium: {
      /*
       * While the size specified by `n` is greater than the size of type long
       * plus one, while the size specified by `n` is less than four times the
       * size of type long minus one.
       */
      ctx->n = sizeof( long ) + 2;
      break;
    }

    case CStringReqMemmove_Pre_N_BelowBlock: {
      /*
       * While the size specified by `n` is equal to four times the size of
       * type long minus one.
       */
      ctx->n = 4 * sizeof( long ) - 1;
      break;
    }

    case CStringReqMemmove_Pre_N_Block: {
      /*
       * While the size specified by `n` is equal to four times the size of
       * type long.
       */
      ctx->n = 4 * sizeof( long );
      break;
    }

    case CStringReqMemmove_Pre_N_AboveBlock: {
      /*
       * While the size specified by `n` is equal to four times the size of
       * type long plus one.
       */
      ctx->n = 4 * sizeof( long ) + 1;
      break;
    }

    case CStringReqMemmove_Pre_N_Long: {
      /*
       * While the size specified by `n` is greater than four times the size of
       * type long plus one.
       */
      ctx->n = 9 * sizeof( long ) + 1;
      break;
    }

    case CStringReqMemmove_Pre_N_NA:
      break;
  }
}

static void CStringReqMemmove_Pre_DestAlignment_Prepare(
  CStringReqMemmove_Context          *ctx,
  CStringReqMemmove_Pre_DestAlignment state
)
{
  switch ( state ) {
    case CStringReqMemmove_Pre_DestAlignment_Aligned: {
      /*
       * While the memory area referenced by the `dest` parameter starts at an
       * address which is aligned to the size of type long.
       */
      ctx->dest_misalign = 0;
      break;
    }

    case CStringReqMemmove_Pre_DestAlignment_AboveAligned: {
      /*
       * While the memory area referenced by the `dest` parameter starts one
       * byte above an address which is aligned to the size of type long.
       */
      ctx->dest_misalign = 1;
      break;
    }

    case CStringReqMemmove_Pre_DestAlignment_Between: {
      /*
       * While the memory area referenced by the `dest` parameter starts more
       * than one byte above an address which is aligned to the size of type
       * long, while the memory area referenced by the `dest` parameter starts
       * more than one byte below the next address which is aligned to the size
       * of type long.
       */
      ctx->dest_misalign = 2;
      break;
    }

    case CStringReqMemmove_Pre_DestAlignment_BelowAligned: {
      /*
       * While the memory area referenced by the `dest` parameter starts one
       * byte below an address which is aligned to the size of type long.
       */
      ctx->dest_misalign = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemmove_Pre_DestAlignment_NA:
      break;
  }
}

static void CStringReqMemmove_Pre_SrcAlignment_Prepare(
  CStringReqMemmove_Context         *ctx,
  CStringReqMemmove_Pre_SrcAlignment state
)
{
  switch ( state ) {
    case CStringReqMemmove_Pre_SrcAlignment_Aligned: {
      /*
       * While the memory area referenced by the `src` parameter starts at an
       * address which is aligned to the size of type long.
       */
      ctx->src_misalign = 0;
      break;
    }

    case CStringReqMemmove_Pre_SrcAlignment_AboveAligned: {
      /*
       * While the memory area referenced by the `src` parameter starts one
       * byte above an address which is aligned to the size of type long.
       */
      ctx->src_misalign = 1;
      break;
    }

    case CStringReqMemmove_Pre_SrcAlignment_Between: {
      /*
       * While the memory area referenced by the `src` parameter starts more
       * than one byte above an address which is aligned to the size of type
       * long, while the memory area referenced by the `src` parameter starts
       * more than one byte below the next address which is aligned to the size
       * of type long.
       */
      ctx->src_misalign = 2;
      break;
    }

    case CStringReqMemmove_Pre_SrcAlignment_BelowAligned: {
      /*
       * While the memory area referenced by the `src` parameter starts one
       * byte below an address which is aligned to the size of type long.
       */
      ctx->src_misalign = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemmove_Pre_SrcAlignment_NA:
      break;
  }

  ctx->dest_misalign = Resolve( ctx->dest_misalign, ctx->src_misalign );
  ctx->src_misalign = Resolve( ctx->src_misalign, ctx->dest_misalign );
}

static void CStringReqMemmove_Pre_Overlap_Prepare(
  CStringReqMemmove_Context    *ctx,
  CStringReqMemmove_Pre_Overlap state
)
{
  switch ( state ) {
    case CStringReqMemmove_Pre_Overlap_DisjointBelow: {
      /*
       * While the memory areas referenced by the `dest` and `src` parameters
       * do not overlap, while the `dest` parameter is less than the `src`
       * parameter.
       */
      ctx->dest_offset = 2 * sizeof( long ) + ctx->dest_misalign;
      ctx->src_offset = 14 * sizeof( long ) + ctx->src_misalign;
      break;
    }

    case CStringReqMemmove_Pre_Overlap_DisjointAbove: {
      /*
       * While the memory areas referenced by the `dest` and `src` parameters
       * do not overlap, while the `dest` parameter is greater than the `src`
       * parameter.
       */
      ctx->src_offset = 2 * sizeof( long ) + ctx->src_misalign;
      ctx->dest_offset = 14 * sizeof( long ) + ctx->dest_misalign;
      break;
    }

    case CStringReqMemmove_Pre_Overlap_Same: {
      /*
       * While the `dest` parameter is equal to the `src` parameter.
       */
      ctx->dest_offset = 2 * sizeof( long ) + ctx->dest_misalign;
      ctx->src_offset = ctx->dest_offset;
      break;
    }

    case CStringReqMemmove_Pre_Overlap_DestBelow: {
      /*
       * While the memory areas referenced by the `dest` and `src` parameters
       * overlap, while the `dest` parameter is less than the `src` parameter.
       */
      ctx->dest_offset = 2 * sizeof( long ) + ctx->dest_misalign;
      ctx->src_offset = ctx->dest_offset +
                        Distance( ctx->src_misalign, ctx->dest_misalign );
      break;
    }

    case CStringReqMemmove_Pre_Overlap_DestAbove: {
      /*
       * While the memory areas referenced by the `dest` and `src` parameters
       * overlap, while the `dest` parameter is greater than the `src`
       * parameter.
       */
      ctx->src_offset = 2 * sizeof( long ) + ctx->src_misalign;
      ctx->dest_offset = ctx->src_offset +
                         Distance( ctx->dest_misalign, ctx->src_misalign );
      break;
    }

    case CStringReqMemmove_Pre_Overlap_NA:
      break;
  }
}

static void CStringReqMemmove_Post_Result_Check(
  CStringReqMemmove_Context    *ctx,
  CStringReqMemmove_Post_Result state
)
{
  switch ( state ) {
    case CStringReqMemmove_Post_Result_Dest: {
      /*
       * The return value of memmove() shall be the value of the `dest`
       * parameter.
       */
      T_eq_ptr( ctx->retval, ctx->dest );
      break;
    }

    case CStringReqMemmove_Post_Result_NA:
      break;
  }
}

static void CStringReqMemmove_Post_Dest_Check(
  CStringReqMemmove_Context  *ctx,
  CStringReqMemmove_Post_Dest state
)
{
  switch ( state ) {
    case CStringReqMemmove_Post_Dest_Copy: {
      /*
       * The first `n` bytes which the memory area referenced by the `src`
       * parameter held before the call shall be copied to the first `n` bytes
       * of the memory area referenced by the `dest` parameter.
       */
      T_eq_mem( ctx->dest, ctx->copy, ctx->n );
      break;
    }

    case CStringReqMemmove_Post_Dest_NA:
      break;
  }
}

static void CStringReqMemmove_Post_Outside_Check(
  CStringReqMemmove_Context     *ctx,
  CStringReqMemmove_Post_Outside state
)
{
  switch ( state ) {
    case CStringReqMemmove_Post_Outside_Nop: {
      /*
       * The bytes outside the first `n` bytes of the memory area referenced by
       * the `dest` parameter shall not be modified.
       */
      CheckOutside( ctx );
      break;
    }

    case CStringReqMemmove_Post_Outside_NA:
      break;
  }
}

static void CStringReqMemmove_Post_Errno_Check(
  CStringReqMemmove_Context   *ctx,
  CStringReqMemmove_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqMemmove_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqMemmove_Post_Errno_NA:
      break;
  }
}

static void CStringReqMemmove_Prepare( CStringReqMemmove_Context *ctx )
{
  ctx->n = 0;
  ctx->dest_misalign = 0;
  ctx->src_misalign = 0;
  ctx->dest_offset = 0;
  ctx->src_offset = 0;
  ctx->retval = NULL;
}

static void CStringReqMemmove_Action( CStringReqMemmove_Context *ctx )
{
  Arrange( ctx );
  errno = -1;
  ctx->retval = memmove( ctx->dest, ctx->src, ctx->n );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqMemmove_Entry
CStringReqMemmove_Entries[] = {
  { 0, 0, 0, 0, 0, CStringReqMemmove_Post_Result_Dest,
    CStringReqMemmove_Post_Dest_Copy, CStringReqMemmove_Post_Outside_Nop,
    CStringReqMemmove_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, CStringReqMemmove_Post_Result_NA,
    CStringReqMemmove_Post_Dest_NA, CStringReqMemmove_Post_Outside_NA,
    CStringReqMemmove_Post_Errno_NA },
  { 1, 0, 0, 0, 0, CStringReqMemmove_Post_Result_NA,
    CStringReqMemmove_Post_Dest_NA, CStringReqMemmove_Post_Outside_NA,
    CStringReqMemmove_Post_Errno_NA },
  { 0, 0, 0, 0, 0, CStringReqMemmove_Post_Result_Dest,
    CStringReqMemmove_Post_Dest_NA, CStringReqMemmove_Post_Outside_Nop,
    CStringReqMemmove_Post_Errno_Nop }
};

static const uint8_t
CStringReqMemmove_Map[] = {
  3, 3, 3, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3,
  3, 3, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3, 3,
  3, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3, 3, 1, 2, 2, 3, 3, 3,
  2, 2, 0, 0, 0, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2,
  2, 0, 0, 0, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2,
  0, 0, 0, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0,
  0, 0, 2, 2, 0, 0, 0, 2, 2, 0, 0, 1, 0, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 0, 0, 0,
  1, 2, 0, 0, 0, 0, 2, 2, 0, 0, 1, 0, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1,
  2, 0, 0, 0, 0, 2, 2, 0, 0, 1, 0, 2, 0, 0, 1, 0, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2,
  0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 2, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 0, 0, 1, 2, 0,
  0, 0, 1, 2, 0, 0, 0, 0, 2, 2, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 1, 2, 0, 0, 0, 0, 2, 2, 0, 0, 1, 0, 2, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 0, 0,
  1, 2, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CStringReqMemmove_Scope( void *arg, char *buf, size_t n )
{
  CStringReqMemmove_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqMemmove_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqMemmove_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqMemmove_Scope,
  .initial_context = &CStringReqMemmove_Instance
};

static inline CStringReqMemmove_Entry CStringReqMemmove_PopEntry(
  CStringReqMemmove_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqMemmove_Entries[ CStringReqMemmove_Map[ index ] ];
}

static void CStringReqMemmove_TestVariant( CStringReqMemmove_Context *ctx )
{
  CStringReqMemmove_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqMemmove_Pre_DestAlignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqMemmove_Pre_SrcAlignment_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqMemmove_Pre_Overlap_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqMemmove_Action( ctx );
  CStringReqMemmove_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqMemmove_Post_Dest_Check( ctx, ctx->Map.entry.Post_Dest );
  CStringReqMemmove_Post_Outside_Check( ctx, ctx->Map.entry.Post_Outside );
  CStringReqMemmove_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqMemmove( void )
 */
T_TEST_CASE_FIXTURE( CStringReqMemmove, &CStringReqMemmove_Fixture )
{
  CStringReqMemmove_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringReqMemmove_Pre_N_Zero;
    ctx->Map.pcs[ 0 ] < CStringReqMemmove_Pre_N_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqMemmove_Pre_DestAlignment_Aligned;
      ctx->Map.pcs[ 1 ] < CStringReqMemmove_Pre_DestAlignment_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringReqMemmove_Pre_SrcAlignment_Aligned;
        ctx->Map.pcs[ 2 ] < CStringReqMemmove_Pre_SrcAlignment_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CStringReqMemmove_Pre_Overlap_DisjointBelow;
          ctx->Map.pcs[ 3 ] < CStringReqMemmove_Pre_Overlap_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = CStringReqMemmove_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CStringReqMemmove_Prepare( ctx );
          CStringReqMemmove_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
