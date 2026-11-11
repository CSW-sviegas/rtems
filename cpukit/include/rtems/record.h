/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSRecord
 *
 * @brief This header file provides the interfaces of the event recording.
 */

/*
 * Copyright (C) 2018, 2026 embedded brains GmbH & Co. KG
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

/*
 * The spec:/... comments allow you to identify content which is
 * duplicated in the rtems and rtems-docs repositories.
 */

/* Find related documentation with spec:/rtems/record/if/header */

#ifndef _RTEMS_RECORD_H
#define _RTEMS_RECORD_H

#include <stddef.h>
#include <stdint.h>
#include <rtems/recorddata.h>
#include <rtems/recordimpl.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/cpuopts.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Find related documentation with spec:/rtems/record/if/caller */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event and an ::RTEMS_RECORD_LINE
 *   event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller() _Record_Caller( RTEMS_RETURN_ADDRESS() )

/* Find related documentation with spec:/rtems/record/if/caller-3 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and an extra event.
 *
 * @param[in] event is the event without a time stamp of the third item.
 *
 * @param[in] data is the event data of the third item.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_3( event, data ) \
  _Record_Caller_3( RTEMS_RETURN_ADDRESS(), event, data )

/* Find related documentation with spec:/rtems/record/if/caller-4 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and two extra events.
 *
 * @param[in] event_0 is the event without a time stamp of the third item.
 *
 * @param[in] data_0 is the event data of the third item.
 *
 * @param[in] event_1 is the event without a time stamp of the fourth item.
 *
 * @param[in] data_1 is the event data of the fourth item.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_4( event_0, data_0, event_1, data_1 ) \
  _Record_Caller_4( RTEMS_RETURN_ADDRESS(), event_0, data_0, event_1, data_1 )

/* Find related documentation with spec:/rtems/record/if/caller-arg */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and one argument event.
 *
 * @param[in] data is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg( data ) \
  _Record_Caller_arg( RTEMS_RETURN_ADDRESS(), data )

