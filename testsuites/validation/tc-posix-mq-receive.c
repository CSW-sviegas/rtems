/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqReceive
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
 * @defgroup CMqueueReqReceive spec:/c/mqueue/req/receive
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CMqueueReqReceive_Pre_MsgPrio_Obj,
  CMqueueReqReceive_Pre_MsgPrio_Null,
  CMqueueReqReceive_Pre_MsgPrio_NA
} CMqueueReqReceive_Pre_MsgPrio;

typedef enum {
  CMqueueReqReceive_Pre_MsgLen_Max,
  CMqueueReqReceive_Pre_MsgLen_Greater,
  CMqueueReqReceive_Pre_MsgLen_TooSmall,
  CMqueueReqReceive_Pre_MsgLen_NA
} CMqueueReqReceive_Pre_MsgLen;

typedef enum {
  CMqueueReqReceive_Pre_MqDes_Valid,
  CMqueueReqReceive_Pre_MqDes_Invalid,
  CMqueueReqReceive_Pre_MqDes_NA
} CMqueueReqReceive_Pre_MqDes;

typedef enum {
  CMqueueReqReceive_Pre_StoredPrio_Low,
  CMqueueReqReceive_Pre_StoredPrio_High,
  CMqueueReqReceive_Pre_StoredPrio_NA
} CMqueueReqReceive_Pre_StoredPrio;

typedef enum {
  CMqueueReqReceive_Pre_StoredSize_One,
  CMqueueReqReceive_Pre_StoredSize_Max,
  CMqueueReqReceive_Pre_StoredSize_NA
} CMqueueReqReceive_Pre_StoredSize;

typedef enum {
  CMqueueReqReceive_Pre_Queue_Empty,
  CMqueueReqReceive_Pre_Queue_Pending,
  CMqueueReqReceive_Pre_Queue_Sender,
  CMqueueReqReceive_Pre_Queue_NA
} CMqueueReqReceive_Pre_Queue;

typedef enum {
  CMqueueReqReceive_Pre_Access_ReadWrite,
  CMqueueReqReceive_Pre_Access_ReadOnly,
  CMqueueReqReceive_Pre_Access_WriteOnly,
  CMqueueReqReceive_Pre_Access_NA
} CMqueueReqReceive_Pre_Access;

typedef enum {
  CMqueueReqReceive_Pre_NonBlock_Yes,
  CMqueueReqReceive_Pre_NonBlock_No,
  CMqueueReqReceive_Pre_NonBlock_NA
} CMqueueReqReceive_Pre_NonBlock;

typedef enum {
  CMqueueReqReceive_Pre_Open_Yes,
  CMqueueReqReceive_Pre_Open_No,
  CMqueueReqReceive_Pre_Open_NA
} CMqueueReqReceive_Pre_Open;

typedef enum {
  CMqueueReqReceive_Post_Status_One,
  CMqueueReqReceive_Post_Status_Max,
  CMqueueReqReceive_Post_Status_MinusOne,
  CMqueueReqReceive_Post_Status_NA
} CMqueueReqReceive_Post_Status;

typedef enum {
  CMqueueReqReceive_Post_Errno_Nop,
  CMqueueReqReceive_Post_Errno_Badf,
  CMqueueReqReceive_Post_Errno_MsgSize,
  CMqueueReqReceive_Post_Errno_Again,
  CMqueueReqReceive_Post_Errno_NA
} CMqueueReqReceive_Post_Errno;

typedef enum {
  CMqueueReqReceive_Post_Buffer_One,
  CMqueueReqReceive_Post_Buffer_Max,
  CMqueueReqReceive_Post_Buffer_Nop,
  CMqueueReqReceive_Post_Buffer_NA
} CMqueueReqReceive_Post_Buffer;

typedef enum {
  CMqueueReqReceive_Post_Priority_Low,
  CMqueueReqReceive_Post_Priority_High,
  CMqueueReqReceive_Post_Priority_Nop,
  CMqueueReqReceive_Post_Priority_NA
} CMqueueReqReceive_Post_Priority;

