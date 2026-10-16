/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrValIntr
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

#include <rtems.h>
#include <bsp/irq-generic.h>

#include "tr-intr-entry-install.h"
#include "tr-intr-get-priority.h"
#include "tr-intr-set-priority.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrValIntr spec:/rtems/intr/val/intr
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @brief Tests some @ref RTEMSAPIClassicIntr directives.
 *
 * This test case performs the following actions:
 *
 * - Validate rtems_interrupt_local_disable() and
 *   rtems_interrupt_local_enable().
 *
 *   - Check that maskable interrupts are enabled before the call to
 *     rtems_interrupt_local_disable() and disabled afterwards.
 *
 *   - Check that maskable interrupts are disabled before the call to
 *     rtems_interrupt_local_disable() and disabled afterwards.
 *
 *   - Check that the maskable interrupt status is restored by the call to
 *     rtems_interrupt_local_enable() according to the `_isr_cookie` parameter.
 *     In this case maskable interrupts are still disabled afterwards.
 *
 *   - Check that the maskable interrupt status is restored by the call to
 *     rtems_interrupt_local_enable() according to the `_isr_cookie` parameter.
 *     In this case maskable interrupts are enabled afterwards.
 *
 * - Validate rtems_interrupt_lock_interrupt_disable().
 *
 *   - Check that maskable interrupts are enabled before the call to
 *     rtems_interrupt_lock_interrupt_disable() and disabled afterwards.
 *
 *   - Restore the status of maskable interrupts with
 *     rtems_interrupt_lock_interrupt_enable(). Check that maskable interrupts
 *     are enabled afterwards.
 *
 * - Validate the interrupt entry initialization.
 *
 *   - Check that the entry is properly initialized by
 *     RTEMS_INTERRUPT_ENTRY_INITIALIZER().
 *
 *   - Call rtems_interrupt_entry_initialize(). Check that the entry is
 *     properly initialized by rtems_interrupt_entry_initialize().
 *
 * - Check rtems_interrupt_get_priority() for each valid vector.
 *
 * - Check rtems_interrupt_set_priority() for each valid vector.
 *
 * - Check rtems_interrupt_entry_install() for each valid vector which is
 *   maskable and which carries no entry.
 *
 * @{
 */

static void ProbeSpurious( void *arg, rtems_vector_number vector )
{
  bool *spurious;

  spurious = arg;
  *spurious = true;
  (void) rtems_interrupt_vector_disable( vector );
}

static bool HasActiveSource(
  rtems_vector_number               vector,
  const rtems_interrupt_attributes *attr
)
{
  rtems_status_code sc;
  bool              pending;
  bool              spurious;

  if ( attr->can_clear ) {
    (void) ClearSoftwareInterrupt( vector );
  }

  pending = false;
  sc = rtems_interrupt_is_pending( vector, &pending );

  if ( sc == RTEMS_SUCCESSFUL && pending ) {
    return true;
  }

  spurious = false;
  SetDefaultInterruptHandler( ProbeSpurious, &spurious );
  (void) rtems_interrupt_vector_enable( vector );
  (void) rtems_interrupt_vector_disable( vector );
  SetDefaultInterruptHandler( NULL, NULL );

  return spurious;
}

static bool IsFreeForEntryInstall(
  rtems_vector_number               vector,
  const rtems_interrupt_attributes *attr
)
{
  if ( !attr->is_maskable ) {
    return false;
  }

  if (
    ( attr->can_enable || attr->maybe_enable ) &&
    !( attr->can_disable || attr->maybe_disable )
  ) {
    return false;
  }

  if ( HasInterruptVectorEntriesInstalled( vector ) ) {
    return false;
  }

  return !HasActiveSource( vector, attr );
}

static void EntryRoutine( void *arg )
{
  (void) arg;
}

static void EntryRoutine2( void *arg )
{
  (void) arg;
}

/**
 * @brief Validate rtems_interrupt_local_disable() and
 *   rtems_interrupt_local_enable().
 */
static void RtemsIntrValIntr_Action_0( void )
{
  rtems_interrupt_level level;
  rtems_interrupt_level level_2;

  /*
   * Check that maskable interrupts are enabled before the call to
   * rtems_interrupt_local_disable() and disabled afterwards.
   */
  T_true( AreInterruptsEnabled() );
  rtems_interrupt_local_disable( level );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that maskable interrupts are disabled before the call to
   * rtems_interrupt_local_disable() and disabled afterwards.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_local_disable( level_2 );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that the maskable interrupt status is restored by the call to
   * rtems_interrupt_local_enable() according to the `_isr_cookie` parameter.
   * In this case maskable interrupts are still disabled afterwards.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_local_enable( level_2 );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that the maskable interrupt status is restored by the call to
   * rtems_interrupt_local_enable() according to the `_isr_cookie` parameter.
   * In this case maskable interrupts are enabled afterwards.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_local_enable( level );
  T_true( AreInterruptsEnabled() );
}

/**
 * @brief Validate rtems_interrupt_lock_interrupt_disable().
 */
static void RtemsIntrValIntr_Action_1( void )
{
  rtems_interrupt_lock_context lock_context;

  /*
   * Check that maskable interrupts are enabled before the call to
   * rtems_interrupt_lock_interrupt_disable() and disabled afterwards.
   */
  T_true( AreInterruptsEnabled() );
  rtems_interrupt_lock_interrupt_disable( &lock_context );
  T_false( AreInterruptsEnabled() );

  /*
   * Restore the status of maskable interrupts with
   * rtems_interrupt_lock_interrupt_enable(). Check that maskable interrupts
   * are enabled afterwards.
   */
  rtems_interrupt_lock_interrupt_enable( &lock_context );
  T_true( AreInterruptsEnabled() );
}

/**
 * @brief Validate the interrupt entry initialization.
 */
static void RtemsIntrValIntr_Action_2( void )
{
  int                   entry_arg;
  int                   entry_arg_2;
  const char            entry_info[] = "1";
  const char            entry_info_2[] = "1";
  rtems_interrupt_entry entry = RTEMS_INTERRUPT_ENTRY_INITIALIZER(
    EntryRoutine,
    &entry_arg,
    entry_info
  );

  /*
   * Check that the entry is properly initialized by
   * RTEMS_INTERRUPT_ENTRY_INITIALIZER().
   */
  T_eq_ptr( entry.handler, EntryRoutine );
  T_eq_ptr( entry.arg, &entry_arg );
  T_eq_ptr( entry.next, NULL );
  T_eq_ptr( entry.info, entry_info );

  /*
   * Call rtems_interrupt_entry_initialize(). Check that the entry is properly
   * initialized by rtems_interrupt_entry_initialize().
   */
  entry.next = &entry;
  rtems_interrupt_entry_initialize(
    &entry,
    EntryRoutine2,
    &entry_arg_2,
    entry_info_2
  );
  T_eq_ptr( entry.handler, EntryRoutine2 );
  T_eq_ptr( entry.arg, &entry_arg_2 );
  T_eq_ptr( entry.next, NULL );
  T_eq_ptr( entry.info, entry_info_2 );
}

/**
 * @brief Check rtems_interrupt_get_priority() for each valid vector.
 */
static void RtemsIntrValIntr_Action_3( void )
{
  rtems_vector_number vector;

  for ( vector = 0; vector < BSP_INTERRUPT_VECTOR_COUNT; ++vector ) {
    rtems_interrupt_attributes attr;
    rtems_status_code          sc;

    memset( &attr, 0, sizeof( attr ) );
    sc = rtems_interrupt_get_attributes( vector, &attr );

    if ( sc == RTEMS_INVALID_ID ) {
      continue;
    }

    T_rsc_success( sc );
    RtemsIntrReqGetPriority_Run( vector, attr.can_get_priority );
  }
}

/**
 * @brief Check rtems_interrupt_set_priority() for each valid vector.
 */
static void RtemsIntrValIntr_Action_4( void )
{
  rtems_vector_number vector;

  for ( vector = 0; vector < BSP_INTERRUPT_VECTOR_COUNT; ++vector ) {
    rtems_interrupt_attributes attr;
    rtems_status_code          sc;

    memset( &attr, 0, sizeof( attr ) );
    sc = rtems_interrupt_get_attributes( vector, &attr );

    if ( sc == RTEMS_INVALID_ID ) {
      continue;
    }

    T_rsc_success( sc );
    RtemsIntrReqSetPriority_Run(
      vector,
      attr.maximum_priority,
      attr.can_set_priority
    );
  }
}

/**
 * @brief Check rtems_interrupt_entry_install() for each valid vector which is
 *   maskable and which carries no entry.
 */
static void RtemsIntrValIntr_Action_5( void )
{
  rtems_vector_number vector;

  for ( vector = 0; vector < BSP_INTERRUPT_VECTOR_COUNT; ++vector ) {
    rtems_interrupt_attributes attr;
    rtems_status_code          sc;
    bool                       enabled;

    memset( &attr, 0, sizeof( attr ) );
    sc = rtems_interrupt_get_attributes( vector, &attr );

    if ( sc == RTEMS_INVALID_ID ) {
      continue;
    }

    T_rsc_success( sc );

    /* The probe of IsFreeForEntryInstall() disables the vector. */
    enabled = false;
    (void) rtems_interrupt_vector_is_enabled( vector, &enabled );

    if ( !IsFreeForEntryInstall( vector, &attr ) ) {
      continue;
    }

    RtemsIntrReqEntryInstall_Run( vector, &attr, enabled );
  }
}

/**
 * @fn void T_case_body_RtemsIntrValIntr( void )
 */
T_TEST_CASE( RtemsIntrValIntr )
{
  RtemsIntrValIntr_Action_0();
  RtemsIntrValIntr_Action_1();
  RtemsIntrValIntr_Action_2();
  RtemsIntrValIntr_Action_3();
  RtemsIntrValIntr_Action_4();
  RtemsIntrValIntr_Action_5();
}

/** @} */
