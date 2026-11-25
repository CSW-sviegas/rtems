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
#include <setjmp.h>
#include <string.h>
#include <rtems/posix/mqueueimpl.h>
#include <rtems/score/threaddispatch.h>

#include "tr-tq-enqueue-fifo.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqReceive spec:/c/mqueue/req/receive
 *
 * @ingroup TestsuitesValidationMqueue
 *
 * @{
 */

typedef enum {
  CMqueueReqReceive_Pre_MsgPrio_Obj,
  CMqueueReqReceive_Pre_MsgPrio_Null,
  CMqueueReqReceive_Pre_MsgPrio_NA
} CMqueueReqReceive_Pre_MsgPrio;

typedef enum {
  CMqueueReqReceive_Pre_MsgLen_Zero,
  CMqueueReqReceive_Pre_MsgLen_One,
  CMqueueReqReceive_Pre_MsgLen_Below,
  CMqueueReqReceive_Pre_MsgLen_Max,
  CMqueueReqReceive_Pre_MsgLen_Greater,
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
  CMqueueReqReceive_Pre_StoredSize_Zero,
  CMqueueReqReceive_Pre_StoredSize_One,
  CMqueueReqReceive_Pre_StoredSize_Below,
  CMqueueReqReceive_Pre_StoredSize_Max,
  CMqueueReqReceive_Pre_StoredSize_NA
} CMqueueReqReceive_Pre_StoredSize;

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
  CMqueueReqReceive_Pre_Messages_Zero,
  CMqueueReqReceive_Pre_Messages_Some,
  CMqueueReqReceive_Pre_Messages_Full,
  CMqueueReqReceive_Pre_Messages_NA
} CMqueueReqReceive_Pre_Messages;

typedef enum {
  CMqueueReqReceive_Pre_Waiters_None,
  CMqueueReqReceive_Pre_Waiters_Sender,
  CMqueueReqReceive_Pre_Waiters_NA
} CMqueueReqReceive_Pre_Waiters;

typedef enum {
  CMqueueReqReceive_Pre_Open_Yes,
  CMqueueReqReceive_Pre_Open_No,
  CMqueueReqReceive_Pre_Open_NA
} CMqueueReqReceive_Pre_Open;

typedef enum {
  CMqueueReqReceive_Pre_Context_Task,
  CMqueueReqReceive_Pre_Context_Interrupt,
  CMqueueReqReceive_Pre_Context_NA
} CMqueueReqReceive_Pre_Context;

typedef enum {
  CMqueueReqReceive_Post_Status_Zero,
  CMqueueReqReceive_Post_Status_One,
  CMqueueReqReceive_Post_Status_Below,
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
  CMqueueReqReceive_Post_Buffer_Below,
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

typedef enum {
  CMqueueReqReceive_Post_Wait_Yes,
  CMqueueReqReceive_Post_Wait_No,
  CMqueueReqReceive_Post_Wait_NA
} CMqueueReqReceive_Post_Wait;

typedef enum {
  CMqueueReqReceive_Post_Tasks_Fifo,
  CMqueueReqReceive_Post_Tasks_NA
} CMqueueReqReceive_Post_Tasks;

typedef enum {
  CMqueueReqReceive_Post_FatalError_Yes,
  CMqueueReqReceive_Post_FatalError_Nop,
  CMqueueReqReceive_Post_FatalError_NA
} CMqueueReqReceive_Post_FatalError;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_MsgPrio_NA : 1;
  uint32_t Pre_MsgLen_NA : 1;
  uint32_t Pre_MqDes_NA : 1;
  uint32_t Pre_StoredPrio_NA : 1;
  uint32_t Pre_StoredSize_NA : 1;
  uint32_t Pre_Access_NA : 1;
  uint32_t Pre_NonBlock_NA : 1;
  uint32_t Pre_Messages_NA : 1;
  uint32_t Pre_Waiters_NA : 1;
  uint32_t Pre_Open_NA : 1;
  uint32_t Pre_Context_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Errno : 3;
  uint32_t Post_Buffer : 3;
  uint32_t Post_Priority : 2;
  uint32_t Post_Queue : 2;
  uint32_t Post_Sender : 2;
  uint32_t Post_Wait : 2;
  uint32_t Post_Tasks : 1;
  uint32_t Post_FatalError : 2;
} CMqueueReqReceive_Entry;

#define MSG_SIZE 8

/**
 * @brief Test context for spec:/c/mqueue/req/receive test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;

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
   * @brief This member contains the flags used to create the message queue.
   */
  int oflag;

  /**
   * @brief If this member is true, then the `mqdes` parameter is valid.
   */
  bool valid;

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
   * @brief If this member is true, then the mq_receive() function is called
   *   from within interrupt context.
   */
  bool interrupt;

  /**
   * @brief This member contains the identifier of the task which calls
   *   mq_receive().
   */
  rtems_id worker_id;

  /**
   * @brief This member references the TCB of the task which calls
   *   mq_receive().
   */
  rtems_tcb *worker_tcb;

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
   * @brief If this member is true, then the task which waits to send a message
   *   started its call.
   */
  bool sender_started;

  /**
   * @brief If this member is true, then the call of the task which waits to
   *   send a message returned.
   */
  volatile bool sender_done;

  /**
   * @brief This member contains the return value of the call of the task which
   *   waits to send a message.
   */
  int sender_status;

  /**
   * @brief This member contains the count of fatal errors.
   */
  uint32_t fatal_calls;

  /**
   * @brief This member provides the context to return from a fatal error.
   */
  jmp_buf fatal_context;

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
  { "Zero", "One", "Below", "Max", "Greater", "NA" };

