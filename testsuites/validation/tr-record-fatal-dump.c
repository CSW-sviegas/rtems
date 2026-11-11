/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordValFatalDump
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <string.h>
#include <rtems/bspIo.h>
#include <rtems/record.h>
#include <rtems/sysinit.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/timecounter.h>

#include "tr-record-fatal-dump.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsRecordValFatalDump spec:/rtems/record/val/fatal-dump
 *
 * @ingroup TestsuitesFatalRecordDump
 *
 * @brief Tests the fatal error dump of the event recording.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by the system initialization. It replaces
 *   the kernel character output by a handler which collects the output and
 *   triggers a fatal error. A fatal extension which runs after the fatal
 *   extension of the event recording restores the kernel character output.
 *
 *   - Check that the fatal error has the expected source and code.
 *
 *   - Check that the system output the begin line, a base64 encoding of a
 *     record dump, and the end line.
 *
 *   - Check that the last items of the processor in the dump are the uptime
 *     items followed by the fatal source item and the fatal code item.
 *
 * - Wait until each other processor produced the items of its fatal error,
 *   which is the SMP shutdown response.
 *
 *   - Check that each other processor produced the uptime items, the fatal
 *     source item, and the fatal code item of the SMP shutdown response.
 *
 * - Call the fatal extension of the event recording a second time with another
 *   fatal code.
 *
 *   - Check that the system produced no output for the second fatal error.
 *
 *   - Check that the system produced the uptime items, the fatal source item,
 *     and the fatal code item for the second fatal error.
 *
 * - Call the fatal extension of the event recording with a fatal error of the
 *   SMP source which is not the SMP shutdown response. The action covers the
 *   branch of the fatal extension for the SMP source. The dump of the first
 *   action suppresses the output of this fatal error.
 *
 *   - Check that the system produced no output for this fatal error.
 *
 *   - Check that the system produced the uptime items, the fatal source item,
 *     and the fatal code item of this fatal error.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/record/val/fatal-dump test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsRecordValFatalDump_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsRecordValFatalDump_Run() parameter.
   */
  rtems_fatal_code code;
} RtemsRecordValFatalDump_Context;

static RtemsRecordValFatalDump_Context RtemsRecordValFatalDump_Instance;

#define FATAL_CODE 0x12345678

#define BEGIN_LINE "\n*** BEGIN OF RECORDS BASE64 ***\n"

#define END_LINE "\n*** END OF RECORDS BASE64 ***\n"

typedef struct {
  const uint8_t *data;
  size_t         size;
  size_t         pos;
} Stream;

static BSP_output_char_function_type output_char;

static char output[ 8192 ];

static size_t output_size;

static uint8_t dump[ 4096 ];

static sbintime_t uptime_before;

static size_t decoded_size;

static uint32_t fatal_cpu_index;

static void CaptureChar( char c )
{
  if ( c != '\r' && output_size < sizeof( output ) ) {
    output[ output_size ] = c;
    ++output_size;
  }
}

static void StartCapture( void )
{
  output_size = 0;
  output_char = BSP_output_char;
  BSP_output_char = CaptureChar;
}

static void StopCapture( void )
{
  BSP_output_char = output_char;
}

static sbintime_t uptime_after;

void RtemsRecordValFatalDump_StopCapture(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  (void) always_set_to_false;

#if defined( RTEMS_SMP )
  if (
    source == RTEMS_FATAL_SOURCE_SMP && code == SMP_FATAL_SHUTDOWN_RESPONSE
  ) {
    /*
     * The other processors stop here, so that the processor of the fatal
     * error alone stops the capture and runs the test.
     */
    while ( true ) {
      /* Wait for the end of the test */
    }
  }
#else
  (void) source;
  (void) code;
#endif

  StopCapture();
  uptime_after = _Timecounter_Sbinuptime();
}

static void Trigger( void )
{
  fatal_cpu_index = rtems_scheduler_get_processor();
  uptime_before = _Timecounter_Sbinuptime();
  StartCapture();
  rtems_fatal( RTEMS_FATAL_SOURCE_APPLICATION, FATAL_CODE );
}

RTEMS_SYSINIT_ITEM(
  Trigger,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST
);

