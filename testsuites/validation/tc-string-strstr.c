/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrstr
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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqStrstr spec:/c/string/req/strstr
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * Each variant builds a pair of strings and calls strstr() for it. For a
 * needle of up to three characters, and for a haystack of at most the needle
 * length plus one, a table provides the strings. The entries for a needle of
 * up to three characters come from an enumeration of all strings over a few
 * letters. A recipe table builds the strings for longer needles from one of
 * several needle shapes. A longer haystack starts with a fill of 2560
 * characters which do not occur in the needle. For a needle of at least five
 * characters, a longer haystack is built for three needle shapes, which lead
 * the search through both of its paths with left halves of different lengths.
 * For a needle with a border, an inner near match before an occurrence starts
 * one period of the needle before it. The test compares each result with a
 * search of its own.
 *
 * @{
 */

typedef enum {
  CStringReqStrstr_Pre_Length_Empty,
  CStringReqStrstr_Pre_Length_One,
  CStringReqStrstr_Pre_Length_Two,
  CStringReqStrstr_Pre_Length_Three,
  CStringReqStrstr_Pre_Length_Four,
  CStringReqStrstr_Pre_Length_Five,
  CStringReqStrstr_Pre_Length_Long,
  CStringReqStrstr_Pre_Length_NA
} CStringReqStrstr_Pre_Length;

typedef enum {
  CStringReqStrstr_Pre_Haystack_Shorter,
  CStringReqStrstr_Pre_Haystack_BelowNeedle,
  CStringReqStrstr_Pre_Haystack_EqualNeedle,
  CStringReqStrstr_Pre_Haystack_AboveNeedle,
  CStringReqStrstr_Pre_Haystack_Longer,
  CStringReqStrstr_Pre_Haystack_NA
} CStringReqStrstr_Pre_Haystack;

typedef enum {
  CStringReqStrstr_Pre_Border_Yes,
  CStringReqStrstr_Pre_Border_No,
  CStringReqStrstr_Pre_Border_NA
} CStringReqStrstr_Pre_Border;

typedef enum {
  CStringReqStrstr_Pre_Occurrences_None,
  CStringReqStrstr_Pre_Occurrences_One,
  CStringReqStrstr_Pre_Occurrences_Several,
  CStringReqStrstr_Pre_Occurrences_NA
} CStringReqStrstr_Pre_Occurrences;

typedef enum {
  CStringReqStrstr_Pre_Position_Start,
  CStringReqStrstr_Pre_Position_Between,
  CStringReqStrstr_Pre_Position_End,
  CStringReqStrstr_Pre_Position_NA
} CStringReqStrstr_Pre_Position;

typedef enum {
  CStringReqStrstr_Pre_Overlap_Yes,
  CStringReqStrstr_Pre_Overlap_No,
  CStringReqStrstr_Pre_Overlap_NA
} CStringReqStrstr_Pre_Overlap;

typedef enum {
  CStringReqStrstr_Pre_Partial_No,
  CStringReqStrstr_Pre_Partial_Inside,
  CStringReqStrstr_Pre_Partial_Tail,
  CStringReqStrstr_Pre_Partial_NA
} CStringReqStrstr_Pre_Partial;

typedef enum {
  CStringReqStrstr_Pre_NearMatch_None,
  CStringReqStrstr_Pre_NearMatch_First,
  CStringReqStrstr_Pre_NearMatch_Inner,
  CStringReqStrstr_Pre_NearMatch_Last,
  CStringReqStrstr_Pre_NearMatch_NA
} CStringReqStrstr_Pre_NearMatch;

typedef enum {
  CStringReqStrstr_Pre_HighByte_Yes,
  CStringReqStrstr_Pre_HighByte_No,
  CStringReqStrstr_Pre_HighByte_NA
} CStringReqStrstr_Pre_HighByte;

typedef enum {
  CStringReqStrstr_Post_Result_S1,
  CStringReqStrstr_Post_Result_Occurrence,
  CStringReqStrstr_Post_Result_Null,
  CStringReqStrstr_Post_Result_NA
} CStringReqStrstr_Post_Result;

typedef enum {
  CStringReqStrstr_Post_S1_Nop,
  CStringReqStrstr_Post_S1_NA
} CStringReqStrstr_Post_S1;

typedef enum {
  CStringReqStrstr_Post_S2_Nop,
  CStringReqStrstr_Post_S2_NA
} CStringReqStrstr_Post_S2;

typedef enum {
  CStringReqStrstr_Post_Errno_Nop,
  CStringReqStrstr_Post_Errno_NA
} CStringReqStrstr_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Length_NA : 1;
  uint16_t Pre_Haystack_NA : 1;
  uint16_t Pre_Border_NA : 1;
  uint16_t Pre_Occurrences_NA : 1;
  uint16_t Pre_Position_NA : 1;
  uint16_t Pre_Overlap_NA : 1;
  uint16_t Pre_Partial_NA : 1;
  uint16_t Pre_NearMatch_NA : 1;
  uint16_t Pre_HighByte_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_S1 : 1;
  uint16_t Post_S2 : 1;
  uint16_t Post_Errno : 1;
} CStringReqStrstr_Entry;

typedef enum {
  OCCURRENCES_NONE,
  OCCURRENCES_ONE,
  OCCURRENCES_SEVERAL
} Occurrences;

typedef enum {
  POSITION_NONE,
  POSITION_START,
  POSITION_BETWEEN,
  POSITION_END
} Position;

typedef enum { PARTIAL_NO, PARTIAL_INSIDE, PARTIAL_TAIL } Partial;

typedef enum {
  NEAR_MATCH_NONE,
  NEAR_MATCH_FIRST,
  NEAR_MATCH_INNER,
  NEAR_MATCH_LAST
} NearMatch;

typedef enum {
  HAYSTACK_SHORTER,
  HAYSTACK_BELOW,
  HAYSTACK_EQUAL,
  HAYSTACK_ABOVE,
  HAYSTACK_LONGER
} Haystack;

/**
 * @brief Test context for spec:/c/string/req/strstr test case.
 */
typedef struct {
  /**
   * @brief This member specifies the length of the string referenced by the
   *   `s2` parameter.
   */
  size_t length;

  /**
   * @brief This member specifies the length of the string referenced by the
   *   `s1` parameter relative to the length of the string referenced by the
   *   `s2` parameter.
   */
  Haystack haystack;

  /**
   * @brief This member specifies the shape of the string referenced by the
   *   `s2` parameter for a longer haystack.
   */
  size_t shape;

  /**
   * @brief This member contains the period of the string referenced by the
   *   `s2` parameter for a longer haystack.
   */
  size_t period;

  /**
   * @brief This member is true, if the string referenced by the `s2` parameter
   *   has a border.
   */
  bool border;

  /**
   * @brief This member specifies the number of occurrences.
   */
  Occurrences occurrences;

  /**
   * @brief This member specifies the position of the first occurrence.
   */
  Position position;

  /**
   * @brief This member is true, if two occurrences share a character.
   */
  bool overlap;

  /**
   * @brief This member specifies the partial match.
   */
  Partial partial;

  /**
   * @brief This member specifies the near match.
   */
  NearMatch near_match;

  /**
   * @brief This member is true, if the string referenced by the `s2` parameter
   *   holds a character with a value greater than 127.
   */
  bool high;

  /**
   * @brief This member contains the length of the string referenced by the
   *   `s1` parameter.
   */
  size_t size;

  /**
   * @brief This member contains the count of strstr() calls.
   */
  size_t count;

  /**
   * @brief This member contains the expected return value of each strstr()
   *   call.
   */
  const char *expected[ 3 ];

  /**
   * @brief This member contains the return value of each strstr() call.
   */
  char *retval[ 3 ];

  /**
   * @brief This member contains the comparison of the string referenced by the
   *   `s1` parameter before and after each strstr() call.
   */
  int s1_cmp[ 3 ];

  /**
   * @brief This member contains the comparison of the string referenced by the
   *   `s2` parameter before and after each strstr() call.
   */
  int s2_cmp[ 3 ];

  /**
   * @brief This member provides the string referenced by the `s1` parameter.
   */
  char s1_buf[ 8192 ];

  /**
   * @brief This member provides the string referenced by the `s2` parameter.
   */
  char s2_buf[ 304 ];

  /**
   * @brief This member contains the string referenced by the `s1` parameter
   *   before the strstr() call.
   */
  char s1_before[ 8192 ];

  /**
   * @brief This member contains the string referenced by the `s2` parameter
   *   before the strstr() call.
   */
  char s2_before[ 304 ];

  /**
   * @brief This member contains the value of errno after each strstr() call.
   */
  int errno_value[ 3 ];

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 9 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 9 ];

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
    CStringReqStrstr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrstr_Context;

static CStringReqStrstr_Context CStringReqStrstr_Instance;

static const char *const CStringReqStrstr_PreDesc_Length[] =
  { "Empty", "One", "Two", "Three", "Four", "Five", "Long", "NA" };

static const char *const CStringReqStrstr_PreDesc_Haystack[] =
  { "Shorter", "BelowNeedle", "EqualNeedle", "AboveNeedle", "Longer", "NA" };

static const char *const CStringReqStrstr_PreDesc_Border[] =
  { "Yes", "No", "NA" };

static const char *const CStringReqStrstr_PreDesc_Occurrences[] =
  { "None", "One", "Several", "NA" };

static const char *const CStringReqStrstr_PreDesc_Position[] =
  { "Start", "Between", "End", "NA" };

static const char *const CStringReqStrstr_PreDesc_Overlap[] =
  { "Yes", "No", "NA" };

static const char *const CStringReqStrstr_PreDesc_Partial[] =
  { "No", "Inside", "Tail", "NA" };

static const char *const CStringReqStrstr_PreDesc_NearMatch[] =
  { "None", "First", "Inner", "Last", "NA" };

static const char *const CStringReqStrstr_PreDesc_HighByte[] =
  { "Yes", "No", "NA" };

