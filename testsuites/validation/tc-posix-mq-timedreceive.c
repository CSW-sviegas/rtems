/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqTimedreceive
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
#include <time.h>
#include <rtems/posix/mqueueimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqTimedreceive spec:/c/mqueue/req/timedreceive
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CMqueueReqTimedreceive_Pre_MsgPrio_Obj,
  CMqueueReqTimedreceive_Pre_MsgPrio_Null,
  CMqueueReqTimedreceive_Pre_MsgPrio_NA
} CMqueueReqTimedreceive_Pre_MsgPrio;

typedef enum {
  CMqueueReqTimedreceive_Pre_MsgLen_Max,
  CMqueueReqTimedreceive_Pre_MsgLen_Greater,
  CMqueueReqTimedreceive_Pre_MsgLen_TooSmall,
  CMqueueReqTimedreceive_Pre_MsgLen_NA
} CMqueueReqTimedreceive_Pre_MsgLen;

typedef enum {
  CMqueueReqTimedreceive_Pre_MqDes_Valid,
  CMqueueReqTimedreceive_Pre_MqDes_Invalid,
  CMqueueReqTimedreceive_Pre_MqDes_NA
} CMqueueReqTimedreceive_Pre_MqDes;

typedef enum {
  CMqueueReqTimedreceive_Pre_StoredPrio_Low,
  CMqueueReqTimedreceive_Pre_StoredPrio_High,
  CMqueueReqTimedreceive_Pre_StoredPrio_NA
} CMqueueReqTimedreceive_Pre_StoredPrio;

typedef enum {
  CMqueueReqTimedreceive_Pre_StoredSize_One,
  CMqueueReqTimedreceive_Pre_StoredSize_Max,
  CMqueueReqTimedreceive_Pre_StoredSize_NA
} CMqueueReqTimedreceive_Pre_StoredSize;

typedef enum {
  CMqueueReqTimedreceive_Pre_Queue_Empty,
  CMqueueReqTimedreceive_Pre_Queue_Pending,
  CMqueueReqTimedreceive_Pre_Queue_Sender,
  CMqueueReqTimedreceive_Pre_Queue_NA
} CMqueueReqTimedreceive_Pre_Queue;

typedef enum {
  CMqueueReqTimedreceive_Pre_Access_ReadWrite,
  CMqueueReqTimedreceive_Pre_Access_ReadOnly,
  CMqueueReqTimedreceive_Pre_Access_WriteOnly,
  CMqueueReqTimedreceive_Pre_Access_NA
} CMqueueReqTimedreceive_Pre_Access;

typedef enum {
  CMqueueReqTimedreceive_Pre_NonBlock_Yes,
  CMqueueReqTimedreceive_Pre_NonBlock_No,
  CMqueueReqTimedreceive_Pre_NonBlock_NA
} CMqueueReqTimedreceive_Pre_NonBlock;

typedef enum {
  CMqueueReqTimedreceive_Pre_Open_Yes,
  CMqueueReqTimedreceive_Pre_Open_No,
  CMqueueReqTimedreceive_Pre_Open_NA
} CMqueueReqTimedreceive_Pre_Open;

typedef enum {
  CMqueueReqTimedreceive_Pre_Abstime_Future,
  CMqueueReqTimedreceive_Pre_Abstime_PastOrNow,
  CMqueueReqTimedreceive_Pre_Abstime_InvalidNsec,
  CMqueueReqTimedreceive_Pre_Abstime_Null,
  CMqueueReqTimedreceive_Pre_Abstime_NA
} CMqueueReqTimedreceive_Pre_Abstime;

typedef enum {
  CMqueueReqTimedreceive_Pre_Send_Yes,
  CMqueueReqTimedreceive_Pre_Send_No,
  CMqueueReqTimedreceive_Pre_Send_NA
} CMqueueReqTimedreceive_Pre_Send;

typedef enum {
  CMqueueReqTimedreceive_Post_Status_One,
  CMqueueReqTimedreceive_Post_Status_Max,
  CMqueueReqTimedreceive_Post_Status_MinusOne,
  CMqueueReqTimedreceive_Post_Status_NA
} CMqueueReqTimedreceive_Post_Status;

typedef enum {
  CMqueueReqTimedreceive_Post_Errno_Nop,
  CMqueueReqTimedreceive_Post_Errno_Inval,
  CMqueueReqTimedreceive_Post_Errno_Badf,
  CMqueueReqTimedreceive_Post_Errno_MsgSize,
  CMqueueReqTimedreceive_Post_Errno_Again,
  CMqueueReqTimedreceive_Post_Errno_TimedOut,
  CMqueueReqTimedreceive_Post_Errno_NA
} CMqueueReqTimedreceive_Post_Errno;

typedef enum {
  CMqueueReqTimedreceive_Post_Buffer_One,
  CMqueueReqTimedreceive_Post_Buffer_Max,
  CMqueueReqTimedreceive_Post_Buffer_Nop,
  CMqueueReqTimedreceive_Post_Buffer_NA
} CMqueueReqTimedreceive_Post_Buffer;

typedef enum {
  CMqueueReqTimedreceive_Post_Priority_Low,
  CMqueueReqTimedreceive_Post_Priority_High,
  CMqueueReqTimedreceive_Post_Priority_Nop,
  CMqueueReqTimedreceive_Post_Priority_NA
} CMqueueReqTimedreceive_Post_Priority;

