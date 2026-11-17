/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreIsrReqLockRelease
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

#include "tr-isr-lock-release.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreIsrReqLockRelease spec:/score/isr/req/lock-release
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_ISRRestoreLevel_NA : 1;
  uint16_t Pre_ISRLevel_NA : 1;
  uint16_t Pre_Acquire_NA : 1;
  uint16_t Pre_SavedISRLevel_NA : 1;
  uint16_t Pre_Waiters_NA : 1;
  uint16_t Post_ISRLevel : 2;
  uint16_t Post_Lock : 2;
} ScoreIsrReqLockRelease_Entry;

/**
 * @brief Test context for spec:/score/isr/req/lock-release test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then maskable interrupts shall be enabled
   *   before the acquire.
   */
  bool isr_enabled;

  /**
   * @brief If this member is true, then maskable interrupts shall be enabled
   *   at the release.
   */
  bool isr_enabled_at_release;

  /**
   * @brief If this member is true, then the lock shall be acquired by a
   *   separate disable of maskable interrupts and the acquire which does not
   *   change their status.
   */
  bool disable_acquire_isr;

  /**
   * @brief If this member is true, then another processor shall wait for the
   *   lock before the release.
   */
  bool waiter;

  /**
   * @brief This member contains the index of the processor which waits for the
   *   lock.
   */
  uint32_t other_cpu_index;

  /**
   * @brief This member contains the lock context of the acquire and the
   *   release.
   */
  ISR_lock_Context lock_context;

  /**
   * @brief This member is true, if maskable interrupts were enabled after the
   *   release.
   */
  bool isr_enabled_after;

  /**
   * @brief This member is true, if the lock was free right after the release.
   */
  bool free_after;

  /**
   * @brief This member contains the ticket of the processor which waits for
   *   the lock.
   */
  unsigned int waiter_ticket;

  /**
   * @brief This member contains the ticket of the owner of the lock right
   *   after the release.
   */
  unsigned int owner_ticket;

  /**
   * @brief This member contains the worker which waits for the lock on another
   *   processor.
   */
  ISRLockWorker worker;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreIsrReqLockRelease_Run() parameter.
   */
  ISRLockContext *lock_ctx;

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
    ScoreIsrReqLockRelease_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreIsrReqLockRelease_Context;

static ScoreIsrReqLockRelease_Context ScoreIsrReqLockRelease_Instance;

static const char *const ScoreIsrReqLockRelease_PreDesc_ISRRestoreLevel[] =
  { "ToContext", "ToObject", "None", "NA" };

static const char *const ScoreIsrReqLockRelease_PreDesc_ISRLevel[] =
  { "Enabled", "Disabled", "NA" };

static const char *const ScoreIsrReqLockRelease_PreDesc_Acquire[] =
  { "Matching", "DisableAcquireIsr", "NA" };

static const char *const ScoreIsrReqLockRelease_PreDesc_SavedISRLevel[] =
  { "Enabled", "Disabled", "NA" };

static const char *const ScoreIsrReqLockRelease_PreDesc_Waiters[] =
  { "None", "Some", "NA" };

static const char *const *const ScoreIsrReqLockRelease_PreDesc[] = {
  ScoreIsrReqLockRelease_PreDesc_ISRRestoreLevel,
  ScoreIsrReqLockRelease_PreDesc_ISRLevel,
  ScoreIsrReqLockRelease_PreDesc_Acquire,
  ScoreIsrReqLockRelease_PreDesc_SavedISRLevel,
  ScoreIsrReqLockRelease_PreDesc_Waiters,
  NULL
};

