/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSetspecific
 */

/*
 * Copyright (C) 2026 Critical Software S.A.
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

#include <errno.h>
#include <pthread.h>
#include <rtems/posix/keyimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSetspecific spec:/c/pthread/req/setspecific
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CPthreadReqSetspecific_Pre_Key_Valid,
  CPthreadReqSetspecific_Pre_Key_Invalid,
  CPthreadReqSetspecific_Pre_Key_NA
} CPthreadReqSetspecific_Pre_Key;

typedef enum {
  CPthreadReqSetspecific_Pre_Value_Obj,
  CPthreadReqSetspecific_Pre_Value_Null,
  CPthreadReqSetspecific_Pre_Value_NA
} CPthreadReqSetspecific_Pre_Value;

typedef enum {
  CPthreadReqSetspecific_Pre_OtherKey_Yes,
  CPthreadReqSetspecific_Pre_OtherKey_No,
  CPthreadReqSetspecific_Pre_OtherKey_NA
} CPthreadReqSetspecific_Pre_OtherKey;

typedef enum {
  CPthreadReqSetspecific_Pre_Associated_Yes,
  CPthreadReqSetspecific_Pre_Associated_No,
  CPthreadReqSetspecific_Pre_Associated_NA
} CPthreadReqSetspecific_Pre_Associated;

typedef enum {
  CPthreadReqSetspecific_Pre_Pairs_Available,
  CPthreadReqSetspecific_Pre_Pairs_Exhausted,
  CPthreadReqSetspecific_Pre_Pairs_NA
} CPthreadReqSetspecific_Pre_Pairs;

typedef enum {
  CPthreadReqSetspecific_Post_Status_Ok,
  CPthreadReqSetspecific_Post_Status_Inval,
  CPthreadReqSetspecific_Post_Status_NoMem,
  CPthreadReqSetspecific_Post_Status_NA
} CPthreadReqSetspecific_Post_Status;

typedef enum {
  CPthreadReqSetspecific_Post_Value_Obj,
  CPthreadReqSetspecific_Post_Value_Null,
  CPthreadReqSetspecific_Post_Value_Nop,
  CPthreadReqSetspecific_Post_Value_NA
} CPthreadReqSetspecific_Post_Value;

typedef enum {
  CPthreadReqSetspecific_Post_Pairs_Allocated,
  CPthreadReqSetspecific_Post_Pairs_Released,
  CPthreadReqSetspecific_Post_Pairs_Nop,
  CPthreadReqSetspecific_Post_Pairs_NA
} CPthreadReqSetspecific_Post_Pairs;

typedef enum {
  CPthreadReqSetspecific_Post_OtherValues_Nop,
  CPthreadReqSetspecific_Post_OtherValues_NA
} CPthreadReqSetspecific_Post_OtherValues;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Key_NA : 1;
  uint16_t Pre_Value_NA : 1;
  uint16_t Pre_OtherKey_NA : 1;
  uint16_t Pre_Associated_NA : 1;
  uint16_t Pre_Pairs_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Value : 2;
  uint16_t Post_Pairs : 2;
  uint16_t Post_OtherValues : 1;
} CPthreadReqSetspecific_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/setspecific test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `key` parameter value.
   */
  pthread_key_t key;

  /**
   * @brief This member specifies the `value` parameter value.
   */
  const void *value;

  /**
   * @brief This member contains the return value of the pthread_setspecific()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the key created for the test.
   */
  pthread_key_t key_obj;

  /**
   * @brief This member contains the value which the calling thread associated
   *   with the key before the call.
   */
  void *value_before;

  /**
   * @brief This member contains the count of inactive key value pairs before
   *   the call.
   */
  size_t pairs_before;

  /**
   * @brief This member contains the key value pairs taken to use up the
   *   inactive key value pairs.
   */
  POSIX_Keys_Key_value_pair *pairs[ 16 ];

  /**
   * @brief This member contains the count of key value pairs taken to use up
   *   the inactive key value pairs.
   */
  size_t pairs_used;

  /**
   * @brief This member contains the keys other than the key created for the
   *   test. The first key is less than and the second key is greater than the
   *   key created for the test.
   */
  pthread_key_t other_keys[ 2 ];

  /**
   * @brief This member contains the values which the calling thread associated
   *   with the other keys before the call.
   */
  void *other_before[ 2 ];

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 5 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    CPthreadReqSetspecific_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSetspecific_Context;

