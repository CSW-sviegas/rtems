/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (C) 2005, 2026 Frontgrade Gaisler AB
 * Copyright (C) 2012, 2026 embedded brains GmbH & Co. KG
 * Copyright (C) 1995, 2015 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBBSP_SPARC_LEON3_BSP_H
#define LIBBSP_SPARC_LEON3_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/irq-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSBSPsSPARCLEON3 LEON3 and LEON4
 *
 * @ingroup RTEMSBSPsSPARC
 *
 * @brief LEON3 and LEON4 Board Support Package.
 *
 * @{
 */

/* SPARC CPU variant: LEON3 */
#define LEON3 1

#define BSP_FEATURE_IRQ_EXTENSION

/*
 *  BSP provides its own Idle thread body
 */
void *bsp_idle_thread( uintptr_t ignored );
#define BSP_IDLE_TASK_BODY bsp_idle_thread

/* Maximum supported APBUARTs by BSP */
#define BSP_NUMBER_OF_TERMIOS_PORTS 8

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_leon_open_eth_driver_attach(
  struct rtems_bsdnet_ifconfig *config,
  int                           attach
);
extern int rtems_smc91111_driver_attach_leon3(
  struct rtems_bsdnet_ifconfig *config,
  int                           attach
);
extern int rtems_leon_greth_driver_attach(
  struct rtems_bsdnet_ifconfig *config,
  int                           attach
);

#define RTEMS_BSP_NETWORK_DRIVER_NAME_OPENETH "open_eth1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH_OPENETH \
  rtems_leon_open_eth_driver_attach
#define RTEMS_BSP_NETWORK_DRIVER_NAME_SMC91111 "smc_eth1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH_SMC91111 \
  rtems_smc91111_driver_attach_leon3
#define RTEMS_BSP_NETWORK_DRIVER_NAME_GRETH   "gr_eth1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH_GRETH rtems_leon_greth_driver_attach

#ifndef RTEMS_BSP_NETWORK_DRIVER_NAME
#define RTEMS_BSP_NETWORK_DRIVER_NAME   RTEMS_BSP_NETWORK_DRIVER_NAME_GRETH
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH RTEMS_BSP_NETWORK_DRIVER_ATTACH_GRETH
#endif

#define HAS_SMC91111

/* Configure GRETH driver */
#define GRETH_SUPPORTED
#define GRETH_MEM_LOAD( addr ) leon_r32_no_cache( (uintptr_t) addr )

extern int CPU_SPARC_HAS_SNOOPING;

/* Constants */

/*
 *  Information placed in the linkcmds file.
 */

extern int RAM_START;
extern int RAM_END;
extern int RAM_SIZE;

extern int PROM_START;
extern int PROM_END;
extern int PROM_SIZE;

extern int CLOCK_SPEED;

extern int end; /* last address in the program */

void BSP_fatal_exit( uint32_t error );

/*
 *  Delay for the specified number of microseconds.
 */
void rtems_bsp_delay( int usecs );

/* Interrupt Service Routine (ISR) pointer */
typedef void ( *bsp_shared_isr )( void *arg );

/* Registers a shared IRQ handler, and enable it at IRQ controller. Multiple
 * interrupt handlers may use the same IRQ number, all ISRs will be called
 * when an interrupt on that line is fired.
 *
 * Arguments
 *  irq       System IRQ number
 *  info      Optional Name of IRQ source
 *  isr       Function pointer to the ISR
 *  arg       Second argument to function isr
 */
RTEMS_DEPRECATED static inline int BSP_shared_interrupt_register(
  int            irq,
  const char    *info,
  bsp_shared_isr isr,
  void          *arg
)
{
  return rtems_interrupt_handler_install(
    irq,
    info,
    RTEMS_INTERRUPT_SHARED,
    isr,
    arg
  );
}

/* Unregister previously registered shared IRQ handler.
 *
 * Arguments
 *  irq       System IRQ number
 *  isr       Function pointer to the ISR
 *  arg       Second argument to function isr
 */
RTEMS_DEPRECATED static inline int BSP_shared_interrupt_unregister(
  int            irq,
  bsp_shared_isr isr,
  void          *arg
)
{
  return rtems_interrupt_handler_remove( irq, isr, arg );
}

/* Clear interrupt pending on IRQ controller, this is typically done on a
 * level triggered interrupt source such as PCI to avoid taking double IRQs.
 * In such a case the interrupt source must be cleared first on LEON, before
 * acknowledging the IRQ with this function.
 *
 * Arguments
 *  irq       System IRQ number
 */
RTEMS_DEPRECATED static inline void BSP_shared_interrupt_clear( int irq )
{
  (void) rtems_interrupt_clear( (rtems_vector_number) irq );
}

/* Enable Interrupt. This function will unmask the IRQ at the interrupt
 * controller. This is normally done by _register(). Note that this will
 * affect all ISRs on this IRQ.
 *
 * Arguments
 *  irq       System IRQ number
 */
RTEMS_DEPRECATED static inline void BSP_shared_interrupt_unmask( int irq )
{
  (void) rtems_interrupt_vector_enable( (rtems_vector_number) irq );
}

/* Disable Interrupt. This function will mask one IRQ at the interrupt
 * controller. This is normally done by _unregister().  Note that this will
 * affect all ISRs on this IRQ.
 *
 * Arguments
 *  irq         System IRQ number
 */
RTEMS_DEPRECATED static inline void BSP_shared_interrupt_mask( int irq )
{
  (void) rtems_interrupt_vector_disable( (rtems_vector_number) irq );
}

#if defined( RTEMS_SMP ) || defined( RTEMS_MULTIPROCESSING )
/**
 * @brief This constant is the bus line of the shared memory driver and of the
 *   inter-processor interrupts.
 *
 * The BSP option LEON3_IPI_BUS_LINE defines the initial value.  The constant
 * is weakly linked, so an application may define it.  Where the BSP uses the
 * interrupt map, keep the value zero.  The BSP maps bus line 0 to the
 * controller line defined by the BSP option LEON3_IPI_CONTROLLER_LINE.
 */
extern const unsigned char LEON3_mp_irq;
#endif

/* Common driver build-time configurations. On small systems undefine
 * [DRIVER]_INFO_AVAIL to avoid info routines get dragged in. It is good
 * for debugging and printing information about the system, but makes the
 * image bigger.
 */
#define AMBAPPBUS_INFO_AVAIL /* AMBAPP Bus driver */
#define APBUART_INFO_AVAIL   /* APBUART Console driver */
#define GPTIMER_INFO_AVAIL   /* GPTIMER Timer driver */
#define GRETH_INFO_AVAIL     /* GRETH Ethernet driver */

#ifdef __cplusplus
}
#endif

/** @} */

#endif