static int Base64Value( char c )
{
  if ( c >= 'A' && c <= 'Z' ) {
    return c - 'A';
  }

  if ( c >= 'a' && c <= 'z' ) {
    return c - 'a' + 26;
  }

  if ( c >= '0' && c <= '9' ) {
    return c - '0' + 52;
  }

  if ( c == '+' ) {
    return 62;
  }

  if ( c == '/' ) {
    return 63;
  }

  return -1;
}

static size_t Base64Decode( const char *in, size_t in_size )
{
  uint32_t value;
  int      bits;
  size_t   i;
  size_t   n;

  value = 0;
  bits = 0;
  n = 0;

  for ( i = 0; i < in_size; ++i ) {
    int v;

    if ( in[ i ] == '\n' ) {
      continue;
    }

    if ( in[ i ] == '=' ) {
      break;
    }

    v = Base64Value( in[ i ] );

    if ( v < 0 ) {
      return SIZE_MAX;
    }

    value = ( value << 6 ) | (uint32_t) v;
    bits += 6;

    if ( bits >= 8 ) {
      bits -= 8;

      if ( n >= sizeof( dump ) ) {
        return SIZE_MAX;
      }

      dump[ n ] = (uint8_t) ( value >> bits );
      ++n;
    }
  }

  return n;
}

static bool StreamNext( Stream *stream, rtems_record_item *item )
{
  if ( stream->pos + sizeof( *item ) > stream->size ) {
    return false;
  }

  memcpy( item, &stream->data[ stream->pos ], sizeof( *item ) );
  stream->pos += sizeof( *item );
  return true;
}

static void PushLast( rtems_record_item *last, const rtems_record_item *item )
{
  last[ 0 ] = last[ 1 ];
  last[ 1 ] = last[ 2 ];
  last[ 2 ] = last[ 3 ];
  last[ 3 ] = *item;
}

static void CheckFatalItems(
  const rtems_record_item *items,
  rtems_fatal_source       source,
  rtems_fatal_code         code,
  sbintime_t               before,
  sbintime_t               after
)
{
  sbintime_t uptime;

  T_eq_u32(
    RTEMS_RECORD_GET_EVENT( items[ 0 ].event ),
    RTEMS_RECORD_UPTIME_LOW
  );
  T_eq_u32(
    RTEMS_RECORD_GET_EVENT( items[ 1 ].event ),
    RTEMS_RECORD_UPTIME_HIGH
  );
  T_eq_u32(
    RTEMS_RECORD_GET_EVENT( items[ 2 ].event ),
    RTEMS_RECORD_FATAL_SOURCE
  );
  T_eq_ulong( items[ 2 ].data, source );
  T_eq_u32(
    RTEMS_RECORD_GET_EVENT( items[ 3 ].event ),
    RTEMS_RECORD_FATAL_CODE
  );
  T_eq_ulong( items[ 3 ].data, code );
  uptime = (sbintime_t) ( ( (uint64_t) items[ 1 ].data << 32 ) |
                          (uint32_t) items[ 0 ].data );
  T_ge_ull( (unsigned long long) uptime, (unsigned long long) before );
  T_le_ull( (unsigned long long) uptime, (unsigned long long) after );
}

/*
 * Get the last four items of the section of the processor in the record
 * stream of the dump.
 */
static bool GetLastItemsOfDump(
  size_t             size,
  uint32_t           cpu_index,
  rtems_record_item *last
)
{
  Stream            stream;
  rtems_record_item item;
  uint32_t          magic;
  size_t            count;
  bool              in_section;

  stream.data = dump;
  stream.size = size;
  stream.pos = 2 * sizeof( uint32_t );

  if ( size < stream.pos ) {
    return false;
  }

  memcpy( &magic, &dump[ sizeof( uint32_t ) ], sizeof( magic ) );

  if ( magic != RTEMS_RECORD_MAGIC ) {
    return false;
  }

  count = 0;
  in_section = false;

  while ( StreamNext( &stream, &item ) ) {
    if ( item.event == RTEMS_RECORD_PROCESSOR ) {
      in_section = item.data == cpu_index;
      continue;
    }

    if ( in_section ) {
      PushLast( last, &item );
      ++count;
    }
  }

  return count >= 4;
}