/* Find related documentation with spec:/rtems/record/if/caller-arg-10 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and ten argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * @param[in] data_8 is the event data of the ::RTEMS_RECORD_ARG_8 event.
 *
 * @param[in] data_9 is the event data of the ::RTEMS_RECORD_ARG_9 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_10( \
  data_0,                           \
  data_1,                           \
  data_2,                           \
  data_3,                           \
  data_4,                           \
  data_5,                           \
  data_6,                           \
  data_7,                           \
  data_8,                           \
  data_9                            \
)                                   \
  _Record_Caller_arg_10(            \
    RTEMS_RETURN_ADDRESS(),         \
    data_0,                         \
    data_1,                         \
    data_2,                         \
    data_3,                         \
    data_4,                         \
    data_5,                         \
    data_6,                         \
    data_7,                         \
    data_8,                         \
    data_9                          \
  )

/* Find related documentation with spec:/rtems/record/if/caller-arg-2 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and two argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_2( data_0, data_1 ) \
  _Record_Caller_arg_2( RTEMS_RETURN_ADDRESS(), data_0, data_1 )

/* Find related documentation with spec:/rtems/record/if/caller-arg-3 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and three argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_3( data_0, data_1, data_2 ) \
  _Record_Caller_arg_3( RTEMS_RETURN_ADDRESS(), data_0, data_1, data_2 )

/* Find related documentation with spec:/rtems/record/if/caller-arg-4 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and four argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_4( data_0, data_1, data_2, data_3 ) \
  _Record_Caller_arg_4(                                             \
    RTEMS_RETURN_ADDRESS(),                                         \
    data_0,                                                         \
    data_1,                                                         \
    data_2,                                                         \
    data_3                                                          \
  )

/* Find related documentation with spec:/rtems/record/if/caller-arg-5 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and five argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_5( data_0, data_1, data_2, data_3, data_4 ) \
  _Record_Caller_arg_5(                                                     \
    RTEMS_RETURN_ADDRESS(),                                                 \
    data_0,                                                                 \
    data_1,                                                                 \
    data_2,                                                                 \
    data_3,                                                                 \
    data_4                                                                  \
  )

/* Find related documentation with spec:/rtems/record/if/caller-arg-6 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and six argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_6( \
  data_0,                          \
  data_1,                          \
  data_2,                          \
  data_3,                          \
  data_4,                          \
  data_5                           \
)                                  \
  _Record_Caller_arg_6(            \
    RTEMS_RETURN_ADDRESS(),        \
    data_0,                        \
    data_1,                        \
    data_2,                        \
    data_3,                        \
    data_4,                        \
    data_5                         \
  )

/* Find related documentation with spec:/rtems/record/if/caller-arg-7 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and seven argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_7( \
  data_0,                          \
  data_1,                          \
  data_2,                          \
  data_3,                          \
  data_4,                          \
  data_5,                          \
  data_6                           \
)                                  \
  _Record_Caller_arg_7(            \
    RTEMS_RETURN_ADDRESS(),        \
    data_0,                        \
    data_1,                        \
    data_2,                        \
    data_3,                        \
    data_4,                        \
    data_5,                        \
    data_6                         \
  )

/* Find related documentation with spec:/rtems/record/if/caller-arg-8 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and eight argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_8( \
  data_0,                          \
  data_1,                          \
  data_2,                          \
  data_3,                          \
  data_4,                          \
  data_5,                          \
  data_6,                          \
  data_7                           \
)                                  \
  _Record_Caller_arg_8(            \
    RTEMS_RETURN_ADDRESS(),        \
    data_0,                        \
    data_1,                        \
    data_2,                        \
    data_3,                        \
    data_4,                        \
    data_5,                        \
    data_6,                        \
    data_7                         \
  )

/* Find related documentation with spec:/rtems/record/if/caller-arg-9 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_CALLER event, an ::RTEMS_RECORD_LINE
 *   event, and nine argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * @param[in] data_8 is the event data of the ::RTEMS_RECORD_ARG_8 event.
 *
 * The event data of the ::RTEMS_RECORD_CALLER event is the return address of
 * the function which calls the macro. The event data of the
 * ::RTEMS_RECORD_LINE event is the address of the macro call.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_caller_arg_9( \
  data_0,                          \
  data_1,                          \
  data_2,                          \
  data_3,                          \
  data_4,                          \
  data_5,                          \
  data_6,                          \
  data_7,                          \
  data_8                           \
)                                  \
  _Record_Caller_arg_9(            \
    RTEMS_RETURN_ADDRESS(),        \
    data_0,                        \
    data_1,                        \
    data_2,                        \
    data_3,                        \
    data_4,                        \
    data_5,                        \
    data_6,                        \
    data_7,                        \
    data_8                         \
  )

/* Find related documentation with spec:/rtems/record/if/context */

/**
 * @ingroup RTEMSRecord
 *
 * @brief This structure provides a context to add and commit record items.
 *
 * Only the record context directives shall access the members of the
 * structure.
 */
typedef struct {
  /**
   * @brief This member references the record control of the processor.
   */
  Record_Control *control;

  /**
   * @brief This member contains the head index.
   */
  unsigned int head;

  /**
   * @brief This member contains the time event of the context.
   */
  uint32_t now;

  /**
   * @brief This member contains the interrupt level.
   */
  uint32_t level;
} rtems_record_context;

/* Find related documentation with spec:/rtems/record/if/add */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Adds the record item to the record context.
 *
 * @param[in,out] context is the record context which rtems_record_prepare() or
 *   rtems_record_prepare_critical() initialized.
 *
 * @param[in] event is the event without a time stamp.
 *
 * @param[in] data is the event data.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
