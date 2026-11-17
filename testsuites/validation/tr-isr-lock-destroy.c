/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreIsrReqLockDestroy
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

#include <string.h>

#include "tr-isr-lock-destroy.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreIsrReqLockDestroy spec:/score/isr/req/lock-destroy
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_ISRLevel_NA : 1;
  uint8_t Post_ISRLevel : 1;
  uint8_t Post_Lock : 1;
} ScoreIsrReqLockDestroy_Entry;

/**
 * @brief Test context for spec:/score/isr/req/lock-destroy test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then maskable interrupts shall be enabled
   *   before the destroy.
   */
  bool isr_enabled;

  /**
   * @brief This member is true, if maskable interrupts were enabled before the
   *   destroy.
   */
  bool isr_enabled_before;

  /**
   * @brief This member is true, if maskable interrupts were enabled after the
   *   destroy.
   */
  bool isr_enabled_after;

  /**
   * @brief This member contains a copy of the lock object before the destroy.
   */
  uint8_t lock_before[ 256 ];

  /**
   * @brief This member contains the result of the comparison of the lock
   *   object after the destroy with the copy before the destroy.
   */
  int lock_compare;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreIsrReqLockDestroy_Run() parameter.
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
    ScoreIsrReqLockDestroy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreIsrReqLockDestroy_Context;

static ScoreIsrReqLockDestroy_Context ScoreIsrReqLockDestroy_Instance;

static const char *const ScoreIsrReqLockDestroy_PreDesc_ISRLevel[] =
  { "Enabled", "Disabled", "NA" };

static const char *const *const ScoreIsrReqLockDestroy_PreDesc[] = {
  ScoreIsrReqLockDestroy_PreDesc_ISRLevel,
  NULL
};

