/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqSend
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
 * @defgroup CMqueueReqSend spec:/c/mqueue/req/send
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CMqueueReqSend_Pre_MsgPrio_Low,
  CMqueueReqSend_Pre_MsgPrio_Mid,
  CMqueueReqSend_Pre_MsgPrio_High,
  CMqueueReqSend_Pre_MsgPrio_TooHigh,
  CMqueueReqSend_Pre_MsgPrio_NA
} CMqueueReqSend_Pre_MsgPrio;

typedef enum {
  CMqueueReqSend_Pre_MsgLen_One,
  CMqueueReqSend_Pre_MsgLen_Max,
  CMqueueReqSend_Pre_MsgLen_TooLong,
  CMqueueReqSend_Pre_MsgLen_NA
} CMqueueReqSend_Pre_MsgLen;

typedef enum {
  CMqueueReqSend_Pre_MqDes_Valid,
  CMqueueReqSend_Pre_MqDes_Invalid,
  CMqueueReqSend_Pre_MqDes_NA
} CMqueueReqSend_Pre_MqDes;

typedef enum {
  CMqueueReqSend_Pre_Queue_Empty,
  CMqueueReqSend_Pre_Queue_Receiver,
  CMqueueReqSend_Pre_Queue_Pending,
  CMqueueReqSend_Pre_Queue_Full,
  CMqueueReqSend_Pre_Queue_NA
} CMqueueReqSend_Pre_Queue;

typedef enum {
  CMqueueReqSend_Pre_Access_ReadWrite,
  CMqueueReqSend_Pre_Access_WriteOnly,
  CMqueueReqSend_Pre_Access_ReadOnly,
  CMqueueReqSend_Pre_Access_NA
} CMqueueReqSend_Pre_Access;

typedef enum {
  CMqueueReqSend_Pre_NonBlock_Yes,
  CMqueueReqSend_Pre_NonBlock_No,
  CMqueueReqSend_Pre_NonBlock_NA
} CMqueueReqSend_Pre_NonBlock;

typedef enum {
  CMqueueReqSend_Pre_Open_Yes,
  CMqueueReqSend_Pre_Open_No,
  CMqueueReqSend_Pre_Open_NA
} CMqueueReqSend_Pre_Open;

typedef enum {
  CMqueueReqSend_Post_Status_Zero,
  CMqueueReqSend_Post_Status_MinusOne,
  CMqueueReqSend_Post_Status_NA
} CMqueueReqSend_Post_Status;

typedef enum {
  CMqueueReqSend_Post_Errno_Nop,
  CMqueueReqSend_Post_Errno_Inval,
  CMqueueReqSend_Post_Errno_Badf,
  CMqueueReqSend_Post_Errno_MsgSize,
  CMqueueReqSend_Post_Errno_Again,
  CMqueueReqSend_Post_Errno_NA
} CMqueueReqSend_Post_Errno;

typedef enum {
  CMqueueReqSend_Post_Message_Receiver,
  CMqueueReqSend_Post_Message_Front,
  CMqueueReqSend_Post_Message_Back,
  CMqueueReqSend_Post_Message_Nop,
  CMqueueReqSend_Post_Message_NA
} CMqueueReqSend_Post_Message;

typedef enum {
  CMqueueReqSend_Post_Priority_Low,
  CMqueueReqSend_Post_Priority_Mid,
  CMqueueReqSend_Post_Priority_High,
  CMqueueReqSend_Post_Priority_NA
} CMqueueReqSend_Post_Priority;

typedef enum {
  CMqueueReqSend_Post_Size_One,
  CMqueueReqSend_Post_Size_Max,
  CMqueueReqSend_Post_Size_NA
} CMqueueReqSend_Post_Size;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_MsgPrio_NA : 1;
  uint32_t Pre_MsgLen_NA : 1;
  uint32_t Pre_MqDes_NA : 1;
  uint32_t Pre_Queue_NA : 1;
  uint32_t Pre_Access_NA : 1;
  uint32_t Pre_NonBlock_NA : 1;
  uint32_t Pre_Open_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 3;
  uint32_t Post_Message : 3;
  uint32_t Post_Priority : 2;
  uint32_t Post_Size : 2;
} CMqueueReqSend_Entry;

