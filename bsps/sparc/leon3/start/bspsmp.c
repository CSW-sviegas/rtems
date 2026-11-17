/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This source file contains the SMP support of the LEON3 BSP.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/fatal.h>
#include <bsp/irq-generic.h>
#include <bsp/leon3.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <rtems/score/assert.h>
#include <rtems/score/smpimpl.h>
#include <stdlib.h>

#if !defined( __leon__ ) || defined( RTEMS_PARAVIRT )
uint32_t _CPU_SMP_Get_current_processor( void )
{
  return _LEON3_Get_current_processor();
}
#endif

static void bsp_inter_processor_interrupt( void *arg )
{
  (void) arg;
  _SMP_Inter_processor_interrupt_handler( _Per_CPU_Get() );
}

void bsp_start_on_secondary_processor( Per_CPU_Control *cpu_self )
{
  if ( !leon3_data_cache_snooping_enabled() ) {
    bsp_fatal( LEON3_FATAL_INVALID_CACHE_CONFIG_SECONDARY_PROCESSOR );
  }

  _SMP_Start_multitasking_on_secondary_processor( cpu_self );
}

static rtems_interrupt_entry
  leon3_inter_processor_interrupt_entry = RTEMS_INTERRUPT_ENTRY_INITIALIZER(
    bsp_inter_processor_interrupt,
    NULL,
    "IPI"
  );

static void leon3_install_inter_processor_interrupt( void )
{
  rtems_status_code   sc;
  rtems_vector_number irq;

  irq = LEON3_mp_irq;

  bsp_interrupt_set_affinity( irq, _SMP_Get_online_processors() );

  sc = rtems_interrupt_entry_install(
    irq,
    RTEMS_INTERRUPT_SHARED,
    &leon3_inter_processor_interrupt_entry
  );
  _Assert_Unused_variable_equals( sc, RTEMS_SUCCESSFUL );
}

uint32_t _CPU_SMP_Initialize( void )
{
  if ( !leon3_data_cache_snooping_enabled() ) {
    bsp_fatal( LEON3_FATAL_INVALID_CACHE_CONFIG_BOOT_PROCESSOR );
  }

  return leon3_get_cpu_count( LEON3_IrqCtrl_Regs );
}

bool _CPU_SMP_Start_processor( uint32_t cpu_index )
{
  #if defined( RTEMS_DEBUG )
  printk( "Waking CPU %d\n", cpu_index );
  #endif

  grlib_store_32(
    &LEON3_IrqCtrl_Regs->mpstat,
    IRQAMP_MPSTAT_STATUS( 1U << cpu_index )
  );

  return true;
}

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count )
{
  (void) cpu_count;

#if !defined( RTEMS_DRVMGR_STARTUP )
  leon3_install_inter_processor_interrupt();
#endif
}

void _CPU_SMP_Prepare_start_multitasking( void )
{
  /* Do nothing */
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
  /* send interrupt to destination CPU */
  grlib_store_32(
    &LEON3_IrqCtrl_Regs->piforce[ target_processor_index ],
#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
    1U << leon3_irqmap_get_unchecked( LEON3_mp_irq )
#else
    1U << LEON3_mp_irq
#endif
  );
}

#if defined( RTEMS_DRVMGR_STARTUP )
RTEMS_SYSINIT_ITEM(
  leon3_install_inter_processor_interrupt,
  RTEMS_SYSINIT_DRVMGR_LEVEL_1,
  RTEMS_SYSINIT_ORDER_LAST_BUT_4
);
#endif