static bool IsShutdownResponse( const rtems_record_item *last )
{
#if defined( RTEMS_SMP )
  return RTEMS_RECORD_GET_EVENT( last[ 2 ].event ) ==
           RTEMS_RECORD_FATAL_SOURCE &&
         last[ 2 ].data == RTEMS_FATAL_SOURCE_SMP &&
         RTEMS_RECORD_GET_EVENT( last[ 3 ].event ) ==
           RTEMS_RECORD_FATAL_CODE &&
         last[ 3 ].data == SMP_FATAL_SHUTDOWN_RESPONSE;
#else
  (void) last;
  return false;
#endif
}

static bool GetLastItemsOfBuffer(
  uint32_t           cpu_index,
  rtems_record_item *last
);

/*
 * Wait until the processor produced the items of the SMP shutdown response.
 * They are in the dump or in the record buffer of the processor.
 */
static bool WaitForShutdownResponse(
  uint32_t           cpu_index,
  rtems_record_item *last
)
{
  uint32_t i;

  memset( last, 0, 4 * sizeof( *last ) );

  if (
    decoded_size != SIZE_MAX &&
    GetLastItemsOfDump( decoded_size, cpu_index, last ) &&
    IsShutdownResponse( last )
  ) {
    return true;
  }

  for ( i = 0; i < 1000000; ++i ) {
    if (
      GetLastItemsOfBuffer( cpu_index, last ) && IsShutdownResponse( last )
    ) {
      return true;
    }
  }

  return false;
}

static bool GetLastItemsOfBuffer( uint32_t cpu_index, rtems_record_item *last )
{
  rtems_record_fetch_control control;
  rtems_record_item          items[ 64 ];
  rtems_record_fetch_status  status;
  size_t                     count;

  rtems_record_fetch_initialize( &control, items, RTEMS_ARRAY_SIZE( items ) );
  count = 0;

  do {
    uint32_t current;
    size_t   i;

    status = rtems_record_fetch( &control );
    current = (uint32_t) control.fetched_items[ 0 ].data;

    for ( i = 1; i < control.fetched_count; ++i ) {
      const rtems_record_item *item;

      item = &control.fetched_items[ i ];

      if (
        current != cpu_index || item->event == RTEMS_RECORD_PER_CPU_OVERFLOW
      ) {
        continue;
      }

      PushLast( last, item );
      ++count;
    }
  } while ( status == RTEMS_RECORD_FETCH_CONTINUE );

  return count >= 4;
}

static T_fixture RtemsRecordValFatalDump_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsRecordValFatalDump_Instance
};

/**
 * @brief The test action is carried out by the system initialization. It
 *   replaces the kernel character output by a handler which collects the
 *   output and triggers a fatal error. A fatal extension which runs after the
 *   fatal extension of the event recording restores the kernel character
 *   output.
 */
static void RtemsRecordValFatalDump_Action_0(
  RtemsRecordValFatalDump_Context *ctx
)
{
  rtems_fatal_source source;
  rtems_fatal_code   code;
  size_t             begin_size;
  size_t             end_size;
  rtems_record_item  last[ 4 ];
  bool               has_last;

  source = ctx->source;
  code = ctx->code;
  begin_size = sizeof( BEGIN_LINE ) - 1;
  end_size = sizeof( END_LINE ) - 1;
  decoded_size = SIZE_MAX;
  memset( last, 0, sizeof( last ) );
  has_last = false;

  if ( output_size >= begin_size + end_size ) {
    decoded_size = Base64Decode(
      &output[ begin_size ],
      output_size - begin_size - end_size
    );
  }

  if ( decoded_size != SIZE_MAX ) {
    has_last = GetLastItemsOfDump( decoded_size, fatal_cpu_index, last );
  }

  /*
   * Check that the fatal error has the expected source and code.
   */
  T_eq_int( source, RTEMS_FATAL_SOURCE_APPLICATION );
  T_eq_ulong( code, FATAL_CODE );

  /*
   * Check that the system output the begin line, a base64 encoding of a record
   * dump, and the end line.
   */
  T_ge_sz( output_size, begin_size + end_size );
  T_eq_mem( &output[ 0 ], BEGIN_LINE, begin_size );
  T_eq_mem( &output[ output_size - end_size ], END_LINE, end_size );
  T_ne_sz( decoded_size, SIZE_MAX );
  T_true( has_last );

  /*
   * Check that the last items of the processor in the dump are the uptime
   * items followed by the fatal source item and the fatal code item.
   */
  CheckFatalItems( last, source, code, uptime_before, uptime_after );
}