static const char *const *const CStringReqStrstr_PreDesc[] = {
  CStringReqStrstr_PreDesc_Length,
  CStringReqStrstr_PreDesc_Haystack,
  CStringReqStrstr_PreDesc_Border,
  CStringReqStrstr_PreDesc_Occurrences,
  CStringReqStrstr_PreDesc_Position,
  CStringReqStrstr_PreDesc_Overlap,
  CStringReqStrstr_PreDesc_Partial,
  CStringReqStrstr_PreDesc_NearMatch,
  CStringReqStrstr_PreDesc_HighByte,
  NULL
};

typedef CStringReqStrstr_Context Context;

#define FILLER 'c'

#define HIGH 0xb0

typedef struct {
  unsigned char length;
  unsigned char haystack;
  unsigned char border;
  unsigned char occurrences;
  unsigned char position;
  unsigned char overlap;
  unsigned char partial;
  unsigned char near_match;
  const char   *s2;
  const char   *s1;
} Literal;

typedef struct {
  unsigned char haystack;
  unsigned char border;
  unsigned char occurrences;
  unsigned char position;
  unsigned char overlap;
  unsigned char partial;
  unsigned char near_match;
  unsigned char family;
  unsigned char base;
  unsigned char lead;
  unsigned char trail;
  unsigned char at[ 2 ];
  unsigned char with[ 2 ];
} Recipe;

static const Literal literals[] = {
  { 0, 2, 0, 1, 1, 0, 0, 0, "", "" },
  { 0, 3, 0, 2, 1, 0, 0, 0, "", "c" },
  { 0, 4, 0, 2, 1, 0, 0, 0, "", "cc" },
  { 1, 1, 0, 0, 0, 0, 0, 0, "d", "" },
  { 1, 2, 0, 0, 0, 0, 0, 1, "d", "c" },
  { 1, 2, 0, 1, 1, 0, 0, 0, "d", "d" },
  { 1, 3, 0, 0, 0, 0, 0, 1, "d", "cc" },
  { 1, 3, 0, 1, 1, 0, 0, 0, "d", "dc" },
  { 1, 3, 0, 1, 3, 0, 0, 1, "d", "cd" },
  { 1, 3, 0, 2, 1, 0, 0, 0, "d", "dd" },
  { 1, 4, 0, 0, 0, 0, 0, 1, "a", "ccc" },
  { 1, 4, 0, 1, 1, 0, 0, 0, "a", "acc" },
  { 1, 4, 0, 1, 2, 0, 0, 1, "a", "cac" },
  { 1, 4, 0, 1, 3, 0, 0, 1, "a", "cca" },
  { 1, 4, 0, 2, 1, 0, 0, 0, "a", "aaa" },
  { 1, 4, 0, 2, 2, 0, 0, 1, "a", "caa" },
  { 2, 0, 0, 0, 0, 0, 0, 0, "de", "" },
  { 2, 0, 1, 0, 0, 0, 0, 0, "dd", "" },
  { 2, 1, 0, 0, 0, 0, 0, 0, "de", "e" },
  { 2, 1, 0, 0, 0, 0, 2, 0, "de", "d" },
  { 2, 1, 1, 0, 0, 0, 0, 0, "dd", "c" },
  { 2, 1, 1, 0, 0, 0, 2, 0, "dd", "d" },
  { 2, 2, 0, 0, 0, 0, 0, 0, "de", "ec" },
  { 2, 2, 0, 0, 0, 0, 0, 1, "de", "ee" },
  { 2, 2, 0, 0, 0, 0, 1, 3, "de", "dc" },
  { 2, 2, 0, 0, 0, 0, 2, 0, "de", "ed" },
  { 2, 2, 0, 0, 0, 0, 2, 3, "de", "dd" },
  { 2, 2, 0, 1, 1, 0, 0, 0, "de", "de" },
  { 2, 2, 1, 0, 0, 0, 0, 0, "dd", "cc" },
  { 2, 2, 1, 0, 0, 0, 1, 3, "dd", "dc" },
  { 2, 2, 1, 0, 0, 0, 2, 1, "dd", "cd" },
  { 2, 2, 1, 1, 1, 0, 0, 0, "dd", "dd" },
  { 2, 3, 0, 0, 0, 0, 0, 0, "de", "ecc" },
  { 2, 3, 0, 0, 0, 0, 0, 1, "de", "eee" },
  { 2, 3, 0, 0, 0, 0, 1, 3, "de", "dce" },
  { 2, 3, 0, 0, 0, 0, 2, 0, "de", "ecd" },
  { 2, 3, 0, 0, 0, 0, 2, 1, "de", "eed" },
  { 2, 3, 0, 0, 0, 0, 2, 3, "de", "ddd" },
  { 2, 3, 0, 1, 1, 0, 0, 0, "de", "dee" },
  { 2, 3, 0, 1, 1, 0, 2, 0, "de", "ded" },
  { 2, 3, 0, 1, 3, 0, 0, 0, "de", "ede" },
  { 2, 3, 0, 1, 3, 0, 1, 3, "de", "dde" },
  { 2, 3, 1, 0, 0, 0, 0, 0, "dd", "ccc" },
  { 2, 3, 1, 0, 0, 0, 1, 1, "dd", "cdc" },
  { 2, 3, 1, 0, 0, 0, 1, 3, "dd", "dcc" },
  { 2, 3, 1, 0, 0, 0, 2, 1, "dd", "ccd" },
  { 2, 3, 1, 0, 0, 0, 2, 3, "dd", "dcd" },
  { 2, 3, 1, 1, 1, 0, 0, 0, "dd", "ddc" },
  { 2, 3, 1, 1, 3, 0, 0, 1, "dd", "cdd" },
  { 2, 3, 1, 2, 1, 1, 0, 0, "dd", "ddd" },
  { 2, 4, 0, 0, 0, 0, 0, 0, "ab", "bccc" },
  { 2, 4, 0, 0, 0, 0, 0, 1, "ab", "bbbb" },
  { 2, 4, 0, 0, 0, 0, 1, 1, "ab", "bbac" },
  { 2, 4, 0, 0, 0, 0, 1, 3, "ab", "acbb" },
  { 2, 4, 0, 0, 0, 0, 2, 0, "ab", "bcca" },
  { 2, 4, 0, 0, 0, 0, 2, 1, "ab", "bbaa" },
  { 2, 4, 0, 0, 0, 0, 2, 3, "ab", "aaaa" },
  { 2, 4, 0, 1, 1, 0, 0, 0, "ab", "abbb" },
  { 2, 4, 0, 1, 1, 0, 2, 0, "ab", "abaa" },
  { 2, 4, 0, 1, 2, 0, 0, 0, "ab", "babb" },
  { 2, 4, 0, 1, 2, 0, 0, 1, "ab", "bbabb" },
  { 2, 4, 0, 1, 2, 0, 1, 1, "ab", "bbaabb" },
  { 2, 4, 0, 1, 2, 0, 1, 3, "ab", "aabb" },
  { 2, 4, 0, 1, 2, 0, 2, 0, "ab", "baba" },
  { 2, 4, 0, 1, 2, 0, 2, 1, "ab", "bbaba" },
  { 2, 4, 0, 1, 2, 0, 2, 3, "ab", "aaba" },
  { 2, 4, 0, 1, 3, 0, 0, 0, "ab", "bcab" },
  { 2, 4, 0, 1, 3, 0, 0, 1, "ab", "bbab" },
  { 2, 4, 0, 1, 3, 0, 1, 1, "ab", "bbaab" },
  { 2, 4, 0, 1, 3, 0, 1, 3, "ab", "aaab" },
  { 2, 4, 0, 2, 1, 0, 0, 0, "ab", "abab" },
  { 2, 4, 0, 2, 1, 0, 2, 0, "ab", "ababa" },
  { 2, 4, 0, 2, 2, 0, 0, 0, "ab", "babab" },
  { 2, 4, 0, 2, 2, 0, 0, 1, "ab", "bbabab" },
  { 2, 4, 0, 2, 2, 0, 1, 1, "ab", "bbaabab" },
  { 2, 4, 0, 2, 2, 0, 1, 3, "ab", "aabab" },
  { 2, 4, 0, 2, 2, 0, 2, 0, "ab", "bababa" },
  { 2, 4, 0, 2, 2, 0, 2, 1, "ab", "bbababa" },
  { 2, 4, 0, 2, 2, 0, 2, 3, "ab", "aababa" },
  { 2, 4, 1, 0, 0, 0, 0, 0, "aa", "cccc" },
  { 2, 4, 1, 0, 0, 0, 1, 1, "aa", "ccac" },
  { 2, 4, 1, 0, 0, 0, 1, 3, "aa", "acac" },
  { 2, 4, 1, 0, 0, 0, 2, 1, "aa", "caca" },
  { 2, 4, 1, 0, 0, 0, 2, 3, "aa", "acca" },
  { 2, 4, 1, 1, 1, 0, 0, 0, "aa", "aacc" },
  { 2, 4, 1, 1, 1, 0, 2, 0, "aa", "aaca" },
  { 2, 4, 1, 1, 2, 0, 0, 1, "aa", "caac" },
  { 2, 4, 1, 1, 2, 0, 1, 1, "aa", "cacaac" },
  { 2, 4, 1, 1, 2, 0, 1, 3, "aa", "acaac" },
  { 2, 4, 1, 1, 2, 0, 2, 1, "aa", "caaca" },
  { 2, 4, 1, 1, 2, 0, 2, 3, "aa", "acaaca" },
  { 2, 4, 1, 1, 3, 0, 0, 1, "aa", "ccaa" },
  { 2, 4, 1, 1, 3, 0, 1, 1, "aa", "cacaa" },
  { 2, 4, 1, 1, 3, 0, 1, 3, "aa", "acaa" },
  { 2, 4, 1, 2, 1, 0, 0, 0, "aa", "aacaa" },
  { 2, 4, 1, 2, 1, 0, 2, 0, "aa", "aacaaca" },
  { 2, 4, 1, 2, 1, 1, 0, 0, "aa", "aaaa" },
  { 2, 4, 1, 2, 1, 1, 2, 0, "aa", "aaaca" },
  { 2, 4, 1, 2, 2, 0, 0, 1, "aa", "caacaa" },
  { 2, 4, 1, 2, 2, 0, 1, 1, "aa", "cacaacaa" },
  { 2, 4, 1, 2, 2, 0, 1, 3, "aa", "acaacaa" },
  { 2, 4, 1, 2, 2, 0, 2, 1, "aa", "caacaaca" },
  { 2, 4, 1, 2, 2, 0, 2, 3, "aa", "acaacaaca" },
  { 2, 4, 1, 2, 2, 1, 0, 1, "aa", "caaa" },
  { 2, 4, 1, 2, 2, 1, 1, 1, "aa", "cacaaa" },
  { 2, 4, 1, 2, 2, 1, 1, 3, "aa", "acaaa" },
  { 2, 4, 1, 2, 2, 1, 2, 1, "aa", "caaaca" },
  { 2, 4, 1, 2, 2, 1, 2, 3, "aa", "acaaaca" },
  { 3, 0, 0, 0, 0, 0, 0, 0, "dde", "" },
  { 3, 0, 0, 0, 0, 0, 2, 0, "dde", "d" },
  { 3, 0, 1, 0, 0, 0, 0, 0, "ddd", "" },
  { 3, 0, 1, 0, 0, 0, 2, 0, "ddd", "d" },
  { 3, 1, 0, 0, 0, 0, 0, 0, "dde", "ee" },
  { 3, 1, 0, 0, 0, 0, 1, 0, "dde", "de" },
  { 3, 1, 0, 0, 0, 0, 2, 0, "dde", "dd" },
  { 3, 1, 1, 0, 0, 0, 0, 0, "ddd", "cc" },
  { 3, 1, 1, 0, 0, 0, 1, 0, "ddd", "dc" },
  { 3, 1, 1, 0, 0, 0, 2, 0, "ddd", "dd" },
  { 3, 2, 0, 0, 0, 0, 0, 0, "dde", "eee" },
  { 3, 2, 0, 0, 0, 0, 0, 1, "dee", "eee" },
  { 3, 2, 0, 0, 0, 0, 1, 0, "dde", "edc" },
  { 3, 2, 0, 0, 0, 0, 1, 1, "dde", "ede" },
  { 3, 2, 0, 0, 0, 0, 1, 2, "dde", "dee" },
  { 3, 2, 0, 0, 0, 0, 1, 3, "dde", "ddc" },
  { 3, 2, 0, 0, 0, 0, 2, 0, "dde", "edd" },
  { 3, 2, 0, 0, 0, 0, 2, 2, "dee", "dde" },
  { 3, 2, 0, 0, 0, 0, 2, 3, "dde", "ddd" },
  { 3, 2, 0, 1, 1, 0, 0, 0, "dde", "dde" },
  { 3, 2, 1, 0, 0, 0, 0, 0, "ddd", "ccc" },
  { 3, 2, 1, 0, 0, 0, 1, 0, "ddd", "cdc" },
  { 3, 2, 1, 0, 0, 0, 1, 3, "ddd", "ddc" },
  { 3, 2, 1, 0, 0, 0, 2, 0, "ddd", "ccd" },
  { 3, 2, 1, 0, 0, 0, 2, 1, "ddd", "cdd" },
  { 3, 2, 1, 0, 0, 0, 2, 2, "ddd", "dcd" },
  { 3, 2, 1, 1, 1, 0, 0, 0, "ddd", "ddd" },
  { 3, 3, 0, 0, 0, 0, 0, 0, "dde", "eeee" },
  { 3, 3, 0, 0, 0, 0, 0, 1, "dee", "eeee" },
  { 3, 3, 0, 0, 0, 0, 1, 0, "dde", "dece" },
  { 3, 3, 0, 0, 0, 0, 1, 1, "dde", "dede" },
  { 3, 3, 0, 0, 0, 0, 1, 2, "dde", "deee" },
  { 3, 3, 0, 0, 0, 0, 1, 3, "dde", "ddce" },
  { 3, 3, 0, 0, 0, 0, 2, 0, "dde", "dedd" },
  { 3, 3, 0, 0, 0, 0, 2, 1, "dde", "eded" },
  { 3, 3, 0, 0, 0, 0, 2, 2, "dde", "deed" },
  { 3, 3, 0, 0, 0, 0, 2, 3, "dde", "dddd" },
  { 3, 3, 0, 1, 1, 0, 0, 0, "dde", "ddee" },
  { 3, 3, 0, 1, 1, 0, 2, 0, "dde", "dded" },
  { 3, 3, 0, 1, 3, 0, 0, 0, "dde", "edde" },
  { 3, 3, 0, 1, 3, 0, 1, 0, "def", "ddef" },
  { 3, 3, 0, 1, 3, 0, 1, 2, "dee", "ddee" },
  { 3, 3, 0, 1, 3, 0, 1, 3, "dde", "ddde" },
  { 3, 3, 1, 0, 0, 0, 0, 0, "ddd", "cccc" },
  { 3, 3, 1, 0, 0, 0, 1, 0, "ddd", "ccdc" },
  { 3, 3, 1, 0, 0, 0, 1, 1, "ddd", "cddc" },
  { 3, 3, 1, 0, 0, 0, 1, 2, "ddd", "dcdc" },
  { 3, 3, 1, 0, 0, 0, 1, 3, "ddd", "ddcc" },
  { 3, 3, 1, 0, 0, 0, 2, 0, "ddd", "dccd" },
  { 3, 3, 1, 0, 0, 0, 2, 1, "ddd", "ccdd" },
  { 3, 3, 1, 0, 0, 0, 2, 2, "ddd", "dcdd" },
  { 3, 3, 1, 0, 0, 0, 2, 3, "ddd", "ddcd" },
  { 3, 3, 1, 1, 1, 0, 0, 0, "ddd", "dddc" },
  { 3, 3, 1, 1, 1, 0, 2, 0, "ded", "dedd" },
  { 3, 3, 1, 1, 3, 0, 0, 0, "ded", "eded" },
  { 3, 3, 1, 1, 3, 0, 0, 1, "ddd", "cddd" },
  { 3, 3, 1, 1, 3, 0, 1, 0, "ded", "dded" },
  { 3, 3, 1, 2, 1, 1, 0, 0, "ddd", "dddd" },
};

