/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplRecord
 *
 * @brief This source file contains the implementation of
 *   rtems_record_dump().
 */

/*
 * Copyright (C) 2020, 2024 embedded brains GmbH & Co. KG
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

#include <rtems/recorddump.h>
#include <rtems/score/threadimpl.h>

typedef struct {
  rtems_record_dump_chunk chunk;
  void                   *arg;
} dump_context;

static void dump_chunk( dump_context *ctx, const void *data, size_t length )
{
  ( *ctx->chunk )( ctx->arg, data, length );
}

static bool thread_names_visitor( rtems_tcb *tcb, void *arg )
{
  dump_context     *ctx;
  rtems_record_data data;
  char              name[ 2 * THREAD_DEFAULT_MAXIMUM_NAME_SIZE ];
  rtems_record_item items[ 1 + sizeof( name ) / sizeof( data ) ];
  size_t            len;
  size_t            used;

  ctx = arg;
  items[ 0 ].event = RTEMS_RECORD_THREAD_ID;
  items[ 0 ].data = tcb->Object.id;

  len = _Thread_Get_name( tcb, name, sizeof( name ) );

  if ( len >= sizeof( name ) ) {
    len = sizeof( name ) - 1;
  }

  used = _Record_String_to_items(
    RTEMS_RECORD_THREAD_NAME,
    name,
    len,
    &items[ 1 ],
    RTEMS_ARRAY_SIZE( items ) - 1
  );
  dump_chunk( ctx, items, ( used + 1 ) * sizeof( items[ 0 ] ) );

  return false;
}

void rtems_record_dump( rtems_record_dump_chunk chunk, void *arg )
{
  Record_Stream_header       header;
  size_t                     size;
  dump_context               ctx;
  rtems_record_fetch_control control;
  rtems_record_item          items[ 128 ];
  rtems_record_fetch_status  status;

  ctx.chunk = chunk;
  ctx.arg = arg;

  size = _Record_Stream_header_initialize( &header );
  dump_chunk( &ctx, &header, size );
  _Thread_Iterate( thread_names_visitor, &ctx );
  rtems_record_fetch_initialize(
    &control,
    &items[ 0 ],
    RTEMS_ARRAY_SIZE( items )
  );

  do {
    status = rtems_record_fetch( &control );
    dump_chunk(
      &ctx,
      control.fetched_items,
      control.fetched_count * sizeof( *control.fetched_items )
    );
  } while ( status == RTEMS_RECORD_FETCH_CONTINUE );
}