static void ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_Prepare(
  ScoreIsrReqLockRelease_Context            *ctx,
  ScoreIsrReqLockRelease_Pre_ISRRestoreLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_ToContext: {
      /*
       * Where the lock disables maskable interrupts at the acquire and
       * restores their status from the lock context at the release.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_TO_CONTEXT ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_ToObject: {
      /*
       * Where the lock disables maskable interrupts at the acquire and
       * restores their status from the lock object at the release.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_TO_OBJECT ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_None: {
      /*
       * Where the lock does not change the status of maskable interrupts.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_NONE ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockRelease_Pre_ISRLevel_Prepare(
  ScoreIsrReqLockRelease_Context     *ctx,
  ScoreIsrReqLockRelease_Pre_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockRelease_Pre_ISRLevel_Enabled: {
      /*
       * While maskable interrupts are enabled on the calling processor.
       */
      ctx->isr_enabled_at_release = true;
      break;
    }

    case ScoreIsrReqLockRelease_Pre_ISRLevel_Disabled: {
      /*
       * While maskable interrupts are disabled on the calling processor.
       */
      ctx->isr_enabled_at_release = false;
      break;
    }

    case ScoreIsrReqLockRelease_Pre_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockRelease_Pre_Acquire_Prepare(
  ScoreIsrReqLockRelease_Context    *ctx,
  ScoreIsrReqLockRelease_Pre_Acquire state
)
{
  switch ( state ) {
    case ScoreIsrReqLockRelease_Pre_Acquire_Matching: {
      /*
       * While the calling processor acquired the lock by the acquire which
       * belongs to the release.
       */
      ctx->disable_acquire_isr = false;
      break;
    }

    case ScoreIsrReqLockRelease_Pre_Acquire_DisableAcquireIsr: {
      /*
       * While the calling processor disabled maskable interrupts by a separate
       * call and then acquired the lock by the acquire which does not change
       * the status of maskable interrupts.
       */
      ctx->disable_acquire_isr = true;
      break;
    }

    case ScoreIsrReqLockRelease_Pre_Acquire_NA:
      break;
  }
}

static void ScoreIsrReqLockRelease_Pre_SavedISRLevel_Prepare(
  ScoreIsrReqLockRelease_Context          *ctx,
  ScoreIsrReqLockRelease_Pre_SavedISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockRelease_Pre_SavedISRLevel_Enabled: {
      /*
       * While maskable interrupts were enabled on the calling processor before
       * the calls which acquired the lock.
       */
      ctx->isr_enabled = true;
      break;
    }

    case ScoreIsrReqLockRelease_Pre_SavedISRLevel_Disabled: {
      /*
       * While maskable interrupts were disabled on the calling processor
       * before the calls which acquired the lock.
       */
      ctx->isr_enabled = false;
      break;
    }

    case ScoreIsrReqLockRelease_Pre_SavedISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockRelease_Pre_Waiters_Prepare(
  ScoreIsrReqLockRelease_Context    *ctx,
  ScoreIsrReqLockRelease_Pre_Waiters state
)
{
  switch ( state ) {
    case ScoreIsrReqLockRelease_Pre_Waiters_None: {
      /*
       * While no other processor waits for the lock.
       */
      ctx->waiter = false;
      break;
    }

    case ScoreIsrReqLockRelease_Pre_Waiters_Some: {
      /*
       * While another processor waits for the lock.
       */
      ctx->waiter = true;

      #if defined( RTEMS_SMP )
      if ( ISRLockGetOtherProcessors( &ctx->other_cpu_index, 1 ) == 0 ) {
        ctx->Map.skip = true;
      }
      #endif
      break;
    }

    case ScoreIsrReqLockRelease_Pre_Waiters_NA:
      break;
  }
}

static void ScoreIsrReqLockRelease_Post_ISRLevel_Check(
  ScoreIsrReqLockRelease_Context      *ctx,
  ScoreIsrReqLockRelease_Post_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockRelease_Post_ISRLevel_Enabled: {
      /*
       * Maskable interrupts shall be enabled on the calling processor.
       */
      T_true( ctx->isr_enabled_after );
      break;
    }

    case ScoreIsrReqLockRelease_Post_ISRLevel_Disabled: {
      /*
       * Maskable interrupts shall be disabled on the calling processor.
       */
      T_false( ctx->isr_enabled_after );
      break;
    }

    case ScoreIsrReqLockRelease_Post_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockRelease_Post_Lock_Check(
  ScoreIsrReqLockRelease_Context  *ctx,
  ScoreIsrReqLockRelease_Post_Lock state
)
{
  switch ( state ) {
    case ScoreIsrReqLockRelease_Post_Lock_Free: {
      /*
       * The lock shall be free.
       */
      T_true( ctx->free_after );
      break;
    }

    case ScoreIsrReqLockRelease_Post_Lock_Waiter: {
      /*
       * The processor which waits for the lock shall own the lock.
       */
      T_false( ctx->free_after );
      T_eq_uint( ctx->owner_ticket, ctx->waiter_ticket );
      break;
    }

    case ScoreIsrReqLockRelease_Post_Lock_NA:
      break;
  }
}

static void ScoreIsrReqLockRelease_Prepare(
  ScoreIsrReqLockRelease_Context *ctx
)
{
  ( *ctx->lock_ctx->initialize )( ctx->lock_ctx );
  ctx->isr_enabled_after = false;
  ctx->free_after = false;
  ctx->waiter_ticket = 0;
  ctx->owner_ticket = 1;
}

static void ScoreIsrReqLockRelease_Action(
  ScoreIsrReqLockRelease_Context *ctx
)
{
  ISR_Level level;

  level = 0;

  if ( !ctx->isr_enabled ) {
    _ISR_Local_disable( level );
  }

  if ( ctx->disable_acquire_isr ) {
    ( *ctx->lock_ctx->disable_acquire_isr )(
      ctx->lock_ctx,
      &ctx->lock_context
    );
  } else {
    ( *ctx->lock_ctx->acquire )( ctx->lock_ctx, &ctx->lock_context );
  }

  #if defined( RTEMS_SMP )
  if ( ctx->waiter ) {
    const SMP_ticket_lock_Control *ticket_lock;

    ticket_lock = ( *ctx->lock_ctx->get_ticket_lock )( ctx->lock_ctx );
    ISRLockStartWorker(
      ctx->lock_ctx,
      &ctx->worker,
      ctx->other_cpu_index,
      0,
      true
    );
    TicketLockWaitForOthers( ticket_lock, 1 );
    ctx->waiter_ticket = TicketLockGetLastTicket( ticket_lock );
  }
  #endif

  T_quiet_eq( AreInterruptsEnabled(), ctx->isr_enabled_at_release );
  ( *ctx->lock_ctx->release )( ctx->lock_ctx, &ctx->lock_context );
  ctx->free_after = ISRLockIsFree( ctx->lock_ctx );
  ctx->isr_enabled_after = AreInterruptsEnabled();

  #if defined( RTEMS_SMP )
  if ( ctx->waiter ) {
    ctx->owner_ticket = TicketLockGetOwnerTicket(
      ( *ctx->lock_ctx->get_ticket_lock )( ctx->lock_ctx )
    );
  }
  #endif

  #if defined( RTEMS_SMP )
  if ( ctx->waiter ) {
    ISRLockClearWorkerHold( &ctx->worker );
  }
  #endif

  if (
    ctx->disable_acquire_isr && ctx->lock_ctx->restore == ISR_LOCK_RESTORE_NONE
  ) {
    ( *ctx->lock_ctx->interrupt_enable )( ctx->lock_ctx, &ctx->lock_context );
  }

  if ( !ctx->isr_enabled ) {
    _ISR_Local_enable( level );
  }

  #if defined( RTEMS_SMP )
  if ( ctx->waiter ) {
    ISRLockWaitForWorker( &ctx->worker );
  }
  #endif
}

static void ScoreIsrReqLockRelease_Cleanup(
  ScoreIsrReqLockRelease_Context *ctx
)
{
  ( *ctx->lock_ctx->destroy )( ctx->lock_ctx );
}

/* clang-format off */

static const ScoreIsrReqLockRelease_Entry
ScoreIsrReqLockRelease_Entries[] = {
  { 1, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_NA,
    ScoreIsrReqLockRelease_Post_Lock_NA },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_Disabled,
    ScoreIsrReqLockRelease_Post_Lock_Free },
#else
  { 0, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_Disabled,
    ScoreIsrReqLockRelease_Post_Lock_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_Disabled,
    ScoreIsrReqLockRelease_Post_Lock_Waiter },
#else
  { 1, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_NA,
    ScoreIsrReqLockRelease_Post_Lock_NA },
#endif
  { 1, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_NA,
    ScoreIsrReqLockRelease_Post_Lock_NA },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_Enabled,
    ScoreIsrReqLockRelease_Post_Lock_Free },
#else
  { 0, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_Enabled,
    ScoreIsrReqLockRelease_Post_Lock_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_Enabled,
    ScoreIsrReqLockRelease_Post_Lock_Waiter },
#else
  { 1, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_NA,
    ScoreIsrReqLockRelease_Post_Lock_NA },
#endif
  { 1, 0, 0, 0, 0, 0, ScoreIsrReqLockRelease_Post_ISRLevel_NA,
    ScoreIsrReqLockRelease_Post_Lock_NA }
};

static const uint8_t
ScoreIsrReqLockRelease_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 1, 2, 4, 5, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5,
  1, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 1, 2, 1, 2, 1, 2
};

