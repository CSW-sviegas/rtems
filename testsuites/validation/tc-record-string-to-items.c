/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordReqStringToItems
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

#include <rtems/record.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsRecordReqStringToItems spec:/rtems/record/req/string-to-items
 *
 * @ingroup TestsuitesValidationRecord0
 *
 * @{
 */

typedef enum {
  RtemsRecordReqStringToItems_Pre_Length_Empty,
  RtemsRecordReqStringToItems_Pre_Length_PartialItem,
  RtemsRecordReqStringToItems_Pre_Length_FullItems,
  RtemsRecordReqStringToItems_Pre_Length_NA
} RtemsRecordReqStringToItems_Pre_Length;

typedef enum {
  RtemsRecordReqStringToItems_Pre_Capacity_Less,
  RtemsRecordReqStringToItems_Pre_Capacity_Equal,
  RtemsRecordReqStringToItems_Pre_Capacity_Greater,
  RtemsRecordReqStringToItems_Pre_Capacity_NA
} RtemsRecordReqStringToItems_Pre_Capacity;

typedef enum {
  RtemsRecordReqStringToItems_Post_Items_Packed,
  RtemsRecordReqStringToItems_Post_Items_NA
} RtemsRecordReqStringToItems_Post_Items;

typedef enum {
  RtemsRecordReqStringToItems_Post_Count_Needed,
  RtemsRecordReqStringToItems_Post_Count_Capacity,
  RtemsRecordReqStringToItems_Post_Count_NA
} RtemsRecordReqStringToItems_Post_Count;

typedef enum {
  RtemsRecordReqStringToItems_Post_Rest_Unchanged,
  RtemsRecordReqStringToItems_Post_Rest_NA
} RtemsRecordReqStringToItems_Post_Rest;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Length_NA : 1;
  uint8_t Pre_Capacity_NA : 1;
  uint8_t Post_Items : 1;
  uint8_t Post_Count : 2;
  uint8_t Post_Rest : 1;
} RtemsRecordReqStringToItems_Entry;

/**
 * @brief Test context for spec:/rtems/record/req/string-to-items test case.
 */
