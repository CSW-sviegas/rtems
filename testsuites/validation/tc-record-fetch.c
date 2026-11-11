/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordReqFetch
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
#include <string.h>
#include <rtems/record.h>
#include <rtems/score/percpu.h>
#include <rtems/score/watchdogimpl.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsRecordReqFetch spec:/rtems/record/req/fetch
 *
 * @ingroup TestsuitesValidationRecord0
 *
 * The setup removes the uptime watchdog of each processor, so that only the
 * test produces items. The test produces the items of a processor through
 * rtems_record_prepare_critical() with the control of that processor. The data
 * of the produced items increments by one for each item. A second call of
 * rtems_record_fetch() after the action observes the processor which the
 * record fetch control selected. For the producer states, an interrupt
 * produces items while the call copies them. The interrupt produces only after
 * the call moved the tail of the record buffer and before the call returned.
 * The test repeats the interrupt test until the fetched items show the
 * overwrite. For the uncommitted production, the interrupt writes the items
 * without a commit, and the test commits them after the call returned. This
 * emulates a producer on another processor. The call copies the items byte by
 * byte, so the test does not check the data of the items which were in the
 * copy at the interrupt.
 *
 * @{
 */

typedef enum {
  RtemsRecordReqFetch_Pre_LastProcessor_Yes,
  RtemsRecordReqFetch_Pre_LastProcessor_No,
  RtemsRecordReqFetch_Pre_LastProcessor_NA
} RtemsRecordReqFetch_Pre_LastProcessor;

typedef enum {
  RtemsRecordReqFetch_Pre_Resume_No,
  RtemsRecordReqFetch_Pre_Resume_Yes,
  RtemsRecordReqFetch_Pre_Resume_NA
} RtemsRecordReqFetch_Pre_Resume;

typedef enum {
  RtemsRecordReqFetch_Pre_ItemCount_Invalid,
  RtemsRecordReqFetch_Pre_ItemCount_Small,
  RtemsRecordReqFetch_Pre_ItemCount_Enough,
  RtemsRecordReqFetch_Pre_ItemCount_NA
} RtemsRecordReqFetch_Pre_ItemCount;

typedef enum {
  RtemsRecordReqFetch_Pre_Buffer_Empty,
  RtemsRecordReqFetch_Pre_Buffer_Partial,
  RtemsRecordReqFetch_Pre_Buffer_Overflow,
  RtemsRecordReqFetch_Pre_Buffer_Lost,
  RtemsRecordReqFetch_Pre_Buffer_NA
} RtemsRecordReqFetch_Pre_Buffer;

typedef enum {
  RtemsRecordReqFetch_Pre_Producer_Idle,
  RtemsRecordReqFetch_Pre_Producer_Some,
  RtemsRecordReqFetch_Pre_Producer_All,
  RtemsRecordReqFetch_Pre_Producer_Uncommitted,
  RtemsRecordReqFetch_Pre_Producer_NA
} RtemsRecordReqFetch_Pre_Producer;

typedef enum {
  RtemsRecordReqFetch_Post_Status_Done,
  RtemsRecordReqFetch_Post_Status_Continue,
  RtemsRecordReqFetch_Post_Status_InvalidItemCount,
  RtemsRecordReqFetch_Post_Status_NA
} RtemsRecordReqFetch_Post_Status;

typedef enum {
  RtemsRecordReqFetch_Post_Fetched_None,
  RtemsRecordReqFetch_Post_Fetched_Header,
  RtemsRecordReqFetch_Post_Fetched_All,
  RtemsRecordReqFetch_Post_Fetched_Part,
  RtemsRecordReqFetch_Post_Fetched_Overflow,
  RtemsRecordReqFetch_Post_Fetched_OverflowOnly,
  RtemsRecordReqFetch_Post_Fetched_NA
} RtemsRecordReqFetch_Post_Fetched;

typedef enum {
  RtemsRecordReqFetch_Post_Copy_Provided,
  RtemsRecordReqFetch_Post_Copy_Replaced,
  RtemsRecordReqFetch_Post_Copy_NA
} RtemsRecordReqFetch_Post_Copy;

typedef enum {
  RtemsRecordReqFetch_Post_Lost_Uncounted,
  RtemsRecordReqFetch_Post_Lost_NA
} RtemsRecordReqFetch_Post_Lost;

typedef enum {
  RtemsRecordReqFetch_Post_Next_Same,
  RtemsRecordReqFetch_Post_Next_Following,
  RtemsRecordReqFetch_Post_Next_First,
  RtemsRecordReqFetch_Post_Next_Rejected,
  RtemsRecordReqFetch_Post_Next_NA
} RtemsRecordReqFetch_Post_Next;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_LastProcessor_NA : 1;
  uint32_t Pre_Resume_NA : 1;
  uint32_t Pre_ItemCount_NA : 1;
  uint32_t Pre_Buffer_NA : 1;
  uint32_t Pre_Producer_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Fetched : 3;
  uint32_t Post_Copy : 2;
  uint32_t Post_Lost : 1;
  uint32_t Post_Next : 3;
} RtemsRecordReqFetch_Entry;

#define ITEM_COUNT_INVALID 0

#define ITEM_COUNT_SMALL 1

#define ITEM_COUNT_ENOUGH 2

#define ITEM_COUNT_ENOUGH_MAX 64

#define BUFFER_EMPTY 0

#define BUFFER_PARTIAL 1

#define BUFFER_OVERFLOW 2

#define BUFFER_LOST 3

#define PRODUCER_IDLE 0

#define PRODUCER_SOME 1

#define PRODUCER_ALL 2

#define PRODUCER_UNCOMMITTED 3

/**
 * @brief Test context for spec:/rtems/record/req/fetch test case.
 */
