/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of the assert()
 *   invocation support.
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

#include "tx-assert.h"

#ifdef __cplusplus
const AssertLanguage AssertLanguageOfInvocation = ASSERT_LANGUAGE_CXX;
#else
const AssertLanguage AssertLanguageOfInvocation = ASSERT_LANGUAGE_C;
#endif

const char AssertFuncString[] = "Function";

static int Evaluate( AssertInvocation *invocation )
{
  ++invocation->evaluations;
  return invocation->value;
}

static void SetExpected( AssertInvocation *invocation, int line )
{
  invocation->file = __FILE__;
  invocation->line = line;
  invocation->expression = "Evaluate( invocation )";
}

/*
 * Each inclusion of <assert.h> defines the assert() macro according to the
 * definition of NDEBUG at this point.  The assert() macro expands
 * __ASSERT_FUNC at the invocation.  The header provides the default
 * definition of __ASSERT_FUNC.
 */

#undef NDEBUG
#undef __ASSERT_FUNC
#include <assert.h>

static void Assert( AssertInvocation *invocation )
{
  SetExpected( invocation, __LINE__ + 1 );
  assert( Evaluate( invocation ) );
}

#undef __ASSERT_FUNC
#define __ASSERT_FUNC NULL

static void AssertFuncNull( AssertInvocation *invocation )
{
  SetExpected( invocation, __LINE__ + 1 );
  assert( Evaluate( invocation ) );
}

#undef __ASSERT_FUNC
#define __ASSERT_FUNC AssertFuncString

static void AssertFuncDefined( AssertInvocation *invocation )
{
  SetExpected( invocation, __LINE__ + 1 );
  assert( Evaluate( invocation ) );
}

/*
 * The header does not define __ASSERT_FUNC, if NDEBUG is defined.  The
 * assert() macro does not expand __ASSERT_FUNC in this case.
 */

#define NDEBUG
#undef __ASSERT_FUNC
#include <assert.h>

static void AssertNDEBUG( AssertInvocation *invocation )
{
  (void) invocation;
  assert( Evaluate( invocation ) );
}

void AssertInvoke(
  bool              ndebug,
  AssertFunc        assert_func,
  AssertInvocation *invocation
)
{
  static void ( *const asserts[] )(
    AssertInvocation *
  ) = { Assert, AssertFuncNull, AssertFuncDefined };

  if ( ndebug ) {
    AssertNDEBUG( invocation );
  } else {
    ( *asserts[ assert_func ] )( invocation );
  }
}