static CPthreadReqSetspecific_Context CPthreadReqSetspecific_Instance;

static const char *const CPthreadReqSetspecific_PreDesc_Key[] =
  { "Valid", "Invalid", "NA" };

static const char *const CPthreadReqSetspecific_PreDesc_Value[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqSetspecific_PreDesc_OtherKey[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqSetspecific_PreDesc_Associated[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqSetspecific_PreDesc_Pairs[] =
  { "Available", "Exhausted", "NA" };

static const char *const *const CPthreadReqSetspecific_PreDesc[] = {
  CPthreadReqSetspecific_PreDesc_Key,
  CPthreadReqSetspecific_PreDesc_Value,
  CPthreadReqSetspecific_PreDesc_OtherKey,
  CPthreadReqSetspecific_PreDesc_Associated,
  CPthreadReqSetspecific_PreDesc_Pairs,
  NULL
};

#define KEY_INVALID ( (pthread_key_t) 0xffffffff )

static int old_value;

static int new_value;

static int other_value;

typedef CPthreadReqSetspecific_Context Context;

/*
 * The key under test gets the middle identifier of three new keys, so that
 * the other keys are on both sides of it in the tree of the calling thread.
 */
static void CreateKeys( Context *ctx )
{
  pthread_key_t keys[ 3 ];
  size_t        i;
  size_t        j;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( keys ); ++i ) {
    T_eq_int( pthread_key_create( &keys[ i ], NULL ), 0 );
  }

  for ( i = 1; i < RTEMS_ARRAY_SIZE( keys ); ++i ) {
    for ( j = i; j > 0 && keys[ j - 1 ] > keys[ j ]; --j ) {
      pthread_key_t key;

      key = keys[ j - 1 ];
      keys[ j - 1 ] = keys[ j ];
      keys[ j ] = key;
    }
  }

  ctx->other_keys[ 0 ] = keys[ 0 ];
  ctx->key_obj = keys[ 1 ];
  ctx->other_keys[ 1 ] = keys[ 2 ];
}

static void *GetValue( pthread_key_t key )
{
  const POSIX_Keys_Key_value_pair *pair;

  pair = _POSIX_Keys_Key_value_find( key, GetExecuting() );

  if ( pair == NULL ) {
    return NULL;
  }

  return pair->value;
}

static size_t CountFreePairs( void )
{
  const Chain_Control *chain;
  const Chain_Node    *node;
  size_t               count;

  chain = &_POSIX_Keys_Keypool.Free;
  node = _Chain_Immutable_first( chain );
  count = 0;

  while ( node != _Chain_Immutable_tail( chain ) ) {
    ++count;
    node = _Chain_Immutable_next( node );
  }

  return count;
}

static void CPthreadReqSetspecific_Pre_Key_Prepare(
  CPthreadReqSetspecific_Context *ctx,
  CPthreadReqSetspecific_Pre_Key  state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Pre_Key_Valid: {
      /*
       * While the `key` parameter is a valid thread-specific data key.
       */
      ctx->key = ctx->key_obj;
      break;
    }

    case CPthreadReqSetspecific_Pre_Key_Invalid: {
      /*
       * While the `key` parameter is not a valid thread-specific data key.
       */
      ctx->key = KEY_INVALID;
      break;
    }

    case CPthreadReqSetspecific_Pre_Key_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Pre_Value_Prepare(
  CPthreadReqSetspecific_Context  *ctx,
  CPthreadReqSetspecific_Pre_Value state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Pre_Value_Obj: {
      /*
       * While the `value` parameter is not NULL.
       */
      ctx->value = &new_value;
      break;
    }

    case CPthreadReqSetspecific_Pre_Value_Null: {
      /*
       * While the `value` parameter is NULL.
       */
      ctx->value = NULL;
      break;
    }

    case CPthreadReqSetspecific_Pre_Value_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Pre_OtherKey_Prepare(
  CPthreadReqSetspecific_Context     *ctx,
  CPthreadReqSetspecific_Pre_OtherKey state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Pre_OtherKey_Yes: {
      /*
       * While the calling thread has values associated with keys other than
       * the key specified by the `key` parameter.
       */
      T_eq_int( pthread_setspecific( ctx->other_keys[ 0 ], &other_value ), 0 );
      T_eq_int( pthread_setspecific( ctx->other_keys[ 1 ], &other_value ), 0 );
      break;
    }

    case CPthreadReqSetspecific_Pre_OtherKey_No: {
      /*
       * While the calling thread has no value associated with a key other than
       * the key specified by the `key` parameter.
       */
      T_null(
        _POSIX_Keys_Key_value_find( ctx->other_keys[ 0 ], GetExecuting() )
      );
      T_null(
        _POSIX_Keys_Key_value_find( ctx->other_keys[ 1 ], GetExecuting() )
      );
      break;
    }

    case CPthreadReqSetspecific_Pre_OtherKey_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Pre_Associated_Prepare(
  CPthreadReqSetspecific_Context       *ctx,
  CPthreadReqSetspecific_Pre_Associated state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Pre_Associated_Yes: {
      /*
       * While the calling thread has a value associated with the key specified
       * by the `key` parameter.
       */
      T_eq_int( pthread_setspecific( ctx->key_obj, &old_value ), 0 );
      break;
    }

    case CPthreadReqSetspecific_Pre_Associated_No: {
      /*
       * While the calling thread has no value associated with the key
       * specified by the `key` parameter.
       */
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, GetExecuting() ) );
      break;
    }

    case CPthreadReqSetspecific_Pre_Associated_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Pre_Pairs_Prepare(
  CPthreadReqSetspecific_Context  *ctx,
  CPthreadReqSetspecific_Pre_Pairs state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Pre_Pairs_Available: {
      /*
       * While at least one key value pair is inactive, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      T_gt_sz( CountFreePairs(), 0 );
      break;
    }

    case CPthreadReqSetspecific_Pre_Pairs_Exhausted: {
      /*
       * While no key value pair is inactive, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      _Objects_Allocator_lock();

      while ( ctx->pairs_used < RTEMS_ARRAY_SIZE( ctx->pairs ) ) {
        POSIX_Keys_Key_value_pair *pair;

        pair = _POSIX_Keys_Key_value_allocate();

        if ( pair == NULL ) {
          break;
        }

        ctx->pairs[ ctx->pairs_used ] = pair;
        ++ctx->pairs_used;
      }

      _Objects_Allocator_unlock();

      T_lt_sz( ctx->pairs_used, RTEMS_ARRAY_SIZE( ctx->pairs ) );
      T_eq_sz( CountFreePairs(), 0 );
      break;
    }

    case CPthreadReqSetspecific_Pre_Pairs_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Post_Status_Check(
  CPthreadReqSetspecific_Context    *ctx,
  CPthreadReqSetspecific_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Post_Status_Ok: {
      /*
       * The return value of pthread_setspecific() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSetspecific_Post_Status_Inval: {
      /*
       * The return value of pthread_setspecific() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqSetspecific_Post_Status_NoMem: {
      /*
       * The return value of pthread_setspecific() shall be ENOMEM.
       */
      T_eq_int( ctx->status, ENOMEM );
      break;
    }

    case CPthreadReqSetspecific_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Post_Value_Check(
  CPthreadReqSetspecific_Context   *ctx,
  CPthreadReqSetspecific_Post_Value state
)
{
  const POSIX_Keys_Key_value_pair *pair;

  switch ( state ) {
    case CPthreadReqSetspecific_Post_Value_Obj: {
      /*
       * The value which the calling thread associated with the key specified
       * by the `key` parameter shall be the value specified by the `value`
       * parameter.
       */
      pair = _POSIX_Keys_Key_value_find( ctx->key_obj, GetExecuting() );
      T_assert_not_null( pair );
      T_eq_ptr( pair->value, &new_value );
      break;
    }

    case CPthreadReqSetspecific_Post_Value_Null: {
      /*
       * The calling thread shall have no value associated with the key
       * specified by the `key` parameter.
       */
      T_null( _POSIX_Keys_Key_value_find( ctx->key_obj, GetExecuting() ) );
      break;
    }

    case CPthreadReqSetspecific_Post_Value_Nop: {
      /*
       * The value which the calling thread associated with the key specified
       * by the `key` parameter shall not be modified.
       */
      T_eq_ptr( GetValue( ctx->key_obj ), ctx->value_before );
      break;
    }

    case CPthreadReqSetspecific_Post_Value_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Post_Pairs_Check(
  CPthreadReqSetspecific_Context   *ctx,
  CPthreadReqSetspecific_Post_Pairs state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Post_Pairs_Allocated: {
      /*
       * One inactive key value pair shall be used, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      T_eq_sz( CountFreePairs(), ctx->pairs_before - 1 );
      break;
    }

    case CPthreadReqSetspecific_Post_Pairs_Released: {
      /*
       * One key value pair shall become inactive, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      T_eq_sz( CountFreePairs(), ctx->pairs_before + 1 );
      break;
    }

    case CPthreadReqSetspecific_Post_Pairs_Nop: {
      /*
       * The count of inactive key value pairs shall not be modified, see
       * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS.
       */
      T_eq_sz( CountFreePairs(), ctx->pairs_before );
      break;
    }

    case CPthreadReqSetspecific_Post_Pairs_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Post_OtherValues_Check(
  CPthreadReqSetspecific_Context         *ctx,
  CPthreadReqSetspecific_Post_OtherValues state
)
{
  switch ( state ) {
    case CPthreadReqSetspecific_Post_OtherValues_Nop: {
      /*
       * The values which the calling thread associated with keys other than
       * the key specified by the `key` parameter shall not be modified.
       */
      T_eq_ptr( GetValue( ctx->other_keys[ 0 ] ), ctx->other_before[ 0 ] );
      T_eq_ptr( GetValue( ctx->other_keys[ 1 ] ), ctx->other_before[ 1 ] );
      break;
    }

    case CPthreadReqSetspecific_Post_OtherValues_NA:
      break;
  }
}

static void CPthreadReqSetspecific_Prepare(
  CPthreadReqSetspecific_Context *ctx
)
{
  ctx->pairs_used = 0;
  CreateKeys( ctx );
}

static void CPthreadReqSetspecific_Action(
  CPthreadReqSetspecific_Context *ctx
)
{
  ctx->value_before = GetValue( ctx->key_obj );
  ctx->other_before[ 0 ] = GetValue( ctx->other_keys[ 0 ] );
  ctx->other_before[ 1 ] = GetValue( ctx->other_keys[ 1 ] );
  ctx->pairs_before = CountFreePairs();
  ctx->status = pthread_setspecific( ctx->key, ctx->value );
}

static void CPthreadReqSetspecific_Cleanup(
  CPthreadReqSetspecific_Context *ctx
)
{
  _Objects_Allocator_lock();

  while ( ctx->pairs_used > 0 ) {
    --ctx->pairs_used;
    _Freechain_Put( &_POSIX_Keys_Keypool, ctx->pairs[ ctx->pairs_used ] );
  }

  _Objects_Allocator_unlock();

  T_eq_int( pthread_key_delete( ctx->key_obj ), 0 );
  T_eq_int( pthread_key_delete( ctx->other_keys[ 0 ] ), 0 );
  T_eq_int( pthread_key_delete( ctx->other_keys[ 1 ] ), 0 );
}

/* clang-format off */

static const CPthreadReqSetspecific_Entry
CPthreadReqSetspecific_Entries[] = {
  { 1, 0, 0, 0, 0, 0, CPthreadReqSetspecific_Post_Status_NA,
    CPthreadReqSetspecific_Post_Value_NA, CPthreadReqSetspecific_Post_Pairs_NA,
    CPthreadReqSetspecific_Post_OtherValues_NA },
  { 0, 0, 0, 0, 0, 0, CPthreadReqSetspecific_Post_Status_Ok,
    CPthreadReqSetspecific_Post_Value_Obj,
    CPthreadReqSetspecific_Post_Pairs_Nop,
    CPthreadReqSetspecific_Post_OtherValues_Nop },
  { 0, 0, 0, 0, 0, 0, CPthreadReqSetspecific_Post_Status_Ok,
    CPthreadReqSetspecific_Post_Value_Null,
    CPthreadReqSetspecific_Post_Pairs_Released,
    CPthreadReqSetspecific_Post_OtherValues_Nop },
  { 0, 0, 0, 0, 0, 0, CPthreadReqSetspecific_Post_Status_Ok,
    CPthreadReqSetspecific_Post_Value_Null,
    CPthreadReqSetspecific_Post_Pairs_Nop,
    CPthreadReqSetspecific_Post_OtherValues_Nop },
  { 0, 0, 0, 1, 1, 0, CPthreadReqSetspecific_Post_Status_Inval,
    CPthreadReqSetspecific_Post_Value_NA,
    CPthreadReqSetspecific_Post_Pairs_Nop,
    CPthreadReqSetspecific_Post_OtherValues_Nop },
  { 0, 0, 0, 0, 0, 0, CPthreadReqSetspecific_Post_Status_Ok,
    CPthreadReqSetspecific_Post_Value_Obj,
    CPthreadReqSetspecific_Post_Pairs_Allocated,
    CPthreadReqSetspecific_Post_OtherValues_Nop },
  { 0, 0, 0, 0, 0, 0, CPthreadReqSetspecific_Post_Status_NoMem,
    CPthreadReqSetspecific_Post_Value_Nop,
    CPthreadReqSetspecific_Post_Pairs_Nop,
    CPthreadReqSetspecific_Post_OtherValues_Nop }
};

static const uint8_t
CPthreadReqSetspecific_Map[] = {
  1, 1, 5, 6, 1, 1, 5, 6, 2, 2, 3, 3, 2, 2, 3, 3, 4, 4, 0, 0, 0, 0, 0, 0, 4, 4,
  0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqSetspecific_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSetspecific_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqSetspecific_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqSetspecific_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqSetspecific_Scope,
  .initial_context = &CPthreadReqSetspecific_Instance
};

static inline CPthreadReqSetspecific_Entry CPthreadReqSetspecific_PopEntry(
  CPthreadReqSetspecific_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSetspecific_Entries[ CPthreadReqSetspecific_Map[ index ] ];
}

static void CPthreadReqSetspecific_SetPreConditionStates(
  CPthreadReqSetspecific_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_OtherKey_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqSetspecific_Pre_OtherKey_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Associated_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqSetspecific_Pre_Associated_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
}

static void CPthreadReqSetspecific_TestVariant(
  CPthreadReqSetspecific_Context *ctx
)
{
  CPthreadReqSetspecific_Pre_Key_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSetspecific_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSetspecific_Pre_OtherKey_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqSetspecific_Pre_Associated_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqSetspecific_Pre_Pairs_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqSetspecific_Action( ctx );
  CPthreadReqSetspecific_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqSetspecific_Post_Value_Check( ctx, ctx->Map.entry.Post_Value );
  CPthreadReqSetspecific_Post_Pairs_Check( ctx, ctx->Map.entry.Post_Pairs );
  CPthreadReqSetspecific_Post_OtherValues_Check(
    ctx,
    ctx->Map.entry.Post_OtherValues
  );
}

/**
 * @fn void T_case_body_CPthreadReqSetspecific( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqSetspecific, &CPthreadReqSetspecific_Fixture )
{
  CPthreadReqSetspecific_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqSetspecific_Pre_Key_Valid;
    ctx->Map.pci[ 0 ] < CPthreadReqSetspecific_Pre_Key_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqSetspecific_Pre_Value_Obj;
      ctx->Map.pci[ 1 ] < CPthreadReqSetspecific_Pre_Value_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqSetspecific_Pre_OtherKey_Yes;
        ctx->Map.pci[ 2 ] < CPthreadReqSetspecific_Pre_OtherKey_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqSetspecific_Pre_Associated_Yes;
          ctx->Map.pci[ 3 ] < CPthreadReqSetspecific_Pre_Associated_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqSetspecific_Pre_Pairs_Available;
            ctx->Map.pci[ 4 ] < CPthreadReqSetspecific_Pre_Pairs_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = CPthreadReqSetspecific_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            CPthreadReqSetspecific_SetPreConditionStates( ctx );
            CPthreadReqSetspecific_Prepare( ctx );
            CPthreadReqSetspecific_TestVariant( ctx );
            CPthreadReqSetspecific_Cleanup( ctx );
          }
        }
      }
    }
  }
}

/** @} */