static const char *const CMqueueReqReceive_PreDesc_MqDes[] =
  { "Valid", "Invalid", "NA" };

static const char *const CMqueueReqReceive_PreDesc_StoredPrio[] =
  { "Low", "High", "NA" };

static const char *const CMqueueReqReceive_PreDesc_StoredSize[] =
  { "Zero", "One", "Below", "Max", "NA" };

static const char *const CMqueueReqReceive_PreDesc_Access[] =
  { "ReadWrite", "ReadOnly", "WriteOnly", "NA" };

static const char *const CMqueueReqReceive_PreDesc_NonBlock[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqReceive_PreDesc_Messages[] =
  { "Zero", "Some", "Full", "NA" };

static const char *const CMqueueReqReceive_PreDesc_Waiters[] =
  { "None", "Sender", "NA" };

static const char *const CMqueueReqReceive_PreDesc_Open[] =
  { "Yes", "No", "NA" };

static const char *const CMqueueReqReceive_PreDesc_Context[] =
  { "Task", "Interrupt", "NA" };

static const char *const *const CMqueueReqReceive_PreDesc[] = {
  CMqueueReqReceive_PreDesc_MsgPrio,
  CMqueueReqReceive_PreDesc_MsgLen,
  CMqueueReqReceive_PreDesc_MqDes,
  CMqueueReqReceive_PreDesc_StoredPrio,
  CMqueueReqReceive_PreDesc_StoredSize,
  CMqueueReqReceive_PreDesc_Access,
  CMqueueReqReceive_PreDesc_NonBlock,
  CMqueueReqReceive_PreDesc_Messages,
  CMqueueReqReceive_PreDesc_Waiters,
  CMqueueReqReceive_PreDesc_Open,
  CMqueueReqReceive_PreDesc_Context,
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

static Context *ToContext( TQContext *tq_ctx )
{
  return RTEMS_CONTAINER_OF( tq_ctx, Context, tq_ctx );
}

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

    if ( buffer->size > 0 && content[ 0 ] == byte ) {
      return buffer;
    }

    if ( buffer->size == 0 && byte == RECEIVE_BYTE ) {
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

static void CheckQueueUnchanged( const Context *ctx )
{
  if ( ctx->queue == NULL ) {
    return;
  }

  T_eq_u32( GetPendingCount( ctx ), ctx->pending_count );

  if ( ctx->pending_count > 0 ) {
    T_eq_ptr( FindMessage( ctx, RECEIVE_BYTE ), GetFirstMessage( ctx ) );
  }
}

static void Create( Context *ctx )
{
  static const struct mq_attr attr = {
    .mq_maxmsg = MAX_MSG,
    .mq_msgsize = MSG_SIZE
  };

  ctx->queue_id = mq_open( NAME, O_CREAT | O_EXCL | ctx->oflag, 0, &attr );
  T_assert_ne_u32( ctx->queue_id, (mqd_t) -1 );
  ctx->queue = GetQueue( ctx->queue_id );
  T_assert_not_null( ctx->queue );
}

static void Send( Context *ctx, uint8_t byte, size_t size, unsigned int prio )
{
  char msg[ MSG_SIZE ];
  int  oflag;

  /* Send through a descriptor open for writing which does not block */
  oflag = ctx->queue->oflag;
  ctx->queue->oflag = O_RDWR | O_NONBLOCK;
  memset( msg, byte, sizeof( msg ) );
  T_eq_int( mq_send( ctx->queue_id, msg, size, prio ), 0 );
  ctx->queue->oflag = oflag;
}

static void SendStored( Context *ctx )
{
  Send( ctx, RECEIVE_BYTE, ctx->stored_size, ctx->stored_prio );
}

static void SendFiller( Context *ctx )
{
  Send( ctx, FILLER_BYTE, MSG_SIZE, 0 );
}

static void ReceiveAny( Context *ctx )
{
  char buf[ MSG_SIZE ];
  int  oflag;

  /* Receive through a descriptor open for reading which does not block */
  oflag = ctx->queue->oflag;
  ctx->queue->oflag = O_RDWR | O_NONBLOCK;
  T_ge_ssz( mq_receive( ctx->queue_id, buf, sizeof( buf ), NULL ), 0 );
  ctx->queue->oflag = oflag;
}

static void StartSender( Context *ctx )
{
  int oflag;

  /* Let the sender wait on a descriptor open for writing which blocks */
  oflag = ctx->queue->oflag;
  ctx->queue->oflag = O_RDWR;
  ctx->sender_started = true;
  SendEvents( ctx->sender_id, EVENT_SEND );
  WaitForExecutionStop( ctx->sender_id );
  T_false( ctx->sender_done );
  ctx->queue->oflag = oflag;
}

static void ReleaseWorker( Context *ctx )
{
  Send( ctx, FILLER_BYTE, 1, 0 );
  WaitForExecutionStop( ctx->worker_id );
  T_true( ctx->done );
}

static void DoReceive( Context *ctx )
{
  errno = ERRNO_POISON;
  ctx->status = mq_receive(
    ctx->mqdes,
    ctx->buf,
    ctx->msg_len,
    ctx->msg_prio
  );
  ctx->errno_value = errno;
}

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context *ctx;

  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_ulong( code, INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL );

  ctx = arg;
  ++ctx->fatal_calls;
  longjmp( ctx->fatal_context, 1 );
}

static void ReceiveWithinISR( void *arg )
{
  Context *ctx;

  ctx = arg;
  SetFatalHandler( Fatal, ctx );

  if ( setjmp( ctx->fatal_context ) == 0 ) {
    DoReceive( ctx );
    ctx->done = true;
  } else {
    /*
     * The call may have enqueued the interrupted thread on the thread queue
     * of the message queue before the fatal error.  A message for the
     * waiting thread removes it from the thread queue.
     */
    if ( GetExecuting()->Wait.queue != NULL ) {
      Send( ctx, FILLER_BYTE, 1, 0 );
    }
    _Thread_Dispatch_unnest( _Per_CPU_Get() );
  }

  SetFatalHandler( NULL, NULL );
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    SuspendSelf();
    DoReceive( ctx );
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
    ctx->sender_status = mq_send( ctx->queue_id, msg, MSG_SIZE, 0 );
    ctx->sender_done = true;
  }
}

static Status_Control Enqueue( TQContext *tq_ctx, TQWait wait )
{
  Context *ctx;
  char     buf[ MSG_SIZE ];
  ssize_t  n;

  ctx = ToContext( tq_ctx );
  (void) wait;
  n = mq_receive( ctx->queue_id, buf, sizeof( buf ), NULL );

  return STATUS_BUILD( 0, n < 0 ? errno : 0 );
}

static void EnqueueDone( TQContext *tq_ctx )
{
  Context *ctx;
  uint32_t i;

  ctx = ToContext( tq_ctx );

  for ( i = 0; i < tq_ctx->how_many; ++i ) {
    Send( ctx, FILLER_BYTE, 1, 0 );
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
    case CMqueueReqReceive_Pre_MsgLen_Zero: {
      /*
       * While the `msg_len` parameter is zero.
       */
      ctx->msg_len = 0;
      break;
    }

    case CMqueueReqReceive_Pre_MsgLen_One: {
      /*
       * While the `msg_len` parameter is one.
       */
      ctx->msg_len = 1;
      break;
    }

    case CMqueueReqReceive_Pre_MsgLen_Below: {
      /*
       * While the `msg_len` parameter is greater than one and less than the
       * maximum message size of the message queue.
       */
      ctx->msg_len = MSG_SIZE - 1;
      break;
    }

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
      ctx->valid = true;
      break;
    }

    case CMqueueReqReceive_Pre_MqDes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->valid = false;
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
       * is MQ_PRIO_MAX minus one.
       */
      ctx->stored_prio = MQ_PRIO_MAX - 1;
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
    case CMqueueReqReceive_Pre_StoredSize_Zero: {
      /*
       * While the message size of the first message of the message queue is
       * zero.
       */
      ctx->stored_size = 0;
      break;
    }

    case CMqueueReqReceive_Pre_StoredSize_One: {
      /*
       * While the message size of the first message of the message queue is
       * one.
       */
      ctx->stored_size = 1;
      break;
    }

    case CMqueueReqReceive_Pre_StoredSize_Below: {
      /*
       * While the message size of the first message of the message queue is
       * greater than one and less than the maximum message size of the message
       * queue.
       */
      ctx->stored_size = MSG_SIZE - 1;
      break;
    }

    case CMqueueReqReceive_Pre_StoredSize_Max: {
      /*
       * While the message size of the first message of the message queue is
       * equal to the maximum message size of the message queue.
       */
      ctx->stored_size = MSG_SIZE;
      break;
    }

    case CMqueueReqReceive_Pre_StoredSize_NA:
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
      ctx->oflag |= O_NONBLOCK;
      break;
    }

    case CMqueueReqReceive_Pre_NonBlock_No: {
      /*
       * While the flags of the message queue do not contain O_NONBLOCK.
       */
      /* O_NONBLOCK is not set */
      break;
    }

    case CMqueueReqReceive_Pre_NonBlock_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_Messages_Prepare(
  CMqueueReqReceive_Context     *ctx,
  CMqueueReqReceive_Pre_Messages state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Messages_Zero: {
      /*
       * While the message queue contains no message.
       */
      Create( ctx );
      break;
    }

    case CMqueueReqReceive_Pre_Messages_Some: {
      /*
       * While the message queue contains at least one message and is not full.
       */
      Create( ctx );
      SendStored( ctx );
      break;
    }

    case CMqueueReqReceive_Pre_Messages_Full: {
      /*
       * While the message queue is full.
       */
      Create( ctx );
      SendStored( ctx );
      SendFiller( ctx );
      break;
    }

    case CMqueueReqReceive_Pre_Messages_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_Waiters_Prepare(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Pre_Waiters state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Waiters_None: {
      /*
       * While no thread waits to send a message to the message queue.
       */
      /* No thread waits */
      break;
    }

    case CMqueueReqReceive_Pre_Waiters_Sender: {
      /*
       * While a thread waits to send a message to the message queue.
       */
      StartSender( ctx );
      break;
    }

    case CMqueueReqReceive_Pre_Waiters_NA:
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

static void CMqueueReqReceive_Pre_Context_Prepare(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Pre_Context state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Context_Task: {
      /*
       * While the mq_receive() function is called from within task context.
       */
      ctx->interrupt = false;
      break;
    }

    case CMqueueReqReceive_Pre_Context_Interrupt: {
      /*
       * While the mq_receive() function is called from within interrupt
       * context.
       */
      ctx->interrupt = true;
      break;
    }

    case CMqueueReqReceive_Pre_Context_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Status_Check(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Status_Zero: {
      /*
       * The return value of mq_receive() shall be zero.
       */
      T_eq_ssz( ctx->status, 0 );
      break;
    }

    case CMqueueReqReceive_Post_Status_One: {
      /*
       * The return value of mq_receive() shall be one.
       */
      T_eq_ssz( ctx->status, 1 );
      break;
    }

    case CMqueueReqReceive_Post_Status_Below: {
      /*
       * The return value of mq_receive() shall be the message size of the
       * first message, which is greater than one and less than the maximum
       * message size.
       */
      T_eq_ssz( ctx->status, MSG_SIZE - 1 );
      break;
    }

    case CMqueueReqReceive_Post_Status_Max: {
      /*
       * The return value of mq_receive() shall be the maximum message size of
       * the message queue.
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
       * first message of one byte followed by the bytes of the buffer before
       * the call.
       */
      CheckBuffer( ctx, 1 );
      break;
    }

    case CMqueueReqReceive_Post_Buffer_Below: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall consist of the
       * first message, which is greater than one byte and less than the
       * maximum message size, followed by the bytes of the buffer before the
       * call.
       */
      CheckBuffer( ctx, MSG_SIZE - 1 );
      break;
    }

    case CMqueueReqReceive_Post_Buffer_Max: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall consist of the
       * first message of the maximum message size followed by the bytes of the
       * buffer before the call.
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
       * The object referenced by the `msg_prio` parameter shall be MQ_PRIO_MAX
       * minus one.
       */
      T_eq_uint( ctx->prio_obj, MQ_PRIO_MAX - 1 );
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
       * The first message shall be removed from the message queue.
       */
      T_null( FindMessage( ctx, RECEIVE_BYTE ) );
      break;
    }

    case CMqueueReqReceive_Post_Queue_Nop: {
      /*
       * The message queue shall not be modified by the mq_receive() call.
       */
      CheckQueueUnchanged( ctx );
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
       * to the end of the message queue, and the call of this thread shall
       * return zero.
       */
      T_true( ctx->sender_done );
      T_eq_int( ctx->sender_status, 0 );
      T_eq_ptr( FindMessage( ctx, SENDER_BYTE ), GetLastMessage( ctx ) );
      break;
    }

    case CMqueueReqReceive_Post_Sender_Nop: {
      /*
       * The thread which waits to send a message shall still wait.
       */
      T_false( ctx->sender_done );
      T_null( FindMessage( ctx, SENDER_BYTE ) );
      break;
    }

    case CMqueueReqReceive_Post_Sender_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Wait_Check(
  CMqueueReqReceive_Context  *ctx,
  CMqueueReqReceive_Post_Wait state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Wait_Yes: {
      /*
       * The calling thread shall wait for a message of the message queue.
       */
      T_false( ctx->done );
      T_eq_ptr(
        ctx->worker_tcb->Wait.queue,
        &ctx->queue->Message_queue.Wait_queue.Queue
      );
      break;
    }

    case CMqueueReqReceive_Post_Wait_No: {
      /*
       * The mq_receive() call shall return.
       */
      T_true( ctx->done );
      break;
    }

    case CMqueueReqReceive_Post_Wait_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Tasks_Check(
  CMqueueReqReceive_Context   *ctx,
  CMqueueReqReceive_Post_Tasks state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Tasks_Fifo: {
      /*
       * The calling thread shall be enqueued in FIFO order as specified by
       * spec:/score/tq/req/enqueue-fifo.
       */
      ReleaseWorker( ctx );
      ScoreTqReqEnqueueFifo_Run( &ctx->tq_ctx );
      break;
    }

    case CMqueueReqReceive_Post_Tasks_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_FatalError_Check(
  CMqueueReqReceive_Context        *ctx,
  CMqueueReqReceive_Post_FatalError state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_FatalError_Yes: {
      /*
       * The fatal error with a fatal source of INTERNAL_ERROR_CORE and a fatal
       * code of INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL shall occur
       * by the mq_receive() call.
       */
      T_eq_u32( ctx->fatal_calls, 1 );
      break;
    }

    case CMqueueReqReceive_Post_FatalError_Nop: {
      /*
       * The mq_receive() call shall not cause a fatal error.
       */
      T_eq_u32( ctx->fatal_calls, 0 );
      break;
    }

    case CMqueueReqReceive_Post_FatalError_NA:
      break;
  }
}

static void CMqueueReqReceive_Setup( CMqueueReqReceive_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.enqueue_prepare = TQDoNothing;
  ctx->tq_ctx.enqueue_done = EnqueueDone;
  ctx->tq_ctx.enqueue = Enqueue;
  ctx->tq_ctx.surrender = TQDoNothingSuccessfully;
  ctx->tq_ctx.convert_status = TQConvertStatusPOSIX;
  TQInitialize( &ctx->tq_ctx );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
  WaitForExecutionStop( ctx->worker_id );
  ctx->worker_tcb = GetThread( ctx->worker_id );
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
  TQDestroy( &ctx->tq_ctx );
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
  ctx->queue = NULL;
  ctx->stored_prio = 0;
  ctx->stored_size = 0;
  ctx->oflag = O_RDWR;
  ctx->done = false;
  ctx->sender_started = false;
  ctx->sender_done = false;
  ctx->sender_status = -1;
  ctx->fatal_calls = 0;
  ctx->prio_obj = PRIO_POISON;
  ctx->pending_count = 0;
  memset( ctx->buf, BUFFER_POISON, sizeof( ctx->buf ) );
}

static void CMqueueReqReceive_Action( CMqueueReqReceive_Context *ctx )
{
  if ( ctx->valid ) {
    ctx->mqdes = ctx->queue_id;
    ctx->pending_count = GetPendingCount( ctx );
  } else {
    ctx->mqdes = (mqd_t) INVALID_ID;
  }

  if ( ctx->interrupt ) {
    CallWithinISR( ReceiveWithinISR, ctx );
  } else {
    ResumeTask( ctx->worker_id );
    WaitForExecutionStop( ctx->worker_id );
  }

  if ( ctx->sender_started ) {
    WaitForExecutionStop( ctx->sender_id );
  }
}

static void CMqueueReqReceive_Cleanup( CMqueueReqReceive_Context *ctx )
{
  if ( !ctx->interrupt && ctx->valid && !ctx->done ) {
    ReleaseWorker( ctx );
  }

  if ( ctx->queue != NULL ) {
    if ( ctx->queue->open_count == 0 ) {
      T_eq_u32( mq_open( NAME, O_RDWR ), ctx->queue_id );
    }

    if ( ctx->sender_started && !ctx->sender_done ) {
      ReceiveAny( ctx );
      WaitForExecutionStop( ctx->sender_id );
      T_true( ctx->sender_done );
    }

    T_eq_int( mq_close( ctx->queue_id ), 0 );
    T_eq_int( mq_unlink( NAME ), 0 );
    ctx->queue = NULL;
  }
}

/* clang-format off */

static const CMqueueReqReceive_Entry
CMqueueReqReceive_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_NA,
    CMqueueReqReceive_Post_Errno_NA, CMqueueReqReceive_Post_Buffer_NA,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_NA,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_NA,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_Nop, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_Nop, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_Nop, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_Nop, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_MsgSize, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_Low, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Zero,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_One,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_One,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Below,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Below,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Max,
    CMqueueReqReceive_Post_Errno_Nop, CMqueueReqReceive_Post_Buffer_Max,
    CMqueueReqReceive_Post_Priority_High, CMqueueReqReceive_Post_Queue_Removed,
    CMqueueReqReceive_Post_Sender_Moved, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_NA,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Badf, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_NA,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Again, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_Nop, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_NA,
    CMqueueReqReceive_Post_Errno_NA, CMqueueReqReceive_Post_Buffer_NA,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_Yes,
    CMqueueReqReceive_Post_Tasks_Fifo, CMqueueReqReceive_Post_FatalError_Nop },
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_NA,
    CMqueueReqReceive_Post_Errno_NA, CMqueueReqReceive_Post_Buffer_NA,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_NA,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Yes },
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_Again, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Priority_NA, CMqueueReqReceive_Post_Queue_Nop,
    CMqueueReqReceive_Post_Sender_NA, CMqueueReqReceive_Post_Wait_No,
    CMqueueReqReceive_Post_Tasks_NA, CMqueueReqReceive_Post_FatalError_Nop }
};

