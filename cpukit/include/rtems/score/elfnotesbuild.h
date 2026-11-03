/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIELFNotes
 *
 * @brief This header file provides the assembler macro which emits the ELF
 *   notes of the build.
 *
 * The start file of each BSP includes this header file and invokes
 * RTEMS_ELF_NOTES.  The header file includes the version control key header
 * of the build tree.
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

#ifndef _RTEMS_SCORE_ELFNOTESBUILD_H
#define _RTEMS_SCORE_ELFNOTESBUILD_H

#include <rtems/elfnotes.h>
#include <rtems/score/cpuopts.h>
#include <bspopts.h>

#include "version-vc-key.h"

#if defined( ASM )

/*
 * The symbol .L_rtems_elf_note_separator is one after the first option of the
 * build options note.
 */
/* clang-format off */
.macro _RTEMS_ELF_NOTE_OPTION name
.if .L_rtems_elf_note_separator
.ascii " "
.endif
.ascii "RTEMS_\name"
.set .L_rtems_elf_note_separator, 1
.endm

.macro RTEMS_ELF_NOTES
RTEMS_ELF_NOTE_BEGIN RTEMS_ELF_NOTE_VERSION
#if defined(RTEMS_VERSION_CONTROL_KEY)
.ascii RTEMS_VERSION, ".", RTEMS_VERSION_CONTROL_KEY
#else
.ascii RTEMS_VERSION
#endif
RTEMS_ELF_NOTE_END
RTEMS_ELF_NOTE_BEGIN RTEMS_ELF_NOTE_BUILD_OPTIONS
.set .L_rtems_elf_note_separator, 0
#if RTEMS_DEBUG
_RTEMS_ELF_NOTE_OPTION DEBUG
#endif
#if RTEMS_MULTIPROCESSING
_RTEMS_ELF_NOTE_OPTION MULTIPROCESSING
#endif
#if RTEMS_NETWORKING
_RTEMS_ELF_NOTE_OPTION NETWORKING
#endif
#if RTEMS_PARAVIRT
_RTEMS_ELF_NOTE_OPTION PARAVIRT
#endif
#if RTEMS_POSIX_API
_RTEMS_ELF_NOTE_OPTION POSIX_API
#endif
#if RTEMS_PROFILING
_RTEMS_ELF_NOTE_OPTION PROFILING
#endif
#if RTEMS_SMP
_RTEMS_ELF_NOTE_OPTION SMP
#endif
RTEMS_ELF_NOTE_END
RTEMS_ELF_NOTE_BEGIN RTEMS_ELF_NOTE_TOOLS
.ascii __VERSION__
RTEMS_ELF_NOTE_END
RTEMS_ELF_NOTE_BEGIN RTEMS_ELF_NOTE_ARCH_BSP
.ascii RTEMS_ARCH_BSP
RTEMS_ELF_NOTE_END
RTEMS_ELF_NOTE_BEGIN RTEMS_ELF_NOTE_BUILD_LABEL
.ascii RTEMS_BUILD_LABEL
RTEMS_ELF_NOTE_END
.endm
/* clang-format on */

#endif /* ASM */

#endif /* _RTEMS_SCORE_ELFNOTESBUILD_H */
