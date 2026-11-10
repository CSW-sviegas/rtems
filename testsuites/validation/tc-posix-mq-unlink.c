/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqUnlink
 */

/*
 * Copyright (C) 2026 Critical Software S.A.
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

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <mqueue.h>
#include <string.h>
#include <rtems/posix/mqueueimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqUnlink spec:/c/mqueue/req/unlink
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CMqueueReqUnlink_Pre_Name_Valid,
  CMqueueReqUnlink_Pre_Name_Null,
  CMqueueReqUnlink_Pre_Name_TooLong,
  CMqueueReqUnlink_Pre_Name_NA
} CMqueueReqUnlink_Pre_Name;

typedef enum {
  CMqueueReqUnlink_Pre_Exists_Yes,
  CMqueueReqUnlink_Pre_Exists_No,
  CMqueueReqUnlink_Pre_Exists_NA
} CMqueueReqUnlink_Pre_Exists;

typedef enum {
  CMqueueReqUnlink_Pre_Open_Yes,
  CMqueueReqUnlink_Pre_Open_No,
  CMqueueReqUnlink_Pre_Open_NA
} CMqueueReqUnlink_Pre_Open;

typedef enum {
  CMqueueReqUnlink_Post_Status_Zero,
  CMqueueReqUnlink_Post_Status_MinusOne,
  CMqueueReqUnlink_Post_Status_NA
} CMqueueReqUnlink_Post_Status;

typedef enum {
  CMqueueReqUnlink_Post_Errno_Nop,
  CMqueueReqUnlink_Post_Errno_Inval,
  CMqueueReqUnlink_Post_Errno_NameTooLong,
  CMqueueReqUnlink_Post_Errno_NoEnt,
  CMqueueReqUnlink_Post_Errno_NA
} CMqueueReqUnlink_Post_Errno;

typedef enum {
  CMqueueReqUnlink_Post_Name_Removed,
  CMqueueReqUnlink_Post_Name_Nop,
  CMqueueReqUnlink_Post_Name_NA
} CMqueueReqUnlink_Post_Name;

typedef enum {
  CMqueueReqUnlink_Post_Queue_Deleted,
  CMqueueReqUnlink_Post_Queue_Nop,
  CMqueueReqUnlink_Post_Queue_NA
} CMqueueReqUnlink_Post_Queue;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Exists_NA : 1;
  uint16_t Pre_Open_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 3;
  uint16_t Post_Name : 2;
  uint16_t Post_Queue : 2;
} CMqueueReqUnlink_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/unlink test case.
 */
