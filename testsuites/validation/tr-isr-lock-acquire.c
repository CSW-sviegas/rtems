/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreIsrReqLockAcquire
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

#include "tr-isr-lock-acquire.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreIsrReqLockAcquire spec:/score/isr/req/lock-acquire
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_ISRRestoreLevel_NA : 1;
  uint16_t Pre_ISRLevel_NA : 1;
  uint16_t Pre_Owner_NA : 1;
  uint16_t Post_ISRLevel : 1;
  uint16_t Post_Saved : 2;
  uint16_t Post_Owner : 1;
  uint16_t Post_Wait : 1;
} ScoreIsrReqLockAcquire_Entry;

/**
 * @brief Test context for spec:/score/isr/req/lock-acquire test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then maskable interrupts shall be enabled
   *   before the acquire.
   */
  bool isr_enabled;

  /**
   * @brief If this member is true, then another processor shall own the lock
   *   before the acquire.
   */
  bool other;

  /**
   * @brief This member contains the index of the processor which owns the lock
   *   before the acquire.
   */
  uint32_t other_cpu_index;

  /**
   * @brief This member contains the lock context of the acquire.
   */
  ISR_lock_Context lock_context;

  /**
   * @brief This member is true, if maskable interrupts were enabled after the
   *   acquire.
   */
  bool isr_enabled_after;

  /**
   * @brief This member is true, if the lock was free after the acquire.
   */
  bool free_after;

  /**
   * @brief This member is true, if the saved status of maskable interrupts is
   *   enabled.
   */
  bool saved_enabled;

  /**
   * @brief This member is true, if the owner released the lock before the
   *   acquire returned.
   */
  bool owner_released;

  /**
   * @brief This member contains the worker which owns the lock on another
   *   processor.
   */
  ISRLockWorker worker;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreIsrReqLockAcquire_Run() parameter.
   */
  ISRLockContext *lock_ctx;

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
    ScoreIsrReqLockAcquire_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreIsrReqLockAcquire_Context;

static ScoreIsrReqLockAcquire_Context ScoreIsrReqLockAcquire_Instance;

static const char *const ScoreIsrReqLockAcquire_PreDesc_ISRRestoreLevel[] =
  { "ToContext", "ToObject", "None", "NA" };

static const char *const ScoreIsrReqLockAcquire_PreDesc_ISRLevel[] =
  { "Enabled", "Disabled", "NA" };

static const char *const ScoreIsrReqLockAcquire_PreDesc_Owner[] =
  { "Free", "Other", "NA" };

static const char *const *const ScoreIsrReqLockAcquire_PreDesc[] = {
  ScoreIsrReqLockAcquire_PreDesc_ISRRestoreLevel,
  ScoreIsrReqLockAcquire_PreDesc_ISRLevel,
  ScoreIsrReqLockAcquire_PreDesc_Owner,
  NULL
};