/* clang-format on */

static size_t ScoreIsrReqLockRelease_Scope( void *arg, char *buf, size_t n )
{
  ScoreIsrReqLockRelease_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreIsrReqLockRelease_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreIsrReqLockRelease_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreIsrReqLockRelease_Scope,
  .initial_context = &ScoreIsrReqLockRelease_Instance
};

static const uint8_t ScoreIsrReqLockRelease_Weights[] = { 16, 8, 4, 2, 1 };

static void ScoreIsrReqLockRelease_Skip(
  ScoreIsrReqLockRelease_Context *ctx,
  size_t                          index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreIsrReqLockRelease_Pre_ISRLevel_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreIsrReqLockRelease_Pre_Acquire_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pcs[ 3 ] = ScoreIsrReqLockRelease_Pre_SavedISRLevel_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pcs[ 4 ] = ScoreIsrReqLockRelease_Pre_Waiters_NA - 1;
      break;
  }
}

static inline ScoreIsrReqLockRelease_Entry ScoreIsrReqLockRelease_PopEntry(
  ScoreIsrReqLockRelease_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 5; ++i ) {
      index += ScoreIsrReqLockRelease_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreIsrReqLockRelease_Entries[ ScoreIsrReqLockRelease_Map[ index ] ];
}

static void ScoreIsrReqLockRelease_TestVariant(
  ScoreIsrReqLockRelease_Context *ctx
)
{
  ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreIsrReqLockRelease_Skip( ctx, 0 );
    return;
  }

  ScoreIsrReqLockRelease_Pre_ISRLevel_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreIsrReqLockRelease_Pre_Acquire_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreIsrReqLockRelease_Pre_SavedISRLevel_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  ScoreIsrReqLockRelease_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 4 ] );

  if ( ctx->Map.skip ) {
    ScoreIsrReqLockRelease_Skip( ctx, 4 );
    return;
  }

  ScoreIsrReqLockRelease_Action( ctx );
  ScoreIsrReqLockRelease_Post_ISRLevel_Check(
    ctx,
    ctx->Map.entry.Post_ISRLevel
  );
  ScoreIsrReqLockRelease_Post_Lock_Check( ctx, ctx->Map.entry.Post_Lock );
}