static const Recipe recipes[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, { 0, 0 }, { 0, 0 } },
  { 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, { 5, 0 }, { 1, 0 } },
  { 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, { 0, 0 }, { 0, 0 } },
  { 0, 1, 0, 0, 0, 0, 0, 1, 9, 0, 0, { 0, 0 }, { 0, 0 } },
  { 0, 1, 0, 0, 0, 1, 0, 1, 2, 0, 0, { 5, 0 }, { 1, 0 } },
  { 0, 1, 0, 0, 0, 2, 0, 1, 2, 0, 0, { 0, 0 }, { 0, 0 } },
  { 1, 0, 0, 0, 0, 0, 0, 0, 5, 0, 1, { 0, 0 }, { 0, 0 } },
  { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, { 6, 0 }, { 1, 0 } },
  { 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, { 0, 0 }, { 0, 0 } },
  { 1, 1, 0, 0, 0, 0, 0, 1, 5, 0, 1, { 0, 0 }, { 0, 0 } },
  { 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, { 6, 0 }, { 1, 0 } },
  { 1, 1, 0, 0, 0, 2, 0, 1, 1, 0, 0, { 0, 0 }, { 0, 0 } },
  { 2, 0, 0, 0, 0, 0, 0, 0, 5, 1, 1, { 0, 0 }, { 0, 0 } },
  { 2, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, { 1, 0 }, { 1, 0 } },
  { 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, { 1, 2 }, { 1, 1 } },
  { 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, { 1, 0 }, { 1, 0 } },
  { 2, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, { 2, 0 }, { 1, 0 } },
  { 2, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, { 7, 0 }, { 1, 0 } },
  { 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, { 1, 7 }, { 1, 2 } },
  { 2, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, { 2, 0 }, { 2, 0 } },
  { 2, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, { 7, 0 }, { 2, 0 } },
  { 2, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, { 0, 0 }, { 0, 0 } },
  { 2, 1, 0, 0, 0, 0, 0, 1, 5, 1, 1, { 0, 0 }, { 0, 0 } },
  { 2, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, { 1, 7 }, { 1, 1 } },
  { 2, 1, 0, 0, 0, 1, 2, 6, 0, 0, 0, { 6, 0 }, { 1, 0 } },
  { 2, 1, 0, 0, 0, 1, 3, 1, 0, 0, 0, { 7, 0 }, { 1, 0 } },
  { 2, 1, 0, 0, 0, 2, 0, 1, 0, 0, 0, { 1, 2 }, { 1, 1 } },
  { 2, 1, 0, 0, 0, 2, 1, 1, 0, 0, 0, { 1, 0 }, { 1, 0 } },
  { 2, 1, 0, 0, 0, 2, 2, 1, 0, 0, 0, { 2, 0 }, { 1, 0 } },
  { 2, 1, 0, 0, 0, 2, 3, 5, 0, 0, 0, { 7, 0 }, { 3, 0 } },
  { 2, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, { 0, 0 }, { 0, 0 } },
  { 3, 0, 0, 0, 0, 0, 0, 0, 6, 1, 1, { 0, 0 }, { 0, 0 } },
  { 3, 0, 0, 0, 0, 0, 1, 2, 0, 0, 1, { 1, 0 }, { 1, 0 } },
  { 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, { 1, 2 }, { 1, 1 } },
  { 3, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, { 1, 0 }, { 1, 0 } },
  { 3, 0, 0, 0, 0, 1, 2, 0, 0, 0, 1, { 2, 0 }, { 1, 0 } },
  { 3, 0, 0, 0, 0, 1, 3, 0, 0, 0, 1, { 7, 0 }, { 1, 0 } },
  { 3, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, { 1, 2 }, { 1, 1 } },
  { 3, 0, 0, 0, 0, 2, 1, 0, 0, 0, 2, { 1, 0 }, { 1, 0 } },
  { 3, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, { 2, 0 }, { 1, 0 } },
  { 3, 0, 0, 0, 0, 2, 3, 0, 0, 0, 2, { 7, 0 }, { 1, 0 } },
  { 3, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, { 0, 0 }, { 0, 0 } },
  { 3, 0, 1, 1, 0, 2, 0, 0, 0, 0, 2, { 0, 0 }, { 0, 0 } },
  { 3, 0, 1, 3, 0, 0, 0, 0, 0, 0, 3, { 1, 7 }, { 1, 2 } },
  { 3, 0, 1, 3, 0, 1, 0, 9, 0, 2, 0, { 0, 0 }, { 0, 0 } },
  { 3, 0, 1, 3, 0, 1, 2, 2, 0, 0, 3, { 2, 0 }, { 2, 0 } },
  { 3, 0, 1, 3, 0, 1, 3, 0, 0, 0, 3, { 7, 0 }, { 2, 0 } },
  { 3, 1, 0, 0, 0, 0, 0, 1, 6, 1, 1, { 0, 0 }, { 0, 0 } },
  { 3, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, { 1, 2 }, { 1, 1 } },
  { 3, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, { 1, 0 }, { 1, 0 } },
  { 3, 1, 0, 0, 0, 1, 2, 1, 0, 0, 1, { 2, 0 }, { 1, 0 } },
  { 3, 1, 0, 0, 0, 1, 3, 1, 0, 0, 1, { 7, 0 }, { 1, 0 } },
  { 3, 1, 0, 0, 0, 2, 0, 1, 0, 0, 2, { 2, 3 }, { 1, 1 } },
  { 3, 1, 0, 0, 0, 2, 1, 1, 0, 0, 2, { 1, 2 }, { 1, 1 } },
  { 3, 1, 0, 0, 0, 2, 2, 1, 0, 0, 2, { 2, 0 }, { 1, 0 } },
  { 3, 1, 0, 0, 0, 2, 3, 1, 0, 0, 2, { 7, 0 }, { 1, 0 } },
  { 3, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, { 0, 0 }, { 0, 0 } },
  { 3, 1, 1, 1, 0, 2, 0, 5, 0, 0, 2, { 0, 0 }, { 0, 0 } },
  { 3, 1, 1, 3, 0, 0, 0, 5, 0, 1, 0, { 0, 0 }, { 0, 0 } },
  { 3, 1, 1, 3, 0, 0, 1, 1, 0, 0, 2, { 1, 0 }, { 1, 0 } },
  { 3, 1, 1, 3, 0, 1, 0, 5, 0, 0, 2, { 2, 3 }, { 2, 3 } },
  { 3, 1, 2, 1, 1, 0, 0, 1, 0, 0, 2, { 0, 0 }, { 0, 0 } },
};