static void ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel_Prepare(
  ScoreIsrReqLockAcquire_Context            *ctx,
  ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel_ToContext: {
      /*
       * Where the lock disables maskable interrupts at the acquire and
       * restores their status from the lock context at the release.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_TO_CONTEXT ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel_ToObject: {
      /*
       * Where the lock disables maskable interrupts at the acquire and
       * restores their status from the lock object at the release.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_TO_OBJECT ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel_None: {
      /*
       * Where the lock does not change the status of maskable interrupts.
       */
      if ( ctx->lock_ctx->restore != ISR_LOCK_RESTORE_NONE ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockAcquire_Pre_ISRLevel_Prepare(
  ScoreIsrReqLockAcquire_Context     *ctx,
  ScoreIsrReqLockAcquire_Pre_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockAcquire_Pre_ISRLevel_Enabled: {
      /*
       * While maskable interrupts are enabled on the calling processor.
       */
      ctx->isr_enabled = true;
      break;
    }

    case ScoreIsrReqLockAcquire_Pre_ISRLevel_Disabled: {
      /*
       * While maskable interrupts are disabled on the calling processor.
       */
      ctx->isr_enabled = false;
      break;
    }

    case ScoreIsrReqLockAcquire_Pre_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockAcquire_Pre_Owner_Prepare(
  ScoreIsrReqLockAcquire_Context  *ctx,
  ScoreIsrReqLockAcquire_Pre_Owner state
)
{
  switch ( state ) {
    case ScoreIsrReqLockAcquire_Pre_Owner_Free: {
      /*
       * While no processor owns the lock.
       */
      ctx->other = false;
      break;
    }

    case ScoreIsrReqLockAcquire_Pre_Owner_Other: {
      /*
       * While another processor owns the lock.
       */
      ctx->other = true;

      #if defined( RTEMS_SMP )
      if ( ISRLockGetOtherProcessors( &ctx->other_cpu_index, 1 ) == 0 ) {
        ctx->Map.skip = true;
      }
      #endif
      break;
    }

    case ScoreIsrReqLockAcquire_Pre_Owner_NA:
      break;
  }
}

static void ScoreIsrReqLockAcquire_Post_ISRLevel_Check(
  ScoreIsrReqLockAcquire_Context      *ctx,
  ScoreIsrReqLockAcquire_Post_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled: {
      /*
       * Maskable interrupts shall be disabled on the calling processor.
       */
      T_false( ctx->isr_enabled_after );
      break;
    }

    case ScoreIsrReqLockAcquire_Post_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockAcquire_Post_Saved_Check(
  ScoreIsrReqLockAcquire_Context   *ctx,
  ScoreIsrReqLockAcquire_Post_Saved state
)
{
  switch ( state ) {
    case ScoreIsrReqLockAcquire_Post_Saved_Enabled: {
      /*
       * The lock shall save that maskable interrupts were enabled.
       */
      T_true( ctx->saved_enabled );
      break;
    }

    case ScoreIsrReqLockAcquire_Post_Saved_Disabled: {
      /*
       * The lock shall save that maskable interrupts were disabled.
       */
      T_false( ctx->saved_enabled );
      break;
    }

    case ScoreIsrReqLockAcquire_Post_Saved_NA:
      break;
  }
}

static void ScoreIsrReqLockAcquire_Post_Owner_Check(
  ScoreIsrReqLockAcquire_Context   *ctx,
  ScoreIsrReqLockAcquire_Post_Owner state
)
{
  switch ( state ) {
    case ScoreIsrReqLockAcquire_Post_Owner_Caller: {
      /*
       * The calling processor shall own the lock.
       */
      T_false( ctx->free_after );
      break;
    }

    case ScoreIsrReqLockAcquire_Post_Owner_NA:
      break;
  }
}

static void ScoreIsrReqLockAcquire_Post_Wait_Check(
  ScoreIsrReqLockAcquire_Context  *ctx,
  ScoreIsrReqLockAcquire_Post_Wait state
)
{
  switch ( state ) {
    case ScoreIsrReqLockAcquire_Post_Wait_UntilReleased: {
      /*
       * The calling processor shall wait until the owner releases the lock.
       */
      T_true( ctx->owner_released );
      break;
    }

    case ScoreIsrReqLockAcquire_Post_Wait_NA:
      break;
  }
}

static void ScoreIsrReqLockAcquire_Prepare(
  ScoreIsrReqLockAcquire_Context *ctx
)
{
  ( *ctx->lock_ctx->initialize )( ctx->lock_ctx );
  ctx->isr_enabled_after = true;
  ctx->free_after = true;
  ctx->saved_enabled = false;
  ctx->owner_released = false;
}

static void ScoreIsrReqLockAcquire_Action(
  ScoreIsrReqLockAcquire_Context *ctx
)
{
  ISR_Level level;

  level = 0;

  if ( !ctx->isr_enabled ) {
    _ISR_Local_disable( level );
  }

  #if defined( RTEMS_SMP )
  if ( ctx->other ) {
    ISRLockStartWorker(
      ctx->lock_ctx,
      &ctx->worker,
      ctx->other_cpu_index,
      1,
      false
    );
    TicketLockWaitForOwned(
      ( *ctx->lock_ctx->get_ticket_lock )( ctx->lock_ctx )
    );
  }
  #endif

  ( *ctx->lock_ctx->acquire )( ctx->lock_ctx, &ctx->lock_context );
  ctx->isr_enabled_after = AreInterruptsEnabled();
  ctx->free_after = ISRLockIsFree( ctx->lock_ctx );

  #if defined( RTEMS_SMP )
  if ( ctx->other ) {
    ctx->owner_released = ISRLockWorkerReleased( &ctx->worker );
  }
  #endif

  if ( ctx->lock_ctx->get_saved_level != NULL ) {
    ctx->saved_enabled = _ISR_Is_enabled(
      ( *ctx->lock_ctx->get_saved_level )( ctx->lock_ctx, &ctx->lock_context )
    );
  }

  ( *ctx->lock_ctx->release )( ctx->lock_ctx, &ctx->lock_context );

  if ( !ctx->isr_enabled ) {
    _ISR_Local_enable( level );
  }

  #if defined( RTEMS_SMP )
  if ( ctx->other ) {
    ISRLockWaitForWorker( &ctx->worker );
  }
  #endif
}

static void ScoreIsrReqLockAcquire_Cleanup(
  ScoreIsrReqLockAcquire_Context *ctx
)
{
  ( *ctx->lock_ctx->destroy )( ctx->lock_ctx );
}

/* clang-format off */

static const ScoreIsrReqLockAcquire_Entry
ScoreIsrReqLockAcquire_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_Enabled,
    ScoreIsrReqLockAcquire_Post_Owner_Caller,
    ScoreIsrReqLockAcquire_Post_Wait_NA },
#else
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_Enabled,
    ScoreIsrReqLockAcquire_Post_Owner_NA, ScoreIsrReqLockAcquire_Post_Wait_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_Enabled,
    ScoreIsrReqLockAcquire_Post_Owner_Caller,
    ScoreIsrReqLockAcquire_Post_Wait_UntilReleased },
#else
  { 1, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_NA,
    ScoreIsrReqLockAcquire_Post_Saved_NA, ScoreIsrReqLockAcquire_Post_Owner_NA,
    ScoreIsrReqLockAcquire_Post_Wait_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_Disabled,
    ScoreIsrReqLockAcquire_Post_Owner_Caller,
    ScoreIsrReqLockAcquire_Post_Wait_NA },
#else
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_Disabled,
    ScoreIsrReqLockAcquire_Post_Owner_NA, ScoreIsrReqLockAcquire_Post_Wait_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_Disabled,
    ScoreIsrReqLockAcquire_Post_Owner_Caller,
    ScoreIsrReqLockAcquire_Post_Wait_UntilReleased },
#else
  { 1, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_NA,
    ScoreIsrReqLockAcquire_Post_Saved_NA, ScoreIsrReqLockAcquire_Post_Owner_NA,
    ScoreIsrReqLockAcquire_Post_Wait_NA },
#endif
  { 1, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_NA,
    ScoreIsrReqLockAcquire_Post_Saved_NA, ScoreIsrReqLockAcquire_Post_Owner_NA,
    ScoreIsrReqLockAcquire_Post_Wait_NA },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_NA,
    ScoreIsrReqLockAcquire_Post_Owner_Caller,
    ScoreIsrReqLockAcquire_Post_Wait_NA },
#else
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_NA, ScoreIsrReqLockAcquire_Post_Owner_NA,
    ScoreIsrReqLockAcquire_Post_Wait_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_Disabled,
    ScoreIsrReqLockAcquire_Post_Saved_NA,
    ScoreIsrReqLockAcquire_Post_Owner_Caller,
    ScoreIsrReqLockAcquire_Post_Wait_UntilReleased }