static inline void rtems_record_add(
  rtems_record_context *context,
  rtems_record_event    event,
  rtems_record_data     data
)
{
  Record_Control    *control;
  rtems_record_item *item;
  unsigned int       head;

  control = context->control;
  head = context->head;
  item = &control->Items[ _Record_Index( control, head ) ];
  context->head = head + 1;

  item->event = context->now | event;
  item->data = data;
}

/* Find related documentation with spec:/rtems/record/if/commit-critical */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Commits the record items of the record context in a critical section
 *   with interrupts disabled.
 *
 * @param[in] context is the record context which
 *   rtems_record_prepare_critical() initialized.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
static inline void rtems_record_commit_critical(
  rtems_record_context *context
)
{
  _Atomic_Store_uint(
    &context->control->head,
    context->head,
    ATOMIC_ORDER_RELEASE
  );
}

/* Find related documentation with spec:/rtems/record/if/commit */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Commits the record items of the record context and restores the
 *   interrupt level.
 *
 * @param[in] context is the record context which rtems_record_prepare()
 *   initialized.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
static inline void rtems_record_commit( rtems_record_context *context )
{
  rtems_record_commit_critical( context );
  RTEMS_COMPILER_MEMORY_BARRIER();
  _CPU_ISR_Enable( context->level );
}