#define MSG_SIZE 4

typedef struct {
  volatile bool is_waiting;
  uint32_t      wait_count;
} Receiver;

/**
 * @brief Test context for spec:/c/mqueue/req/send test case.
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
   * @brief This member contains the number of pending messages before the
   *   mq_send() call.
   */
  uint32_t pending_count;

  /**
   * @brief If this member is true, then the message queue was full.
   */
  bool full;

  /**
   * @brief This member contains the identifier of the task which calls
   *   mq_send().
   */
  rtems_id worker_id;

  /**
   * @brief If this member is true, then the mq_send() call returned.
   */
  volatile bool done;

  /**
   * @brief This member contains the identifier of the task which waits to
   *   receive a message.
   */
  rtems_id receiver_id;

  /**
   * @brief This member provides the state of the receiving task.
   */
  Receiver receiver[ 1 ];

  /**
   * @brief If this member is true, then the receiving task waited for a
   *   message before the mq_send() call.
   */
  bool receiver_waits;

  /**
   * @brief If this member is true, then the receiving task received a message
   *   by the mq_send() call.
   */
  bool received;

  /**
   * @brief This member contains the message received by the receiving task.
   */
  char recv_buf[ MSG_SIZE ];

  /**
   * @brief This member contains the message to send.
   */
  char msg[ MSG_SIZE + 1 ];

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
  unsigned int msg_prio;

  /**
   * @brief This member contains the return value of the mq_send() call.
   */
  int status;

  /**
   * @brief This member contains the value of errno after the mq_send() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 7 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 7 ];

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
    CMqueueReqSend_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqSend_Context;

static CMqueueReqSend_Context CMqueueReqSend_Instance;

static const char *const CMqueueReqSend_PreDesc_MsgPrio[] =
  { "Low", "Mid", "High", "TooHigh", "NA" };

static const char *const CMqueueReqSend_PreDesc_MsgLen[] =
  { "One", "Max", "TooLong", "NA" };

static const char *const CMqueueReqSend_PreDesc_MqDes[] =
  { "Valid", "Invalid", "NA" };

static const char *const CMqueueReqSend_PreDesc_Queue[] =
  { "Empty", "Receiver", "Pending", "Full", "NA" };

static const char *const CMqueueReqSend_PreDesc_Access[] =
  { "ReadWrite", "WriteOnly", "ReadOnly", "NA" };

static const char *const CMqueueReqSend_PreDesc_NonBlock[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqSend_PreDesc_Open[] = { "Yes", "No", "NA" };

static const char *const *const CMqueueReqSend_PreDesc[] = {
  CMqueueReqSend_PreDesc_MsgPrio,
  CMqueueReqSend_PreDesc_MsgLen,
  CMqueueReqSend_PreDesc_MqDes,
  CMqueueReqSend_PreDesc_Queue,
  CMqueueReqSend_PreDesc_Access,
  CMqueueReqSend_PreDesc_NonBlock,
  CMqueueReqSend_PreDesc_Open,
  NULL
};

typedef CMqueueReqSend_Context Context;

#define NAME "/mq-send"

#define ERRNO_POISON -1

#define MAX_MSG 2

#define PRIO_MID ( MQ_PRIO_MAX / 2 )

#define SEND_BYTE 0x5a

#define FILLER_BYTE 0xa5

#define RECV_POISON 0xff

#define EVENT_RECEIVE RTEMS_EVENT_0

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

static const CORE_message_queue_Buffer *FindSentMessage( const Context *ctx )
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

    if ( content[ 0 ] == SEND_BYTE ) {
      return buffer;
    }

    node = _Chain_Immutable_next( node );
  }

  return NULL;
}

static unsigned int GetSentPriority( const Context *ctx )
{
  const CORE_message_queue_Buffer *buffer;

  if ( ctx->received ) {
    return _POSIX_Message_queue_Priority_from_core(
      (CORE_message_queue_Submit_types) ctx->receiver->wait_count
    );
  }

  buffer = FindSentMessage( ctx );
  T_quiet_assert_not_null( buffer );

  return _POSIX_Message_queue_Priority_from_core( buffer->priority );
}

static void CheckSentContent( const Context *ctx, size_t size )
{
  const CORE_message_queue_Buffer *buffer;
  const char                      *content;

  if ( ctx->received ) {
    size_t i;

    /* The sending call copies the message to the buffer of the receiver */
    for ( i = size; i < sizeof( ctx->recv_buf ); ++i ) {
      T_eq_u8( (uint8_t) ctx->recv_buf[ i ], RECV_POISON );
    }

    content = ctx->recv_buf;
  } else {
    buffer = FindSentMessage( ctx );
    T_quiet_assert_not_null( buffer );
    T_eq_sz( buffer->size, size );
    content = (const char *) &buffer->buffer[ 0 ];
  }

  T_eq_mem( content, ctx->msg, size );
}

