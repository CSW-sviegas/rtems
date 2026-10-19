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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
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
 * @defgroup CMqueueReqReceive spec:/c/mqueue/req/receive
 *
 * @{
 */

typedef enum {
  CMqueueReqReceive_Pre_Mqdes_Valid,
  CMqueueReqReceive_Pre_Mqdes_WriteOnly,
  CMqueueReqReceive_Pre_Mqdes_Closed,
  CMqueueReqReceive_Pre_Mqdes_Invalid,
  CMqueueReqReceive_Pre_Mqdes_NA
} CMqueueReqReceive_Pre_Mqdes;

typedef enum {
  CMqueueReqReceive_Pre_MsgLen_Valid,
  CMqueueReqReceive_Pre_MsgLen_TooSmall,
  CMqueueReqReceive_Pre_MsgLen_NA
} CMqueueReqReceive_Pre_MsgLen;

typedef enum {
  CMqueueReqReceive_Pre_MsgPrio_Valid,
  CMqueueReqReceive_Pre_MsgPrio_Null,
  CMqueueReqReceive_Pre_MsgPrio_NA
} CMqueueReqReceive_Pre_MsgPrio;

typedef enum {
  CMqueueReqReceive_Pre_Queue_Pending,
  CMqueueReqReceive_Pre_Queue_Empty,
  CMqueueReqReceive_Pre_Queue_NA
} CMqueueReqReceive_Pre_Queue;

typedef enum {
  CMqueueReqReceive_Pre_Blocking_Yes,
  CMqueueReqReceive_Pre_Blocking_No,
  CMqueueReqReceive_Pre_Blocking_NA
} CMqueueReqReceive_Pre_Blocking;

typedef enum {
  CMqueueReqReceive_Post_Status_Length,
  CMqueueReqReceive_Post_Status_MinusOne,
  CMqueueReqReceive_Post_Status_NA
} CMqueueReqReceive_Post_Status;

typedef enum {
  CMqueueReqReceive_Post_Errno_Unchanged,
  CMqueueReqReceive_Post_Errno_EBADF,
  CMqueueReqReceive_Post_Errno_EMSGSIZE,
  CMqueueReqReceive_Post_Errno_EAGAIN,
  CMqueueReqReceive_Post_Errno_NA
} CMqueueReqReceive_Post_Errno;

typedef enum {
  CMqueueReqReceive_Post_Buffer_Received,
  CMqueueReqReceive_Post_Buffer_Nop,
  CMqueueReqReceive_Post_Buffer_NA
} CMqueueReqReceive_Post_Buffer;

typedef enum {
  CMqueueReqReceive_Post_Prio_Received,
  CMqueueReqReceive_Post_Prio_Nop,
  CMqueueReqReceive_Post_Prio_NA
} CMqueueReqReceive_Post_Prio;

typedef enum {
  CMqueueReqReceive_Post_Message_Dequeued,
  CMqueueReqReceive_Post_Message_Nop,
  CMqueueReqReceive_Post_Message_NA
} CMqueueReqReceive_Post_Message;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Mqdes_NA : 1;
  uint32_t Pre_MsgLen_NA : 1;
  uint32_t Pre_MsgPrio_NA : 1;
  uint32_t Pre_Queue_NA : 1;
  uint32_t Pre_Blocking_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 3;
  uint32_t Post_Buffer : 2;
  uint32_t Post_Prio : 2;
  uint32_t Post_Message : 2;
} CMqueueReqReceive_Entry;

#define MQ_NAME "/MQRECV"

#define MSG_SIZE 4

#define MSG_PRIO 3

#define INVALID_MQ ( ( mqd_t ) - 1 )

