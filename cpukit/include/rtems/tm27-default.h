/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup shared_tm27
 *
 * @brief This header file provides an empty default of the time test 27
 *   support.
 */

/*
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

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/**
 * @defgroup RTEMSTimeTest27Support Time Test 27 Support
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief The Time Test 27 (TM27) support is used to measure the timing of
 *   the interrupt processing.
 *
 * The TMS27 support should provide two software generated interrupt requests,
 * one low priority request raised by Cause_tm27_intr() and one higher priority
 * request raised by Lower_tm27_intr().  Both requests should be cleared by
 * Clear_tm27_intr().  A handler provided by the test should be installed
 * through Install_tm27_vector().  This function should initialize the system
 * so that the software generated interrupt requests can be raised and cleared.
 *
 * If the raise functions Cause_tm27_intr() and Lower_tm27_intr() do not
 * trigger the interrupt immediately, then the TM27 support shall define
 * MUST_WAIT_FOR_INTERRUPT to one, otherwise MUST_WAIT_FOR_INTERRUPT shall be
 * defined to zero.
 *
 * The TM27 support may define TM27_INTERRUPT_VECTOR_DEFAULT to indicate the
 * interrupt vector of the interrupt request raised by Cause_tm27_intr().
 *
 * The TM27 support may define TM27_INTERRUPT_VECTOR_ALTERNATIVE to provide an
 * alternative software generated interrupt request which is raised by
 * _TM27_Raise_alternative() and cleared by _TM27_Clear_alternative().  Both
 * functions shall return an RTEMS status code.  This interrupt vector may be
 * used to test the interrupt controller support on targets which do not
 * provide generic software generated interrupts.
 */

/**
 *  @defgroup shared_tm27 Time Test 27 Default
 *
 *  @ingroup RTEMSTimeTest27Support
 *
 *  @brief Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( _handler ) \
  do { \
    (void) _handler; \
  } while (0)

#define Cause_tm27_intr()  \
  do { \
    ; \
  } while (0)


#define Clear_tm27_intr() \
  do { \
    ; \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    ; \
  } while (0)

#endif