/* Find related documentation with spec:/rtems/record/if/entry-10 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and ten argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * @param[in] data_8 is the event data of the ::RTEMS_RECORD_ARG_8 event.
 *
 * @param[in] data_9 is the event data of the ::RTEMS_RECORD_ARG_9 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_10( \
  event,                       \
  data_0,                      \
  data_1,                      \
  data_2,                      \
  data_3,                      \
  data_4,                      \
  data_5,                      \
  data_6,                      \
  data_7,                      \
  data_8,                      \
  data_9                       \
)                              \
  _Record_Entry_10(            \
    RTEMS_RETURN_ADDRESS(),    \
    event,                     \
    data_0,                    \
    data_1,                    \
    data_2,                    \
    data_3,                    \
    data_4,                    \
    data_5,                    \
    data_6,                    \
    data_7,                    \
    data_8,                    \
    data_9                     \
  )

/* Find related documentation with spec:/rtems/record/if/entry-2 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and two argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_2( event, data_0, data_1 ) \
  _Record_Entry_2( RTEMS_RETURN_ADDRESS(), event, data_0, data_1 )

/* Find related documentation with spec:/rtems/record/if/entry-3 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and three argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_3( event, data_0, data_1, data_2 ) \
  _Record_Entry_3( RTEMS_RETURN_ADDRESS(), event, data_0, data_1, data_2 )

/* Find related documentation with spec:/rtems/record/if/entry-4 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and four argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_4( event, data_0, data_1, data_2, data_3 ) \
  _Record_Entry_4(                                                    \
    RTEMS_RETURN_ADDRESS(),                                           \
    event,                                                            \
    data_0,                                                           \
    data_1,                                                           \
    data_2,                                                           \
    data_3                                                            \
  )

/* Find related documentation with spec:/rtems/record/if/entry-5 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and five argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_5( event, data_0, data_1, data_2, data_3, data_4 ) \
  _Record_Entry_5(                                                            \
    RTEMS_RETURN_ADDRESS(),                                                   \
    event,                                                                    \
    data_0,                                                                   \
    data_1,                                                                   \
    data_2,                                                                   \
    data_3,                                                                   \
    data_4                                                                    \
  )

/* Find related documentation with spec:/rtems/record/if/entry-6 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and six argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_6( \
  event,                      \
  data_0,                     \
  data_1,                     \
  data_2,                     \
  data_3,                     \
  data_4,                     \
  data_5                      \
)                             \
  _Record_Entry_6(            \
    RTEMS_RETURN_ADDRESS(),   \
    event,                    \
    data_0,                   \
    data_1,                   \
    data_2,                   \
    data_3,                   \
    data_4,                   \
    data_5                    \
  )

/* Find related documentation with spec:/rtems/record/if/entry-7 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and seven argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_7( \
  event,                      \
  data_0,                     \
  data_1,                     \
  data_2,                     \
  data_3,                     \
  data_4,                     \
  data_5,                     \
  data_6                      \
)                             \
  _Record_Entry_7(            \
    RTEMS_RETURN_ADDRESS(),   \
    event,                    \
    data_0,                   \
    data_1,                   \
    data_2,                   \
    data_3,                   \
    data_4,                   \
    data_5,                   \
    data_6                    \
  )

/* Find related documentation with spec:/rtems/record/if/entry-8 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and eight argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_8( \
  event,                      \
  data_0,                     \
  data_1,                     \
  data_2,                     \
  data_3,                     \
  data_4,                     \
  data_5,                     \
  data_6,                     \
  data_7                      \
)                             \
  _Record_Entry_8(            \
    RTEMS_RETURN_ADDRESS(),   \
    event,                    \
    data_0,                   \
    data_1,                   \
    data_2,                   \
    data_3,                   \
    data_4,                   \
    data_5,                   \
    data_6,                   \
    data_7                    \
  )

/* Find related documentation with spec:/rtems/record/if/entry-9 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and nine argument events.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * @param[in] data_8 is the event data of the ::RTEMS_RECORD_ARG_8 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_9( \
  event,                      \
  data_0,                     \
  data_1,                     \
  data_2,                     \
  data_3,                     \
  data_4,                     \
  data_5,                     \
  data_6,                     \
  data_7,                     \
  data_8                      \
)                             \
  _Record_Entry_9(            \
    RTEMS_RETURN_ADDRESS(),   \
    event,                    \
    data_0,                   \
    data_1,                   \
    data_2,                   \
    data_3,                   \
    data_4,                   \
    data_5,                   \
    data_6,                   \
    data_7,                   \
    data_8                    \
  )

/* Find related documentation with spec:/rtems/record/if/exit-10 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and ten return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_RETURN_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_RETURN_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_RETURN_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_RETURN_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_RETURN_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_RETURN_7 event.
 *
 * @param[in] data_8 is the event data of the ::RTEMS_RECORD_RETURN_8 event.
 *
 * @param[in] data_9 is the event data of the ::RTEMS_RECORD_RETURN_9 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_10( \
  event,                      \
  data_0,                     \
  data_1,                     \
  data_2,                     \
  data_3,                     \
  data_4,                     \
  data_5,                     \
  data_6,                     \
  data_7,                     \
  data_8,                     \
  data_9                      \
)                             \
  _Record_Exit_10(            \
    RTEMS_RETURN_ADDRESS(),   \
    event,                    \
    data_0,                   \
    data_1,                   \
    data_2,                   \
    data_3,                   \
    data_4,                   \
    data_5,                   \
    data_6,                   \
    data_7,                   \
    data_8,                   \
    data_9                    \
  )

/* Find related documentation with spec:/rtems/record/if/exit-2 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and two return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_2( event, data_0, data_1 ) \
  _Record_Exit_2( RTEMS_RETURN_ADDRESS(), event, data_0, data_1 )

/* Find related documentation with spec:/rtems/record/if/exit-3 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and three return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_RETURN_2 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_3( event, data_0, data_1, data_2 ) \
  _Record_Exit_3( RTEMS_RETURN_ADDRESS(), event, data_0, data_1, data_2 )

/* Find related documentation with spec:/rtems/record/if/exit-4 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and four return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_RETURN_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_RETURN_3 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_4( event, data_0, data_1, data_2, data_3 ) \
  _Record_Exit_4(                                                    \
    RTEMS_RETURN_ADDRESS(),                                          \
    event,                                                           \
    data_0,                                                          \
    data_1,                                                          \
    data_2,                                                          \
    data_3                                                           \
  )

/* Find related documentation with spec:/rtems/record/if/exit-5 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and five return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_RETURN_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_RETURN_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_RETURN_4 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_5( event, data_0, data_1, data_2, data_3, data_4 ) \
  _Record_Exit_5(                                                            \
    RTEMS_RETURN_ADDRESS(),                                                  \
    event,                                                                   \
    data_0,                                                                  \
    data_1,                                                                  \
    data_2,                                                                  \
    data_3,                                                                  \
    data_4                                                                   \
  )

/* Find related documentation with spec:/rtems/record/if/exit-6 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and six return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_RETURN_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_RETURN_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_RETURN_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_RETURN_5 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_6( \
  event,                     \
  data_0,                    \
  data_1,                    \
  data_2,                    \
  data_3,                    \
  data_4,                    \
  data_5                     \
)                            \
  _Record_Exit_6(            \
    RTEMS_RETURN_ADDRESS(),  \
    event,                   \
    data_0,                  \
    data_1,                  \
    data_2,                  \
    data_3,                  \
    data_4,                  \
    data_5                   \
  )

/* Find related documentation with spec:/rtems/record/if/exit-7 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and seven return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_RETURN_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_RETURN_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_RETURN_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_RETURN_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_RETURN_6 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_7( \
  event,                     \
  data_0,                    \
  data_1,                    \
  data_2,                    \
  data_3,                    \
  data_4,                    \
  data_5,                    \
  data_6                     \
)                            \
  _Record_Exit_7(            \
    RTEMS_RETURN_ADDRESS(),  \
    event,                   \
    data_0,                  \
    data_1,                  \
    data_2,                  \
    data_3,                  \
    data_4,                  \
    data_5,                  \
    data_6                   \
  )

/* Find related documentation with spec:/rtems/record/if/exit-8 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and eight return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_RETURN_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_RETURN_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_RETURN_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_RETURN_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_RETURN_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_RETURN_7 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_8( \
  event,                     \
  data_0,                    \
  data_1,                    \
  data_2,                    \
  data_3,                    \
  data_4,                    \
  data_5,                    \
  data_6,                    \
  data_7                     \
)                            \
  _Record_Exit_8(            \
    RTEMS_RETURN_ADDRESS(),  \
    event,                   \
    data_0,                  \
    data_1,                  \
    data_2,                  \
    data_3,                  \
    data_4,                  \
    data_5,                  \
    data_6,                  \
    data_7                   \
  )

/* Find related documentation with spec:/rtems/record/if/exit-9 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and nine return events.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_RETURN_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_RETURN_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_RETURN_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_RETURN_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_RETURN_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_RETURN_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_RETURN_7 event.
 *
 * @param[in] data_8 is the event data of the ::RTEMS_RECORD_RETURN_8 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_9( \
  event,                     \
  data_0,                    \
  data_1,                    \
  data_2,                    \
  data_3,                    \
  data_4,                    \
  data_5,                    \
  data_6,                    \
  data_7,                    \
  data_8                     \
)                            \
  _Record_Exit_9(            \
    RTEMS_RETURN_ADDRESS(),  \
    event,                   \
    data_0,                  \
    data_1,                  \
    data_2,                  \
    data_3,                  \
    data_4,                  \
    data_5,                  \
    data_6,                  \
    data_7,                  \
    data_8                   \
  )

/* Find related documentation with spec:/rtems/record/if/fetch-control */

