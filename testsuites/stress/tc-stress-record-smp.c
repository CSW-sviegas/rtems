/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordValStressFetchSmp
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

#include <inttypes.h>
#include <rtems.h>
#include <stdatomic.h>
#include <rtems/record.h>
#include <sys/cpuset.h>
#include <rtems/score/percpu.h>
#include <rtems/score/watchdogimpl.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsRecordValStressFetchSmp \
 *   spec:/rtems/record/val/stress-fetch-smp
 *
 * @ingroup TestsuitesStressRecordSmp
 *
 * @brief Stresses rtems_record_fetch() with a producer on each other
 *   processor.
 *
 * A fetch of the record items competes with the producers on the other
 * processors. A producer can overwrite an item between two fetches or while a
 * fetch copies it. This test checks that rtems_record_fetch() returns each
 * item either unchanged or counts it as lost.
 *
 * The test runs one producer on each processor except processor 0, up to 32
 * processors. Each producer is pinned to its processor and produces items of
 * the event RTEMS_RECORD_USER_0 with the event data 0, 1, 2 and so on. It
 * produces with interrupts disabled, so that no other item enters its record
 * buffer. The test removes the uptime watchdogs for the same reason. The
 * producers use rtems_record_produce(), so each production consists of one
 * item. The record buffer has 16 items.
 *
 * The runner fetches the items of all processors on processor 0. The load
 * consists of four rounds of 10 s. The rounds use item arrays of 3, 4 and 9
 * items and of the item count which rtems_record_get_item_count_for_fetch()
 * returns. A small item array makes the fetch of one processor take several
 * calls. After the rounds, the runner stops the producers and fetches the
 * remaining items.
 *
 * The event data of a fetched item tells its position in the stream of its
 * producer. The test checks that each fetched item follows from the item
 * before it and from the lost items which the overflow items report in
 * between. An item which a fetch returned twice, out of order or after an
 * overwrite shows up as an order error. An item which got lost without a count
 * shows up as an order error as well. At the end, the fetched items and the
 * lost items of each producer must add up to its produced items.
 *
 * The test suite of this test case exists only in an SMP build without the
 * pre- qualified configuration. The build option BUILD_STRESS_TESTS enables
 * it.
 *
 * This test case performs the following actions:
 *
 * - Start one producer on each processor except processor 0. Fetch the record
 *   items of all processors with an item array of each item count of the
 *   rounds. Stop the producers. Fetch the remaining record items.
 *
 *   - Check that each call of rtems_record_fetch() returned
 *     RTEMS_RECORD_FETCH_CONTINUE or RTEMS_RECORD_FETCH_DONE. Check that each
 *     fetched item array started with an RTEMS_RECORD_PROCESSOR item, which an
 *     optional RTEMS_RECORD_PER_CPU_OVERFLOW item followed. Check that the
 *     other items were items of the producer of the processor.
 *
 *   - Check that the event data of each fetched item of a producer was the
 *     event data of the item fetched before it, plus one, plus the count of
 *     lost items which the overflow items reported in between.
 *
 *   - Check that the fetched items and the lost items of each producer added
 *     up to the items which the producer produced.
 *
 *   - Check that each round fetched at least one item of a producer.
 *
 *   - Check that the producers overwrote items before the fetches copied them.
 *
 * - Print the figures of the rounds and of the producers.
 *
 * @{
 */

#define PROCESSOR_MAX 32

#define ROUND_COUNT 4

#define ROUND_SECONDS 10

#define ITEM_COUNT_MAX 64

#define PRODUCER_PRIORITY 2

typedef struct {
  /**
   * @brief This member contains the identifier of the producer task.
   */
  rtems_id id;

  /**
   * @brief This member contains the count of items which the producer
   *   produced.
   */
  uintptr_t produced;
} Producer;

typedef struct {
  /**
   * @brief This member contains the event data of the next item.
   */
  uintptr_t next;

  /**
   * @brief This member contains the count of lost items since the last
   *   fetched item.
   */
  uintptr_t pending;

  /**
   * @brief This member contains the count of fetched items.
   */
  uint64_t fetched;

  /**
   * @brief This member contains the count of lost items.
   */
  uint64_t lost;
} Processor;

typedef struct {
  /**
   * @brief This member contains the item count of the item array.
   */
  size_t item_count;

  /**
   * @brief This member contains the count of fetches.
   */
  uint64_t fetches;

  /**
   * @brief This member contains the count of fetched items of producers.
   */
  uint64_t items;

  /**
   * @brief This member contains the count of lost items.
   */
  uint64_t lost;
} Round;

/**
 * @brief Test context for spec:/rtems/record/val/stress-fetch-smp test case.
 */
typedef struct {
  /**
   * @brief This member contains the context of each producer.
   */
  Producer producers[ PROCESSOR_MAX - 1 ];

  /**
   * @brief This member contains the count of producers.
   */
  uint32_t producer_count;

  /**
   * @brief This member contains the fetch state of each processor.
   */
  Processor processors[ PROCESSOR_MAX ];

  /**
   * @brief This member contains the figures of each round.
   */
  Round rounds[ ROUND_COUNT ];

  /**
   * @brief This member is true, while the producers shall produce.
   */
  atomic_uint run;

  /**
   * @brief This member contains the count of producers which stopped.
   */
  atomic_uint acks;

  /**
   * @brief This member contains the record fetch control.
   */
  rtems_record_fetch_control control;

  /**
   * @brief This member provides the item array of the fetches.
   */
  rtems_record_item items[ ITEM_COUNT_MAX ];

  /**
   * @brief This member contains the count of fetches which returned an
   *   unexpected status or an unexpected item.
   */
  uint32_t format_errors;

  /**
   * @brief This member contains the count of fetched items of a producer whose
   *   event data did not follow from the item fetched before it.
   */
  uint32_t order_errors;

  /**
   * @brief This member contains the expiration time of the uptime watchdog of
   *   each processor.
   */
  uint64_t expire[ PROCESSOR_MAX ];

  /**
   * @brief This member contains the processor affinity of the runner before
   *   the test.
   */
  cpu_set_t affinity;
} RtemsRecordValStressFetchSmp_Context;

static RtemsRecordValStressFetchSmp_Context
  RtemsRecordValStressFetchSmp_Instance;

typedef RtemsRecordValStressFetchSmp_Context Context;

static void ProducerBody( rtems_task_argument arg )
{
  Context              *ctx;
  rtems_interrupt_level level;
  uintptr_t             data;

  ctx = T_fixture_context();
  data = 0;

  rtems_interrupt_local_disable( level );

  while ( atomic_load_explicit( &ctx->run, memory_order_relaxed ) != 0 ) {
    rtems_record_produce( RTEMS_RECORD_USER_0, data );
    ++data;
  }

  rtems_interrupt_local_enable( level );

  ctx->producers[ arg ].produced = data;
  atomic_fetch_add_explicit( &ctx->acks, 1, memory_order_release );
  (void) rtems_task_suspend( RTEMS_SELF );
}

static void Process( Context *ctx, Round *round )
{
  const rtems_record_item *items;
  size_t                   count;
  size_t                   i;
  uint32_t                 cpu_index;
  Processor               *processor;

  items = ctx->control.fetched_items;
  count = ctx->control.fetched_count;

  if (
    count == 0 ||
    RTEMS_RECORD_GET_EVENT( items[ 0 ].event ) != RTEMS_RECORD_PROCESSOR ||
    items[ 0 ].data > ctx->producer_count
  ) {
    ++ctx->format_errors;
    return;
  }

  cpu_index = (uint32_t) items[ 0 ].data;
  processor = &ctx->processors[ cpu_index ];

  for ( i = 1; i < count; ++i ) {
    rtems_record_event event;
    uintptr_t          data;

    event = RTEMS_RECORD_GET_EVENT( items[ i ].event );
    data = items[ i ].data;

    if ( event == RTEMS_RECORD_PER_CPU_OVERFLOW && i == 1 ) {
      processor->pending += data;
      processor->lost += data;
      round->lost += data;
    } else if ( event == RTEMS_RECORD_USER_0 && cpu_index != 0 ) {
      if ( data != processor->next + processor->pending ) {
        ++ctx->order_errors;
      }

      processor->next = data + 1;
      processor->pending = 0;
      ++processor->fetched;
      ++round->items;
    } else {
      ++ctx->format_errors;
    }
  }
}

static rtems_record_fetch_status Fetch( Context *ctx, Round *round )
{
  rtems_record_fetch_status status;

  status = rtems_record_fetch( &ctx->control );
  ++round->fetches;

  if (
    status == RTEMS_RECORD_FETCH_CONTINUE || status == RTEMS_RECORD_FETCH_DONE
  ) {
    Process( ctx, round );
  } else {
    ++ctx->format_errors;
  }

  return status;
}

static void RunRound( Context *ctx, Round *round )
{
  uint64_t end;

  rtems_record_fetch_initialize(
    &ctx->control,
    &ctx->items[ 0 ],
    round->item_count
  );
  end = rtems_clock_get_uptime_nanoseconds() +
        UINT64_C( 1000000000 ) * ROUND_SECONDS;

  while ( rtems_clock_get_uptime_nanoseconds() < end ) {
    (void) Fetch( ctx, round );
  }
}

static void Drain( Context *ctx, Round *round )
{
  bool fetched;

  fetched = false;

  while ( true ) {
    rtems_record_fetch_status status;

    status = Fetch( ctx, round );

    if ( ctx->control.fetched_count > 1 ) {
      fetched = true;
    }

    if ( status != RTEMS_RECORD_FETCH_CONTINUE ) {
      if ( status != RTEMS_RECORD_FETCH_DONE || !fetched ) {
        break;
      }

      fetched = false;
    }
  }
}

static uint32_t CountBalanceErrors( const Context *ctx )
{
  uint32_t errors;
  uint32_t i;

  errors = 0;

  for ( i = 0; i < ctx->producer_count; ++i ) {
    const Processor *processor;

    processor = &ctx->processors[ i + 1 ];

    if (
      processor->next + processor->pending != ctx->producers[ i ].produced
    ) {
      ++errors;
    }
  }

  return errors;
}

static uint32_t CountIdleRounds( const Context *ctx )
{
  uint32_t idle;
  size_t   i;

  idle = 0;

  for ( i = 0; i < ROUND_COUNT; ++i ) {
    if ( ctx->producer_count > 0 && ctx->rounds[ i ].items == 0 ) {
      ++idle;
    }
  }

  return idle;
}

static uint64_t CountLostItems( const Context *ctx )
{
  uint64_t lost;
  size_t   i;

  lost = 0;

  for ( i = 0; i < ROUND_COUNT; ++i ) {
    lost += ctx->rounds[ i ].lost;
  }

  return lost;
}

static void PrintFigures( const Context *ctx )
{
  uint32_t i;

  for ( i = 0; i < ROUND_COUNT; ++i ) {
    const Round *round;

    round = &ctx->rounds[ i ];
    T_printf(
      "round %" PRIu32 ": item count %zu, fetches %" PRIu64
      ", fetched %" PRIu64 ", lost %" PRIu64 "\n",
      i,
      round->item_count,
      round->fetches,
      round->items,
      round->lost
    );
  }

  for ( i = 0; i < ctx->producer_count; ++i ) {
    const Processor *processor;

    processor = &ctx->processors[ i + 1 ];
    T_printf(
      "processor %" PRIu32 ": produced %" PRIuPTR ", fetched %" PRIu64
      ", lost %" PRIu64 "\n",
      i + 1,
      ctx->producers[ i ].produced,
      processor->fetched,
      processor->lost
    );
  }
}

static void RtemsRecordValStressFetchSmp_Setup(
  RtemsRecordValStressFetchSmp_Context *ctx
)
{
  rtems_status_code sc;
  cpu_set_t         set;
  uint32_t          n;
  uint32_t          i;

  n = rtems_scheduler_get_processor_maximum();
  T_assert_le_u32( n, PROCESSOR_MAX );
  T_assert_le_sz( rtems_record_get_item_count_for_fetch(), ITEM_COUNT_MAX );
  ctx->producer_count = n - 1;
  ctx->rounds[ 0 ].item_count = 3;
  ctx->rounds[ 1 ].item_count = 4;
  ctx->rounds[ 2 ].item_count = 9;
  ctx->rounds[ 3 ].item_count = rtems_record_get_item_count_for_fetch();

  sc = rtems_task_get_affinity(
    RTEMS_SELF,
    sizeof( ctx->affinity ),
    &ctx->affinity
  );
  T_assert_rsc_success( sc );
  CPU_ZERO( &set );
  CPU_SET( 0, &set );
  sc = rtems_task_set_affinity( RTEMS_SELF, sizeof( set ), &set );
  T_assert_rsc_success( sc );

  for ( i = 0; i < n; ++i ) {
    rtems_interrupt_level level;
    Record_Control       *control;

    control = _Per_CPU_Get_by_index( i )->record;
    rtems_interrupt_local_disable( level );
    _Watchdog_Per_CPU_remove_ticks( &control->Watchdog );
    ctx->expire[ i ] = control->Watchdog.expire;
    rtems_interrupt_local_enable( level );
    control->tail = _Record_Head( control );
  }

  for ( i = 0; i < ctx->producer_count; ++i ) {
    Producer *p;

    p = &ctx->producers[ i ];
    sc = rtems_task_create(
      rtems_build_name( 'P', 'R', 'O', 'D' ),
      PRODUCER_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &p->id
    );
    T_assert_rsc_success( sc );

    CPU_ZERO( &set );
    CPU_SET( (int) ( i + 1 ), &set );
    sc = rtems_task_set_affinity( p->id, sizeof( set ), &set );
    T_assert_rsc_success( sc );
  }
}

static void RtemsRecordValStressFetchSmp_Setup_Wrap( void *arg )
{
  RtemsRecordValStressFetchSmp_Context *ctx;

  ctx = arg;
  RtemsRecordValStressFetchSmp_Setup( ctx );
}

static void RtemsRecordValStressFetchSmp_Teardown(
  RtemsRecordValStressFetchSmp_Context *ctx
)
{
  uint32_t i;

  for ( i = 0; i < ctx->producer_count; ++i ) {
    if ( ctx->producers[ i ].id != 0 ) {
      (void) rtems_task_delete( ctx->producers[ i ].id );
    }
  }

  for ( i = 0; i <= ctx->producer_count; ++i ) {
    rtems_interrupt_level level;
    Per_CPU_Control      *cpu;

    cpu = _Per_CPU_Get_by_index( i );
    rtems_interrupt_local_disable( level );
    (void) _Watchdog_Per_CPU_insert(
      &cpu->record->Watchdog,
      cpu,
      &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
      ctx->expire[ i ]
    );
    rtems_interrupt_local_enable( level );
  }

  (void) rtems_task_set_affinity(
    RTEMS_SELF,
    sizeof( ctx->affinity ),
    &ctx->affinity
  );
}

static void RtemsRecordValStressFetchSmp_Teardown_Wrap( void *arg )
{
  RtemsRecordValStressFetchSmp_Context *ctx;

  ctx = arg;
  RtemsRecordValStressFetchSmp_Teardown( ctx );
}

static T_fixture RtemsRecordValStressFetchSmp_Fixture = {
  .setup = RtemsRecordValStressFetchSmp_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRecordValStressFetchSmp_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsRecordValStressFetchSmp_Instance
};

/**
 * @brief Start one producer on each processor except processor 0. Fetch the
 *   record items of all processors with an item array of each item count of
 *   the rounds. Stop the producers. Fetch the remaining record items.
 */
static void RtemsRecordValStressFetchSmp_Action_0(
  RtemsRecordValStressFetchSmp_Context *ctx
)
{
  rtems_status_code sc;
  uint32_t          i;

  atomic_store_explicit( &ctx->run, 1, memory_order_relaxed );

  for ( i = 0; i < ctx->producer_count; ++i ) {
    sc = rtems_task_start( ctx->producers[ i ].id, ProducerBody, i );
    T_assert_rsc_success( sc );
  }

  for ( i = 0; i < ROUND_COUNT; ++i ) {
    RunRound( ctx, &ctx->rounds[ i ] );
  }

  atomic_store_explicit( &ctx->run, 0, memory_order_relaxed );

  while (
    atomic_load_explicit( &ctx->acks, memory_order_acquire ) !=
    ctx->producer_count
  ) {
    /* Wait for the producers */
  }

  Drain( ctx, &ctx->rounds[ ROUND_COUNT - 1 ] );

  /*
   * Check that each call of rtems_record_fetch() returned
   * RTEMS_RECORD_FETCH_CONTINUE or RTEMS_RECORD_FETCH_DONE. Check that each
   * fetched item array started with an RTEMS_RECORD_PROCESSOR item, which an
   * optional RTEMS_RECORD_PER_CPU_OVERFLOW item followed. Check that the other
   * items were items of the producer of the processor.
   */
  T_eq_u32( ctx->format_errors, 0 );

  /*
   * Check that the event data of each fetched item of a producer was the event
   * data of the item fetched before it, plus one, plus the count of lost items
   * which the overflow items reported in between.
   */
  T_eq_u32( ctx->order_errors, 0 );

  /*
   * Check that the fetched items and the lost items of each producer added up
   * to the items which the producer produced.
   */
  T_eq_u32( CountBalanceErrors( ctx ), 0 );

  /*
   * Check that each round fetched at least one item of a producer.
   */
  T_eq_u32( CountIdleRounds( ctx ), 0 );

  /*
   * Check that the producers overwrote items before the fetches copied them.
   */
  T_true( ctx->producer_count == 0 || CountLostItems( ctx ) > 0 );
}

/**
 * @brief Print the figures of the rounds and of the producers.
 */
static void RtemsRecordValStressFetchSmp_Action_1(
  RtemsRecordValStressFetchSmp_Context *ctx
)
{
  PrintFigures( ctx );
}

/**
 * @fn void T_case_body_RtemsRecordValStressFetchSmp( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsRecordValStressFetchSmp,
  &RtemsRecordValStressFetchSmp_Fixture
)
{
  RtemsRecordValStressFetchSmp_Context *ctx;

  ctx = T_fixture_context();

  RtemsRecordValStressFetchSmp_Action_0( ctx );
  RtemsRecordValStressFetchSmp_Action_1( ctx );
}

/** @} */