typedef struct {
  /**
   * @brief This member contains the message queue descriptor of the message
   *   queue created for the test.
   */
  mqd_t queue_id;

  /**
   * @brief This member references the control block of the message queue
   *   created for the test.
   */
  POSIX_Message_queue_Control *queue;

  /**
   * @brief This member contains the message queue descriptor of the other
   *   message queue.
   */
  mqd_t other_id;

  /**
   * @brief This member references the control block of the other message
   *   queue.
   */
  POSIX_Message_queue_Control *other;

  /**
   * @brief This member contains the number of inactive message queue objects
   *   before the mq_unlink() call.
   */
  size_t inactive_count;

  /**
   * @brief This member contains a name which is longer than the maximum length
   *   of a message queue name.
   */
  char long_name[ _POSIX_PATH_MAX + 2 ];

  /**
   * @brief This member specifies the `name` parameter value.
   */
  const char *name;

  /**
   * @brief This member contains the return value of the mq_unlink() call.
   */
  int status;

  /**
   * @brief This member contains the value of errno after the mq_unlink() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 3 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    CMqueueReqUnlink_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqUnlink_Context;

static CMqueueReqUnlink_Context CMqueueReqUnlink_Instance;

static const char *const CMqueueReqUnlink_PreDesc_Name[] =
  { "Valid", "Null", "TooLong", "NA" };

static const char *const CMqueueReqUnlink_PreDesc_Exists[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqUnlink_PreDesc_Open[] =
  { "Yes", "No", "NA" };

static const char *const *const CMqueueReqUnlink_PreDesc[] = {
  CMqueueReqUnlink_PreDesc_Name,
  CMqueueReqUnlink_PreDesc_Exists,
  CMqueueReqUnlink_PreDesc_Open,
  NULL
};

#define NAME "/mq-unlink"

#define OTHER_NAME "/mq-unlink-other"

#define ERRNO_POISON -1

static POSIX_Message_queue_Control *GetQueue( mqd_t mqdes )
{
  return (POSIX_Message_queue_Control *) _Objects_Get_no_protection(
    (Objects_Id) mqdes,
    &_POSIX_Message_queue_Information
  );
}

static POSIX_Message_queue_Control *GetQueueByName( const char *name )
{
  POSIX_Message_queue_Control *queue;
  Objects_Get_by_name_error    error;

  _Objects_Allocator_lock();
  queue = _POSIX_Message_queue_Get_by_name( name, NULL, &error );
  _Objects_Allocator_unlock();

  return queue;
}

static size_t GetInactiveCount( void )
{
  const Chain_Control *chain;
  const Chain_Node    *node;
  size_t               count;

  chain = &_POSIX_Message_queue_Information.Inactive;
  node = _Chain_Immutable_first( chain );
  count = 0;

  while ( node != _Chain_Immutable_tail( chain ) ) {
    ++count;
    node = _Chain_Immutable_next( node );
  }

  return count;
}

static void CMqueueReqUnlink_Pre_Name_Prepare(
  CMqueueReqUnlink_Context *ctx,
  CMqueueReqUnlink_Pre_Name state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Pre_Name_Valid: {
      /*
       * While the `name` parameter references a string which is not longer
       * than the maximum length of a message queue name.
       */
      ctx->name = NAME;
      break;
    }

    case CMqueueReqUnlink_Pre_Name_Null: {
      /*
       * While the `name` parameter is NULL.
       */
      ctx->name = NULL;
      break;
    }

    case CMqueueReqUnlink_Pre_Name_TooLong: {
      /*
       * While the `name` parameter references a string which is longer than
       * the maximum length of a message queue name.
       */
      ctx->name = ctx->long_name;
      break;
    }

    case CMqueueReqUnlink_Pre_Name_NA:
      break;
  }
}

static void CMqueueReqUnlink_Pre_Exists_Prepare(
  CMqueueReqUnlink_Context   *ctx,
  CMqueueReqUnlink_Pre_Exists state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Pre_Exists_Yes: {
      /*
       * While a message queue has the name specified by the `name` parameter.
       */
      ctx->queue_id = mq_open( NAME, O_CREAT | O_EXCL | O_RDWR, 0, NULL );
      T_ne_u32( ctx->queue_id, (mqd_t) -1 );
      ctx->queue = GetQueue( ctx->queue_id );
      T_assert_not_null( ctx->queue );
      break;
    }

    case CMqueueReqUnlink_Pre_Exists_No: {
      /*
       * While no message queue has the name specified by the `name` parameter.
       */
      /* No message queue has the name */
      break;
    }

    case CMqueueReqUnlink_Pre_Exists_NA:
      break;
  }
}

static void CMqueueReqUnlink_Pre_Open_Prepare(
  CMqueueReqUnlink_Context *ctx,
  CMqueueReqUnlink_Pre_Open state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Pre_Open_Yes: {
      /*
       * While the message queue is open.
       */
      /* The message queue was opened by its creation */
      break;
    }

    case CMqueueReqUnlink_Pre_Open_No: {
      /*
       * While the message queue is not open.
       */
      T_eq_int( mq_close( ctx->queue_id ), 0 );
      break;
    }

    case CMqueueReqUnlink_Pre_Open_NA:
      break;
  }
}

