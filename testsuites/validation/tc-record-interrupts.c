/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordValInterrupts
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
#include <rtems/score/atomic.h>
#include <rtems/score/percpu.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsRecordValInterrupts spec:/rtems/record/val/interrupts
 *
 * @ingroup TestsuitesValidationRecord2
 *
 * @brief Tests the interrupt items of the event recording.
 *
 * This test case performs the following actions:
 *
 * - Install an interrupt entry for a testable interrupt vector. The handler
 *   produces an item. Enable the vector and raise it. Remove the interrupt
 *   entry.
 *
 *   - Check that the system produced an interrupt entry item with the vector
 *     number before the item of the handler and an interrupt exit item with
 *     the vector number after it.
 *
 * - Enable the testable interrupt vector, which has no interrupt entry, and
 *   raise it. The default interrupt handler of the test replaces the spurious
 *   interrupt fatal error.
 *
 *   - Check that the system produced an interrupt entry item and an interrupt
 *     exit item with the vector number around the call of the default
 *     interrupt handler.
 *
 * - Install an interrupt entry for the testable interrupt vector and enable
 *   the vector. Call rtems_record_interrupt_disable(), raise the vector, and
 *   call rtems_record_interrupt_enable(), so that the interrupt is pending
 *   when the directive restores the interrupt level. Wait until the handler
 *   ran. Remove the interrupt entry. Keep the items of the processor which
 *   called the directives.
 *
 *   - Check that the RTEMS_RECORD_ISR_ENABLE item of
 *     rtems_record_interrupt_enable() precedes each
 *     RTEMS_RECORD_INTERRUPT_ENTRY item of the vector on the processor.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/record/val/interrupts test case.
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

  /**
   * @brief This member contains the items fetched from all configured
   *   processors.
   */
  rtems_record_item items[ 256 ];

  /**
   * @brief This member contains the count of fetched items.
   */
  size_t item_count;

  /**
   * @brief This member contains the count of lost items.
   */
  rtems_record_data overflow;

  /**
   * @brief This member contains the testable interrupt vector.
   */
  rtems_vector_number vector;

  /**
   * @brief This member counts the calls of the interrupt handler.
   */
  Atomic_Uint handler_counter;

  /**
   * @brief This member counts the calls of the default interrupt handler for
   *   the testable interrupt vector.
   */
  uint32_t default_handler_counter;
} RtemsRecordValInterrupts_Context;

static RtemsRecordValInterrupts_Context RtemsRecordValInterrupts_Instance;

typedef RtemsRecordValInterrupts_Context Context;

static void ClearRecords( void )
{
  uint32_t cpu_index;

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    Record_Control *control;

    control = _Per_CPU_Get_by_index( cpu_index )->record;
    control->tail = _Record_Head( control );
  }
}

static void CollectRecords( Context *ctx )
{
  rtems_record_fetch_status status;

  ctx->item_count = 0;
  ctx->overflow = 0;
  rtems_record_fetch_initialize(
    &ctx->fetch,
    ctx->fetch_storage,
    RTEMS_ARRAY_SIZE( ctx->fetch_storage )
  );

  do {
    size_t i;

    status = rtems_record_fetch( &ctx->fetch );

    for ( i = 0; i < ctx->fetch.fetched_count; ++i ) {
      const rtems_record_item *item;
      uint32_t                 event;

      item = &ctx->fetch.fetched_items[ i ];
      event = RTEMS_RECORD_GET_EVENT( item->event );

      if ( event == RTEMS_RECORD_PROCESSOR ) {
        continue;
      }

      if ( event == RTEMS_RECORD_PER_CPU_OVERFLOW ) {
        ctx->overflow += item->data;
        continue;
      }

      if ( ctx->item_count < RTEMS_ARRAY_SIZE( ctx->items ) ) {
        ctx->items[ ctx->item_count ] = *item;
        ++ctx->item_count;
      }
    }
  } while ( status == RTEMS_RECORD_FETCH_CONTINUE );
}

static void CollectProcessorRecords( Context *ctx, uint32_t cpu_index )
{
  rtems_record_fetch_status status;
  bool                      keep;

  ctx->item_count = 0;
  ctx->overflow = 0;
  keep = false;
  rtems_record_fetch_initialize(
    &ctx->fetch,
    ctx->fetch_storage,
    RTEMS_ARRAY_SIZE( ctx->fetch_storage )
  );

  do {
    size_t i;

    status = rtems_record_fetch( &ctx->fetch );

    for ( i = 0; i < ctx->fetch.fetched_count; ++i ) {
      const rtems_record_item *item;
      uint32_t                 event;

      item = &ctx->fetch.fetched_items[ i ];
      event = RTEMS_RECORD_GET_EVENT( item->event );

      if ( event == RTEMS_RECORD_PROCESSOR ) {
        keep = ( item->data == cpu_index );
        continue;
      }

      if ( event == RTEMS_RECORD_PER_CPU_OVERFLOW ) {
        ctx->overflow += item->data;
        continue;
      }

      if ( keep && ctx->item_count < RTEMS_ARRAY_SIZE( ctx->items ) ) {
        ctx->items[ ctx->item_count ] = *item;
        ++ctx->item_count;
      }
    }
  } while ( status == RTEMS_RECORD_FETCH_CONTINUE );
}