/**
 * @ingroup RTEMSRecord
 *
 * @brief This structure controls the record fetches of rtems_record_fetch().
 *
 * rtems_record_fetch_initialize() shall initialize the structure.
 */
typedef struct {
  /**
   * @brief This member references the first item which the last call of
   *   rtems_record_fetch() fetched.
   */
  rtems_record_item *fetched_items;

  /**
   * @brief This member contains the count of items which the last call of
   *   rtems_record_fetch() fetched.
   */
  size_t fetched_count;

  /**
   * @brief This member contains the state of the record fetches.
   *
   * Only rtems_record_fetch_initialize() and rtems_record_fetch() shall access
   * the members.
   */
  struct {
    #if defined( RTEMS_SMP )
    /**
     * @brief This member contains the index of the processor from which the next
     *   record items are fetched.
     */
    uint32_t cpu_index;
    #endif

    /**
     * @brief This member contains the count of record items which need to be
     *   fetched from the current processor before the next processor is selected.
     */
    size_t cpu_todo;

    /**
     * @brief This member references the item array which stores the fetched items.
     */
    rtems_record_item *storage_items;

    /**
     * @brief This member contains the count of items of the array referenced by
     *   `storage_items`.
     */
    size_t storage_item_count;
  } internal;
} rtems_record_fetch_control;

