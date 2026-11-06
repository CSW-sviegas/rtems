/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSemReqSeizeWait
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "tr-sem-seize-wait.h"
#include "tr-tq-enqueue-fifo.h"
#include "tr-tq-enqueue-priority.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreSemReqSeizeWait spec:/score/sem/req/seize-wait
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * The enqueue runners check the thread timer of a blocked worker against the
 * ticks of its watchdog. The test requires that no clock tick occurs between
 * the enqueue and the check.
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Discipline_NA : 1;
  uint16_t Pre_Timeout_NA : 1;
  uint16_t Pre_Count_NA : 1;
  uint16_t Post_Status : 1;
  uint16_t Post_Enqueued : 2;
  uint16_t Post_Count : 2;
  uint16_t Post_Timer : 2;
} ScoreSemReqSeizeWait_Entry;

/**
 * @brief Test context for spec:/score/sem/req/seize-wait test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreSemReqSeizeWait_Run() parameter.
   */
  TQSemContext *tq_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    ScoreSemReqSeizeWait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreSemReqSeizeWait_Context;

static ScoreSemReqSeizeWait_Context ScoreSemReqSeizeWait_Instance;

static const char *const ScoreSemReqSeizeWait_PreDesc_Discipline[] =
  { "FIFO", "Priority", "NA" };

static const char *const ScoreSemReqSeizeWait_PreDesc_Timeout[] =
  { "Ticks", "None", "NA" };

static const char *const ScoreSemReqSeizeWait_PreDesc_Count[] =
  { "Zero", "Positive", "NA" };

static const char *const *const ScoreSemReqSeizeWait_PreDesc[] = {
  ScoreSemReqSeizeWait_PreDesc_Discipline,
  ScoreSemReqSeizeWait_PreDesc_Timeout,
  ScoreSemReqSeizeWait_PreDesc_Count,
  NULL
};

typedef ScoreSemReqSeizeWait_Context Context;

static Status_Control Status( const Context *ctx, Status_Control status )
{
  return TQConvertStatus( &ctx->tq_ctx->base, status );
}

static void GetProperties( TQContext *base, TQWorkerKind enqueued_worker )
{
  TQSemContext        *ctx;
  Thread_Control      *thread;
  Thread_queue_Context queue_context;
  States_Control       state;
  TaskTimerInfo        timer_info;

  ctx = (TQSemContext *) base;
  thread = ctx->base.worker_tcb[ enqueued_worker ];

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_State_acquire( thread, &queue_context.Lock_context.Lock_context );
  state = thread->current_state;
  _Thread_State_release( thread, &queue_context.Lock_context.Lock_context );
  T_eq_u32( state, STATES_WAITING_FOR_SEMAPHORE );

  GetTaskTimerInfoByThread( thread, &timer_info );

  if ( base->wait == TQ_WAIT_TIMED ) {
    T_eq_int( timer_info.state, TASK_TIMER_TICKS );
    T_eq_u64(
      timer_info.expire_ticks - timer_info.now_ticks,
      base->timeout_ticks
    );
  } else {
    T_eq_int( timer_info.state, TASK_TIMER_INACTIVE );
  }

  T_eq_u32( TQSemGetCount( ctx ), 0 );
}

