/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the interfaces of the assert() invocation
 *   support.
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

#ifndef _TX_ASSERT_H
#define _TX_ASSERT_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSTestSuitesValidation
 *
 * @{
 */

/**
 * @brief This enumeration lists the programming languages of the translation
 *   unit which contains the assert() invocations.
 */
typedef enum { ASSERT_LANGUAGE_C, ASSERT_LANGUAGE_CXX } AssertLanguage;

/**
 * @brief This enumeration lists the definitions of `__ASSERT_FUNC` at an
 *   assert() invocation which expands it.
 */
typedef enum {
  ASSERT_FUNC_DEFAULT,
  ASSERT_FUNC_NULL,
  ASSERT_FUNC_STRING
} AssertFunc;

/**
 * @brief This structure contains the input and the output of an assert()
 *   invocation.
 */
typedef struct {
  /**
   * @brief This member contains the value of the expression.
   */
  int value;

  /**
   * @brief This member contains the count of expression evaluations.
   */
  size_t evaluations;

  /**
   * @brief This member references the presumed name of the source file of
   *   the invocation.
   */
  const char *file;

  /**
   * @brief This member contains the presumed line number of the invocation.
   */
  int line;

  /**
   * @brief This member references the text of the expression of the
   *   invocation.
   */
  const char *expression;
} AssertInvocation;

/**
 * @brief This constant contains the programming language of the translation
 *   unit which contains the assert() invocations.
 */
extern const AssertLanguage AssertLanguageOfInvocation;

/**
 * @brief This object contains the string which is the definition of
 *   `__ASSERT_FUNC` for ::ASSERT_FUNC_STRING.
 */
extern const char AssertFuncString[];

/**
 * @brief Invokes assert() with the definition of `NDEBUG` at the inclusion of
 *   `<assert.h>` and the definition of `__ASSERT_FUNC` at the invocation.
 *
 * Where ndebug is false and assert_func is ::ASSERT_FUNC_DEFAULT, the function
 * which encloses the invocation has the name `Assert`, the return type `void`
 * and one parameter of type `AssertInvocation *`.
 *
 * @param ndebug is true, if `NDEBUG` is defined at the inclusion of
 *   `<assert.h>`.
 *
 * @param assert_func is the definition of `__ASSERT_FUNC` at the invocation.
 *   The function ignores it, if `ndebug` is true.
 *
 * @param[in, out] invocation is the input and the output of the invocation.
 */
void AssertInvoke(
  bool              ndebug,
  AssertFunc        assert_func,
  AssertInvocation *invocation
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TX_ASSERT_H */