static size_t FindEvent( const Context *ctx, rtems_record_event event )
{
  size_t i;

  for ( i = 0; i < ctx->item_count; ++i ) {
    if ( RTEMS_RECORD_GET_EVENT( ctx->items[ i ].event ) == event ) {
      return i;
    }
  }

  return SIZE_MAX;
}

static size_t FindItem(
  const Context     *ctx,
  size_t             begin,
  rtems_record_event event,
  rtems_record_data  data
)
{
  size_t i;

  for ( i = begin; i < ctx->item_count; ++i ) {
    const rtems_record_item *item;

    item = &ctx->items[ i ];

    if (
      RTEMS_RECORD_GET_EVENT( item->event ) == event && item->data == data
    ) {
      return i;
    }
  }

  return SIZE_MAX;
}

static bool IsItem(
  const Context     *ctx,
  size_t             index,
  rtems_record_event event,
  rtems_record_data  data
)
{
  const rtems_record_item *item;

  if ( index >= ctx->item_count ) {
    return false;
  }

  item = &ctx->items[ index ];

  return RTEMS_RECORD_GET_EVENT( item->event ) == event && item->data == data;
}

static Context *default_handler_context;

void __real_bsp_interrupt_handler_default( rtems_vector_number vector );

void __wrap_bsp_interrupt_handler_default( rtems_vector_number vector );

void __wrap_bsp_interrupt_handler_default( rtems_vector_number vector )
{
  Context *ctx;

  ctx = default_handler_context;

  if ( ctx == NULL || vector != ctx->vector ) {
    __real_bsp_interrupt_handler_default( vector );
    return;
  }

  (void) ClearSoftwareInterrupt( vector );
  ++ctx->default_handler_counter;
}

/*
 * Keep only the items of the test.  A clock tick interrupt may nest into the
 * interrupt of the test and produce items in between.
 */
static void KeepTestItems( Context *ctx )
{
  size_t i;
  size_t n;

  n = 0;

  for ( i = 0; i < ctx->item_count; ++i ) {
    const rtems_record_item *item;
    uint32_t                 event;

    item = &ctx->items[ i ];
    event = RTEMS_RECORD_GET_EVENT( item->event );

    if (
      ( event == RTEMS_RECORD_INTERRUPT_ENTRY ||
        event == RTEMS_RECORD_INTERRUPT_EXIT ||
        event == RTEMS_RECORD_USER( 1 ) ) &&
      item->data == ctx->vector
    ) {
      ctx->items[ n ] = *item;
      ++n;
    }
  }

  ctx->item_count = n;
}

static void InterruptHandler( void *arg )
{
  Context *ctx;

  ctx = arg;
  (void) ClearSoftwareInterrupt( ctx->vector );
  rtems_record_produce( RTEMS_RECORD_USER( 1 ), ctx->vector );
  (void)
    _Atomic_Fetch_add_uint( &ctx->handler_counter, 1, ATOMIC_ORDER_RELAXED );
}

static void RtemsRecordValInterrupts_Setup(
  RtemsRecordValInterrupts_Context *ctx
)
{
  rtems_interrupt_attributes required;

  memset( &required, 0, sizeof( required ) );
  required.can_raise = true;
  ctx->vector = GetTestableInterruptVector( &required );
  T_assert_ne_u32( ctx->vector, UINT32_MAX );
}

static void RtemsRecordValInterrupts_Setup_Wrap( void *arg )
{
  RtemsRecordValInterrupts_Context *ctx;

  ctx = arg;
  RtemsRecordValInterrupts_Setup( ctx );
}

static T_fixture RtemsRecordValInterrupts_Fixture = {
  .setup = RtemsRecordValInterrupts_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsRecordValInterrupts_Instance
};

/**
 * @brief Install an interrupt entry for a testable interrupt vector. The
 *   handler produces an item. Enable the vector and raise it. Remove the
 *   interrupt entry.
 */
static void RtemsRecordValInterrupts_Action_0(
  RtemsRecordValInterrupts_Context *ctx
)
{
  rtems_status_code     sc;
  rtems_interrupt_entry entry;

  rtems_interrupt_entry_initialize( &entry, InterruptHandler, ctx, "Test" );
  sc = rtems_interrupt_entry_install(
    ctx->vector,
    RTEMS_INTERRUPT_SHARED,
    &entry
  );
  T_rsc_success( sc );
  ClearRecords();
  (void) rtems_interrupt_vector_enable( ctx->vector );
  sc = RaiseSoftwareInterrupt( ctx->vector );
  T_rsc_success( sc );
  (void) rtems_interrupt_vector_disable( ctx->vector );
  CollectRecords( ctx );
  KeepTestItems( ctx );
  sc = rtems_interrupt_entry_remove( ctx->vector, &entry );
  T_rsc_success( sc );

  /*
   * Check that the system produced an interrupt entry item with the vector
   * number before the item of the handler and an interrupt exit item with the
   * vector number after it.
   */
  size_t i;

  T_eq_uint(
    _Atomic_Load_uint( &ctx->handler_counter, ATOMIC_ORDER_RELAXED ),
    1
  );
  T_eq_ulong( ctx->overflow, 0 );
  i = FindItem( ctx, 0, RTEMS_RECORD_USER( 1 ), ctx->vector );
  T_ne_sz( i, SIZE_MAX );
  T_ge_sz( i, 1 );

  if ( i != SIZE_MAX && i >= 1 ) {
    T_true( IsItem( ctx, i - 1, RTEMS_RECORD_INTERRUPT_ENTRY, ctx->vector ) );
    T_true( IsItem( ctx, i + 1, RTEMS_RECORD_INTERRUPT_EXIT, ctx->vector ) );
  }
}