#else
  { 1, 0, 0, 0, ScoreIsrReqLockAcquire_Post_ISRLevel_NA,
    ScoreIsrReqLockAcquire_Post_Saved_NA, ScoreIsrReqLockAcquire_Post_Owner_NA,
    ScoreIsrReqLockAcquire_Post_Wait_NA }
#endif
};

static const uint8_t
ScoreIsrReqLockAcquire_Map[] = {
  0, 1, 2, 3, 0, 1, 2, 3, 4, 4, 5, 6
};

/* clang-format on */

static size_t ScoreIsrReqLockAcquire_Scope( void *arg, char *buf, size_t n )
{
  ScoreIsrReqLockAcquire_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreIsrReqLockAcquire_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreIsrReqLockAcquire_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreIsrReqLockAcquire_Scope,
  .initial_context = &ScoreIsrReqLockAcquire_Instance
};

static const uint8_t ScoreIsrReqLockAcquire_Weights[] = { 4, 2, 1 };

static void ScoreIsrReqLockAcquire_Skip(
  ScoreIsrReqLockAcquire_Context *ctx,
  size_t                          index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreIsrReqLockAcquire_Pre_ISRLevel_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreIsrReqLockAcquire_Pre_Owner_NA - 1;
      break;
  }
}

static inline ScoreIsrReqLockAcquire_Entry ScoreIsrReqLockAcquire_PopEntry(
  ScoreIsrReqLockAcquire_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreIsrReqLockAcquire_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreIsrReqLockAcquire_Entries[ ScoreIsrReqLockAcquire_Map[ index ] ];
}

