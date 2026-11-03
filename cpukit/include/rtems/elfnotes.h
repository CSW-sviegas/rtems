/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIELFNotes
 *
 * @brief This header file provides the interfaces of the RTEMS ELF notes.
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

#ifndef _RTEMS_ELFNOTES_H
#define _RTEMS_ELFNOTES_H

/**
 * @defgroup RTEMSAPIELFNotes ELF Notes
 *
 * @ingroup RTEMSAPI
 *
 * @brief The RTEMS ELF notes describe the build of an executable.
 *
 * The notes reside in the section ``.note.rtems``.  This section is not
 * allocated, so the notes are not part of the loaded image.  The owner name
 * of each note is ``RTEMS``.  The descriptor of each note is a string ended
 * by a NUL character.
 *
 * @{
 */

/**
 * @brief This note type indicates the RTEMS version.
 *
 * The descriptor is the string returned by rtems_version().
 */
#define RTEMS_ELF_NOTE_VERSION 1

/**
 * @brief This note type indicates the enabled build options.
 *
 * The descriptor is the list of the enabled options of ``RTEMS_DEBUG``,
 * ``RTEMS_MULTIPROCESSING``, ``RTEMS_NETWORKING``, ``RTEMS_PARAVIRT``,
 * ``RTEMS_POSIX_API``, ``RTEMS_PROFILING``, and ``RTEMS_SMP`` in this order.
 * One space character separates two options.
 */
#define RTEMS_ELF_NOTE_BUILD_OPTIONS 2

/**
 * @brief This note type indicates the tools.
 *
 * The descriptor is the value of ``__VERSION__``.
 */
#define RTEMS_ELF_NOTE_TOOLS 3

/**
 * @brief This note type indicates the architecture and the BSP.
 *
 * The descriptor has the form ``<arch>/<bsp>``.
 */
#define RTEMS_ELF_NOTE_ARCH_BSP 4

/**
 * @brief This note type indicates the build label.
 *
 * The descriptor is the string returned by rtems_get_build_label().
 */
#define RTEMS_ELF_NOTE_BUILD_LABEL 5

/**
 * @brief This note type indicates the test state of a test program.
 *
 * The descriptor is one of ``EXPECTED_PASS``, ``EXPECTED_FAIL``,
 * ``USER_INPUT``, ``INDETERMINATE``, and ``BENCHMARK``.
 */
#define RTEMS_ELF_NOTE_TEST_STATE 6

/**
 * @brief This define expands to the section type of the note section in the
 *   assembler syntax of the target.
 */
/* clang-format off */
#if defined(__arm__)
#define RTEMS_ELF_NOTE_SECTION_TYPE %note
#else
#define RTEMS_ELF_NOTE_SECTION_TYPE @note
#endif
/* clang-format on */

#if defined( ASM )

/*
 * The descriptor of a note follows the RTEMS_ELF_NOTE_BEGIN macro.  The
 * RTEMS_ELF_NOTE_END macro ends the descriptor with a NUL character.
 */
/* clang-format off */
.macro RTEMS_ELF_NOTE_BEGIN type
.pushsection .note.rtems, "", RTEMS_ELF_NOTE_SECTION_TYPE
.balign 4
.long 1002f - 1001f
.long 1004f - 1003f
.long \type
1001: .asciz "RTEMS"
1002: .balign 4
1003:
.endm

.macro RTEMS_ELF_NOTE_END
.byte 0
1004: .balign 4
.popsection
.endm
/* clang-format on */

#else /* ASM */

#define _RTEMS_ELF_NOTE_STRING( _x ) #_x

#define _RTEMS_ELF_NOTE_XSTRING( _x ) _RTEMS_ELF_NOTE_STRING( _x )

/**
 * @brief This define expands to the assembler text which begins a note of
 *   the type.
 *
 * The assembler text of the descriptor shall follow this text.
 * #RTEMS_ELF_NOTE_END_STRING shall follow the descriptor.
 *
 * @param _type is the note type.
 */
/* clang-format off */
#define RTEMS_ELF_NOTE_BEGIN_STRING( _type ) \
  ".pushsection .note.rtems, \"\", " \
  _RTEMS_ELF_NOTE_XSTRING( RTEMS_ELF_NOTE_SECTION_TYPE ) "\n" \
  ".balign 4\n" \
  ".long 1002f - 1001f\n" \
  ".long 1004f - 1003f\n" \
  ".long " _RTEMS_ELF_NOTE_XSTRING( _type ) "\n" \
  "1001: .asciz \"RTEMS\"\n" \
  "1002: .balign 4\n" \
  "1003:\n"
/* clang-format on */

/**
 * @brief This define expands to the assembler text which ends a note.
 */
#define RTEMS_ELF_NOTE_END_STRING \
  ".byte 0\n"                     \
  "1004: .balign 4\n"             \
  ".popsection\n"

#endif /* ASM */

/** @} */

#endif /* _RTEMS_ELFNOTES_H */