static T_fixture_node ScoreIsrReqLockRelease_Node;

static T_remark ScoreIsrReqLockRelease_Remark = {
  .next = NULL,
  .remark = "ScoreIsrReqLockRelease"
};

void ScoreIsrReqLockRelease_Run( ISRLockContext *lock_ctx )
{
  ScoreIsrReqLockRelease_Context *ctx;

  ctx = &ScoreIsrReqLockRelease_Instance;
  ctx->lock_ctx = lock_ctx;

  ctx = T_push_fixture(
    &ScoreIsrReqLockRelease_Node,
    &ScoreIsrReqLockRelease_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_ToContext;
    ctx->Map.pcs[ 0 ] < ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreIsrReqLockRelease_Pre_ISRLevel_Enabled;
      ctx->Map.pcs[ 1 ] < ScoreIsrReqLockRelease_Pre_ISRLevel_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreIsrReqLockRelease_Pre_Acquire_Matching;
        ctx->Map.pcs[ 2 ] < ScoreIsrReqLockRelease_Pre_Acquire_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = ScoreIsrReqLockRelease_Pre_SavedISRLevel_Enabled;
          ctx->Map.pcs[ 3 ] < ScoreIsrReqLockRelease_Pre_SavedISRLevel_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = ScoreIsrReqLockRelease_Pre_Waiters_None;
            ctx->Map.pcs[ 4 ] < ScoreIsrReqLockRelease_Pre_Waiters_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = ScoreIsrReqLockRelease_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            ScoreIsrReqLockRelease_Prepare( ctx );
            ScoreIsrReqLockRelease_TestVariant( ctx );
            ScoreIsrReqLockRelease_Cleanup( ctx );
          }
        }
      }
    }
  }

  T_add_remark( &ScoreIsrReqLockRelease_Remark );
  T_pop_fixture();
}

/** @} */