static void SetFlags( Context *ctx, int oflag )
{
  struct mq_attr attr;

  memset( &attr, 0, sizeof( attr ) );
  attr.mq_flags = oflag;
  T_eq_int( mq_setattr( ctx->queue_id, &attr, NULL ), 0 );
}

static void SendFiller( Context *ctx )
{
  char filler[ MSG_SIZE ];

  memset( filler, FILLER_BYTE, sizeof( filler ) );
  T_eq_int( mq_send( ctx->queue_id, filler, MSG_SIZE, PRIO_MID ), 0 );
}

static void ReceiveFiller( Context *ctx )
{
  char         buf[ MSG_SIZE ];
  unsigned int prio;
  int          oflag;

  oflag = ctx->queue->oflag;
  SetFlags( ctx, O_RDWR );
  T_eq_ssz( mq_receive( ctx->queue_id, buf, sizeof( buf ), &prio ), MSG_SIZE );
  T_eq_u8( (uint8_t) buf[ 0 ], FILLER_BYTE );
  SetFlags( ctx, oflag );
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    SuspendSelf();
    errno = ERRNO_POISON;
    ctx->status = mq_send( ctx->mqdes, ctx->msg, ctx->msg_len, ctx->msg_prio );
    ctx->errno_value = errno;
    ctx->done = true;
  }
}

static void ReceiverTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    (void) ReceiveAnyEvents();
    ctx->receiver->is_waiting = true;
    (void) mq_receive(
      ctx->queue_id,
      ctx->recv_buf,
      sizeof( ctx->recv_buf ),
      NULL
    );

    /* The sending call stores the priority in the waiting thread */
    ctx->receiver->wait_count = GetExecuting()->Wait.count;
    ctx->receiver->is_waiting = false;
  }
}

static void CMqueueReqSend_Pre_MsgPrio_Prepare(
  CMqueueReqSend_Context    *ctx,
  CMqueueReqSend_Pre_MsgPrio state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_MsgPrio_Low: {
      /*
       * While the `msg_prio` parameter is zero.
       */
      ctx->msg_prio = 0;
      break;
    }

    case CMqueueReqSend_Pre_MsgPrio_Mid: {
      /*
       * While the `msg_prio` parameter is equal to the message priority of the
       * messages in the message queue.
       */
      ctx->msg_prio = PRIO_MID;
      break;
    }

    case CMqueueReqSend_Pre_MsgPrio_High: {
      /*
       * While the `msg_prio` parameter is equal to MQ_PRIO_MAX.
       */
      ctx->msg_prio = MQ_PRIO_MAX;
      break;
    }

    case CMqueueReqSend_Pre_MsgPrio_TooHigh: {
      /*
       * While the `msg_prio` parameter is greater than MQ_PRIO_MAX.
       */
      ctx->msg_prio = MQ_PRIO_MAX + 1;
      break;
    }

    case CMqueueReqSend_Pre_MsgPrio_NA:
      break;
  }
}

