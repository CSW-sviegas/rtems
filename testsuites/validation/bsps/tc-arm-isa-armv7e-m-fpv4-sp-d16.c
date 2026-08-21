/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuArmValIsaArmv7eMFpv4SpD16
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

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuArmValIsaArmv7eMFpv4SpD16 \
 *   spec:/score/cpu/arm/val/isa-armv7e-m-fpv4-sp-d16
 *
 * @ingroup TestsuitesBspsValidationBspNoClock0
 *
 * @brief Tests the target architecture.
 *
 * This test case performs the following actions:
 *
 * - Read the predefined macros of the compiler which describe the target
 *   architecture.
 *
 *   - Check that the compiler generates code for Armv7E-M with the
 *     floating-point extension FPv4-SP-D16.
 *
 * @{
 */

#if defined( __ARM_FEATURE_DSP )
#define ARCH_DSP __ARM_FEATURE_DSP
#else
#define ARCH_DSP 0
#endif

#if defined( __ARM_FP )
#define ARCH_FP __ARM_FP
#else
#define ARCH_FP 0x0
#endif

#if defined( __ARM_NEON )
#define ARCH_NEON __ARM_NEON
#else
#define ARCH_NEON 0
#endif

/**
 * @brief Read the predefined macros of the compiler which describe the target
 *   architecture.
 */
static void ScoreCpuArmValIsaArmv7eMFpv4SpD16_Action_0( void )
{
  /* Nothing to do */

  /*
   * Check that the compiler generates code for Armv7E-M with the
   * floating-point extension FPv4-SP-D16.
   */
  T_step_eq_int( 0, __ARM_ARCH, 7 );
  T_step_eq_int( 1, __ARM_ARCH_PROFILE, 'M' );
  T_step_eq_int( 2, ARCH_DSP, 1 );
  T_step_eq_int( 3, ARCH_FP, 0x4 );
  T_step_eq_int( 4, ARCH_NEON, 0 );
}

/**
 * @fn void T_case_body_ScoreCpuArmValIsaArmv7eMFpv4SpD16( void )
 */
T_TEST_CASE( ScoreCpuArmValIsaArmv7eMFpv4SpD16 )
{
  T_plan( 5 );

  ScoreCpuArmValIsaArmv7eMFpv4SpD16_Action_0();
}

/** @} */
