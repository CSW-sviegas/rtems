/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordValUptimeLongTick
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
#include <rtems/record.h>
#include <sys/time.h>
#include <rtems/score/percpu.h>
#include <rtems/score/timecounter.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsRecordValUptimeLongTick \
 *   spec:/rtems/record/val/uptime-long-tick
 *
 * @ingroup TestsuitesValidationRecord3
 *
 * @brief Tests the periodic uptime items of the event recording with a clock
 *   tick which lasts longer than two to the power of RTEMS_RECORD_TIME_BITS
 *   counter ticks.
 *
 * This test case performs the following actions:
 *
 * - Remove the items of each processor. Wait for four clock ticks. Fetch the
 *   items of each processor.
 *
 *   - Check that a clock tick of the test suite lasts longer than two to the
 *     power of RTEMS_RECORD_TIME_BITS counter ticks.
 *
 *   - Check that each processor produced at least two uptime item pairs and
 *     that the uptimes of consecutive pairs differ by one clock tick.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/record/val/uptime-long-tick test case.
 */
typedef struct {
  /**
   * @brief This member contains the record fetch control.
   */
  rtems_record_fetch_control fetch;

  /**
   * @brief This member provides the storage of the record fetch control.
   */
  rtems_record_item fetch_storage[ 64 ];
} RtemsRecordValUptimeLongTick_Context;

static RtemsRecordValUptimeLongTick_Context
  RtemsRecordValUptimeLongTick_Instance;

typedef RtemsRecordValUptimeLongTick_Context Context;

/*
 * Get the largest count of clock ticks which lasts at most two to the power of
 * RTEMS_RECORD_TIME_BITS counter ticks.
 */
static uint64_t TicksWithinTimeBits( void )
{
  uint64_t interval;

  interval = UINT64_C( 1 ) << RTEMS_RECORD_TIME_BITS;
  interval *= rtems_clock_get_ticks_per_second();
  interval /= rtems_counter_frequency();

  return interval;
}

static rtems_interval ClockTicks( sbintime_t duration )
{
  sbintime_t tick;

  tick = SBT_1S / rtems_clock_get_ticks_per_second();

  return (rtems_interval) ( ( duration + tick / 2 ) / tick );
}

static void CheckProcessor(
  Context   *ctx,
  uint32_t   cpu_index,
  sbintime_t before,
  sbintime_t after
)
{
  rtems_record_fetch_status status;
  size_t                    pairs;
  size_t                    i;
  sbintime_t                previous;

  rtems_record_fetch_initialize(
    &ctx->fetch,
    ctx->fetch_storage,
    RTEMS_ARRAY_SIZE( ctx->fetch_storage )
  );
#if defined( RTEMS_SMP )
  ctx->fetch.internal.cpu_index = cpu_index;
#else
  (void) cpu_index;
#endif
  status = rtems_record_fetch( &ctx->fetch );
  T_ne_int( status, RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT );
  pairs = 0;
  previous = 0;

  for ( i = 1; i + 1 < ctx->fetch.fetched_count; i += 2 ) {
    const rtems_record_item *items;
    sbintime_t               uptime;

    items = &ctx->fetch.fetched_items[ i ];
    T_eq_u32(
      RTEMS_RECORD_GET_EVENT( items[ 0 ].event ),
      RTEMS_RECORD_UPTIME_LOW
    );
    T_eq_u32(
      RTEMS_RECORD_GET_EVENT( items[ 1 ].event ),
      RTEMS_RECORD_UPTIME_HIGH
    );
    uptime = (sbintime_t) ( ( (uint64_t) items[ 1 ].data << 32 ) |
                            (uint32_t) items[ 0 ].data );
    T_ge_ull( (unsigned long long) uptime, (unsigned long long) before );
    T_le_ull( (unsigned long long) uptime, (unsigned long long) after );

    if ( pairs > 0 ) {
      T_eq_u32( ClockTicks( uptime - previous ), 1 );
    }

    previous = uptime;
    ++pairs;
  }

  T_ge_sz( pairs, 2 );
}

static T_fixture RtemsRecordValUptimeLongTick_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsRecordValUptimeLongTick_Instance
};

/**
 * @brief Remove the items of each processor. Wait for four clock ticks. Fetch
 *   the items of each processor.
 */
static void RtemsRecordValUptimeLongTick_Action_0(
  RtemsRecordValUptimeLongTick_Context *ctx
)
{
  sbintime_t before;
  sbintime_t after;
  uint32_t   cpu_index;
  uint32_t   cpu_max;

  cpu_max = rtems_scheduler_get_processor_maximum();

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    Record_Control *control;

    control = _Per_CPU_Get_by_index( cpu_index )->record;
    control->tail = _Record_Head( control );
  }

  before = _Timecounter_Sbinuptime();
  (void) rtems_task_wake_after( 4 );
  after = _Timecounter_Sbinuptime();

  /*
   * Check that a clock tick of the test suite lasts longer than two to the
   * power of RTEMS_RECORD_TIME_BITS counter ticks.
   */
  T_eq_u64( TicksWithinTimeBits(), 0 );

  /*
   * Check that each processor produced at least two uptime item pairs and that
   * the uptimes of consecutive pairs differ by one clock tick.
   */
  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    CheckProcessor( ctx, cpu_index, before, after );
  }
}

/**
 * @fn void T_case_body_RtemsRecordValUptimeLongTick( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsRecordValUptimeLongTick,
  &RtemsRecordValUptimeLongTick_Fixture
)
{
  RtemsRecordValUptimeLongTick_Context *ctx;

  ctx = T_fixture_context();

  RtemsRecordValUptimeLongTick_Action_0( ctx );
}

/** @} */
