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
#include <rtems.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqTimedreceive spec:/c/mqueue/req/timedreceive
 *
 * @{
 */

typedef enum {
  CMqueueReqTimedreceive_Pre_Flags_Blocking,
  CMqueueReqTimedreceive_Pre_Flags_NonBlock,
  CMqueueReqTimedreceive_Pre_Flags_WriteOnly,
  CMqueueReqTimedreceive_Pre_Flags_NA
} CMqueueReqTimedreceive_Pre_Flags;

typedef enum {
  CMqueueReqTimedreceive_Pre_Messages_None,
  CMqueueReqTimedreceive_Pre_Messages_One,
  CMqueueReqTimedreceive_Pre_Messages_NA
} CMqueueReqTimedreceive_Pre_Messages;

typedef enum {
  CMqueueReqTimedreceive_Pre_Mqdes_Valid,
  CMqueueReqTimedreceive_Pre_Mqdes_Closed,
  CMqueueReqTimedreceive_Pre_Mqdes_Invalid,
  CMqueueReqTimedreceive_Pre_Mqdes_NA
} CMqueueReqTimedreceive_Pre_Mqdes;

typedef enum {
  CMqueueReqTimedreceive_Pre_MsgLen_Enough,
  CMqueueReqTimedreceive_Pre_MsgLen_TooSmall,
  CMqueueReqTimedreceive_Pre_MsgLen_NA
} CMqueueReqTimedreceive_Pre_MsgLen;

typedef enum {
  CMqueueReqTimedreceive_Pre_MsgPrio_Valid,
  CMqueueReqTimedreceive_Pre_MsgPrio_Null,
  CMqueueReqTimedreceive_Pre_MsgPrio_NA
} CMqueueReqTimedreceive_Pre_MsgPrio;

typedef enum {
  CMqueueReqTimedreceive_Pre_Abstime_Future,
  CMqueueReqTimedreceive_Pre_Abstime_Past,
  CMqueueReqTimedreceive_Pre_Abstime_Invalid,
  CMqueueReqTimedreceive_Pre_Abstime_Null,
  CMqueueReqTimedreceive_Pre_Abstime_NA
} CMqueueReqTimedreceive_Pre_Abstime;

typedef enum {
  CMqueueReqTimedreceive_Post_Status_Length,
  CMqueueReqTimedreceive_Post_Status_MinusOne,
  CMqueueReqTimedreceive_Post_Status_NA
} CMqueueReqTimedreceive_Post_Status;

typedef enum {
  CMqueueReqTimedreceive_Post_Errno_Unchanged,
  CMqueueReqTimedreceive_Post_Errno_EAGAIN,
  CMqueueReqTimedreceive_Post_Errno_EBADF,
  CMqueueReqTimedreceive_Post_Errno_EINVAL,
  CMqueueReqTimedreceive_Post_Errno_EMSGSIZE,
  CMqueueReqTimedreceive_Post_Errno_ETIMEDOUT,
  CMqueueReqTimedreceive_Post_Errno_NA
} CMqueueReqTimedreceive_Post_Errno;

typedef enum {
  CMqueueReqTimedreceive_Post_Msg_Set,
  CMqueueReqTimedreceive_Post_Msg_Nop,
  CMqueueReqTimedreceive_Post_Msg_NA
} CMqueueReqTimedreceive_Post_Msg;

typedef enum {
  CMqueueReqTimedreceive_Post_Prio_Set,
  CMqueueReqTimedreceive_Post_Prio_Nop,
  CMqueueReqTimedreceive_Post_Prio_NA
} CMqueueReqTimedreceive_Post_Prio;

typedef enum {
  CMqueueReqTimedreceive_Post_Timer_Inactive,
  CMqueueReqTimedreceive_Post_Timer_Realtime,
  CMqueueReqTimedreceive_Post_Timer_NA
} CMqueueReqTimedreceive_Post_Timer;

typedef enum {
  CMqueueReqTimedreceive_Post_Queue_Removed,
  CMqueueReqTimedreceive_Post_Queue_Nop,
  CMqueueReqTimedreceive_Post_Queue_NA
} CMqueueReqTimedreceive_Post_Queue;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Flags_NA : 1;
  uint32_t Pre_Messages_NA : 1;
  uint32_t Pre_Mqdes_NA : 1;
  uint32_t Pre_MsgLen_NA : 1;
  uint32_t Pre_MsgPrio_NA : 1;
  uint32_t Pre_Abstime_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 3;
  uint32_t Post_Msg : 2;
  uint32_t Post_Prio : 2;
  uint32_t Post_Timer : 2;
  uint32_t Post_Queue : 2;
} CMqueueReqTimedreceive_Entry;

