/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValGr712rc
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

#include <bsp.h>
#include <grlib/ftmctrl-regs.h>
#include <grlib/io.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ValGr712rc spec:/bsp/sparc/leon3/val/gr712rc
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @brief This test case collection provides validation test cases for the
 *   `sparc/gr712rc` BSP.
 *
 * This test case performs the following actions:
 *
 * - Validate the use of the `-mfix-gr712rc` compiler option.
 *
 *   - Check that the compiler built-in define `__FIX_LEON3FT_B2BST` is
 *     defined.
 *
 *   - Check that the compiler built-in define `__FIX_LEON3FT_TN0018` is
 *     defined.
 *
 *   - Check that the `SPARC_LEON3FT_B2BST_NOP` define expands to a `nop`
 *     instruction.
 *
 * - Validate the required FTMCTRL bootloader settings.
 *
 *   - Check that the bootloader did set the MCFG2.TRP register field.
 *
 * - Decode the machine code of a function which has a floating-point operate
 *   instruction at the target of an integer branch.
 *
 *   - Check that no integer branch targets a floating-point operate
 *     instruction.
 *
 *   - Check that an integer branch targets a `nop` instruction which a
 *     floating-point operate instruction follows.
 *
 * - Decode the machine code of a function which has a control transfer
 *   instruction after an `fdivd` instruction.
 *
 *   - Check that the function contains an `fdiv` or `fsqrt` instruction.
 *
 *   - Check that a `nop` instruction separates each `fdiv` and `fsqrt`
 *     instruction from a control transfer instruction in the next three
 *     instructions.
 *
 * @{
 */

/*
 * The FTMCTRL registers are at the start of the APB area, see table 28 of the
 * GR712RC user manual.
 */
#define GR712RC_FTMCTRL_BASE 0x80000000

#define SPARC_NOP 0x01000000U

#define SPARC_RETURN_FROM_LEAF 0x81c3e008U

#define MAX_FUNCTION_SIZE 64

typedef struct {
  size_t fpop;
  size_t nop_fpop;
} BranchTargets;

typedef struct {
  size_t div_sqrt;
  size_t unguarded;
} DivSqrtSequences;

/*
 * The workarounds of the compiler depend on the code shape.  The optimize
 * attribute fixes the shape for every optimization level of the build.  The
 * function is not instrumented, so the coverage build has the same shape.
 */
#define ERRATA_FUNCTION                 \
  __attribute__((                       \
    __noinline__,                       \
    __noclone__,                        \
    __no_profile_instrument_function__, \
    __optimize__( "O2" )                \
  ))

static ERRATA_FUNCTION double FpopAtBranchTarget( int a, double x, double y )
{
  if ( a != 0 ) {
    x = x + y;
  }

  return x * y;
}

static ERRATA_FUNCTION double DivBeforeReturn( double x, double y )
{
  return x / y;
}

static size_t GetFunctionSize( const uint32_t *code )
{
  size_t i;

  for ( i = 0; i < MAX_FUNCTION_SIZE - 1; ++i ) {
    if ( code[ i ] == SPARC_RETURN_FROM_LEAF ) {
      /* Include the delay slot */
      return i + 2;
    }
  }

  return MAX_FUNCTION_SIZE;
}

static bool IsFpop( uint32_t insn )
{
  uint32_t op3;

  op3 = ( insn >> 19 ) & 0x3f;
  return ( insn >> 30 ) == 2 && ( op3 == 0x34 || op3 == 0x35 );
}

static bool IsDivSqrt( uint32_t insn )
{
  uint32_t opf;

  if ( ( insn >> 30 ) != 2 || ( ( insn >> 19 ) & 0x3f ) != 0x34 ) {
    return false;
  }

  opf = ( insn >> 5 ) & 0x1ff;
  return ( opf >= 0x29 && opf <= 0x2b ) || ( opf >= 0x4d && opf <= 0x4f );
}

static bool IsIntegerBranch( uint32_t insn )
{
  return ( insn >> 30 ) == 0 && ( ( insn >> 22 ) & 0x7 ) == 2;
}

static bool IsControlTransfer( uint32_t insn )
{
  uint32_t op;
  uint32_t op2;
  uint32_t op3;

  op = insn >> 30;

  if ( op == 1 ) {
    /* call */
    return true;
  }

  if ( op == 0 ) {
    /* Bicc, FBfcc, and CBccc */
    op2 = ( insn >> 22 ) & 0x7;
    return op2 == 2 || op2 == 6 || op2 == 7;
  }

  if ( op == 2 ) {
    /* jmpl, rett, and Ticc */
    op3 = ( insn >> 19 ) & 0x3f;
    return op3 == 0x38 || op3 == 0x39 || op3 == 0x3a;
  }

  return false;
}

static void CountBranchTargets( const uint32_t *code, BranchTargets *targets )
{
  size_t n;
  size_t i;

  n = GetFunctionSize( code );
  targets->fpop = 0;
  targets->nop_fpop = 0;

  for ( i = 0; i < n; ++i ) {
    int32_t disp;
    size_t  target;

    if ( !IsIntegerBranch( code[ i ] ) ) {
      continue;
    }

    disp = (int32_t) ( code[ i ] << 10 ) >> 10;
    target = (size_t) ( (ptrdiff_t) i + disp );

    if ( target >= n ) {
      continue;
    }

    if ( IsFpop( code[ target ] ) ) {
      ++targets->fpop;
    }

    if (
      code[ target ] == SPARC_NOP && target + 1 < n &&
      IsFpop( code[ target + 1 ] )
    ) {
      ++targets->nop_fpop;
    }
  }
}

static void CountDivSqrtSequences(
  const uint32_t   *code,
  DivSqrtSequences *sequences
)
{
  size_t n;
  size_t i;

  n = GetFunctionSize( code );
  sequences->div_sqrt = 0;
  sequences->unguarded = 0;

  for ( i = 0; i < n; ++i ) {
    size_t j;

    if ( !IsDivSqrt( code[ i ] ) ) {
      continue;
    }

    ++sequences->div_sqrt;

    for ( j = i + 1; j < n && j <= i + 3; ++j ) {
      if ( code[ j ] == SPARC_NOP ) {
        break;
      }

      if ( IsControlTransfer( code[ j ] ) ) {
        ++sequences->unguarded;
        break;
      }
    }
  }
}

/**
 * @brief Validate the use of the `-mfix-gr712rc` compiler option.
 */
static void BspSparcLeon3ValGr712rc_Action_0( void )
{
  const char *s;

  /*
   * Check that the compiler built-in define `__FIX_LEON3FT_B2BST` is defined.
   */
  #if !defined( __FIX_LEON3FT_B2BST )
  #error "__FIX_LEON3FT_B2BST is not defined"
  #endif

  /*
   * Check that the compiler built-in define `__FIX_LEON3FT_TN0018` is defined.
   */
  #if !defined( __FIX_LEON3FT_TN0018 )
  #error "__FIX_LEON3FT_TN0018 is not defined"
  #endif

  /*
   * Check that the `SPARC_LEON3FT_B2BST_NOP` define expands to a `nop`
   * instruction.
   */
  s = RTEMS_XSTRING( SPARC_LEON3FT_B2BST_NOP );
  T_true( IsEqualIgnoreWhiteSpace( s, "nop" ) );
}

/**
 * @brief Validate the required FTMCTRL bootloader settings.
 */
static void BspSparcLeon3ValGr712rc_Action_1( void )
{
  ftmctrl *regs;
  uint32_t mcfg2;

  regs = (ftmctrl *) GR712RC_FTMCTRL_BASE;
  mcfg2 = grlib_load_32( &regs->mcfg2 );

  /*
   * Check that the bootloader did set the MCFG2.TRP register field.
   */
  T_ne_u32( mcfg2 & FTMCTRL_MCFG2_TRP, 0 );
}

/**
 * @brief Decode the machine code of a function which has a floating-point
 *   operate instruction at the target of an integer branch.
 */
static void BspSparcLeon3ValGr712rc_Action_2( void )
{
  BranchTargets targets;

  CountBranchTargets(
    (const uint32_t *) (uintptr_t) FpopAtBranchTarget,
    &targets
  );

  /*
   * Check that no integer branch targets a floating-point operate instruction.
   */
  T_eq_sz( targets.fpop, 0 );

  /*
   * Check that an integer branch targets a `nop` instruction which a
   * floating-point operate instruction follows.
   */
  T_ne_sz( targets.nop_fpop, 0 );
}

/**
 * @brief Decode the machine code of a function which has a control transfer
 *   instruction after an `fdivd` instruction.
 */
static void BspSparcLeon3ValGr712rc_Action_3( void )
{
  DivSqrtSequences sequences;

  CountDivSqrtSequences(
    (const uint32_t *) (uintptr_t) DivBeforeReturn,
    &sequences
  );

  /*
   * Check that the function contains an `fdiv` or `fsqrt` instruction.
   */
  T_ne_sz( sequences.div_sqrt, 0 );

  /*
   * Check that a `nop` instruction separates each `fdiv` and `fsqrt`
   * instruction from a control transfer instruction in the next three
   * instructions.
   */
  T_eq_sz( sequences.unguarded, 0 );
}

/**
 * @fn void T_case_body_BspSparcLeon3ValGr712rc( void )
 */
T_TEST_CASE( BspSparcLeon3ValGr712rc )
{
  BspSparcLeon3ValGr712rc_Action_0();
  BspSparcLeon3ValGr712rc_Action_1();
  BspSparcLeon3ValGr712rc_Action_2();
  BspSparcLeon3ValGr712rc_Action_3();
}

/** @} */