static const char recipe_chars[] = { '\0', FILLER, 'd', 'e' };

static bool IsVariant(
  const Context *ctx,
  unsigned char  haystack,
  unsigned char  border,
  unsigned char  occurrences,
  unsigned char  position,
  unsigned char  overlap,
  unsigned char  partial,
  unsigned char  near_match
)
{
  return haystack == ctx->haystack && border == (unsigned char) ctx->border &&
         occurrences == ctx->occurrences && position == ctx->position &&
         overlap == (unsigned char) ctx->overlap && partial == ctx->partial &&
         near_match == ctx->near_match;
}

static void AddFill( Context *ctx, size_t n )
{
  memset( &ctx->s1_buf[ ctx->size ], FILLER, n );
  ctx->size += n;
}

static void AddPrefix( Context *ctx, size_t n )
{
  memcpy( &ctx->s1_buf[ ctx->size ], ctx->s2_buf, n );
  ctx->size += n;
}

static size_t ShapeCount( const Context *ctx )
{
  if ( ctx->haystack == HAYSTACK_LONGER && ctx->length >= 5 ) {
    return 3;
  }

  return 1;
}

static void MakeLongerNeedle( Context *ctx )
{
  static const char *const periods[] = { "ab", "abe", "abbe" };
  const char              *period;
  size_t                   shape;
  size_t                   n;
  size_t                   i;

  shape = ctx->shape;
  period = periods[ shape ];
  ctx->period = strlen( period );
  n = ctx->length;

  for ( i = 0; i < n; ++i ) {
    char c;

    if ( ctx->border ) {
      c = period[ i % ctx->period ];
    } else if ( i == 0 ) {
      c = 'b';
    } else if ( shape == 0 ) {
      c = 'a';
    } else if ( shape == 1 ) {
      c = i + 1 == n ? 'e' : 'a';
    } else {
      c = ( i % 2 ) != 0 ? 'a' : 'e';
    }

    ctx->s2_buf[ i ] = c;
  }

  ctx->s2_buf[ n ] = '\0';
}

static void AddNearMatch( Context *ctx )
{
  size_t len;
  size_t i;

  len = ctx->length;

  if (
    ctx->near_match == NEAR_MATCH_FIRST && ctx->border &&
    ctx->partial == PARTIAL_NO
  ) {
    /* The occurrence holds the border at the end of the near match */
    AddFill( ctx, 1 );
    memcpy( &ctx->s1_buf[ ctx->size ], &ctx->s2_buf[ 1 ], ctx->period - 1 );
    ctx->size += ctx->period - 1;
    return;
  }

  if (
    ctx->near_match == NEAR_MATCH_INNER && ctx->border &&
    ctx->occurrences != OCCURRENCES_NONE
  ) {
    /* The near match starts one period before the occurrence */
    AddPrefix( ctx, ctx->period );
    ctx->s1_buf[ ctx->size - ctx->period + 1 ] = FILLER;
    return;
  }

  switch ( ctx->near_match ) {
    case NEAR_MATCH_FIRST:
      i = 0;
      break;
    case NEAR_MATCH_INNER:
      i = len / 2;
      break;
    case NEAR_MATCH_LAST:
      i = len - 1;
      break;
    default:
      return;
  }

  AddPrefix( ctx, len );
  ctx->s1_buf[ ctx->size - len + i ] = FILLER;
  AddFill( ctx, 1 );
}

static void AddMisses( Context *ctx, size_t fill )
{
  AddFill( ctx, fill );

  if ( ctx->partial == PARTIAL_INSIDE ) {
    AddPrefix( ctx, ctx->length - 2 );
    AddFill( ctx, 2 );
  }

  AddNearMatch( ctx );
}

static void BuildLonger( Context *ctx, size_t fill )
{
  size_t len;

  len = ctx->length;
  MakeLongerNeedle( ctx );
  ctx->size = 0;

  if ( ctx->occurrences == OCCURRENCES_NONE ) {
    AddMisses( ctx, fill );
  } else {
    if ( ctx->position != POSITION_START ) {
      AddMisses( ctx, fill );
    }

    AddPrefix( ctx, len );

    if ( ctx->occurrences == OCCURRENCES_SEVERAL ) {
      if ( ctx->overlap ) {
        memcpy(
          &ctx->s1_buf[ ctx->size ],
          &ctx->s2_buf[ len - ctx->period ],
          ctx->period
        );
        ctx->size += ctx->period;
      } else {
        AddFill( ctx, 1 );
        AddPrefix( ctx, len );
      }
    }

    if ( ctx->position == POSITION_BETWEEN ) {
      AddFill( ctx, fill + 1 );
    } else if ( ctx->position == POSITION_START ) {
      AddFill( ctx, fill );
    }
  }

  if ( ctx->partial == PARTIAL_TAIL ) {
    AddFill( ctx, 1 );
    AddPrefix( ctx, len - 1 );
  }
}

static bool BuildLiteral( Context *ctx )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( literals ); ++i ) {
    const Literal *lit;

    lit = &literals[ i ];

    if (
      lit->length == ctx->length && IsVariant(
                                      ctx,
                                      lit->haystack,
                                      lit->border,
                                      lit->occurrences,
                                      lit->position,
                                      lit->overlap,
                                      lit->partial,
                                      lit->near_match
                                    )
    ) {
      ctx->size = strlen( lit->s1 );
      memcpy( ctx->s1_buf, lit->s1, ctx->size );
      memcpy( ctx->s2_buf, lit->s2, ctx->length );
      ctx->s2_buf[ ctx->length ] = '\0';
      return true;
    }
  }

  return false;
}

static void MakeFamilyNeedle( Context *ctx, unsigned char family )
{
  size_t n;
  size_t i;

  n = ctx->length;

  for ( i = 0; i < n; ++i ) {
    char c;

    switch ( family ) {
      case 0:
        c = i + 1 == n ? 'e' : 'd';
        break;
      case 1:
        c = 'd';
        break;
      case 2:
        c = i == 0 ? 'd' : 'e';
        break;
      case 5:
        c = i == 1 ? 'e' : 'd';
        break;
      case 6:
        c = ( i == 1 || i + 1 == n ) ? 'e' : 'd';
        break;
      default:
        c = i + 1 == n ? 'e' : ( i + 2 == n ? 'f' : 'd' );
        break;
    }

    ctx->s2_buf[ i ] = c;
  }

  ctx->s2_buf[ n ] = '\0';
}

static size_t RecipeIndex( size_t n, unsigned char at )
{
  switch ( at ) {
    case 1:
      return 0;
    case 2:
      return 1;
    case 3:
      return 2;
    case 4:
      return n / 2;
    default:
      return n - ( 8 - (size_t) at );
  }
}

