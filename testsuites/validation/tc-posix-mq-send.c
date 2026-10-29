/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqSend
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
#include <fcntl.h>
#include <limits.h>
#include <mqueue.h>
#include <string.h>

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
  CMqueueReqSend_Pre_MsgPrio_Valid,
  CMqueueReqSend_Pre_MsgPrio_Invalid,
  CMqueueReqSend_Pre_MsgPrio_NA
} CMqueueReqSend_Pre_MsgPrio;

typedef enum {
  CMqueueReqSend_Pre_Mqdes_Valid,
  CMqueueReqSend_Pre_Mqdes_ReadOnly,
  CMqueueReqSend_Pre_Mqdes_Closed,
  CMqueueReqSend_Pre_Mqdes_Invalid,
  CMqueueReqSend_Pre_Mqdes_NA
} CMqueueReqSend_Pre_Mqdes;

typedef enum {
  CMqueueReqSend_Pre_MsgLen_Fits,
  CMqueueReqSend_Pre_MsgLen_TooLong,
  CMqueueReqSend_Pre_MsgLen_NA
} CMqueueReqSend_Pre_MsgLen;

typedef enum {
  CMqueueReqSend_Pre_Queue_Empty,
  CMqueueReqSend_Pre_Queue_Receiver,
  CMqueueReqSend_Pre_Queue_Full,
  CMqueueReqSend_Pre_Queue_NA
} CMqueueReqSend_Pre_Queue;

typedef enum {
  CMqueueReqSend_Pre_Blocking_Yes,
  CMqueueReqSend_Pre_Blocking_No,
  CMqueueReqSend_Pre_Blocking_NA
} CMqueueReqSend_Pre_Blocking;

typedef enum {
  CMqueueReqSend_Post_Status_Zero,
  CMqueueReqSend_Post_Status_MinusOne,
  CMqueueReqSend_Post_Status_NA
} CMqueueReqSend_Post_Status;

typedef enum {
  CMqueueReqSend_Post_Errno_Unchanged,
  CMqueueReqSend_Post_Errno_EINVAL,
  CMqueueReqSend_Post_Errno_EBADF,
  CMqueueReqSend_Post_Errno_EMSGSIZE,
  CMqueueReqSend_Post_Errno_EAGAIN,
  CMqueueReqSend_Post_Errno_NA
} CMqueueReqSend_Post_Errno;

typedef enum {
  CMqueueReqSend_Post_Message_Enqueued,
  CMqueueReqSend_Post_Message_Delivered,
  CMqueueReqSend_Post_Message_Nop,
  CMqueueReqSend_Post_Message_NA
} CMqueueReqSend_Post_Message;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_MsgPrio_NA : 1;
  uint16_t Pre_Mqdes_NA : 1;
  uint16_t Pre_MsgLen_NA : 1;
  uint16_t Pre_Queue_NA : 1;
  uint16_t Pre_Blocking_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 3;
  uint16_t Post_Message : 2;
} CMqueueReqSend_Entry;

#define MQ_NAME "/MQSEND"

#define MSG_SIZE 4

#define INVALID_MQ ( ( mqd_t ) - 1 )

/**
 * @brief Test context for spec:/c/mqueue/req/send test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `mqdes` parameter value.
   */
  mqd_t mqdes;

  /**
   * @brief This member specifies the `msg_ptr` parameter value.
   */
  const char *msg_ptr;

  /**
   * @brief This member specifies the `msg_len` parameter value.
   */
  size_t msg_len;

  /**
   * @brief This member specifies the `msg_prio` parameter value.
   */
  unsigned int msg_prio;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the descriptor used to prepare and check the
   *   message queue.
   */
  mqd_t mq;

  /**
   * @brief This member contains the attributes used to open and modify the
   *   message queue.
   */
  struct mq_attr mq_attr;

  /**
   * @brief If this member is true, then the descriptor of the message queue
   *   used for the preparation was closed.
   */
  bool mq_closed;

  /**
   * @brief If this member is true, then the message queue was filled during
   *   the preparation.
   */
  bool queue_full;

  /**
   * @brief This member contains the message to send.
   */
  uint8_t msg[ MSG_SIZE ];

  /**
   * @brief This member contains the message which fills the message queue.
   */
  uint8_t old_msg[ MSG_SIZE ];

  /**
   * @brief This member contains the identifier of the worker task which
   *   receives messages.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the buffer of the worker task for the received
   *   message.
   */
  char recv_buf[ MSG_SIZE ];

  /**
   * @brief This member contains the length of the message received by the
   *   worker task.
   */
  ssize_t recv_len;

  /**
   * @brief This member contains the message priority of the message received
   *   by the worker task.
   */
  unsigned int recv_prio;

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
  { "Valid", "Invalid", "NA" };

