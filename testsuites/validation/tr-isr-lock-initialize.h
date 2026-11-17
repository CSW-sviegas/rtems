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

#ifndef _TR_ISR_LOCK_INITIALIZE_H
#define _TR_ISR_LOCK_INITIALIZE_H

#include "tx-isr-lock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreIsrReqLockInitialize
 *
 * @{
 */

typedef enum {
  ScoreIsrReqLockInitialize_Pre_Method_Initializer,
  ScoreIsrReqLockInitialize_Pre_Method_Function,
  ScoreIsrReqLockInitialize_Pre_Method_NA
} ScoreIsrReqLockInitialize_Pre_Method;

typedef enum {
  ScoreIsrReqLockInitialize_Pre_ISRLevel_Enabled,
  ScoreIsrReqLockInitialize_Pre_ISRLevel_Disabled,
  ScoreIsrReqLockInitialize_Pre_ISRLevel_NA
} ScoreIsrReqLockInitialize_Pre_ISRLevel;

typedef enum {
  ScoreIsrReqLockInitialize_Pre_State_Arbitrary,
  ScoreIsrReqLockInitialize_Pre_State_Destroyed,
  ScoreIsrReqLockInitialize_Pre_State_NA
} ScoreIsrReqLockInitialize_Pre_State;

typedef enum {
  ScoreIsrReqLockInitialize_Post_ISRLevel_Nop,
  ScoreIsrReqLockInitialize_Post_ISRLevel_NA
} ScoreIsrReqLockInitialize_Post_ISRLevel;

typedef enum {
  ScoreIsrReqLockInitialize_Post_Lock_Free,
  ScoreIsrReqLockInitialize_Post_Lock_NA
} ScoreIsrReqLockInitialize_Post_Lock;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] lock_ctx is the lock context.
 */
void ScoreIsrReqLockInitialize_Run( ISRLockContext *lock_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_ISR_LOCK_INITIALIZE_H */
