/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsBase64ReqEncode
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

#include <string.h>
#include <rtems/base64.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsBase64ReqEncode spec:/rtems/base64/req/encode
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsBase64ReqEncode_Pre_Alphabet_Base64,
  RtemsBase64ReqEncode_Pre_Alphabet_Base64url,
  RtemsBase64ReqEncode_Pre_Alphabet_NA
} RtemsBase64ReqEncode_Pre_Alphabet;

typedef enum {
  RtemsBase64ReqEncode_Pre_Source_Empty,
  RtemsBase64ReqEncode_Pre_Source_Triplets,
  RtemsBase64ReqEncode_Pre_Source_OneByteLeft,
  RtemsBase64ReqEncode_Pre_Source_TwoBytesLeft,
  RtemsBase64ReqEncode_Pre_Source_NA
} RtemsBase64ReqEncode_Pre_Source;

typedef enum {
  RtemsBase64ReqEncode_Pre_WordLength_UpToFour,
  RtemsBase64ReqEncode_Pre_WordLength_AboveFour,
  RtemsBase64ReqEncode_Pre_WordLength_NA
} RtemsBase64ReqEncode_Pre_WordLength;

typedef enum {
  RtemsBase64ReqEncode_Pre_Words_None,
  RtemsBase64ReqEncode_Pre_Words_One,
  RtemsBase64ReqEncode_Pre_Words_Several,
  RtemsBase64ReqEncode_Pre_Words_NA
} RtemsBase64ReqEncode_Pre_Words;

typedef enum {
  RtemsBase64ReqEncode_Pre_WordBreak_Empty,
  RtemsBase64ReqEncode_Pre_WordBreak_NonEmpty,
  RtemsBase64ReqEncode_Pre_WordBreak_NullPointer,
  RtemsBase64ReqEncode_Pre_WordBreak_NA
} RtemsBase64ReqEncode_Pre_WordBreak;

typedef enum {
  RtemsBase64ReqEncode_Post_Encoding_Base64,
  RtemsBase64ReqEncode_Post_Encoding_Base64url,
  RtemsBase64ReqEncode_Post_Encoding_NA
} RtemsBase64ReqEncode_Post_Encoding;

typedef enum {
  RtemsBase64ReqEncode_Post_WordBreaks_None,
  RtemsBase64ReqEncode_Post_WordBreaks_BetweenWords,
  RtemsBase64ReqEncode_Post_WordBreaks_NA
} RtemsBase64ReqEncode_Post_WordBreaks;

typedef enum {
  RtemsBase64ReqEncode_Post_Count_Output,
  RtemsBase64ReqEncode_Post_Count_NA
} RtemsBase64ReqEncode_Post_Count;

typedef enum {
  RtemsBase64ReqEncode_Post_Argument_Passed,
  RtemsBase64ReqEncode_Post_Argument_NA
} RtemsBase64ReqEncode_Post_Argument;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Alphabet_NA : 1;
  uint16_t Pre_Source_NA : 1;
  uint16_t Pre_WordLength_NA : 1;
  uint16_t Pre_Words_NA : 1;
  uint16_t Pre_WordBreak_NA : 1;
  uint16_t Post_Encoding : 2;
  uint16_t Post_WordBreaks : 2;
  uint16_t Post_Count : 1;
  uint16_t Post_Argument : 1;
} RtemsBase64ReqEncode_Entry;

#define REMAINDER_EMPTY -1

#define WORD_BREAK "|#"

/**
 * @brief Test context for spec:/rtems/base64/req/encode test case.
 */