static const char *const CMqueueReqSend_PreDesc_Mqdes[] =
  { "Valid", "ReadOnly", "Closed", "Invalid", "NA" };

static const char *const CMqueueReqSend_PreDesc_MsgLen[] =
  { "Fits", "TooLong", "NA" };

static const char *const CMqueueReqSend_PreDesc_Queue[] =
  { "Empty", "Receiver", "Full", "NA" };

static const char *const CMqueueReqSend_PreDesc_Blocking[] =
  { "Yes", "No", "NA" };

static const char *const *const CMqueueReqSend_PreDesc[] = {
  CMqueueReqSend_PreDesc_MsgPrio,
  CMqueueReqSend_PreDesc_Mqdes,
  CMqueueReqSend_PreDesc_MsgLen,
  CMqueueReqSend_PreDesc_Queue,
  CMqueueReqSend_PreDesc_Blocking,
  NULL
};

typedef CMqueueReqSend_Context Context;

static const rtems_event_set EVENT_RECEIVE = RTEMS_EVENT_17;

static void WorkerTask( rtems_task_argument arg )
{
  Context *ctx = (Context *) arg;

  while ( true ) {
    (void) ReceiveAnyEvents();
    ctx->recv_len = mq_receive(
      ctx->mq,
      ctx->recv_buf,
      sizeof( ctx->recv_buf ),
      &ctx->recv_prio
    );
  }
}

static long GetPendingCount( Context *ctx )
{
  struct mq_attr attr;

  if ( ctx->mq_closed ) {
    ctx->mq = mq_open( MQ_NAME, O_RDWR );
    T_ne_u32( ctx->mq, INVALID_MQ );
    ctx->mq_closed = false;
  }

  T_eq_int( mq_getattr( ctx->mq, &attr ), 0 );
  return attr.mq_curmsgs;
}

static void CheckReceive(
  Context       *ctx,
  const uint8_t *expected,
  unsigned int   expected_prio
)
{
  char         buf[ MSG_SIZE ];
  unsigned int prio;

  T_eq_long( mq_receive( ctx->mq, buf, sizeof( buf ), &prio ), MSG_SIZE );
  T_eq_mem( buf, expected, MSG_SIZE );
  T_eq_uint( prio, expected_prio );
}

