/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplRecord
 *
 * @brief This header file provides the implementation interfaces of the
 *   event recording.
 */

/*
 * Copyright (C) 2018, 2024 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_RECORDIMPL_H
#define _RTEMS_RECORDIMPL_H

#include <rtems/recorddata.h>

#include <rtems/rtems/intr.h>
#include <rtems/score/atomic.h>
#include <rtems/score/cpu.h>
#include <rtems/score/interr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/watchdog.h>
#include <rtems/counter.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSImplRecord Event Recording
 *
 * @ingroup RTEMSImpl
 *
 * @brief This group contains the event recording implementation.
 *
 * @{
 */

typedef struct Record_Control {
  Atomic_Uint      head;
  unsigned int     tail;
  unsigned int     mask;
  Watchdog_Control Watchdog;
  RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )
  rtems_record_item Items[ RTEMS_ZERO_LENGTH_ARRAY ];
} Record_Control;

typedef struct {
  unsigned int    item_count;
  Record_Control *controls;
} Record_Configuration;

extern const Record_Configuration _Record_Configuration;

void _Record_Initialize( void );

void _Record_Interrupt_initialize( void );

extern rtems_interrupt_entry *_Record_Interrupt_dispatch_table[];

bool _Record_Thread_create(
  struct _Thread_Control *executing,
  struct _Thread_Control *created
);

void _Record_Thread_start(
  struct _Thread_Control *executing,
  struct _Thread_Control *started
);

void _Record_Thread_restart(
  struct _Thread_Control *executing,
  struct _Thread_Control *restarted
);

void _Record_Thread_delete(
  struct _Thread_Control *executing,
  struct _Thread_Control *deleted
);

void _Record_Thread_switch(
  struct _Thread_Control *executing,
  struct _Thread_Control *heir
);

void _Record_Thread_begin( struct _Thread_Control *executing );

void _Record_Thread_exitted( struct _Thread_Control *executing );

void _Record_Fatal_dump_base64(
  Internal_errors_Source source,
  bool                   always_set_to_false,
  Internal_errors_t      code
);

void _Record_Fatal_dump_base64_zlib(
  Internal_errors_Source source,
  bool                   always_set_to_false,
  Internal_errors_t      code
);

void _Record_Thread_terminate( struct _Thread_Control *executing );

static inline unsigned int _Record_Index(
  const Record_Control *control,
  unsigned int          index
)
{
  return index & control->mask;
}

static inline unsigned int _Record_Head( Record_Control *control )
{
#ifdef RTEMS_SMP
  /*
   * Use a read-modify-write operation to get the last value stored by the
   * record producer.
   */
  return _Atomic_Fetch_add_uint( &control->head, 0, ATOMIC_ORDER_ACQUIRE );
#else
  return _Atomic_Load_uint( &control->head, ATOMIC_ORDER_ACQUIRE );
#endif
}

static inline unsigned int _Record_Tail( const Record_Control *control )
{
  return control->tail;
}

static inline bool _Record_Is_overflow(
  const Record_Control *control,
  unsigned int          tail,
  unsigned int          head
)
{
  return head - tail >= control->mask + 1U;
}

static inline unsigned int _Record_Capacity(
  const Record_Control *control,
  unsigned int          tail,
  unsigned int          head
)
{
  return ( tail - head - 1U ) & control->mask;
}

static inline rtems_counter_ticks _Record_Now( void )
{
  return rtems_counter_read();
}

typedef struct RTEMS_PACKED {
  uint32_t          format;
  uint32_t          magic;
  rtems_record_item Version;
  rtems_record_item Processor_maximum;
  rtems_record_item Count;
  rtems_record_item Frequency;
  rtems_record_item Info[ 64 ];
} Record_Stream_header;

size_t _Record_Stream_header_initialize( Record_Stream_header *header );

size_t _Record_String_to_items(
  rtems_record_event event,
  const char        *str,
  size_t             len,
  rtems_record_item *items,
  size_t             item_count
);

void _Record_Caller( void *return_address );

void _Record_Caller_3(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data
);

void _Record_Caller_4(
  void              *return_address,
  rtems_record_event event_0,
  rtems_record_data  data_0,
  rtems_record_event event_1,
  rtems_record_data  data_1
);

void _Record_Caller_arg( void *return_address, rtems_record_data data );

void _Record_Caller_arg_2(
  void             *return_address,
  rtems_record_data data_0,
  rtems_record_data data_1
);

void _Record_Caller_arg_3(
  void             *return_address,
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2
);

void _Record_Caller_arg_4(
  void             *return_address,
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3
);

void _Record_Caller_arg_5(
  void             *return_address,
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4
);

void _Record_Caller_arg_6(
  void             *return_address,
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5
);

void _Record_Caller_arg_7(
  void             *return_address,
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6
);

void _Record_Caller_arg_8(
  void             *return_address,
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7
);

void _Record_Caller_arg_9(
  void             *return_address,
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

void _Record_Caller_arg_10(
  void             *return_address,
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

void _Record_Entry_2(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1
);

void _Record_Entry_3(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2
);

void _Record_Entry_4(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3
);

void _Record_Entry_5(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4
);

void _Record_Entry_6(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5
);

void _Record_Entry_7(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6
);

void _Record_Entry_8(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7
);

void _Record_Entry_9(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7,
  rtems_record_data  data_8
);

void _Record_Entry_10(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7,
  rtems_record_data  data_8,
  rtems_record_data  data_9
);

void _Record_Exit_2(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1
);

void _Record_Exit_3(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2
);

void _Record_Exit_4(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3
);

void _Record_Exit_5(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4
);

void _Record_Exit_6(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5
);

void _Record_Exit_7(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6
);

void _Record_Exit_8(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7
);

void _Record_Exit_9(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7,
  rtems_record_data  data_8
);

void _Record_Exit_10(
  void              *return_address,
  rtems_record_event event,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7,
  rtems_record_data  data_8,
  rtems_record_data  data_9
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RECORDIMPL_H */