static const uint8_t
CMqueueReqReceive_Map[] = {
  27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  13, 13, 13, 13, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3,
  13, 13, 13, 13, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3,
  0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1,
  0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0,
  0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0,
  8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8,
  2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
  1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1,
  0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0,
  0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0,
  8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2,
  2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
  1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1,
  0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0,
  0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2,
  2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2,
  9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 53, 53, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9,
  9, 3, 3, 27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9,
  9, 3, 3, 27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9,
  9, 3, 3, 27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9,
  9, 3, 3, 13, 13, 13, 13, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3,
  3, 3, 3, 13, 13, 13, 13, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3,
  3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1,
  1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0,
  0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0,
  0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2,
  2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2,
  2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9,
  9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1,
  1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0,
  0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2,
  2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2,
  2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9,
  9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1,
  1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
  0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8,
  8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2,
  2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9,
  9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 53, 53,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8,
  2, 2, 9, 9, 3, 3, 27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8,
  2, 2, 9, 9, 3, 3, 27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8,
  2, 2, 9, 9, 3, 3, 27, 27, 13, 13, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8,
  2, 2, 9, 9, 3, 3, 13, 13, 13, 13, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2,
  2, 2, 3, 3, 3, 3, 13, 13, 13, 13, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2,
  2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2,
  9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
  1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1,
  0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8,
  2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2,
  9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
  1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0,
  8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8,
  2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2,
  9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0,
  0, 0, 8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0,
  8, 8, 2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8,
  2, 2, 9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 0, 0, 0, 0, 8, 8, 2, 2,
  9, 9, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3,
  53, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 55, 13, 13, 0, 0, 0, 0, 29, 29, 1, 1, 0, 0, 0,
  0, 30, 30, 2, 2, 31, 31, 3, 3, 56, 57, 13, 13, 0, 0, 0, 0, 29, 29, 1, 1, 0,
  0, 0, 0, 30, 30, 2, 2, 31, 31, 3, 3, 55, 55, 13, 13, 0, 0, 0, 0, 29, 29, 1,
  1, 0, 0, 0, 0, 30, 30, 2, 2, 31, 31, 3, 3, 56, 57, 13, 13, 0, 0, 0, 0, 29,
  29, 1, 1, 0, 0, 0, 0, 30, 30, 2, 2, 31, 31, 3, 3, 13, 13, 13, 13, 0, 0, 0, 0,
  1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 13, 13, 13, 13, 0, 0, 0, 0,
  1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 32,
  32, 1, 1, 0, 0, 0, 0, 33, 33, 2, 2, 34, 34, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 32,
  32, 1, 1, 0, 0, 0, 0, 33, 33, 2, 2, 34, 34, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 32,
  32, 1, 1, 0, 0, 0, 0, 33, 33, 2, 2, 34, 34, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 32,
  32, 1, 1, 0, 0, 0, 0, 33, 33, 2, 2, 34, 34, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
  1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 1, 1,
  0, 0, 0, 0, 36, 36, 2, 2, 37, 37, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 1, 1,
  0, 0, 0, 0, 36, 36, 2, 2, 37, 37, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 1, 1,
  0, 0, 0, 0, 36, 36, 2, 2, 37, 37, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 1, 1,
  0, 0, 0, 0, 36, 36, 2, 2, 37, 37, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 1, 1, 0, 0, 0,
  0, 39, 39, 2, 2, 40, 40, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 1, 1, 0, 0, 0,
  0, 39, 39, 2, 2, 40, 40, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 1, 1, 0, 0, 0,
  0, 39, 39, 2, 2, 40, 40, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 1, 1, 0, 0, 0,
  0, 39, 39, 2, 2, 40, 40, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2,
  2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 41, 41, 1, 1, 0, 0, 0, 0, 42,
  42, 2, 2, 43, 43, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 41, 41, 1, 1, 0, 0, 0, 0, 42,
  42, 2, 2, 43, 43, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 41, 41, 1, 1, 0, 0, 0, 0, 42,
  42, 2, 2, 43, 43, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 41, 41, 1, 1, 0, 0, 0, 0, 42,
  42, 2, 2, 43, 43, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2,
  2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 44, 44, 1, 1, 0, 0, 0, 0, 45, 45, 2, 2,
  46, 46, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 44, 44, 1, 1, 0, 0, 0, 0, 45, 45, 2, 2,
  46, 46, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 44, 44, 1, 1, 0, 0, 0, 0, 45, 45, 2, 2,
  46, 46, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 44, 44, 1, 1, 0, 0, 0, 0, 45, 45, 2, 2,
  46, 46, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3,
  3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 47, 47, 1, 1, 0, 0, 0, 0, 48, 48, 2, 2, 49, 49, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 47, 47, 1, 1, 0, 0, 0, 0, 48, 48, 2, 2, 49, 49, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 47, 47, 1, 1, 0, 0, 0, 0, 48, 48, 2, 2, 49, 49, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 47, 47, 1, 1, 0, 0, 0, 0, 48, 48, 2, 2, 49, 49, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 50, 50, 1, 1, 0, 0, 0, 0, 51, 51, 2, 2, 52, 52, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 50, 50, 1, 1, 0, 0, 0, 0, 51, 51, 2, 2, 52, 52, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 50, 50, 1, 1, 0, 0, 0, 0, 51, 51, 2, 2, 52, 52, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 50, 50, 1, 1, 0, 0, 0, 0, 51, 51, 2, 2, 52, 52, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 53, 53, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 55, 55, 13, 13, 0, 0, 0, 0, 29, 29, 1, 1, 0, 0, 0, 0, 30, 30, 2, 2, 31,
  31, 3, 3, 56, 57, 13, 13, 0, 0, 0, 0, 29, 29, 1, 1, 0, 0, 0, 0, 30, 30, 2, 2,
  31, 31, 3, 3, 55, 55, 13, 13, 0, 0, 0, 0, 29, 29, 1, 1, 0, 0, 0, 0, 30, 30,
  2, 2, 31, 31, 3, 3, 56, 57, 13, 13, 0, 0, 0, 0, 29, 29, 1, 1, 0, 0, 0, 0, 30,
  30, 2, 2, 31, 31, 3, 3, 13, 13, 13, 13, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  2, 2, 2, 2, 3, 3, 3, 3, 13, 13, 13, 13, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 1, 1, 0, 0, 0, 0, 33,
  33, 2, 2, 34, 34, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 1, 1, 0, 0, 0, 0, 33,
  33, 2, 2, 34, 34, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 1, 1, 0, 0, 0, 0, 33,
  33, 2, 2, 34, 34, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 1, 1, 0, 0, 0, 0, 33,
  33, 2, 2, 34, 34, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2,
  2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 1, 1, 0, 0, 0, 0, 36, 36, 2, 2,
  37, 37, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 1, 1, 0, 0, 0, 0, 36, 36, 2, 2,
  37, 37, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 1, 1, 0, 0, 0, 0, 36, 36, 2, 2,
  37, 37, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 1, 1, 0, 0, 0, 0, 36, 36, 2, 2,
  37, 37, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3,
  3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 1, 1, 0, 0, 0, 0, 39, 39, 2, 2, 40, 40, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 1, 1, 0, 0, 0, 0, 39, 39, 2, 2, 40, 40, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 1, 1, 0, 0, 0, 0, 39, 39, 2, 2, 40, 40, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 1, 1, 0, 0, 0, 0, 39, 39, 2, 2, 40, 40, 3,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 41, 41, 1, 1, 0, 0, 0, 0, 42, 42, 2, 2, 43, 43, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 41, 41, 1, 1, 0, 0, 0, 0, 42, 42, 2, 2, 43, 43, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 41, 41, 1, 1, 0, 0, 0, 0, 42, 42, 2, 2, 43, 43, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 41, 41, 1, 1, 0, 0, 0, 0, 42, 42, 2, 2, 43, 43, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 44, 44, 1, 1, 0, 0, 0, 0, 45, 45, 2, 2, 46, 46, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 44, 44, 1, 1, 0, 0, 0, 0, 45, 45, 2, 2, 46, 46, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 44, 44, 1, 1, 0, 0, 0, 0, 45, 45, 2, 2, 46, 46, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 44, 44, 1, 1, 0, 0, 0, 0, 45, 45, 2, 2, 46, 46, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 47, 47,
  1, 1, 0, 0, 0, 0, 48, 48, 2, 2, 49, 49, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 47, 47,
  1, 1, 0, 0, 0, 0, 48, 48, 2, 2, 49, 49, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 47, 47,
  1, 1, 0, 0, 0, 0, 48, 48, 2, 2, 49, 49, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 47, 47,
  1, 1, 0, 0, 0, 0, 48, 48, 2, 2, 49, 49, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 1, 1, 0,
  0, 0, 0, 51, 51, 2, 2, 52, 52, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 1, 1, 0,
  0, 0, 0, 51, 51, 2, 2, 52, 52, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 1, 1, 0,
  0, 0, 0, 51, 51, 2, 2, 52, 52, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 1, 1, 0,
  0, 0, 0, 51, 51, 2, 2, 52, 52, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
  0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 2, 2, 2, 2, 3, 3, 3, 3, 53, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 28, 14, 14, 0, 0,
  0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 28, 28, 14, 14,
  0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 28, 28, 14,
  14, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 28, 28,
  14, 14, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 14,
  14, 14, 14, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 14,
  14, 14, 14, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0,
  0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
  4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4,
  4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4,
  4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4,
  0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0,
  0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0,
  0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0,
  0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0,
  0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0,
  0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0,
  0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5,
  5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5,
  5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5,
  6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6,
  6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6,
  6, 54, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 28, 14, 14, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0,
  0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 28, 28, 14, 14, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 28, 28, 14, 14, 0, 0, 0, 0, 10, 10,
  4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 28, 28, 14, 14, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 14, 14, 14, 14, 0, 0, 0, 0,
  4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 14, 14, 14, 14, 0, 0, 0, 0,
  4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4,
  4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4,
  4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4,
  0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4,
  0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0,
  0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0,
  0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0,
  0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0,
  0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0,
  0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0,
  0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5,
  5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5,
  5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5,
  6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6,
  6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0,
  0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0,
  0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 54, 54, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 28, 28, 14, 14, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12,
  12, 6, 6, 28, 28, 14, 14, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 28, 28, 14, 14, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11,
  5, 5, 12, 12, 6, 6, 28, 28, 14, 14, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 14, 14, 14, 14, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0,
  5, 5, 5, 5, 6, 6, 6, 6, 14, 14, 14, 14, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0,
  5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11,
  11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5,
  5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5,
  6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5,
  12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6,
  6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0,
  0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0,
  0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  0, 10, 10, 4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4,
  4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
  4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
  4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
  4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
  4, 4, 0, 0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4,
  4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4,
  0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0,
  0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0,
  0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0,
  0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 4, 4, 0,
  0, 0, 0, 11, 11, 5, 5, 12, 12, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0,
  0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0,
  0, 5, 5, 5, 5, 6, 6, 6, 6, 54, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 58, 58, 14, 14, 0, 0,
  0, 0, 15, 15, 4, 4, 0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 56, 57, 14, 14,
  0, 0, 0, 0, 15, 15, 4, 4, 0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 58, 58, 14,
  14, 0, 0, 0, 0, 15, 15, 4, 4, 0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 56, 57,
  14, 14, 0, 0, 0, 0, 15, 15, 4, 4, 0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 14,
  14, 14, 14, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 14,
  14, 14, 14, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0,
  0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
  4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24,
  24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24,
  24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24,
  24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24,
  24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4,
  4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4,
  4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 4, 4,
  0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 4, 4,
  0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 4, 4,
  0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 4, 4,
  0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4,
  0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0,
  0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0,
  0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0,
  0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0,
  0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0,
  0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0,
  0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5,
  5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22,
  22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22,
  22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22,
  22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22,
  22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5,
  5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5,
  6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5,
  26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5,
  26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5,
  26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5,
  26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6,
  6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6,
  6, 54, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 58, 58, 14, 14, 0, 0, 0, 0, 15, 15, 4, 4, 0, 0,
  0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 56, 57, 14, 14, 0, 0, 0, 0, 15, 15, 4, 4,
  0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 58, 58, 14, 14, 0, 0, 0, 0, 15, 15,
  4, 4, 0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 56, 57, 14, 14, 0, 0, 0, 0, 15,
  15, 4, 4, 0, 0, 0, 0, 16, 16, 5, 5, 17, 17, 6, 6, 14, 14, 14, 14, 0, 0, 0, 0,
  4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 14, 14, 14, 14, 0, 0, 0, 0,
  4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18,
  18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18,
  18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18,
  18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18,
  18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5, 20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4,
  4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4,
  4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4,
  0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4,
  0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4,
  0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4,
  0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4,
  0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0,
  0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0,
  0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0,
  0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0,
  0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0,
  0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0,
  0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5,
  5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 4, 4, 0, 0, 0, 0, 16,
  16, 5, 5, 17, 17, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 4, 4, 0, 0, 0, 0, 16,
  16, 5, 5, 17, 17, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 4, 4, 0, 0, 0, 0, 16,
  16, 5, 5, 17, 17, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 4, 4, 0, 0, 0, 0, 16,
  16, 5, 5, 17, 17, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5,
  5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5,
  6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5,
  20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5,
  20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5,
  20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 4, 4, 0, 0, 0, 0, 19, 19, 5, 5,
  20, 20, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6,
  6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 4, 4, 0, 0, 0, 0, 22, 22, 5, 5, 23, 23, 6,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0,
  0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 24, 24, 4, 4, 0, 0, 0, 0, 25, 25, 5, 5, 26, 26, 6, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0,
  0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, 54, 54, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0
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

  if ( ctx->Map.entry.Pre_Access_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqReceive_Pre_Access_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_NonBlock_NA ) {
    ctx->Map.pcs[ 6 ] = CMqueueReqReceive_Pre_NonBlock_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_Messages_NA ) {
    ctx->Map.pcs[ 7 ] = CMqueueReqReceive_Pre_Messages_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_Waiters_NA ) {
    ctx->Map.pcs[ 8 ] = CMqueueReqReceive_Pre_Waiters_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 9 ] = CMqueueReqReceive_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 9 ] = ctx->Map.pci[ 9 ];
  }

  ctx->Map.pcs[ 10 ] = ctx->Map.pci[ 10 ];
}