static void CMqueueReqSend_Pre_MsgLen_Prepare(
  CMqueueReqSend_Context   *ctx,
  CMqueueReqSend_Pre_MsgLen state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_MsgLen_One: {
      /*
       * While the `msg_len` parameter is one.
       */
      ctx->msg_len = 1;
      break;
    }

    case CMqueueReqSend_Pre_MsgLen_Max: {
      /*
       * While the `msg_len` parameter is equal to the maximum message size of
       * the message queue.
       */
      ctx->msg_len = MSG_SIZE;
      break;
    }

    case CMqueueReqSend_Pre_MsgLen_TooLong: {
      /*
       * While the `msg_len` parameter is greater than the maximum message size
       * of the message queue.
       */
      ctx->msg_len = MSG_SIZE + 1;
      break;
    }

    case CMqueueReqSend_Pre_MsgLen_NA:
      break;
  }
}

static void CMqueueReqSend_Pre_MqDes_Prepare(
  CMqueueReqSend_Context  *ctx,
  CMqueueReqSend_Pre_MqDes state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_MqDes_Valid: {
      /*
       * While the `mqdes` parameter is the message queue descriptor of a
       * message queue.
       */
      ctx->mqdes = ctx->queue_id;
      break;
    }

    case CMqueueReqSend_Pre_MqDes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = (mqd_t) INVALID_ID;
      break;
    }

    case CMqueueReqSend_Pre_MqDes_NA:
      break;
  }
}

static void CMqueueReqSend_Pre_Queue_Prepare(
  CMqueueReqSend_Context  *ctx,
  CMqueueReqSend_Pre_Queue state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_Queue_Empty: {
      /*
       * While the message queue contains no message and no thread waits to
       * receive a message from the message queue.
       */
      /* The message queue was created without messages */
      break;
    }

    case CMqueueReqSend_Pre_Queue_Receiver: {
      /*
       * While the message queue contains no message and a thread waits to
       * receive a message from the message queue.
       */
      ctx->receiver_waits = true;
      SendEvents( ctx->receiver_id, EVENT_RECEIVE );
      WaitForExecutionStop( ctx->receiver_id );
      break;
    }

    case CMqueueReqSend_Pre_Queue_Pending: {
      /*
       * While the message queue contains a message and is not full.
       */
      SendFiller( ctx );
      break;
    }

    case CMqueueReqSend_Pre_Queue_Full: {
      /*
       * While the message queue is full and a thread receives a message from
       * the message queue when the calling thread waits for space.
       */
      ctx->full = true;

      while ( GetPendingCount( ctx ) < MAX_MSG ) {
        SendFiller( ctx );
      }
      break;
    }

    case CMqueueReqSend_Pre_Queue_NA:
      break;
  }
}

static void CMqueueReqSend_Pre_Access_Prepare(
  CMqueueReqSend_Context   *ctx,
  CMqueueReqSend_Pre_Access state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_Access_ReadWrite: {
      /*
       * While the message queue is open for reading and writing.
       */
      ctx->oflag = O_RDWR;
      break;
    }

    case CMqueueReqSend_Pre_Access_WriteOnly: {
      /*
       * While the message queue is open for writing only.
       */
      ctx->oflag = O_WRONLY;
      break;
    }

    case CMqueueReqSend_Pre_Access_ReadOnly: {
      /*
       * While the message queue is open for reading only.
       */
      ctx->oflag = O_RDONLY;
      break;
    }

    case CMqueueReqSend_Pre_Access_NA:
      break;
  }
}

static void CMqueueReqSend_Pre_NonBlock_Prepare(
  CMqueueReqSend_Context     *ctx,
  CMqueueReqSend_Pre_NonBlock state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_NonBlock_Yes: {
      /*
       * While the flags of the message queue contain O_NONBLOCK.
       */
      SetFlags( ctx, ctx->oflag | O_NONBLOCK );
      break;
    }

    case CMqueueReqSend_Pre_NonBlock_No: {
      /*
       * While the flags of the message queue do not contain O_NONBLOCK.
       */
      SetFlags( ctx, ctx->oflag );
      break;
    }

    case CMqueueReqSend_Pre_NonBlock_NA:
      break;
  }
}