/**
 * @brief Test context for spec:/c/mqueue/req/receive test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `mqdes` parameter value.
   */
  mqd_t mqdes;

  /**
   * @brief This member specifies the `msg_ptr` parameter value.
   */
  char *msg_ptr;

  /**
   * @brief This member specifies the `msg_len` parameter value.
   */
  size_t msg_len;

  /**
   * @brief This member specifies the `msg_prio` parameter value.
   */
  unsigned int *msg_prio;

  /**
   * @brief This member contains the return value.
   */
  ssize_t retval;

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
   * @brief If this member is true, then a message was sent to the message
   *   queue during the preparation.
   */
  bool queue_pending;

  /**
   * @brief This member contains the message sent to the message queue.
   */
  uint8_t msg[ MSG_SIZE ];

  /**
   * @brief This member contains the buffer which receives the message.
   */
  char recv_buf[ MSG_SIZE ];

  /**
   * @brief This member contains the expected content of the buffer if no
   *   message was received.
   */
  uint8_t untouched_buf[ MSG_SIZE ];

  /**
   * @brief This member contains the object which receives the message
   *   priority.
   */
  unsigned int prio_value;

  /**
   * @brief This member contains the identifier of the worker task which sends
   *   the message.
   */
  rtems_id worker_id;

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
    CMqueueReqReceive_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqReceive_Context;

static CMqueueReqReceive_Context
  CMqueueReqReceive_Instance;

static const char * const CMqueueReqReceive_PreDesc_Mqdes[] = {
  "Valid",
  "WriteOnly",
  "Closed",
  "Invalid",
  "NA"
};

static const char * const CMqueueReqReceive_PreDesc_MsgLen[] = {
  "Valid",
  "TooSmall",
  "NA"
};

