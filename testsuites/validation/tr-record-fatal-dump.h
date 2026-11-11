/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordValFatalDump
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

#ifndef _TR_RECORD_FATAL_DUMP_H
#define _TR_RECORD_FATAL_DUMP_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RtemsRecordValFatalDump
 *
 * @{
 */

/**
 * @brief Stops the capture of the kernel character output.
 *
 * @param source is the fatal source.
 *
 * @param always_set_to_false is always false.
 *
 * @param code is the fatal code.
 */
void RtemsRecordValFatalDump_StopCapture(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
);

/**
 * @brief Runs the parameterized test case.
 *
 * @param source is fatal source.
 *
 * @param code is fatal code.
 */
void RtemsRecordValFatalDump_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_RECORD_FATAL_DUMP_H */