static void ScoreIsrReqLockAcquire_TestVariant(
  ScoreIsrReqLockAcquire_Context *ctx
)
{
  ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreIsrReqLockAcquire_Skip( ctx, 0 );
    return;
  }

  ScoreIsrReqLockAcquire_Pre_ISRLevel_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreIsrReqLockAcquire_Pre_Owner_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    ScoreIsrReqLockAcquire_Skip( ctx, 2 );
    return;
  }

  ScoreIsrReqLockAcquire_Action( ctx );
  ScoreIsrReqLockAcquire_Post_ISRLevel_Check(
    ctx,
    ctx->Map.entry.Post_ISRLevel
  );
  ScoreIsrReqLockAcquire_Post_Saved_Check( ctx, ctx->Map.entry.Post_Saved );
  ScoreIsrReqLockAcquire_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  ScoreIsrReqLockAcquire_Post_Wait_Check( ctx, ctx->Map.entry.Post_Wait );
}

static T_fixture_node ScoreIsrReqLockAcquire_Node;

static T_remark ScoreIsrReqLockAcquire_Remark = {
  .next = NULL,
  .remark = "ScoreIsrReqLockAcquire"
};

void ScoreIsrReqLockAcquire_Run( ISRLockContext *lock_ctx )
{
  ScoreIsrReqLockAcquire_Context *ctx;

  ctx = &ScoreIsrReqLockAcquire_Instance;
  ctx->lock_ctx = lock_ctx;

  ctx = T_push_fixture(
    &ScoreIsrReqLockAcquire_Node,
    &ScoreIsrReqLockAcquire_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel_ToContext;
    ctx->Map.pcs[ 0 ] < ScoreIsrReqLockAcquire_Pre_ISRRestoreLevel_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreIsrReqLockAcquire_Pre_ISRLevel_Enabled;
      ctx->Map.pcs[ 1 ] < ScoreIsrReqLockAcquire_Pre_ISRLevel_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreIsrReqLockAcquire_Pre_Owner_Free;
        ctx->Map.pcs[ 2 ] < ScoreIsrReqLockAcquire_Pre_Owner_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreIsrReqLockAcquire_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        ScoreIsrReqLockAcquire_Prepare( ctx );
        ScoreIsrReqLockAcquire_TestVariant( ctx );
        ScoreIsrReqLockAcquire_Cleanup( ctx );
      }
    }
  }

  T_add_remark( &ScoreIsrReqLockAcquire_Remark );
  T_pop_fixture();
}

/** @} */