#define MSG_SIZE 4

/**
 * @brief Test context for spec:/c/mqueue/req/timedreceive test case.
 */
typedef struct {
  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the timer information of the worker task.
   */
  TaskTimerInfo timer_info;

  /**
   * @brief This member contains the descriptor of the message queue created
   *   for the test variant.
   */
  mqd_t mq;

  /**
   * @brief This member contains the number of messages in the message queue
   *   before the mq_timedreceive() call.
   */
  long curmsgs;

  /**
   * @brief This member provides the buffer referenced by the `msg_ptr`
   *   parameter.
   */
  char buf[ MSG_SIZE ];

  /**
   * @brief This member provides the object referenced by the `msg_prio`
   *   parameter.
   */
  unsigned int prio_obj;

  /**
   * @brief This member provides the object referenced by the `abs_timeout`
   *   parameter.
   */
  struct timespec abstime_obj;

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
   * @brief This member specifies the `abs_timeout` parameter value.
   */
  const struct timespec *abs_timeout;

  /**
   * @brief This member contains the return value of the mq_timedreceive()
   *   call.
   */
  ssize_t retval;

  /**
   * @brief This member contains the errno value after the mq_timedreceive()
   *   call.
   */
  int eno;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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

static CMqueueReqTimedreceive_Context
  CMqueueReqTimedreceive_Instance;

static const char * const CMqueueReqTimedreceive_PreDesc_Flags[] = {
  "Blocking",
  "NonBlock",
  "WriteOnly",
  "NA"
};

static const char * const CMqueueReqTimedreceive_PreDesc_Messages[] = {
  "None",
  "One",
  "NA"
};

static const char * const CMqueueReqTimedreceive_PreDesc_Mqdes[] = {
  "Valid",
  "Closed",
  "Invalid",
  "NA"
};

static const char * const CMqueueReqTimedreceive_PreDesc_MsgLen[] = {
  "Enough",
  "TooSmall",
  "NA"
};

static const char * const CMqueueReqTimedreceive_PreDesc_MsgPrio[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CMqueueReqTimedreceive_PreDesc_Abstime[] = {
  "Future",
  "Past",
  "Invalid",
  "Null",
  "NA"
};

static const char * const * const CMqueueReqTimedreceive_PreDesc[] = {
  CMqueueReqTimedreceive_PreDesc_Flags,
  CMqueueReqTimedreceive_PreDesc_Messages,
  CMqueueReqTimedreceive_PreDesc_Mqdes,
  CMqueueReqTimedreceive_PreDesc_MsgLen,
  CMqueueReqTimedreceive_PreDesc_MsgPrio,
  CMqueueReqTimedreceive_PreDesc_Abstime,
  NULL
};

#define QUEUE_NAME "mq"

#define MESSAGE_SIZE 3

#define MESSAGE_PRIO 5

static const struct mq_attr queue_attr = { 0, 1, MSG_SIZE, 0 };

static const char message[] = "xyz";

typedef CMqueueReqTimedreceive_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    SuspendSelf();
    errno = -1;
    ctx->retval = mq_timedreceive(
      ctx->mqdes,
      ctx->msg_ptr,
      ctx->msg_len,
      ctx->msg_prio,
      ctx->abs_timeout
    );
    ctx->eno = errno;
  }
}