static void ScoreSemReqSeizeWait_Pre_Discipline_Prepare(
  ScoreSemReqSeizeWait_Context       *ctx,
  ScoreSemReqSeizeWait_Pre_Discipline state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Pre_Discipline_FIFO: {
      /*
       * Where the thread queue of the semaphore uses the FIFO discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_FIFO ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreSemReqSeizeWait_Pre_Discipline_Priority: {
      /*
       * Where the thread queue of the semaphore uses the priority discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_PRIORITY ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreSemReqSeizeWait_Pre_Discipline_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Pre_Timeout_Prepare(
  ScoreSemReqSeizeWait_Context    *ctx,
  ScoreSemReqSeizeWait_Pre_Timeout state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Pre_Timeout_Ticks: {
      /*
       * Where the directive waits with a timeout in clock ticks.
       */
      if ( ctx->tq_ctx->base.wait != TQ_WAIT_TIMED ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreSemReqSeizeWait_Pre_Timeout_None: {
      /*
       * Where the directive waits without a timeout.
       */
      if ( ctx->tq_ctx->base.wait != TQ_WAIT_FOREVER ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreSemReqSeizeWait_Pre_Timeout_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Pre_Count_Prepare(
  ScoreSemReqSeizeWait_Context  *ctx,
  ScoreSemReqSeizeWait_Pre_Count state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Pre_Count_Zero: {
      /*
       * While the count of the semaphore is zero.
       */
      /* Done by TQEnqueuePrepareDefault() */
      break;
    }

    case ScoreSemReqSeizeWait_Pre_Count_Positive: {
      /*
       * While the count of the semaphore is greater than zero.
       */
      TQSemSetCount( ctx->tq_ctx, 2 );
      break;
    }

    case ScoreSemReqSeizeWait_Pre_Count_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Post_Status_Check(
  ScoreSemReqSeizeWait_Context    *ctx,
  ScoreSemReqSeizeWait_Post_Status state
)
{
  Status_Control status;

  switch ( state ) {
    case ScoreSemReqSeizeWait_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      status = TQEnqueue( &ctx->tq_ctx->base, ctx->tq_ctx->base.wait );
      T_eq_int( status, Status( ctx, STATUS_SUCCESSFUL ) );
      break;
    }

    case ScoreSemReqSeizeWait_Post_Status_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Post_Enqueued_Check(
  ScoreSemReqSeizeWait_Context      *ctx,
  ScoreSemReqSeizeWait_Post_Enqueued state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Post_Enqueued_No: {
      /*
       * The calling thread shall not be enqueued on the thread queue of the
       * semaphore.
       */
      /* The test runner would block if the worker is enqueued */
      break;
    }

    case ScoreSemReqSeizeWait_Post_Enqueued_FIFO: {
      /*
       * The calling thread shall be enqueued in FIFO order.
       */
      ScoreTqReqEnqueueFifo_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreSemReqSeizeWait_Post_Enqueued_Priority: {
      /*
       * The calling thread shall be enqueued in priority order.
       */
      ScoreTqReqEnqueuePriority_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreSemReqSeizeWait_Post_Enqueued_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Post_Count_Check(
  ScoreSemReqSeizeWait_Context   *ctx,
  ScoreSemReqSeizeWait_Post_Count state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Post_Count_Nop: {
      /*
       * The count of the semaphore shall not be modified.
       */
      /* Checked by GetProperties() */
      break;
    }

    case ScoreSemReqSeizeWait_Post_Count_MinusOne: {
      /*
       * The count of the semaphore shall be decremented by one.
       */
      T_eq_u32( TQSemGetCount( ctx->tq_ctx ), 1 );
      break;
    }

    case ScoreSemReqSeizeWait_Post_Count_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Post_Timer_Check(
  ScoreSemReqSeizeWait_Post_Timer state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Post_Timer_Ticks: {
      /*
       * The thread timer of the calling task shall fire after the specified
       * clock ticks.
       */
      /* Checked by GetProperties() */
      break;
    }

    case ScoreSemReqSeizeWait_Post_Timer_No: {
      /*
       * The thread timer of the calling task shall be inactive.
       */
      T_eq_int(
        T_get_thread_timer_state( RTEMS_SELF ),
        T_THREAD_TIMER_INACTIVE
      );
      break;
    }

    case ScoreSemReqSeizeWait_Post_Timer_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Prepare( ScoreSemReqSeizeWait_Context *ctx )
{
  ctx->tq_ctx->base.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_ctx->base.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_ctx->base.get_properties = GetProperties;
}

static void ScoreSemReqSeizeWait_Action( void )
{
  /* Action performed by Status post-condition */
}

static void ScoreSemReqSeizeWait_Cleanup( ScoreSemReqSeizeWait_Context *ctx )
{
  ctx->tq_ctx->base.get_properties = NULL;
}

/* clang-format off */

static const ScoreSemReqSeizeWait_Entry
ScoreSemReqSeizeWait_Entries[] = {
  { 0, 0, 0, 0, ScoreSemReqSeizeWait_Post_Status_Ok,
    ScoreSemReqSeizeWait_Post_Enqueued_No,
    ScoreSemReqSeizeWait_Post_Count_MinusOne,
    ScoreSemReqSeizeWait_Post_Timer_No },
  { 0, 0, 0, 0, ScoreSemReqSeizeWait_Post_Status_NA,
    ScoreSemReqSeizeWait_Post_Enqueued_FIFO,
    ScoreSemReqSeizeWait_Post_Count_Nop, ScoreSemReqSeizeWait_Post_Timer_Ticks },
  { 0, 0, 0, 0, ScoreSemReqSeizeWait_Post_Status_NA,
    ScoreSemReqSeizeWait_Post_Enqueued_FIFO,
    ScoreSemReqSeizeWait_Post_Count_Nop, ScoreSemReqSeizeWait_Post_Timer_No },
  { 0, 0, 0, 0, ScoreSemReqSeizeWait_Post_Status_NA,
    ScoreSemReqSeizeWait_Post_Enqueued_Priority,
    ScoreSemReqSeizeWait_Post_Count_Nop, ScoreSemReqSeizeWait_Post_Timer_Ticks },
  { 0, 0, 0, 0, ScoreSemReqSeizeWait_Post_Status_NA,
    ScoreSemReqSeizeWait_Post_Enqueued_Priority,
    ScoreSemReqSeizeWait_Post_Count_Nop, ScoreSemReqSeizeWait_Post_Timer_No }
};

static const uint8_t
ScoreSemReqSeizeWait_Map[] = {
  1, 0, 2, 0, 3, 0, 4, 0
};

/* clang-format on */

static size_t ScoreSemReqSeizeWait_Scope( void *arg, char *buf, size_t n )
{
  ScoreSemReqSeizeWait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreSemReqSeizeWait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreSemReqSeizeWait_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreSemReqSeizeWait_Scope,
  .initial_context = &ScoreSemReqSeizeWait_Instance
};

static const uint8_t ScoreSemReqSeizeWait_Weights[] = { 4, 2, 1 };

static void ScoreSemReqSeizeWait_Skip(
  ScoreSemReqSeizeWait_Context *ctx,
  size_t                        index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreSemReqSeizeWait_Pre_Timeout_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreSemReqSeizeWait_Pre_Count_NA - 1;
      break;
  }
}

static inline ScoreSemReqSeizeWait_Entry ScoreSemReqSeizeWait_PopEntry(
  ScoreSemReqSeizeWait_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreSemReqSeizeWait_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreSemReqSeizeWait_Entries[ ScoreSemReqSeizeWait_Map[ index ] ];
}

static void ScoreSemReqSeizeWait_TestVariant(
  ScoreSemReqSeizeWait_Context *ctx
)
{
  ScoreSemReqSeizeWait_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreSemReqSeizeWait_Skip( ctx, 0 );
    return;
  }

  ScoreSemReqSeizeWait_Pre_Timeout_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreSemReqSeizeWait_Skip( ctx, 1 );
    return;
  }

  ScoreSemReqSeizeWait_Pre_Count_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreSemReqSeizeWait_Action();
  ScoreSemReqSeizeWait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreSemReqSeizeWait_Post_Enqueued_Check(
    ctx,
    ctx->Map.entry.Post_Enqueued
  );
  ScoreSemReqSeizeWait_Post_Count_Check( ctx, ctx->Map.entry.Post_Count );
  ScoreSemReqSeizeWait_Post_Timer_Check( ctx->Map.entry.Post_Timer );
}

static T_fixture_node ScoreSemReqSeizeWait_Node;

static T_remark ScoreSemReqSeizeWait_Remark = {
  .next = NULL,
  .remark = "ScoreSemReqSeizeWait"
};

void ScoreSemReqSeizeWait_Run( TQSemContext *tq_ctx )
{
  ScoreSemReqSeizeWait_Context *ctx;

  ctx = &ScoreSemReqSeizeWait_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreSemReqSeizeWait_Node,
    &ScoreSemReqSeizeWait_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreSemReqSeizeWait_Pre_Discipline_FIFO;
    ctx->Map.pcs[ 0 ] < ScoreSemReqSeizeWait_Pre_Discipline_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreSemReqSeizeWait_Pre_Timeout_Ticks;
      ctx->Map.pcs[ 1 ] < ScoreSemReqSeizeWait_Pre_Timeout_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreSemReqSeizeWait_Pre_Count_Zero;
        ctx->Map.pcs[ 2 ] < ScoreSemReqSeizeWait_Pre_Count_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreSemReqSeizeWait_PopEntry( ctx );
        ScoreSemReqSeizeWait_Prepare( ctx );
        ScoreSemReqSeizeWait_TestVariant( ctx );
        ScoreSemReqSeizeWait_Cleanup( ctx );
      }
    }
  }

  T_add_remark( &ScoreSemReqSeizeWait_Remark );
  T_pop_fixture();
}

/** @} */
