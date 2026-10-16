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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp/irq-generic.h>
#include <rtems/irq-extension.h>

#include "tr-intr-entry-install.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqEntryInstall spec:/rtems/intr/req/entry-install
 *
 * @ingroup TestsuitesValidationIntr
 *
 * The test runs for one interrupt vector. The attributes of the interrupt
 * vector decide which states of CanEnable and CanRaise the test run reaches. A
 * read back of the enabled status after an arrangement decides which state of
 * IsEnabled the test run reaches. The test action raises the interrupt vector
 * after a successful call, and the entries count their dispatch.
 *
 * @{
 */

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Vector_NA : 1;
  uint32_t Pre_InstallOptions_NA : 1;
  uint32_t Pre_EnableOption_NA : 1;
  uint32_t Pre_Entry_NA : 1;
  uint32_t Pre_Routine_NA : 1;
  uint32_t Pre_Init_NA : 1;
  uint32_t Pre_ISR_NA : 1;
  uint32_t Pre_CanEnable_NA : 1;
  uint32_t Pre_CanRaise_NA : 1;
  uint32_t Pre_Installed_NA : 1;
  uint32_t Pre_IsEnabled_NA : 1;
  uint32_t Post_Status : 4;
  uint32_t Post_Enabled : 2;
  uint32_t Post_Installed : 2;
  uint32_t Post_Dispatch : 2;
} RtemsIntrReqEntryInstall_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/entry-install test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the service was initialized during
   *   setup.
   */
  bool initialized_during_setup;

  /**
   * @brief This member provides a counter for handler invocations.
   */
  uint32_t handler_counter;

  /**
   * @brief This member provides a counter snapshot for each entry.
   */
  uint32_t counter_by_entry[ 3 ];

  /**
   * @brief If this member is true, then the service shall be initialized.
   */
  bool initialized;

  /**
   * @brief If this member is true, then rtems_interrupt_entry_install() shall
   *   be called from within interrupt context.
   */
  bool isr;

  /**
   * @brief This member contains the enabled status before the
   *   rtems_interrupt_entry_install() call.
   */
  bool enabled_before;

  /**
   * @brief This member contains the enabled status after the
   *   rtems_interrupt_entry_install() call.
   */
  bool enabled_after;

  /**
   * @brief If this member is true, then the test action raised the interrupt
   *   vector.
   */
  bool raised;

  /**
   * @brief If this member is true, then rtems_interrupt_vector_enable()
   *   enables the interrupt vector of the test run.
   */
  bool can_enable;

  /**
   * @brief This member provides the count of interrupts which no entry
   *   handled.
   */
  uint32_t spurious_counter;

  /**
   * @brief This member provides the count of visited entries.
   */
  uint32_t visited_entries;

  /**
   * @brief This member provides another rtems_interrupt_entry object.
   */
  rtems_interrupt_entry other_entry;

  /**
   * @brief If this member is true, then another entry was installed.
   */
  bool other_installed;

  /**
   * @brief This member provides a third rtems_interrupt_entry object.
   */
  rtems_interrupt_entry third_entry;

  /**
   * @brief If this member is true, then the third entry was installed.
   */
  bool third_installed;

  /**
   * @brief This member provides the options used to install the other
   *   rtems_interrupt_entry object.
   */
  rtems_option other_options;

  /**
   * @brief This member provides the rtems_interrupt_entry object.
   */
  rtems_interrupt_entry entry_obj;

  /**
   * @brief This member specifies the `vector` parameter value.
   */
  rtems_vector_number vector;

  /**
   * @brief This member specifies the `options` parameter value.
   */
  rtems_option options;

  /**
   * @brief This member specifies the `entry` parameter value.
   */
  rtems_interrupt_entry *entry;

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_entry_install() call.
   */
  rtems_status_code status;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsIntrReqEntryInstall_Run() parameter.
   */
  rtems_vector_number test_vector;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsIntrReqEntryInstall_Run() parameter.
   */
  const rtems_interrupt_attributes *attributes;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsIntrReqEntryInstall_Run() parameter.
   */
  bool test_vector_was_enabled;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 11 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 11 ];

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
    RtemsIntrReqEntryInstall_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqEntryInstall_Context;

static RtemsIntrReqEntryInstall_Context RtemsIntrReqEntryInstall_Instance;

