/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreIsrReqLockFifo
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

#include "tr-isr-lock-fifo.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreIsrReqLockFifo spec:/score/isr/req/lock-fifo
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_ISRRestoreLevel_NA : 1;
  uint8_t Pre_Waiters_NA : 1;
  uint8_t Post_Owner : 1;
} ScoreIsrReqLockFifo_Entry;

/**
 * @brief Test context for spec:/score/isr/req/lock-fifo test case.
 */
typedef struct {
  /**
   * @brief This member contains the indices of the processors of the workers.
   */
  uint32_t cpu_indices[ 8 ];

  /**
   * @brief This member contains the count of workers.
   */
  uint32_t worker_count;

  /**
   * @brief This member contains the workers which wait for the lock.
   */
  ISRLockWorker workers[ 8 ];

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreIsrReqLockFifo_Run() parameter.
   */
  ISRLockContext *lock_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    ScoreIsrReqLockFifo_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreIsrReqLockFifo_Context;

static ScoreIsrReqLockFifo_Context ScoreIsrReqLockFifo_Instance;

static const char *const ScoreIsrReqLockFifo_PreDesc_ISRRestoreLevel[] =
  { "ToContext", "ToObject", "None", "NA" };

static const char *const ScoreIsrReqLockFifo_PreDesc_Waiters[] = {
  "More",
  "NA"
};

static const char *const *const ScoreIsrReqLockFifo_PreDesc[] = {
  ScoreIsrReqLockFifo_PreDesc_ISRRestoreLevel,
  ScoreIsrReqLockFifo_PreDesc_Waiters,
  NULL
};

static void ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_Prepare(
  ScoreIsrReqLockFifo_Context            *ctx,
  ScoreIsrReqLockFifo_Pre_ISRRestoreLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_ToContext: {
      /*
       * Where the lock disables maskable interrupts at the acquire and
       * restores their status from the lock context at the release.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_TO_CONTEXT ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_ToObject: {
      /*
       * Where the lock disables maskable interrupts at the acquire and
       * restores their status from the lock object at the release.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_TO_OBJECT ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_None: {
      /*
       * Where the lock does not change the status of maskable interrupts.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_NONE ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockFifo_Pre_Waiters_Prepare(
  ScoreIsrReqLockFifo_Context    *ctx,
  ScoreIsrReqLockFifo_Pre_Waiters state
)
{
  switch ( state ) {
    case ScoreIsrReqLockFifo_Pre_Waiters_More: {
      /*
       * While at least two other processors wait for the lock.
       */
      if ( ctx->worker_count < 2 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockFifo_Pre_Waiters_NA:
      break;
  }
}

static void ScoreIsrReqLockFifo_Post_Owner_Check(
  ScoreIsrReqLockFifo_Context   *ctx,
  ScoreIsrReqLockFifo_Post_Owner state
)
{
  unsigned int i;

  switch ( state ) {
    case ScoreIsrReqLockFifo_Post_Owner_FirstWaiter: {
      /*
       * The processor which started to wait first shall own the lock.
       */
      for ( i = 0; i < ctx->worker_count; ++i ) {
        T_eq_uint( ctx->workers[ i ].sequence, i );
      }
      break;
    }

    case ScoreIsrReqLockFifo_Post_Owner_NA:
      break;
  }
}

static void ScoreIsrReqLockFifo_Prepare( ScoreIsrReqLockFifo_Context *ctx )
{
  ( *ctx->lock_ctx->initialize )( ctx->lock_ctx );
  ctx->worker_count = ISRLockGetOtherProcessors(
    ctx->cpu_indices,
    RTEMS_ARRAY_SIZE( ctx->cpu_indices )
  );
}

static void ScoreIsrReqLockFifo_Action( ScoreIsrReqLockFifo_Context *ctx )
{
  const SMP_ticket_lock_Control *ticket_lock;
  ISR_Level                      level;
  ISR_lock_Context               lock_context;
  unsigned int                   i;

  ticket_lock = ( *ctx->lock_ctx->get_ticket_lock )( ctx->lock_ctx );
  ctx->lock_ctx->sequence = 0;
  _ISR_Local_disable( level );
  ( *ctx->lock_ctx->acquire )( ctx->lock_ctx, &lock_context );

  for ( i = 0; i < ctx->worker_count; ++i ) {
    ISRLockStartWorker(
      ctx->lock_ctx,
      &ctx->workers[ i ],
      ctx->cpu_indices[ i ],
      0,
      false
    );
    TicketLockWaitForOthers( ticket_lock, i + 1 );
  }

  ( *ctx->lock_ctx->release )( ctx->lock_ctx, &lock_context );
  _ISR_Local_enable( level );

  for ( i = 0; i < ctx->worker_count; ++i ) {
    ISRLockWaitForWorker( &ctx->workers[ i ] );
  }
}

static void ScoreIsrReqLockFifo_Cleanup( ScoreIsrReqLockFifo_Context *ctx )
{
  ( *ctx->lock_ctx->destroy )( ctx->lock_ctx );
}

/* clang-format off */

static const ScoreIsrReqLockFifo_Entry
ScoreIsrReqLockFifo_Entries[] = {
  { 0, 0, 0, ScoreIsrReqLockFifo_Post_Owner_FirstWaiter }
};

static const uint8_t
ScoreIsrReqLockFifo_Map[] = {
  0, 0, 0
};

/* clang-format on */

static size_t ScoreIsrReqLockFifo_Scope( void *arg, char *buf, size_t n )
{
  ScoreIsrReqLockFifo_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreIsrReqLockFifo_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreIsrReqLockFifo_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreIsrReqLockFifo_Scope,
  .initial_context = &ScoreIsrReqLockFifo_Instance
};

static const uint8_t ScoreIsrReqLockFifo_Weights[] = { 1, 1 };

static void ScoreIsrReqLockFifo_Skip(
  ScoreIsrReqLockFifo_Context *ctx,
  size_t                       index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreIsrReqLockFifo_Pre_Waiters_NA - 1;
      break;
  }
}

static inline ScoreIsrReqLockFifo_Entry ScoreIsrReqLockFifo_PopEntry(
  ScoreIsrReqLockFifo_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 2; ++i ) {
      index += ScoreIsrReqLockFifo_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreIsrReqLockFifo_Entries[ ScoreIsrReqLockFifo_Map[ index ] ];
}

static void ScoreIsrReqLockFifo_TestVariant( ScoreIsrReqLockFifo_Context *ctx )
{
  ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreIsrReqLockFifo_Skip( ctx, 0 );
    return;
  }

  ScoreIsrReqLockFifo_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreIsrReqLockFifo_Skip( ctx, 1 );
    return;
  }

  ScoreIsrReqLockFifo_Action( ctx );
  ScoreIsrReqLockFifo_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
}

static T_fixture_node ScoreIsrReqLockFifo_Node;

static T_remark ScoreIsrReqLockFifo_Remark = {
  .next = NULL,
  .remark = "ScoreIsrReqLockFifo"
};

void ScoreIsrReqLockFifo_Run( ISRLockContext *lock_ctx )
{
  ScoreIsrReqLockFifo_Context *ctx;

  ctx = &ScoreIsrReqLockFifo_Instance;
  ctx->lock_ctx = lock_ctx;

  ctx = T_push_fixture(
    &ScoreIsrReqLockFifo_Node,
    &ScoreIsrReqLockFifo_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_ToContext;
    ctx->Map.pcs[ 0 ] < ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreIsrReqLockFifo_Pre_Waiters_More;
      ctx->Map.pcs[ 1 ] < ScoreIsrReqLockFifo_Pre_Waiters_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = ScoreIsrReqLockFifo_PopEntry( ctx );
      ScoreIsrReqLockFifo_Prepare( ctx );
      ScoreIsrReqLockFifo_TestVariant( ctx );
      ScoreIsrReqLockFifo_Cleanup( ctx );
    }
  }

  T_add_remark( &ScoreIsrReqLockFifo_Remark );
  T_pop_fixture();
}

/** @} */
