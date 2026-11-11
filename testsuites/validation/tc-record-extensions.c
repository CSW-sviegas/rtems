/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordValExtensions
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
#include <rtems/score/isrlevel.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/userextdata.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsRecordValExtensions spec:/rtems/record/val/extensions
 *
 * @ingroup TestsuitesValidationRecord1
 *
 * @brief Tests the thread items of the event recording.
 *
 * This test case performs the following actions:
 *
 * - Create a task.
 *
 *   - Check that the system produced a create item with the task identifier
 *     followed by a name item with the task name.
 *
 * - Start the task. The task has a higher priority than the runner. The entry
 *   of the task returns. The fatal error handler of the test lets the task
 *   exit.
 *
 *   - Check that the entry of the task ran and returned.
 *
 *   - Check that the system produced a start item with the task identifier.
 *
 *   - Check that the system produced for the switch from the runner to the
 *     task a switch out item with the runner identifier, a stack item, and a
 *     switch in item with the task identifier. The stack usage of the runner
 *     at the switch is greater than at the frame of this action. It exceeds
 *     the usage at this frame by less than the stack which the call chain of
 *     the start needs, and it is less than the stack size.
 *
 *   - Check that the system produced a begin item, an exitted item, and a
 *     terminate item with the task identifier in this order.
 *
 * - Create and start a task which blocks. Restart the task.
 *
 *   - Check that the system produced a restart item with the task identifier.
 *
 * - Delete the blocked task. Create a task, which frees the deleted task.
 *   Delete the new task.
 *
 *   - Check that the system produced a terminate item and a delete item with
 *     the identifier of the blocked task.
 *
 * - Find the initial extension of the event recording.
 *
 *   - Check that the initial extension of the event recording has no fatal
 *     error handler.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/record/val/extensions test case.
 */
typedef struct {
  /**
   * @brief This member references the initial extension of the event
   *   recording.
   */
  const User_extensions_Table *record_extension;

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
   * @brief This member contains the runner identifier.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the stack size of the runner.
   */
  size_t runner_stack_size;

  /**
   * @brief This member contains the stack usage of the runner at the frame of
   *   the action which starts the worker.
   */
  uintptr_t runner_stack_usage;

  /**
   * @brief This member contains the worker identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member is true, if the worker ran.
   */
  bool worker_ran;

  /**
   * @brief This member contains the blocker identifier.
   */
  rtems_id blocker_id;

  /**
   * @brief This member counts the starts of the blocker.
   */
  uint32_t blocker_counter;

  /**
   * @brief This member contains the source of the fatal error.
   */
  rtems_fatal_source fatal_source;

  /**
   * @brief This member contains the code of the fatal error.
   */
  rtems_fatal_code fatal_code;
} RtemsRecordValExtensions_Context;

static RtemsRecordValExtensions_Context RtemsRecordValExtensions_Instance;

typedef RtemsRecordValExtensions_Context Context;

/*
 * This is the maximum stack size which the call chain from the start of the
 * worker to the thread switch extensions needs.
 */
#define CALL_CHAIN_STACK_MAX 2048

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

static rtems_record_data NameData( const char *name )
{
  rtems_record_data data;
  size_t            i;

  data = 0;

  for ( i = 0; i < sizeof( data ) && name[ i ] != '\0'; ++i ) {
    data |= (rtems_record_data) (unsigned char) name[ i ] << ( 8 * i );
  }

  return data;
}

static void FatalTaskExit(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context *ctx;

  ctx = arg;
  ctx->fatal_source = source;
  ctx->fatal_code = code;
  _ISR_Set_level( 0 );
  rtems_task_exit();
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ctx->worker_ran = true;
}

static void Blocker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ++ctx->blocker_counter;
  (void) ReceiveAnyEvents();
}

static void RtemsRecordValExtensions_Setup(
  RtemsRecordValExtensions_Context *ctx
)
{
  SetSelfPriority( PRIO_NORMAL );
  SetFatalHandler( FatalTaskExit, ctx );
}

static void RtemsRecordValExtensions_Setup_Wrap( void *arg )
{
  RtemsRecordValExtensions_Context *ctx;

  ctx = arg;
  RtemsRecordValExtensions_Setup( ctx );
}

