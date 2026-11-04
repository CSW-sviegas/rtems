/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsCpuuseValResetPointInit
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
#include <time.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/timespec.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsCpuuseValResetPointInit \
 *   spec:/rtems/cpuuse/val/reset-point-init
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @brief Tests the CPU usage reset point which the system initialization
 *   establishes and the start of the processor time accounting.
 *
 * No test case of the test suite may call rtems_cpu_usage_reset().
 *
 * This test case performs the following actions:
 *
 * - Read the uptime and the processor time of the executing thread. Restore
 *   the counter of the software timecounter after each read.
 *
 *   - Check that the time of the CLOCK_PROCESS_CPUTIME_ID clock is equal to
 *     the uptime.
 *
 *   - Check that the time of the CLOCK_THREAD_CPUTIME_ID clock is equal to the
 *     processor time which the executing thread used since its creation.
 *
 *   - Check that the sum of the processor time which each thread used since
 *     its creation is less than or equal to the processor maximum times the
 *     uptime. Read the uptime after the sum.
 *
 * @{
 */

static bool SumProcessorTimeUsed( rtems_tcb *thread, void *arg )
{
  Timestamp_Control *sum;

  sum = arg;
  *sum += _Thread_Get_CPU_time_used( thread );
  return false;
}

/**
 * @brief Read the uptime and the processor time of the executing thread.
 *   Restore the counter of the software timecounter after each read.
 */
static void RtemsCpuuseValResetPointInit_Action_0( void )
{
  Thread_Control   *self;
  Timestamp_Control used;
  struct timespec   uptime;
  struct timespec   expected;
  struct timespec   ts;
  uint32_t          counter;

  self = GetThread( RTEMS_SELF );
  counter = GetTimecountCounter();
  T_rsc_success( rtems_clock_get_uptime( &uptime ) );
  SetTimecountCounter( counter );
  used = _Thread_Get_CPU_time_used( self );
  SetTimecountCounter( counter );

  /*
   * Check that the time of the CLOCK_PROCESS_CPUTIME_ID clock is equal to the
   * uptime.
   */
  T_eq_int( clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), 0 );
  SetTimecountCounter( counter );
  T_eq_i64( ts.tv_sec, uptime.tv_sec );
  T_eq_long( ts.tv_nsec, uptime.tv_nsec );

  /*
   * Check that the time of the CLOCK_THREAD_CPUTIME_ID clock is equal to the
   * processor time which the executing thread used since its creation.
   */
  T_eq_int( clock_gettime( CLOCK_THREAD_CPUTIME_ID, &ts ), 0 );
  SetTimecountCounter( counter );
  _Timestamp_To_timespec( &used, &expected );
  T_eq_i64( ts.tv_sec, expected.tv_sec );
  T_eq_long( ts.tv_nsec, expected.tv_nsec );

  /*
   * Check that the sum of the processor time which each thread used since its
   * creation is less than or equal to the processor maximum times the uptime.
   * Read the uptime after the sum.
   */
  Timestamp_Control sum;

  sum = 0;
  rtems_task_iterate( SumProcessorTimeUsed, &sum );
  T_rsc_success( rtems_clock_get_uptime( &uptime ) );
  T_le_u64(
    _Timestamp_Get_as_nanoseconds( &sum ),
    rtems_scheduler_get_processor_maximum() *
      _Timespec_Get_as_nanoseconds( &uptime )
  );
}

/**
 * @fn void T_case_body_RtemsCpuuseValResetPointInit( void )
 */
T_TEST_CASE( RtemsCpuuseValResetPointInit )
{
  RtemsCpuuseValResetPointInit_Action_0();
}

/** @} */
