/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreIsrReqLockIsrEnable
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

#include "tr-isr-lock-isr-enable.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreIsrReqLockIsrEnable spec:/score/isr/req/lock-isr-enable
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_SavedISRLevel_NA : 1;
  uint8_t Post_ISRLevel : 2;
} ScoreIsrReqLockIsrEnable_Entry;

/**
 * @brief Test context for spec:/score/isr/req/lock-isr-enable test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the lock context shall contain the
   *   status of enabled maskable interrupts before the call.
   */
  bool saved_enabled;

  /**
   * @brief This member is true, if maskable interrupts were enabled after the
   *   call.
   */
  bool isr_enabled_after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreIsrReqLockIsrEnable_Run() parameter.
   */
  ISRLockContext *lock_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    ScoreIsrReqLockIsrEnable_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreIsrReqLockIsrEnable_Context;

static ScoreIsrReqLockIsrEnable_Context ScoreIsrReqLockIsrEnable_Instance;

static const char *const ScoreIsrReqLockIsrEnable_PreDesc_SavedISRLevel[] =
  { "Enabled", "Disabled", "NA" };

static const char *const *const ScoreIsrReqLockIsrEnable_PreDesc[] = {
  ScoreIsrReqLockIsrEnable_PreDesc_SavedISRLevel,
  NULL
};

static void ScoreIsrReqLockIsrEnable_Pre_SavedISRLevel_Prepare(
  ScoreIsrReqLockIsrEnable_Context          *ctx,
  ScoreIsrReqLockIsrEnable_Pre_SavedISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockIsrEnable_Pre_SavedISRLevel_Enabled: {
      /*
       * While the lock context contains the status of enabled maskable
       * interrupts.
       */
      ctx->saved_enabled = true;
      break;
    }

    case ScoreIsrReqLockIsrEnable_Pre_SavedISRLevel_Disabled: {
      /*
       * While the lock context contains the status of disabled maskable
       * interrupts.
       */
      ctx->saved_enabled = false;
      break;
    }

    case ScoreIsrReqLockIsrEnable_Pre_SavedISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockIsrEnable_Post_ISRLevel_Check(
  ScoreIsrReqLockIsrEnable_Context      *ctx,
  ScoreIsrReqLockIsrEnable_Post_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockIsrEnable_Post_ISRLevel_Enabled: {
      /*
       * Maskable interrupts shall be enabled on the calling processor.
       */
      T_true( ctx->isr_enabled_after );
      break;
    }

    case ScoreIsrReqLockIsrEnable_Post_ISRLevel_Disabled: {
      /*
       * Maskable interrupts shall be disabled on the calling processor.
       */
      T_false( ctx->isr_enabled_after );
      break;
    }

    case ScoreIsrReqLockIsrEnable_Post_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockIsrEnable_Action(
  ScoreIsrReqLockIsrEnable_Context *ctx
)
{
  ISR_lock_Context lock_context;
  ISR_Level        level;

  _ISR_Local_disable( level );

  if ( ctx->saved_enabled ) {
    _ISR_Local_enable( level );
  }

  ( *ctx->lock_ctx->interrupt_disable )( ctx->lock_ctx, &lock_context );
  T_quiet_false( AreInterruptsEnabled() );
  ( *ctx->lock_ctx->interrupt_enable )( ctx->lock_ctx, &lock_context );
  ctx->isr_enabled_after = AreInterruptsEnabled();
  _ISR_Local_enable( level );
}

/* clang-format off */

static const ScoreIsrReqLockIsrEnable_Entry
ScoreIsrReqLockIsrEnable_Entries[] = {
  { 0, 0, ScoreIsrReqLockIsrEnable_Post_ISRLevel_Enabled },
  { 0, 0, ScoreIsrReqLockIsrEnable_Post_ISRLevel_Disabled }
};

static const uint8_t
ScoreIsrReqLockIsrEnable_Map[] = {
  0, 1
};

/* clang-format on */

static size_t ScoreIsrReqLockIsrEnable_Scope( void *arg, char *buf, size_t n )
{
  ScoreIsrReqLockIsrEnable_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreIsrReqLockIsrEnable_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreIsrReqLockIsrEnable_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreIsrReqLockIsrEnable_Scope,
  .initial_context = &ScoreIsrReqLockIsrEnable_Instance
};

static inline ScoreIsrReqLockIsrEnable_Entry ScoreIsrReqLockIsrEnable_PopEntry(
  ScoreIsrReqLockIsrEnable_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreIsrReqLockIsrEnable_Entries
    [ ScoreIsrReqLockIsrEnable_Map[ index ] ];
}

static void ScoreIsrReqLockIsrEnable_TestVariant(
  ScoreIsrReqLockIsrEnable_Context *ctx
)
{
  ScoreIsrReqLockIsrEnable_Pre_SavedISRLevel_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreIsrReqLockIsrEnable_Action( ctx );
  ScoreIsrReqLockIsrEnable_Post_ISRLevel_Check(
    ctx,
    ctx->Map.entry.Post_ISRLevel
  );
}

static T_fixture_node ScoreIsrReqLockIsrEnable_Node;

static T_remark ScoreIsrReqLockIsrEnable_Remark = {
  .next = NULL,
  .remark = "ScoreIsrReqLockIsrEnable"
};

void ScoreIsrReqLockIsrEnable_Run( ISRLockContext *lock_ctx )
{
  ScoreIsrReqLockIsrEnable_Context *ctx;

  ctx = &ScoreIsrReqLockIsrEnable_Instance;
  ctx->lock_ctx = lock_ctx;

  ctx = T_push_fixture(
    &ScoreIsrReqLockIsrEnable_Node,
    &ScoreIsrReqLockIsrEnable_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreIsrReqLockIsrEnable_Pre_SavedISRLevel_Enabled;
    ctx->Map.pcs[ 0 ] < ScoreIsrReqLockIsrEnable_Pre_SavedISRLevel_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreIsrReqLockIsrEnable_PopEntry( ctx );
    ScoreIsrReqLockIsrEnable_TestVariant( ctx );
  }

  T_add_remark( &ScoreIsrReqLockIsrEnable_Remark );
  T_pop_fixture();
}

/** @} */