static void RtemsRecordValExtensions_Teardown( void )
{
  SetFatalHandler( NULL, NULL );
  RestoreRunnerPriority();
}

static void RtemsRecordValExtensions_Teardown_Wrap( void *arg )
{
  (void) arg;
  RtemsRecordValExtensions_Teardown();
}

static T_fixture RtemsRecordValExtensions_Fixture = {
  .setup = RtemsRecordValExtensions_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRecordValExtensions_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsRecordValExtensions_Instance
};

/**
 * @brief Create a task.
 */
static void RtemsRecordValExtensions_Action_0(
  RtemsRecordValExtensions_Context *ctx
)
{
  ClearRecords();
  ctx->worker_id = CreateTask( "RECX", PRIO_HIGH );
  CollectRecords( ctx );

  /*
   * Check that the system produced a create item with the task identifier
   * followed by a name item with the task name.
   */
  size_t i;

  T_eq_ulong( ctx->overflow, 0 );
  i = FindItem( ctx, 0, RTEMS_RECORD_THREAD_CREATE, ctx->worker_id );
  T_ne_sz( i, SIZE_MAX );
  T_true( IsItem( ctx, i + 1, RTEMS_RECORD_THREAD_NAME, NameData( "RECX" ) ) );
}

/**
 * @brief Start the task. The task has a higher priority than the runner. The
 *   entry of the task returns. The fatal error handler of the test lets the
 *   task exit.
 */
static void RtemsRecordValExtensions_Action_1(
  RtemsRecordValExtensions_Context *ctx
)
{
  const Thread_Control *runner;
  uintptr_t             stack_begin;
  uintptr_t             frame;

  runner = _Thread_Get_executing();
  ctx->runner_id = rtems_task_self();
  ctx->runner_stack_size = runner->Start.Initial_stack.size;
  stack_begin = (uintptr_t) runner->Start.Initial_stack.area;
  frame = (uintptr_t) __builtin_frame_address( 0 );
  #if CPU_STACK_GROWS_UP == TRUE
  ctx->runner_stack_usage = frame - stack_begin;
  #else
  ctx->runner_stack_usage = stack_begin + ctx->runner_stack_size - frame;
  #endif
  ClearRecords();
  StartTask( ctx->worker_id, Worker, ctx );
  CollectRecords( ctx );

  /*
   * Check that the entry of the task ran and returned.
   */
  T_true( ctx->worker_ran );
  T_eq_int( ctx->fatal_source, INTERNAL_ERROR_CORE );
  T_eq_ulong( ctx->fatal_code, INTERNAL_ERROR_THREAD_EXITTED );
  T_eq_ulong( ctx->overflow, 0 );

  /*
   * Check that the system produced a start item with the task identifier.
   */
  T_ne_sz(
    FindItem( ctx, 0, RTEMS_RECORD_THREAD_START, ctx->worker_id ),
    SIZE_MAX
  );

  /*
   * Check that the system produced for the switch from the runner to the task
   * a switch out item with the runner identifier, a stack item, and a switch
   * in item with the task identifier. The stack usage of the runner at the
   * switch is greater than at the frame of this action. It exceeds the usage
   * at this frame by less than the stack which the call chain of the start
   * needs, and it is less than the stack size.
   */
  size_t i;

  i = FindItem( ctx, 0, RTEMS_RECORD_THREAD_SWITCH_IN, ctx->worker_id );
  T_ne_sz( i, SIZE_MAX );
  T_ge_sz( i, 2 );

  if ( i != SIZE_MAX && i >= 2 ) {
    T_true(
      IsItem( ctx, i - 2, RTEMS_RECORD_THREAD_SWITCH_OUT, ctx->runner_id )
    );
    T_eq_u32(
      RTEMS_RECORD_GET_EVENT( ctx->items[ i - 1 ].event ),
      RTEMS_RECORD_THREAD_STACK_CURRENT
    );
    T_gt_ulong( ctx->items[ i - 1 ].data, ctx->runner_stack_usage );
    T_lt_ulong(
      ctx->items[ i - 1 ].data,
      ctx->runner_stack_usage + CALL_CHAIN_STACK_MAX
    );
    T_lt_ulong( ctx->items[ i - 1 ].data, ctx->runner_stack_size );
  }

  /*
   * Check that the system produced a begin item, an exitted item, and a
   * terminate item with the task identifier in this order.
   */
  size_t begin;
  size_t exitted;
  size_t terminate;

  begin = FindItem( ctx, 0, RTEMS_RECORD_THREAD_BEGIN, ctx->worker_id );
  T_ne_sz( begin, SIZE_MAX );
  exitted = FindItem(
    ctx,
    begin,
    RTEMS_RECORD_THREAD_EXITTED,
    ctx->worker_id
  );
  T_ne_sz( exitted, SIZE_MAX );
  terminate = FindItem(
    ctx,
    exitted,
    RTEMS_RECORD_THREAD_TERMINATE,
    ctx->worker_id
  );
  T_ne_sz( terminate, SIZE_MAX );
}