static void CMqueueReqSend_Pre_MsgPrio_Prepare(
  CMqueueReqSend_Context    *ctx,
  CMqueueReqSend_Pre_MsgPrio state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_MsgPrio_Valid: {
      /*
       * While the `msg_prio` parameter is less than or equal to MQ_PRIO_MAX.
       */
      ctx->msg_prio = MQ_PRIO_MAX;
      break;
    }

    case CMqueueReqSend_Pre_MsgPrio_Invalid: {
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

static void CMqueueReqSend_Pre_Mqdes_Prepare(
  CMqueueReqSend_Context  *ctx,
  CMqueueReqSend_Pre_Mqdes state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_Mqdes_Valid: {
      /*
       * While the `mqdes` parameter is a message queue descriptor of a message
       * queue opened for writing.
       */
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqSend_Pre_Mqdes_ReadOnly: {
      /*
       * While the `mqdes` parameter is a message queue descriptor of a message
       * queue opened for reading only.
       */
      T_eq_int( mq_close( ctx->mq ), 0 );
      T_eq_int( mq_unlink( MQ_NAME ), 0 );
      ctx->mq = mq_open( MQ_NAME, O_CREAT | O_RDONLY, 0600, &ctx->mq_attr );
      T_ne_u32( ctx->mq, INVALID_MQ );
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqSend_Pre_Mqdes_Closed: {
      /*
       * While the `mqdes` parameter is a message queue descriptor of a message
       * queue which was closed.
       */
      ctx->mqdes = ctx->mq;
      T_eq_int( mq_close( ctx->mq ), 0 );
      ctx->mq_closed = true;
      break;
    }

    case CMqueueReqSend_Pre_Mqdes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor of a
       * message queue.
       */
      ctx->mqdes = INVALID_MQ;
      break;
    }

    case CMqueueReqSend_Pre_Mqdes_NA:
      break;
  }
}

static void CMqueueReqSend_Pre_MsgLen_Prepare(
  CMqueueReqSend_Context   *ctx,
  CMqueueReqSend_Pre_MsgLen state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_MsgLen_Fits: {
      /*
       * While the `msg_len` parameter is less than or equal to the maximum
       * message size of the message queue.
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

static void CMqueueReqSend_Pre_Queue_Prepare(
  CMqueueReqSend_Context  *ctx,
  CMqueueReqSend_Pre_Queue state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_Queue_Empty: {
      /*
       * While the message queue is empty and no thread waits to receive a
       * message.
       */
      /* Already prepared */
      break;
    }

    case CMqueueReqSend_Pre_Queue_Receiver: {
      /*
       * While the message queue is empty and a thread waits in mq_receive() to
       * receive a message.
       */
      SetPriority( ctx->worker_id, PRIO_HIGH );
      SendEvents( ctx->worker_id, EVENT_RECEIVE );
      break;
    }

    case CMqueueReqSend_Pre_Queue_Full: {
      /*
       * While the message queue is full.
       */
      ctx->queue_full = true;
      T_eq_int(
        mq_send( ctx->mq, (const char *) ctx->old_msg, MSG_SIZE, 0 ),
        0
      );
      break;
    }

    case CMqueueReqSend_Pre_Queue_NA:
      break;
  }
}

static void CMqueueReqSend_Pre_Blocking_Prepare(
  CMqueueReqSend_Context     *ctx,
  CMqueueReqSend_Pre_Blocking state
)
{
  switch ( state ) {
    case CMqueueReqSend_Pre_Blocking_Yes: {
      /*
       * While the O_NONBLOCK flag is not set for the message queue
       * description, while a thread receives a message once the calling thread
       * is blocked.
       */
      SetPriority( ctx->worker_id, PRIO_LOW );
      SendEvents( ctx->worker_id, EVENT_RECEIVE );
      break;
    }

    case CMqueueReqSend_Pre_Blocking_No: {
      /*
       * While the O_NONBLOCK flag is set for the message queue description.
       */
      ctx->mq_attr.mq_flags = O_RDWR | O_NONBLOCK;
      T_eq_int( mq_setattr( ctx->mq, &ctx->mq_attr, NULL ), 0 );
      break;
    }

    case CMqueueReqSend_Pre_Blocking_NA:
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
       * The return value of mq_send() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CMqueueReqSend_Post_Status_MinusOne: {
      /*
       * The return value of mq_send() shall be equal to minus one.
       */
      T_eq_int( ctx->retval, -1 );
      break;
    }

    case CMqueueReqSend_Post_Status_NA:
      break;
  }
}

static void CMqueueReqSend_Post_Errno_Check( CMqueueReqSend_Post_Errno state )
{
  switch ( state ) {
    case CMqueueReqSend_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_send() call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CMqueueReqSend_Post_Errno_EINVAL: {
      /*
       * The errno shall be set to EINVAL.
       */
      T_eq_int( errno, EINVAL );
      break;
    }

    case CMqueueReqSend_Post_Errno_EBADF: {
      /*
       * The errno shall be set to EBADF.
       */
      T_eq_int( errno, EBADF );
      break;
    }

    case CMqueueReqSend_Post_Errno_EMSGSIZE: {
      /*
       * The errno shall be set to EMSGSIZE.
       */
      T_eq_int( errno, EMSGSIZE );
      break;
    }

    case CMqueueReqSend_Post_Errno_EAGAIN: {
      /*
       * The errno shall be set to EAGAIN.
       */
      T_eq_int( errno, EAGAIN );
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
    case CMqueueReqSend_Post_Message_Enqueued: {
      /*
       * The message specified by `msg_ptr` shall be added to the message queue
       * with the message priority specified by `msg_prio`.
       */
      T_eq_long( GetPendingCount( ctx ), 1 );
      CheckReceive( ctx, ctx->msg, ctx->msg_prio );
      break;
    }

    case CMqueueReqSend_Post_Message_Delivered: {
      /*
       * The message specified by `msg_ptr` shall be received by the thread
       * waiting in mq_receive() with the message priority specified by
       * `msg_prio`.
       */
      T_eq_long( GetPendingCount( ctx ), 0 );
      T_eq_long( ctx->recv_len, MSG_SIZE );
      T_eq_mem( ctx->recv_buf, ctx->msg, MSG_SIZE );
      T_eq_uint( ctx->recv_prio, ctx->msg_prio );
      break;
    }

    case CMqueueReqSend_Post_Message_Nop: {
      /*
       * The message queue shall not be changed by the mq_send() call.
       */
      if ( ctx->queue_full ) {
        T_eq_long( GetPendingCount( ctx ), 1 );
        CheckReceive( ctx, ctx->old_msg, 0 );
      } else {
        T_eq_long( GetPendingCount( ctx ), 0 );
      }
      break;
    }

    case CMqueueReqSend_Post_Message_NA:
      break;
  }
}

static void CMqueueReqSend_Setup( CMqueueReqSend_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, WorkerTask, ctx );
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
  ctx->mq_attr.mq_flags = 0;
  ctx->mq_attr.mq_maxmsg = 1;
  ctx->mq_attr.mq_msgsize = MSG_SIZE;
  ctx->mq = mq_open( MQ_NAME, O_CREAT | O_RDWR, 0600, &ctx->mq_attr );
  T_ne_u32( ctx->mq, INVALID_MQ );
  ctx->mqdes = ctx->mq;
  ctx->mq_closed = false;
  ctx->queue_full = false;
  memset( ctx->msg, 0x5a, sizeof( ctx->msg ) );
  memset( ctx->old_msg, 0xa5, sizeof( ctx->old_msg ) );
  ctx->msg_ptr = (const char *) ctx->msg;
  ctx->msg_len = MSG_SIZE;
  ctx->msg_prio = MQ_PRIO_MAX;
  ctx->recv_len = -1;
  ctx->recv_prio = UINT_MAX;
  errno = -1;
}

static void CMqueueReqSend_Action( CMqueueReqSend_Context *ctx )
{
  ctx->retval = mq_send(
    ctx->mqdes,
    ctx->msg_ptr,
    ctx->msg_len,
    ctx->msg_prio
  );
}

static void CMqueueReqSend_Cleanup( CMqueueReqSend_Context *ctx )
{
  if ( !ctx->mq_closed ) {
    T_eq_int( mq_close( ctx->mq ), 0 );
  }

  T_eq_int( mq_unlink( MQ_NAME ), 0 );
}

/* clang-format off */

static const CMqueueReqSend_Entry
CMqueueReqSend_Entries[] = {
  { 1, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_NA,
    CMqueueReqSend_Post_Errno_NA, CMqueueReqSend_Post_Message_NA },
  { 0, 0, 0, 1, 1, 1, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_EBADF, CMqueueReqSend_Post_Message_Nop },
  { 0, 0, 0, 0, 0, 1, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Unchanged, CMqueueReqSend_Post_Message_Enqueued },
  { 0, 0, 0, 0, 0, 1, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Unchanged, CMqueueReqSend_Post_Message_Delivered },
  { 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_Zero,
    CMqueueReqSend_Post_Errno_Unchanged, CMqueueReqSend_Post_Message_Enqueued },
  { 0, 0, 0, 0, 0, 0, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_EAGAIN, CMqueueReqSend_Post_Message_Nop },
  { 0, 0, 0, 0, 1, 1, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_EMSGSIZE, CMqueueReqSend_Post_Message_Nop },
  { 0, 0, 1, 1, 1, 1, CMqueueReqSend_Post_Status_MinusOne,
    CMqueueReqSend_Post_Errno_EINVAL, CMqueueReqSend_Post_Message_Nop }
};

static const uint8_t
CMqueueReqSend_Map[] = {
  2, 0, 3, 0, 4, 5, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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

  if ( ctx->Map.entry.Pre_Mqdes_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqSend_Pre_Mqdes_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_MsgLen_NA ) {
    ctx->Map.pcs[ 2 ] = CMqueueReqSend_Pre_MsgLen_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Queue_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqSend_Pre_Queue_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Blocking_NA ) {
    ctx->Map.pcs[ 4 ] = CMqueueReqSend_Pre_Blocking_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }
}

static void CMqueueReqSend_TestVariant( CMqueueReqSend_Context *ctx )
{
  CMqueueReqSend_Pre_MsgPrio_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqSend_Pre_Mqdes_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqSend_Pre_MsgLen_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqSend_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqSend_Pre_Blocking_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqSend_Action( ctx );
  CMqueueReqSend_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqSend_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CMqueueReqSend_Post_Message_Check( ctx, ctx->Map.entry.Post_Message );
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
    ctx->Map.pci[ 0 ] = CMqueueReqSend_Pre_MsgPrio_Valid;
    ctx->Map.pci[ 0 ] < CMqueueReqSend_Pre_MsgPrio_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqSend_Pre_Mqdes_Valid;
      ctx->Map.pci[ 1 ] < CMqueueReqSend_Pre_Mqdes_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqSend_Pre_MsgLen_Fits;
        ctx->Map.pci[ 2 ] < CMqueueReqSend_Pre_MsgLen_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqSend_Pre_Queue_Empty;
          ctx->Map.pci[ 3 ] < CMqueueReqSend_Pre_Queue_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqSend_Pre_Blocking_Yes;
            ctx->Map.pci[ 4 ] < CMqueueReqSend_Pre_Blocking_NA;
            ++ctx->Map.pci[ 4 ]
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

/** @} */