static const char * const CMqueueReqReceive_PreDesc_MsgPrio[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CMqueueReqReceive_PreDesc_Queue[] = {
  "Pending",
  "Empty",
  "NA"
};

static const char * const CMqueueReqReceive_PreDesc_Blocking[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const CMqueueReqReceive_PreDesc[] = {
  CMqueueReqReceive_PreDesc_Mqdes,
  CMqueueReqReceive_PreDesc_MsgLen,
  CMqueueReqReceive_PreDesc_MsgPrio,
  CMqueueReqReceive_PreDesc_Queue,
  CMqueueReqReceive_PreDesc_Blocking,
  NULL
};

typedef CMqueueReqReceive_Context Context;

static const rtems_event_set EVENT_SEND = RTEMS_EVENT_17;

static void WorkerTask( rtems_task_argument arg )
{
  Context *ctx = (Context *) arg;

  while ( true ) {
    (void) ReceiveAnyEvents();
    T_eq_int(
      mq_send( ctx->mq, (const char *) ctx->msg, MSG_SIZE, MSG_PRIO ),
      0
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

static void CheckReceive( Context *ctx )
{
  char         buf[ MSG_SIZE ];
  unsigned int prio;

  T_eq_long( mq_receive( ctx->mq, buf, sizeof( buf ), &prio ), MSG_SIZE );
  T_eq_mem( buf, ctx->msg, MSG_SIZE );
  T_eq_uint( prio, MSG_PRIO );
}

static void CMqueueReqReceive_Pre_Mqdes_Prepare(
  CMqueueReqReceive_Context  *ctx,
  CMqueueReqReceive_Pre_Mqdes state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Mqdes_Valid: {
      /*
       * While the `mqdes` parameter is a message queue descriptor of a message
       * queue opened for reading.
       */
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqReceive_Pre_Mqdes_WriteOnly: {
      /*
       * While the `mqdes` parameter is a message queue descriptor of a message
       * queue opened for writing only.
       */
      T_eq_int( mq_close( ctx->mq ), 0 );
      T_eq_int( mq_unlink( MQ_NAME ), 0 );
      ctx->mq = mq_open( MQ_NAME, O_CREAT | O_WRONLY, 0600, &ctx->mq_attr );
      T_ne_u32( ctx->mq, INVALID_MQ );
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqReceive_Pre_Mqdes_Closed: {
      /*
       * While the `mqdes` parameter is a message queue descriptor of a message
       * queue which was closed and not unlinked.
       */
      ctx->mqdes = ctx->mq;
      T_eq_int( mq_close( ctx->mq ), 0 );
      ctx->mq_closed = true;
      break;
    }

    case CMqueueReqReceive_Pre_Mqdes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor of a
       * message queue.
       */
      ctx->mqdes = INVALID_MQ;
      break;
    }

    case CMqueueReqReceive_Pre_Mqdes_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_MsgLen_Prepare(
  CMqueueReqReceive_Context   *ctx,
  CMqueueReqReceive_Pre_MsgLen state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_MsgLen_Valid: {
      /*
       * While the `msg_len` parameter is greater than or equal to the maximum
       * message size of the message queue.
       */
      ctx->msg_len = MSG_SIZE;
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

static void CMqueueReqReceive_Pre_MsgPrio_Prepare(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Pre_MsgPrio state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_MsgPrio_Valid: {
      /*
       * While the `msg_prio` parameter references an object of type `unsigned
       * int`.
       */
      ctx->msg_prio = &ctx->prio_value;
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

static void CMqueueReqReceive_Pre_Queue_Prepare(
  CMqueueReqReceive_Context  *ctx,
  CMqueueReqReceive_Pre_Queue state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Queue_Pending: {
      /*
       * While the message queue contains a message.
       */
      ctx->queue_pending = true;
      T_eq_int(
        mq_send( ctx->mq, (const char *) ctx->msg, MSG_SIZE, MSG_PRIO ),
        0
      );
      break;
    }

    case CMqueueReqReceive_Pre_Queue_Empty: {
      /*
       * While the message queue is empty.
       */
      /* Already prepared */
      break;
    }

    case CMqueueReqReceive_Pre_Queue_NA:
      break;
  }
}

static void CMqueueReqReceive_Pre_Blocking_Prepare(
  CMqueueReqReceive_Context     *ctx,
  CMqueueReqReceive_Pre_Blocking state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Pre_Blocking_Yes: {
      /*
       * While the O_NONBLOCK flag is not set for the message queue
       * description, while a thread sends a message once the calling thread is
       * blocked.
       */
      ctx->mq_attr.mq_flags = O_RDWR;
      T_eq_int( mq_setattr( ctx->mq, &ctx->mq_attr, NULL ), 0 );
      SendEvents( ctx->worker_id, EVENT_SEND );
      break;
    }

    case CMqueueReqReceive_Pre_Blocking_No: {
      /*
       * While the O_NONBLOCK flag is set for the message queue description.
       */
      /* Already prepared */
      break;
    }

    case CMqueueReqReceive_Pre_Blocking_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Status_Check(
  CMqueueReqReceive_Context    *ctx,
  CMqueueReqReceive_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Status_Length: {
      /*
       * The return value of mq_receive() shall be equal to the message size of
       * the received message.
       */
      T_eq_long( ctx->retval, MSG_SIZE );
      break;
    }

    case CMqueueReqReceive_Post_Status_MinusOne: {
      /*
       * The return value of mq_receive() shall be equal to minus one.
       */
      T_eq_long( ctx->retval, -1 );
      break;
    }

    case CMqueueReqReceive_Post_Status_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Errno_Check(
  CMqueueReqReceive_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_receive() call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CMqueueReqReceive_Post_Errno_EBADF: {
      /*
       * The errno shall be set to EBADF.
       */
      T_eq_int( errno, EBADF );
      break;
    }

    case CMqueueReqReceive_Post_Errno_EMSGSIZE: {
      /*
       * The errno shall be set to EMSGSIZE.
       */
      T_eq_int( errno, EMSGSIZE );
      break;
    }

    case CMqueueReqReceive_Post_Errno_EAGAIN: {
      /*
       * The errno shall be set to EAGAIN.
       */
      T_eq_int( errno, EAGAIN );
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
    case CMqueueReqReceive_Post_Buffer_Received: {
      /*
       * The buffer referenced by `msg_ptr` shall contain the received message.
       */
      T_eq_mem( ctx->recv_buf, ctx->msg, MSG_SIZE );
      break;
    }

    case CMqueueReqReceive_Post_Buffer_Nop: {
      /*
       * The buffer referenced by `msg_ptr` shall not be modified by the
       * mq_receive() call.
       */
      T_eq_mem( ctx->recv_buf, ctx->untouched_buf, MSG_SIZE );
      break;
    }

    case CMqueueReqReceive_Post_Buffer_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Prio_Check(
  CMqueueReqReceive_Context  *ctx,
  CMqueueReqReceive_Post_Prio state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Prio_Received: {
      /*
       * The object referenced by `msg_prio` shall be set to the message
       * priority of the received message.
       */
      T_eq_uint( ctx->prio_value, MSG_PRIO );
      break;
    }

    case CMqueueReqReceive_Post_Prio_Nop: {
      /*
       * The object referenced by `msg_prio` shall not be modified by the
       * mq_receive() call.
       */
      T_eq_uint( ctx->prio_value, UINT_MAX );
      break;
    }

    case CMqueueReqReceive_Post_Prio_NA:
      break;
  }
}

static void CMqueueReqReceive_Post_Message_Check(
  CMqueueReqReceive_Context     *ctx,
  CMqueueReqReceive_Post_Message state
)
{
  switch ( state ) {
    case CMqueueReqReceive_Post_Message_Dequeued: {
      /*
       * The received message shall be removed from the message queue.
       */
      T_eq_long( GetPendingCount( ctx ), 0 );
      break;
    }

    case CMqueueReqReceive_Post_Message_Nop: {
      /*
       * The message queue shall not be changed by the mq_receive() call.
       */
      if ( ctx->queue_pending ) {
        T_eq_long( GetPendingCount( ctx ), 1 );
        CheckReceive( ctx );
      } else {
        T_eq_long( GetPendingCount( ctx ), 0 );
      }
      break;
    }

    case CMqueueReqReceive_Post_Message_NA:
      break;
  }
}

static void CMqueueReqReceive_Setup( CMqueueReqReceive_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
  StartTask( ctx->worker_id, WorkerTask, ctx );
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
  ctx->mq_attr.mq_flags = O_RDWR | O_NONBLOCK;
  ctx->mq_attr.mq_maxmsg = 1;
  ctx->mq_attr.mq_msgsize = MSG_SIZE;
  ctx->mq = mq_open(
    MQ_NAME,
    O_CREAT | O_RDWR | O_NONBLOCK,
    0600,
    &ctx->mq_attr
  );
  T_ne_u32( ctx->mq, INVALID_MQ );
  ctx->mqdes = ctx->mq;
  ctx->mq_closed = false;
  ctx->queue_pending = false;
  memset( ctx->msg, 0x5a, sizeof( ctx->msg ) );
  memset( ctx->recv_buf, 0xa5, sizeof( ctx->recv_buf ) );
  memset( ctx->untouched_buf, 0xa5, sizeof( ctx->untouched_buf ) );
  ctx->msg_ptr = ctx->recv_buf;
  ctx->msg_len = MSG_SIZE;
  ctx->prio_value = UINT_MAX;
  ctx->msg_prio = &ctx->prio_value;
  errno = -1;
}

static void CMqueueReqReceive_Action( CMqueueReqReceive_Context *ctx )
{
  ctx->retval = mq_receive(
    ctx->mqdes,
    ctx->msg_ptr,
    ctx->msg_len,
    ctx->msg_prio
  );
}

static void CMqueueReqReceive_Cleanup( CMqueueReqReceive_Context *ctx )
{
  if ( !ctx->mq_closed ) {
    T_eq_int( mq_close( ctx->mq ), 0 );
  }

  T_eq_int( mq_unlink( MQ_NAME ), 0 );
}

static const CMqueueReqReceive_Entry
CMqueueReqReceive_Entries[] = {
  { 0, 0, 1, 1, 1, 1, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_EBADF, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Prio_Nop, CMqueueReqReceive_Post_Message_Nop },
  { 0, 0, 0, 1, 0, 1, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_EMSGSIZE, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Prio_Nop, CMqueueReqReceive_Post_Message_Nop },
  { 0, 0, 0, 1, 1, 1, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_EBADF, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Prio_Nop, CMqueueReqReceive_Post_Message_Nop },
  { 0, 0, 0, 1, 1, 1, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_EMSGSIZE, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Prio_Nop, CMqueueReqReceive_Post_Message_Nop },
  { 0, 0, 0, 0, 0, 1, CMqueueReqReceive_Post_Status_Length,
    CMqueueReqReceive_Post_Errno_Unchanged,
    CMqueueReqReceive_Post_Buffer_Received,
    CMqueueReqReceive_Post_Prio_Received,
    CMqueueReqReceive_Post_Message_Dequeued },
  { 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_MinusOne,
    CMqueueReqReceive_Post_Errno_EAGAIN, CMqueueReqReceive_Post_Buffer_Nop,
    CMqueueReqReceive_Post_Prio_Nop, CMqueueReqReceive_Post_Message_Nop },
  { 0, 0, 0, 0, 0, 1, CMqueueReqReceive_Post_Status_Length,
    CMqueueReqReceive_Post_Errno_Unchanged,
    CMqueueReqReceive_Post_Buffer_Received, CMqueueReqReceive_Post_Prio_NA,
    CMqueueReqReceive_Post_Message_Dequeued },
  { 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Length,
    CMqueueReqReceive_Post_Errno_Unchanged,
    CMqueueReqReceive_Post_Buffer_Received,
    CMqueueReqReceive_Post_Prio_Received,
    CMqueueReqReceive_Post_Message_Dequeued },
  { 0, 0, 0, 0, 0, 0, CMqueueReqReceive_Post_Status_Length,
    CMqueueReqReceive_Post_Errno_Unchanged,
    CMqueueReqReceive_Post_Buffer_Received, CMqueueReqReceive_Post_Prio_NA,
    CMqueueReqReceive_Post_Message_Dequeued }
};

static const uint8_t
CMqueueReqReceive_Map[] = {
  4, 4, 7, 5, 6, 6, 8, 5, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

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
  return CMqueueReqReceive_Entries[
    CMqueueReqReceive_Map[ index ]
  ];
}

static void CMqueueReqReceive_SetPreConditionStates(
  CMqueueReqReceive_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_MsgLen_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqReceive_Pre_MsgLen_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_MsgPrio_NA ) {
    ctx->Map.pcs[ 2 ] = CMqueueReqReceive_Pre_MsgPrio_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Queue_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqReceive_Pre_Queue_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Blocking_NA ) {
    ctx->Map.pcs[ 4 ] = CMqueueReqReceive_Pre_Blocking_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }
}

static void CMqueueReqReceive_TestVariant( CMqueueReqReceive_Context *ctx )
{
  CMqueueReqReceive_Pre_Mqdes_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqReceive_Pre_MsgLen_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqReceive_Pre_MsgPrio_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqReceive_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqReceive_Pre_Blocking_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqReceive_Action( ctx );
  CMqueueReqReceive_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqReceive_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CMqueueReqReceive_Post_Buffer_Check( ctx, ctx->Map.entry.Post_Buffer );
  CMqueueReqReceive_Post_Prio_Check( ctx, ctx->Map.entry.Post_Prio );
  CMqueueReqReceive_Post_Message_Check( ctx, ctx->Map.entry.Post_Message );
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
    ctx->Map.pci[ 0 ] = CMqueueReqReceive_Pre_Mqdes_Valid;
    ctx->Map.pci[ 0 ] < CMqueueReqReceive_Pre_Mqdes_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqReceive_Pre_MsgLen_Valid;
      ctx->Map.pci[ 1 ] < CMqueueReqReceive_Pre_MsgLen_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqReceive_Pre_MsgPrio_Valid;
        ctx->Map.pci[ 2 ] < CMqueueReqReceive_Pre_MsgPrio_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqReceive_Pre_Queue_Pending;
          ctx->Map.pci[ 3 ] < CMqueueReqReceive_Pre_Queue_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqReceive_Pre_Blocking_Yes;
            ctx->Map.pci[ 4 ] < CMqueueReqReceive_Pre_Blocking_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = CMqueueReqReceive_PopEntry( ctx );
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

/** @} */