static void CMqueueReqSend_Pre_Open_Prepare(
  CMqueueReqSend_Context *ctx,
  CMqueueReqSend_Pre_Open state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_Open_Yes: {
      /*
       * While the message queue is open.
       */
      /* The message queue was opened by its creation */
      break;
    }

    case CMqueueReqSend_Pre_Open_No: {
      /*
       * While the message queue is not open.
       */
      T_eq_int( mq_close( ctx->queue_id ), 0 );
      break;
    }

    case CMqueueReqSend_Pre_Open_NA:
      break;
  }
}

static void CMqueueReqSend_Post_Status_Check(
  CMqueueReqSend_Context    *ctx,
  CMqueueReqSend_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqSend_Post_Status_Zero: {
      /*
       * The return value of mq_send() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CMqueueReqSend_Post_Status_MinusOne: {
      /*
       * The return value of mq_send() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CMqueueReqSend_Post_Status_NA:
      break;
  }
}

static void CMqueueReqSend_Post_Errno_Check(
  CMqueueReqSend_Context   *ctx,
  CMqueueReqSend_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqSend_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified by the mq_send() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CMqueueReqSend_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CMqueueReqSend_Post_Errno_Badf: {
      /*
       * The value of errno shall be EBADF.
       */
      T_eq_int( ctx->errno_value, EBADF );
      break;
    }

    case CMqueueReqSend_Post_Errno_MsgSize: {
      /*
       * The value of errno shall be EMSGSIZE.
       */
      T_eq_int( ctx->errno_value, EMSGSIZE );
      break;
    }

    case CMqueueReqSend_Post_Errno_Again: {
      /*
       * The value of errno shall be EAGAIN.
       */
      T_eq_int( ctx->errno_value, EAGAIN );
      break;
    }

    case CMqueueReqSend_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqSend_Post_Message_Check(
  CMqueueReqSend_Context     *ctx,
  CMqueueReqSend_Post_Message state
)
{
  switch ( state ) {
    case CMqueueReqSend_Post_Message_Receiver: {
      /*
       * The message shall be received by the thread which waits to receive a
       * message from the message queue.
       */
      T_true( ctx->received );
      T_eq_u8( ctx->recv_buf[ 0 ], SEND_BYTE );
      T_null( FindSentMessage( ctx ) );
      break;
    }

    case CMqueueReqSend_Post_Message_Front: {
      /*
       * The message shall be the first message of the message queue.
       */
      T_false( ctx->received );
      T_eq_ptr( FindSentMessage( ctx ), GetFirstMessage( ctx ) );
      break;
    }

    case CMqueueReqSend_Post_Message_Back: {
      /*
       * The message shall be the last message of the message queue.
       */
      T_false( ctx->received );
      T_eq_ptr( FindSentMessage( ctx ), GetLastMessage( ctx ) );
      break;
    }

    case CMqueueReqSend_Post_Message_Nop: {
      /*
       * Neither the message queue nor a waiting thread shall receive the
       * message.
       */
      T_false( ctx->received );
      T_null( FindSentMessage( ctx ) );
      T_eq_u32( GetPendingCount( ctx ), ctx->pending_count );
      break;
    }

    case CMqueueReqSend_Post_Message_NA:
      break;
  }
}

static void CMqueueReqSend_Post_Priority_Check(
  CMqueueReqSend_Context      *ctx,
  CMqueueReqSend_Post_Priority state
)
{
  switch ( state ) {
    case CMqueueReqSend_Post_Priority_Low: {
      /*
       * The message priority of the message shall be zero.
       */
      T_eq_uint( GetSentPriority( ctx ), 0 );
      break;
    }

    case CMqueueReqSend_Post_Priority_Mid: {
      /*
       * The message priority of the message shall be the value of the
       * `msg_prio` parameter.
       */
      T_eq_uint( GetSentPriority( ctx ), PRIO_MID );
      break;
    }

    case CMqueueReqSend_Post_Priority_High: {
      /*
       * The message priority of the message shall be MQ_PRIO_MAX.
       */
      T_eq_uint( GetSentPriority( ctx ), MQ_PRIO_MAX );
      break;
    }

    case CMqueueReqSend_Post_Priority_NA:
      break;
  }
}

static void CMqueueReqSend_Post_Size_Check(
  CMqueueReqSend_Context  *ctx,
  CMqueueReqSend_Post_Size state
)
{
  switch ( state ) {
    case CMqueueReqSend_Post_Size_One: {
      /*
       * The message shall consist of the first byte referenced by the
       * `msg_ptr` parameter.
       */
      CheckSentContent( ctx, 1 );
      break;
    }

    case CMqueueReqSend_Post_Size_Max: {
      /*
       * The message shall consist of the bytes referenced by the `msg_ptr`
       * parameter of the size specified by the `msg_len` parameter.
       */
      CheckSentContent( ctx, MSG_SIZE );
      break;
    }

    case CMqueueReqSend_Post_Size_NA:
      break;
  }
}

static void CMqueueReqSend_Setup( CMqueueReqSend_Context *ctx )
{
  memset( ctx->msg, SEND_BYTE, sizeof( ctx->msg ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
  WaitForExecutionStop( ctx->worker_id );
  ctx->receiver_id = CreateTask( "RECV", PRIO_HIGH );
  StartTask( ctx->receiver_id, ReceiverTask, ctx );
  WaitForExecutionStop( ctx->receiver_id );
}

static void CMqueueReqSend_Setup_Wrap( void *arg )
{
  CMqueueReqSend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqSend_Setup( ctx );
}

static void CMqueueReqSend_Teardown( CMqueueReqSend_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  DeleteTask( ctx->receiver_id );
  RestoreRunnerPriority();
}

static void CMqueueReqSend_Teardown_Wrap( void *arg )
{
  CMqueueReqSend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqSend_Teardown( ctx );
}

static void CMqueueReqSend_Prepare( CMqueueReqSend_Context *ctx )
{
  static const struct mq_attr attr = {
    .mq_maxmsg = MAX_MSG,
    .mq_msgsize = MSG_SIZE
  };

  ctx->queue_id = mq_open( NAME, O_CREAT | O_EXCL | O_RDWR, 0, &attr );
  T_ne_u32( ctx->queue_id, (mqd_t) -1 );
  ctx->queue = GetQueue( ctx->queue_id );
  T_assert_not_null( ctx->queue );
  ctx->full = false;
  ctx->done = false;
  ctx->receiver_waits = false;
  ctx->received = false;
  memset( ctx->recv_buf, RECV_POISON, sizeof( ctx->recv_buf ) );
}

static void CMqueueReqSend_Action( CMqueueReqSend_Context *ctx )
{
  ctx->pending_count = GetPendingCount( ctx );
  ResumeTask( ctx->worker_id );
  WaitForExecutionStop( ctx->worker_id );

  if ( !ctx->done ) {
    T_true( ctx->full );
    ReceiveFiller( ctx );
    WaitForExecutionStop( ctx->worker_id );
    T_true( ctx->done );
  }

  ctx->received = ctx->receiver_waits && !ctx->receiver->is_waiting;
}

static void CMqueueReqSend_Cleanup( CMqueueReqSend_Context *ctx )
{
  if ( ctx->queue->open_count == 0 ) {
    T_eq_u32( mq_open( NAME, O_RDWR ), ctx->queue_id );
  }

  SetFlags( ctx, O_RDWR );

  if ( ctx->receiver_waits && !ctx->received ) {
    T_eq_int( mq_send( ctx->queue_id, "X", 1, 0 ), 0 );
    WaitForExecutionStop( ctx->receiver_id );
  }

  T_eq_int( mq_close( ctx->queue_id ), 0 );
  T_eq_int( mq_unlink( NAME ), 0 );
}

/* clang-format off */

static const CMqueueReqSend_Entry
CMqueueReqSend_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_NA,
    CMqueueReqSend_Post_Errno_NA, CMqueueReqSend_Post_Message_NA,
    CMqueueReqSend_Post_Priority_NA, CMqueueReqSend_Post_Size_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_Badf, CMqueueReqSend_Post_Message_Nop,
    CMqueueReqSend_Post_Priority_NA, CMqueueReqSend_Post_Size_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_Inval, CMqueueReqSend_Post_Message_Nop,
    CMqueueReqSend_Post_Priority_NA, CMqueueReqSend_Post_Size_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_MsgSize, CMqueueReqSend_Post_Message_Nop,
    CMqueueReqSend_Post_Priority_NA, CMqueueReqSend_Post_Size_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_Again, CMqueueReqSend_Post_Message_Nop,
    CMqueueReqSend_Post_Priority_NA, CMqueueReqSend_Post_Size_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Back,
    CMqueueReqSend_Post_Priority_Low, CMqueueReqSend_Post_Size_One },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Back,
    CMqueueReqSend_Post_Priority_Low, CMqueueReqSend_Post_Size_Max },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Back,
    CMqueueReqSend_Post_Priority_Mid, CMqueueReqSend_Post_Size_One },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Back,
    CMqueueReqSend_Post_Priority_Mid, CMqueueReqSend_Post_Size_Max },
  { 0, 0, 0, 0, 1, 1, 1, 1, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_Badf, CMqueueReqSend_Post_Message_Nop,
    CMqueueReqSend_Post_Priority_NA, CMqueueReqSend_Post_Size_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Front,
    CMqueueReqSend_Post_Priority_High, CMqueueReqSend_Post_Size_One },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Front,
    CMqueueReqSend_Post_Priority_High, CMqueueReqSend_Post_Size_Max },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Receiver,
    CMqueueReqSend_Post_Priority_Low, CMqueueReqSend_Post_Size_One },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Receiver,
    CMqueueReqSend_Post_Priority_Low, CMqueueReqSend_Post_Size_Max },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Receiver,
    CMqueueReqSend_Post_Priority_Mid, CMqueueReqSend_Post_Size_One },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Receiver,
    CMqueueReqSend_Post_Priority_Mid, CMqueueReqSend_Post_Size_Max },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Back,
    CMqueueReqSend_Post_Priority_High, CMqueueReqSend_Post_Size_One },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Receiver,
    CMqueueReqSend_Post_Priority_High, CMqueueReqSend_Post_Size_One },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Back,
    CMqueueReqSend_Post_Priority_High, CMqueueReqSend_Post_Size_Max },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Nop, CMqueueReqSend_Post_Message_Receiver,
    CMqueueReqSend_Post_Priority_High, CMqueueReqSend_Post_Size_Max },
  { 0, 0, 0, 0, 1, 1, 1, 1, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_Inval, CMqueueReqSend_Post_Message_Nop,
    CMqueueReqSend_Post_Priority_NA, CMqueueReqSend_Post_Size_NA }
};

