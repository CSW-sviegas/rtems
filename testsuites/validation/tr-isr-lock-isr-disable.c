/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreIsrReqLockIsrDisable
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

#include "tr-isr-lock-isr-disable.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreIsrReqLockIsrDisable spec:/score/isr/req/lock-isr-disable
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_ISRLevel_NA : 1;
  uint8_t Post_ISRLevel : 1;
  uint8_t Post_Saved : 2;
} ScoreIsrReqLockIsrDisable_Entry;

/**
 * @brief Test context for spec:/score/isr/req/lock-isr-disable test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then maskable interrupts shall be enabled
   *   before the call.
   */
  bool isr_enabled;

  /**
   * @brief This member is true, if maskable interrupts were enabled after the
   *   call.
   */
  bool isr_enabled_after;

  /**
   * @brief This member is true, if the lock context contains the status of
   *   enabled maskable interrupts after the call.
   */
  bool saved_enabled;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreIsrReqLockIsrDisable_Run() parameter.
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
    ScoreIsrReqLockIsrDisable_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreIsrReqLockIsrDisable_Context;

static ScoreIsrReqLockIsrDisable_Context ScoreIsrReqLockIsrDisable_Instance;

static const char *const ScoreIsrReqLockIsrDisable_PreDesc_ISRLevel[] =
  { "Enabled", "Disabled", "NA" };

static const char *const *const ScoreIsrReqLockIsrDisable_PreDesc[] = {
  ScoreIsrReqLockIsrDisable_PreDesc_ISRLevel,
  NULL
};

static void ScoreIsrReqLockIsrDisable_Pre_ISRLevel_Prepare(
  ScoreIsrReqLockIsrDisable_Context     *ctx,
  ScoreIsrReqLockIsrDisable_Pre_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockIsrDisable_Pre_ISRLevel_Enabled: {
      /*
       * While maskable interrupts are enabled on the calling processor.
       */
      ctx->isr_enabled = true;
      break;
    }

    case ScoreIsrReqLockIsrDisable_Pre_ISRLevel_Disabled: {
      /*
       * While maskable interrupts are disabled on the calling processor.
       */
      ctx->isr_enabled = false;
      break;
    }

    case ScoreIsrReqLockIsrDisable_Pre_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockIsrDisable_Post_ISRLevel_Check(
  ScoreIsrReqLockIsrDisable_Context      *ctx,
  ScoreIsrReqLockIsrDisable_Post_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockIsrDisable_Post_ISRLevel_Disabled: {
      /*
       * Maskable interrupts shall be disabled on the calling processor.
       */
      T_false( ctx->isr_enabled_after );
      break;
    }

    case ScoreIsrReqLockIsrDisable_Post_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockIsrDisable_Post_Saved_Check(
  ScoreIsrReqLockIsrDisable_Context   *ctx,
  ScoreIsrReqLockIsrDisable_Post_Saved state
)
{
  switch ( state ) {
    case ScoreIsrReqLockIsrDisable_Post_Saved_Enabled: {
      /*
       * The lock context shall contain the status of enabled maskable
       * interrupts.
       */
      T_true( ctx->saved_enabled );
      break;
    }

    case ScoreIsrReqLockIsrDisable_Post_Saved_Disabled: {
      /*
       * The lock context shall contain the status of disabled maskable
       * interrupts.
       */
      T_false( ctx->saved_enabled );
      break;
    }

    case ScoreIsrReqLockIsrDisable_Post_Saved_NA:
      break;
  }
}

static void ScoreIsrReqLockIsrDisable_Action(
  ScoreIsrReqLockIsrDisable_Context *ctx
)
{
  ISR_lock_Context lock_context;
  ISR_Level        level;

  _ISR_Local_disable( level );

  if ( ctx->isr_enabled ) {
    _ISR_Local_enable( level );
  }

  ( *ctx->lock_ctx->interrupt_disable )( ctx->lock_ctx, &lock_context );
  ctx->isr_enabled_after = AreInterruptsEnabled();
  ctx->saved_enabled = _ISR_Is_enabled(
    ( *ctx->lock_ctx->get_saved_level )( ctx->lock_ctx, &lock_context )
  );
  _ISR_Local_enable( level );
}

/* clang-format off */

static const ScoreIsrReqLockIsrDisable_Entry
ScoreIsrReqLockIsrDisable_Entries[] = {
  { 0, 0, ScoreIsrReqLockIsrDisable_Post_ISRLevel_Disabled,
    ScoreIsrReqLockIsrDisable_Post_Saved_Enabled },
  { 0, 0, ScoreIsrReqLockIsrDisable_Post_ISRLevel_Disabled,
    ScoreIsrReqLockIsrDisable_Post_Saved_Disabled }
};

static const uint8_t
ScoreIsrReqLockIsrDisable_Map[] = {
  0, 1
};

/* clang-format on */

static size_t ScoreIsrReqLockIsrDisable_Scope( void *arg, char *buf, size_t n )
{
  ScoreIsrReqLockIsrDisable_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreIsrReqLockIsrDisable_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreIsrReqLockIsrDisable_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreIsrReqLockIsrDisable_Scope,
  .initial_context = &ScoreIsrReqLockIsrDisable_Instance
};

static inline ScoreIsrReqLockIsrDisable_Entry
ScoreIsrReqLockIsrDisable_PopEntry( ScoreIsrReqLockIsrDisable_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreIsrReqLockIsrDisable_Entries
    [ ScoreIsrReqLockIsrDisable_Map[ index ] ];
}

static void ScoreIsrReqLockIsrDisable_TestVariant(
  ScoreIsrReqLockIsrDisable_Context *ctx
)
{
  ScoreIsrReqLockIsrDisable_Pre_ISRLevel_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreIsrReqLockIsrDisable_Action( ctx );
  ScoreIsrReqLockIsrDisable_Post_ISRLevel_Check(
    ctx,
    ctx->Map.entry.Post_ISRLevel
  );
  ScoreIsrReqLockIsrDisable_Post_Saved_Check( ctx, ctx->Map.entry.Post_Saved );
}

static T_fixture_node ScoreIsrReqLockIsrDisable_Node;

static T_remark ScoreIsrReqLockIsrDisable_Remark = {
  .next = NULL,
  .remark = "ScoreIsrReqLockIsrDisable"
};

void ScoreIsrReqLockIsrDisable_Run( ISRLockContext *lock_ctx )
{
  ScoreIsrReqLockIsrDisable_Context *ctx;

  ctx = &ScoreIsrReqLockIsrDisable_Instance;
  ctx->lock_ctx = lock_ctx;

  ctx = T_push_fixture(
    &ScoreIsrReqLockIsrDisable_Node,
    &ScoreIsrReqLockIsrDisable_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreIsrReqLockIsrDisable_Pre_ISRLevel_Enabled;
    ctx->Map.pcs[ 0 ] < ScoreIsrReqLockIsrDisable_Pre_ISRLevel_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreIsrReqLockIsrDisable_PopEntry( ctx );
    ScoreIsrReqLockIsrDisable_TestVariant( ctx );
  }

  T_add_remark( &ScoreIsrReqLockIsrDisable_Remark );
  T_pop_fixture();
}

/** @} */
