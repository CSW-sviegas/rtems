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

#ifndef _TR_ISR_LOCK_FIFO_H
#define _TR_ISR_LOCK_FIFO_H

#include "tx-isr-lock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreIsrReqLockFifo
 *
 * @{
 */

typedef enum {
  ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_ToContext,
  ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_ToObject,
  ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_None,
  ScoreIsrReqLockFifo_Pre_ISRRestoreLevel_NA
} ScoreIsrReqLockFifo_Pre_ISRRestoreLevel;

typedef enum {
  ScoreIsrReqLockFifo_Pre_Waiters_More,
  ScoreIsrReqLockFifo_Pre_Waiters_NA
} ScoreIsrReqLockFifo_Pre_Waiters;

typedef enum {
  ScoreIsrReqLockFifo_Post_Owner_FirstWaiter,
  ScoreIsrReqLockFifo_Post_Owner_NA
} ScoreIsrReqLockFifo_Post_Owner;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] lock_ctx is the lock context.
 */
void ScoreIsrReqLockFifo_Run( ISRLockContext *lock_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_ISR_LOCK_FIFO_H */