typedef struct {
  /**
   * @brief This member contains the record fetch control.
   */
  rtems_record_fetch_control control;

  /**
   * @brief This member provides the storage of the record fetch control.
   */
  rtems_record_item storage[ ITEM_COUNT_ENOUGH_MAX ];

  /**
   * @brief This member contains a copy of the items which the call fetched.
   */
  rtems_record_item fetched[ ITEM_COUNT_ENOUGH_MAX ];

  /**
   * @brief This member contains the count of the items which the call fetched.
   */
  size_t fetched_count;

  /**
   * @brief This member contains the fetched items reference after the call.
   */
  const rtems_record_item *fetched_items;

  /**
   * @brief This member contains the index of the processor of the fetch.
   */
  uint32_t cpu_index;

  /**
   * @brief This member specifies if the call continues a fetch.
   */
  bool resume;

  /**
   * @brief This member specifies the item count state.
   */
  int item_count;

  /**
   * @brief This member specifies the record buffer state.
   */
  int buffer;

  /**
   * @brief This member contains the item count of the record fetch control.
   */
  size_t count;

  /**
   * @brief This member contains the data of the next produced item.
   */
  rtems_record_data next_data;

  /**
   * @brief This member contains the expected data of the first fetched item of
   *   the record buffer.
   */
  rtems_record_data expected_first_data;

  /**
   * @brief This member contains the expected count of the fetched items of the
   *   record buffer.
   */
  size_t expected_count;

  /**
   * @brief This member contains the expected count of the lost items.
   */
  rtems_record_data expected_overflow;

  /**
   * @brief This member contains the expected data of the first item of the
   *   record buffer which the next call fetches.
   */
  rtems_record_data expected_next_data;

  /**
   * @brief This member contains the return status of the call.
   */
  rtems_record_fetch_status status;

  /**
   * @brief This member contains the return status of the next call.
   */
  rtems_record_fetch_status next_status;

  /**
   * @brief This member contains the processor index which the next call
   *   fetched.
   */
  uint32_t next_cpu_index;

  /**
   * @brief This member contains the data of the first item of the record
   *   buffer which the next call fetched.
   */
  rtems_record_data next_first_data;

  /**
   * @brief This member specifies the producer state.
   */
  int producer;

  /**
   * @brief This member contains the tail of the record buffer before the call.
   */
  unsigned int tail_before;

  /**
   * @brief This member is true, if the call returned.
   */
  volatile bool action_done;

  /**
   * @brief This member is true, if the next first data can be predicted.
   */
  bool next_data_valid;

  /**
   * @brief This member is true, if the call started.
   */
  volatile bool in_action;

  /**
   * @brief This member is true, if the interrupt produced the items of the
   *   action.
   */
  volatile bool interrupt_produced;

  /**
   * @brief This member contains the record context of the uncommitted
   *   production.
   */
  rtems_record_context uncommitted;

  /**
   * @brief This member is true, if the uncommitted production waits for its
   *   commit.
   */
  bool uncommitted_pending;

  /**
   * @brief This member contains the head of the record buffer at the start of
   *   the uncommitted production.
   */
  unsigned int uncommitted_head;

  /**
   * @brief This member contains the data of the first item of the uncommitted
   *   production.
   */
  rtems_record_data uncommitted_data;

  /**
   * @brief This member contains the position of the first copied item of the
   *   call.
   */
  unsigned int copied_begin;

  /**
   * @brief This member contains the count of the copied items with a stored
   *   data word at the start of the uncommitted production.
   */
  size_t stored_before;

  /**
   * @brief This member contains the expiration time of the uptime watchdog of
   *   each processor.
   */
  uint64_t expire[ CPU_MAXIMUM_PROCESSORS ];

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    RtemsRecordReqFetch_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsRecordReqFetch_Context;

static RtemsRecordReqFetch_Context RtemsRecordReqFetch_Instance;

static const char *const RtemsRecordReqFetch_PreDesc_LastProcessor[] =
  { "Yes", "No", "NA" };

static const char *const RtemsRecordReqFetch_PreDesc_Resume[] =
  { "No", "Yes", "NA" };

static const char *const RtemsRecordReqFetch_PreDesc_ItemCount[] =
  { "Invalid", "Small", "Enough", "NA" };

static const char *const RtemsRecordReqFetch_PreDesc_Buffer[] =
  { "Empty", "Partial", "Overflow", "Lost", "NA" };

static const char *const RtemsRecordReqFetch_PreDesc_Producer[] =
  { "Idle", "Some", "All", "Uncommitted", "NA" };

static const char *const *const RtemsRecordReqFetch_PreDesc[] = {
  RtemsRecordReqFetch_PreDesc_LastProcessor,
  RtemsRecordReqFetch_PreDesc_Resume,
  RtemsRecordReqFetch_PreDesc_ItemCount,
  RtemsRecordReqFetch_PreDesc_Buffer,
  RtemsRecordReqFetch_PreDesc_Producer,
  NULL
};

typedef RtemsRecordReqFetch_Context Context;

static size_t Capacity( void )
{
  return rtems_record_get_item_count_for_fetch() - 2;
}

static Record_Control *GetControl( const Context *ctx )
{
  return _Per_CPU_Get_by_index( ctx->cpu_index )->record;
}

static void Produce( Context *ctx, size_t n )
{
  rtems_interrupt_level level;
  size_t                i;

  rtems_interrupt_local_disable( level );

  for ( i = 0; i < n; ++i ) {
    rtems_record_context context;

    rtems_record_prepare_critical(
      &context,
      _Per_CPU_Get_by_index( ctx->cpu_index )
    );
    rtems_record_add( &context, RTEMS_RECORD_USER_0, ctx->next_data );
    rtems_record_commit_critical( &context );
    ++ctx->next_data;
  }

  rtems_interrupt_local_enable( level );
}

static void ProduceUncommitted( Context *ctx, size_t n )
{
  size_t i;

  rtems_record_prepare_critical(
    &ctx->uncommitted,
    _Per_CPU_Get_by_index( ctx->cpu_index )
  );
  ctx->uncommitted_head = ctx->uncommitted.head;
  ctx->uncommitted_data = ctx->next_data;

  for ( i = 0; i < n; ++i ) {
    rtems_record_add( &ctx->uncommitted, RTEMS_RECORD_USER_0, ctx->next_data );
    ++ctx->next_data;
  }

  ctx->uncommitted_pending = true;
}

static void CommitUncommitted( Context *ctx )
{
  rtems_interrupt_level level;

  T_true( ctx->uncommitted_pending );

  if ( ctx->uncommitted_pending ) {
    rtems_interrupt_local_disable( level );
    rtems_record_commit_critical( &ctx->uncommitted );
    rtems_interrupt_local_enable( level );
    ctx->uncommitted_pending = false;
  }

  ctx->next_data_valid = false;
}

static void Initialize( Context *ctx, size_t count )
{
  ctx->count = count;
  rtems_record_fetch_initialize( &ctx->control, &ctx->storage[ 0 ], count );
#if defined( RTEMS_SMP )
  ctx->control.internal.cpu_index = ctx->cpu_index;
#endif
}

static size_t Minimum( size_t a, size_t b )
{
  return a < b ? a : b;
}

static void Arrange( Context *ctx )
{
  Record_Control   *control;
  size_t            capacity;
  size_t            produced;
  size_t            remaining;
  rtems_record_data first;

  control = GetControl( ctx );
  control->tail = _Record_Head( control );
  capacity = Capacity();
  ctx->next_data = 1000;
  ctx->fetched_items = NULL;
  ctx->fetched_count = 0;
  ctx->expected_overflow = 0;
  ctx->expected_count = 0;
  ctx->next_cpu_index = UINT32_MAX;
  ctx->next_first_data = 0;
  ctx->next_data_valid = true;
  ctx->in_action = false;
  ctx->action_done = false;
  ctx->interrupt_produced = false;
  ctx->uncommitted_pending = false;

  if ( ctx->buffer == BUFFER_EMPTY ) {
    produced = 0;
  } else if ( ctx->buffer == BUFFER_PARTIAL ) {
    produced = 5;
  } else {
    produced = capacity + 3;
  }

  if ( ctx->item_count == ITEM_COUNT_INVALID ) {
    Initialize( ctx, 2 );
    Produce( ctx, produced );
    return;
  }

  if ( !ctx->resume ) {
    if ( ctx->item_count == ITEM_COUNT_SMALL ) {
      Initialize( ctx, 6 );
    } else {
      Initialize( ctx, capacity + 2 );
    }

    first = ctx->next_data;
    Produce( ctx, produced );

    if ( produced > capacity ) {
      ctx->expected_overflow = produced - capacity;
    }

    remaining = produced - ctx->expected_overflow;
    ctx->expected_first_data = first + ctx->expected_overflow;
  } else {
    rtems_record_fetch_status status;
    size_t                    lost;

    Initialize( ctx, 6 );

    if ( ctx->item_count == ITEM_COUNT_SMALL ) {
      remaining = ctx->buffer == BUFFER_OVERFLOW ? 7 : 5;
    } else {
      remaining = 4;
    }

    first = ctx->next_data;
    Produce( ctx, 4 + remaining );
    status = rtems_record_fetch( &ctx->control );
    T_quiet_eq_int( status, RTEMS_RECORD_FETCH_CONTINUE );
    first += 4;

    if ( ctx->buffer == BUFFER_OVERFLOW || ctx->buffer == BUFFER_LOST ) {
      size_t produced_after;

      if ( ctx->item_count == ITEM_COUNT_SMALL ) {
        produced_after = capacity - 5;
      } else if ( ctx->buffer == BUFFER_OVERFLOW ) {
        produced_after = capacity - 2;
      } else {
        produced_after = capacity;
      }

      Produce( ctx, produced_after );
      lost = remaining + produced_after - capacity;
      ctx->expected_overflow = lost;
      remaining -= lost;
      first += lost;
    }

    ctx->expected_first_data = first;
  }

  ctx->expected_count = Minimum( remaining, ctx->count - 2 );
  ctx->expected_next_data = ctx->expected_first_data + ctx->expected_count;
  ctx->tail_before = control->tail;
  memset( ctx->storage, 0, sizeof( ctx->storage ) );
}

static void InterruptPrepare( void *arg )
{
  Arrange( arg );
}

static void InterruptAction( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->in_action = true;
  ctx->status = rtems_record_fetch( &ctx->control );
  ctx->action_done = true;
}

static T_interrupt_test_state Interrupt( void *arg )
{
  Context        *ctx;
  Record_Control *control;
  unsigned int    copied_begin;
  unsigned int    filled;
  size_t          capacity;
  size_t          copied;
  size_t          produced;
  size_t          overwritten;

  ctx = arg;

  if ( !ctx->in_action ) {
    return T_INTERRUPT_TEST_EARLY;
  }

  if ( ctx->action_done ) {
    return T_INTERRUPT_TEST_LATE;
  }

  /*
   * The watchdog of the interrupt test calls this handler on each clock tick.
   * The action may last more than one clock tick.
   */
  if ( ctx->interrupt_produced ) {
    return T_INTERRUPT_TEST_DONE;
  }

  control = GetControl( ctx );

  if ( control->tail == ctx->tail_before ) {
    return T_INTERRUPT_TEST_EARLY;
  }

  /*
   * The fetch checks for overwritten items after it copied the last item.
   * An overwrite after the copy of the last item is not visible to it.
   */
  if (
    ctx->expected_count > 0 &&
    ctx->storage[ 1 + ctx->expected_count ].data != 0
  ) {
    return T_INTERRUPT_TEST_LATE;
  }

  capacity = Capacity();
  copied = ctx->expected_count;
  copied_begin = control->tail - (unsigned int) copied;
  filled = _Record_Head( control ) - copied_begin;

  if ( ctx->producer == PRODUCER_UNCOMMITTED ) {
    size_t stored;

    stored = 0;

    while ( stored < copied && ctx->storage[ 2 + stored ].data != 0 ) {
      ++stored;
    }

    /*
     * The call copies the items byte by byte.  The copy of the last item with
     * a stored data word and of the item after it may be in progress.  The
     * check needs a later item which the call copies after the production.
     */
    if ( stored + 1 >= copied ) {
      return T_INTERRUPT_TEST_LATE;
    }

    ctx->copied_begin = copied_begin;
    ctx->stored_before = stored;
    ProduceUncommitted( ctx, capacity + 1 - filled + copied );
    ctx->interrupt_produced = true;

    return T_INTERRUPT_TEST_DONE;
  }

  if ( ctx->producer == PRODUCER_SOME ) {
    produced = capacity + 1 - filled;
  } else {
    produced = capacity + 1 - filled + copied;
  }

  Produce( ctx, produced );
  ctx->interrupt_produced = true;
  overwritten = filled + produced - capacity;

  if ( overwritten > copied ) {
    overwritten = copied;
  }

  ctx->expected_overflow += overwritten;
  ctx->expected_first_data += overwritten;
  ctx->expected_count = copied - overwritten;
  ctx->next_data_valid = false;

  return T_INTERRUPT_TEST_DONE;
}

static const T_interrupt_test_config interrupt_config = {
  .prepare = InterruptPrepare,
  .action = InterruptAction,
  .interrupt = Interrupt,
  .max_iteration_count = 1000
};

static bool HasOverwrite( const Context *ctx )
{
  const rtems_record_fetch_control *control;

  control = &ctx->control;

  return control->fetched_items != NULL && control->fetched_count >= 2 &&
         control->fetched_items[ 1 ].event == RTEMS_RECORD_PER_CPU_OVERFLOW &&
         control->fetched_items[ 1 ].data == ctx->expected_overflow;
}

static void InterruptFetch( Context *ctx )
{
  T_interrupt_test_state state;
  uint32_t               i;

  state = T_INTERRUPT_TEST_INITIAL;

  for ( i = 0; i < 10; ++i ) {
    state = T_interrupt_test( &interrupt_config, ctx );

    if (
      state == T_INTERRUPT_TEST_DONE &&
      ( ctx->producer == PRODUCER_UNCOMMITTED || HasOverwrite( ctx ) )
    ) {
      break;
    }
  }

  T_eq_int( state, T_INTERRUPT_TEST_DONE );
}

static rtems_record_data FirstUserData(
  const rtems_record_fetch_control *control
)
{
  size_t i;

  for ( i = 0; i < control->fetched_count; ++i ) {
    const rtems_record_item *item;

    item = &control->fetched_items[ i ];

    if ( RTEMS_RECORD_GET_EVENT( item->event ) == RTEMS_RECORD_USER_0 ) {
      return item->data;
    }
  }

  return 0;
}

static void CheckFetched( const Context *ctx, bool overflow, size_t count )
{
  size_t i;
  size_t header;

  header = overflow ? 2 : 1;
  T_eq_sz( ctx->fetched_count, header + count );
  T_eq_u32( ctx->fetched[ 0 ].event, RTEMS_RECORD_PROCESSOR );
  T_eq_ulong( ctx->fetched[ 0 ].data, ctx->cpu_index );

  if ( overflow ) {
    T_eq_u32( ctx->fetched[ 1 ].event, RTEMS_RECORD_PER_CPU_OVERFLOW );
    T_eq_ulong( ctx->fetched[ 1 ].data, ctx->expected_overflow );
  }

  for ( i = 0; i < count && header + i < ctx->fetched_count; ++i ) {
    const rtems_record_item *item;

    item = &ctx->fetched[ header + i ];
    T_eq_u32( RTEMS_RECORD_GET_EVENT( item->event ), RTEMS_RECORD_USER_0 );
  }
}

static size_t FetchedHeader( const Context *ctx )
{
  return ctx->expected_overflow != 0 ? 2 : 1;
}

static void CheckProvided( const Context *ctx )
{
  size_t header;
  size_t i;

  header = FetchedHeader( ctx );

  for ( i = 0; header + i < ctx->fetched_count; ++i ) {
    T_eq_ulong(
      ctx->fetched[ header + i ].data,
      ctx->expected_first_data + i
    );
  }
}

static void CheckUncounted( const Context *ctx )
{
  if ( ctx->expected_overflow != 0 ) {
    T_eq_u32( ctx->fetched[ 1 ].event, RTEMS_RECORD_PER_CPU_OVERFLOW );
    T_eq_ulong( ctx->fetched[ 1 ].data, ctx->expected_overflow );
  } else {
    T_true(
      ctx->fetched_count < 2 ||
      ctx->fetched[ 1 ].event != RTEMS_RECORD_PER_CPU_OVERFLOW
    );
  }
}

static void CheckReplaced( const Context *ctx )
{
  size_t       header;
  size_t       i;
  unsigned int ring;

  header = FetchedHeader( ctx );
  ring = GetControl( ctx )->mask + 1;

  for ( i = 0; header + i < ctx->fetched_count; ++i ) {
    const rtems_record_item *item;
    rtems_record_data        provided;
    rtems_record_data        replaced;

    item = &ctx->fetched[ header + i ];
    provided = ctx->expected_first_data + i;
    replaced = ctx->uncommitted_data + ( ctx->copied_begin + (unsigned int) i +
                                         ring - ctx->uncommitted_head );

    if ( i + 1 < ctx->stored_before ) {
      T_eq_ulong( item->data, provided );
    } else if ( i > ctx->stored_before ) {
      T_eq_ulong( item->data, replaced );
    }
  }
}

static void RtemsRecordReqFetch_Pre_LastProcessor_Prepare(
  RtemsRecordReqFetch_Context          *ctx,
  RtemsRecordReqFetch_Pre_LastProcessor state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Pre_LastProcessor_Yes: {
      /*
       * While the index of the processor of the record fetch control is one
       * less than the value returned by
       * rtems_scheduler_get_processor_maximum().
       */
      ctx->cpu_index = rtems_scheduler_get_processor_maximum() - 1;
      break;
    }

    case RtemsRecordReqFetch_Pre_LastProcessor_No: {
      /*
       * While the index of the processor of the record fetch control is less
       * than the value returned by rtems_scheduler_get_processor_maximum()
       * minus one.
       */
      if ( rtems_scheduler_get_processor_maximum() < 2 ) {
        ctx->Map.skip = true;
      } else {
        ctx->cpu_index = 0;
      }
      break;
    }

    case RtemsRecordReqFetch_Pre_LastProcessor_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Pre_Resume_Prepare(
  RtemsRecordReqFetch_Context   *ctx,
  RtemsRecordReqFetch_Pre_Resume state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Pre_Resume_No: {
      /*
       * While the previous call of rtems_record_fetch() with the record fetch
       * control fetched all record items of the processor which the record
       * buffer provided, or while no call of rtems_record_fetch() used the
       * record fetch control.
       */
      ctx->resume = false;
      break;
    }

    case RtemsRecordReqFetch_Pre_Resume_Yes: {
      /*
       * While the previous call of rtems_record_fetch() with the record fetch
       * control left record items of the processor.
       */
      ctx->resume = true;
      break;
    }

    case RtemsRecordReqFetch_Pre_Resume_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Pre_ItemCount_Prepare(
  RtemsRecordReqFetch_Context      *ctx,
  RtemsRecordReqFetch_Pre_ItemCount state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Pre_ItemCount_Invalid: {
      /*
       * While the item count of the record fetch control is less than three.
       */
      ctx->item_count = ITEM_COUNT_INVALID;
      break;
    }

    case RtemsRecordReqFetch_Pre_ItemCount_Small: {
      /*
       * While the item count of the record fetch control is at least three,
       * while the item count is less than the count of the record items which
       * the record buffer of the processor provides plus two.
       */
      ctx->item_count = ITEM_COUNT_SMALL;
      break;
    }

    case RtemsRecordReqFetch_Pre_ItemCount_Enough: {
      /*
       * While the item count of the record fetch control is greater than or
       * equal to the count of the record items which the record buffer of the
       * processor provides plus two.
       */
      ctx->item_count = ITEM_COUNT_ENOUGH;
      break;
    }

    case RtemsRecordReqFetch_Pre_ItemCount_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Pre_Buffer_Prepare(
  RtemsRecordReqFetch_Context   *ctx,
  RtemsRecordReqFetch_Pre_Buffer state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Pre_Buffer_Empty: {
      /*
       * While the record buffer of the processor provides no record item.
       */
      ctx->buffer = BUFFER_EMPTY;
      break;
    }

    case RtemsRecordReqFetch_Pre_Buffer_Partial: {
      /*
       * While the record buffer of the processor provides at least one record
       * item, while no record item was lost since the previous call of
       * rtems_record_fetch() with the record fetch control.
       */
      ctx->buffer = BUFFER_PARTIAL;
      break;
    }

    case RtemsRecordReqFetch_Pre_Buffer_Overflow: {
      /*
       * While record items of the record buffer of the processor were lost
       * since the previous call of rtems_record_fetch() with the record fetch
       * control, while the record buffer still provides at least one record
       * item of the fetch.
       */
      ctx->buffer = BUFFER_OVERFLOW;
      break;
    }

    case RtemsRecordReqFetch_Pre_Buffer_Lost: {
      /*
       * While all remaining record items of a continued fetch were lost since
       * the previous call of rtems_record_fetch() with the record fetch
       * control.
       */
      ctx->buffer = BUFFER_LOST;
      break;
    }

    case RtemsRecordReqFetch_Pre_Buffer_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Pre_Producer_Prepare(
  RtemsRecordReqFetch_Context     *ctx,
  RtemsRecordReqFetch_Pre_Producer state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Pre_Producer_Idle: {
      /*
       * While no producer writes a record item to the record buffer of the
       * processor during the call.
       */
      ctx->producer = PRODUCER_IDLE;
      break;
    }

    case RtemsRecordReqFetch_Pre_Producer_Some: {
      /*
       * While a producer writes and commits record items during the call which
       * overwrite some of the record items which the call copies from the
       * record buffer of the processor.
       */
      ctx->producer = PRODUCER_SOME;
      break;
    }

    case RtemsRecordReqFetch_Pre_Producer_All: {
      /*
       * While a producer writes and commits record items during the call which
       * overwrite all record items which the call copies from the record
       * buffer of the processor and at least one more record item.
       */
      ctx->producer = PRODUCER_ALL;
      break;
    }

    case RtemsRecordReqFetch_Pre_Producer_Uncommitted: {
      /*
       * While a producer on another processor writes record items during the
       * call which overwrite record items which the call copies from the
       * record buffer of the processor after the write, while the producer
       * commits these record items after the call.
       */
      ctx->producer = PRODUCER_UNCOMMITTED;
      break;
    }

    case RtemsRecordReqFetch_Pre_Producer_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Post_Status_Check(
  RtemsRecordReqFetch_Context    *ctx,
  RtemsRecordReqFetch_Post_Status state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Post_Status_Done: {
      /*
       * The return status of rtems_record_fetch() shall be
       * RTEMS_RECORD_FETCH_DONE.
       */
      T_eq_int( ctx->status, RTEMS_RECORD_FETCH_DONE );
      break;
    }

    case RtemsRecordReqFetch_Post_Status_Continue: {
      /*
       * The return status of rtems_record_fetch() shall be
       * RTEMS_RECORD_FETCH_CONTINUE.
       */
      T_eq_int( ctx->status, RTEMS_RECORD_FETCH_CONTINUE );
      break;
    }

    case RtemsRecordReqFetch_Post_Status_InvalidItemCount: {
      /*
       * The return status of rtems_record_fetch() shall be
       * RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT.
       */
      T_eq_int( ctx->status, RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT );
      break;
    }

    case RtemsRecordReqFetch_Post_Status_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Post_Fetched_Check(
  RtemsRecordReqFetch_Context     *ctx,
  RtemsRecordReqFetch_Post_Fetched state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Post_Fetched_None: {
      /*
       * The `fetched_items` member of the record fetch control shall be NULL.
       */
      T_null( ctx->fetched_items );
      break;
    }

    case RtemsRecordReqFetch_Post_Fetched_Header: {
      /*
       * The record fetch control shall reference exactly one fetched record
       * item with the event RTEMS_RECORD_PROCESSOR and the index of the
       * processor as event data.
       */
      CheckFetched( ctx, false, 0 );
      break;
    }

    case RtemsRecordReqFetch_Post_Fetched_All: {
      /*
       * The record fetch control shall reference a record item with the event
       * RTEMS_RECORD_PROCESSOR and the index of the processor as event data,
       * followed by all record items which the record buffer of the processor
       * provided, from the oldest to the newest.
       */
      CheckFetched( ctx, false, ctx->expected_count );
      break;
    }

    case RtemsRecordReqFetch_Post_Fetched_Part: {
      /*
       * The record fetch control shall reference a record item with the event
       * RTEMS_RECORD_PROCESSOR and the index of the processor as event data,
       * followed by the oldest record items which the record buffer of the
       * processor provided with a count of the item count of the record fetch
       * control minus two.
       */
      T_eq_sz( ctx->expected_count, ctx->count - 2 );
      CheckFetched( ctx, false, ctx->expected_count );
      break;
    }

    case RtemsRecordReqFetch_Post_Fetched_Overflow: {
      /*
       * The record fetch control shall reference a record item with the event
       * RTEMS_RECORD_PROCESSOR and the index of the processor as event data,
       * followed by a record item with the event RTEMS_RECORD_PER_CPU_OVERFLOW
       * and the count of the lost record items as event data, followed by the
       * oldest record items of the fetch which the record buffer of the
       * processor still provided.
       */
      CheckFetched( ctx, true, ctx->expected_count );
      break;
    }

    case RtemsRecordReqFetch_Post_Fetched_OverflowOnly: {
      /*
       * The record fetch control shall reference a record item with the event
       * RTEMS_RECORD_PROCESSOR and the index of the processor as event data,
       * followed by a record item with the event RTEMS_RECORD_PER_CPU_OVERFLOW
       * and the count of the lost record items as event data, and no other
       * record item.
       */
      T_eq_sz( ctx->expected_count, 0 );
      CheckFetched( ctx, true, 0 );
      break;
    }

    case RtemsRecordReqFetch_Post_Fetched_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Post_Copy_Check(
  RtemsRecordReqFetch_Context  *ctx,
  RtemsRecordReqFetch_Post_Copy state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Post_Copy_Provided: {
      /*
       * Each record item which the record fetch control references after the
       * RTEMS_RECORD_PROCESSOR item and the optional
       * RTEMS_RECORD_PER_CPU_OVERFLOW item shall be a record item which the
       * record buffer of the processor provided at the start of the call.
       */
      CheckProvided( ctx );
      break;
    }

    case RtemsRecordReqFetch_Post_Copy_Replaced: {
      /*
       * For each record item which the call copied after the producer
       * overwrote it, the record fetch control shall reference the record item
       * which the producer wrote in place of the overwritten record item, or a
       * record item which consists of parts of the overwritten record item and
       * of the record item which the producer wrote.
       */
      CheckReplaced( ctx );
      break;
    }

    case RtemsRecordReqFetch_Post_Copy_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Post_Lost_Check(
  RtemsRecordReqFetch_Context  *ctx,
  RtemsRecordReqFetch_Post_Lost state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Post_Lost_Uncounted: {
      /*
       * The count of the lost record items which the record fetch control
       * references shall not include a record item which the call copied after
       * the producer overwrote it.
       */
      CheckUncounted( ctx );
      break;
    }

    case RtemsRecordReqFetch_Post_Lost_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Post_Next_Check(
  RtemsRecordReqFetch_Context  *ctx,
  RtemsRecordReqFetch_Post_Next state
)
{
  switch ( state ) {
    case RtemsRecordReqFetch_Post_Next_Same: {
      /*
       * The next call of rtems_record_fetch() with the record fetch control
       * shall fetch the remaining record items of the same processor.
       */
      T_eq_u32( ctx->next_cpu_index, ctx->cpu_index );

      if ( ctx->next_data_valid ) {
        T_eq_ulong( ctx->next_first_data, ctx->expected_next_data );
      }
      break;
    }

    case RtemsRecordReqFetch_Post_Next_Following: {
      /*
       * The next call of rtems_record_fetch() with the record fetch control
       * shall fetch the record items of the processor with the next index.
       */
      T_eq_u32( ctx->next_cpu_index, ctx->cpu_index + 1 );
      break;
    }

    case RtemsRecordReqFetch_Post_Next_First: {
      /*
       * The next call of rtems_record_fetch() with the record fetch control
       * shall fetch the record items of the processor with index zero.
       */
      T_eq_u32( ctx->next_cpu_index, 0 );
      break;
    }

    case RtemsRecordReqFetch_Post_Next_Rejected: {
      /*
       * The next call of rtems_record_fetch() with the record fetch control
       * shall return RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT.
       */
      T_eq_int( ctx->next_status, RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT );
      break;
    }

    case RtemsRecordReqFetch_Post_Next_NA:
      break;
  }
}

static void RtemsRecordReqFetch_Setup( RtemsRecordReqFetch_Context *ctx )
{
  uint32_t cpu_index;

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    rtems_interrupt_level level;
    Watchdog_Control     *watchdog;

    watchdog = &_Per_CPU_Get_by_index( cpu_index )->record->Watchdog;
    rtems_interrupt_local_disable( level );
    _Watchdog_Per_CPU_remove_ticks( watchdog );
    ctx->expire[ cpu_index ] = watchdog->expire;
    rtems_interrupt_local_enable( level );
  }
}

static void RtemsRecordReqFetch_Setup_Wrap( void *arg )
{
  RtemsRecordReqFetch_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRecordReqFetch_Setup( ctx );
}

static void RtemsRecordReqFetch_Teardown( RtemsRecordReqFetch_Context *ctx )
{
  uint32_t cpu_index;

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    rtems_interrupt_level level;
    Per_CPU_Control      *cpu;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    rtems_interrupt_local_disable( level );
    (void) _Watchdog_Per_CPU_insert(
      &cpu->record->Watchdog,
      cpu,
      &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
      ctx->expire[ cpu_index ]
    );
    rtems_interrupt_local_enable( level );
  }
}

static void RtemsRecordReqFetch_Teardown_Wrap( void *arg )
{
  RtemsRecordReqFetch_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRecordReqFetch_Teardown( ctx );
}

static void RtemsRecordReqFetch_Action( RtemsRecordReqFetch_Context *ctx )
{
  if ( ctx->producer == PRODUCER_IDLE || ctx->Map.entry.Skip ) {
    Arrange( ctx );
    ctx->status = rtems_record_fetch( &ctx->control );
  } else {
    InterruptFetch( ctx );
  }

  if ( ctx->producer == PRODUCER_UNCOMMITTED && !ctx->Map.entry.Skip ) {
    CommitUncommitted( ctx );
  }

  ctx->fetched_items = ctx->control.fetched_items;

  if ( ctx->fetched_items != NULL ) {
    T_quiet_le_sz(
      ctx->control.fetched_count,
      RTEMS_ARRAY_SIZE( ctx->fetched )
    );
    memcpy(
      ctx->fetched,
      ctx->control.fetched_items,
      ctx->control.fetched_count * sizeof( ctx->fetched[ 0 ] )
    );
    ctx->fetched_count = ctx->control.fetched_count;
  }

  ctx->next_status = rtems_record_fetch( &ctx->control );

  if ( ctx->control.fetched_items != NULL ) {
    ctx->next_cpu_index = (uint32_t) ctx->control.fetched_items[ 0 ].data;
    ctx->next_first_data = FirstUserData( &ctx->control );
  }
}

/* clang-format off */

static const RtemsRecordReqFetch_Entry
RtemsRecordReqFetch_Entries[] = {
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_Overflow,
    RtemsRecordReqFetch_Post_Copy_Provided, RtemsRecordReqFetch_Post_Lost_NA,
    RtemsRecordReqFetch_Post_Next_Same },
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Done,
    RtemsRecordReqFetch_Post_Fetched_Overflow,
    RtemsRecordReqFetch_Post_Copy_Provided, RtemsRecordReqFetch_Post_Lost_NA,
    RtemsRecordReqFetch_Post_Next_First },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_Overflow,
    RtemsRecordReqFetch_Post_Copy_Provided, RtemsRecordReqFetch_Post_Lost_NA,
    RtemsRecordReqFetch_Post_Next_Following },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_InvalidItemCount,
    RtemsRecordReqFetch_Post_Fetched_None, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_Rejected },
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_Part,
    RtemsRecordReqFetch_Post_Copy_Provided, RtemsRecordReqFetch_Post_Lost_NA,
    RtemsRecordReqFetch_Post_Next_Same },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_Part,
    RtemsRecordReqFetch_Post_Copy_Replaced,
    RtemsRecordReqFetch_Post_Lost_Uncounted, RtemsRecordReqFetch_Post_Next_Same },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_Overflow,
    RtemsRecordReqFetch_Post_Copy_Replaced,
    RtemsRecordReqFetch_Post_Lost_Uncounted, RtemsRecordReqFetch_Post_Next_Same },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Done,
    RtemsRecordReqFetch_Post_Fetched_All,
    RtemsRecordReqFetch_Post_Copy_Provided, RtemsRecordReqFetch_Post_Lost_NA,
    RtemsRecordReqFetch_Post_Next_First },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Done,
    RtemsRecordReqFetch_Post_Fetched_All,
    RtemsRecordReqFetch_Post_Copy_Replaced,
    RtemsRecordReqFetch_Post_Lost_Uncounted,
    RtemsRecordReqFetch_Post_Next_First },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Done,
    RtemsRecordReqFetch_Post_Fetched_Overflow,
    RtemsRecordReqFetch_Post_Copy_Replaced,
    RtemsRecordReqFetch_Post_Lost_Uncounted,
    RtemsRecordReqFetch_Post_Next_First },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_All,
    RtemsRecordReqFetch_Post_Copy_Provided, RtemsRecordReqFetch_Post_Lost_NA,
    RtemsRecordReqFetch_Post_Next_Following },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_All,
    RtemsRecordReqFetch_Post_Copy_Replaced,
    RtemsRecordReqFetch_Post_Lost_Uncounted,
    RtemsRecordReqFetch_Post_Next_Following },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_Overflow,
    RtemsRecordReqFetch_Post_Copy_Replaced,
    RtemsRecordReqFetch_Post_Lost_Uncounted,
    RtemsRecordReqFetch_Post_Next_Following },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Done,
    RtemsRecordReqFetch_Post_Fetched_Header, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_First },
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Done,
    RtemsRecordReqFetch_Post_Fetched_OverflowOnly,
    RtemsRecordReqFetch_Post_Copy_NA, RtemsRecordReqFetch_Post_Lost_NA,
    RtemsRecordReqFetch_Post_Next_First },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_Header, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_Following },
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_Continue,
    RtemsRecordReqFetch_Post_Fetched_OverflowOnly,
    RtemsRecordReqFetch_Post_Copy_NA, RtemsRecordReqFetch_Post_Lost_NA,
    RtemsRecordReqFetch_Post_Next_Following }