static void ScoreIsrReqLockDestroy_Pre_ISRLevel_Prepare(
  ScoreIsrReqLockDestroy_Context     *ctx,
  ScoreIsrReqLockDestroy_Pre_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockDestroy_Pre_ISRLevel_Enabled: {
      /*
       * While maskable interrupts are enabled on the calling processor.
       */
      ctx->isr_enabled = true;
      break;
    }

    case ScoreIsrReqLockDestroy_Pre_ISRLevel_Disabled: {
      /*
       * While maskable interrupts are disabled on the calling processor.
       */
      ctx->isr_enabled = false;
      break;
    }

    case ScoreIsrReqLockDestroy_Pre_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockDestroy_Post_ISRLevel_Check(
  ScoreIsrReqLockDestroy_Context      *ctx,
  ScoreIsrReqLockDestroy_Post_ISRLevel state
)
{
  switch ( state ) {
    case ScoreIsrReqLockDestroy_Post_ISRLevel_Nop: {
      /*
       * The status of maskable interrupts on the calling processor shall not
       * change.
       */
      T_eq( ctx->isr_enabled_after, ctx->isr_enabled_before );
      break;
    }

    case ScoreIsrReqLockDestroy_Post_ISRLevel_NA:
      break;
  }
}

static void ScoreIsrReqLockDestroy_Post_Lock_Check(
  ScoreIsrReqLockDestroy_Context  *ctx,
  ScoreIsrReqLockDestroy_Post_Lock state
)
{
  switch ( state ) {
    case ScoreIsrReqLockDestroy_Post_Lock_Nop: {
      /*
       * The lock object shall not change.
       */
      T_eq_int( ctx->lock_compare, 0 );
      break;
    }

    case ScoreIsrReqLockDestroy_Post_Lock_NA:
      break;
  }
}

static void ScoreIsrReqLockDestroy_Prepare(
  ScoreIsrReqLockDestroy_Context *ctx
)
{
  ( *ctx->lock_ctx->initialize )( ctx->lock_ctx );
}

static void ScoreIsrReqLockDestroy_Action(
  ScoreIsrReqLockDestroy_Context *ctx
)
{
  ISR_Level level;

  level = 0;

  if ( !ctx->isr_enabled ) {
    _ISR_Local_disable( level );
  }

  T_quiet_le_sz( ctx->lock_ctx->lock_size, sizeof( ctx->lock_before ) );
  memcpy( ctx->lock_before, ctx->lock_ctx->lock, ctx->lock_ctx->lock_size );
  ctx->isr_enabled_before = AreInterruptsEnabled();
  ( *ctx->lock_ctx->destroy )( ctx->lock_ctx );
  ctx->isr_enabled_after = AreInterruptsEnabled();
  ctx->lock_compare = memcmp(
    ctx->lock_before,
    ctx->lock_ctx->lock,
    ctx->lock_ctx->lock_size
  );

  if ( !ctx->isr_enabled ) {
    _ISR_Local_enable( level );
  }
}

/* clang-format off */

static const ScoreIsrReqLockDestroy_Entry
ScoreIsrReqLockDestroy_Entries[] = {
#if defined(RTEMS_SMP) && defined(RTEMS_PROFILING)
  { 0, 0, ScoreIsrReqLockDestroy_Post_ISRLevel_Nop,
    ScoreIsrReqLockDestroy_Post_Lock_NA }
#else
  { 0, 0, ScoreIsrReqLockDestroy_Post_ISRLevel_Nop,
    ScoreIsrReqLockDestroy_Post_Lock_Nop }
#endif
};

static const uint8_t
ScoreIsrReqLockDestroy_Map[] = {
  0, 0
};

/* clang-format on */

static size_t ScoreIsrReqLockDestroy_Scope( void *arg, char *buf, size_t n )
{
  ScoreIsrReqLockDestroy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreIsrReqLockDestroy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreIsrReqLockDestroy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreIsrReqLockDestroy_Scope,
  .initial_context = &ScoreIsrReqLockDestroy_Instance
};

static inline ScoreIsrReqLockDestroy_Entry ScoreIsrReqLockDestroy_PopEntry(
  ScoreIsrReqLockDestroy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreIsrReqLockDestroy_Entries[ ScoreIsrReqLockDestroy_Map[ index ] ];
}

static void ScoreIsrReqLockDestroy_TestVariant(
  ScoreIsrReqLockDestroy_Context *ctx
)
{
  ScoreIsrReqLockDestroy_Pre_ISRLevel_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreIsrReqLockDestroy_Action( ctx );
  ScoreIsrReqLockDestroy_Post_ISRLevel_Check(
    ctx,
    ctx->Map.entry.Post_ISRLevel
  );
  ScoreIsrReqLockDestroy_Post_Lock_Check( ctx, ctx->Map.entry.Post_Lock );
}

static T_fixture_node ScoreIsrReqLockDestroy_Node;

static T_remark ScoreIsrReqLockDestroy_Remark = {
  .next = NULL,
  .remark = "ScoreIsrReqLockDestroy"
};

void ScoreIsrReqLockDestroy_Run( ISRLockContext *lock_ctx )
{
  ScoreIsrReqLockDestroy_Context *ctx;

  ctx = &ScoreIsrReqLockDestroy_Instance;
  ctx->lock_ctx = lock_ctx;

  ctx = T_push_fixture(
    &ScoreIsrReqLockDestroy_Node,
    &ScoreIsrReqLockDestroy_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreIsrReqLockDestroy_Pre_ISRLevel_Enabled;
    ctx->Map.pcs[ 0 ] < ScoreIsrReqLockDestroy_Pre_ISRLevel_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreIsrReqLockDestroy_PopEntry( ctx );
    ScoreIsrReqLockDestroy_Prepare( ctx );
    ScoreIsrReqLockDestroy_TestVariant( ctx );
  }

  T_add_remark( &ScoreIsrReqLockDestroy_Remark );
  T_pop_fixture();
}

/** @} */
