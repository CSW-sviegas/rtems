/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesFatalRecordDump
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

#include "tr-record-fatal-dump.h"
#include "ts-record.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesFatalRecordDump spec:/testsuites/fatal-record-dump
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite provides a configuration with the fatal
 *   error dump of the event recording enabled. The system initialization
 *   triggers a fatal error.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesFatalRecordDump";

#define FATAL_SYSINIT_RUN RtemsRecordValFatalDump_Run

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS 4

#define CONFIGURE_RECORD_PER_PROCESSOR_ITEMS TEST_RECORD_PER_PROCESSOR_ITEMS

#define CONFIGURE_RECORD_FATAL_DUMP_BASE64

#define FATAL_SYSINIT_INITIAL_EXTENSION \
  { .fatal = RtemsRecordValFatalDump_StopCapture }

#include "ts-fatal-sysinit.h"

/** @} */
