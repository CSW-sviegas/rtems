/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqEntryInstall
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#ifndef _TR_INTR_ENTRY_INSTALL_H
#define _TR_INTR_ENTRY_INSTALL_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RtemsIntrReqEntryInstall
 *
 * @{
 */

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Vector_Valid,
  RtemsIntrReqEntryInstall_Pre_Vector_Invalid,
  RtemsIntrReqEntryInstall_Pre_Vector_NA
} RtemsIntrReqEntryInstall_Pre_Vector;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_InstallOptions_Unique,
  RtemsIntrReqEntryInstall_Pre_InstallOptions_Shared,
  RtemsIntrReqEntryInstall_Pre_InstallOptions_Replace,
  RtemsIntrReqEntryInstall_Pre_InstallOptions_NA
} RtemsIntrReqEntryInstall_Pre_InstallOptions;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_EnableOption_Enable,
  RtemsIntrReqEntryInstall_Pre_EnableOption_NoEnable,
  RtemsIntrReqEntryInstall_Pre_EnableOption_NA
} RtemsIntrReqEntryInstall_Pre_EnableOption;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Entry_Obj,
  RtemsIntrReqEntryInstall_Pre_Entry_Null,
  RtemsIntrReqEntryInstall_Pre_Entry_NA
} RtemsIntrReqEntryInstall_Pre_Entry;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Routine_Valid,
  RtemsIntrReqEntryInstall_Pre_Routine_Null,
  RtemsIntrReqEntryInstall_Pre_Routine_NA
} RtemsIntrReqEntryInstall_Pre_Routine;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Init_Yes,
  RtemsIntrReqEntryInstall_Pre_Init_No,
  RtemsIntrReqEntryInstall_Pre_Init_NA
} RtemsIntrReqEntryInstall_Pre_Init;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_ISR_Yes,
  RtemsIntrReqEntryInstall_Pre_ISR_No,
  RtemsIntrReqEntryInstall_Pre_ISR_NA
} RtemsIntrReqEntryInstall_Pre_ISR;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_CanEnable_Yes,
  RtemsIntrReqEntryInstall_Pre_CanEnable_No,
  RtemsIntrReqEntryInstall_Pre_CanEnable_NA
} RtemsIntrReqEntryInstall_Pre_CanEnable;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_CanRaise_Yes,
  RtemsIntrReqEntryInstall_Pre_CanRaise_No,
  RtemsIntrReqEntryInstall_Pre_CanRaise_NA
} RtemsIntrReqEntryInstall_Pre_CanRaise;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Installed_None,
  RtemsIntrReqEntryInstall_Pre_Installed_Unique,
  RtemsIntrReqEntryInstall_Pre_Installed_Other,
  RtemsIntrReqEntryInstall_Pre_Installed_EqRoutine,
  RtemsIntrReqEntryInstall_Pre_Installed_EqArg,
  RtemsIntrReqEntryInstall_Pre_Installed_Match,
  RtemsIntrReqEntryInstall_Pre_Installed_NA
} RtemsIntrReqEntryInstall_Pre_Installed;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_IsEnabled_Yes,
  RtemsIntrReqEntryInstall_Pre_IsEnabled_No,
  RtemsIntrReqEntryInstall_Pre_IsEnabled_NA
} RtemsIntrReqEntryInstall_Pre_IsEnabled;

typedef enum {
  RtemsIntrReqEntryInstall_Post_Status_Ok,
  RtemsIntrReqEntryInstall_Post_Status_InvAddr,
  RtemsIntrReqEntryInstall_Post_Status_IncStat,
  RtemsIntrReqEntryInstall_Post_Status_InvId,
  RtemsIntrReqEntryInstall_Post_Status_InvNum,
  RtemsIntrReqEntryInstall_Post_Status_CalledFromISR,
  RtemsIntrReqEntryInstall_Post_Status_InUse,
  RtemsIntrReqEntryInstall_Post_Status_TooMany,
  RtemsIntrReqEntryInstall_Post_Status_NA
} RtemsIntrReqEntryInstall_Post_Status;

typedef enum {
  RtemsIntrReqEntryInstall_Post_Enabled_Nop,
  RtemsIntrReqEntryInstall_Post_Enabled_Yes,
  RtemsIntrReqEntryInstall_Post_Enabled_NA
} RtemsIntrReqEntryInstall_Post_Enabled;

typedef enum {
  RtemsIntrReqEntryInstall_Post_Installed_No,
  RtemsIntrReqEntryInstall_Post_Installed_Last,
  RtemsIntrReqEntryInstall_Post_Installed_NA
} RtemsIntrReqEntryInstall_Post_Installed;

typedef enum {
  RtemsIntrReqEntryInstall_Post_Dispatch_InOrder,
  RtemsIntrReqEntryInstall_Post_Dispatch_No,
  RtemsIntrReqEntryInstall_Post_Dispatch_Device,
  RtemsIntrReqEntryInstall_Post_Dispatch_NA
} RtemsIntrReqEntryInstall_Post_Dispatch;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in] test_vector is the interrupt vector number.
 *
 * @param[in] attributes is the attribute set of the interrupt vector.
 *
 * @param[in] test_vector_was_enabled is true, if the interrupt vector was
 *   enabled before the test run, otherwise false.
 */
void RtemsIntrReqEntryInstall_Run(
  rtems_vector_number               test_vector,
  const rtems_interrupt_attributes *attributes,
  bool                              test_vector_was_enabled
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_INTR_ENTRY_INSTALL_H */