static bool BuildRecipe( Context *ctx )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( recipes ); ++i ) {
    const Recipe *r;
    size_t        n;
    size_t        size;
    size_t        start;
    size_t        j;

    r = &recipes[ i ];

    if ( !IsVariant(
           ctx,
           r->haystack,
           r->border,
           r->occurrences,
           r->position,
           r->overlap,
           r->partial,
           r->near_match
         ) ) {
      continue;
    }

    n = ctx->length;
    MakeFamilyNeedle( ctx, r->family );
    ctx->size = 0;

    if ( r->lead != 0 ) {
      ctx->s1_buf[ ctx->size ] = recipe_chars[ r->lead ];
      ++ctx->size;
    }

    start = ctx->size;

    switch ( r->base ) {
      case 0:
        size = n;
        memcpy( &ctx->s1_buf[ start ], ctx->s2_buf, size );
        break;
      case 1:
        size = n - 1;
        memcpy( &ctx->s1_buf[ start ], ctx->s2_buf, size );
        break;
      case 2:
        size = n - 2;
        memcpy( &ctx->s1_buf[ start ], ctx->s2_buf, size );
        break;
      case 3:
        size = n - 1;
        memcpy( &ctx->s1_buf[ start ], &ctx->s2_buf[ 1 ], size );
        break;
      case 4:
        size = n - 2;
        memcpy( &ctx->s1_buf[ start ], &ctx->s2_buf[ 2 ], size );
        break;
      case 9:
        size = 0;
        break;
      default:
        size = n + r->base - 7;
        memset( &ctx->s1_buf[ start ], FILLER, size );
        break;
    }

    for ( j = 0; j < 2; ++j ) {
      size_t at;

      if ( r->at[ j ] == 0 ) {
        continue;
      }

      at = RecipeIndex( n, r->at[ j ] );

      if ( at < size ) {
        ctx->s1_buf[ start + at ] = recipe_chars[ r->with[ j ] ];
      }
    }

    ctx->size += size;

    if ( r->trail != 0 ) {
      ctx->s1_buf[ ctx->size ] = recipe_chars[ r->trail ];
      ++ctx->size;
    }

    return true;
  }

  return false;
}

static void MapHighByte( Context *ctx )
{
  char   letter;
  size_t i;

  letter = ctx->s2_buf[ 0 ];

  for ( i = 0; i < ctx->length; ++i ) {
    if ( ctx->s2_buf[ i ] == letter ) {
      ctx->s2_buf[ i ] = (char) HIGH;
    }
  }

  for ( i = 0; i < ctx->size; ++i ) {
    if ( ctx->s1_buf[ i ] == letter ) {
      ctx->s1_buf[ i ] = (char) HIGH;
    }
  }
}

static bool Build( Context *ctx, size_t fill )
{
  bool ok;

  if ( ctx->haystack == HAYSTACK_LONGER && ctx->length >= 3 ) {
    BuildLonger( ctx, fill );
    ok = true;
  } else if ( ctx->length <= 3 ) {
    ok = BuildLiteral( ctx );
  } else {
    ok = BuildRecipe( ctx );
  }

  if ( ok && ctx->high && ctx->length > 0 ) {
    MapHighByte( ctx );
  }

  ctx->s1_buf[ ctx->size ] = '\0';
  return ok;
}

static const char *Find( const Context *ctx )
{
  size_t i;

  for ( i = 0; i + ctx->length <= ctx->size; ++i ) {
    if ( memcmp( &ctx->s1_buf[ i ], ctx->s2_buf, ctx->length ) == 0 ) {
      return &ctx->s1_buf[ i ];
    }
  }

  return NULL;
}