static void CMqueueReqReceive_TestVariant( CMqueueReqReceive_Context *ctx )
{
  CMqueueReqReceive_Pre_MsgPrio_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqReceive_Pre_MsgLen_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqReceive_Pre_MqDes_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqReceive_Pre_StoredPrio_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqReceive_Pre_StoredSize_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqReceive_Pre_Access_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqReceive_Pre_NonBlock_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CMqueueReqReceive_Pre_Messages_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CMqueueReqReceive_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  CMqueueReqReceive_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  CMqueueReqReceive_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 10 ] );
  CMqueueReqReceive_Action( ctx );
  CMqueueReqReceive_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqReceive_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqReceive_Post_Buffer_Check( ctx, ctx->Map.entry.Post_Buffer );
  CMqueueReqReceive_Post_Priority_Check( ctx, ctx->Map.entry.Post_Priority );
  CMqueueReqReceive_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
  CMqueueReqReceive_Post_Sender_Check( ctx, ctx->Map.entry.Post_Sender );
  CMqueueReqReceive_Post_Wait_Check( ctx, ctx->Map.entry.Post_Wait );
  CMqueueReqReceive_Post_Tasks_Check( ctx, ctx->Map.entry.Post_Tasks );
  CMqueueReqReceive_Post_FatalError_Check(
    ctx,
    ctx->Map.entry.Post_FatalError
  );
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
      ctx->Map.pci[ 1 ] = CMqueueReqReceive_Pre_MsgLen_Zero;
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
            ctx->Map.pci[ 4 ] = CMqueueReqReceive_Pre_StoredSize_Zero;
            ctx->Map.pci[ 4 ] < CMqueueReqReceive_Pre_StoredSize_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqReceive_Pre_Access_ReadWrite;
              ctx->Map.pci[ 5 ] < CMqueueReqReceive_Pre_Access_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CMqueueReqReceive_Pre_NonBlock_Yes;
                ctx->Map.pci[ 6 ] < CMqueueReqReceive_Pre_NonBlock_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CMqueueReqReceive_Pre_Messages_Zero;
                  ctx->Map.pci[ 7 ] < CMqueueReqReceive_Pre_Messages_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = CMqueueReqReceive_Pre_Waiters_None;
                    ctx->Map.pci[ 8 ] < CMqueueReqReceive_Pre_Waiters_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    for (
                      ctx->Map.pci[ 9 ] = CMqueueReqReceive_Pre_Open_Yes;
                      ctx->Map.pci[ 9 ] < CMqueueReqReceive_Pre_Open_NA;
                      ++ctx->Map.pci[ 9 ]
                    ) {
                      for (
                        ctx->Map.pci[ 10 ] =
                          CMqueueReqReceive_Pre_Context_Task;
                        ctx->Map.pci[ 10 ] < CMqueueReqReceive_Pre_Context_NA;
                        ++ctx->Map.pci[ 10 ]
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
  }
}

/** @} */
