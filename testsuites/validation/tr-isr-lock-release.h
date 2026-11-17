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

#ifndef _TR_ISR_LOCK_RELEASE_H
#define _TR_ISR_LOCK_RELEASE_H

#include "tx-isr-lock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreIsrReqLockRelease
 *
 * @{
 */

typedef enum {
  ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_ToContext,
  ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_ToObject,
  ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_None,
  ScoreIsrReqLockRelease_Pre_ISRRestoreLevel_NA
} ScoreIsrReqLockRelease_Pre_ISRRestoreLevel;

typedef enum {
  ScoreIsrReqLockRelease_Pre_ISRLevel_Enabled,
  ScoreIsrReqLockRelease_Pre_ISRLevel_Disabled,
  ScoreIsrReqLockRelease_Pre_ISRLevel_NA
} ScoreIsrReqLockRelease_Pre_ISRLevel;

typedef enum {
  ScoreIsrReqLockRelease_Pre_Acquire_Matching,
  ScoreIsrReqLockRelease_Pre_Acquire_DisableAcquireIsr,
  ScoreIsrReqLockRelease_Pre_Acquire_NA
} ScoreIsrReqLockRelease_Pre_Acquire;

typedef enum {
  ScoreIsrReqLockRelease_Pre_SavedISRLevel_Enabled,
  ScoreIsrReqLockRelease_Pre_SavedISRLevel_Disabled,
  ScoreIsrReqLockRelease_Pre_SavedISRLevel_NA
} ScoreIsrReqLockRelease_Pre_SavedISRLevel;

typedef enum {
  ScoreIsrReqLockRelease_Pre_Waiters_None,
  ScoreIsrReqLockRelease_Pre_Waiters_Some,
  ScoreIsrReqLockRelease_Pre_Waiters_NA
} ScoreIsrReqLockRelease_Pre_Waiters;

typedef enum {
  ScoreIsrReqLockRelease_Post_ISRLevel_Enabled,
  ScoreIsrReqLockRelease_Post_ISRLevel_Disabled,
  ScoreIsrReqLockRelease_Post_ISRLevel_NA
} ScoreIsrReqLockRelease_Post_ISRLevel;

typedef enum {
  ScoreIsrReqLockRelease_Post_Lock_Free,
  ScoreIsrReqLockRelease_Post_Lock_Waiter,
  ScoreIsrReqLockRelease_Post_Lock_NA
} ScoreIsrReqLockRelease_Post_Lock;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] lock_ctx is the lock context.
 */
void ScoreIsrReqLockRelease_Run( ISRLockContext *lock_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_ISR_LOCK_RELEASE_H */