/* Find related documentation with spec:/rtems/record/if/fetch-initialize */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Initializes the record fetch control.
 *
 * @param[out] control is the record fetch control.
 *
 * @param[out] items is the item array which stores the fetched items.
 *
 * @param[in] count is the count of items of the array referenced by `items`.
 *
 * The function shall be called before the record fetch control is passed to
 * rtems_record_fetch().
 *
 * @par Notes
 * See rtems_record_get_item_count_for_fetch().
 */
void rtems_record_fetch_initialize(
  rtems_record_fetch_control *control,
  rtems_record_item          *items,
  size_t                      count
);

/* Find related documentation with spec:/rtems/record/if/fetch-status */

/**
 * @ingroup RTEMSRecord
 *
 * @brief This enumeration defines the status codes which rtems_record_fetch()
 *   returns.
 */
typedef enum {
  /**
   * @brief This status indicates that the current round of record fetches for
   *   all configured processors is done.
   */
  RTEMS_RECORD_FETCH_DONE,

  /**
   * @brief This status indicates that the current round of record fetches for
   *   all configured processors has to continue.
   */
  RTEMS_RECORD_FETCH_CONTINUE,

  /**
   * @brief This status indicates that the item count of the fetch control is
   *   invalid.
   */
  RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT
} rtems_record_fetch_status;

/* Find related documentation with spec:/rtems/record/if/fetch */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Fetches record items from the record buffers of the processors.
 *
 * @param[in,out] control is the record fetch control which
 *   rtems_record_fetch_initialize() initialized.
 *
 * The function returns the fetched items and their count through the record
 * fetch control.
 *
 * @retval ::RTEMS_RECORD_FETCH_DONE The current round of record fetches for
 *   all configured processors is done.
 *
 * @retval ::RTEMS_RECORD_FETCH_CONTINUE The current round of record fetches
 *   for all configured processors has to continue.
 *
 * @retval ::RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT The item count of the record
 *   fetch control is invalid.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
rtems_record_fetch_status rtems_record_fetch(
  rtems_record_fetch_control *control
);

/* Find related documentation with spec:/rtems/record/if/get-item-count-for-fetch */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Gets the item count which fetches all available items of one
 *   processor through one call of rtems_record_fetch().
 *
 * The value depends on @ref CONFIGURE_RECORD_PER_PROCESSOR_ITEMS.
 *
 * @return Returns the item count which fetches all available items of one
 *   processor through one call of rtems_record_fetch().
 */
size_t rtems_record_get_item_count_for_fetch( void );

/* Find related documentation with spec:/rtems/record/if/interrupt-disable */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Disables interrupts and produces an ::RTEMS_RECORD_ISR_DISABLE event.
 *
 * The event data is the return address of the function.
 *
 * @return Returns the previous interrupt level.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
uint32_t rtems_record_interrupt_disable( void );

/* Find related documentation with spec:/rtems/record/if/interrupt-enable */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Restores the interrupt level and produces an
 *   ::RTEMS_RECORD_ISR_ENABLE event.
 *
 * @param[in] level is the previous interrupt level. It shall be the value
 *   which the corresponding call of rtems_record_interrupt_disable() returned.
 *
 * The event data is the return address of the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_interrupt_enable( uint32_t level );

/* Find related documentation with spec:/rtems/record/if/line */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line( void );