static void CMqueueReqUnlink_Post_Status_Check(
  CMqueueReqUnlink_Context    *ctx,
  CMqueueReqUnlink_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Post_Status_Zero: {
      /*
       * The return value of mq_unlink() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CMqueueReqUnlink_Post_Status_MinusOne: {
      /*
       * The return value of mq_unlink() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CMqueueReqUnlink_Post_Status_NA:
      break;
  }
}

static void CMqueueReqUnlink_Post_Errno_Check(
  CMqueueReqUnlink_Context   *ctx,
  CMqueueReqUnlink_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified by the mq_unlink() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CMqueueReqUnlink_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CMqueueReqUnlink_Post_Errno_NameTooLong: {
      /*
       * The value of errno shall be ENAMETOOLONG.
       */
      T_eq_int( ctx->errno_value, ENAMETOOLONG );
      break;
    }

    case CMqueueReqUnlink_Post_Errno_NoEnt: {
      /*
       * The value of errno shall be ENOENT.
       */
      T_eq_int( ctx->errno_value, ENOENT );
      break;
    }

    case CMqueueReqUnlink_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqUnlink_Post_Name_Check(
  CMqueueReqUnlink_Context  *ctx,
  CMqueueReqUnlink_Post_Name state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Post_Name_Removed: {
      /*
       * The name specified by the `name` parameter shall be removed from the
       * message queue.
       */
      T_null( GetQueueByName( NAME ) );
      T_false( ctx->queue->linked );
      T_eq_ptr( GetQueueByName( OTHER_NAME ), ctx->other );
      break;
    }

    case CMqueueReqUnlink_Post_Name_Nop: {
      /*
       * No name of a message queue shall be removed by the mq_unlink() call.
       */
      T_eq_ptr( GetQueueByName( NAME ), ctx->queue );
      T_eq_ptr( GetQueueByName( OTHER_NAME ), ctx->other );
      break;
    }

    case CMqueueReqUnlink_Post_Name_NA:
      break;
  }
}

static void CMqueueReqUnlink_Post_Queue_Check(
  CMqueueReqUnlink_Context   *ctx,
  CMqueueReqUnlink_Post_Queue state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Post_Queue_Deleted: {
      /*
       * The message queue shall be deleted.
       */
      T_eq_sz( GetInactiveCount(), ctx->inactive_count + 1 );
      break;
    }

    case CMqueueReqUnlink_Post_Queue_Nop: {
      /*
       * No message queue shall be deleted by the mq_unlink() call.
       */
      T_eq_sz( GetInactiveCount(), ctx->inactive_count );
      break;
    }

    case CMqueueReqUnlink_Post_Queue_NA:
      break;
  }
}

static void CMqueueReqUnlink_Setup( CMqueueReqUnlink_Context *ctx )
{
  memset( ctx->long_name, 'a', sizeof( ctx->long_name ) - 1 );
  ctx->long_name[ 0 ] = '/';
  ctx->long_name[ sizeof( ctx->long_name ) - 1 ] = '\0';
  ctx->other_id = mq_open( OTHER_NAME, O_CREAT | O_EXCL | O_RDWR, 0, NULL );
  T_ne_u32( ctx->other_id, (mqd_t) -1 );
  ctx->other = GetQueue( ctx->other_id );
  T_assert_not_null( ctx->other );
}

static void CMqueueReqUnlink_Setup_Wrap( void *arg )
{
  CMqueueReqUnlink_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqUnlink_Setup( ctx );
}

static void CMqueueReqUnlink_Teardown( CMqueueReqUnlink_Context *ctx )
{
  T_eq_int( mq_unlink( OTHER_NAME ), 0 );
  T_eq_int( mq_close( ctx->other_id ), 0 );
}

static void CMqueueReqUnlink_Teardown_Wrap( void *arg )
{
  CMqueueReqUnlink_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqUnlink_Teardown( ctx );
}

static void CMqueueReqUnlink_Prepare( CMqueueReqUnlink_Context *ctx )
{
  ctx->queue = NULL;
}

static void CMqueueReqUnlink_Action( CMqueueReqUnlink_Context *ctx )
{
  ctx->inactive_count = GetInactiveCount();
  errno = ERRNO_POISON;
  ctx->status = mq_unlink( ctx->name );
  ctx->errno_value = errno;
}

static void CMqueueReqUnlink_Cleanup( CMqueueReqUnlink_Context *ctx )
{
  if ( ctx->queue != NULL ) {
    if ( ctx->queue->linked ) {
      T_eq_int( mq_unlink( NAME ), 0 );
    }

    if ( ctx->queue->open_count > 0 ) {
      T_eq_int( mq_close( ctx->queue_id ), 0 );
    }

    ctx->queue = NULL;
  }
}