/**
 * @brief Enable the testable interrupt vector, which has no interrupt entry,
 *   and raise it. The default interrupt handler of the test replaces the
 *   spurious interrupt fatal error.
 */
static void RtemsRecordValInterrupts_Action_1(
  RtemsRecordValInterrupts_Context *ctx
)
{
  rtems_status_code sc;

  ClearRecords();
  default_handler_context = ctx;
  (void) rtems_interrupt_vector_enable( ctx->vector );
  sc = RaiseSoftwareInterrupt( ctx->vector );
  T_rsc_success( sc );
  (void) rtems_interrupt_vector_disable( ctx->vector );
  default_handler_context = NULL;
  CollectRecords( ctx );
  KeepTestItems( ctx );

  /*
   * Check that the system produced an interrupt entry item and an interrupt
   * exit item with the vector number around the call of the default interrupt
   * handler.
   */
  size_t i;

  T_eq_u32( ctx->default_handler_counter, 1 );
  T_eq_ulong( ctx->overflow, 0 );
  i = FindItem( ctx, 0, RTEMS_RECORD_INTERRUPT_ENTRY, ctx->vector );
  T_ne_sz( i, SIZE_MAX );
  T_true( IsItem( ctx, i + 1, RTEMS_RECORD_INTERRUPT_EXIT, ctx->vector ) );
}

/**
 * @brief Install an interrupt entry for the testable interrupt vector and
 *   enable the vector. Call rtems_record_interrupt_disable(), raise the
 *   vector, and call rtems_record_interrupt_enable(), so that the interrupt is
 *   pending when the directive restores the interrupt level. Wait until the
 *   handler ran. Remove the interrupt entry. Keep the items of the processor
 *   which called the directives.
 */
static void RtemsRecordValInterrupts_Action_2(
  RtemsRecordValInterrupts_Context *ctx
)
{
  rtems_status_code     sc;
  rtems_interrupt_entry entry;
  uint32_t              level;
  uint32_t              cpu_index;
  unsigned int          handler_counter;

  rtems_interrupt_entry_initialize( &entry, InterruptHandler, ctx, "Test" );
  sc = rtems_interrupt_entry_install(
    ctx->vector,
    RTEMS_INTERRUPT_SHARED,
    &entry
  );
  T_rsc_success( sc );
  handler_counter = _Atomic_Load_uint(
    &ctx->handler_counter,
    ATOMIC_ORDER_RELAXED
  );
  ClearRecords();
  (void) rtems_interrupt_vector_enable( ctx->vector );
  level = rtems_record_interrupt_disable();
  cpu_index = rtems_scheduler_get_processor();
  sc = RaiseSoftwareInterrupt( ctx->vector );
  rtems_record_interrupt_enable( level );
  T_rsc_success( sc );

  while (
    _Atomic_Load_uint( &ctx->handler_counter, ATOMIC_ORDER_RELAXED ) ==
    handler_counter
  ) {
    /* Wait */
  }

  (void) rtems_interrupt_vector_disable( ctx->vector );
  CollectProcessorRecords( ctx, cpu_index );
  sc = rtems_interrupt_entry_remove( ctx->vector, &entry );
  T_rsc_success( sc );

  /*
   * Check that the RTEMS_RECORD_ISR_ENABLE item of
   * rtems_record_interrupt_enable() precedes each RTEMS_RECORD_INTERRUPT_ENTRY
   * item of the vector on the processor.
   */
  size_t enable_index;
  size_t entry_index;

  enable_index = FindEvent( ctx, RTEMS_RECORD_ISR_ENABLE );
  entry_index = FindItem( ctx, 0, RTEMS_RECORD_INTERRUPT_ENTRY, ctx->vector );
  T_ne_sz( enable_index, SIZE_MAX );
  T_ne_sz( entry_index, SIZE_MAX );
  T_lt_sz( enable_index, entry_index );
}

/**
 * @fn void T_case_body_RtemsRecordValInterrupts( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsRecordValInterrupts,
  &RtemsRecordValInterrupts_Fixture
)
{
  RtemsRecordValInterrupts_Context *ctx;

  ctx = T_fixture_context();

  RtemsRecordValInterrupts_Action_0( ctx );
  RtemsRecordValInterrupts_Action_1( ctx );
  RtemsRecordValInterrupts_Action_2( ctx );
}

/** @} */