/* Find related documentation with spec:/rtems/record/if/line-2 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and an extra event.
 *
 * @param[in] event is the event without a time stamp of the second item.
 *
 * @param[in] data is the event data of the second item.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_2( rtems_record_event event, rtems_record_data data );

/* Find related documentation with spec:/rtems/record/if/line-3 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and two extra events.
 *
 * @param[in] event_0 is the event without a time stamp of the second item.
 *
 * @param[in] data_0 is the event data of the second item.
 *
 * @param[in] event_1 is the event without a time stamp of the third item.
 *
 * @param[in] data_1 is the event data of the third item.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_3(
  rtems_record_event event_0,
  rtems_record_data  data_0,
  rtems_record_event event_1,
  rtems_record_data  data_1
);

/* Find related documentation with spec:/rtems/record/if/line-arg */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and one argument event.
 *
 * @param[in] data is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg( rtems_record_data data );

/* Find related documentation with spec:/rtems/record/if/line-arg-10 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and ten argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * @param[in] data_8 is the event data of the ::RTEMS_RECORD_ARG_8 event.
 *
 * @param[in] data_9 is the event data of the ::RTEMS_RECORD_ARG_9 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_10(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7,
  rtems_record_data data_8,
  rtems_record_data data_9
);

/* Find related documentation with spec:/rtems/record/if/line-arg-2 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and two argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_2(
  rtems_record_data data_0,
  rtems_record_data data_1
);

/* Find related documentation with spec:/rtems/record/if/line-arg-3 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and three argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_3(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2
);

/* Find related documentation with spec:/rtems/record/if/line-arg-4 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and four argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_4(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3
);

/* Find related documentation with spec:/rtems/record/if/line-arg-5 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and five argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_5(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4
);

/* Find related documentation with spec:/rtems/record/if/line-arg-6 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and six argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_6(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5
);

/* Find related documentation with spec:/rtems/record/if/line-arg-7 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and seven argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_7(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6
);

/* Find related documentation with spec:/rtems/record/if/line-arg-8 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and eight argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_8(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7
);

/* Find related documentation with spec:/rtems/record/if/line-arg-9 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces an ::RTEMS_RECORD_LINE event and nine argument events.
 *
 * @param[in] data_0 is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * @param[in] data_1 is the event data of the ::RTEMS_RECORD_ARG_1 event.
 *
 * @param[in] data_2 is the event data of the ::RTEMS_RECORD_ARG_2 event.
 *
 * @param[in] data_3 is the event data of the ::RTEMS_RECORD_ARG_3 event.
 *
 * @param[in] data_4 is the event data of the ::RTEMS_RECORD_ARG_4 event.
 *
 * @param[in] data_5 is the event data of the ::RTEMS_RECORD_ARG_5 event.
 *
 * @param[in] data_6 is the event data of the ::RTEMS_RECORD_ARG_6 event.
 *
 * @param[in] data_7 is the event data of the ::RTEMS_RECORD_ARG_7 event.
 *
 * @param[in] data_8 is the event data of the ::RTEMS_RECORD_ARG_8 event.
 *
 * The event data of the ::RTEMS_RECORD_LINE event is the address of the call
 * to the function.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_line_arg_9(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7,
  rtems_record_data data_8
);

/* Find related documentation with spec:/rtems/record/if/prepare */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Disables interrupts and prepares to add and commit record items.
 *
 * @param[out] context is the record context which shall be used for the
 *   following rtems_record_add() and rtems_record_commit() calls. The context
 *   may have an arbitrary content at function entry.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
static inline void rtems_record_prepare( rtems_record_context *context )
{
  uint32_t               level;
  const Per_CPU_Control *cpu_self;
  Record_Control        *control;
  unsigned int           head;

  _CPU_ISR_Disable( level );
  RTEMS_COMPILER_MEMORY_BARRIER();
  context->now = RTEMS_RECORD_TIME_EVENT( _Record_Now(), 0 );
  context->level = level;
  cpu_self = _Per_CPU_Get();
  control = cpu_self->record;
  context->control = control;
  head = _Record_Head( control );
  context->head = head;
}