/* clang-format off */

static const CMqueueReqUnlink_Entry
CMqueueReqUnlink_Entries[] = {
  { 1, 0, 0, 0, CMqueueReqUnlink_Post_Status_NA,
    CMqueueReqUnlink_Post_Errno_NA, CMqueueReqUnlink_Post_Name_NA,
    CMqueueReqUnlink_Post_Queue_NA },
  { 0, 0, 0, 0, CMqueueReqUnlink_Post_Status_Zero,
    CMqueueReqUnlink_Post_Errno_Nop, CMqueueReqUnlink_Post_Name_Removed,
    CMqueueReqUnlink_Post_Queue_Nop },
  { 0, 0, 0, 0, CMqueueReqUnlink_Post_Status_Zero,
    CMqueueReqUnlink_Post_Errno_Nop, CMqueueReqUnlink_Post_Name_Removed,
    CMqueueReqUnlink_Post_Queue_Deleted },
  { 0, 0, 0, 1, CMqueueReqUnlink_Post_Status_MinusOne,
    CMqueueReqUnlink_Post_Errno_NoEnt, CMqueueReqUnlink_Post_Name_Nop,
    CMqueueReqUnlink_Post_Queue_Nop },
  { 0, 0, 1, 1, CMqueueReqUnlink_Post_Status_MinusOne,
    CMqueueReqUnlink_Post_Errno_Inval, CMqueueReqUnlink_Post_Name_Nop,
    CMqueueReqUnlink_Post_Queue_Nop },
  { 0, 0, 1, 1, CMqueueReqUnlink_Post_Status_MinusOne,
    CMqueueReqUnlink_Post_Errno_NameTooLong, CMqueueReqUnlink_Post_Name_Nop,
    CMqueueReqUnlink_Post_Queue_Nop }
};

static const uint8_t
CMqueueReqUnlink_Map[] = {
  1, 2, 3, 0, 4, 0, 0, 0, 5, 0, 0, 0
};

/* clang-format on */

static size_t CMqueueReqUnlink_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqUnlink_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqUnlink_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqUnlink_Fixture = {
  .setup = CMqueueReqUnlink_Setup_Wrap,
  .stop = NULL,
  .teardown = CMqueueReqUnlink_Teardown_Wrap,
  .scope = CMqueueReqUnlink_Scope,
  .initial_context = &CMqueueReqUnlink_Instance
};

static inline CMqueueReqUnlink_Entry CMqueueReqUnlink_PopEntry(
  CMqueueReqUnlink_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqUnlink_Entries[ CMqueueReqUnlink_Map[ index ] ];
}

static void CMqueueReqUnlink_SetPreConditionStates(
  CMqueueReqUnlink_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Exists_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqUnlink_Pre_Exists_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 2 ] = CMqueueReqUnlink_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CMqueueReqUnlink_TestVariant( CMqueueReqUnlink_Context *ctx )
{
  CMqueueReqUnlink_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqUnlink_Pre_Exists_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqUnlink_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqUnlink_Action( ctx );
  CMqueueReqUnlink_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqUnlink_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqUnlink_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  CMqueueReqUnlink_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
}

/**
 * @fn void T_case_body_CMqueueReqUnlink( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqUnlink, &CMqueueReqUnlink_Fixture )
{
  CMqueueReqUnlink_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqUnlink_Pre_Name_Valid;
    ctx->Map.pci[ 0 ] < CMqueueReqUnlink_Pre_Name_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqUnlink_Pre_Exists_Yes;
      ctx->Map.pci[ 1 ] < CMqueueReqUnlink_Pre_Exists_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqUnlink_Pre_Open_Yes;
        ctx->Map.pci[ 2 ] < CMqueueReqUnlink_Pre_Open_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CMqueueReqUnlink_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CMqueueReqUnlink_SetPreConditionStates( ctx );
        CMqueueReqUnlink_Prepare( ctx );
        CMqueueReqUnlink_TestVariant( ctx );
        CMqueueReqUnlink_Cleanup( ctx );
      }
    }
  }
}

/** @} */