typedef struct {
  /**
   * @brief This member contains the length of the string.
   */
  size_t length;

  /**
   * @brief This member contains the count of items which the string needs.
   */
  size_t needed;

  /**
   * @brief This member contains the count of items of the item array.
   */
  size_t item_count;

  /**
   * @brief This member provides the item array.
   */
  rtems_record_item items[ 8 ];

  /**
   * @brief This member contains the return value of the conversion.
   */
  size_t count;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    RtemsRecordReqStringToItems_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsRecordReqStringToItems_Context;

static RtemsRecordReqStringToItems_Context
  RtemsRecordReqStringToItems_Instance;

static const char *const RtemsRecordReqStringToItems_PreDesc_Length[] =
  { "Empty", "PartialItem", "FullItems", "NA" };

static const char *const RtemsRecordReqStringToItems_PreDesc_Capacity[] =
  { "Less", "Equal", "Greater", "NA" };

static const char *const *const RtemsRecordReqStringToItems_PreDesc[] = {
  RtemsRecordReqStringToItems_PreDesc_Length,
  RtemsRecordReqStringToItems_PreDesc_Capacity,
  NULL
};

typedef RtemsRecordReqStringToItems_Context Context;

#define DATA_SIZE sizeof( rtems_record_data )

#define FILL_EVENT UINT32_C( 0xffffffff )

#define FILL_DATA ( (rtems_record_data) 0x5a5a5a5a )

static const char string[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnop";

static size_t Needed( size_t length )
{
  return ( length + DATA_SIZE - 1 ) / DATA_SIZE;
}

static void CheckPacked( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->count && i < ctx->item_count; ++i ) {
    rtems_record_data expected;
    size_t            j;

    expected = 0;

    for ( j = 0; j < DATA_SIZE; ++j ) {
      size_t k;

      k = i * DATA_SIZE + j;

      if ( k < ctx->length ) {
        expected |= (rtems_record_data) (unsigned char) string[ k ]
                    << ( 8 * j );
      }
    }

    T_eq_u32( ctx->items[ i ].event, RTEMS_RECORD_USER( 3 ) );
    T_eq_ulong( ctx->items[ i ].data, expected );
  }
}

static void CheckRest( const Context *ctx )
{
  size_t i;

  for ( i = ctx->count; i < RTEMS_ARRAY_SIZE( ctx->items ); ++i ) {
    T_eq_u32( ctx->items[ i ].event, FILL_EVENT );
    T_eq_ulong( ctx->items[ i ].data, FILL_DATA );
  }
}

static void RtemsRecordReqStringToItems_Pre_Length_Prepare(
  RtemsRecordReqStringToItems_Context   *ctx,
  RtemsRecordReqStringToItems_Pre_Length state
)
{
  switch ( state ) {
    case RtemsRecordReqStringToItems_Pre_Length_Empty: {
      /*
       * While the string is empty.
       */
      ctx->length = 0;
      break;
    }

    case RtemsRecordReqStringToItems_Pre_Length_PartialItem: {
      /*
       * While the length of the string is not a multiple of the size of the
       * event data.
       */
      ctx->length = 2 * DATA_SIZE + 1;
      break;
    }

    case RtemsRecordReqStringToItems_Pre_Length_FullItems: {
      /*
       * While the length of the string is a positive multiple of the size of
       * the event data.
       */
      ctx->length = 2 * DATA_SIZE;
      break;
    }

    case RtemsRecordReqStringToItems_Pre_Length_NA:
      break;
  }
}

static void RtemsRecordReqStringToItems_Pre_Capacity_Prepare(
  RtemsRecordReqStringToItems_Context     *ctx,
  RtemsRecordReqStringToItems_Pre_Capacity state
)
{
  switch ( state ) {
    case RtemsRecordReqStringToItems_Pre_Capacity_Less: {
      /*
       * While the item array has less items than the string needs.
       */
      ctx->item_count = Needed( ctx->length ) - 1;
      break;
    }

    case RtemsRecordReqStringToItems_Pre_Capacity_Equal: {
      /*
       * While the item array has as many items as the string needs.
       */
      ctx->item_count = Needed( ctx->length );
      break;
    }

    case RtemsRecordReqStringToItems_Pre_Capacity_Greater: {
      /*
       * While the item array has more items than the string needs.
       */
      ctx->item_count = Needed( ctx->length ) + 2;
      break;
    }

    case RtemsRecordReqStringToItems_Pre_Capacity_NA:
      break;
  }
}

static void RtemsRecordReqStringToItems_Post_Items_Check(
  RtemsRecordReqStringToItems_Context   *ctx,
  RtemsRecordReqStringToItems_Post_Items state
)
{
  switch ( state ) {
    case RtemsRecordReqStringToItems_Post_Items_Packed: {
      /*
       * Each item which the event recording wrote shall hold the event and the
       * next characters of the string in the bytes of its event data, the
       * first character in the least significant byte, and zero bytes after
       * the last character.
       */
      CheckPacked( ctx );
      break;
    }

    case RtemsRecordReqStringToItems_Post_Items_NA:
      break;
  }
}

static void RtemsRecordReqStringToItems_Post_Count_Check(
  RtemsRecordReqStringToItems_Context   *ctx,
  RtemsRecordReqStringToItems_Post_Count state
)
{
  switch ( state ) {
    case RtemsRecordReqStringToItems_Post_Count_Needed: {
      /*
       * The event recording shall return the count of items which the string
       * needs.
       */
      T_eq_sz( ctx->count, ctx->needed );
      break;
    }

    case RtemsRecordReqStringToItems_Post_Count_Capacity: {
      /*
       * The event recording shall return the count of items of the item array.
       */
      T_eq_sz( ctx->count, ctx->item_count );
      break;
    }

    case RtemsRecordReqStringToItems_Post_Count_NA:
      break;
  }
}

static void RtemsRecordReqStringToItems_Post_Rest_Check(
  RtemsRecordReqStringToItems_Context  *ctx,
  RtemsRecordReqStringToItems_Post_Rest state
)
{
  switch ( state ) {
    case RtemsRecordReqStringToItems_Post_Rest_Unchanged: {
      /*
       * The items of the item array after the written items shall be
       * unchanged.
       */
      CheckRest( ctx );
      break;
    }

    case RtemsRecordReqStringToItems_Post_Rest_NA:
      break;
  }
}

static void RtemsRecordReqStringToItems_Action(
  RtemsRecordReqStringToItems_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.entry.Skip ) {
    return;
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->items ); ++i ) {
    ctx->items[ i ].event = FILL_EVENT;
    ctx->items[ i ].data = FILL_DATA;
  }

  ctx->needed = Needed( ctx->length );
  T_quiet_le_sz( ctx->item_count, RTEMS_ARRAY_SIZE( ctx->items ) );
  ctx->count = _Record_String_to_items(
    RTEMS_RECORD_USER( 3 ),
    string,
    ctx->length,
    ctx->items,
    ctx->item_count
  );
}