static const char *const RtemsIntrReqEntryInstall_PreDesc_Vector[] =
  { "Valid", "Invalid", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_InstallOptions[] =
  { "Unique", "Shared", "Replace", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_EnableOption[] =
  { "Enable", "NoEnable", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_Entry[] =
  { "Obj", "Null", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_Routine[] =
  { "Valid", "Null", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_Init[] =
  { "Yes", "No", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_ISR[] =
  { "Yes", "No", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_CanEnable[] =
  { "Yes", "No", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_CanRaise[] =
  { "Yes", "No", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_Installed[] =
  { "None", "Unique", "Other", "EqRoutine", "EqArg", "Match", "NA" };

static const char *const RtemsIntrReqEntryInstall_PreDesc_IsEnabled[] =
  { "Yes", "No", "NA" };

static const char *const *const RtemsIntrReqEntryInstall_PreDesc[] = {
  RtemsIntrReqEntryInstall_PreDesc_Vector,
  RtemsIntrReqEntryInstall_PreDesc_InstallOptions,
  RtemsIntrReqEntryInstall_PreDesc_EnableOption,
  RtemsIntrReqEntryInstall_PreDesc_Entry,
  RtemsIntrReqEntryInstall_PreDesc_Routine,
  RtemsIntrReqEntryInstall_PreDesc_Init,
  RtemsIntrReqEntryInstall_PreDesc_ISR,
  RtemsIntrReqEntryInstall_PreDesc_CanEnable,
  RtemsIntrReqEntryInstall_PreDesc_CanRaise,
  RtemsIntrReqEntryInstall_PreDesc_Installed,
  RtemsIntrReqEntryInstall_PreDesc_IsEnabled,
  NULL
};

typedef RtemsIntrReqEntryInstall_Context Context;

static char entry_arg;

static char other_arg;

static char third_arg;

static const char entry_info[] = "Entry";

static const char other_info[] = "Other";

static const char third_info[] = "Third";

static void Install(
  Context                *ctx,
  rtems_option            options,
  rtems_interrupt_handler routine,
  void                   *arg
)
{
  rtems_status_code sc;

  ctx->other_options = options;
  rtems_interrupt_entry_initialize(
    &ctx->other_entry,
    routine,
    arg,
    other_info
  );

  sc = rtems_interrupt_entry_install(
    ctx->test_vector,
    options,
    &ctx->other_entry
  );
  T_rsc_success( sc );

  ctx->other_installed = true;
}

static void Spurious( void *arg, rtems_vector_number vector )
{
  Context *ctx;

  ctx = arg;
  ++ctx->spurious_counter;
  (void) rtems_interrupt_vector_disable( vector );
}

static void ClearTestVector( const Context *ctx )
{
  if ( ctx->attributes->can_clear ) {
    (void) ClearSoftwareInterrupt( ctx->test_vector );
  }
}

static bool SetEnabled( Context *ctx, bool enable )
{
  rtems_status_code sc;
  bool              enabled;

  if ( enable ) {
    ClearTestVector( ctx );
    (void) rtems_interrupt_vector_enable( ctx->test_vector );
  } else {
    (void) rtems_interrupt_vector_disable( ctx->test_vector );
  }

  enabled = !enable;
  sc = rtems_interrupt_vector_is_enabled( ctx->test_vector, &enabled );
  T_rsc_success( sc );

  return enabled == enable;
}

static void Routine( Context *ctx, uint32_t counter )
{
  rtems_status_code sc;

  ctx->handler_counter = counter;

  if ( !ctx->attributes->cleared_by_acknowledge ) {
    sc = ClearSoftwareInterrupt( ctx->test_vector );
    T_rsc_success( sc );
  }

  /*
   * At most three entries are installed.  A further call is an interrupt
   * which the clear did not stop.
   */
  if ( counter > 3 ) {
    sc = rtems_interrupt_vector_disable( ctx->test_vector );
    T_rsc_success( sc );
  }
}

static void EntryRoutine( void *arg )
{
  Context *ctx;
  uint32_t counter;

  ctx = T_fixture_context();
  counter = ctx->handler_counter + 1;

  if ( arg == &other_arg ) {
    ctx->counter_by_entry[ 1 ] = counter;
  } else {
    ctx->counter_by_entry[ 0 ] = counter;
    T_eq_ptr( arg, &entry_arg );
  }

  Routine( ctx, counter );
}

static void OtherRoutine( void *arg )
{
  Context *ctx;
  uint32_t counter;

  (void) arg;
  ctx = T_fixture_context();
  counter = ctx->handler_counter + 1;
  ctx->counter_by_entry[ 1 ] = counter;
  Routine( ctx, counter );
}

static void ThirdRoutine( void *arg )
{
  Context *ctx;
  uint32_t counter;

  ctx = T_fixture_context();
  counter = ctx->handler_counter + 1;
  ctx->counter_by_entry[ 2 ] = counter;
  T_eq_ptr( arg, &third_arg );
  Routine( ctx, counter );
}

static void InstallThird( Context *ctx )
{
  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    &ctx->third_entry,
    ThirdRoutine,
    &third_arg,
    third_info
  );

  sc = rtems_interrupt_entry_install(
    ctx->test_vector,
    RTEMS_INTERRUPT_SHARED,
    &ctx->third_entry
  );
  T_rsc_success( sc );

  ctx->third_installed = true;
}

static void Action( void *arg )
{
  Context          *ctx;
  rtems_status_code sc;

  ctx = arg;

  sc = rtems_interrupt_vector_is_enabled(
    ctx->test_vector,
    &ctx->enabled_before
  );
  T_rsc_success( sc );

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized
  );

  ctx->status = rtems_interrupt_entry_install(
    ctx->vector,
    ctx->options,
    ctx->entry
  );

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized_during_setup
  );

  sc = rtems_interrupt_vector_is_enabled(
    ctx->test_vector,
    &ctx->enabled_after
  );
  T_rsc_success( sc );

  if ( ctx->status == RTEMS_SUCCESSFUL && ctx->attributes->can_raise ) {
    sc = RaiseSoftwareInterrupt( ctx->test_vector );
    T_rsc_success( sc );
    ctx->raised = true;
  }
}

static void VisitInstalled(
  void                   *arg,
  const char             *info,
  rtems_option            options,
  rtems_interrupt_handler handler_routine,
  void                   *handler_arg
)
{
  Context *ctx;
  uint32_t visited_entries;

  ctx = arg;
  visited_entries = ctx->visited_entries;

  if ( visited_entries == 0 && ctx->other_installed ) {
    T_eq_ptr( info, other_info );
    T_eq_u32( options, ctx->other_options & RTEMS_INTERRUPT_UNIQUE );
    T_eq_ptr( handler_routine, ctx->other_entry.handler );
    T_eq_ptr( handler_arg, ctx->other_entry.arg );
  } else if ( visited_entries == 1 && ctx->third_installed ) {
    T_eq_ptr( info, third_info );
    T_eq_u32( options, RTEMS_INTERRUPT_SHARED );
    T_eq_ptr( handler_routine, ThirdRoutine );
    T_eq_ptr( handler_arg, &third_arg );
  } else {
    T_eq_ptr( info, entry_info );
    T_eq_u32( options, ctx->options & RTEMS_INTERRUPT_UNIQUE );
    T_eq_ptr( handler_routine, ctx->entry_obj.handler );
    T_eq_ptr( handler_arg, ctx->entry_obj.arg );
  }

  ctx->visited_entries = visited_entries + 1;
}

static void RtemsIntrReqEntryInstall_Pre_Vector_Prepare(
  RtemsIntrReqEntryInstall_Context   *ctx,
  RtemsIntrReqEntryInstall_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Vector_Valid: {
      /*
       * While the `vector` parameter is associated with an interrupt vector.
       */
      ctx->vector = ctx->test_vector;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Vector_Invalid: {
      /*
       * While the `vector` parameter is not associated with an interrupt
       * vector.
       */
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_InstallOptions_Prepare(
  RtemsIntrReqEntryInstall_Context           *ctx,
  RtemsIntrReqEntryInstall_Pre_InstallOptions state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_InstallOptions_Unique: {
      /*
       * While the `options` parameter has the RTEMS_INTERRUPT_UNIQUE option
       * set.
       */
      ctx->options = RTEMS_INTERRUPT_UNIQUE;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_InstallOptions_Shared: {
      /*
       * While the `options` parameter has the RTEMS_INTERRUPT_UNIQUE option
       * and the RTEMS_INTERRUPT_REPLACE option cleared.
       */
      ctx->options = RTEMS_INTERRUPT_SHARED;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_InstallOptions_Replace: {
      /*
       * While the `options` parameter has the RTEMS_INTERRUPT_REPLACE option
       * set.
       */
      ctx->options = RTEMS_INTERRUPT_REPLACE;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_InstallOptions_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_EnableOption_Prepare(
  RtemsIntrReqEntryInstall_Context         *ctx,
  RtemsIntrReqEntryInstall_Pre_EnableOption state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_EnableOption_Enable: {
      /*
       * While the `options` parameter has the RTEMS_INTERRUPT_NO_ENABLE option
       * cleared.
       */
      ctx->options |= RTEMS_INTERRUPT_ENABLE;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_EnableOption_NoEnable: {
      /*
       * While the `options` parameter has the RTEMS_INTERRUPT_NO_ENABLE option
       * set.
       */
      ctx->options |= RTEMS_INTERRUPT_NO_ENABLE;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_EnableOption_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Entry_Prepare(
  RtemsIntrReqEntryInstall_Context  *ctx,
  RtemsIntrReqEntryInstall_Pre_Entry state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Entry_Obj: {
      /*
       * While the `entry` parameter references an object of type
       * rtems_interrupt_entry.
       */
      ctx->entry = &ctx->entry_obj;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Entry_Null: {
      /*
       * While the `entry` parameter is equal to NULL.
       */
      ctx->entry = NULL;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Entry_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Routine_Prepare(
  RtemsIntrReqEntryInstall_Context    *ctx,
  RtemsIntrReqEntryInstall_Pre_Routine state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Routine_Valid: {
      /*
       * While the handler routine of the object referenced by the `entry`
       * parameter is valid.
       */
      rtems_interrupt_entry_initialize(
        &ctx->entry_obj,
        EntryRoutine,
        &entry_arg,
        entry_info
      );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Routine_Null: {
      /*
       * While the handler routine of the object referenced by the `entry`
       * parameter is equal to NULL.
       */
      rtems_interrupt_entry_initialize(
        &ctx->entry_obj,
        NULL,
        &entry_arg,
        entry_info
      );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Routine_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Init_Prepare(
  RtemsIntrReqEntryInstall_Context *ctx,
  RtemsIntrReqEntryInstall_Pre_Init state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Init_Yes: {
      /*
       * While the service is initialized.
       */
      ctx->initialized = true;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Init_No: {
      /*
       * While the service is not initialized.
       */
      ctx->initialized = false;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Init_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_ISR_Prepare(
  RtemsIntrReqEntryInstall_Context *ctx,
  RtemsIntrReqEntryInstall_Pre_ISR  state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_ISR_Yes: {
      /*
       * While rtems_interrupt_entry_install() is called from within interrupt
       * context.
       */
      ctx->isr = true;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_ISR_No: {
      /*
       * While rtems_interrupt_entry_install() is not called from within
       * interrupt context.
       */
      ctx->isr = false;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_ISR_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_CanEnable_Prepare(
  RtemsIntrReqEntryInstall_Context      *ctx,
  RtemsIntrReqEntryInstall_Pre_CanEnable state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_CanEnable_Yes: {
      /*
       * While rtems_interrupt_vector_enable() enables the interrupt vector
       * associated with the `vector` parameter.
       */
      if ( !ctx->can_enable ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_CanEnable_No: {
      /*
       * While rtems_interrupt_vector_enable() does not enable the interrupt
       * vector associated with the `vector` parameter.
       */
      if ( ctx->can_enable ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_CanEnable_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_CanRaise_Prepare(
  RtemsIntrReqEntryInstall_Context     *ctx,
  RtemsIntrReqEntryInstall_Pre_CanRaise state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_CanRaise_Yes: {
      /*
       * While the can_raise member of the rtems_interrupt_attributes object of
       * the interrupt vector associated with the `vector` parameter is true.
       */
      if ( !ctx->attributes->can_raise ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_CanRaise_No: {
      /*
       * While the can_raise member of the rtems_interrupt_attributes object of
       * the interrupt vector associated with the `vector` parameter is false.
       */
      if ( ctx->attributes->can_raise ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_CanRaise_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Installed_Prepare(
  RtemsIntrReqEntryInstall_Context      *ctx,
  RtemsIntrReqEntryInstall_Pre_Installed state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Installed_None: {
      /*
       * While no entry is installed at the interrupt vector specified by the
       * `vector` parameter.
       */
      /* Nothing to do */
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_Unique: {
      /*
       * While a unique entry is installed at the interrupt vector specified by
       * the `vector` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_UNIQUE, EntryRoutine, &entry_arg );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_Other: {
      /*
       * While at least one non-unique entry is installed at the interrupt
       * vector specified by the `vector` parameter, while all entries
       * installed at the interrupt vector specified by the `vector` parameter
       * have a handler routine which is not equal to the handler routine of
       * the object referenced by the `entry` parameter, while all entries
       * installed at the interrupt vector specified by the `vector` parameter
       * have a handler argument which is not equal to the handler argument of
       * the object referenced by the `entry` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_SHARED, OtherRoutine, &other_arg );
      InstallThird( ctx );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_EqRoutine: {
      /*
       * While at least one non-unique entry is installed at the interrupt
       * vector specified by the `vector` parameter, while at least one entry
       * installed at the interrupt vector specified by the `vector` parameter
       * has a handler routine which is equal to the handler routine of the
       * object referenced by the `entry` parameter, while all entries
       * installed at the interrupt vector specified by the `vector` parameter
       * have a handler argument which is not equal to the handler argument of
       * the object referenced by the `entry` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_SHARED, EntryRoutine, &other_arg );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_EqArg: {
      /*
       * While at least one non-unique entry is installed at the interrupt
       * vector specified by the `vector` parameter, while all entries
       * installed at the interrupt vector specified by the `vector` parameter
       * have a handler routine which is not equal to the handler routine of
       * the object referenced by the `entry` parameter, while at least one
       * entry installed at the interrupt vector specified by the `vector`
       * parameter has a handler argument which is equal to the handler
       * argument of the object referenced by the `entry` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_SHARED, OtherRoutine, &other_arg );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_Match: {
      /*
       * While at least one non-unique entry with a handler routine which is
       * equal to the handler routine of the object referenced by the `entry`
       * parameter and with a handler argument which is equal to the handler
       * argument of the object referenced by the `entry` parameter is
       * installed at the interrupt vector specified by the `vector` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_SHARED, EntryRoutine, &entry_arg );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_IsEnabled_Prepare(
  RtemsIntrReqEntryInstall_Context      *ctx,
  RtemsIntrReqEntryInstall_Pre_IsEnabled state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_IsEnabled_Yes: {
      /*
       * While the interrupt vector associated with the `vector` parameter is
       * enabled.
       */
      if ( !SetEnabled( ctx, true ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_IsEnabled_No: {
      /*
       * While the interrupt vector associated with the `vector` parameter is
       * disabled.
       */
      if ( !SetEnabled( ctx, false ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_IsEnabled_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Post_Status_Check(
  RtemsIntrReqEntryInstall_Context    *ctx,
  RtemsIntrReqEntryInstall_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_IncStat: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_InvNum: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_CalledFromISR: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_CALLED_FROM_ISR.
       */
      T_rsc( ctx->status, RTEMS_CALLED_FROM_ISR );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_InUse: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_RESOURCE_IN_USE.
       */
      T_rsc( ctx->status, RTEMS_RESOURCE_IN_USE );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_TooMany: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Post_Enabled_Check(
  RtemsIntrReqEntryInstall_Context     *ctx,
  RtemsIntrReqEntryInstall_Post_Enabled state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Post_Enabled_Nop: {
      /*
       * The enabled status of the interrupt vector specified by `vector` shall
       * not be modified by the rtems_interrupt_entry_install() call.
       */
      T_eq( ctx->enabled_after, ctx->enabled_before );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Enabled_Yes: {
      /*
       * The interrupt vector specified by `vector` shall be enabled.
       */
      T_true( ctx->enabled_after );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Enabled_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Post_Installed_Check(
  RtemsIntrReqEntryInstall_Context       *ctx,
  RtemsIntrReqEntryInstall_Post_Installed state
)
{
  rtems_status_code sc;

  ctx->visited_entries = 0;
  sc = rtems_interrupt_handler_iterate(
    ctx->test_vector,
    VisitInstalled,
    ctx
  );
  T_rsc_success( sc );

  switch ( state ) {
    case RtemsIntrReqEntryInstall_Post_Installed_No: {
      /*
       * The entry referenced by `entry` shall not be installed at the
       * interrupt vector specified by `vector`.
       */
      if ( ctx->other_installed && ctx->third_installed ) {
        T_eq_u32( ctx->visited_entries, 2 );
      } else if ( ctx->other_installed ) {
        T_eq_u32( ctx->visited_entries, 1 );
      } else {
        T_eq_u32( ctx->visited_entries, 0 );
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Installed_Last: {
      /*
       * The entry referenced by `entry` shall be installed as the last entry
       * at the interrupt vector specified by `vector`.
       */
      if ( ctx->other_installed && ctx->third_installed ) {
        T_eq_u32( ctx->visited_entries, 3 );
      } else if ( ctx->other_installed ) {
        T_eq_u32( ctx->visited_entries, 2 );
      } else {
        T_eq_u32( ctx->visited_entries, 1 );
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Installed_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Post_Dispatch_Check(
  RtemsIntrReqEntryInstall_Context      *ctx,
  RtemsIntrReqEntryInstall_Post_Dispatch state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Post_Dispatch_InOrder: {
      /*
       * The interrupt service shall dispatch each entry installed at the
       * interrupt vector specified by `vector` once in the order of
       * installation.
       */
      uint32_t counter;

      counter = 1;

      if ( ctx->other_installed ) {
        T_eq_u32( ctx->counter_by_entry[ 1 ], counter );
        ++counter;
      }

      if ( ctx->third_installed ) {
        T_eq_u32( ctx->counter_by_entry[ 2 ], counter );
        ++counter;
      }

      T_eq_u32( ctx->counter_by_entry[ 0 ], counter );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Dispatch_No: {
      /*
       * The interrupt service shall dispatch no entry installed at the
       * interrupt vector specified by `vector`.
       */
      T_eq_u32( ctx->counter_by_entry[ 0 ], 0 );
      T_eq_u32( ctx->counter_by_entry[ 1 ], 0 );
      T_eq_u32( ctx->counter_by_entry[ 2 ], 0 );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Dispatch_Device: {
      /*
       * The interrupt service shall dispatch the entries installed at the
       * interrupt vector specified by `vector` as the device which drives the
       * interrupt vector requests it.
       */
      /* The test cannot raise the interrupt vector. */
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Dispatch_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Setup(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  ctx->initialized_during_setup = bsp_interrupt_is_initialized();
  ctx->spurious_counter = 0;
  SetDefaultInterruptHandler( Spurious, ctx );
  ctx->can_enable = ctx->attributes->can_enable;

  if ( !ctx->can_enable && ctx->attributes->maybe_enable ) {
    ctx->can_enable = SetEnabled( ctx, true );
    (void) rtems_interrupt_vector_disable( ctx->test_vector );
  }
}

static void RtemsIntrReqEntryInstall_Setup_Wrap( void *arg )
{
  RtemsIntrReqEntryInstall_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqEntryInstall_Setup( ctx );
}

static void RtemsIntrReqEntryInstall_Teardown(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  if ( ctx->test_vector_was_enabled ) {
    (void) rtems_interrupt_vector_enable( ctx->test_vector );
  } else {
    (void) rtems_interrupt_vector_disable( ctx->test_vector );
  }

  SetDefaultInterruptHandler( NULL, NULL );
  T_eq_u32( ctx->spurious_counter, 0 );
}

static void RtemsIntrReqEntryInstall_Teardown_Wrap( void *arg )
{
  RtemsIntrReqEntryInstall_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqEntryInstall_Teardown( ctx );
}

static void RtemsIntrReqEntryInstall_Prepare(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->counter_by_entry ); ++i ) {
    ctx->counter_by_entry[ i ] = 0;
  }

  ctx->handler_counter = 0;
  ctx->other_installed = false;
  ctx->third_installed = false;
  ctx->raised = false;

  /* The cleanup also runs for a variant which skips before the action. */
  ctx->status = RTEMS_NOT_DEFINED;
  ClearTestVector( ctx );
}

static void RtemsIntrReqEntryInstall_Action(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  if ( ctx->isr ) {
    CallWithinISR( Action, ctx );
  } else {
    Action( ctx );
  }
}

static void RtemsIntrReqEntryInstall_Cleanup(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  rtems_status_code sc;

  if ( ctx->raised ) {
    if ( ctx->attributes->can_clear ) {
      (void) ClearSoftwareInterrupt( ctx->test_vector );
    } else {
      /*
       * The interrupt controller decides whether the enable dispatches the
       * raised interrupt.  A dispatch through the installed entries
       * acknowledges it.
       */
      (void) rtems_interrupt_vector_enable( ctx->test_vector );
    }
  }

  if ( ctx->third_installed ) {
    sc = rtems_interrupt_entry_remove( ctx->test_vector, &ctx->third_entry );
    T_rsc_success( sc );
  }

  if ( ctx->other_installed ) {
    sc = rtems_interrupt_entry_remove( ctx->test_vector, &ctx->other_entry );
    T_rsc_success( sc );
  }

  if ( ctx->status == RTEMS_SUCCESSFUL ) {
    sc = rtems_interrupt_entry_remove( ctx->test_vector, ctx->entry );
    T_rsc_success( sc );
  }
}

/* clang-format off */

static const RtemsIntrReqEntryInstall_Entry
RtemsIntrReqEntryInstall_Entries[] = {
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_InvAddr,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_NA,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1,
    RtemsIntrReqEntryInstall_Post_Status_InvAddr,
    RtemsIntrReqEntryInstall_Post_Enabled_NA,
    RtemsIntrReqEntryInstall_Post_Installed_NA,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_IncStat,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    RtemsIntrReqEntryInstall_Post_Status_IncStat,
    RtemsIntrReqEntryInstall_Post_Enabled_NA,
    RtemsIntrReqEntryInstall_Post_Installed_NA,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_InvAddr,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    RtemsIntrReqEntryInstall_Post_Status_InvId,
    RtemsIntrReqEntryInstall_Post_Enabled_NA,
    RtemsIntrReqEntryInstall_Post_Installed_NA,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    RtemsIntrReqEntryInstall_Post_Status_InvAddr,
    RtemsIntrReqEntryInstall_Post_Enabled_NA,
    RtemsIntrReqEntryInstall_Post_Installed_NA,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_CalledFromISR,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_InUse,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_InvNum,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_Ok,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_Last,
    RtemsIntrReqEntryInstall_Post_Dispatch_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_Ok,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_Last,
    RtemsIntrReqEntryInstall_Post_Dispatch_InOrder },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_Ok,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_Last,
    RtemsIntrReqEntryInstall_Post_Dispatch_Device },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_TooMany,
    RtemsIntrReqEntryInstall_Post_Enabled_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No,
    RtemsIntrReqEntryInstall_Post_Dispatch_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_Ok,
    RtemsIntrReqEntryInstall_Post_Enabled_Yes,
    RtemsIntrReqEntryInstall_Post_Installed_Last,
    RtemsIntrReqEntryInstall_Post_Dispatch_InOrder },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_Ok,
    RtemsIntrReqEntryInstall_Post_Enabled_Yes,
    RtemsIntrReqEntryInstall_Post_Installed_Last,
    RtemsIntrReqEntryInstall_Post_Dispatch_Device }
};

static const uint8_t
RtemsIntrReqEntryInstall_Map[] = {
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 11, 14, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 15, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 11, 10, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 10, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 11, 10, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 12, 10, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 11, 10, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 12, 10, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 11, 14, 8, 8, 11, 10, 11, 10, 11, 10, 13, 13, 12, 15, 8, 8,
  12, 10, 12, 10, 12, 10, 13, 13, 11, 10, 8, 8, 11, 10, 11, 10, 11, 10, 13, 13,
  12, 10, 8, 8, 12, 10, 12, 10, 12, 10, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 11, 10, 8, 8, 11, 10, 11, 10, 11, 10, 13, 13, 12, 10,
  8, 8, 12, 10, 12, 10, 12, 10, 13, 13, 11, 10, 8, 8, 11, 10, 11, 10, 11, 10,
  13, 13, 12, 10, 8, 8, 12, 10, 12, 10, 12, 10, 13, 13, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t RtemsIntrReqEntryInstall_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqEntryInstall_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqEntryInstall_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqEntryInstall_Fixture = {
  .setup = RtemsIntrReqEntryInstall_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsIntrReqEntryInstall_Teardown_Wrap,
  .scope = RtemsIntrReqEntryInstall_Scope,
  .initial_context = &RtemsIntrReqEntryInstall_Instance
};

static const uint16_t RtemsIntrReqEntryInstall_Weights[] =
  { 4608, 1536, 768, 384, 192, 96, 48, 24, 12, 2, 1 };

static void RtemsIntrReqEntryInstall_Skip(
  RtemsIntrReqEntryInstall_Context *ctx,
  size_t                            index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = RtemsIntrReqEntryInstall_Pre_InstallOptions_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = RtemsIntrReqEntryInstall_Pre_EnableOption_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = RtemsIntrReqEntryInstall_Pre_Entry_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pci[ 4 ] = RtemsIntrReqEntryInstall_Pre_Routine_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pci[ 5 ] = RtemsIntrReqEntryInstall_Pre_Init_NA - 1;
      /* Fall through */
    case 6:
      ctx->Map.pci[ 6 ] = RtemsIntrReqEntryInstall_Pre_ISR_NA - 1;
      /* Fall through */
    case 7:
      ctx->Map.pci[ 7 ] = RtemsIntrReqEntryInstall_Pre_CanEnable_NA - 1;
      /* Fall through */
    case 8:
      ctx->Map.pci[ 8 ] = RtemsIntrReqEntryInstall_Pre_CanRaise_NA - 1;
      /* Fall through */
    case 9:
      ctx->Map.pci[ 9 ] = RtemsIntrReqEntryInstall_Pre_Installed_NA - 1;
      /* Fall through */
    case 10:
      ctx->Map.pci[ 10 ] = RtemsIntrReqEntryInstall_Pre_IsEnabled_NA - 1;
      break;
  }
}

static inline RtemsIntrReqEntryInstall_Entry RtemsIntrReqEntryInstall_PopEntry(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 11; ++i ) {
      index += RtemsIntrReqEntryInstall_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return RtemsIntrReqEntryInstall_Entries
    [ RtemsIntrReqEntryInstall_Map[ index ] ];
}

static void RtemsIntrReqEntryInstall_SetPreConditionStates(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];

  if ( ctx->Map.entry.Pre_Routine_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsIntrReqEntryInstall_Pre_Routine_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];

  if ( ctx->Map.entry.Pre_CanEnable_NA ) {
    ctx->Map.pcs[ 7 ] = RtemsIntrReqEntryInstall_Pre_CanEnable_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_CanRaise_NA ) {
    ctx->Map.pcs[ 8 ] = RtemsIntrReqEntryInstall_Pre_CanRaise_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }

  if ( ctx->Map.entry.Pre_Installed_NA ) {
    ctx->Map.pcs[ 9 ] = RtemsIntrReqEntryInstall_Pre_Installed_NA;
  } else {
    ctx->Map.pcs[ 9 ] = ctx->Map.pci[ 9 ];
  }

  if ( ctx->Map.entry.Pre_IsEnabled_NA ) {
    ctx->Map.pcs[ 10 ] = RtemsIntrReqEntryInstall_Pre_IsEnabled_NA;
  } else {
    ctx->Map.pcs[ 10 ] = ctx->Map.pci[ 10 ];
  }
}

static void RtemsIntrReqEntryInstall_TestVariant(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  RtemsIntrReqEntryInstall_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqEntryInstall_Pre_InstallOptions_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  RtemsIntrReqEntryInstall_Pre_EnableOption_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqEntryInstall_Pre_Entry_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsIntrReqEntryInstall_Pre_Routine_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsIntrReqEntryInstall_Pre_Init_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsIntrReqEntryInstall_Pre_ISR_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsIntrReqEntryInstall_Pre_CanEnable_Prepare( ctx, ctx->Map.pcs[ 7 ] );

  if ( ctx->Map.skip ) {
    RtemsIntrReqEntryInstall_Skip( ctx, 7 );
    return;
  }

  RtemsIntrReqEntryInstall_Pre_CanRaise_Prepare( ctx, ctx->Map.pcs[ 8 ] );

  if ( ctx->Map.skip ) {
    RtemsIntrReqEntryInstall_Skip( ctx, 8 );
    return;
  }

  RtemsIntrReqEntryInstall_Pre_Installed_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  RtemsIntrReqEntryInstall_Pre_IsEnabled_Prepare( ctx, ctx->Map.pcs[ 10 ] );

  if ( ctx->Map.skip ) {
    RtemsIntrReqEntryInstall_Skip( ctx, 10 );
    return;
  }

  RtemsIntrReqEntryInstall_Action( ctx );
  RtemsIntrReqEntryInstall_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsIntrReqEntryInstall_Post_Enabled_Check(
    ctx,
    ctx->Map.entry.Post_Enabled
  );
  RtemsIntrReqEntryInstall_Post_Installed_Check(
    ctx,
    ctx->Map.entry.Post_Installed
  );
  RtemsIntrReqEntryInstall_Post_Dispatch_Check(
    ctx,
    ctx->Map.entry.Post_Dispatch
  );
}

static T_fixture_node RtemsIntrReqEntryInstall_Node;

static T_remark RtemsIntrReqEntryInstall_Remark = {
  .next = NULL,
  .remark = "RtemsIntrReqEntryInstall"
};

void RtemsIntrReqEntryInstall_Run(
  rtems_vector_number               test_vector,
  const rtems_interrupt_attributes *attributes,
  bool                              test_vector_was_enabled
)
{
  RtemsIntrReqEntryInstall_Context *ctx;

  ctx = &RtemsIntrReqEntryInstall_Instance;
  ctx->test_vector = test_vector;
  ctx->attributes = attributes;
  ctx->test_vector_was_enabled = test_vector_was_enabled;

  ctx = T_push_fixture(
    &RtemsIntrReqEntryInstall_Node,
    &RtemsIntrReqEntryInstall_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqEntryInstall_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqEntryInstall_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqEntryInstall_Pre_InstallOptions_Unique;
      ctx->Map.pci[ 1 ] < RtemsIntrReqEntryInstall_Pre_InstallOptions_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqEntryInstall_Pre_EnableOption_Enable;
        ctx->Map.pci[ 2 ] < RtemsIntrReqEntryInstall_Pre_EnableOption_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsIntrReqEntryInstall_Pre_Entry_Obj;
          ctx->Map.pci[ 3 ] < RtemsIntrReqEntryInstall_Pre_Entry_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsIntrReqEntryInstall_Pre_Routine_Valid;
            ctx->Map.pci[ 4 ] < RtemsIntrReqEntryInstall_Pre_Routine_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsIntrReqEntryInstall_Pre_Init_Yes;
              ctx->Map.pci[ 5 ] < RtemsIntrReqEntryInstall_Pre_Init_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = RtemsIntrReqEntryInstall_Pre_ISR_Yes;
                ctx->Map.pci[ 6 ] < RtemsIntrReqEntryInstall_Pre_ISR_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] =
                    RtemsIntrReqEntryInstall_Pre_CanEnable_Yes;
                  ctx->Map.pci[ 7 ] <
                  RtemsIntrReqEntryInstall_Pre_CanEnable_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] =
                      RtemsIntrReqEntryInstall_Pre_CanRaise_Yes;
                    ctx->Map.pci[ 8 ] <
                    RtemsIntrReqEntryInstall_Pre_CanRaise_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    for (
                      ctx->Map.pci[ 9 ] =
                        RtemsIntrReqEntryInstall_Pre_Installed_None;
                      ctx->Map.pci[ 9 ] <
                      RtemsIntrReqEntryInstall_Pre_Installed_NA;
                      ++ctx->Map.pci[ 9 ]
                    ) {
                      for (
                        ctx->Map.pci[ 10 ] =
                          RtemsIntrReqEntryInstall_Pre_IsEnabled_Yes;
                        ctx->Map.pci[ 10 ] <
                        RtemsIntrReqEntryInstall_Pre_IsEnabled_NA;
                        ++ctx->Map.pci[ 10 ]
                      ) {
                        ctx->Map.entry = RtemsIntrReqEntryInstall_PopEntry(
                          ctx
                        );
                        RtemsIntrReqEntryInstall_SetPreConditionStates( ctx );
                        RtemsIntrReqEntryInstall_Prepare( ctx );
                        RtemsIntrReqEntryInstall_TestVariant( ctx );
                        RtemsIntrReqEntryInstall_Cleanup( ctx );
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  T_add_remark( &RtemsIntrReqEntryInstall_Remark );
  T_pop_fixture();
}

/** @} */