typedef enum {
  CMqueueReqTimedreceive_Post_Queue_Removed,
  CMqueueReqTimedreceive_Post_Queue_Nop,
  CMqueueReqTimedreceive_Post_Queue_NA
} CMqueueReqTimedreceive_Post_Queue;

typedef enum {
  CMqueueReqTimedreceive_Post_Sender_Moved,
  CMqueueReqTimedreceive_Post_Sender_Nop,
  CMqueueReqTimedreceive_Post_Sender_NA
} CMqueueReqTimedreceive_Post_Sender;

typedef enum {
  CMqueueReqTimedreceive_Post_Timer_Inactive,
  CMqueueReqTimedreceive_Post_Timer_Realtime,
  CMqueueReqTimedreceive_Post_Timer_NA
} CMqueueReqTimedreceive_Post_Timer;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_MsgPrio_NA : 1;
  uint32_t Pre_MsgLen_NA : 1;
  uint32_t Pre_MqDes_NA : 1;
  uint32_t Pre_StoredPrio_NA : 1;
  uint32_t Pre_StoredSize_NA : 1;
  uint32_t Pre_Queue_NA : 1;
  uint32_t Pre_Access_NA : 1;
  uint32_t Pre_NonBlock_NA : 1;
  uint32_t Pre_Open_NA : 1;
  uint32_t Pre_Abstime_NA : 1;
  uint32_t Pre_Send_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 3;
  uint32_t Post_Buffer : 2;
  uint32_t Post_Priority : 2;
  uint32_t Post_Queue : 2;
  uint32_t Post_Sender : 2;
  uint32_t Post_Timer : 2;
} CMqueueReqTimedreceive_Entry;

#define MSG_SIZE 4

typedef struct {
  volatile bool done;
} Sender;

/**
 * @brief Test context for spec:/c/mqueue/req/timedreceive test case.
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
   * @brief This member contains the flags of the message queue without
   *   O_NONBLOCK.
   */
  int oflag;

  /**
   * @brief This member contains the message priority of the message to
   *   receive.
   */
  unsigned int stored_prio;

  /**
   * @brief This member contains the message size of the message to receive.
   */
  size_t stored_size;

  /**
   * @brief This member contains the number of pending messages before the
   *   mq_timedreceive() call.
   */
  uint32_t pending_count;

  /**
   * @brief If this member is true, then the message queue was empty.
   */
  bool empty;

  /**
   * @brief This member contains the identifier of the task which calls
   *   mq_timedreceive().
   */
  rtems_id worker_id;

  /**
   * @brief If this member is true, then the mq_timedreceive() call returned.
   */
  volatile bool done;

  /**
   * @brief This member contains the identifier of the task which waits to send
   *   a message.
   */
  rtems_id sender_id;

  /**
   * @brief This member provides the state of the sending task.
   */
  Sender sender[ 1 ];

  /**
   * @brief If this member is true, then the sending task waited before the
   *   mq_timedreceive() call.
   */
  bool sender_waits;

  /**
   * @brief This member provides the buffer referenced by the `msg_ptr`
   *   parameter.
   */
  char buf[ MSG_SIZE + 1 ];

  /**
   * @brief This member provides the object referenced by the `msg_prio`
   *   parameter.
   */
  unsigned int prio_obj;

  /**
   * @brief This member specifies the `mqdes` parameter value.
   */
  mqd_t mqdes;

  /**
   * @brief This member specifies the `msg_len` parameter value.
   */
  size_t msg_len;

  /**
   * @brief This member specifies the `msg_prio` parameter value.
   */
  unsigned int *msg_prio;

  /**
   * @brief This member contains the return value of the mq_timedreceive()
   *   call.
   */
  ssize_t status;

  /**
   * @brief This member contains the value of errno after the mq_timedreceive()
   *   call.
   */
  int errno_value;

  /**
   * @brief This member specifies the `abs_timeout` parameter value.
   */
  const struct timespec *abstime;

  /**
   * @brief This member provides the object referenced by the `abs_timeout`
   *   parameter.
   */
  struct timespec abstime_obj;

  /**
   * @brief If this member is true, then the runner sends a message when the
   *   calling task waits.
   */
  bool send;

  /**
   * @brief This member contains the timer information of the calling task.
   */
  TaskTimerInfo timer_info;

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
    CMqueueReqTimedreceive_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqTimedreceive_Context;

static CMqueueReqTimedreceive_Context CMqueueReqTimedreceive_Instance;