typedef enum {
  CMqueueReqReceive_Post_Queue_Removed,
  CMqueueReqReceive_Post_Queue_Nop,
  CMqueueReqReceive_Post_Queue_NA
} CMqueueReqReceive_Post_Queue;

typedef enum {
  CMqueueReqReceive_Post_Sender_Moved,
  CMqueueReqReceive_Post_Sender_Nop,
  CMqueueReqReceive_Post_Sender_NA
} CMqueueReqReceive_Post_Sender;

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
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 3;
  uint32_t Post_Buffer : 2;
  uint32_t Post_Priority : 2;
  uint32_t Post_Queue : 2;
  uint32_t Post_Sender : 2;
} CMqueueReqReceive_Entry;

#define MSG_SIZE 4

typedef struct {
  volatile bool done;
} Sender;

/**
 * @brief Test context for spec:/c/mqueue/req/receive test case.
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
   *   mq_receive() call.
   */
  uint32_t pending_count;

  /**
   * @brief If this member is true, then the message queue was empty.
   */
  bool empty;

  /**
   * @brief This member contains the identifier of the task which calls
   *   mq_receive().
   */
  rtems_id worker_id;

  /**
   * @brief If this member is true, then the mq_receive() call returned.
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
   *   mq_receive() call.
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
   * @brief This member contains the return value of the mq_receive() call.
   */
  ssize_t status;

  /**
   * @brief This member contains the value of errno after the mq_receive()
   *   call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 9 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 9 ];

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
    CMqueueReqReceive_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqReceive_Context;

static CMqueueReqReceive_Context CMqueueReqReceive_Instance;

static const char *const CMqueueReqReceive_PreDesc_MsgPrio[] =
  { "Obj", "Null", "NA" };

static const char *const CMqueueReqReceive_PreDesc_MsgLen[] =
  { "Max", "Greater", "TooSmall", "NA" };

static const char *const CMqueueReqReceive_PreDesc_MqDes[] =
  { "Valid", "Invalid", "NA" };

static const char *const CMqueueReqReceive_PreDesc_StoredPrio[] =
  { "Low", "High", "NA" };

static const char *const CMqueueReqReceive_PreDesc_StoredSize[] =
  { "One", "Max", "NA" };

static const char *const CMqueueReqReceive_PreDesc_Queue[] =
  { "Empty", "Pending", "Sender", "NA" };

static const char *const CMqueueReqReceive_PreDesc_Access[] =
  { "ReadWrite", "ReadOnly", "WriteOnly", "NA" };

static const char *const CMqueueReqReceive_PreDesc_NonBlock[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqReceive_PreDesc_Open[] =
  { "Yes", "No", "NA" };

static const char *const *const CMqueueReqReceive_PreDesc[] = {
  CMqueueReqReceive_PreDesc_MsgPrio,
  CMqueueReqReceive_PreDesc_MsgLen,
  CMqueueReqReceive_PreDesc_MqDes,
  CMqueueReqReceive_PreDesc_StoredPrio,
  CMqueueReqReceive_PreDesc_StoredSize,
  CMqueueReqReceive_PreDesc_Queue,
  CMqueueReqReceive_PreDesc_Access,
  CMqueueReqReceive_PreDesc_NonBlock,
  CMqueueReqReceive_PreDesc_Open,
  NULL
};

typedef CMqueueReqReceive_Context Context;

#define NAME "/mq-receive"

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
    ctx->status = mq_receive(
      ctx->mqdes,
      ctx->buf,
      ctx->msg_len,
      ctx->msg_prio
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

static void CMqueueReqReceive_Pre_MsgPrio_Prepare(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Pre_MsgPrio state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_MsgPrio_Obj: {
      /*
       * While the `msg_prio` parameter references an object of type unsigned
       * int.
       */
      ctx->msg_prio = &ctx->prio_obj;
      break;
    }

    case CMqueueReqReceive_Pre_MsgPrio_Null: {
      /*
       * While the `msg_prio` parameter is NULL.
       */
      ctx->msg_prio = NULL;
      break;
    }

    case CMqueueReqReceive_Pre_MsgPrio_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_MsgLen_Prepare(
  CMqueueReqReceive_Context   *ctx,
  CMqueueReqReceive_Pre_MsgLen state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_MsgLen_Max: {
      /*
       * While the `msg_len` parameter is equal to the maximum message size of
       * the message queue.
       */
      ctx->msg_len = MSG_SIZE;
      break;
    }

    case CMqueueReqReceive_Pre_MsgLen_Greater: {
      /*
       * While the `msg_len` parameter is greater than the maximum message size
       * of the message queue.
       */
      ctx->msg_len = MSG_SIZE + 1;
      break;
    }

    case CMqueueReqReceive_Pre_MsgLen_TooSmall: {
      /*
       * While the `msg_len` parameter is less than the maximum message size of
       * the message queue.
       */
      ctx->msg_len = MSG_SIZE - 1;
      break;
    }

    case CMqueueReqReceive_Pre_MsgLen_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_MqDes_Prepare(
  CMqueueReqReceive_Context  *ctx,
  CMqueueReqReceive_Pre_MqDes state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_MqDes_Valid: {
      /*
       * While the `mqdes` parameter is the message queue descriptor of a
       * message queue.
       */
      ctx->mqdes = ctx->queue_id;
      break;
    }

    case CMqueueReqReceive_Pre_MqDes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = (mqd_t) INVALID_ID;
      break;
    }

    case CMqueueReqReceive_Pre_MqDes_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_StoredPrio_Prepare(
  CMqueueReqReceive_Context       *ctx,
  CMqueueReqReceive_Pre_StoredPrio state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_StoredPrio_Low: {
      /*
       * While the message priority of the first message of the message queue
       * is zero.
       */
      ctx->stored_prio = 0;
      break;
    }

    case CMqueueReqReceive_Pre_StoredPrio_High: {
      /*
       * While the message priority of the first message of the message queue
       * is greater than zero.
       */
      ctx->stored_prio = MQ_PRIO_MAX;
      break;
    }

    case CMqueueReqReceive_Pre_StoredPrio_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_StoredSize_Prepare(
  CMqueueReqReceive_Context       *ctx,
  CMqueueReqReceive_Pre_StoredSize state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_StoredSize_One: {
      /*
       * While the message size of the first message of the message queue is
       * one.
       */
      ctx->stored_size = 1;
      break;
    }

    case CMqueueReqReceive_Pre_StoredSize_Max: {
      /*
       * While the message size of the first message of the message queue is
       * greater than one.
       */
      ctx->stored_size = MSG_SIZE;
      break;
    }

    case CMqueueReqReceive_Pre_StoredSize_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_Queue_Prepare(
  CMqueueReqReceive_Context  *ctx,
  CMqueueReqReceive_Pre_Queue state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Queue_Empty: {
      /*
       * While the message queue contains no message and a thread sends the
       * first message to the message queue when the calling thread waits for a
       * message.
       */
      ctx->empty = true;
      break;
    }

    case CMqueueReqReceive_Pre_Queue_Pending: {
      /*
       * While the message queue contains messages and no thread waits to send
       * a message to the message queue.
       */
      SendStored( ctx );
      SendFiller( ctx );
      break;
    }

    case CMqueueReqReceive_Pre_Queue_Sender: {
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

    case CMqueueReqReceive_Pre_Queue_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_Access_Prepare(
  CMqueueReqReceive_Context   *ctx,
  CMqueueReqReceive_Pre_Access state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Access_ReadWrite: {
      /*
       * While the message queue is open for reading and writing.
       */
      ctx->oflag = O_RDWR;
      break;
    }

    case CMqueueReqReceive_Pre_Access_ReadOnly: {
      /*
       * While the message queue is open for reading only.
       */
      ctx->oflag = O_RDONLY;
      break;
    }

    case CMqueueReqReceive_Pre_Access_WriteOnly: {
      /*
       * While the message queue is open for writing only.
       */
      ctx->oflag = O_WRONLY;
      break;
    }

    case CMqueueReqReceive_Pre_Access_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_NonBlock_Prepare(
  CMqueueReqReceive_Context     *ctx,
  CMqueueReqReceive_Pre_NonBlock state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_NonBlock_Yes: {
      /*
       * While the flags of the message queue contain O_NONBLOCK.
       */
      SetFlags( ctx, ctx->oflag | O_NONBLOCK );
      break;
    }

    case CMqueueReqReceive_Pre_NonBlock_No: {
      /*
       * While the flags of the message queue do not contain O_NONBLOCK.
       */
      SetFlags( ctx, ctx->oflag );
      break;
    }

    case CMqueueReqReceive_Pre_NonBlock_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_Open_Prepare(
  CMqueueReqReceive_Context *ctx,
  CMqueueReqReceive_Pre_Open state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Open_Yes: {
      /*
       * While the message queue is open.
       */
      /* The message queue was opened by its creation */
      break;
    }

    case CMqueueReqReceive_Pre_Open_No: {
      /*
       * While the message queue is not open.
       */
      T_eq_int( mq_close( ctx->queue_id ), 0 );
      break;
    }

    case CMqueueReqReceive_Pre_Open_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Status_Check(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Status_One: {
      /*
       * The return value of mq_receive() shall be one.
       */
      T_eq_ssz( ctx->status, 1 );
      break;
    }

    case CMqueueReqReceive_Post_Status_Max: {
      /*
       * The return value of mq_receive() shall be the message size of the
       * received message.
       */
      T_eq_ssz( ctx->status, MSG_SIZE );
      break;
    }

    case CMqueueReqReceive_Post_Status_MinusOne: {
      /*
       * The return value of mq_receive() shall be minus one.
       */
      T_eq_ssz( ctx->status, -1 );
      break;
    }

    case CMqueueReqReceive_Post_Status_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Errno_Check(
  CMqueueReqReceive_Context   *ctx,
  CMqueueReqReceive_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified by the mq_receive() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CMqueueReqReceive_Post_Errno_Badf: {
      /*
       * The value of errno shall be EBADF.
       */
      T_eq_int( ctx->errno_value, EBADF );
      break;
    }

    case CMqueueReqReceive_Post_Errno_MsgSize: {
      /*
       * The value of errno shall be EMSGSIZE.
       */
      T_eq_int( ctx->errno_value, EMSGSIZE );
      break;
    }

    case CMqueueReqReceive_Post_Errno_Again: {
      /*
       * The value of errno shall be EAGAIN.
       */
      T_eq_int( ctx->errno_value, EAGAIN );
      break;
    }

    case CMqueueReqReceive_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Buffer_Check(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Post_Buffer state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Buffer_One: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall consist of the
       * received message of one byte followed by the bytes of the buffer
       * before the call.
       */
      CheckBuffer( ctx, 1 );
      break;
    }

    case CMqueueReqReceive_Post_Buffer_Max: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall consist of the
       * received message of the maximum message size followed by the bytes of
       * the buffer before the call.
       */
      CheckBuffer( ctx, MSG_SIZE );
      break;
    }

    case CMqueueReqReceive_Post_Buffer_Nop: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall not be
       * modified.
       */
      CheckBuffer( ctx, 0 );
      break;
    }

    case CMqueueReqReceive_Post_Buffer_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Priority_Check(
  CMqueueReqReceive_Context      *ctx,
  CMqueueReqReceive_Post_Priority state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Priority_Low: {
      /*
       * The object referenced by the `msg_prio` parameter shall be zero.
       */
      T_eq_uint( ctx->prio_obj, 0 );
      break;
    }

    case CMqueueReqReceive_Post_Priority_High: {
      /*
       * The object referenced by the `msg_prio` parameter shall be the message
       * priority of the received message.
       */
      T_eq_uint( ctx->prio_obj, MQ_PRIO_MAX );
      break;
    }

    case CMqueueReqReceive_Post_Priority_Nop: {
      /*
       * The object referenced by the `msg_prio` parameter shall not be
       * modified.
       */
      T_eq_uint( ctx->prio_obj, PRIO_POISON );
      break;
    }

    case CMqueueReqReceive_Post_Priority_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Queue_Check(
  CMqueueReqReceive_Context   *ctx,
  CMqueueReqReceive_Post_Queue state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Queue_Removed: {
      /*
       * The received message shall be removed from the message queue.
       */
      T_null( FindMessage( ctx, RECEIVE_BYTE ) );
      T_eq_ptr( FindMessage( ctx, FILLER_BYTE ), GetFirstMessage( ctx ) );
      break;
    }

    case CMqueueReqReceive_Post_Queue_Nop: {
      /*
       * The message queue shall not be modified by the mq_receive() call.
       */
      T_eq_u32( GetPendingCount( ctx ), ctx->pending_count );

      if ( ctx->pending_count > 0 ) {
        T_eq_ptr( FindMessage( ctx, RECEIVE_BYTE ), GetFirstMessage( ctx ) );
      }
      break;
    }

    case CMqueueReqReceive_Post_Queue_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Sender_Check(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Post_Sender state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Sender_Moved: {
      /*
       * The message of the thread which waits to send a message shall be added
       * to the message queue.
       */
      T_true( ctx->sender->done );
      T_eq_ptr( FindMessage( ctx, SENDER_BYTE ), GetLastMessage( ctx ) );
      break;
    }

    case CMqueueReqReceive_Post_Sender_Nop: {
      /*
       * The thread which waits to send a message shall still wait.
       */
      T_false( ctx->sender->done );
      T_null( FindMessage( ctx, SENDER_BYTE ) );
      break;
    }

    case CMqueueReqReceive_Post_Sender_NA:
      break;
  }
}

static void CMqueueReqReceive_Setup( CMqueueReqReceive_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
  WaitForExecutionStop( ctx->worker_id );
  ctx->sender_id = CreateTask( "SEND", PRIO_HIGH );
  StartTask( ctx->sender_id, SenderTask, ctx );
  WaitForExecutionStop( ctx->sender_id );
}

static void CMqueueReqReceive_Setup_Wrap( void *arg )
{
  CMqueueReqReceive_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqReceive_Setup( ctx );
}

static void CMqueueReqReceive_Teardown( CMqueueReqReceive_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  DeleteTask( ctx->sender_id );
  RestoreRunnerPriority();
}

static void CMqueueReqReceive_Teardown_Wrap( void *arg )
{
  CMqueueReqReceive_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqReceive_Teardown( ctx );
}

static void CMqueueReqReceive_Prepare( CMqueueReqReceive_Context *ctx )
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
}

static void CMqueueReqReceive_Action( CMqueueReqReceive_Context *ctx )
{
  ctx->pending_count = GetPendingCount( ctx );
  ResumeTask( ctx->worker_id );
  WaitForExecutionStop( ctx->worker_id );

  if ( !ctx->done ) {
    T_true( ctx->empty );
    SendStored( ctx );
    WaitForExecutionStop( ctx->worker_id );
    T_true( ctx->done );
  }

  if ( ctx->sender_waits ) {
    WaitForExecutionStop( ctx->sender_id );
  }
}

static void CMqueueReqReceive_Cleanup( CMqueueReqReceive_Context *ctx )
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

static const CMqueueReqReceive_Entry
CMqueueReqReceive_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_NA,
    CMqueueReqReceive_Post_Errno_NA, CMqueueReqReceive_Post_Buffer_NA,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_NA,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Again, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Again, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_NA,
    CMqueueReqReceive_Post_Sender_NA },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_NA,
    CMqueueReqReceive_Post_Sender_NA }
};

static const uint8_t
CMqueueReqReceive_Map[] = {
  9, 1, 25, 1, 9, 1, 25, 1, 1, 1, 1, 1, 15, 1, 15, 1, 15, 1, 15, 1, 1, 1, 1, 1,
  16, 3, 16, 3, 16, 3, 16, 3, 3, 3, 3, 3, 9, 1, 26, 1, 9, 1, 26, 1, 1, 1, 1, 1,
  17, 1, 17, 1, 17, 1, 17, 1, 1, 1, 1, 1, 18, 3, 18, 3, 18, 3, 18, 3, 3, 3, 3,
  3, 9, 1, 27, 1, 9, 1, 27, 1, 1, 1, 1, 1, 19, 1, 19, 1, 19, 1, 19, 1, 1, 1, 1,
  1, 20, 3, 20, 3, 20, 3, 20, 3, 3, 3, 3, 3, 9, 1, 28, 1, 9, 1, 28, 1, 1, 1, 1,
  1, 21, 1, 21, 1, 21, 1, 21, 1, 1, 1, 1, 1, 22, 3, 22, 3, 22, 3, 22, 3, 3, 3,
  3, 3, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 1, 25, 1, 9, 1, 25, 1,
  1, 1, 1, 1, 15, 1, 15, 1, 15, 1, 15, 1, 1, 1, 1, 1, 16, 3, 16, 3, 16, 3, 16,
  3, 3, 3, 3, 3, 9, 1, 26, 1, 9, 1, 26, 1, 1, 1, 1, 1, 17, 1, 17, 1, 17, 1, 17,
  1, 1, 1, 1, 1, 18, 3, 18, 3, 18, 3, 18, 3, 3, 3, 3, 3, 9, 1, 27, 1, 9, 1, 27,
  1, 1, 1, 1, 1, 19, 1, 19, 1, 19, 1, 19, 1, 1, 1, 1, 1, 20, 3, 20, 3, 20, 3,
  20, 3, 3, 3, 3, 3, 9, 1, 28, 1, 9, 1, 28, 1, 1, 1, 1, 1, 21, 1, 21, 1, 21, 1,
  21, 1, 1, 1, 1, 1, 22, 3, 22, 3, 22, 3, 22, 3, 3, 3, 3, 3, 29, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5,
  5, 5, 1, 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5,
  1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3,
  3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1,
  7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 5, 5,
  5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 29, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 2, 23, 2, 10, 2, 23, 2, 2, 2, 2, 2, 11,
  2, 11, 2, 11, 2, 11, 2, 2, 2, 2, 2, 12, 4, 12, 4, 12, 4, 12, 4, 4, 4, 4, 4,
  10, 2, 24, 2, 10, 2, 24, 2, 2, 2, 2, 2, 13, 2, 13, 2, 13, 2, 13, 2, 2, 2, 2,
  2, 14, 4, 14, 4, 14, 4, 14, 4, 4, 4, 4, 4, 10, 2, 23, 2, 10, 2, 23, 2, 2, 2,
  2, 2, 11, 2, 11, 2, 11, 2, 11, 2, 2, 2, 2, 2, 12, 4, 12, 4, 12, 4, 12, 4, 4,
  4, 4, 4, 10, 2, 24, 2, 10, 2, 24, 2, 2, 2, 2, 2, 13, 2, 13, 2, 13, 2, 13, 2,
  2, 2, 2, 2, 14, 4, 14, 4, 14, 4, 14, 4, 4, 4, 4, 4, 30, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 10, 2, 23, 2, 10, 2, 23, 2, 2, 2, 2, 2, 11, 2, 11, 2, 11,
  2, 11, 2, 2, 2, 2, 2, 12, 4, 12, 4, 12, 4, 12, 4, 4, 4, 4, 4, 10, 2, 24, 2,
  10, 2, 24, 2, 2, 2, 2, 2, 13, 2, 13, 2, 13, 2, 13, 2, 2, 2, 2, 2, 14, 4, 14,
  4, 14, 4, 14, 4, 4, 4, 4, 4, 10, 2, 23, 2, 10, 2, 23, 2, 2, 2, 2, 2, 11, 2,
  11, 2, 11, 2, 11, 2, 2, 2, 2, 2, 12, 4, 12, 4, 12, 4, 12, 4, 4, 4, 4, 4, 10,
  2, 24, 2, 10, 2, 24, 2, 2, 2, 2, 2, 13, 2, 13, 2, 13, 2, 13, 2, 2, 2, 2, 2,
  14, 4, 14, 4, 14, 4, 14, 4, 4, 4, 4, 4, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2,
  8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6, 6,
  6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 6, 6, 6, 6,
  6, 6, 6, 6, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 8, 8, 8, 8, 8, 8,
  8, 8, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6,
  2, 2, 2, 2, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 30, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CMqueueReqReceive_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqReceive_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqReceive_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqReceive_Fixture = {
  .setup = CMqueueReqReceive_Setup_Wrap,
  .stop = NULL,
  .teardown = CMqueueReqReceive_Teardown_Wrap,
  .scope = CMqueueReqReceive_Scope,
  .initial_context = &CMqueueReqReceive_Instance
};

static inline CMqueueReqReceive_Entry CMqueueReqReceive_PopEntry(
  CMqueueReqReceive_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqReceive_Entries[ CMqueueReqReceive_Map[ index ] ];
}

static void CMqueueReqReceive_SetPreConditionStates(
  CMqueueReqReceive_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_StoredPrio_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqReceive_Pre_StoredPrio_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_StoredSize_NA ) {
    ctx->Map.pcs[ 4 ] = CMqueueReqReceive_Pre_StoredSize_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Queue_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqReceive_Pre_Queue_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Access_NA ) {
    ctx->Map.pcs[ 6 ] = CMqueueReqReceive_Pre_Access_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_NonBlock_NA ) {
    ctx->Map.pcs[ 7 ] = CMqueueReqReceive_Pre_NonBlock_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 8 ] = CMqueueReqReceive_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }
}

static void CMqueueReqReceive_TestVariant( CMqueueReqReceive_Context *ctx )
{
  CMqueueReqReceive_Pre_MsgPrio_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqReceive_Pre_MsgLen_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqReceive_Pre_MqDes_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqReceive_Pre_StoredPrio_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqReceive_Pre_StoredSize_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqReceive_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqReceive_Pre_Access_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CMqueueReqReceive_Pre_NonBlock_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CMqueueReqReceive_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  CMqueueReqReceive_Action( ctx );
  CMqueueReqReceive_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqReceive_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqReceive_Post_Buffer_Check( ctx, ctx->Map.entry.Post_Buffer );
  CMqueueReqReceive_Post_Priority_Check( ctx, ctx->Map.entry.Post_Priority );
  CMqueueReqReceive_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
  CMqueueReqReceive_Post_Sender_Check( ctx, ctx->Map.entry.Post_Sender );
}

/**
 * @fn void T_case_body_CMqueueReqReceive( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqReceive, &CMqueueReqReceive_Fixture )
{
  CMqueueReqReceive_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqReceive_Pre_MsgPrio_Obj;
    ctx->Map.pci[ 0 ] < CMqueueReqReceive_Pre_MsgPrio_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqReceive_Pre_MsgLen_Max;
      ctx->Map.pci[ 1 ] < CMqueueReqReceive_Pre_MsgLen_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqReceive_Pre_MqDes_Valid;
        ctx->Map.pci[ 2 ] < CMqueueReqReceive_Pre_MqDes_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqReceive_Pre_StoredPrio_Low;
          ctx->Map.pci[ 3 ] < CMqueueReqReceive_Pre_StoredPrio_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqReceive_Pre_StoredSize_One;
            ctx->Map.pci[ 4 ] < CMqueueReqReceive_Pre_StoredSize_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqReceive_Pre_Queue_Empty;
              ctx->Map.pci[ 5 ] < CMqueueReqReceive_Pre_Queue_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CMqueueReqReceive_Pre_Access_ReadWrite;
                ctx->Map.pci[ 6 ] < CMqueueReqReceive_Pre_Access_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CMqueueReqReceive_Pre_NonBlock_Yes;
                  ctx->Map.pci[ 7 ] < CMqueueReqReceive_Pre_NonBlock_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = CMqueueReqReceive_Pre_Open_Yes;
                    ctx->Map.pci[ 8 ] < CMqueueReqReceive_Pre_Open_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    ctx->Map.entry = CMqueueReqReceive_PopEntry( ctx );

                    if ( ctx->Map.entry.Skip ) {
                      continue;
                    }

                    CMqueueReqReceive_SetPreConditionStates( ctx );
                    CMqueueReqReceive_Prepare( ctx );
                    CMqueueReqReceive_TestVariant( ctx );
                    CMqueueReqReceive_Cleanup( ctx );
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