typedef struct {
  /**
   * @brief This member references the encoder under test.
   */
  int ( *encode )(
    IO_Put_char,
    void *,
    const void *,
    size_t,
    const char *,
    int
  );

  /**
   * @brief This member references the alphabet of the encoder under test.
   */
  const char *alphabet;

  /**
   * @brief This member contains the remainder of the source length.
   */
  int remainder;

  /**
   * @brief This member contains the count of groups of the output.
   */
  size_t groups;

  /**
   * @brief This member contains the word length.
   */
  int word_length;

  /**
   * @brief This member contains the count of groups of a word.
   */
  size_t word_groups;

  /**
   * @brief This member references the word break string.
   */
  const char *word_break;

  /**
   * @brief This member contains the source buffer.
   */
  uint8_t source[ 16 ];

  /**
   * @brief This member contains the length of the source buffer.
   */
  size_t source_size;

  /**
   * @brief This member contains the output of the encoder.
   */
  char out[ 64 ];

  /**
   * @brief This member contains the count of output characters.
   */
  size_t out_count;

  /**
   * @brief This member contains the output without the word breaks.
   */
  char stripped[ 64 ];

  /**
   * @brief This member contains the expected encoding.
   */
  char expected[ 64 ];

  /**
   * @brief This member contains the length of the expected encoding.
   */
  size_t expected_count;

  /**
   * @brief This member contains the return value of the encoder.
   */
  int count;

  /**
   * @brief This member is true, if each output call received the argument.
   */
  bool argument_passed;

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
    RtemsBase64ReqEncode_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsBase64ReqEncode_Context;

static RtemsBase64ReqEncode_Context RtemsBase64ReqEncode_Instance;

static const char *const RtemsBase64ReqEncode_PreDesc_Alphabet[] =
  { "Base64", "Base64url", "NA" };

static const char *const RtemsBase64ReqEncode_PreDesc_Source[] =
  { "Empty", "Triplets", "OneByteLeft", "TwoBytesLeft", "NA" };

static const char *const RtemsBase64ReqEncode_PreDesc_WordLength[] =
  { "UpToFour", "AboveFour", "NA" };

static const char *const RtemsBase64ReqEncode_PreDesc_Words[] =
  { "None", "One", "Several", "NA" };

static const char *const RtemsBase64ReqEncode_PreDesc_WordBreak[] =
  { "Empty", "NonEmpty", "NullPointer", "NA" };

static const char *const *const RtemsBase64ReqEncode_PreDesc[] = {
  RtemsBase64ReqEncode_PreDesc_Alphabet,
  RtemsBase64ReqEncode_PreDesc_Source,
  RtemsBase64ReqEncode_PreDesc_WordLength,
  RtemsBase64ReqEncode_PreDesc_Words,
  RtemsBase64ReqEncode_PreDesc_WordBreak,
  NULL
};

typedef RtemsBase64ReqEncode_Context Context;

static const char base64_alphabet[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char base64url_alphabet[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static const uint8_t alphabet_end[] = { 0xfb, 0xff, 0xbf };

static void PutChar( int c, void *arg )
{
  Context *ctx;

  ctx = T_fixture_context();

  if ( arg != ctx ) {
    ctx->argument_passed = false;
  }

  if ( ctx->out_count < sizeof( ctx->out ) ) {
    ctx->out[ ctx->out_count ] = (char) c;
    ++ctx->out_count;
  }
}

/*
 * Encode the source as RFC 4648 section 4 states it, through a bit
 * accumulator.
 */
static size_t Encode(
  const char    *alphabet,
  char          *out,
  size_t         out_size,
  const uint8_t *in,
  size_t         in_size
)
{
  uint32_t bits;
  int      count;
  size_t   i;
  size_t   n;

  bits = 0;
  count = 0;
  n = 0;

  for ( i = 0; i < in_size; ++i ) {
    bits = ( bits << 8 ) | in[ i ];
    count += 8;

    while ( count >= 6 ) {
      count -= 6;
      T_quiet_lt_sz( n, out_size );
      out[ n ] = alphabet[ ( bits >> count ) & 0x3f ];
      ++n;
    }
  }

  if ( count > 0 ) {
    T_quiet_lt_sz( n, out_size );
    out[ n ] = alphabet[ ( bits << ( 6 - count ) ) & 0x3f ];
    ++n;
  }

  while ( n % 4 != 0 ) {
    T_quiet_lt_sz( n, out_size );
    out[ n ] = '=';
    ++n;
  }

  return n;
}

static void CheckWordBreaks( const Context *ctx )
{
  size_t word_size;
  size_t break_size;
  size_t in;
  size_t out;

  word_size = 4 * ctx->word_groups;
  break_size = ctx->word_break != NULL ? strlen( ctx->word_break ) : 0;
  in = 0;
  out = 0;

  while ( in < ctx->expected_count ) {
    size_t n;

    n = ctx->expected_count - in;

    if ( n > word_size ) {
      n = word_size;
    }

    T_quiet_le_sz( out + n, ctx->out_count );
    T_eq_nstr( &ctx->out[ out ], &ctx->expected[ in ], n );
    in += n;
    out += n;

    if ( in < ctx->expected_count ) {
      T_quiet_le_sz( out + break_size, ctx->out_count );
      T_eq_nstr( &ctx->out[ out ], ctx->word_break, break_size );
      out += break_size;
    }
  }

  T_eq_sz( out, ctx->out_count );
}

static void RtemsBase64ReqEncode_Pre_Alphabet_Prepare(
  RtemsBase64ReqEncode_Context     *ctx,
  RtemsBase64ReqEncode_Pre_Alphabet state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Pre_Alphabet_Base64: {
      /*
       * While the encoder uses the base64 alphabet.
       */
      ctx->encode = _Base64_Encode;
      ctx->alphabet = base64_alphabet;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Alphabet_Base64url: {
      /*
       * While the encoder uses the base64url alphabet.
       */
      ctx->encode = _Base64url_Encode;
      ctx->alphabet = base64url_alphabet;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Alphabet_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Pre_Source_Prepare(
  RtemsBase64ReqEncode_Context   *ctx,
  RtemsBase64ReqEncode_Pre_Source state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Pre_Source_Empty: {
      /*
       * While the source buffer is empty.
       */
      ctx->remainder = REMAINDER_EMPTY;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Source_Triplets: {
      /*
       * While the length of the source buffer is a positive multiple of three.
       */
      ctx->remainder = 0;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Source_OneByteLeft: {
      /*
       * While the length of the source buffer divided by three leaves a
       * remainder of one.
       */
      ctx->remainder = 1;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Source_TwoBytesLeft: {
      /*
       * While the length of the source buffer divided by three leaves a
       * remainder of two.
       */
      ctx->remainder = 2;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Source_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Pre_WordLength_Prepare(
  RtemsBase64ReqEncode_Context       *ctx,
  RtemsBase64ReqEncode_Pre_WordLength state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Pre_WordLength_UpToFour: {
      /*
       * While the word length is less than or equal to four.
       */
      ctx->word_length = 2;
      ctx->word_groups = 1;
      break;
    }

    case RtemsBase64ReqEncode_Pre_WordLength_AboveFour: {
      /*
       * While the word length is greater than four.
       */
      ctx->word_length = 12;
      ctx->word_groups = 2;
      break;
    }

    case RtemsBase64ReqEncode_Pre_WordLength_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Pre_Words_Prepare(
  RtemsBase64ReqEncode_Context  *ctx,
  RtemsBase64ReqEncode_Pre_Words state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Pre_Words_None: {
      /*
       * While the output consists of no base64 word.
       */
      ctx->groups = 0;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Words_One: {
      /*
       * While the output consists of one base64 word.
       */
      ctx->groups = 1;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Words_Several: {
      /*
       * While the output consists of more than one base64 word.
       */
      ctx->groups = 3;
      break;
    }

    case RtemsBase64ReqEncode_Pre_Words_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Pre_WordBreak_Prepare(
  RtemsBase64ReqEncode_Context      *ctx,
  RtemsBase64ReqEncode_Pre_WordBreak state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Pre_WordBreak_Empty: {
      /*
       * While the word break string is empty.
       */
      ctx->word_break = "";
      break;
    }

    case RtemsBase64ReqEncode_Pre_WordBreak_NonEmpty: {
      /*
       * While the word break string is not empty.
       */
      ctx->word_break = WORD_BREAK;
      break;
    }

    case RtemsBase64ReqEncode_Pre_WordBreak_NullPointer: {
      /*
       * While the word break string is NULL.
       */
      ctx->word_break = NULL;
      break;
    }

    case RtemsBase64ReqEncode_Pre_WordBreak_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Post_Encoding_Check(
  RtemsBase64ReqEncode_Context      *ctx,
  RtemsBase64ReqEncode_Post_Encoding state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Post_Encoding_Base64: {
      /*
       * The characters which the base64 encoder outputs, apart from the
       * characters of the word breaks, shall be the base64 encoding with
       * padding of the source buffer.
       */
      size_t i;
      size_t n;

      n = 0;

      for ( i = 0; i < ctx->out_count; ++i ) {
        if ( strchr( WORD_BREAK, ctx->out[ i ] ) == NULL ) {
          T_quiet_lt_sz( n, sizeof( ctx->stripped ) );
          ctx->stripped[ n ] = ctx->out[ i ];
          ++n;
        }
      }

      T_eq_sz( n, ctx->expected_count );
      T_eq_nstr( ctx->stripped, ctx->expected, ctx->expected_count );
      break;
    }

    case RtemsBase64ReqEncode_Post_Encoding_Base64url: {
      /*
       * The characters which the base64 encoder outputs, apart from the
       * characters of the word breaks, shall be the base64url encoding with
       * padding of the source buffer.
       */
      size_t i;
      size_t n;

      n = 0;

      for ( i = 0; i < ctx->out_count; ++i ) {
        if ( strchr( WORD_BREAK, ctx->out[ i ] ) == NULL ) {
          T_quiet_lt_sz( n, sizeof( ctx->stripped ) );
          ctx->stripped[ n ] = ctx->out[ i ];
          ++n;
        }
      }

      T_eq_sz( n, ctx->expected_count );
      T_eq_nstr( ctx->stripped, ctx->expected, ctx->expected_count );
      break;
    }

    case RtemsBase64ReqEncode_Post_Encoding_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Post_WordBreaks_Check(
  RtemsBase64ReqEncode_Context        *ctx,
  RtemsBase64ReqEncode_Post_WordBreaks state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Post_WordBreaks_None: {
      /*
       * The base64 encoder shall output no word break.
       */
      T_eq_sz( ctx->out_count, ctx->expected_count );
      T_eq_nstr( ctx->out, ctx->expected, ctx->expected_count );
      break;
    }

    case RtemsBase64ReqEncode_Post_WordBreaks_BetweenWords: {
      /*
       * The base64 encoder shall output the word break string after each
       * base64 word which more output follows.
       */
      CheckWordBreaks( ctx );
      break;
    }

    case RtemsBase64ReqEncode_Post_WordBreaks_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Post_Count_Check(
  RtemsBase64ReqEncode_Context   *ctx,
  RtemsBase64ReqEncode_Post_Count state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Post_Count_Output: {
      /*
       * The base64 encoder shall return the count of characters which it
       * output, including the characters of the word breaks.
       */
      T_eq_int( ctx->count, (int) ctx->out_count );
      break;
    }

    case RtemsBase64ReqEncode_Post_Count_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Post_Argument_Check(
  RtemsBase64ReqEncode_Context      *ctx,
  RtemsBase64ReqEncode_Post_Argument state
)
{
  switch ( state ) {
    case RtemsBase64ReqEncode_Post_Argument_Passed: {
      /*
       * The base64 encoder shall pass the argument to each call of the put
       * character function.
       */
      T_true( ctx->argument_passed );
      break;
    }

    case RtemsBase64ReqEncode_Post_Argument_NA:
      break;
  }
}

static void RtemsBase64ReqEncode_Action( RtemsBase64ReqEncode_Context *ctx )
{
  size_t i;

  if ( ctx->Map.entry.Skip ) {
    return;
  }

  if ( ctx->remainder == REMAINDER_EMPTY ) {
    ctx->source_size = 0;
  } else if ( ctx->remainder == 0 ) {
    ctx->source_size = 3 * ctx->groups;
  } else {
    ctx->source_size = 3 * ( ctx->groups - 1 ) + (size_t) ctx->remainder;
  }

  T_quiet_le_sz( ctx->source_size, sizeof( ctx->source ) );

  for ( i = 0; i < ctx->source_size; ++i ) {
    ctx->source[ i ] = (uint8_t) ( 37 * i + 11 );
  }

  /* These bytes give the indices 62 and 63 of the alphabet. */
  for ( i = 0; i < ctx->source_size && i < sizeof( alphabet_end ); ++i ) {
    ctx->source[ i ] = alphabet_end[ i ];
  }

  ctx->expected_count = Encode(
    ctx->alphabet,
    ctx->expected,
    sizeof( ctx->expected ),
    ctx->source,
    ctx->source_size
  );
  ctx->out_count = 0;
  ctx->argument_passed = true;
  ctx->count = ( *ctx->encode )(
    PutChar,
    ctx,
    ctx->source,
    ctx->source_size,
    ctx->word_break,
    ctx->word_length
  );
}

/* clang-format off */

static const RtemsBase64ReqEncode_Entry
RtemsBase64ReqEncode_Entries[] = {
  { 1, 0, 0, 0, 0, 0, RtemsBase64ReqEncode_Post_Encoding_NA,
    RtemsBase64ReqEncode_Post_WordBreaks_NA,
    RtemsBase64ReqEncode_Post_Count_NA, RtemsBase64ReqEncode_Post_Argument_NA },
  { 0, 0, 0, 0, 0, 0, RtemsBase64ReqEncode_Post_Encoding_Base64,
    RtemsBase64ReqEncode_Post_WordBreaks_None,
    RtemsBase64ReqEncode_Post_Count_Output,
    RtemsBase64ReqEncode_Post_Argument_Passed },
  { 0, 0, 0, 0, 0, 0, RtemsBase64ReqEncode_Post_Encoding_Base64url,
    RtemsBase64ReqEncode_Post_WordBreaks_None,
    RtemsBase64ReqEncode_Post_Count_Output,
    RtemsBase64ReqEncode_Post_Argument_Passed },
  { 0, 0, 0, 0, 0, 0, RtemsBase64ReqEncode_Post_Encoding_Base64,
    RtemsBase64ReqEncode_Post_WordBreaks_BetweenWords,
    RtemsBase64ReqEncode_Post_Count_Output,
    RtemsBase64ReqEncode_Post_Argument_Passed },
  { 1, 0, 0, 0, 0, 0, RtemsBase64ReqEncode_Post_Encoding_NA,
    RtemsBase64ReqEncode_Post_WordBreaks_NA,
    RtemsBase64ReqEncode_Post_Count_NA, RtemsBase64ReqEncode_Post_Argument_NA },
  { 0, 0, 0, 0, 0, 0, RtemsBase64ReqEncode_Post_Encoding_Base64url,
    RtemsBase64ReqEncode_Post_WordBreaks_BetweenWords,
    RtemsBase64ReqEncode_Post_Count_Output,
    RtemsBase64ReqEncode_Post_Argument_Passed }
};

static const uint8_t
RtemsBase64ReqEncode_Map[] = {
  1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3,
  4, 0, 0, 0, 1, 1, 1, 3, 3, 4, 0, 0, 0, 1, 1, 1, 3, 3, 4, 0, 0, 0, 1, 1, 1, 3,
  3, 4, 0, 0, 0, 1, 1, 1, 3, 3, 4, 0, 0, 0, 1, 1, 1, 3, 3, 4, 2, 2, 2, 0, 0, 0,
  0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 5, 5, 4, 0, 0, 0, 2, 2,
  2, 5, 5, 4, 0, 0, 0, 2, 2, 2, 5, 5, 4, 0, 0, 0, 2, 2, 2, 5, 5, 4, 0, 0, 0, 2,
  2, 2, 5, 5, 4, 0, 0, 0, 2, 2, 2, 5, 5, 4
};

/* clang-format on */

static size_t RtemsBase64ReqEncode_Scope( void *arg, char *buf, size_t n )
{
  RtemsBase64ReqEncode_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsBase64ReqEncode_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsBase64ReqEncode_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsBase64ReqEncode_Scope,
  .initial_context = &RtemsBase64ReqEncode_Instance
};

static inline RtemsBase64ReqEncode_Entry RtemsBase64ReqEncode_PopEntry(
  RtemsBase64ReqEncode_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsBase64ReqEncode_Entries[ RtemsBase64ReqEncode_Map[ index ] ];
}

static void RtemsBase64ReqEncode_TestVariant(
  RtemsBase64ReqEncode_Context *ctx
)
{
  RtemsBase64ReqEncode_Pre_Alphabet_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsBase64ReqEncode_Pre_Source_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsBase64ReqEncode_Pre_WordLength_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsBase64ReqEncode_Pre_Words_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsBase64ReqEncode_Pre_WordBreak_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsBase64ReqEncode_Action( ctx );
  RtemsBase64ReqEncode_Post_Encoding_Check(
    ctx,
    ctx->Map.entry.Post_Encoding
  );
  RtemsBase64ReqEncode_Post_WordBreaks_Check(
    ctx,
    ctx->Map.entry.Post_WordBreaks
  );
  RtemsBase64ReqEncode_Post_Count_Check( ctx, ctx->Map.entry.Post_Count );
  RtemsBase64ReqEncode_Post_Argument_Check(
    ctx,
    ctx->Map.entry.Post_Argument
  );
}

/**
 * @fn void T_case_body_RtemsBase64ReqEncode( void )
 */
T_TEST_CASE_FIXTURE( RtemsBase64ReqEncode, &RtemsBase64ReqEncode_Fixture )
{
  RtemsBase64ReqEncode_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsBase64ReqEncode_Pre_Alphabet_Base64;
    ctx->Map.pcs[ 0 ] < RtemsBase64ReqEncode_Pre_Alphabet_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsBase64ReqEncode_Pre_Source_Empty;
      ctx->Map.pcs[ 1 ] < RtemsBase64ReqEncode_Pre_Source_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsBase64ReqEncode_Pre_WordLength_UpToFour;
        ctx->Map.pcs[ 2 ] < RtemsBase64ReqEncode_Pre_WordLength_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsBase64ReqEncode_Pre_Words_None;
          ctx->Map.pcs[ 3 ] < RtemsBase64ReqEncode_Pre_Words_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsBase64ReqEncode_Pre_WordBreak_Empty;
            ctx->Map.pcs[ 4 ] < RtemsBase64ReqEncode_Pre_WordBreak_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = RtemsBase64ReqEncode_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            RtemsBase64ReqEncode_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