/**
 * @brief Create and start a task which blocks. Restart the task.
 */
static void RtemsRecordValExtensions_Action_2(
  RtemsRecordValExtensions_Context *ctx
)
{
  ctx->blocker_id = CreateTask( "RECY", PRIO_HIGH );
  StartTask( ctx->blocker_id, Blocker, ctx );
  ClearRecords();
  (void) rtems_task_restart( ctx->blocker_id, (rtems_task_argument) ctx );
  CollectRecords( ctx );

  /*
   * Check that the system produced a restart item with the task identifier.
   */
  T_eq_u32( ctx->blocker_counter, 2 );
  T_eq_ulong( ctx->overflow, 0 );
  T_ne_sz(
    FindItem( ctx, 0, RTEMS_RECORD_THREAD_RESTART, ctx->blocker_id ),
    SIZE_MAX
  );
}

/**
 * @brief Delete the blocked task. Create a task, which frees the deleted task.
 *   Delete the new task.
 */
static void RtemsRecordValExtensions_Action_3(
  RtemsRecordValExtensions_Context *ctx
)
{
  rtems_id id;

  ClearRecords();
  DeleteTask( ctx->blocker_id );
  id = CreateTask( "RECZ", PRIO_HIGH );
  CollectRecords( ctx );
  DeleteTask( id );

  /*
   * Check that the system produced a terminate item and a delete item with the
   * identifier of the blocked task.
   */
  T_eq_ulong( ctx->overflow, 0 );
  T_ne_sz(
    FindItem( ctx, 0, RTEMS_RECORD_THREAD_TERMINATE, ctx->blocker_id ),
    SIZE_MAX
  );
  T_ne_sz(
    FindItem( ctx, 0, RTEMS_RECORD_THREAD_DELETE, ctx->blocker_id ),
    SIZE_MAX
  );
}

/**
 * @brief Find the initial extension of the event recording.
 */
static void RtemsRecordValExtensions_Action_4(
  RtemsRecordValExtensions_Context *ctx
)
{
  size_t i;

  ctx->record_extension = NULL;

  for ( i = 0; i < _User_extensions_Initial_count; ++i ) {
    const User_extensions_Table *table;

    table = &_User_extensions_Initial_extensions[ i ];

    if ( table->thread_create == _Record_Thread_create ) {
      ctx->record_extension = table;
    }
  }

  /*
   * Check that the initial extension of the event recording has no fatal error
   * handler.
   */
  T_not_null( ctx->record_extension );

  if ( ctx->record_extension != NULL ) {
    T_null( ctx->record_extension->fatal );
  }
}

/**
 * @fn void T_case_body_RtemsRecordValExtensions( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsRecordValExtensions,
  &RtemsRecordValExtensions_Fixture
)
{
  RtemsRecordValExtensions_Context *ctx;

  ctx = T_fixture_context();

  RtemsRecordValExtensions_Action_0( ctx );
  RtemsRecordValExtensions_Action_1( ctx );
  RtemsRecordValExtensions_Action_2( ctx );
  RtemsRecordValExtensions_Action_3( ctx );
  RtemsRecordValExtensions_Action_4( ctx );
}

/** @} */