#else
  { 1, 0, 0, 0, 0, 0, RtemsRecordReqFetch_Post_Status_NA,
    RtemsRecordReqFetch_Post_Fetched_NA, RtemsRecordReqFetch_Post_Copy_NA,
    RtemsRecordReqFetch_Post_Lost_NA, RtemsRecordReqFetch_Post_Next_NA }
#endif
};

static const uint8_t
RtemsRecordReqFetch_Map[] = {
  4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 1, 6, 1, 1,
  1, 7, 0, 0, 0, 0, 14, 0, 0, 0, 8, 2, 2, 9, 2, 2, 2, 10, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 1, 6, 1, 1, 1, 7, 0,
  0, 0, 0, 0, 0, 0, 0, 8, 2, 2, 9, 2, 2, 2, 10, 15, 0, 0, 0, 4, 0, 0, 0, 4, 0,
  0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 1, 6, 1, 1, 1, 7, 0, 0, 0, 0,
  16, 0, 0, 0, 11, 3, 3, 12, 3, 3, 3, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 1, 6, 1, 1, 1, 7, 0, 0, 0, 0, 0, 0,
  0, 0, 11, 3, 3, 12, 3, 3, 3, 13, 17, 0, 0, 0
};

/* clang-format on */

static size_t RtemsRecordReqFetch_Scope( void *arg, char *buf, size_t n )
{
  RtemsRecordReqFetch_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsRecordReqFetch_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsRecordReqFetch_Fixture = {
  .setup = RtemsRecordReqFetch_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRecordReqFetch_Teardown_Wrap,
  .scope = RtemsRecordReqFetch_Scope,
  .initial_context = &RtemsRecordReqFetch_Instance
};

static const uint8_t RtemsRecordReqFetch_Weights[] = { 96, 48, 16, 4, 1 };

static void RtemsRecordReqFetch_Skip(
  RtemsRecordReqFetch_Context *ctx,
  size_t                       index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = RtemsRecordReqFetch_Pre_Resume_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = RtemsRecordReqFetch_Pre_ItemCount_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pcs[ 3 ] = RtemsRecordReqFetch_Pre_Buffer_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pcs[ 4 ] = RtemsRecordReqFetch_Pre_Producer_NA - 1;
      break;
  }
}

