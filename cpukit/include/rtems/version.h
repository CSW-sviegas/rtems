/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief This header file provides the Version API.
 */

/*
 * Copyright (C) 2025, 2026 embedded brains GmbH & Co. KG
 * Copyright (C) 2017 Chris Johns
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

/*
 * The spec:/... comments allow you to identify content which is
 * duplicated in the rtems and rtems-docs repositories.
 */

/* Find related documentation with spec:/rtems/version/if/header */

#ifndef _RTEMS_VERSION_H
#define _RTEMS_VERSION_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Find related documentation with spec:/rtems/version/if/group */

/**
 * @defgroup RTEMSAPIClassicVersion Version
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Version API provides functions to return the version or parts of
 *   the version of RTEMS which the application uses.
 *
 * A `VERSION` file in the top directory of the sources sets the version
 * numbers and the release label. The release procedure of the RTEMS Project
 * generates this file, and the repository holds none.
 *
 * Without a `VERSION` file, the version numbers are those of the build system.
 * Where no `VERSION` file exists or the file sets no release label, the build
 * takes the release label from the Git commit of the sources. Sources outside
 * a Git work tree get the release label `not-released`.
 *
 * The engineering manual reserves the empty release label for the releases of
 * the RTEMS Project. A user or a vendor who deploys modified sources sets a
 * unique release label.
 */

/* Find related documentation with spec:/rtems/version/if/board-support-package */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns the board support package name.
 *
 * The name is the name of the BSP which the build configuration selects.
 *
 * @return The board support package name.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
const char *rtems_board_support_package( void );

/* Find related documentation with spec:/rtems/version/if/control-key */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns the release label for the current version of code that has
 *   been built.
 *
 * The directive returns the same string as rtems_version_release_label().
 *
 * @return The release label.
 *
 * @par Notes
 * This directive is deprecated. Use rtems_version_release_label() instead.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
const char *rtems_version_control_key( void );

/* Find related documentation with spec:/rtems/version/if/control-key-is-valid */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns true, if the version control key is valid, otherwise false.
 *
 * @param[in] key is the pointer to the version control key string to check.
 *
 * A version control key is valid, if it is not empty.
 *
 * @retval true The version control key is valid.
 *
 * @retval false Otherwise.
 *
 * @par Notes
 * This directive is deprecated. Use rtems_version_release_label_is_valid()
 * instead.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
static inline bool rtems_version_control_key_is_valid( const char *key )
{
  return key[ 0 ] != '\0';
}

/* Find related documentation with spec:/rtems/version/if/major */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns the version's major number.
 *
 * @return Returns the major number of the RTEMS version.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
int rtems_version_major( void );

/* Find related documentation with spec:/rtems/version/if/minor */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns the version's minor number.
 *
 * @return Returns the minor number of the RTEMS version.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
int rtems_version_minor( void );

/* Find related documentation with spec:/rtems/version/if/release-label */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns the release label for the current version of code that has
 *   been built.
 *
 * The release label is not empty, unless a `VERSION` file sets an empty label.
 *
 * @return The release label.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
const char *rtems_version_release_label( void );

/* Find related documentation with spec:/rtems/version/if/release-label-is-valid */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns true, if the release label is valid, otherwise false.
 *
 * A release label is valid, if it is not empty.
 *
 * @retval true The release label is valid.
 *
 * @retval false Otherwise.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
bool rtems_version_release_label_is_valid( void );

/* Find related documentation with spec:/rtems/version/if/revision */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns the version's revision number.
 *
 * @return Returns the revision of the RTEMS version.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
int rtems_version_revision( void );

/* Find related documentation with spec:/rtems/version/if/version */

/**
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief Returns the version string.
 *
 * The version string consists of the major version number, the minor version
 * number and the revision in decimal digits, separated by `.` characters.
 * Where the release label is not empty, a `.` character and the label follow.
 *
 * @return Returns the version string of RTEMS.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
const char *rtems_version( void );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_VERSION_H */
