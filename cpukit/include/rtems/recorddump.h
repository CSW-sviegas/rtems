/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSRecord
 *
 * @brief This header file provides the interfaces of the event record dumps.
 */

/*
 * Copyright (C) 2020, 2026 embedded brains GmbH & Co. KG
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

/* Find related documentation with spec:/rtems/record/if/header-dump */

#ifndef _RTEMS_RECORDDUMP_H
#define _RTEMS_RECORDDUMP_H

#include <stddef.h>
#include <rtems/record.h>
#include <rtems/score/cpuopts.h>

#if !defined( RTEMS_QUAL )
  #include <zlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Find related documentation with spec:/rtems/record/if/dump-base64 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Dumps the event records in base64 encoding.
 *
 * @param[in] put_char is the handler which outputs a character.
 *
 * @param[in] arg is the argument of the handler.
 *
 * @par Notes
 * See rtems_record_dump().
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
void rtems_record_dump_base64( void ( *put_char )( int, void * ), void *arg );

#if !defined( RTEMS_QUAL )
/* Find related documentation with spec:/rtems/record/if/dump-base64-zlib-context */

/**
 * @ingroup RTEMSRecord
 *
 * @brief This structure provides the context of record dumps with zlib
 *   compression and base64 encoding.
 */
typedef struct {
  /**
   * @brief This member references the handler which outputs a character.
   */
  void ( *put_char )( int, void * );

  /**
   * @brief This member contains the argument of the handler.
   */
  void *arg;

  /**
   * @brief This member contains the count of buffered bytes.
   */
  int out;

  /**
   * @brief This member contains the buffer of the base64 encoding.
   */
  unsigned char buf[ 57 ];

  /**
   * @brief This member contains the zlib stream.
   */
  z_stream stream;

  /**
   * @brief This member references the begin of the free memory area for zlib.
   */
  char *mem_begin;

  /**
   * @brief This member contains the size of the free memory area for zlib in
   *   bytes.
   */
  size_t mem_available;

  /**
   * @brief This member provides the memory area for zlib.
   */
  char mem[ 0x80000 ];
} rtems_record_dump_base64_zlib_context;
#endif

/* Find related documentation with spec:/rtems/record/if/dump-chunk */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Record dump handlers which output a chunk of data shall have this
 *   type.
 *
 * The `arg` parameter is the argument passed to rtems_record_dump(). The
 * `data` parameter references the begin of the chunk. The `length` parameter
 * is the length of the chunk in bytes.
 */
typedef void ( *rtems_record_dump_chunk )(
  void       *arg,
  const void *data,
  size_t      length
);

/* Find related documentation with spec:/rtems/record/if/dump */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Dumps the stream header, the thread names, and the record items of
 *   all processors.
 *
 * @param[in] chunk is the handler which outputs a chunk of data.
 *
 * @param[in] arg is the argument of the handler.
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
void rtems_record_dump( rtems_record_dump_chunk chunk, void *arg );

#if !defined( RTEMS_QUAL )
/* Find related documentation with spec:/rtems/record/if/dump-zlib-base64 */

/**
 * @ingroup RTEMSRecord
 *
 * @brief Dumps the event records compressed with zlib in base64 encoding.
 *
 * @param[out] ctx is the context of the record dump. The context is too large
 *   for normal stack sizes.
 *
 * @param[in] put_char is the handler which outputs a character.
 *
 * @param[in] arg is the argument of the handler.
 *
 * @par Notes
 * See rtems_record_dump().
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
void rtems_record_dump_zlib_base64(
  rtems_record_dump_base64_zlib_context *ctx,
  void ( *put_char )( int, void * ),
  void *arg
);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RECORDDUMP_H */