static inline RtemsRecordReqFetch_Entry RtemsRecordReqFetch_PopEntry(
  RtemsRecordReqFetch_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 5; ++i ) {
      index += RtemsRecordReqFetch_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return RtemsRecordReqFetch_Entries[ RtemsRecordReqFetch_Map[ index ] ];
}

static void RtemsRecordReqFetch_TestVariant( RtemsRecordReqFetch_Context *ctx )
{
  RtemsRecordReqFetch_Pre_LastProcessor_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    RtemsRecordReqFetch_Skip( ctx, 0 );
    return;
  }

  RtemsRecordReqFetch_Pre_Resume_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsRecordReqFetch_Pre_ItemCount_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsRecordReqFetch_Pre_Buffer_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsRecordReqFetch_Pre_Producer_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsRecordReqFetch_Action( ctx );
  RtemsRecordReqFetch_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsRecordReqFetch_Post_Fetched_Check( ctx, ctx->Map.entry.Post_Fetched );
  RtemsRecordReqFetch_Post_Copy_Check( ctx, ctx->Map.entry.Post_Copy );
  RtemsRecordReqFetch_Post_Lost_Check( ctx, ctx->Map.entry.Post_Lost );
  RtemsRecordReqFetch_Post_Next_Check( ctx, ctx->Map.entry.Post_Next );
}

/**
 * @fn void T_case_body_RtemsRecordReqFetch( void )
 */
T_TEST_CASE_FIXTURE( RtemsRecordReqFetch, &RtemsRecordReqFetch_Fixture )
{
  RtemsRecordReqFetch_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = RtemsRecordReqFetch_Pre_LastProcessor_Yes;
    ctx->Map.pcs[ 0 ] < RtemsRecordReqFetch_Pre_LastProcessor_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsRecordReqFetch_Pre_Resume_No;
      ctx->Map.pcs[ 1 ] < RtemsRecordReqFetch_Pre_Resume_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsRecordReqFetch_Pre_ItemCount_Invalid;
        ctx->Map.pcs[ 2 ] < RtemsRecordReqFetch_Pre_ItemCount_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsRecordReqFetch_Pre_Buffer_Empty;
          ctx->Map.pcs[ 3 ] < RtemsRecordReqFetch_Pre_Buffer_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsRecordReqFetch_Pre_Producer_Idle;
            ctx->Map.pcs[ 4 ] < RtemsRecordReqFetch_Pre_Producer_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = RtemsRecordReqFetch_PopEntry( ctx );
            RtemsRecordReqFetch_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
