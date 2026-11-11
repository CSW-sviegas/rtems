/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesValidationRecord3
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

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/test-info.h>
#include <rtems/test.h>
#include <rtems/testopts.h>

#include "ts-record.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesValidationRecord3 spec:/testsuites/validation-record-3
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite provides a configuration with the event
 *   recording enabled and a clock tick of one second.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesValidationRecord3";

static char buffer[ 512 ];

static const T_action actions[] = { T_report_hash_sha256 };

static const T_config test_config = {
  .name = rtems_test_name,
  .buf = buffer,
  .buf_size = sizeof( buffer ),
  .putchar = rtems_put_char,
  .verbosity = RTEMS_TEST_VERBOSITY,
  .now = T_now_clock,
  .allocate = T_memory_allocate,
  .deallocate = T_memory_deallocate,
  .action_count = T_ARRAY_SIZE( actions ),
  .actions = actions
};

static void Init( rtems_task_argument arg )
{
  int exit_code;

  (void) arg;

  rtems_test_begin( rtems_test_name, TEST_STATE );
  T_register();
  exit_code = T_main( &test_config );

  if ( exit_code == 0 ) {
    rtems_test_end( rtems_test_name );
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, (uint32_t) exit_code );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000000

#define CONFIGURE_MAXIMUM_PROCESSORS 4

#define CONFIGURE_RECORD_PER_PROCESSOR_ITEMS TEST_RECORD_PER_PROCESSOR_ITEMS

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE \
  RTEMS_TASK_STORAGE_SIZE(                         \
    2 * RTEMS_MINIMUM_STACK_SIZE,                  \
    RTEMS_DEFAULT_ATTRIBUTES                       \
  )

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/** @} */