/* Find related documentation with spec:/rtems/record/if/prepare-critical */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Prepares to add and commit record items in a critical section with
 *   interrupts disabled.
 *
 * @param[out] context is the record context which shall be used for the
 *   following rtems_record_add() and rtems_record_commit_critical() calls. The
 *   context may have an arbitrary content at function entry.
 *
 * @param[in] cpu_self is the control of the current processor.
 *
 * The function does not disable interrupts. Interrupts shall be disabled from
 * the call of the function to the call of rtems_record_commit_critical().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
static inline void rtems_record_prepare_critical(
  rtems_record_context  *context,
  const Per_CPU_Control *cpu_self
)
{
  Record_Control *control;
  unsigned int    head;

  context->now = RTEMS_RECORD_TIME_EVENT( _Record_Now(), 0 );
  control = cpu_self->record;
  context->control = control;
  head = _Record_Head( control );
  context->head = head;
}

/* Find related documentation with spec:/rtems/record/if/produce */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces a record item.
 *
 * @param[in] event is the event without a time stamp.
 *
 * @param[in] data is the event data.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_produce( rtems_record_event event, rtems_record_data data );

/* Find related documentation with spec:/rtems/record/if/entry */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event.
 *
 * @param[in] event is the entry event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry( event ) \
  rtems_record_produce( event, (rtems_record_data) RTEMS_RETURN_ADDRESS() )

/* Find related documentation with spec:/rtems/record/if/exit */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event.
 *
 * @param[in] event is the exit event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit( event ) \
  rtems_record_produce( event, (rtems_record_data) RTEMS_RETURN_ADDRESS() )

/* Find related documentation with spec:/rtems/record/if/produce-2 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces two record items.
 *
 * @param[in] event_0 is the event without a time stamp of the first item.
 *
 * @param[in] data_0 is the event data of the first item.
 *
 * @param[in] event_1 is the event without a time stamp of the second item.
 *
 * @param[in] data_1 is the event data of the second item.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_produce_2(
  rtems_record_event event_0,
  rtems_record_data  data_0,
  rtems_record_event event_1,
  rtems_record_data  data_1
);

/* Find related documentation with spec:/rtems/record/if/entry-1 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the entry event and an argument event.
 *
 * @param[in] event is the entry event.
 *
 * @param[in] data is the event data of the ::RTEMS_RECORD_ARG_0 event.
 *
 * The event data of the entry event is the return address of the function
 * which calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_entry_1( event, data )     \
  rtems_record_produce_2(                       \
    event,                                      \
    (rtems_record_data) RTEMS_RETURN_ADDRESS(), \
    RTEMS_RECORD_ARG_0,                         \
    data                                        \
  )

/* Find related documentation with spec:/rtems/record/if/exit-1 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the exit event and a return event.
 *
 * @param[in] event is the exit event.
 *
 * @param[in] data is the event data of the ::RTEMS_RECORD_RETURN_0 event.
 *
 * The event data of the exit event is the return address of the function which
 * calls the macro.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
#define rtems_record_exit_1( event, data )      \
  rtems_record_produce_2(                       \
    event,                                      \
    (rtems_record_data) RTEMS_RETURN_ADDRESS(), \
    RTEMS_RECORD_RETURN_0,                      \
    data                                        \
  )

/* Find related documentation with spec:/rtems/record/if/produce-n */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Produces the record items.
 *
 * @param[in] items is the array of record items. The events of the items have
 *   no time stamp.
 *
 * @param[in] n is the count of items in the array.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive shall be called only where the @ref
 *   CONFIGURE_RECORD_PER_PROCESSOR_ITEMS application configuration option is
 *   greater than zero.
 * @endparblock
 */
void rtems_record_produce_n( const rtems_record_item *items, size_t n );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RECORD_H */