/* clang-format off */

static const RtemsRecordReqStringToItems_Entry
RtemsRecordReqStringToItems_Entries[] = {
  { 0, 0, 0, RtemsRecordReqStringToItems_Post_Items_Packed,
    RtemsRecordReqStringToItems_Post_Count_Needed,
    RtemsRecordReqStringToItems_Post_Rest_Unchanged },
  { 0, 0, 0, RtemsRecordReqStringToItems_Post_Items_Packed,
    RtemsRecordReqStringToItems_Post_Count_Capacity,
    RtemsRecordReqStringToItems_Post_Rest_Unchanged },
  { 1, 0, 0, RtemsRecordReqStringToItems_Post_Items_NA,
    RtemsRecordReqStringToItems_Post_Count_NA,
    RtemsRecordReqStringToItems_Post_Rest_NA }
};

static const uint8_t
RtemsRecordReqStringToItems_Map[] = {
  2, 0, 0, 1, 0, 0, 1, 0, 0
};

/* clang-format on */

static size_t RtemsRecordReqStringToItems_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsRecordReqStringToItems_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsRecordReqStringToItems_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsRecordReqStringToItems_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsRecordReqStringToItems_Scope,
  .initial_context = &RtemsRecordReqStringToItems_Instance
};

static inline RtemsRecordReqStringToItems_Entry
RtemsRecordReqStringToItems_PopEntry(
  RtemsRecordReqStringToItems_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsRecordReqStringToItems_Entries
    [ RtemsRecordReqStringToItems_Map[ index ] ];
}

static void RtemsRecordReqStringToItems_TestVariant(
  RtemsRecordReqStringToItems_Context *ctx
)
{
  RtemsRecordReqStringToItems_Pre_Length_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsRecordReqStringToItems_Pre_Capacity_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsRecordReqStringToItems_Action( ctx );
  RtemsRecordReqStringToItems_Post_Items_Check(
    ctx,
    ctx->Map.entry.Post_Items
  );
  RtemsRecordReqStringToItems_Post_Count_Check(
    ctx,
    ctx->Map.entry.Post_Count
  );
  RtemsRecordReqStringToItems_Post_Rest_Check( ctx, ctx->Map.entry.Post_Rest );
}

/**
 * @fn void T_case_body_RtemsRecordReqStringToItems( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsRecordReqStringToItems,
  &RtemsRecordReqStringToItems_Fixture
)
{
  RtemsRecordReqStringToItems_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsRecordReqStringToItems_Pre_Length_Empty;
    ctx->Map.pcs[ 0 ] < RtemsRecordReqStringToItems_Pre_Length_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsRecordReqStringToItems_Pre_Capacity_Less;
      ctx->Map.pcs[ 1 ] < RtemsRecordReqStringToItems_Pre_Capacity_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsRecordReqStringToItems_PopEntry( ctx );
      RtemsRecordReqStringToItems_TestVariant( ctx );
    }
  }
}

/** @} */