static const uint8_t
CMqueueReqSend_Map[] = {
  5, 1, 5, 1, 5, 1, 5, 1, 1, 1, 1, 1, 12, 1, 12, 1, 12, 1, 12, 1, 1, 1, 1, 1,
  5, 1, 5, 1, 5, 1, 5, 1, 1, 1, 1, 1, 4, 1, 5, 1, 4, 1, 5, 1, 1, 1, 1, 1, 9, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 1, 6, 1, 6, 1,
  6, 1, 1, 1, 1, 1, 13, 1, 13, 1, 13, 1, 13, 1, 1, 1, 1, 1, 6, 1, 6, 1, 6, 1,
  6, 1, 1, 1, 1, 1, 4, 1, 6, 1, 4, 1, 6, 1, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 7, 1, 7, 1, 7, 1, 7, 1, 1, 1, 1, 1, 14, 1, 14, 1, 14, 1,
  14, 1, 1, 1, 1, 1, 7, 1, 7, 1, 7, 1, 7, 1, 1, 1, 1, 1, 4, 1, 7, 1, 4, 1, 7,
  1, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 8, 1, 8, 1, 8, 1, 8, 1, 1, 1, 1, 1, 15, 1, 15, 1, 15, 1, 15, 1, 1, 1, 1,
  1, 8, 1, 8, 1, 8, 1, 8, 1, 1, 1, 1, 1, 4, 1, 8, 1, 4, 1, 8, 1, 1, 1, 1, 1, 9,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 3, 1, 3,
  1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3,
  1, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 1, 16, 1, 16, 1, 16, 1, 1, 1, 1,
  1, 17, 1, 17, 1, 17, 1, 17, 1, 1, 1, 1, 1, 10, 1, 10, 1, 10, 1, 10, 1, 1, 1,
  1, 1, 4, 1, 10, 1, 4, 1, 10, 1, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 1, 18, 1, 18, 1, 18, 1, 1, 1, 1, 1, 19,
  1, 19, 1, 19, 1, 19, 1, 1, 1, 1, 1, 11, 1, 11, 1, 11, 1, 11, 1, 1, 1, 1, 1,
  4, 1, 11, 1, 4, 1, 11, 1, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 3,
  1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3,
  1, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 20, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 20, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CMqueueReqSend_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqSend_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqSend_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqSend_Fixture = {
  .setup = CMqueueReqSend_Setup_Wrap,
  .stop = NULL,
  .teardown = CMqueueReqSend_Teardown_Wrap,
  .scope = CMqueueReqSend_Scope,
  .initial_context = &CMqueueReqSend_Instance
};

static inline CMqueueReqSend_Entry CMqueueReqSend_PopEntry(
  CMqueueReqSend_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqSend_Entries[ CMqueueReqSend_Map[ index ] ];
}

static void CMqueueReqSend_SetPreConditionStates( CMqueueReqSend_Context *ctx )
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Queue_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqSend_Pre_Queue_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Access_NA ) {
    ctx->Map.pcs[ 4 ] = CMqueueReqSend_Pre_Access_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_NonBlock_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqSend_Pre_NonBlock_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 6 ] = CMqueueReqSend_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }
}