/**
 * @brief Wait until each other processor produced the items of its fatal
 *   error, which is the SMP shutdown response.
 */
static void RtemsRecordValFatalDump_Action_1( void )
{
  uint32_t cpu_index;
  uint32_t cpu_max;
  size_t   responses;

  cpu_max = rtems_scheduler_get_processor_maximum();
  responses = 0;

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    rtems_record_item last[ 4 ];

    if ( cpu_index == fatal_cpu_index ) {
      continue;
    }

    if ( WaitForShutdownResponse( cpu_index, last ) ) {
      CheckFatalItems(
        last,
        last[ 2 ].data,
        last[ 3 ].data,
        uptime_before,
        _Timecounter_Sbinuptime()
      );
      ++responses;
    }
  }

  /*
   * Check that each other processor produced the uptime items, the fatal
   * source item, and the fatal code item of the SMP shutdown response.
   */
  T_eq_sz( responses, cpu_max - 1 );
}

/**
 * @brief Call the fatal extension of the event recording a second time with
 *   another fatal code.
 */
static void RtemsRecordValFatalDump_Action_2(
  RtemsRecordValFatalDump_Context *ctx
)
{
  rtems_fatal_source source;
  rtems_fatal_code   code;
  sbintime_t         before;
  sbintime_t         after;
  rtems_record_item  last[ 4 ];
  bool               has_last;

  source = ctx->source;
  code = ctx->code;
  memset( last, 0, sizeof( last ) );
  before = _Timecounter_Sbinuptime();
  StartCapture();
  _Record_Fatal_dump_base64( source, false, code + 1 );
  StopCapture();
  after = _Timecounter_Sbinuptime();
  has_last = GetLastItemsOfBuffer( fatal_cpu_index, last );

  /*
   * Check that the system produced no output for the second fatal error.
   */
  T_eq_sz( output_size, 0 );

  /*
   * Check that the system produced the uptime items, the fatal source item,
   * and the fatal code item for the second fatal error.
   */
  T_true( has_last );
  CheckFatalItems( last, source, code + 1, before, after );
}

/**
 * @brief Call the fatal extension of the event recording with a fatal error of
 *   the SMP source which is not the SMP shutdown response. The action covers
 *   the branch of the fatal extension for the SMP source. The dump of the
 *   first action suppresses the output of this fatal error.
 */
static void RtemsRecordValFatalDump_Action_3( void )
{
  rtems_fatal_source source;
  rtems_fatal_code   code;
  sbintime_t         before;
  sbintime_t         after;
  rtems_record_item  last[ 4 ];
  bool               has_last;

  source = RTEMS_FATAL_SOURCE_SMP;
  code = SMP_FATAL_SHUTDOWN;
  memset( last, 0, sizeof( last ) );
  before = _Timecounter_Sbinuptime();
  StartCapture();
  _Record_Fatal_dump_base64( source, false, code );
  StopCapture();
  after = _Timecounter_Sbinuptime();
  has_last = GetLastItemsOfBuffer( fatal_cpu_index, last );

  /*
   * Check that the system produced no output for this fatal error.
   */
  T_eq_sz( output_size, 0 );

  /*
   * Check that the system produced the uptime items, the fatal source item,
   * and the fatal code item of this fatal error.
   */
  T_true( has_last );
  CheckFatalItems( last, source, code, before, after );
}

void RtemsRecordValFatalDump_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  RtemsRecordValFatalDump_Context *ctx;

  ctx = &RtemsRecordValFatalDump_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "RtemsRecordValFatalDump",
    &RtemsRecordValFatalDump_Fixture
  );

  RtemsRecordValFatalDump_Action_0( ctx );
  RtemsRecordValFatalDump_Action_1();
  RtemsRecordValFatalDump_Action_2( ctx );
  RtemsRecordValFatalDump_Action_3();

  T_case_end();
}

/** @} */
