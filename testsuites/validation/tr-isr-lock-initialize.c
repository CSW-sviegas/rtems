/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreIsrReqLockInitialize
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

#include "tr-isr-lock-initialize.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreIsrReqLockInitialize spec:/score/isr/req/lock-initialize
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Method_NA : 1;
  uint8_t Pre_ISRLevel_NA : 1;
  uint8_t Pre_State_NA : 1;
  uint8_t Post_ISRLevel : 1;
  uint8_t Post_Lock : 1;
} ScoreIsrReqLockInitialize_Entry;

/**
 * @brief Test context for spec:/score/isr/req/lock-initialize test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then maskable interrupts shall be enabled
   *   before the initialization.
   */
  bool isr_enabled;

  /**
   * @brief This member is true, if maskable interrupts were enabled before the
   *   initialization.
   */
  bool isr_enabled_before;

  /**
   * @brief This member is true, if maskable interrupts were enabled after the
   *   initialization.
   */
  bool isr_enabled_after;

  /**
   * @brief This member is true, if the lock was free after the initialization.
   */
  bool free_after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreIsrReqLockInitialize_Run() parameter.
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
    ScoreIsrReqLockInitialize_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreIsrReqLockInitialize_Context;

static ScoreIsrReqLockInitialize_Context ScoreIsrReqLockInitialize_Instance;

static const char *const ScoreIsrReqLockInitialize_PreDesc_Method[] =
  { "Initializer", "Function", "NA" };

static const char *const ScoreIsrReqLockInitialize_PreDesc_ISRLevel[] =
  { "Enabled", "Disabled", "NA" };

static const char *const ScoreIsrReqLockInitialize_PreDesc_State[] =
  { "Arbitrary", "Destroyed", "NA" };

static const char *const *const ScoreIsrReqLockInitialize_PreDesc[] = {
  ScoreIsrReqLockInitialize_PreDesc_Method,
  ScoreIsrReqLockInitialize_PreDesc_ISRLevel,
  ScoreIsrReqLockInitialize_PreDesc_State,
  NULL
};

typedef ScoreIsrReqLockInitialize_Context Context;

static void FillArbitrary( Context *ctx )
{
  uint8_t *bytes;
  size_t   i;

  bytes = ctx->lock_ctx->lock;

  for ( i = 0; i < ctx->lock_ctx->lock_size; ++i ) {
    bytes[ i ] = (uint8_t) ( 0x11 * ( ( i % 15 ) + 1 ) );
  }
}

static void MakeDestroyed( Context *ctx )
{
  ISR_Level        level;
  ISR_lock_Context lock_context;

  ( *ctx->lock_ctx->initialize )( ctx->lock_ctx );
  _ISR_Local_disable( level );
  ( *ctx->lock_ctx->acquire )( ctx->lock_ctx, &lock_context );
  ( *ctx->lock_ctx->release )( ctx->lock_ctx, &lock_context );
  _ISR_Local_enable( level );
  ( *ctx->lock_ctx->destroy )( ctx->lock_ctx );
}