static const char *const CMqueueReqTimedreceive_PreDesc_MsgPrio[] =
  { "Obj", "Null", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_MsgLen[] =
  { "Max", "Greater", "TooSmall", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_MqDes[] =
  { "Valid", "Invalid", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_StoredPrio[] =
  { "Low", "High", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_StoredSize[] =
  { "One", "Max", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_Queue[] =
  { "Empty", "Pending", "Sender", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_Access[] =
  { "ReadWrite", "ReadOnly", "WriteOnly", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_NonBlock[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_Open[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_Abstime[] =
  { "Future", "PastOrNow", "InvalidNsec", "Null", "NA" };

static const char *const CMqueueReqTimedreceive_PreDesc_Send[] =
  { "Yes", "No", "NA" };

static const char *const *const CMqueueReqTimedreceive_PreDesc[] = {
  CMqueueReqTimedreceive_PreDesc_MsgPrio,
  CMqueueReqTimedreceive_PreDesc_MsgLen,
  CMqueueReqTimedreceive_PreDesc_MqDes,
  CMqueueReqTimedreceive_PreDesc_StoredPrio,
  CMqueueReqTimedreceive_PreDesc_StoredSize,
  CMqueueReqTimedreceive_PreDesc_Queue,
  CMqueueReqTimedreceive_PreDesc_Access,
  CMqueueReqTimedreceive_PreDesc_NonBlock,
  CMqueueReqTimedreceive_PreDesc_Open,
  CMqueueReqTimedreceive_PreDesc_Abstime,
  CMqueueReqTimedreceive_PreDesc_Send,
  NULL
};

typedef CMqueueReqTimedreceive_Context Context;

#define NAME "/mq-timedreceive"

#define ERRNO_POISON -1

#define PRIO_POISON UINT_MAX

#define BUFFER_POISON 0xff

#define MAX_MSG 2

#define RECEIVE_BYTE 0x5a

#define FILLER_BYTE 0xa5

#define SENDER_BYTE 0x3c

#define EVENT_SEND RTEMS_EVENT_0

static POSIX_Message_queue_Control *GetQueue( mqd_t mqdes )
{
  return (POSIX_Message_queue_Control *) _Objects_Get_no_protection(
    (Objects_Id) mqdes,
    &_POSIX_Message_queue_Information
  );
}

static uint32_t GetPendingCount( const Context *ctx )
{
  return ctx->queue->Message_queue.number_of_pending_messages;
}

static const CORE_message_queue_Buffer *GetFirstMessage( const Context *ctx )
{
  return (const CORE_message_queue_Buffer *) _Chain_Immutable_first(
    &ctx->queue->Message_queue.Pending_messages
  );
}

static const CORE_message_queue_Buffer *GetLastMessage( const Context *ctx )
{
  return (const CORE_message_queue_Buffer *) _Chain_Immutable_last(
    &ctx->queue->Message_queue.Pending_messages
  );
}

static const CORE_message_queue_Buffer *FindMessage(
  const Context *ctx,
  uint8_t        byte
)
{
  const Chain_Control *chain;
  const Chain_Node    *node;

  chain = &ctx->queue->Message_queue.Pending_messages;
  node = _Chain_Immutable_first( chain );

  while ( node != _Chain_Immutable_tail( chain ) ) {
    const CORE_message_queue_Buffer *buffer;
    const uint8_t                   *content;

    buffer = (const CORE_message_queue_Buffer *) node;
    content = (const uint8_t *) &buffer->buffer[ 0 ];

    if ( content[ 0 ] == byte ) {
      return buffer;
    }

    node = _Chain_Immutable_next( node );
  }

  return NULL;
}

static void CheckBuffer( const Context *ctx, size_t size )
{
  size_t i;

  for ( i = 0; i < sizeof( ctx->buf ); ++i ) {
    if ( i < size ) {
      T_eq_u8( (uint8_t) ctx->buf[ i ], RECEIVE_BYTE );
    } else {
      T_eq_u8( (uint8_t) ctx->buf[ i ], BUFFER_POISON );
    }
  }
}

static void SetFlags( Context *ctx, int oflag )
{
  struct mq_attr attr;

  memset( &attr, 0, sizeof( attr ) );
  attr.mq_flags = oflag;
  T_eq_int( mq_setattr( ctx->queue_id, &attr, NULL ), 0 );
}

static void Send( Context *ctx, uint8_t byte, size_t size, unsigned int prio )
{
  char msg[ MSG_SIZE ];
  int  oflag;

  oflag = ctx->queue->oflag;
  SetFlags( ctx, O_RDWR );
  memset( msg, byte, sizeof( msg ) );
  T_eq_int( mq_send( ctx->queue_id, msg, size, prio ), 0 );
  SetFlags( ctx, oflag );
}

static void SendStored( Context *ctx )
{
  Send( ctx, RECEIVE_BYTE, ctx->stored_size, ctx->stored_prio );
}

static void SendFiller( Context *ctx )
{
  Send( ctx, FILLER_BYTE, MSG_SIZE, 0 );
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    SuspendSelf();
    errno = ERRNO_POISON;
    ctx->status = mq_timedreceive(
      ctx->mqdes,
      ctx->buf,
      ctx->msg_len,
      ctx->msg_prio,
      ctx->abstime
    );
    ctx->errno_value = errno;
    ctx->done = true;
  }
}

static void SenderTask( rtems_task_argument arg )
{
  Context *ctx;
  char     msg[ MSG_SIZE ];

  ctx = (Context *) arg;
  memset( msg, SENDER_BYTE, sizeof( msg ) );

  while ( true ) {
    (void) ReceiveAnyEvents();
    T_quiet_eq_int( mq_send( ctx->queue_id, msg, MSG_SIZE, 0 ), 0 );
    ctx->sender->done = true;
  }
}

static void CMqueueReqTimedreceive_Pre_MsgPrio_Prepare(
  CMqueueReqTimedreceive_Context    *ctx,
  CMqueueReqTimedreceive_Pre_MsgPrio state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_MsgPrio_Obj: {
      /*
       * While the `msg_prio` parameter references an object of type unsigned
       * int.
       */
      ctx->msg_prio = &ctx->prio_obj;
      break;
    }

    case CMqueueReqTimedreceive_Pre_MsgPrio_Null: {
      /*
       * While the `msg_prio` parameter is NULL.
       */
      ctx->msg_prio = NULL;
      break;
    }

    case CMqueueReqTimedreceive_Pre_MsgPrio_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_MsgLen_Prepare(
  CMqueueReqTimedreceive_Context   *ctx,
  CMqueueReqTimedreceive_Pre_MsgLen state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_MsgLen_Max: {
      /*
       * While the `msg_len` parameter is equal to the maximum message size of
       * the message queue.
       */
      ctx->msg_len = MSG_SIZE;
      break;
    }

    case CMqueueReqTimedreceive_Pre_MsgLen_Greater: {
      /*
       * While the `msg_len` parameter is greater than the maximum message size
       * of the message queue.
       */
      ctx->msg_len = MSG_SIZE + 1;
      break;
    }

    case CMqueueReqTimedreceive_Pre_MsgLen_TooSmall: {
      /*
       * While the `msg_len` parameter is less than the maximum message size of
       * the message queue.
       */
      ctx->msg_len = MSG_SIZE - 1;
      break;
    }

    case CMqueueReqTimedreceive_Pre_MsgLen_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_MqDes_Prepare(
  CMqueueReqTimedreceive_Context  *ctx,
  CMqueueReqTimedreceive_Pre_MqDes state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_MqDes_Valid: {
      /*
       * While the `mqdes` parameter is the message queue descriptor of a
       * message queue.
       */
      ctx->mqdes = ctx->queue_id;
      break;
    }

    case CMqueueReqTimedreceive_Pre_MqDes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = (mqd_t) INVALID_ID;
      break;
    }

    case CMqueueReqTimedreceive_Pre_MqDes_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_StoredPrio_Prepare(
  CMqueueReqTimedreceive_Context       *ctx,
  CMqueueReqTimedreceive_Pre_StoredPrio state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_StoredPrio_Low: {
      /*
       * While the message priority of the first message of the message queue
       * is zero.
       */
      ctx->stored_prio = 0;
      break;
    }

    case CMqueueReqTimedreceive_Pre_StoredPrio_High: {
      /*
       * While the message priority of the first message of the message queue
       * is greater than zero.
       */
      ctx->stored_prio = MQ_PRIO_MAX;
      break;
    }

    case CMqueueReqTimedreceive_Pre_StoredPrio_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_StoredSize_Prepare(
  CMqueueReqTimedreceive_Context       *ctx,
  CMqueueReqTimedreceive_Pre_StoredSize state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_StoredSize_One: {
      /*
       * While the message size of the first message of the message queue is
       * one.
       */
      ctx->stored_size = 1;
      break;
    }

    case CMqueueReqTimedreceive_Pre_StoredSize_Max: {
      /*
       * While the message size of the first message of the message queue is
       * greater than one.
       */
      ctx->stored_size = MSG_SIZE;
      break;
    }

    case CMqueueReqTimedreceive_Pre_StoredSize_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_Queue_Prepare(
  CMqueueReqTimedreceive_Context  *ctx,
  CMqueueReqTimedreceive_Pre_Queue state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Queue_Empty: {
      /*
       * While the message queue contains no message.
       */
      ctx->empty = true;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Queue_Pending: {
      /*
       * While the message queue contains messages and no thread waits to send
       * a message to the message queue.
       */
      SendStored( ctx );
      SendFiller( ctx );
      break;
    }

    case CMqueueReqTimedreceive_Pre_Queue_Sender: {
      /*
       * While the message queue is full and a thread waits to send a message
       * to the message queue.
       */
      SendStored( ctx );
      SendFiller( ctx );
      SendEvents( ctx->sender_id, EVENT_SEND );
      WaitForExecutionStop( ctx->sender_id );
      T_false( ctx->sender->done );
      ctx->sender_waits = true;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Queue_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_Access_Prepare(
  CMqueueReqTimedreceive_Context   *ctx,
  CMqueueReqTimedreceive_Pre_Access state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Access_ReadWrite: {
      /*
       * While the message queue is open for reading and writing.
       */
      ctx->oflag = O_RDWR;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Access_ReadOnly: {
      /*
       * While the message queue is open for reading only.
       */
      ctx->oflag = O_RDONLY;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Access_WriteOnly: {
      /*
       * While the message queue is open for writing only.
       */
      ctx->oflag = O_WRONLY;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Access_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_NonBlock_Prepare(
  CMqueueReqTimedreceive_Context     *ctx,
  CMqueueReqTimedreceive_Pre_NonBlock state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_NonBlock_Yes: {
      /*
       * While the flags of the message queue contain O_NONBLOCK.
       */
      SetFlags( ctx, ctx->oflag | O_NONBLOCK );
      break;
    }

    case CMqueueReqTimedreceive_Pre_NonBlock_No: {
      /*
       * While the flags of the message queue do not contain O_NONBLOCK.
       */
      SetFlags( ctx, ctx->oflag );
      break;
    }

    case CMqueueReqTimedreceive_Pre_NonBlock_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_Open_Prepare(
  CMqueueReqTimedreceive_Context *ctx,
  CMqueueReqTimedreceive_Pre_Open state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Open_Yes: {
      /*
       * While the message queue is open.
       */
      /* The message queue was opened by its creation */
      break;
    }

    case CMqueueReqTimedreceive_Pre_Open_No: {
      /*
       * While the message queue is not open.
       */
      T_eq_int( mq_close( ctx->queue_id ), 0 );
      break;
    }

    case CMqueueReqTimedreceive_Pre_Open_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_Abstime_Prepare(
  CMqueueReqTimedreceive_Context    *ctx,
  CMqueueReqTimedreceive_Pre_Abstime state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Abstime_Future: {
      /*
       * While the `abs_timeout` parameter references an object of type struct
       * timespec which specifies a time point after the current time of the
       * CLOCK_REALTIME.
       */
      ctx->abstime_obj.tv_sec = 1621322302;
      ctx->abstime_obj.tv_nsec = 0;
      ctx->abstime = &ctx->abstime_obj;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Abstime_PastOrNow: {
      /*
       * While the `abs_timeout` parameter references an object of type struct
       * timespec which specifies a time point before or at the current time of
       * the CLOCK_REALTIME.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = 0;
      ctx->abstime = &ctx->abstime_obj;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Abstime_InvalidNsec: {
      /*
       * While the `abs_timeout` parameter references an object of type struct
       * timespec with an invalid nanoseconds member.
       */
      ctx->abstime_obj.tv_sec = 1621322302;
      ctx->abstime_obj.tv_nsec = -1;
      ctx->abstime = &ctx->abstime_obj;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Abstime_Null: {
      /*
       * While the `abs_timeout` parameter is NULL.
       */
      ctx->abstime = NULL;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Abstime_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_Send_Prepare(
  CMqueueReqTimedreceive_Context *ctx,
  CMqueueReqTimedreceive_Pre_Send state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Send_Yes: {
      /*
       * While a thread sends a message to the message queue when the calling
       * thread waits for a message.
       */
      ctx->send = true;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Send_No: {
      /*
       * While no thread sends a message to the message queue when the calling
       * thread waits for a message.
       */
      ctx->send = false;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Send_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Status_Check(
  CMqueueReqTimedreceive_Context    *ctx,
  CMqueueReqTimedreceive_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Status_One: {
      /*
       * The return value of mq_timedreceive() shall be one.
       */
      T_eq_ssz( ctx->status, 1 );
      break;
    }

    case CMqueueReqTimedreceive_Post_Status_Max: {
      /*
       * The return value of mq_timedreceive() shall be the message size of the
       * received message.
       */
      T_eq_ssz( ctx->status, MSG_SIZE );
      break;
    }

    case CMqueueReqTimedreceive_Post_Status_MinusOne: {
      /*
       * The return value of mq_timedreceive() shall be minus one.
       */
      T_eq_ssz( ctx->status, -1 );
      break;
    }

    case CMqueueReqTimedreceive_Post_Status_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Errno_Check(
  CMqueueReqTimedreceive_Context   *ctx,
  CMqueueReqTimedreceive_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified by the mq_timedreceive()
       * call.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_Badf: {
      /*
       * The value of errno shall be EBADF.
       */
      T_eq_int( ctx->errno_value, EBADF );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_MsgSize: {
      /*
       * The value of errno shall be EMSGSIZE.
       */
      T_eq_int( ctx->errno_value, EMSGSIZE );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_Again: {
      /*
       * The value of errno shall be EAGAIN.
       */
      T_eq_int( ctx->errno_value, EAGAIN );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_TimedOut: {
      /*
       * The value of errno shall be ETIMEDOUT.
       */
      T_eq_int( ctx->errno_value, ETIMEDOUT );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Buffer_Check(
  CMqueueReqTimedreceive_Context    *ctx,
  CMqueueReqTimedreceive_Post_Buffer state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Buffer_One: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall consist of the
       * received message of one byte followed by the bytes of the buffer
       * before the call.
       */
      CheckBuffer( ctx, 1 );
      break;
    }

    case CMqueueReqTimedreceive_Post_Buffer_Max: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall consist of the
       * received message of the maximum message size followed by the bytes of
       * the buffer before the call.
       */
      CheckBuffer( ctx, MSG_SIZE );
      break;
    }

    case CMqueueReqTimedreceive_Post_Buffer_Nop: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall not be
       * modified.
       */
      CheckBuffer( ctx, 0 );
      break;
    }

    case CMqueueReqTimedreceive_Post_Buffer_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Priority_Check(
  CMqueueReqTimedreceive_Context      *ctx,
  CMqueueReqTimedreceive_Post_Priority state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Priority_Low: {
      /*
       * The object referenced by the `msg_prio` parameter shall be zero.
       */
      T_eq_uint( ctx->prio_obj, 0 );
      break;
    }

    case CMqueueReqTimedreceive_Post_Priority_High: {
      /*
       * The object referenced by the `msg_prio` parameter shall be the message
       * priority of the received message.
       */
      T_eq_uint( ctx->prio_obj, MQ_PRIO_MAX );
      break;
    }

    case CMqueueReqTimedreceive_Post_Priority_Nop: {
      /*
       * The object referenced by the `msg_prio` parameter shall not be
       * modified.
       */
      T_eq_uint( ctx->prio_obj, PRIO_POISON );
      break;
    }

    case CMqueueReqTimedreceive_Post_Priority_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Queue_Check(
  CMqueueReqTimedreceive_Context   *ctx,
  CMqueueReqTimedreceive_Post_Queue state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Queue_Removed: {
      /*
       * The received message shall be removed from the message queue.
       */
      T_null( FindMessage( ctx, RECEIVE_BYTE ) );
      T_eq_ptr( FindMessage( ctx, FILLER_BYTE ), GetFirstMessage( ctx ) );
      break;
    }

    case CMqueueReqTimedreceive_Post_Queue_Nop: {
      /*
       * The message queue shall not be modified by the mq_timedreceive() call.
       */
      T_eq_u32( GetPendingCount( ctx ), ctx->pending_count );

      if ( ctx->pending_count > 0 ) {
        T_eq_ptr( FindMessage( ctx, RECEIVE_BYTE ), GetFirstMessage( ctx ) );
      }
      break;
    }

    case CMqueueReqTimedreceive_Post_Queue_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Sender_Check(
  CMqueueReqTimedreceive_Context    *ctx,
  CMqueueReqTimedreceive_Post_Sender state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Sender_Moved: {
      /*
       * The message of the thread which waits to send a message shall be added
       * to the message queue.
       */
      T_true( ctx->sender->done );
      T_eq_ptr( FindMessage( ctx, SENDER_BYTE ), GetLastMessage( ctx ) );
      break;
    }

    case CMqueueReqTimedreceive_Post_Sender_Nop: {
      /*
       * The thread which waits to send a message shall still wait.
       */
      T_false( ctx->sender->done );
      T_null( FindMessage( ctx, SENDER_BYTE ) );
      break;
    }

    case CMqueueReqTimedreceive_Post_Sender_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Timer_Check(
  CMqueueReqTimedreceive_Context   *ctx,
  CMqueueReqTimedreceive_Post_Timer state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case CMqueueReqTimedreceive_Post_Timer_Realtime: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_REALTIME.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_REALTIME );
      break;
    }

    case CMqueueReqTimedreceive_Post_Timer_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Setup( CMqueueReqTimedreceive_Context *ctx )
{
  rtems_time_of_day now = { 1988, 1, 1, 0, 0, 0, 0 };

  T_rsc_success( rtems_clock_set( &now ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
  WaitForExecutionStop( ctx->worker_id );
  ctx->sender_id = CreateTask( "SEND", PRIO_HIGH );
  StartTask( ctx->sender_id, SenderTask, ctx );
  WaitForExecutionStop( ctx->sender_id );
}

static void CMqueueReqTimedreceive_Setup_Wrap( void *arg )
{
  CMqueueReqTimedreceive_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqTimedreceive_Setup( ctx );
}

static void CMqueueReqTimedreceive_Teardown(
  CMqueueReqTimedreceive_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  DeleteTask( ctx->sender_id );
  RestoreRunnerPriority();
}

static void CMqueueReqTimedreceive_Teardown_Wrap( void *arg )
{
  CMqueueReqTimedreceive_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqTimedreceive_Teardown( ctx );
}

static void CMqueueReqTimedreceive_Prepare(
  CMqueueReqTimedreceive_Context *ctx
)
{
  static const struct mq_attr attr = {
    .mq_maxmsg = MAX_MSG,
    .mq_msgsize = MSG_SIZE
  };

  ctx->queue_id = mq_open( NAME, O_CREAT | O_EXCL | O_RDWR, 0, &attr );
  T_ne_u32( ctx->queue_id, (mqd_t) -1 );
  ctx->queue = GetQueue( ctx->queue_id );
  T_assert_not_null( ctx->queue );
  ctx->empty = false;
  ctx->done = false;
  ctx->sender_waits = false;
  ctx->sender->done = false;
  ctx->prio_obj = PRIO_POISON;
  memset( ctx->buf, BUFFER_POISON, sizeof( ctx->buf ) );
  ctx->abstime = NULL;
  ctx->send = false;
}

static void CMqueueReqTimedreceive_Action(
  CMqueueReqTimedreceive_Context *ctx
)
{
  ctx->pending_count = GetPendingCount( ctx );
  ResumeTask( ctx->worker_id );
  WaitForExecutionStop( ctx->worker_id );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );

  if ( !ctx->done ) {
    T_true( ctx->empty );

    if ( ctx->send ) {
      SendStored( ctx );
    } else {
      FinalClockTick();
    }

    WaitForExecutionStop( ctx->worker_id );
    T_true( ctx->done );
  }

  if ( ctx->sender_waits ) {
    WaitForExecutionStop( ctx->sender_id );
  }
}

static void CMqueueReqTimedreceive_Cleanup(
  CMqueueReqTimedreceive_Context *ctx
)
{
  if ( ctx->queue->open_count == 0 ) {
    T_eq_u32( mq_open( NAME, O_RDWR ), ctx->queue_id );
  }

  SetFlags( ctx, O_RDWR );

  if ( ctx->sender_waits && !ctx->sender->done ) {
    char buf[ MSG_SIZE ];

    T_gt_ssz( mq_receive( ctx->queue_id, buf, sizeof( buf ), NULL ), 0 );
    WaitForExecutionStop( ctx->sender_id );
    T_true( ctx->sender->done );
  }

  T_eq_int( mq_close( ctx->queue_id ), 0 );
  T_eq_int( mq_unlink( NAME ), 0 );
}

/* clang-format off */

static const CMqueueReqTimedreceive_Entry
CMqueueReqTimedreceive_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_NA,
    CMqueueReqTimedreceive_Post_Errno_NA,
    CMqueueReqTimedreceive_Post_Buffer_NA,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_NA,
    CMqueueReqTimedreceive_Post_Sender_NA, CMqueueReqTimedreceive_Post_Timer_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Badf,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Badf,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Badf,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Badf,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_MsgSize,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_MsgSize,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Inval,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_MsgSize,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Inval,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_MsgSize,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Again,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_TimedOut,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_TimedOut,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Again,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_TimedOut,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_TimedOut,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_Moved,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_Moved,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_Low,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_Low,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_Moved,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_Low,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_Low,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_Moved,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_High,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_High,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_Moved,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_High,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_High,
    CMqueueReqTimedreceive_Post_Queue_Removed,
    CMqueueReqTimedreceive_Post_Sender_Moved,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_Low,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_Low,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_One,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_One,
    CMqueueReqTimedreceive_Post_Priority_High,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_Max,
    CMqueueReqTimedreceive_Post_Errno_Nop,
    CMqueueReqTimedreceive_Post_Buffer_Max,
    CMqueueReqTimedreceive_Post_Priority_High,
    CMqueueReqTimedreceive_Post_Queue_Nop,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Badf,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_Nop,
    CMqueueReqTimedreceive_Post_Queue_NA,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_Badf,
    CMqueueReqTimedreceive_Post_Buffer_Nop,
    CMqueueReqTimedreceive_Post_Priority_NA,
    CMqueueReqTimedreceive_Post_Queue_NA,
    CMqueueReqTimedreceive_Post_Sender_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive }
};

static const uint8_t
CMqueueReqTimedreceive_Map[] = {
  11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 31, 12, 13, 0, 7, 0, 7, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 31,
  12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0,
  3, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 22,
  0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 3, 0,
  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
  0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 32, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 32, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 23,
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0,
  0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 24, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
  3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 11,
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 33, 12, 13, 0, 7, 0, 7, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 33, 12,
  13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 3,
  0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 26, 0,
  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 34, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 34, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 27, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0,
  0, 3, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
  28, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3,
  0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 35, 0,
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
  0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 31, 12,
  13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 0, 0, 0, 31, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  21, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 22, 0,
  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
  0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0,
  0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
  0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 32, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 32, 12, 13, 0, 7, 0, 7, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
  24, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 3,
  0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0,
  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
  0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 33, 12, 13, 0,
  7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 33, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  25, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 25, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0,
  0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
  0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 0, 0,
  0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3,
  0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 34, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0, 0, 0, 0, 0, 0, 11, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 34, 12, 13, 0, 7, 0, 7, 0, 1, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
  0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,
  5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0,
  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,
  5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0,
  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0,
  0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
  8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0,
  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
  0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0,
  0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,
  5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0,
  0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,
  5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0,
  0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0,
  0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
  8, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0,
  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0,
  0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0,
  0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0,
  0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0,
  0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 8, 0,
  0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0,
  0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0,
  0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
  3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0,
  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0,
  0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0,
  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0,
  0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0,
  0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0,
  0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
  0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0,
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
  0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 29, 15, 16, 0, 9, 0, 9,
  0, 2, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  29, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2,
  0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0,
  0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
  0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,
  0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0,
  0, 4, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
  18, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 4,
  0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
  0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 30, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 30, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0,
  0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
  0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2,
  0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0,
  0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
  0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
  0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
  14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 29, 15, 16, 0, 9, 0, 9, 0,
  2, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 29,
  15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0,
  0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0,
  0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
  0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0,
  4, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 18,
  0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 4, 0,
  0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
  0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0,
  0, 0, 0, 30, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0,
  0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 30, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0,
  0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19,
  0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
  0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0,
  0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0,
  0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
  0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
  4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 36,
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
  0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 29, 15,
  16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
  0, 0, 0, 0, 0, 29, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,
  0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0,
  0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2,
  0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 18, 0,
  0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
  0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0,
  0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
  0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0,
  0, 2, 0, 0, 0, 0, 0, 0, 0, 30, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 30, 15, 16, 0, 9, 0, 9, 0,
  2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
  0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0,
  0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
  0, 2, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
  20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4,
  0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0,
  0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
  0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 29, 15, 16, 0,
  9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0,
  0, 0, 0, 29, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,
  0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 2,
  0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 17, 0,
  0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0,
  0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
  0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0,
  0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4,
  0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 0, 30, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0, 0, 0, 0, 0, 0, 14, 0,
  0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 30, 15, 16, 0, 9, 0, 9, 0, 2, 0, 0,
  0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0,
  0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,
  0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0,
  0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0,
  0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
  0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0,
  0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
  0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0,
  6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,
  0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0,
  6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0,
  0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0,
  0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0,
  0, 10, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
  4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0,
  0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0,
  0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  2, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0,
  0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,
  0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0,
  10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0,
  10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4,
  0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0,
  0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0,
  0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0,
  0, 6, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2,
  0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0,
  0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0,
  0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0,
  0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2,
  0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0,
  0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0,
  0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
  0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
  0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0,
  6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0,
  0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0,
  6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0,
  0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0,
  0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0,
  0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4,
  0, 0, 0, 0, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CMqueueReqTimedreceive_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqTimedreceive_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqTimedreceive_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqTimedreceive_Fixture = {
  .setup = CMqueueReqTimedreceive_Setup_Wrap,
  .stop = NULL,
  .teardown = CMqueueReqTimedreceive_Teardown_Wrap,
  .scope = CMqueueReqTimedreceive_Scope,
  .initial_context = &CMqueueReqTimedreceive_Instance
};

static inline CMqueueReqTimedreceive_Entry CMqueueReqTimedreceive_PopEntry(
  CMqueueReqTimedreceive_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqTimedreceive_Entries[ CMqueueReqTimedreceive_Map[ index ] ];
}

static void CMqueueReqTimedreceive_SetPreConditionStates(
  CMqueueReqTimedreceive_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_StoredPrio_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqTimedreceive_Pre_StoredPrio_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_StoredSize_NA ) {
    ctx->Map.pcs[ 4 ] = CMqueueReqTimedreceive_Pre_StoredSize_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Queue_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqTimedreceive_Pre_Queue_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Access_NA ) {
    ctx->Map.pcs[ 6 ] = CMqueueReqTimedreceive_Pre_Access_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_NonBlock_NA ) {
    ctx->Map.pcs[ 7 ] = CMqueueReqTimedreceive_Pre_NonBlock_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 8 ] = CMqueueReqTimedreceive_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }

  if ( ctx->Map.entry.Pre_Abstime_NA ) {
    ctx->Map.pcs[ 9 ] = CMqueueReqTimedreceive_Pre_Abstime_NA;
  } else {
    ctx->Map.pcs[ 9 ] = ctx->Map.pci[ 9 ];
  }

  if ( ctx->Map.entry.Pre_Send_NA ) {
    ctx->Map.pcs[ 10 ] = CMqueueReqTimedreceive_Pre_Send_NA;
  } else {
    ctx->Map.pcs[ 10 ] = ctx->Map.pci[ 10 ];
  }
}

static void CMqueueReqTimedreceive_TestVariant(
  CMqueueReqTimedreceive_Context *ctx
)
{
  CMqueueReqTimedreceive_Pre_MsgPrio_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqTimedreceive_Pre_MsgLen_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqTimedreceive_Pre_MqDes_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqTimedreceive_Pre_StoredPrio_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqTimedreceive_Pre_StoredSize_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqTimedreceive_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqTimedreceive_Pre_Access_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CMqueueReqTimedreceive_Pre_NonBlock_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CMqueueReqTimedreceive_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  CMqueueReqTimedreceive_Pre_Abstime_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  CMqueueReqTimedreceive_Pre_Send_Prepare( ctx, ctx->Map.pcs[ 10 ] );
  CMqueueReqTimedreceive_Action( ctx );
  CMqueueReqTimedreceive_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqTimedreceive_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqTimedreceive_Post_Buffer_Check( ctx, ctx->Map.entry.Post_Buffer );
  CMqueueReqTimedreceive_Post_Priority_Check(
    ctx,
    ctx->Map.entry.Post_Priority
  );
  CMqueueReqTimedreceive_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
  CMqueueReqTimedreceive_Post_Sender_Check( ctx, ctx->Map.entry.Post_Sender );
  CMqueueReqTimedreceive_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
}

/**
 * @fn void T_case_body_CMqueueReqTimedreceive( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqTimedreceive, &CMqueueReqTimedreceive_Fixture )
{
  CMqueueReqTimedreceive_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqTimedreceive_Pre_MsgPrio_Obj;
    ctx->Map.pci[ 0 ] < CMqueueReqTimedreceive_Pre_MsgPrio_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqTimedreceive_Pre_MsgLen_Max;
      ctx->Map.pci[ 1 ] < CMqueueReqTimedreceive_Pre_MsgLen_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqTimedreceive_Pre_MqDes_Valid;
        ctx->Map.pci[ 2 ] < CMqueueReqTimedreceive_Pre_MqDes_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqTimedreceive_Pre_StoredPrio_Low;
          ctx->Map.pci[ 3 ] < CMqueueReqTimedreceive_Pre_StoredPrio_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqTimedreceive_Pre_StoredSize_One;
            ctx->Map.pci[ 4 ] < CMqueueReqTimedreceive_Pre_StoredSize_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqTimedreceive_Pre_Queue_Empty;
              ctx->Map.pci[ 5 ] < CMqueueReqTimedreceive_Pre_Queue_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] =
                  CMqueueReqTimedreceive_Pre_Access_ReadWrite;
                ctx->Map.pci[ 6 ] < CMqueueReqTimedreceive_Pre_Access_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CMqueueReqTimedreceive_Pre_NonBlock_Yes;
                  ctx->Map.pci[ 7 ] < CMqueueReqTimedreceive_Pre_NonBlock_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = CMqueueReqTimedreceive_Pre_Open_Yes;
                    ctx->Map.pci[ 8 ] < CMqueueReqTimedreceive_Pre_Open_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    for (
                      ctx->Map.pci[ 9 ] =
                        CMqueueReqTimedreceive_Pre_Abstime_Future;
                      ctx->Map.pci[ 9 ] <
                      CMqueueReqTimedreceive_Pre_Abstime_NA;
                      ++ctx->Map.pci[ 9 ]
                    ) {
                      for (
                        ctx->Map.pci[ 10 ] =
                          CMqueueReqTimedreceive_Pre_Send_Yes;
                        ctx->Map.pci[ 10 ] <
                        CMqueueReqTimedreceive_Pre_Send_NA;
                        ++ctx->Map.pci[ 10 ]
                      ) {
                        ctx->Map.entry = CMqueueReqTimedreceive_PopEntry(
                          ctx
                        );

                        if ( ctx->Map.entry.Skip ) {
                          continue;
                        }

                        CMqueueReqTimedreceive_SetPreConditionStates( ctx );
                        CMqueueReqTimedreceive_Prepare( ctx );
                        CMqueueReqTimedreceive_TestVariant( ctx );
                        CMqueueReqTimedreceive_Cleanup( ctx );
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