static void CStringReqStrstr_Pre_Length_Prepare(
  CStringReqStrstr_Context   *ctx,
  CStringReqStrstr_Pre_Length state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_Length_Empty: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * equal to zero.
       */
      ctx->length = 0;
      break;
    }

    case CStringReqStrstr_Pre_Length_One: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * equal to one.
       */
      ctx->length = 1;
      break;
    }

    case CStringReqStrstr_Pre_Length_Two: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * equal to two.
       */
      ctx->length = 2;
      break;
    }

    case CStringReqStrstr_Pre_Length_Three: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * equal to three.
       */
      ctx->length = 3;
      break;
    }

    case CStringReqStrstr_Pre_Length_Four: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * equal to four.
       */
      ctx->length = 4;
      break;
    }

    case CStringReqStrstr_Pre_Length_Five: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * equal to five.
       */
      ctx->length = 5;
      break;
    }

    case CStringReqStrstr_Pre_Length_Long: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * greater than five.
       */
      ctx->length = 300;
      break;
    }

    case CStringReqStrstr_Pre_Length_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_Haystack_Prepare(
  CStringReqStrstr_Context     *ctx,
  CStringReqStrstr_Pre_Haystack state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_Haystack_Shorter: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * less than the length of the string referenced by the `s2` parameter
       * minus one.
       */
      ctx->haystack = HAYSTACK_SHORTER;
      break;
    }

    case CStringReqStrstr_Pre_Haystack_BelowNeedle: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * equal to the length of the string referenced by the `s2` parameter
       * minus one.
       */
      ctx->haystack = HAYSTACK_BELOW;
      break;
    }

    case CStringReqStrstr_Pre_Haystack_EqualNeedle: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * equal to the length of the string referenced by the `s2` parameter.
       */
      ctx->haystack = HAYSTACK_EQUAL;
      break;
    }

    case CStringReqStrstr_Pre_Haystack_AboveNeedle: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * equal to the length of the string referenced by the `s2` parameter
       * plus one.
       */
      ctx->haystack = HAYSTACK_ABOVE;
      break;
    }

    case CStringReqStrstr_Pre_Haystack_Longer: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * greater than the length of the string referenced by the `s2` parameter
       * plus one.
       */
      ctx->haystack = HAYSTACK_LONGER;
      break;
    }

    case CStringReqStrstr_Pre_Haystack_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_Border_Prepare(
  CStringReqStrstr_Context   *ctx,
  CStringReqStrstr_Pre_Border state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_Border_Yes: {
      /*
       * While the string referenced by the `s2` parameter has a border.
       */
      ctx->border = true;
      break;
    }

    case CStringReqStrstr_Pre_Border_No: {
      /*
       * While the string referenced by the `s2` parameter has no border.
       */
      ctx->border = false;
      break;
    }

    case CStringReqStrstr_Pre_Border_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_Occurrences_Prepare(
  CStringReqStrstr_Context        *ctx,
  CStringReqStrstr_Pre_Occurrences state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_Occurrences_None: {
      /*
       * While the string referenced by the `s2` parameter does not occur in
       * the string referenced by the `s1` parameter.
       */
      ctx->occurrences = OCCURRENCES_NONE;
      break;
    }

    case CStringReqStrstr_Pre_Occurrences_One: {
      /*
       * While the string referenced by the `s2` parameter occurs exactly once
       * in the string referenced by the `s1` parameter.
       */
      ctx->occurrences = OCCURRENCES_ONE;
      break;
    }

    case CStringReqStrstr_Pre_Occurrences_Several: {
      /*
       * While the string referenced by the `s2` parameter occurs more than
       * once in the string referenced by the `s1` parameter.
       */
      ctx->occurrences = OCCURRENCES_SEVERAL;
      break;
    }

    case CStringReqStrstr_Pre_Occurrences_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_Position_Prepare(
  CStringReqStrstr_Context     *ctx,
  CStringReqStrstr_Pre_Position state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_Position_Start: {
      /*
       * While the first occurrence of the string referenced by the `s2`
       * parameter in the string referenced by the `s1` parameter is at index
       * zero.
       */
      ctx->position = POSITION_START;
      break;
    }

    case CStringReqStrstr_Pre_Position_Between: {
      /*
       * While the first occurrence of the string referenced by the `s2`
       * parameter in the string referenced by the `s1` parameter is at an
       * index greater than zero, while the first occurrence does not end at
       * the last character of the string referenced by the `s1` parameter.
       */
      ctx->position = POSITION_BETWEEN;
      break;
    }

    case CStringReqStrstr_Pre_Position_End: {
      /*
       * While the first occurrence of the string referenced by the `s2`
       * parameter in the string referenced by the `s1` parameter is at an
       * index greater than zero, while the first occurrence ends at the last
       * character of the string referenced by the `s1` parameter.
       */
      ctx->position = POSITION_END;
      break;
    }

    case CStringReqStrstr_Pre_Position_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_Overlap_Prepare(
  CStringReqStrstr_Context    *ctx,
  CStringReqStrstr_Pre_Overlap state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_Overlap_Yes: {
      /*
       * While two occurrences of the string referenced by the `s2` parameter
       * in the string referenced by the `s1` parameter share a character.
       */
      ctx->overlap = true;
      break;
    }

    case CStringReqStrstr_Pre_Overlap_No: {
      /*
       * While no two occurrences of the string referenced by the `s2`
       * parameter in the string referenced by the `s1` parameter share a
       * character.
       */
      ctx->overlap = false;
      break;
    }

    case CStringReqStrstr_Pre_Overlap_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_Partial_Prepare(
  CStringReqStrstr_Context    *ctx,
  CStringReqStrstr_Pre_Partial state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_Partial_No: {
      /*
       * While the string referenced by the `s1` parameter holds no partial
       * match of the string referenced by the `s2` parameter which no
       * occurrence precedes, while the string referenced by the `s1` parameter
       * does not end with a partial match.
       */
      ctx->partial = PARTIAL_NO;
      break;
    }

    case CStringReqStrstr_Pre_Partial_Inside: {
      /*
       * While the string referenced by the `s1` parameter holds a partial
       * match of the string referenced by the `s2` parameter which no
       * occurrence precedes, while the string referenced by the `s1` parameter
       * does not end with a partial match.
       */
      ctx->partial = PARTIAL_INSIDE;
      break;
    }

    case CStringReqStrstr_Pre_Partial_Tail: {
      /*
       * While the string referenced by the `s1` parameter ends with a partial
       * match of the string referenced by the `s2` parameter.
       */
      ctx->partial = PARTIAL_TAIL;
      break;
    }

    case CStringReqStrstr_Pre_Partial_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_NearMatch_Prepare(
  CStringReqStrstr_Context      *ctx,
  CStringReqStrstr_Pre_NearMatch state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_NearMatch_None: {
      /*
       * While the string referenced by the `s1` parameter holds no near match
       * of the string referenced by the `s2` parameter which no occurrence
       * precedes.
       */
      ctx->near_match = NEAR_MATCH_NONE;
      break;
    }

    case CStringReqStrstr_Pre_NearMatch_First: {
      /*
       * While the first near match of the string referenced by the `s2`
       * parameter in the string referenced by the `s1` parameter which no
       * occurrence precedes differs in the first character.
       */
      ctx->near_match = NEAR_MATCH_FIRST;
      break;
    }

    case CStringReqStrstr_Pre_NearMatch_Inner: {
      /*
       * While the first near match of the string referenced by the `s2`
       * parameter in the string referenced by the `s1` parameter which no
       * occurrence precedes differs in a character other than the first and
       * the last character.
       */
      ctx->near_match = NEAR_MATCH_INNER;
      break;
    }

    case CStringReqStrstr_Pre_NearMatch_Last: {
      /*
       * While the first near match of the string referenced by the `s2`
       * parameter in the string referenced by the `s1` parameter which no
       * occurrence precedes differs in the last character, while the length of
       * the string referenced by the `s2` parameter is greater than one.
       */
      ctx->near_match = NEAR_MATCH_LAST;
      break;
    }

    case CStringReqStrstr_Pre_NearMatch_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_HighByte_Prepare(
  CStringReqStrstr_Context     *ctx,
  CStringReqStrstr_Pre_HighByte state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_HighByte_Yes: {
      /*
       * While the string referenced by the `s2` parameter holds a character
       * with a value greater than 127, where the value is interpreted as type
       * `unsigned char`.
       */
      ctx->high = true;
      break;
    }

    case CStringReqStrstr_Pre_HighByte_No: {
      /*
       * While the string referenced by the `s2` parameter holds no character
       * with a value greater than 127, where the value is interpreted as type
       * `unsigned char`.
       */
      ctx->high = false;
      break;
    }

    case CStringReqStrstr_Pre_HighByte_NA:
      break;
  }
}

static void CStringReqStrstr_Post_Result_Check(
  CStringReqStrstr_Context    *ctx,
  CStringReqStrstr_Post_Result state
)
{
  size_t i;

  T_gt_sz( ctx->count, 0 );

  switch ( state ) {
    case CStringReqStrstr_Post_Result_S1: {
      /*
       * The return value of strstr() shall be the value of the `s1` parameter.
       */
      for ( i = 0; i < ctx->count; ++i ) {
        T_eq_ptr( ctx->retval[ i ], ctx->s1_buf );
      }
      break;
    }

    case CStringReqStrstr_Post_Result_Occurrence: {
      /*
       * The return value of strstr() shall be a pointer to the first character
       * of the first occurrence of the string referenced by the `s2` parameter
       * in the string referenced by the `s1` parameter.
       */
      for ( i = 0; i < ctx->count; ++i ) {
        T_eq_ptr( ctx->retval[ i ], ctx->expected[ i ] );
      }
      break;
    }

    case CStringReqStrstr_Post_Result_Null: {
      /*
       * The return value of strstr() shall be the NULL pointer.
       */
      for ( i = 0; i < ctx->count; ++i ) {
        T_null( ctx->retval[ i ] );
      }
      break;
    }

    case CStringReqStrstr_Post_Result_NA:
      break;
  }
}

static void CStringReqStrstr_Post_S1_Check(
  CStringReqStrstr_Context *ctx,
  CStringReqStrstr_Post_S1  state
)
{
  size_t i;

  switch ( state ) {
    case CStringReqStrstr_Post_S1_Nop: {
      /*
       * The bytes of the string referenced by the `s1` parameter shall not be
       * modified.
       */
      for ( i = 0; i < ctx->count; ++i ) {
        T_eq_int( ctx->s1_cmp[ i ], 0 );
      }
      break;
    }

    case CStringReqStrstr_Post_S1_NA:
      break;
  }
}

static void CStringReqStrstr_Post_S2_Check(
  CStringReqStrstr_Context *ctx,
  CStringReqStrstr_Post_S2  state
)
{
  size_t i;

  switch ( state ) {
    case CStringReqStrstr_Post_S2_Nop: {
      /*
       * The bytes of the string referenced by the `s2` parameter shall not be
       * modified.
       */
      for ( i = 0; i < ctx->count; ++i ) {
        T_eq_int( ctx->s2_cmp[ i ], 0 );
      }
      break;
    }

    case CStringReqStrstr_Post_S2_NA:
      break;
  }
}

static void CStringReqStrstr_Post_Errno_Check(
  CStringReqStrstr_Context   *ctx,
  CStringReqStrstr_Post_Errno state
)
{
  size_t i;

  switch ( state ) {
    case CStringReqStrstr_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      for ( i = 0; i < ctx->count; ++i ) {
        T_eq_int( ctx->errno_value[ i ], -1 );
      }
      break;
    }

    case CStringReqStrstr_Post_Errno_NA:
      break;
  }
}

static void CStringReqStrstr_Prepare( CStringReqStrstr_Context *ctx )
{
  ctx->position = POSITION_NONE;
}

static void CStringReqStrstr_Action( CStringReqStrstr_Context *ctx )
{
  size_t shape;

  ctx->count = 0;

  for ( shape = 0; shape < ShapeCount( ctx ); ++shape ) {
    size_t i;

    i = ctx->count;
    ctx->shape = shape;
    ctx->size = 0;
    T_quiet_true( Build( ctx, 2560 ) );
    ctx->expected[ i ] = Find( ctx );
    memcpy( ctx->s1_before, ctx->s1_buf, sizeof( ctx->s1_buf ) );
    memcpy( ctx->s2_before, ctx->s2_buf, sizeof( ctx->s2_buf ) );
    errno = -1;
    ctx->retval[ i ] = strstr( ctx->s1_buf, ctx->s2_buf );
    ctx->errno_value[ i ] = errno;
    ctx->s1_cmp[ i ] = memcmp(
      ctx->s1_before,
      ctx->s1_buf,
      sizeof( ctx->s1_buf )
    );
    ctx->s2_cmp[ i ] = memcmp(
      ctx->s2_before,
      ctx->s2_buf,
      sizeof( ctx->s2_buf )
    );
    ctx->count = i + 1;
  }
}

/* clang-format off */

static const CStringReqStrstr_Entry
CStringReqStrstr_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_Occurrence,
    CStringReqStrstr_Post_S1_Nop, CStringReqStrstr_Post_S2_Nop,
    CStringReqStrstr_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_Null,
    CStringReqStrstr_Post_S1_Nop, CStringReqStrstr_Post_S2_Nop,
    CStringReqStrstr_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_NA,
    CStringReqStrstr_Post_S1_NA, CStringReqStrstr_Post_S2_NA,
    CStringReqStrstr_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CStringReqStrstr_Post_Result_S1,
    CStringReqStrstr_Post_S1_Nop, CStringReqStrstr_Post_S2_Nop,
    CStringReqStrstr_Post_Errno_Nop }
};

static const uint8_t
CStringReqStrstr_Map[] = {
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 0, 0, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 22, 22, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 22, 22, 24, 24, 24, 24, 24, 24, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 24, 24, 24, 24, 24, 24,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 22, 22, 24,
  24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 17, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 24, 24,
  24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 22, 22, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 0, 0, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 24, 40, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 13, 13, 24, 24, 24, 24, 24, 24, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 24, 24, 24, 24, 24, 24,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 39, 39, 24,
  24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 17, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 24, 24,
  24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 24, 40, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 0, 0, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 39, 39, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 24, 24, 24, 24, 24, 24, 17, 17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 24, 40, 24, 24, 24, 24, 24, 24, 17,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 25, 25, 19, 19, 12, 12, 12, 12, 25, 25, 25, 25,
  25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 25, 25, 19, 19, 12, 12, 12, 12, 25,
  25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 25, 25, 19, 19, 12, 12,
  12, 12, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 22, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 22, 22, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 22, 22, 22,
  10, 10, 10, 10, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
  22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  22, 22, 22, 22, 10, 10, 10, 10, 22, 22, 22, 22, 22, 22, 22, 22, 15, 15, 15,
  15, 15, 15, 15, 15, 22, 22, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 22, 22,
  2, 2, 2, 2, 2, 2, 22, 22, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 22, 22,
  2, 2, 2, 2, 2, 2, 22, 22, 22, 22, 10, 10, 10, 10, 22, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 10, 10, 10, 10, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 25, 25, 25, 25, 12, 12, 12, 12, 25,
  25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 25, 25, 25, 25, 12, 12,
  12, 12, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 25, 25, 25,
  25, 12, 12, 12, 12, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
  25, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 22, 22, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 22, 22, 2, 2, 2, 2, 2,
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  22, 22, 22, 22, 10, 10, 10, 10, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 22, 22, 22, 22, 10, 10, 10, 10, 22, 22, 22, 22, 22, 22, 22,
  22, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 22, 22, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 22, 22, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 22, 22, 22, 22, 10, 10, 10, 10, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20, 20, 9, 9, 9,
  9, 9, 9, 20, 20, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20, 20, 9, 9,
  9, 9, 9, 9, 20, 20, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20, 20, 9,
  9, 9, 9, 9, 9, 20, 20, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15,
  15, 15, 15, 15, 15, 15, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6,
  2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2,
  2, 2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14,
  14, 9, 9, 12, 12, 12, 12, 21, 21, 9, 9, 9, 9, 9, 9, 21, 21, 9, 9, 9, 9, 9, 9,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3,
  9, 9, 12, 12, 12, 12, 21, 21, 9, 9, 9, 9, 9, 9, 21, 21, 9, 9, 9, 9, 9, 9, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9,
  9, 12, 12, 12, 12, 21, 21, 9, 9, 9, 9, 9, 9, 21, 21, 9, 9, 9, 9, 9, 9, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10,
  10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10,
  6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20, 20, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12,
  12, 12, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 18, 18,
  10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
  18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  18, 18, 18, 18, 10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18, 18, 15, 15, 15,
  15, 15, 15, 15, 15, 18, 18, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18,
  2, 2, 2, 2, 2, 2, 18, 18, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18,
  2, 2, 2, 2, 2, 2, 18, 18, 18, 18, 10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18,
  18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 10, 10, 10, 10, 18, 18,
  18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 14, 14, 12, 12, 12, 12, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 3, 3, 12, 12,
  12, 12, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 3,
  3, 12, 12, 12, 12, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  21, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 23, 23, 2, 2, 2, 2, 2,
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  35, 35, 13, 13, 10, 10, 10, 10, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 35, 35, 13, 13, 10, 10, 10, 10, 23, 23, 23, 23, 23, 23, 23,
  23, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 23, 23, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 35, 35, 13, 13, 10, 10, 10, 10, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12,
  12, 12, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 18, 18,
  10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
  18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  18, 18, 18, 18, 10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18, 18, 15, 15, 15,
  15, 15, 15, 15, 15, 18, 18, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18,
  2, 2, 2, 2, 2, 2, 18, 18, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18,
  2, 2, 2, 2, 2, 2, 18, 18, 18, 18, 10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18,
  18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 10, 10, 10, 10, 18, 18,
  18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 14, 14, 12, 12, 12, 12, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 3, 3, 12, 12,
  12, 12, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 3,
  3, 12, 12, 12, 12, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  21, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 23, 23, 2, 2, 2, 2, 2,
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  35, 35, 16, 16, 10, 10, 10, 10, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 35, 35, 11, 11, 10, 10, 10, 10, 23, 23, 23, 23, 23, 23, 23,
  23, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 23, 23, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 35, 35, 16, 16, 10, 10, 10, 10, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12, 12, 12, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 20, 19, 19, 12, 12,
  12, 12, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 18, 18,
  10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
  18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  18, 18, 18, 18, 10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18, 18, 15, 15, 15,
  15, 15, 15, 15, 15, 18, 18, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18,
  2, 2, 2, 2, 2, 2, 18, 18, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 18, 18,
  2, 2, 2, 2, 2, 2, 18, 18, 18, 18, 10, 10, 10, 10, 18, 18, 18, 18, 18, 18, 18,
  18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 10, 10, 10, 10, 18, 18,
  18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 14, 14, 12, 12, 12, 12, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 3, 3, 12, 12,
  12, 12, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 34, 3,
  3, 12, 12, 12, 12, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  21, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 23, 23, 2, 2, 2, 2, 2,
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  35, 35, 11, 11, 10, 10, 10, 10, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 35, 35, 11, 11, 10, 10, 10, 10, 23, 23, 23, 23, 23, 23, 23,
  23, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 23, 23, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 35, 35, 11, 11, 10, 10, 10, 10, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 33, 33, 9, 9, 9,
  9, 9, 9, 33, 33, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 33, 33, 9, 9, 9,
  9, 9, 9, 33, 33, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 33, 33, 9, 9, 9,
  9, 9, 9, 33, 33, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15,
  15, 15, 15, 15, 15, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2,
  2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2,
  2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14,
  9, 9, 12, 12, 12, 12, 33, 33, 9, 9, 9, 9, 9, 9, 33, 33, 9, 9, 9, 9, 9, 9, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9,
  9, 12, 12, 12, 12, 33, 33, 9, 9, 9, 9, 9, 9, 33, 33, 9, 9, 9, 9, 9, 9, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9,
  12, 12, 12, 12, 33, 33, 9, 9, 9, 9, 9, 9, 33, 33, 9, 9, 9, 9, 9, 9, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10,
  10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10,
  6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 30, 30, 9, 9, 9, 9, 9,
  9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 30, 30, 9, 9, 9, 9, 9,
  9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 30, 30, 9, 9, 9, 9, 9, 9,
  3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2,
  2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15,
  15, 15, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2,
  2, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6,
  6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 9, 9, 12,
  12, 12, 12, 30, 30, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12,
  12, 12, 12, 30, 30, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12,
  12, 12, 30, 30, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6,
  6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 30, 30, 27, 27, 28, 28, 14, 14, 36,
  36, 14, 14, 28, 28, 27, 27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 30, 30, 27, 27, 28, 28,
  3, 3, 36, 36, 3, 3, 28, 28, 27, 27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 30, 30, 27, 27,
  28, 28, 3, 3, 36, 36, 3, 3, 28, 28, 27, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 31, 13, 13, 10, 10, 10, 10, 32, 32, 13,
  13, 29, 29, 13, 13, 31, 31, 13, 13, 29, 29, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 31, 13, 13, 10, 10, 10,
  10, 32, 32, 13, 13, 29, 29, 13, 13, 15, 15, 15, 15, 15, 15, 15, 15, 13, 13,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 13, 13,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 31, 31,
  13, 13, 10, 10, 10, 10, 32, 32, 13, 13, 29, 29, 13, 13, 31, 31, 13, 13, 29,
  29, 13, 13, 31, 31, 13, 13, 10, 10, 10, 10, 32, 32, 13, 13, 29, 29, 13, 13,
  31, 31, 13, 13, 29, 29, 13, 13, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 30, 30, 27, 27, 28, 28, 14,
  14, 14, 14, 27, 27, 28, 28, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 30, 30, 27, 27, 28,
  28, 3, 3, 3, 3, 27, 27, 28, 28, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 30, 30, 27, 27,
  28, 28, 3, 3, 3, 3, 27, 27, 28, 28, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 32, 32, 13, 13,
  29, 29, 13, 13, 13, 13, 13, 13, 29, 29, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 32,
  32, 13, 13, 29, 29, 13, 13, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 13, 13, 10, 10, 10,
  10, 32, 32, 13, 13, 29, 29, 13, 13, 13, 13, 13, 13, 29, 29, 13, 13, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12,
  12, 12, 30, 30, 14, 14, 28, 28, 14, 14, 36, 36, 14, 14, 28, 28, 14, 14, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19,
  19, 12, 12, 12, 12, 30, 30, 3, 3, 28, 28, 3, 3, 36, 36, 3, 3, 28, 28, 3, 3,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3,
  19, 19, 12, 12, 12, 12, 30, 30, 3, 3, 28, 28, 3, 3, 36, 36, 3, 3, 28, 28, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31,
  31, 16, 16, 10, 10, 10, 10, 32, 32, 16, 16, 29, 29, 16, 16, 31, 31, 16, 16,
  29, 29, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 31, 31, 11, 11, 10, 10, 10, 10, 32, 32, 26, 26, 29, 29, 26, 26,
  15, 15, 15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 31, 31, 16, 16, 10, 10, 10, 10, 32, 32, 16,
  16, 29, 29, 16, 16, 31, 31, 16, 16, 29, 29, 16, 16, 31, 31, 16, 16, 10, 10,
  10, 10, 32, 32, 16, 16, 29, 29, 16, 16, 31, 31, 16, 16, 29, 29, 16, 16, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 12, 12,
  12, 12, 30, 30, 38, 38, 28, 28, 14, 14, 14, 14, 14, 14, 28, 28, 14, 14, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3,
  12, 12, 12, 12, 30, 30, 38, 38, 28, 28, 3, 3, 3, 3, 3, 3, 28, 28, 3, 3, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3,
  12, 12, 12, 12, 30, 30, 38, 38, 28, 28, 3, 3, 3, 3, 3, 3, 28, 28, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16,
  10, 10, 10, 10, 32, 32, 16, 16, 29, 29, 16, 16, 16, 16, 16, 16, 29, 29, 16,
  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  11, 11, 26, 26, 10, 10, 10, 10, 32, 32, 26, 26, 29, 29, 11, 11, 15, 15, 15,
  15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 26, 26, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 26, 2,
  2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 16, 16, 16, 16, 10, 10, 10, 10, 32, 32, 16, 16, 29, 29, 16, 16, 16,
  16, 16, 16, 29, 29, 16, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 30, 30, 14, 14, 28, 28, 14, 14,
  36, 36, 14, 14, 28, 28, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 30, 30, 3, 3, 28,
  28, 3, 3, 36, 36, 3, 3, 28, 28, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 30, 30, 3, 3,
  28, 28, 3, 3, 36, 36, 3, 3, 28, 28, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 31, 11, 11, 10, 10, 10, 10, 32, 32, 11, 11,
  29, 29, 11, 11, 31, 31, 11, 11, 29, 29, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 31, 11, 11, 10, 10, 10, 10, 32,
  32, 11, 11, 29, 29, 11, 11, 15, 15, 15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 31, 31, 11, 11,
  10, 10, 10, 10, 32, 32, 11, 11, 29, 29, 11, 11, 31, 31, 11, 11, 29, 29, 11,
  11, 31, 31, 11, 11, 10, 10, 10, 10, 32, 32, 11, 11, 29, 29, 11, 11, 31, 31,
  11, 11, 29, 29, 11, 11, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 30, 30, 14, 14, 28, 28, 14, 14, 14, 14,
  14, 14, 28, 28, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 30, 30, 3, 3, 28, 28, 3, 3, 3,
  3, 3, 3, 28, 28, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 30, 30, 3, 3, 28, 28, 3, 3, 3,
  3, 3, 3, 28, 28, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11,
  2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 32, 32, 11, 11, 29, 29, 11, 11,
  11, 11, 11, 11, 29, 29, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 32, 32, 11, 11,
  29, 29, 11, 11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 11, 11, 10, 10, 10, 10, 32, 32,
  11, 11, 29, 29, 11, 11, 11, 11, 11, 11, 29, 29, 11, 11, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 37,
  37, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 37,
  37, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 37,
  37, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6,
  15, 15, 15, 15, 15, 15, 15, 15, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 6, 6, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6,
  6, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 14, 14, 9, 9, 12, 12, 12, 12, 37, 37, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9,
  9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 3, 3, 9, 9, 12, 12, 12, 12, 37, 37, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9,
  9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
  3, 9, 9, 12, 12, 12, 12, 37, 37, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10,
  10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10,
  10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 9, 9, 9, 9,
  9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9,
  9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3,
  3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2,
  2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15,
  15, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2,
  6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6, 6,
  6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 9, 9, 12, 12,
  12, 12, 14, 14, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12,
  12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12, 12,
  3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6,
  15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6,
  6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 27, 27, 27, 27, 14, 14, 14, 14, 14,
  14, 14, 14, 27, 27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 27, 27, 27, 27, 3, 3, 3,
  3, 3, 3, 3, 3, 27, 27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 27, 27, 27, 27, 3, 3,
  3, 3, 3, 3, 3, 3, 27, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13,
  13, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13,
  13, 13, 13, 13, 15, 15, 15, 15, 15, 15, 15, 15, 13, 13, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 13, 13, 13, 13, 10, 10, 10,
  10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14,
  14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 27, 27,
  14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 27, 27,
  3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 27, 27, 3, 3,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13,
  13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13,
  15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  13, 13, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11,
  26, 26, 26, 26, 26, 26, 15, 15, 15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 16, 16, 16, 16, 10,
  10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2,
  2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 11, 11, 26, 26, 10, 10, 10, 10, 11, 11, 26, 26, 11, 11, 11,
  11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 26, 26, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 26, 26, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11,
  11, 11, 11, 11, 11, 11, 11, 15, 15, 15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 11, 11, 11, 11,
  10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2,
  2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11,
  11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 11, 11, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 9, 9, 9,
  9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9,
  9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9,
  3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2,
  2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15,
  15, 15, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2,
  2, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6,
  6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 9, 9, 12,
  12, 12, 12, 14, 14, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12,
  12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12,
  12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6,
  6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 6, 6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9,
  9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9,
  9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
  3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10,
  10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10,
  10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 6, 6, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 9, 9, 12, 12, 12, 12, 14, 14, 9, 9, 9,
  9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9,
  9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9,
  3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2,
  2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15,
  15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6,
  6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12,
  12, 12, 14, 14, 27, 27, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19,
  19, 12, 12, 12, 12, 3, 3, 27, 27, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19,
  19, 12, 12, 12, 12, 3, 3, 27, 27, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13,
  10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 15, 15, 15,
  15, 15, 15, 15, 15, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13,
  2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13,
  2, 2, 2, 2, 2, 2, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 27, 27, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12,
  12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 27, 27, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 27, 27, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10,
  10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 13, 13,
  10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19,
  19, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
  3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16,
  16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 26, 26, 26, 26, 26, 26, 15, 15,
  15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26,
  26, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26,
  26, 2, 2, 2, 2, 2, 2, 16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 10, 10, 10, 10, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12,
  12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 10, 10, 10, 10, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 26, 26, 10, 10,
  10, 10, 11, 11, 26, 26, 11, 11, 11, 11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 26, 26, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 16, 16, 16, 16,
  10, 10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19,
  19, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
  3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11,
  11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 15, 15,
  15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11,
  11, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11,
  11, 2, 2, 2, 2, 2, 2, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12,
  12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10,
  10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 11, 11,
  10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19,
  19, 12, 12, 12, 12, 14, 14, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19,
  19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19,
  12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6,
  6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 6, 6, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 14, 14, 9, 9, 12, 12, 12, 12, 14, 14, 9, 9, 9, 9, 9, 9,
  14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3,
  3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9,
  9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2,
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6,
  6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10,
  10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14,
  14, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3,
  9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9,
  9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15,
  15, 15, 15, 15, 15, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2,
  2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2,
  2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14,
  9, 9, 12, 12, 12, 12, 14, 14, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9,
  9, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9,
  12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6,
  6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 27, 27, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 27, 27, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 27, 27, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13,
  13, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13,
  13, 13, 13, 13, 15, 15, 15, 15, 15, 15, 15, 15, 13, 13, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 13, 13, 13, 13, 10, 10, 10,
  10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14,
  14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 27, 27,
  14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 27, 27,
  3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 27, 27, 3, 3,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13,
  13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13,
  15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  13, 13, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11,
  26, 26, 26, 26, 26, 26, 15, 15, 15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 16, 16, 16, 16, 10,
  10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2,
  2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 11, 11, 26, 26, 10, 10, 10, 10, 11, 11, 26, 26, 11, 11, 11,
  11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 26, 26, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 26, 26, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11,
  11, 11, 11, 11, 11, 11, 11, 15, 15, 15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 11, 11, 11, 11,
  10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2,
  2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11,
  11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 11, 11, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 9, 9, 9,
  9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9,
  9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9,
  3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2,
  2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15,
  15, 15, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2,
  2, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6,
  6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 9, 9, 12,
  12, 12, 12, 14, 14, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12,
  12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12,
  12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6,
  6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 6, 6, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 14, 14, 19, 19, 12, 12, 12, 12, 14, 14, 9, 9, 9, 9, 9, 9, 14, 14, 9, 9, 9,
  9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9,
  9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
  3, 19, 19, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10,
  10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 10, 10, 10,
  10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15, 15, 15, 6, 6, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 9, 9, 12, 12, 12, 12, 14, 14, 9, 9, 9,
  9, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9,
  9, 9, 3, 3, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 9, 9, 12, 12, 12, 12, 3, 3, 9, 9, 9, 9, 9, 9,
  3, 3, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2,
  2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 15, 15, 15, 15, 15, 15,
  15, 15, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 2, 2, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6,
  6, 10, 10, 10, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19, 19, 12, 12,
  12, 12, 14, 14, 27, 27, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19,
  19, 12, 12, 12, 12, 3, 3, 27, 27, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 19,
  19, 12, 12, 12, 12, 3, 3, 27, 27, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13,
  10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 15, 15, 15,
  15, 15, 15, 15, 15, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13,
  2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13,
  2, 2, 2, 2, 2, 2, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 27, 27, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12,
  12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 27, 27, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 27, 27, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10, 10, 10, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 10, 10,
  10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 13, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 13, 13, 13, 13,
  10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19,
  19, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
  3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16,
  16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 26, 26, 26, 26, 26, 26, 15, 15,
  15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26,
  26, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26,
  26, 2, 2, 2, 2, 2, 2, 16, 16, 16, 16, 10, 10, 10, 10, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 10, 10, 10, 10, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12,
  12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 10, 10, 10, 10, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 26, 26, 10, 10,
  10, 10, 11, 11, 26, 26, 11, 11, 11, 11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 26, 26, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 26, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 16, 16, 16, 16,
  10, 10, 10, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 19,
  19, 12, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 3, 3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
  3, 19, 19, 12, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11,
  11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 15, 15,
  15, 15, 15, 15, 15, 15, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11,
  11, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11,
  11, 2, 2, 2, 2, 2, 2, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 12, 12, 12, 12, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12,
  12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 12, 12, 12, 12,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 11, 10, 10,
  10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 15, 15, 15, 15, 15, 15, 15, 15, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 11, 11, 11,
  10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

/* clang-format on */

static size_t CStringReqStrstr_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrstr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrstr_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrstr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrstr_Scope,
  .initial_context = &CStringReqStrstr_Instance
};

static inline CStringReqStrstr_Entry CStringReqStrstr_PopEntry(
  CStringReqStrstr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrstr_Entries[ CStringReqStrstr_Map[ index ] ];
}

static void CStringReqStrstr_SetPreConditionStates(
  CStringReqStrstr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];

  if ( ctx->Map.entry.Pre_Position_NA ) {
    ctx->Map.pcs[ 4 ] = CStringReqStrstr_Pre_Position_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
}

static void CStringReqStrstr_TestVariant( CStringReqStrstr_Context *ctx )
{
  CStringReqStrstr_Pre_Length_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrstr_Pre_Haystack_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrstr_Pre_Border_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqStrstr_Pre_Occurrences_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqStrstr_Pre_Position_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CStringReqStrstr_Pre_Overlap_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CStringReqStrstr_Pre_Partial_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CStringReqStrstr_Pre_NearMatch_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CStringReqStrstr_Pre_HighByte_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  CStringReqStrstr_Action( ctx );
  CStringReqStrstr_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqStrstr_Post_S1_Check( ctx, ctx->Map.entry.Post_S1 );
  CStringReqStrstr_Post_S2_Check( ctx, ctx->Map.entry.Post_S2 );
  CStringReqStrstr_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqStrstr( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrstr, &CStringReqStrstr_Fixture )
{
  CStringReqStrstr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrstr_Pre_Length_Empty;
    ctx->Map.pci[ 0 ] < CStringReqStrstr_Pre_Length_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrstr_Pre_Haystack_Shorter;
      ctx->Map.pci[ 1 ] < CStringReqStrstr_Pre_Haystack_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqStrstr_Pre_Border_Yes;
        ctx->Map.pci[ 2 ] < CStringReqStrstr_Pre_Border_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqStrstr_Pre_Occurrences_None;
          ctx->Map.pci[ 3 ] < CStringReqStrstr_Pre_Occurrences_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CStringReqStrstr_Pre_Position_Start;
            ctx->Map.pci[ 4 ] < CStringReqStrstr_Pre_Position_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CStringReqStrstr_Pre_Overlap_Yes;
              ctx->Map.pci[ 5 ] < CStringReqStrstr_Pre_Overlap_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CStringReqStrstr_Pre_Partial_No;
                ctx->Map.pci[ 6 ] < CStringReqStrstr_Pre_Partial_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CStringReqStrstr_Pre_NearMatch_None;
                  ctx->Map.pci[ 7 ] < CStringReqStrstr_Pre_NearMatch_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = CStringReqStrstr_Pre_HighByte_Yes;
                    ctx->Map.pci[ 8 ] < CStringReqStrstr_Pre_HighByte_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    ctx->Map.entry = CStringReqStrstr_PopEntry( ctx );

                    if ( ctx->Map.entry.Skip ) {
                      continue;
                    }

                    CStringReqStrstr_SetPreConditionStates( ctx );
                    CStringReqStrstr_Prepare( ctx );
                    CStringReqStrstr_TestVariant( ctx );
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
