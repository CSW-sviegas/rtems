/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsVersionValVersion
 */

/*
 * Copyright (C) 2025, 2026 embedded brains GmbH & Co. KG
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

#include <bspopts.h>
#include <version-vc-key.h>
#include <rtems/version.h>
#include <rtems/score/basedefs.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsVersionValVersion spec:/rtems/version/val/version
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the RTEMS version API.
 *
 * This test case performs the following actions:
 *
 * - Call the version directives.
 *
 *   - Check that rtems_board_support_package() returns the name of the BSP of
 *     the build configuration.
 *
 *   - Check that rtems_version_control_key() returns the release label of the
 *     build.
 *
 *   - Check that rtems_version_release_label() returns the release label of
 *     the build.
 *
 *   - Check that rtems_version_major() returns 7.
 *
 *   - Check that rtems_version_minor() returns 0.
 *
 *   - Check that rtems_version_revision() returns 0.
 *
 * @{
 */

#if !defined( RTEMS_VERSION_CONTROL_KEY )
#error "the release label of the build is empty"
#endif

#define RELEASE_LABEL RTEMS_VERSION_CONTROL_KEY

/**
 * @brief Call the version directives.
 */
static void RtemsVersionValVersion_Action_0( void )
{
  /* Nothing to do */

  /*
   * Check that rtems_board_support_package() returns the name of the BSP of
   * the build configuration.
   */
  T_eq_str( rtems_board_support_package(), RTEMS_XSTRING( RTEMS_BSP ) );

  /*
   * Check that rtems_version_control_key() returns the release label of the
   * build.
   */
  T_eq_str( rtems_version_control_key(), RELEASE_LABEL );

  /*
   * Check that rtems_version_release_label() returns the release label of the
   * build.
   */
  T_eq_str( rtems_version_release_label(), RELEASE_LABEL );

  /*
   * Check that rtems_version_major() returns 7.
   */
  T_eq_int( rtems_version_major(), 7 );

  /*
   * Check that rtems_version_minor() returns 0.
   */
  T_eq_int( rtems_version_minor(), 0 );

  /*
   * Check that rtems_version_revision() returns 0.
   */
  T_eq_int( rtems_version_revision(), 0 );
}

/**
 * @fn void T_case_body_RtemsVersionValVersion( void )
 */
T_TEST_CASE( RtemsVersionValVersion )
{
  RtemsVersionValVersion_Action_0();
}

/** @} */
