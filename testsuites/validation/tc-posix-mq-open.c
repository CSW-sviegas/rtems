/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqOpen
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
#include <rtems.h>
#include <string.h>
#include <rtems/posix/mqueueimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/wkspace.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqOpen spec:/c/mqueue/req/open
 *
 * @ingroup TestsuitesValidationMqueue
 *
 * @{
 */

typedef enum {
  CMqueueReqOpen_Pre_Name_Valid,
  CMqueueReqOpen_Pre_Name_Max,
  CMqueueReqOpen_Pre_Name_Null,
  CMqueueReqOpen_Pre_Name_TooLong,
  CMqueueReqOpen_Pre_Name_NA
} CMqueueReqOpen_Pre_Name;

typedef enum {
  CMqueueReqOpen_Pre_Access_ReadOnly,
  CMqueueReqOpen_Pre_Access_WriteOnly,
  CMqueueReqOpen_Pre_Access_ReadWrite,
  CMqueueReqOpen_Pre_Access_NA
} CMqueueReqOpen_Pre_Access;

typedef enum {
  CMqueueReqOpen_Pre_NonBlock_Yes,
  CMqueueReqOpen_Pre_NonBlock_No,
  CMqueueReqOpen_Pre_NonBlock_NA
} CMqueueReqOpen_Pre_NonBlock;

typedef enum {
  CMqueueReqOpen_Pre_Creat_Yes,
  CMqueueReqOpen_Pre_Creat_No,
  CMqueueReqOpen_Pre_Creat_NA
} CMqueueReqOpen_Pre_Creat;

typedef enum {
  CMqueueReqOpen_Pre_Excl_Yes,
  CMqueueReqOpen_Pre_Excl_No,
  CMqueueReqOpen_Pre_Excl_NA
} CMqueueReqOpen_Pre_Excl;

typedef enum {
  CMqueueReqOpen_Pre_Attr_Obj,
  CMqueueReqOpen_Pre_Attr_Null,
  CMqueueReqOpen_Pre_Attr_NA
} CMqueueReqOpen_Pre_Attr;

typedef enum {
  CMqueueReqOpen_Pre_MaxMsg_Positive,
  CMqueueReqOpen_Pre_MaxMsg_NotPositive,
  CMqueueReqOpen_Pre_MaxMsg_Huge,
  CMqueueReqOpen_Pre_MaxMsg_NA
} CMqueueReqOpen_Pre_MaxMsg;

typedef enum {
  CMqueueReqOpen_Pre_MsgSize_Positive,
  CMqueueReqOpen_Pre_MsgSize_NotPositive,
  CMqueueReqOpen_Pre_MsgSize_Huge,
  CMqueueReqOpen_Pre_MsgSize_NA
} CMqueueReqOpen_Pre_MsgSize;

typedef enum {
  CMqueueReqOpen_Pre_Exists_Yes,
  CMqueueReqOpen_Pre_Exists_No,
  CMqueueReqOpen_Pre_Exists_NA
} CMqueueReqOpen_Pre_Exists;

typedef enum {
  CMqueueReqOpen_Pre_Free_Yes,
  CMqueueReqOpen_Pre_Free_No,
  CMqueueReqOpen_Pre_Free_NA
} CMqueueReqOpen_Pre_Free;

typedef enum {
  CMqueueReqOpen_Pre_Memory_Yes,
  CMqueueReqOpen_Pre_Memory_No,
  CMqueueReqOpen_Pre_Memory_NA
} CMqueueReqOpen_Pre_Memory;

typedef enum {
  CMqueueReqOpen_Post_Status_MqDes,
  CMqueueReqOpen_Post_Status_MinusOne,
  CMqueueReqOpen_Post_Status_NA
} CMqueueReqOpen_Post_Status;

typedef enum {
  CMqueueReqOpen_Post_Errno_Nop,
  CMqueueReqOpen_Post_Errno_Inval,
  CMqueueReqOpen_Post_Errno_NameTooLong,
  CMqueueReqOpen_Post_Errno_NoEnt,
  CMqueueReqOpen_Post_Errno_Exist,
  CMqueueReqOpen_Post_Errno_NFile,
  CMqueueReqOpen_Post_Errno_NoMem,
  CMqueueReqOpen_Post_Errno_NoSpc,
  CMqueueReqOpen_Post_Errno_NA
} CMqueueReqOpen_Post_Errno;

typedef enum {
  CMqueueReqOpen_Post_Queue_Created,
  CMqueueReqOpen_Post_Queue_Nop,
  CMqueueReqOpen_Post_Queue_NA
} CMqueueReqOpen_Post_Queue;

typedef enum {
  CMqueueReqOpen_Post_Name_Valid,
  CMqueueReqOpen_Post_Name_Invalid,
  CMqueueReqOpen_Post_Name_NA
} CMqueueReqOpen_Post_Name;

typedef enum {
  CMqueueReqOpen_Post_Access_ReadOnly,
  CMqueueReqOpen_Post_Access_WriteOnly,
  CMqueueReqOpen_Post_Access_ReadWrite,
  CMqueueReqOpen_Post_Access_Nop,
  CMqueueReqOpen_Post_Access_NA
} CMqueueReqOpen_Post_Access;

typedef enum {
  CMqueueReqOpen_Post_NonBlock_Yes,
  CMqueueReqOpen_Post_NonBlock_No,
  CMqueueReqOpen_Post_NonBlock_Nop,
  CMqueueReqOpen_Post_NonBlock_NA
} CMqueueReqOpen_Post_NonBlock;

typedef enum {
  CMqueueReqOpen_Post_MaxMsg_Default,
  CMqueueReqOpen_Post_MaxMsg_Positive,
  CMqueueReqOpen_Post_MaxMsg_Nop,
  CMqueueReqOpen_Post_MaxMsg_NA
} CMqueueReqOpen_Post_MaxMsg;

typedef enum {
  CMqueueReqOpen_Post_MsgSize_Default,
  CMqueueReqOpen_Post_MsgSize_Positive,
  CMqueueReqOpen_Post_MsgSize_Nop,
  CMqueueReqOpen_Post_MsgSize_NA
} CMqueueReqOpen_Post_MsgSize;

typedef enum {
  CMqueueReqOpen_Post_Open_Once,
  CMqueueReqOpen_Post_Open_Twice,
  CMqueueReqOpen_Post_Open_NA
} CMqueueReqOpen_Post_Open;

typedef struct {
  uint64_t Skip : 1;
  uint64_t Pre_Name_NA : 1;
  uint64_t Pre_Access_NA : 1;
  uint64_t Pre_NonBlock_NA : 1;
  uint64_t Pre_Creat_NA : 1;
  uint64_t Pre_Excl_NA : 1;
  uint64_t Pre_Attr_NA : 1;
  uint64_t Pre_MaxMsg_NA : 1;
  uint64_t Pre_MsgSize_NA : 1;
  uint64_t Pre_Exists_NA : 1;
  uint64_t Pre_Free_NA : 1;
  uint64_t Pre_Memory_NA : 1;
  uint64_t Post_Status : 2;
  uint64_t Post_Errno : 4;
  uint64_t Post_Queue : 2;
  uint64_t Post_Name : 2;
  uint64_t Post_Access : 3;
  uint64_t Post_NonBlock : 2;
  uint64_t Post_MaxMsg : 2;
  uint64_t Post_MsgSize : 2;
  uint64_t Post_Open : 2;
} CMqueueReqOpen_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/open test case.
 */
typedef struct {
  /**
   * @brief This member contains a name which is longer than the maximum length
   *   of a message queue name.
   */
  char long_name[ _POSIX_PATH_MAX + 2 ];

  /**
   * @brief This member contains a name which has the maximum length of a
   *   message queue name.
   */
  char max_name[ _POSIX_PATH_MAX + 1 ];

  /**
   * @brief This member contains the message queue descriptor of the existing
   *   message queue.
   */
  mqd_t existing_id;

  /**
   * @brief This member references the control block of the existing message
   *   queue.
   */
  POSIX_Message_queue_Control *existing;

  /**
   * @brief This member provides the object referenced by the message queue
   *   attributes argument.
   */
  struct mq_attr attr_obj;

  /**
   * @brief This member contains the seized message queue objects.
   */
  void *seized_objects;

  /**
   * @brief This member contains the number of seized message queue objects.
   */
  unsigned int seized_count;

  /**
   * @brief This member contains the greedy workspace allocation.
   */
  Heap_Block *greedy;

  /**
   * @brief This member contains the number of inactive message queue objects
   *   before the mq_open() call.
   */
  size_t inactive_count;

  /**
   * @brief This member specifies the `name` parameter value.
   */
  const char *name;

  /**
   * @brief This member specifies the `oflag` parameter value.
   */
  int oflag;

  /**
   * @brief This member specifies the message queue attributes argument value.
   */
  struct mq_attr *attr;

  /**
   * @brief This member contains the return value of the mq_open() call.
   */
  mqd_t status;

  /**
   * @brief This member contains the value of errno after the mq_open() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 11 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 11 ];

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
    CMqueueReqOpen_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqOpen_Context;

static CMqueueReqOpen_Context CMqueueReqOpen_Instance;

static const char *const CMqueueReqOpen_PreDesc_Name[] =
  { "Valid", "Max", "Null", "TooLong", "NA" };

static const char *const CMqueueReqOpen_PreDesc_Access[] =
  { "ReadOnly", "WriteOnly", "ReadWrite", "NA" };

static const char *const CMqueueReqOpen_PreDesc_NonBlock[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqOpen_PreDesc_Creat[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqOpen_PreDesc_Excl[] = { "Yes", "No", "NA" };

static const char *const CMqueueReqOpen_PreDesc_Attr[] =
  { "Obj", "Null", "NA" };

static const char *const CMqueueReqOpen_PreDesc_MaxMsg[] =
  { "Positive", "NotPositive", "Huge", "NA" };

static const char *const CMqueueReqOpen_PreDesc_MsgSize[] =
  { "Positive", "NotPositive", "Huge", "NA" };

static const char *const CMqueueReqOpen_PreDesc_Exists[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqOpen_PreDesc_Free[] = { "Yes", "No", "NA" };

static const char *const CMqueueReqOpen_PreDesc_Memory[] =
  { "Yes", "No", "NA" };

static const char *const *const CMqueueReqOpen_PreDesc[] = {
  CMqueueReqOpen_PreDesc_Name,
  CMqueueReqOpen_PreDesc_Access,
  CMqueueReqOpen_PreDesc_NonBlock,
  CMqueueReqOpen_PreDesc_Creat,
  CMqueueReqOpen_PreDesc_Excl,
  CMqueueReqOpen_PreDesc_Attr,
  CMqueueReqOpen_PreDesc_MaxMsg,
  CMqueueReqOpen_PreDesc_MsgSize,
  CMqueueReqOpen_PreDesc_Exists,
  CMqueueReqOpen_PreDesc_Free,
  CMqueueReqOpen_PreDesc_Memory,
  NULL
};

typedef CMqueueReqOpen_Context Context;

#define NAME "/mq-open"

#define ERRNO_POISON -1

#define MQ_OPEN_FAILED ( ( mqd_t ) - 1 )

#define FLAGS_EXISTING ( O_CREAT | O_EXCL | O_WRONLY )

#define MAX_MSG 3

#define MSG_SIZE 8

#define EXISTING_MAX_MSG 2

#define EXISTING_MSG_SIZE 4

static const struct mq_attr existing_attr = {
  .mq_maxmsg = EXISTING_MAX_MSG,
  .mq_msgsize = EXISTING_MSG_SIZE
};

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

static void Unlink( const char *name )
{
  if ( GetQueueByName( name ) != NULL ) {
    T_eq_int( mq_unlink( name ), 0 );
  }
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

static Heap_Block *GreedyAllocate( void )
{
  Heap_Block *blocks;

  _RTEMS_Lock_allocator();
  blocks = _Heap_Greedy_allocate( &_Workspace_Area, NULL, 0 );
  _RTEMS_Unlock_allocator();

  return blocks;
}

static void GreedyFree( Heap_Block *blocks )
{
  _RTEMS_Lock_allocator();
  _Heap_Greedy_free( &_Workspace_Area, blocks );
  _RTEMS_Unlock_allocator();
}

static void SeizedName( char name[ 14 ], unsigned int index )
{
  (void) memcpy( name, "/mq-seized-00", 14 );
  name[ 11 ] = (char) ( '0' + ( index / 10U ) % 10U );
  name[ 12 ] = (char) ( '0' + index % 10U );
}

static rtems_status_code Create( void *arg, uint32_t *id )
{
  Context *ctx;
  char     name[ 14 ];
  mqd_t    mqdes;

  ctx = arg;
  SeizedName( name, ctx->seized_count );
  mqdes = mq_open( name, O_CREAT | O_EXCL | O_RDWR, 0, &existing_attr );

  if ( mqdes == MQ_OPEN_FAILED ) {
    T_quiet_eq_int( errno, ENFILE );
    return RTEMS_TOO_MANY;
  }

  ++ctx->seized_count;
  *id = (uint32_t) mqdes;
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code Delete( uint32_t id )
{
  POSIX_Message_queue_Control *queue;
  char                         name[ 32 ];

  queue = GetQueue( (mqd_t) id );
  T_quiet_assert_not_null( queue );
  T_quiet_assert_not_null( queue->Object.name.name_p );
  (void) strlcpy( name, queue->Object.name.name_p, sizeof( name ) );
  T_quiet_eq_int( mq_unlink( name ), 0 );
  T_quiet_eq_int( mq_close( (mqd_t) id ), 0 );
  return RTEMS_SUCCESSFUL;
}

static void CMqueueReqOpen_Pre_Name_Prepare(
  CMqueueReqOpen_Context *ctx,
  CMqueueReqOpen_Pre_Name state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Name_Valid: {
      /*
       * While the `name` parameter references a string which is shorter than
       * the maximum length of a message queue name.
       */
      ctx->name = NAME;
      break;
    }

    case CMqueueReqOpen_Pre_Name_Max: {
      /*
       * While the `name` parameter references a string which has the maximum
       * length of a message queue name.
       */
      ctx->name = ctx->max_name;
      break;
    }

    case CMqueueReqOpen_Pre_Name_Null: {
      /*
       * While the `name` parameter is NULL.
       */
      ctx->name = NULL;
      break;
    }

    case CMqueueReqOpen_Pre_Name_TooLong: {
      /*
       * While the `name` parameter references a string which is longer than
       * the maximum length of a message queue name.
       */
      ctx->name = ctx->long_name;
      break;
    }

    case CMqueueReqOpen_Pre_Name_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Access_Prepare(
  CMqueueReqOpen_Context   *ctx,
  CMqueueReqOpen_Pre_Access state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Access_ReadOnly: {
      /*
       * While the `oflag` parameter specifies the access mode for reading
       * only.
       */
      ctx->oflag = O_RDONLY;
      break;
    }

    case CMqueueReqOpen_Pre_Access_WriteOnly: {
      /*
       * While the `oflag` parameter specifies the access mode for writing
       * only.
       */
      ctx->oflag = O_WRONLY;
      break;
    }

    case CMqueueReqOpen_Pre_Access_ReadWrite: {
      /*
       * While the `oflag` parameter specifies the access mode for reading and
       * writing.
       */
      ctx->oflag = O_RDWR;
      break;
    }

    case CMqueueReqOpen_Pre_Access_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_NonBlock_Prepare(
  CMqueueReqOpen_Context     *ctx,
  CMqueueReqOpen_Pre_NonBlock state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_NonBlock_Yes: {
      /*
       * While the `oflag` parameter contains O_NONBLOCK.
       */
      ctx->oflag |= O_NONBLOCK;
      break;
    }

    case CMqueueReqOpen_Pre_NonBlock_No: {
      /*
       * While the `oflag` parameter does not contain O_NONBLOCK.
       */
      /* O_NONBLOCK is not set */
      break;
    }

    case CMqueueReqOpen_Pre_NonBlock_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Creat_Prepare(
  CMqueueReqOpen_Context  *ctx,
  CMqueueReqOpen_Pre_Creat state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Creat_Yes: {
      /*
       * While the `oflag` parameter contains O_CREAT.
       */
      ctx->oflag |= O_CREAT;
      break;
    }

    case CMqueueReqOpen_Pre_Creat_No: {
      /*
       * While the `oflag` parameter does not contain O_CREAT.
       */
      /* O_CREAT is not set */
      break;
    }

    case CMqueueReqOpen_Pre_Creat_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Excl_Prepare(
  CMqueueReqOpen_Context *ctx,
  CMqueueReqOpen_Pre_Excl state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Excl_Yes: {
      /*
       * While the `oflag` parameter contains O_EXCL.
       */
      ctx->oflag |= O_EXCL;
      break;
    }

    case CMqueueReqOpen_Pre_Excl_No: {
      /*
       * While the `oflag` parameter does not contain O_EXCL.
       */
      /* O_EXCL is not set */
      break;
    }

    case CMqueueReqOpen_Pre_Excl_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Attr_Prepare(
  CMqueueReqOpen_Context *ctx,
  CMqueueReqOpen_Pre_Attr state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Attr_Obj: {
      /*
       * While the message queue attributes argument references an object of
       * type struct mq_attr.
       */
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CMqueueReqOpen_Pre_Attr_Null: {
      /*
       * While the message queue attributes argument is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CMqueueReqOpen_Pre_Attr_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_MaxMsg_Prepare(
  CMqueueReqOpen_Context   *ctx,
  CMqueueReqOpen_Pre_MaxMsg state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_MaxMsg_Positive: {
      /*
       * While the mq_maxmsg member of the message queue attributes argument is
       * positive and less than the maximum value of type int.
       */
      ctx->attr_obj.mq_maxmsg = MAX_MSG;
      break;
    }

    case CMqueueReqOpen_Pre_MaxMsg_NotPositive: {
      /*
       * While the mq_maxmsg member of the message queue attributes argument is
       * not positive.
       */
      ctx->attr_obj.mq_maxmsg = 0;
      break;
    }

    case CMqueueReqOpen_Pre_MaxMsg_Huge: {
      /*
       * While the mq_maxmsg member of the message queue attributes argument is
       * the maximum value of type int.
       */
      ctx->attr_obj.mq_maxmsg = INT_MAX;
      break;
    }

    case CMqueueReqOpen_Pre_MaxMsg_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_MsgSize_Prepare(
  CMqueueReqOpen_Context    *ctx,
  CMqueueReqOpen_Pre_MsgSize state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_MsgSize_Positive: {
      /*
       * While the mq_msgsize member of the message queue attributes argument
       * is positive and less than the maximum value of type int.
       */
      ctx->attr_obj.mq_msgsize = MSG_SIZE;
      break;
    }

    case CMqueueReqOpen_Pre_MsgSize_NotPositive: {
      /*
       * While the mq_msgsize member of the message queue attributes argument
       * is not positive.
       */
      ctx->attr_obj.mq_msgsize = 0;
      break;
    }

    case CMqueueReqOpen_Pre_MsgSize_Huge: {
      /*
       * While the mq_msgsize member of the message queue attributes argument
       * is the maximum value of type int.
       */
      ctx->attr_obj.mq_msgsize = INT_MAX;
      break;
    }

    case CMqueueReqOpen_Pre_MsgSize_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Exists_Prepare(
  CMqueueReqOpen_Context   *ctx,
  CMqueueReqOpen_Pre_Exists state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Exists_Yes: {
      /*
       * While a message queue has the name specified by the `name` parameter.
       */
      ctx->existing_id = mq_open(
        ctx->name,
        FLAGS_EXISTING,
        0,
        &existing_attr
      );
      T_ne_u32( ctx->existing_id, MQ_OPEN_FAILED );
      ctx->existing = GetQueue( ctx->existing_id );
      T_assert_not_null( ctx->existing );
      break;
    }

    case CMqueueReqOpen_Pre_Exists_No: {
      /*
       * While no message queue has the name specified by the `name` parameter.
       */
      /* No message queue has the name */
      break;
    }

    case CMqueueReqOpen_Pre_Exists_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Free_Prepare(
  CMqueueReqOpen_Context *ctx,
  CMqueueReqOpen_Pre_Free state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Free_Yes: {
      /*
       * While an inactive message queue object is available.
       */
      /* The test suite configuration provides an inactive object */
      break;
    }

    case CMqueueReqOpen_Pre_Free_No: {
      /*
       * While no inactive message queue object is available.
       */
      ctx->seized_objects = T_seize_objects( Create, ctx );
      break;
    }

    case CMqueueReqOpen_Pre_Free_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Memory_Prepare(
  CMqueueReqOpen_Context   *ctx,
  CMqueueReqOpen_Pre_Memory state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Memory_Yes: {
      /*
       * While the workspace can store the name and the message buffers of the
       * new message queue.
       */
      /* The test suite configuration provides enough memory */
      break;
    }

    case CMqueueReqOpen_Pre_Memory_No: {
      /*
       * While the workspace cannot store the name of the new message queue.
       */
      ctx->greedy = GreedyAllocate();
      break;
    }

    case CMqueueReqOpen_Pre_Memory_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Status_Check(
  CMqueueReqOpen_Context    *ctx,
  CMqueueReqOpen_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_Status_MqDes: {
      /*
       * The return value of mq_open() shall be the message queue descriptor of
       * the message queue with the name specified by the `name` parameter.
       */
      T_ne_u32( ctx->status, MQ_OPEN_FAILED );
      T_not_null( GetQueueByName( ctx->name ) );
      T_eq_ptr( GetQueue( ctx->status ), GetQueueByName( ctx->name ) );
      break;
    }

    case CMqueueReqOpen_Post_Status_MinusOne: {
      /*
       * The return value of mq_open() shall be minus one.
       */
      T_eq_u32( ctx->status, MQ_OPEN_FAILED );
      break;
    }

    case CMqueueReqOpen_Post_Status_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Errno_Check(
  CMqueueReqOpen_Context   *ctx,
  CMqueueReqOpen_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified by the mq_open() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CMqueueReqOpen_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CMqueueReqOpen_Post_Errno_NameTooLong: {
      /*
       * The value of errno shall be ENAMETOOLONG.
       */
      T_eq_int( ctx->errno_value, ENAMETOOLONG );
      break;
    }

    case CMqueueReqOpen_Post_Errno_NoEnt: {
      /*
       * The value of errno shall be ENOENT.
       */
      T_eq_int( ctx->errno_value, ENOENT );
      break;
    }

    case CMqueueReqOpen_Post_Errno_Exist: {
      /*
       * The value of errno shall be EEXIST.
       */
      T_eq_int( ctx->errno_value, EEXIST );
      break;
    }

    case CMqueueReqOpen_Post_Errno_NFile: {
      /*
       * The value of errno shall be ENFILE.
       */
      T_eq_int( ctx->errno_value, ENFILE );
      break;
    }

    case CMqueueReqOpen_Post_Errno_NoMem: {
      /*
       * The value of errno shall be ENOMEM.
       */
      T_eq_int( ctx->errno_value, ENOMEM );
      break;
    }

    case CMqueueReqOpen_Post_Errno_NoSpc: {
      /*
       * The value of errno shall be ENOSPC.
       */
      T_eq_int( ctx->errno_value, ENOSPC );
      break;
    }

    case CMqueueReqOpen_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Queue_Check(
  CMqueueReqOpen_Context   *ctx,
  CMqueueReqOpen_Post_Queue state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_Queue_Created: {
      /*
       * The message queue shall be created.
       */
      T_eq_sz( GetInactiveCount() + 1, ctx->inactive_count );
      break;
    }

    case CMqueueReqOpen_Post_Queue_Nop: {
      /*
       * The mq_open() call shall create no message queue.
       */
      T_eq_sz( GetInactiveCount(), ctx->inactive_count );
      break;
    }

    case CMqueueReqOpen_Post_Queue_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Name_Check(
  CMqueueReqOpen_Context  *ctx,
  CMqueueReqOpen_Post_Name state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_Name_Valid: {
      /*
       * A message queue shall have the name specified by the `name` parameter.
       */
      T_not_null( GetQueueByName( ctx->name ) );
      break;
    }

    case CMqueueReqOpen_Post_Name_Invalid: {
      /*
       * The name specified by the `name` parameter shall be the name of no
       * message queue.
       */
      T_null( GetQueueByName( ctx->name ) );
      break;
    }

    case CMqueueReqOpen_Post_Name_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Access_Check(
  CMqueueReqOpen_Context    *ctx,
  CMqueueReqOpen_Post_Access state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_Access_ReadOnly: {
      /*
       * The flags of the message queue shall specify the access mode for
       * reading only.
       */
      T_eq_int( GetQueueByName( ctx->name )->oflag & O_ACCMODE, O_RDONLY );
      break;
    }

    case CMqueueReqOpen_Post_Access_WriteOnly: {
      /*
       * The flags of the message queue shall specify the access mode for
       * writing only.
       */
      T_eq_int( GetQueueByName( ctx->name )->oflag & O_ACCMODE, O_WRONLY );
      break;
    }

    case CMqueueReqOpen_Post_Access_ReadWrite: {
      /*
       * The flags of the message queue shall specify the access mode for
       * reading and writing.
       */
      T_eq_int( GetQueueByName( ctx->name )->oflag & O_ACCMODE, O_RDWR );
      break;
    }

    case CMqueueReqOpen_Post_Access_Nop: {
      /*
       * The flags of the message queue shall not be modified by the mq_open()
       * call.
       */
      T_eq_int( ctx->existing->oflag, FLAGS_EXISTING );
      break;
    }

    case CMqueueReqOpen_Post_Access_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_NonBlock_Check(
  CMqueueReqOpen_Context      *ctx,
  CMqueueReqOpen_Post_NonBlock state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_NonBlock_Yes: {
      /*
       * The flags of the message queue shall contain O_NONBLOCK.
       */
      T_eq_int( GetQueueByName( ctx->name )->oflag & O_NONBLOCK, O_NONBLOCK );
      break;
    }

    case CMqueueReqOpen_Post_NonBlock_No: {
      /*
       * The flags of the message queue shall not contain O_NONBLOCK.
       */
      T_eq_int( GetQueueByName( ctx->name )->oflag & O_NONBLOCK, 0 );
      break;
    }

    case CMqueueReqOpen_Post_NonBlock_Nop: {
      /*
       * The flags of the message queue shall not be modified by the mq_open()
       * call.
       */
      T_eq_int( ctx->existing->oflag, FLAGS_EXISTING );
      break;
    }

    case CMqueueReqOpen_Post_NonBlock_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_MaxMsg_Check(
  CMqueueReqOpen_Context    *ctx,
  CMqueueReqOpen_Post_MaxMsg state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_MaxMsg_Default: {
      /*
       * The maximum number of messages of the message queue shall be ten.
       */
      T_eq_u32(
        GetQueueByName( ctx->name )->Message_queue.maximum_pending_messages,
        10
      );
      break;
    }

    case CMqueueReqOpen_Post_MaxMsg_Positive: {
      /*
       * The maximum number of messages of the message queue shall be the value
       * of the mq_maxmsg member of the message queue attributes argument.
       */
      T_eq_u32(
        GetQueueByName( ctx->name )->Message_queue.maximum_pending_messages,
        MAX_MSG
      );
      break;
    }

    case CMqueueReqOpen_Post_MaxMsg_Nop: {
      /*
       * The maximum number of messages of the message queue shall not be
       * modified by the mq_open() call.
       */
      T_eq_u32(
        ctx->existing->Message_queue.maximum_pending_messages,
        EXISTING_MAX_MSG
      );
      break;
    }

    case CMqueueReqOpen_Post_MaxMsg_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_MsgSize_Check(
  CMqueueReqOpen_Context     *ctx,
  CMqueueReqOpen_Post_MsgSize state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_MsgSize_Default: {
      /*
       * The maximum message size of the message queue shall be 16.
       */
      T_eq_sz(
        GetQueueByName( ctx->name )->Message_queue.maximum_message_size,
        16
      );
      break;
    }

    case CMqueueReqOpen_Post_MsgSize_Positive: {
      /*
       * The maximum message size of the message queue shall be the value of
       * the mq_msgsize member of the message queue attributes argument.
       */
      T_eq_sz(
        GetQueueByName( ctx->name )->Message_queue.maximum_message_size,
        MSG_SIZE
      );
      break;
    }

    case CMqueueReqOpen_Post_MsgSize_Nop: {
      /*
       * The maximum message size of the message queue shall not be modified by
       * the mq_open() call.
       */
      T_eq_sz(
        ctx->existing->Message_queue.maximum_message_size,
        EXISTING_MSG_SIZE
      );
      break;
    }

    case CMqueueReqOpen_Post_MsgSize_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Open_Check(
  CMqueueReqOpen_Context  *ctx,
  CMqueueReqOpen_Post_Open state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_Open_Once: {
      /*
       * The message queue shall be open once.
       */
      T_eq_u32( GetQueueByName( ctx->name )->open_count, 1 );
      break;
    }

    case CMqueueReqOpen_Post_Open_Twice: {
      /*
       * The message queue shall be open twice.
       */
      T_eq_u32( GetQueueByName( ctx->name )->open_count, 2 );
      break;
    }

    case CMqueueReqOpen_Post_Open_NA:
      break;
  }
}

static void CMqueueReqOpen_Setup( CMqueueReqOpen_Context *ctx )
{
  memset( ctx->long_name, 'a', sizeof( ctx->long_name ) - 1 );
  ctx->long_name[ 0 ] = '/';
  ctx->long_name[ sizeof( ctx->long_name ) - 1 ] = '\0';
  memset( ctx->max_name, 'b', sizeof( ctx->max_name ) - 1 );
  ctx->max_name[ 0 ] = '/';
  ctx->max_name[ sizeof( ctx->max_name ) - 1 ] = '\0';
}

static void CMqueueReqOpen_Setup_Wrap( void *arg )
{
  CMqueueReqOpen_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqOpen_Setup( ctx );
}

static void CMqueueReqOpen_Prepare( CMqueueReqOpen_Context *ctx )
{
  ctx->existing = NULL;
  ctx->seized_objects = NULL;
  ctx->seized_count = 0;
  ctx->greedy = NULL;
  ctx->status = MQ_OPEN_FAILED;
  memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
}

static void CMqueueReqOpen_Action( CMqueueReqOpen_Context *ctx )
{
  ctx->inactive_count = GetInactiveCount();
  errno = ERRNO_POISON;
  ctx->status = mq_open( ctx->name, ctx->oflag, 0, ctx->attr );
  ctx->errno_value = errno;
}

static void CMqueueReqOpen_Cleanup( CMqueueReqOpen_Context *ctx )
{
  if ( ctx->greedy != NULL ) {
    GreedyFree( ctx->greedy );
    ctx->greedy = NULL;
  }

  T_surrender_objects( &ctx->seized_objects, Delete );

  if ( ctx->status != MQ_OPEN_FAILED ) {
    T_eq_int( mq_close( ctx->status ), 0 );
  }

  if ( ctx->existing != NULL ) {
    T_eq_int( mq_close( ctx->existing_id ), 0 );
    ctx->existing = NULL;
  }

  Unlink( NAME );
  Unlink( ctx->max_name );
}

/* clang-format off */

static const CMqueueReqOpen_Entry
CMqueueReqOpen_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_NA,
    CMqueueReqOpen_Post_Errno_NA, CMqueueReqOpen_Post_Queue_NA,
    CMqueueReqOpen_Post_Name_NA, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_Inval, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Invalid, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_NoSpc, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Invalid, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_NFile, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Invalid, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_NFile, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Invalid, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_NFile, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Invalid, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_Nop,
    CMqueueReqOpen_Post_NonBlock_Nop, CMqueueReqOpen_Post_MaxMsg_Nop,
    CMqueueReqOpen_Post_MsgSize_Nop, CMqueueReqOpen_Post_Open_Twice },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_NoMem, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Invalid, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_NoMem, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Invalid, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_NoEnt, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Invalid, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_Exist, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_Nop,
    CMqueueReqOpen_Post_NonBlock_Nop, CMqueueReqOpen_Post_MaxMsg_Nop,
    CMqueueReqOpen_Post_MsgSize_Nop, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_ReadOnly,
    CMqueueReqOpen_Post_NonBlock_Yes, CMqueueReqOpen_Post_MaxMsg_Positive,
    CMqueueReqOpen_Post_MsgSize_Positive, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_ReadOnly,
    CMqueueReqOpen_Post_NonBlock_Yes, CMqueueReqOpen_Post_MaxMsg_Default,
    CMqueueReqOpen_Post_MsgSize_Default, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_ReadOnly,
    CMqueueReqOpen_Post_NonBlock_No, CMqueueReqOpen_Post_MaxMsg_Positive,
    CMqueueReqOpen_Post_MsgSize_Positive, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_ReadOnly,
    CMqueueReqOpen_Post_NonBlock_No, CMqueueReqOpen_Post_MaxMsg_Default,
    CMqueueReqOpen_Post_MsgSize_Default, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_WriteOnly,
    CMqueueReqOpen_Post_NonBlock_Yes, CMqueueReqOpen_Post_MaxMsg_Positive,
    CMqueueReqOpen_Post_MsgSize_Positive, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_WriteOnly,
    CMqueueReqOpen_Post_NonBlock_Yes, CMqueueReqOpen_Post_MaxMsg_Default,
    CMqueueReqOpen_Post_MsgSize_Default, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_WriteOnly,
    CMqueueReqOpen_Post_NonBlock_No, CMqueueReqOpen_Post_MaxMsg_Positive,
    CMqueueReqOpen_Post_MsgSize_Positive, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_WriteOnly,
    CMqueueReqOpen_Post_NonBlock_No, CMqueueReqOpen_Post_MaxMsg_Default,
    CMqueueReqOpen_Post_MsgSize_Default, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_ReadWrite,
    CMqueueReqOpen_Post_NonBlock_Yes, CMqueueReqOpen_Post_MaxMsg_Positive,
    CMqueueReqOpen_Post_MsgSize_Positive, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_ReadWrite,
    CMqueueReqOpen_Post_NonBlock_Yes, CMqueueReqOpen_Post_MaxMsg_Default,
    CMqueueReqOpen_Post_MsgSize_Default, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_ReadWrite,
    CMqueueReqOpen_Post_NonBlock_No, CMqueueReqOpen_Post_MaxMsg_Positive,
    CMqueueReqOpen_Post_MsgSize_Positive, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MqDes,
    CMqueueReqOpen_Post_Errno_Nop, CMqueueReqOpen_Post_Queue_Created,
    CMqueueReqOpen_Post_Name_Valid, CMqueueReqOpen_Post_Access_ReadWrite,
    CMqueueReqOpen_Post_NonBlock_No, CMqueueReqOpen_Post_MaxMsg_Default,
    CMqueueReqOpen_Post_MsgSize_Default, CMqueueReqOpen_Post_Open_Once },
  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_Inval, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_NA, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA },
  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_NameTooLong, CMqueueReqOpen_Post_Queue_Nop,
    CMqueueReqOpen_Post_Name_NA, CMqueueReqOpen_Post_Access_NA,
    CMqueueReqOpen_Post_NonBlock_NA, CMqueueReqOpen_Post_MaxMsg_NA,
    CMqueueReqOpen_Post_MsgSize_NA, CMqueueReqOpen_Post_Open_NA }
};

static const uint8_t
CMqueueReqOpen_Map[] = {
  10, 0, 0, 0, 11, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0,
  0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  12, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 11, 7, 4, 4,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 12, 8, 5, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 13, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0,
  0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  2, 0, 3, 0, 0, 0, 0, 0, 14, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 0, 13, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 14,
  8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 15, 7, 4, 4,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 16, 8, 5, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 15, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0,
  3, 0, 0, 0, 0, 0, 16, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0,
  0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 0, 0, 0, 17, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0,
  0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  18, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 17, 7, 4, 4,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 18, 8, 5, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 19, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0,
  0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  2, 0, 3, 0, 0, 0, 0, 0, 20, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 0, 19, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 20,
  8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 21, 7, 4, 4,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 22, 8, 5, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 21, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0,
  3, 0, 0, 0, 0, 0, 22, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0,
  0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 0, 0, 0, 11, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0,
  0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  12, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 11, 7, 4, 4,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 12, 8, 5, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 13, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0,
  0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  2, 0, 3, 0, 0, 0, 0, 0, 14, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 0, 13, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 14,
  8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 15, 7, 4, 4,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 16, 8, 5, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 15, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0,
  3, 0, 0, 0, 0, 0, 16, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0,
  0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 0, 0, 0, 17, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0,
  0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  18, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 17, 7, 4, 4,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 18, 8, 5, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 19, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0,
  0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  2, 0, 3, 0, 0, 0, 0, 0, 20, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 0, 19, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 20,
  8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 21, 7, 4, 4,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 22, 8, 5, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 21, 7, 4, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
  0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0,
  3, 0, 0, 0, 0, 0, 22, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0,
  0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CMqueueReqOpen_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqOpen_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqOpen_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqOpen_Fixture = {
  .setup = CMqueueReqOpen_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = CMqueueReqOpen_Scope,
  .initial_context = &CMqueueReqOpen_Instance
};

static inline CMqueueReqOpen_Entry CMqueueReqOpen_PopEntry(
  CMqueueReqOpen_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqOpen_Entries[ CMqueueReqOpen_Map[ index ] ];
}

static void CMqueueReqOpen_SetPreConditionStates( CMqueueReqOpen_Context *ctx )
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Access_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqOpen_Pre_Access_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_NonBlock_NA ) {
    ctx->Map.pcs[ 2 ] = CMqueueReqOpen_Pre_NonBlock_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Creat_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqOpen_Pre_Creat_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Excl_NA ) {
    ctx->Map.pcs[ 4 ] = CMqueueReqOpen_Pre_Excl_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Attr_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqOpen_Pre_Attr_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_MaxMsg_NA ) {
    ctx->Map.pcs[ 6 ] = CMqueueReqOpen_Pre_MaxMsg_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_MsgSize_NA ) {
    ctx->Map.pcs[ 7 ] = CMqueueReqOpen_Pre_MsgSize_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_Exists_NA ) {
    ctx->Map.pcs[ 8 ] = CMqueueReqOpen_Pre_Exists_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }

  if ( ctx->Map.entry.Pre_Free_NA ) {
    ctx->Map.pcs[ 9 ] = CMqueueReqOpen_Pre_Free_NA;
  } else {
    ctx->Map.pcs[ 9 ] = ctx->Map.pci[ 9 ];
  }

  if ( ctx->Map.entry.Pre_Memory_NA ) {
    ctx->Map.pcs[ 10 ] = CMqueueReqOpen_Pre_Memory_NA;
  } else {
    ctx->Map.pcs[ 10 ] = ctx->Map.pci[ 10 ];
  }
}

static void CMqueueReqOpen_TestVariant( CMqueueReqOpen_Context *ctx )
{
  CMqueueReqOpen_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqOpen_Pre_Access_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqOpen_Pre_NonBlock_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqOpen_Pre_Creat_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqOpen_Pre_Excl_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqOpen_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqOpen_Pre_MaxMsg_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CMqueueReqOpen_Pre_MsgSize_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CMqueueReqOpen_Pre_Exists_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  CMqueueReqOpen_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  CMqueueReqOpen_Pre_Memory_Prepare( ctx, ctx->Map.pcs[ 10 ] );
  CMqueueReqOpen_Action( ctx );
  CMqueueReqOpen_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqOpen_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqOpen_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
  CMqueueReqOpen_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  CMqueueReqOpen_Post_Access_Check( ctx, ctx->Map.entry.Post_Access );
  CMqueueReqOpen_Post_NonBlock_Check( ctx, ctx->Map.entry.Post_NonBlock );
  CMqueueReqOpen_Post_MaxMsg_Check( ctx, ctx->Map.entry.Post_MaxMsg );
  CMqueueReqOpen_Post_MsgSize_Check( ctx, ctx->Map.entry.Post_MsgSize );
  CMqueueReqOpen_Post_Open_Check( ctx, ctx->Map.entry.Post_Open );
}

/**
 * @fn void T_case_body_CMqueueReqOpen( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqOpen, &CMqueueReqOpen_Fixture )
{
  CMqueueReqOpen_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqOpen_Pre_Name_Valid;
    ctx->Map.pci[ 0 ] < CMqueueReqOpen_Pre_Name_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqOpen_Pre_Access_ReadOnly;
      ctx->Map.pci[ 1 ] < CMqueueReqOpen_Pre_Access_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqOpen_Pre_NonBlock_Yes;
        ctx->Map.pci[ 2 ] < CMqueueReqOpen_Pre_NonBlock_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqOpen_Pre_Creat_Yes;
          ctx->Map.pci[ 3 ] < CMqueueReqOpen_Pre_Creat_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqOpen_Pre_Excl_Yes;
            ctx->Map.pci[ 4 ] < CMqueueReqOpen_Pre_Excl_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqOpen_Pre_Attr_Obj;
              ctx->Map.pci[ 5 ] < CMqueueReqOpen_Pre_Attr_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CMqueueReqOpen_Pre_MaxMsg_Positive;
                ctx->Map.pci[ 6 ] < CMqueueReqOpen_Pre_MaxMsg_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CMqueueReqOpen_Pre_MsgSize_Positive;
                  ctx->Map.pci[ 7 ] < CMqueueReqOpen_Pre_MsgSize_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = CMqueueReqOpen_Pre_Exists_Yes;
                    ctx->Map.pci[ 8 ] < CMqueueReqOpen_Pre_Exists_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    for (
                      ctx->Map.pci[ 9 ] = CMqueueReqOpen_Pre_Free_Yes;
                      ctx->Map.pci[ 9 ] < CMqueueReqOpen_Pre_Free_NA;
                      ++ctx->Map.pci[ 9 ]
                    ) {
                      for (
                        ctx->Map.pci[ 10 ] = CMqueueReqOpen_Pre_Memory_Yes;
                        ctx->Map.pci[ 10 ] < CMqueueReqOpen_Pre_Memory_NA;
                        ++ctx->Map.pci[ 10 ]
                      ) {
                        ctx->Map.entry = CMqueueReqOpen_PopEntry( ctx );

                        if ( ctx->Map.entry.Skip ) {
                          continue;
                        }

                        CMqueueReqOpen_SetPreConditionStates( ctx );
                        CMqueueReqOpen_Prepare( ctx );
                        CMqueueReqOpen_TestVariant( ctx );
                        CMqueueReqOpen_Cleanup( ctx );
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