static void CMqueueReqTimedreceive_Pre_Flags_Prepare(
  CMqueueReqTimedreceive_Context  *ctx,
  CMqueueReqTimedreceive_Pre_Flags state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Flags_Blocking: {
      /*
       * While the message queue was opened for reading without the O_NONBLOCK
       * flag.
       */
      ctx->mq = mq_open( QUEUE_NAME, O_CREAT | O_RDWR, S_IRWXU, &queue_attr );
      T_ne_u32( ctx->mq, (mqd_t) -1 );
      break;
    }

    case CMqueueReqTimedreceive_Pre_Flags_NonBlock: {
      /*
       * While the message queue was opened for reading with the O_NONBLOCK
       * flag.
       */
      ctx->mq = mq_open(
        QUEUE_NAME,
        O_CREAT | O_RDWR | O_NONBLOCK,
        S_IRWXU,
        &queue_attr
      );
      T_ne_u32( ctx->mq, (mqd_t) -1 );
      break;
    }

    case CMqueueReqTimedreceive_Pre_Flags_WriteOnly: {
      /*
       * While the message queue was opened for writing only.
       */
      ctx->mq = mq_open( QUEUE_NAME, O_CREAT | O_WRONLY, S_IRWXU, &queue_attr );
      T_ne_u32( ctx->mq, (mqd_t) -1 );
      break;
    }

    case CMqueueReqTimedreceive_Pre_Flags_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_Messages_Prepare(
  CMqueueReqTimedreceive_Context     *ctx,
  CMqueueReqTimedreceive_Pre_Messages state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Messages_None: {
      /*
       * While the message queue is empty.
       */
      /* Already prepared */
      break;
    }

    case CMqueueReqTimedreceive_Pre_Messages_One: {
      /*
       * While the message queue contains a message.
       */
      T_eq_int( mq_send( ctx->mq, message, MESSAGE_SIZE, MESSAGE_PRIO ), 0 );
      ctx->curmsgs = 1;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Messages_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_Mqdes_Prepare(
  CMqueueReqTimedreceive_Context  *ctx,
  CMqueueReqTimedreceive_Pre_Mqdes state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Mqdes_Valid: {
      /*
       * While the `mqdes` parameter specifies a message queue descriptor of an
       * open message queue.
       */
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Mqdes_Closed: {
      /*
       * While the `mqdes` parameter specifies a message queue descriptor of a
       * message queue which was closed and not unlinked.
       */
      T_eq_int( mq_close( ctx->mq ), 0 );
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Mqdes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = INVALID_ID;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Mqdes_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Pre_MsgLen_Prepare(
  CMqueueReqTimedreceive_Context   *ctx,
  CMqueueReqTimedreceive_Pre_MsgLen state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_MsgLen_Enough: {
      /*
       * While the `msg_len` parameter is greater than or equal to the maximum
       * message size of the message queue.
       */
      ctx->msg_len = MSG_SIZE;
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

static void CMqueueReqTimedreceive_Pre_MsgPrio_Prepare(
  CMqueueReqTimedreceive_Context    *ctx,
  CMqueueReqTimedreceive_Pre_MsgPrio state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_MsgPrio_Valid: {
      /*
       * While the `msg_prio` parameter references an object of type `unsigned
       * int`.
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

static void CMqueueReqTimedreceive_Pre_Abstime_Prepare(
  CMqueueReqTimedreceive_Context    *ctx,
  CMqueueReqTimedreceive_Pre_Abstime state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Pre_Abstime_Future: {
      /*
       * While the `abs_timeout` parameter references an object of type struct
       * timespec specifying a valid time point which is after the current time
       * of the CLOCK_REALTIME.
       */
      ctx->abstime_obj.tv_sec = 1621322302;
      ctx->abstime_obj.tv_nsec = 0;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Abstime_Past: {
      /*
       * While the `abs_timeout` parameter references an object of type struct
       * timespec specifying a valid time point which is before or at the
       * current time of the CLOCK_REALTIME.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = 0;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Abstime_Invalid: {
      /*
       * While the `abs_timeout` parameter references an object of type struct
       * timespec whose nanoseconds value is invalid.
       */
      ctx->abstime_obj.tv_sec = 1621322302;
      ctx->abstime_obj.tv_nsec = -1;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Abstime_Null: {
      /*
       * While the `abs_timeout` parameter is NULL.
       */
      ctx->abs_timeout = NULL;
      break;
    }

    case CMqueueReqTimedreceive_Pre_Abstime_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Status_Check(
  CMqueueReqTimedreceive_Context    *ctx,
  CMqueueReqTimedreceive_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Status_Length: {
      /*
       * The return value of mq_timedreceive() shall be equal to the number of
       * bytes in the received message.
       */
      T_eq_long( ctx->retval, MESSAGE_SIZE );
      break;
    }

    case CMqueueReqTimedreceive_Post_Status_MinusOne: {
      /*
       * The return value of mq_timedreceive() shall be equal to minus one.
       */
      T_eq_long( ctx->retval, -1 );
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
    case CMqueueReqTimedreceive_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_timedreceive() call.
       */
      T_eq_int( ctx->eno, -1 );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_EAGAIN: {
      /*
       * The errno shall be set to EAGAIN.
       */
      T_eq_int( ctx->eno, EAGAIN );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_EBADF: {
      /*
       * The errno shall be set to EBADF.
       */
      T_eq_int( ctx->eno, EBADF );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_EINVAL: {
      /*
       * The errno shall be set to EINVAL.
       */
      T_eq_int( ctx->eno, EINVAL );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_EMSGSIZE: {
      /*
       * The errno shall be set to EMSGSIZE.
       */
      T_eq_int( ctx->eno, EMSGSIZE );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_ETIMEDOUT: {
      /*
       * The errno shall be set to ETIMEDOUT.
       */
      T_eq_int( ctx->eno, ETIMEDOUT );
      break;
    }

    case CMqueueReqTimedreceive_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Msg_Check(
  CMqueueReqTimedreceive_Context *ctx,
  CMqueueReqTimedreceive_Post_Msg state
)
{
  int i;

  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Msg_Set: {
      /*
       * The first bytes of the buffer referenced by the `msg_ptr` parameter
       * shall contain a copy of the received message.
       */
      T_eq_mem( ctx->buf, message, MESSAGE_SIZE );
      break;
    }

    case CMqueueReqTimedreceive_Post_Msg_Nop: {
      /*
       * The buffer referenced by the `msg_ptr` parameter shall not be changed
       * by the mq_timedreceive() call.
       */
      for ( i = 0; i < MSG_SIZE; ++i ) {
        T_eq_u8( ctx->buf[ i ], UINT8_MAX );
      }
      break;
    }

    case CMqueueReqTimedreceive_Post_Msg_NA:
      break;
  }
}

static void CMqueueReqTimedreceive_Post_Prio_Check(
  CMqueueReqTimedreceive_Context  *ctx,
  CMqueueReqTimedreceive_Post_Prio state
)
{
  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Prio_Set: {
      /*
       * The object referenced by the `msg_prio` parameter shall be set to the
       * message priority of the received message.
       */
      T_eq_u32( ctx->prio_obj, MESSAGE_PRIO );
      break;
    }

    case CMqueueReqTimedreceive_Post_Prio_Nop: {
      /*
       * The object referenced by the `msg_prio` parameter shall not be changed
       * by the mq_timedreceive() call.
       */
      T_eq_u32( ctx->prio_obj, UINT_MAX );
      break;
    }

    case CMqueueReqTimedreceive_Post_Prio_NA:
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

static void CMqueueReqTimedreceive_Post_Queue_Check(
  CMqueueReqTimedreceive_Context   *ctx,
  CMqueueReqTimedreceive_Post_Queue state
)
{
  mqd_t          mq;
  struct mq_attr attr;

  mq = mq_open( QUEUE_NAME, O_RDWR );
  T_ne_u32( mq, (mqd_t) -1 );
  T_eq_int( mq_getattr( mq, &attr ), 0 );

  switch ( state ) {
    case CMqueueReqTimedreceive_Post_Queue_Removed: {
      /*
       * The received message shall be removed from the message queue specified
       * by the `mqdes` parameter.
       */
      T_eq_long( attr.mq_curmsgs, 0 );
      break;
    }

    case CMqueueReqTimedreceive_Post_Queue_Nop: {
      /*
       * The message queue shall not be changed by the mq_timedreceive() call.
       */
      T_eq_long( attr.mq_curmsgs, ctx->curmsgs );
      break;
    }

    case CMqueueReqTimedreceive_Post_Queue_NA:
      break;
  }

  T_eq_int( mq_close( mq ), 0 );
}

static void CMqueueReqTimedreceive_Setup( CMqueueReqTimedreceive_Context *ctx )
{
  rtems_time_of_day now = { 1988, 1, 1, 0, 0, 0, 0 };

  T_rsc_success( rtems_clock_set( &now ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
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
  ctx->retval = -1;
  ctx->eno = 0;
  ctx->curmsgs = 0;
  memset( ctx->buf, UINT8_MAX, sizeof( ctx->buf ) );
  ctx->prio_obj = UINT_MAX;
  ctx->msg_ptr = ctx->buf;
  ctx->abs_timeout = &ctx->abstime_obj;
  ctx->abstime_obj.tv_sec = 1621322302;
  ctx->abstime_obj.tv_nsec = 0;
}

static void CMqueueReqTimedreceive_Action(
  CMqueueReqTimedreceive_Context *ctx
)
{
  ResumeTask( ctx->worker_id );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );
  FinalClockTick();
}

static void CMqueueReqTimedreceive_Cleanup(
  CMqueueReqTimedreceive_Context *ctx
)
{
  (void) mq_close( ctx->mq );
  (void) mq_unlink( QUEUE_NAME );
}

static const CMqueueReqTimedreceive_Entry
CMqueueReqTimedreceive_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_EBADF,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_EBADF,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_EMSGSIZE,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_EMSGSIZE,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedreceive_Post_Status_Length,
    CMqueueReqTimedreceive_Post_Errno_Unchanged,
    CMqueueReqTimedreceive_Post_Msg_Set, CMqueueReqTimedreceive_Post_Prio_Set,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Removed },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedreceive_Post_Status_Length,
    CMqueueReqTimedreceive_Post_Errno_Unchanged,
    CMqueueReqTimedreceive_Post_Msg_Set, CMqueueReqTimedreceive_Post_Prio_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Removed },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_EAGAIN,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_EAGAIN,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_EINVAL,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_EINVAL,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_ETIMEDOUT,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_Nop,
    CMqueueReqTimedreceive_Post_Timer_Realtime,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_ETIMEDOUT,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_Nop,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_ETIMEDOUT,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_NA,
    CMqueueReqTimedreceive_Post_Timer_Realtime,
    CMqueueReqTimedreceive_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedreceive_Post_Status_MinusOne,
    CMqueueReqTimedreceive_Post_Errno_ETIMEDOUT,
    CMqueueReqTimedreceive_Post_Msg_Nop, CMqueueReqTimedreceive_Post_Prio_NA,
    CMqueueReqTimedreceive_Post_Timer_Inactive,
    CMqueueReqTimedreceive_Post_Queue_Nop }
};

static const uint8_t
CMqueueReqTimedreceive_Map[] = {
  10, 11, 8, 8, 12, 13, 9, 9, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1,
  2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 4, 4,
  4, 4, 5, 5, 5, 5, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
  3, 3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 6, 6, 6, 6, 7, 7,
  7, 7, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 4, 4, 4, 4, 5, 5, 5, 5, 2, 2,
  2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0,
  1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1,
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  1, 1, 1, 1
};

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
  return CMqueueReqTimedreceive_Entries[
    CMqueueReqTimedreceive_Map[ index ]
  ];
}

static void CMqueueReqTimedreceive_SetPreConditionStates(
  CMqueueReqTimedreceive_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];

  if ( ctx->Map.entry.Pre_Abstime_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqTimedreceive_Pre_Abstime_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }
}

static void CMqueueReqTimedreceive_TestVariant(
  CMqueueReqTimedreceive_Context *ctx
)
{
  CMqueueReqTimedreceive_Pre_Flags_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqTimedreceive_Pre_Messages_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqTimedreceive_Pre_Mqdes_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqTimedreceive_Pre_MsgLen_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqTimedreceive_Pre_MsgPrio_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqTimedreceive_Pre_Abstime_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqTimedreceive_Action( ctx );
  CMqueueReqTimedreceive_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqTimedreceive_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqTimedreceive_Post_Msg_Check( ctx, ctx->Map.entry.Post_Msg );
  CMqueueReqTimedreceive_Post_Prio_Check( ctx, ctx->Map.entry.Post_Prio );
  CMqueueReqTimedreceive_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  CMqueueReqTimedreceive_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
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
    ctx->Map.pci[ 0 ] = CMqueueReqTimedreceive_Pre_Flags_Blocking;
    ctx->Map.pci[ 0 ] < CMqueueReqTimedreceive_Pre_Flags_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqTimedreceive_Pre_Messages_None;
      ctx->Map.pci[ 1 ] < CMqueueReqTimedreceive_Pre_Messages_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqTimedreceive_Pre_Mqdes_Valid;
        ctx->Map.pci[ 2 ] < CMqueueReqTimedreceive_Pre_Mqdes_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqTimedreceive_Pre_MsgLen_Enough;
          ctx->Map.pci[ 3 ] < CMqueueReqTimedreceive_Pre_MsgLen_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqTimedreceive_Pre_MsgPrio_Valid;
            ctx->Map.pci[ 4 ] < CMqueueReqTimedreceive_Pre_MsgPrio_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqTimedreceive_Pre_Abstime_Future;
              ctx->Map.pci[ 5 ] < CMqueueReqTimedreceive_Pre_Abstime_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = CMqueueReqTimedreceive_PopEntry( ctx );
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

/** @} */