static void CMqueueReqSend_TestVariant( CMqueueReqSend_Context *ctx )
{
  CMqueueReqSend_Pre_MsgPrio_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqSend_Pre_MsgLen_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqSend_Pre_MqDes_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqSend_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqSend_Pre_Access_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqSend_Pre_NonBlock_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqSend_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CMqueueReqSend_Action( ctx );
  CMqueueReqSend_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqSend_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqSend_Post_Message_Check( ctx, ctx->Map.entry.Post_Message );
  CMqueueReqSend_Post_Priority_Check( ctx, ctx->Map.entry.Post_Priority );
  CMqueueReqSend_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
}

/**
 * @fn void T_case_body_CMqueueReqSend( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqSend, &CMqueueReqSend_Fixture )
{
  CMqueueReqSend_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqSend_Pre_MsgPrio_Low;
    ctx->Map.pci[ 0 ] < CMqueueReqSend_Pre_MsgPrio_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqSend_Pre_MsgLen_One;
      ctx->Map.pci[ 1 ] < CMqueueReqSend_Pre_MsgLen_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqSend_Pre_MqDes_Valid;
        ctx->Map.pci[ 2 ] < CMqueueReqSend_Pre_MqDes_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqSend_Pre_Queue_Empty;
          ctx->Map.pci[ 3 ] < CMqueueReqSend_Pre_Queue_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqSend_Pre_Access_ReadWrite;
            ctx->Map.pci[ 4 ] < CMqueueReqSend_Pre_Access_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqSend_Pre_NonBlock_Yes;
              ctx->Map.pci[ 5 ] < CMqueueReqSend_Pre_NonBlock_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CMqueueReqSend_Pre_Open_Yes;
                ctx->Map.pci[ 6 ] < CMqueueReqSend_Pre_Open_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                ctx->Map.entry = CMqueueReqSend_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                CMqueueReqSend_SetPreConditionStates( ctx );
                CMqueueReqSend_Prepare( ctx );
                CMqueueReqSend_TestVariant( ctx );
                CMqueueReqSend_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