static void ScoreIsrReqLockInitialize_Pre_Method_Prepare(
  ScoreIsrReqLockInitialize_Context   *ctx,
  ScoreIsrReqLockInitialize_Pre_Method state
)
{
  switch ( state ) {
    case ScoreIsrReqLockInitialize_Pre_Method_Initializer: {
      /*
       * Where the lock is initialized by an initializer for static
       * initialization.
       */
      if ( ctx->lock_ctx->method != ISR_LOCK_METHOD_INITIALIZER ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockInitialize_Pre_Method_Function: {
      /*
       * Where the lock is initialized by a function.
       */
      if ( ctx->lock_ctx->method != ISR_LOCK_METHOD_FUNCTION ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreIsrReqLockInitialize_Pre_Method_NA:
      break;
  }
}

static void ScoreIsrReqLockInitialize_Pre_ISRLevel_Prepare(
  ScoreIsrReqLockInitialize_Context     *ctx,
  ScoreIsrReqLockInitialize_Pre_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockInitialize_Pre_ISRLevel_Enabled: {
      /*
       * While maskable interrupts are enabled on the calling processor.
       */
      ctx->isr_enabled = true;
      break;
    }

    case ScoreIsrReqLockInitialize_Pre_ISRLevel_Disabled: {
      /*
       * While maskable interrupts are disabled on the calling processor.
       */
      ctx->isr_enabled = false;
      break;
    }

    case ScoreIsrReqLockInitialize_Pre_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockInitialize_Pre_State_Prepare(
  ScoreIsrReqLockInitialize_Context  *ctx,
  ScoreIsrReqLockInitialize_Pre_State state
)
{
  switch ( state ) {
    case ScoreIsrReqLockInitialize_Pre_State_Arbitrary: {
      /*
       * While the lock object has an arbitrary content.
       */
      FillArbitrary( ctx );
      break;
    }

    case ScoreIsrReqLockInitialize_Pre_State_Destroyed: {
      /*
       * While the lock is destroyed.
       */
      MakeDestroyed( ctx );
      break;
    }

    case ScoreIsrReqLockInitialize_Pre_State_NA:
      break;
  }
}

static void ScoreIsrReqLockInitialize_Post_ISRLevel_Check(
  ScoreIsrReqLockInitialize_Context      *ctx,
  ScoreIsrReqLockInitialize_Post_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockInitialize_Post_ISRLevel_Nop: {
      /*
       * The status of maskable interrupts on the calling processor shall not
       * change.
       */
      T_eq( ctx->isr_enabled_after, ctx->isr_enabled_before );
      break;
    }

    case ScoreIsrReqLockInitialize_Post_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockInitialize_Post_Lock_Check(
  ScoreIsrReqLockInitialize_Context  *ctx,
  ScoreIsrReqLockInitialize_Post_Lock state
)
{
  switch ( state ) {
    case ScoreIsrReqLockInitialize_Post_Lock_Free: {
      /*
       * The lock shall be free.
       */
      T_true( ctx->free_after );
      break;
    }

    case ScoreIsrReqLockInitialize_Post_Lock_NA:
      break;
  }
}

static void ScoreIsrReqLockInitialize_Action(
  ScoreIsrReqLockInitialize_Context *ctx
)
{
  ISR_Level level;

  level = 0;

  if ( !ctx->isr_enabled ) {
    _ISR_Local_disable( level );
  }

  ctx->isr_enabled_before = AreInterruptsEnabled();
  ( *ctx->lock_ctx->initialize )( ctx->lock_ctx );
  ctx->isr_enabled_after = AreInterruptsEnabled();
  ctx->free_after = ISRLockIsFree( ctx->lock_ctx );

  if ( !ctx->isr_enabled ) {
    _ISR_Local_enable( level );
  }
}

static void ScoreIsrReqLockInitialize_Cleanup(
  ScoreIsrReqLockInitialize_Context *ctx
)
{
  ( *ctx->lock_ctx->destroy )( ctx->lock_ctx );
}

/* clang-format off */

static const ScoreIsrReqLockInitialize_Entry
ScoreIsrReqLockInitialize_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreIsrReqLockInitialize_Post_ISRLevel_Nop,
    ScoreIsrReqLockInitialize_Post_Lock_Free }
#else
  { 0, 0, 0, 0, ScoreIsrReqLockInitialize_Post_ISRLevel_Nop,
    ScoreIsrReqLockInitialize_Post_Lock_NA }
#endif
};

static const uint8_t
ScoreIsrReqLockInitialize_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t ScoreIsrReqLockInitialize_Scope( void *arg, char *buf, size_t n )
{
  ScoreIsrReqLockInitialize_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreIsrReqLockInitialize_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreIsrReqLockInitialize_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreIsrReqLockInitialize_Scope,
  .initial_context = &ScoreIsrReqLockInitialize_Instance
};

static const uint8_t ScoreIsrReqLockInitialize_Weights[] = { 4, 2, 1 };

static void ScoreIsrReqLockInitialize_Skip(
  ScoreIsrReqLockInitialize_Context *ctx,
  size_t                             index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreIsrReqLockInitialize_Pre_ISRLevel_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreIsrReqLockInitialize_Pre_State_NA - 1;
      break;
  }
}

static inline ScoreIsrReqLockInitialize_Entry
ScoreIsrReqLockInitialize_PopEntry( ScoreIsrReqLockInitialize_Context *ctx )
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreIsrReqLockInitialize_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreIsrReqLockInitialize_Entries
    [ ScoreIsrReqLockInitialize_Map[ index ] ];
}

static void ScoreIsrReqLockInitialize_TestVariant(
  ScoreIsrReqLockInitialize_Context *ctx
)
{
  ScoreIsrReqLockInitialize_Pre_Method_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreIsrReqLockInitialize_Skip( ctx, 0 );
    return;
  }

  ScoreIsrReqLockInitialize_Pre_ISRLevel_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreIsrReqLockInitialize_Pre_State_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreIsrReqLockInitialize_Action( ctx );
  ScoreIsrReqLockInitialize_Post_ISRLevel_Check(
    ctx,
    ctx->Map.entry.Post_ISRLevel
  );
  ScoreIsrReqLockInitialize_Post_Lock_Check( ctx, ctx->Map.entry.Post_Lock );
}

static T_fixture_node ScoreIsrReqLockInitialize_Node;

static T_remark ScoreIsrReqLockInitialize_Remark = {
  .next = NULL,
  .remark = "ScoreIsrReqLockInitialize"
};

void ScoreIsrReqLockInitialize_Run( ISRLockContext *lock_ctx )
{
  ScoreIsrReqLockInitialize_Context *ctx;

  ctx = &ScoreIsrReqLockInitialize_Instance;
  ctx->lock_ctx = lock_ctx;

  ctx = T_push_fixture(
    &ScoreIsrReqLockInitialize_Node,
    &ScoreIsrReqLockInitialize_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreIsrReqLockInitialize_Pre_Method_Initializer;
    ctx->Map.pcs[ 0 ] < ScoreIsrReqLockInitialize_Pre_Method_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreIsrReqLockInitialize_Pre_ISRLevel_Enabled;
      ctx->Map.pcs[ 1 ] < ScoreIsrReqLockInitialize_Pre_ISRLevel_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreIsrReqLockInitialize_Pre_State_Arbitrary;
        ctx->Map.pcs[ 2 ] < ScoreIsrReqLockInitialize_Pre_State_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreIsrReqLockInitialize_PopEntry( ctx );
        ScoreIsrReqLockInitialize_TestVariant( ctx );
        ScoreIsrReqLockInitialize_Cleanup( ctx );
      }
    }
  }

  T_add_remark( &ScoreIsrReqLockInitialize_Remark );
  T_pop_fixture();
}

/** @} */
