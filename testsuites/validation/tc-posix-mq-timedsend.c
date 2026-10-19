/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqTimedsend
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
#include <time.h>
#include <sys/stat.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqTimedsend spec:/c/mqueue/req/timedsend
 *
 * @{
 */

typedef enum {
  CMqueueReqTimedsend_Pre_Flags_Blocking,
  CMqueueReqTimedsend_Pre_Flags_NonBlock,
  CMqueueReqTimedsend_Pre_Flags_ReadOnly,
  CMqueueReqTimedsend_Pre_Flags_NA
} CMqueueReqTimedsend_Pre_Flags;

typedef enum {
  CMqueueReqTimedsend_Pre_Full_Yes,
  CMqueueReqTimedsend_Pre_Full_No,
  CMqueueReqTimedsend_Pre_Full_NA
} CMqueueReqTimedsend_Pre_Full;

typedef enum {
  CMqueueReqTimedsend_Pre_Mqdes_Valid,
  CMqueueReqTimedsend_Pre_Mqdes_Closed,
  CMqueueReqTimedsend_Pre_Mqdes_Invalid,
  CMqueueReqTimedsend_Pre_Mqdes_NA
} CMqueueReqTimedsend_Pre_Mqdes;

typedef enum {
  CMqueueReqTimedsend_Pre_MsgLen_Fits,
  CMqueueReqTimedsend_Pre_MsgLen_TooLong,
  CMqueueReqTimedsend_Pre_MsgLen_NA
} CMqueueReqTimedsend_Pre_MsgLen;

typedef enum {
  CMqueueReqTimedsend_Pre_MsgPrio_Valid,
  CMqueueReqTimedsend_Pre_MsgPrio_Invalid,
  CMqueueReqTimedsend_Pre_MsgPrio_NA
} CMqueueReqTimedsend_Pre_MsgPrio;

typedef enum {
  CMqueueReqTimedsend_Pre_Abstime_Future,
  CMqueueReqTimedsend_Pre_Abstime_Past,
  CMqueueReqTimedsend_Pre_Abstime_Invalid,
  CMqueueReqTimedsend_Pre_Abstime_Null,
  CMqueueReqTimedsend_Pre_Abstime_NA
} CMqueueReqTimedsend_Pre_Abstime;

typedef enum {
  CMqueueReqTimedsend_Post_Status_Zero,
  CMqueueReqTimedsend_Post_Status_MinusOne,
  CMqueueReqTimedsend_Post_Status_NA
} CMqueueReqTimedsend_Post_Status;

typedef enum {
  CMqueueReqTimedsend_Post_Errno_Unchanged,
  CMqueueReqTimedsend_Post_Errno_EAGAIN,
  CMqueueReqTimedsend_Post_Errno_EBADF,
  CMqueueReqTimedsend_Post_Errno_EINVAL,
  CMqueueReqTimedsend_Post_Errno_EMSGSIZE,
  CMqueueReqTimedsend_Post_Errno_ETIMEDOUT,
  CMqueueReqTimedsend_Post_Errno_NA
} CMqueueReqTimedsend_Post_Errno;

typedef enum {
  CMqueueReqTimedsend_Post_Timer_Inactive,
  CMqueueReqTimedsend_Post_Timer_Realtime,
  CMqueueReqTimedsend_Post_Timer_NA
} CMqueueReqTimedsend_Post_Timer;

typedef enum {
  CMqueueReqTimedsend_Post_Queue_Added,
  CMqueueReqTimedsend_Post_Queue_Nop,
  CMqueueReqTimedsend_Post_Queue_NA
} CMqueueReqTimedsend_Post_Queue;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Flags_NA : 1;
  uint16_t Pre_Full_NA : 1;
  uint16_t Pre_Mqdes_NA : 1;
  uint16_t Pre_MsgLen_NA : 1;
  uint16_t Pre_MsgPrio_NA : 1;
  uint16_t Pre_Abstime_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 3;
  uint16_t Post_Timer : 2;
  uint16_t Post_Queue : 2;
} CMqueueReqTimedsend_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/timedsend test case.
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
   *   before the mq_timedsend() call.
   */
  long curmsgs;

  /**
   * @brief This member provides the object referenced by the `abstime`
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
   * @brief This member specifies the `abstime` parameter value.
   */
  const struct timespec *abstime;

  /**
   * @brief This member contains the return value of the mq_timedsend() call.
   */
  int retval;

  /**
   * @brief This member contains the errno value after the mq_timedsend() call.
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
    CMqueueReqTimedsend_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqTimedsend_Context;

static CMqueueReqTimedsend_Context
  CMqueueReqTimedsend_Instance;

static const char * const CMqueueReqTimedsend_PreDesc_Flags[] = {
  "Blocking",
  "NonBlock",
  "ReadOnly",
  "NA"
};

static const char * const CMqueueReqTimedsend_PreDesc_Full[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const CMqueueReqTimedsend_PreDesc_Mqdes[] = {
  "Valid",
  "Closed",
  "Invalid",
  "NA"
};

static const char * const CMqueueReqTimedsend_PreDesc_MsgLen[] = {
  "Fits",
  "TooLong",
  "NA"
};

static const char * const CMqueueReqTimedsend_PreDesc_MsgPrio[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const CMqueueReqTimedsend_PreDesc_Abstime[] = {
  "Future",
  "Past",
  "Invalid",
  "Null",
  "NA"
};

static const char * const * const CMqueueReqTimedsend_PreDesc[] = {
  CMqueueReqTimedsend_PreDesc_Flags,
  CMqueueReqTimedsend_PreDesc_Full,
  CMqueueReqTimedsend_PreDesc_Mqdes,
  CMqueueReqTimedsend_PreDesc_MsgLen,
  CMqueueReqTimedsend_PreDesc_MsgPrio,
  CMqueueReqTimedsend_PreDesc_Abstime,
  NULL
};

#define QUEUE_NAME "mq"

#define MSG_SIZE 4

static const struct mq_attr queue_attr = { 0, 1, MSG_SIZE, 0 };

static const char message[] = "abcde";

typedef CMqueueReqTimedsend_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    SuspendSelf();
    errno = -1;
    ctx->retval = mq_timedsend(
      ctx->mqdes,
      ctx->msg_ptr,
      ctx->msg_len,
      ctx->msg_prio,
      ctx->abstime
    );
    ctx->eno = errno;
  }
}

static void CMqueueReqTimedsend_Pre_Flags_Prepare(
  CMqueueReqTimedsend_Context  *ctx,
  CMqueueReqTimedsend_Pre_Flags state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Pre_Flags_Blocking: {
      /*
       * While the message queue was opened for writing without the O_NONBLOCK
       * flag.
       */
      ctx->mq = mq_open( QUEUE_NAME, O_CREAT | O_RDWR, S_IRWXU, &queue_attr );
      T_ne_u32( ctx->mq, (mqd_t) -1 );
      break;
    }

    case CMqueueReqTimedsend_Pre_Flags_NonBlock: {
      /*
       * While the message queue was opened for writing with the O_NONBLOCK
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

    case CMqueueReqTimedsend_Pre_Flags_ReadOnly: {
      /*
       * While the message queue was opened for reading only.
       */
      ctx->mq = mq_open( QUEUE_NAME, O_CREAT | O_RDONLY, S_IRWXU, &queue_attr );
      T_ne_u32( ctx->mq, (mqd_t) -1 );
      break;
    }

    case CMqueueReqTimedsend_Pre_Flags_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Pre_Full_Prepare(
  CMqueueReqTimedsend_Context *ctx,
  CMqueueReqTimedsend_Pre_Full state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Pre_Full_Yes: {
      /*
       * While the message queue is full.
       */
      T_eq_int( mq_send( ctx->mq, "x", 1, 0 ), 0 );
      ctx->curmsgs = 1;
      break;
    }

    case CMqueueReqTimedsend_Pre_Full_No: {
      /*
       * While the message queue is not full.
       */
      /* Already prepared */
      break;
    }

    case CMqueueReqTimedsend_Pre_Full_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Pre_Mqdes_Prepare(
  CMqueueReqTimedsend_Context  *ctx,
  CMqueueReqTimedsend_Pre_Mqdes state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Pre_Mqdes_Valid: {
      /*
       * While the `mqdes` parameter specifies a message queue descriptor of an
       * open message queue.
       */
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqTimedsend_Pre_Mqdes_Closed: {
      /*
       * While the `mqdes` parameter specifies a message queue descriptor of a
       * message queue which was closed and not unlinked.
       */
      T_eq_int( mq_close( ctx->mq ), 0 );
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqTimedsend_Pre_Mqdes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = INVALID_ID;
      break;
    }

    case CMqueueReqTimedsend_Pre_Mqdes_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Pre_MsgLen_Prepare(
  CMqueueReqTimedsend_Context   *ctx,
  CMqueueReqTimedsend_Pre_MsgLen state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Pre_MsgLen_Fits: {
      /*
       * While the `msg_len` parameter is less than or equal to the maximum
       * message size of the message queue.
       */
      ctx->msg_len = MSG_SIZE;
      break;
    }

    case CMqueueReqTimedsend_Pre_MsgLen_TooLong: {
      /*
       * While the `msg_len` parameter is greater than the maximum message size
       * of the message queue.
       */
      ctx->msg_len = MSG_SIZE + 1;
      break;
    }

    case CMqueueReqTimedsend_Pre_MsgLen_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Pre_MsgPrio_Prepare(
  CMqueueReqTimedsend_Context    *ctx,
  CMqueueReqTimedsend_Pre_MsgPrio state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Pre_MsgPrio_Valid: {
      /*
       * While the `msg_prio` parameter is less than or equal to MQ_PRIO_MAX.
       */
      ctx->msg_prio = MQ_PRIO_MAX;
      break;
    }

    case CMqueueReqTimedsend_Pre_MsgPrio_Invalid: {
      /*
       * While the `msg_prio` parameter is greater than MQ_PRIO_MAX.
       */
      ctx->msg_prio = MQ_PRIO_MAX + 1;
      break;
    }

    case CMqueueReqTimedsend_Pre_MsgPrio_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Pre_Abstime_Prepare(
  CMqueueReqTimedsend_Context    *ctx,
  CMqueueReqTimedsend_Pre_Abstime state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Pre_Abstime_Future: {
      /*
       * While the `abstime` parameter references an object of type struct
       * timespec specifying a valid time point which is after the current time
       * of the CLOCK_REALTIME.
       */
      ctx->abstime_obj.tv_sec = 1621322302;
      ctx->abstime_obj.tv_nsec = 0;
      break;
    }

    case CMqueueReqTimedsend_Pre_Abstime_Past: {
      /*
       * While the `abstime` parameter references an object of type struct
       * timespec specifying a valid time point which is before or at the
       * current time of the CLOCK_REALTIME.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = 0;
      break;
    }

    case CMqueueReqTimedsend_Pre_Abstime_Invalid: {
      /*
       * While the `abstime` parameter references an object of type struct
       * timespec whose nanoseconds value is invalid.
       */
      ctx->abstime_obj.tv_sec = 1621322302;
      ctx->abstime_obj.tv_nsec = -1;
      break;
    }

    case CMqueueReqTimedsend_Pre_Abstime_Null: {
      /*
       * While the `abstime` parameter is NULL.
       */
      ctx->abstime = NULL;
      break;
    }

    case CMqueueReqTimedsend_Pre_Abstime_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Post_Status_Check(
  CMqueueReqTimedsend_Context    *ctx,
  CMqueueReqTimedsend_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Post_Status_Zero: {
      /*
       * The return value of mq_timedsend() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CMqueueReqTimedsend_Post_Status_MinusOne: {
      /*
       * The return value of mq_timedsend() shall be equal to minus one.
       */
      T_eq_int( ctx->retval, -1 );
      break;
    }

    case CMqueueReqTimedsend_Post_Status_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Post_Errno_Check(
  CMqueueReqTimedsend_Context   *ctx,
  CMqueueReqTimedsend_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_timedsend() call.
       */
      T_eq_int( ctx->eno, -1 );
      break;
    }

    case CMqueueReqTimedsend_Post_Errno_EAGAIN: {
      /*
       * The errno shall be set to EAGAIN.
       */
      T_eq_int( ctx->eno, EAGAIN );
      break;
    }

    case CMqueueReqTimedsend_Post_Errno_EBADF: {
      /*
       * The errno shall be set to EBADF.
       */
      T_eq_int( ctx->eno, EBADF );
      break;
    }

    case CMqueueReqTimedsend_Post_Errno_EINVAL: {
      /*
       * The errno shall be set to EINVAL.
       */
      T_eq_int( ctx->eno, EINVAL );
      break;
    }

    case CMqueueReqTimedsend_Post_Errno_EMSGSIZE: {
      /*
       * The errno shall be set to EMSGSIZE.
       */
      T_eq_int( ctx->eno, EMSGSIZE );
      break;
    }

    case CMqueueReqTimedsend_Post_Errno_ETIMEDOUT: {
      /*
       * The errno shall be set to ETIMEDOUT.
       */
      T_eq_int( ctx->eno, ETIMEDOUT );
      break;
    }

    case CMqueueReqTimedsend_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Post_Timer_Check(
  CMqueueReqTimedsend_Context   *ctx,
  CMqueueReqTimedsend_Post_Timer state
)
{
  switch ( state ) {
    case CMqueueReqTimedsend_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case CMqueueReqTimedsend_Post_Timer_Realtime: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_REALTIME.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_REALTIME );
      break;
    }

    case CMqueueReqTimedsend_Post_Timer_NA:
      break;
  }
}

static void CMqueueReqTimedsend_Post_Queue_Check(
  CMqueueReqTimedsend_Context   *ctx,
  CMqueueReqTimedsend_Post_Queue state
)
{
  char           buf[ MSG_SIZE ];
  mqd_t          mq;
  struct mq_attr attr;
  unsigned int   prio;
  ssize_t        len;

  mq = mq_open( QUEUE_NAME, O_RDWR );
  T_ne_u32( mq, (mqd_t) -1 );
  T_eq_int( mq_getattr( mq, &attr ), 0 );

  switch ( state ) {
    case CMqueueReqTimedsend_Post_Queue_Added: {
      /*
       * The message referenced by the `msg_ptr` parameter with the size in
       * bytes specified by the `msg_len` parameter and the message priority
       * specified by the `msg_prio` parameter shall be appended to the message
       * queue specified by the `mqdes` parameter.
       */
      T_eq_long( attr.mq_curmsgs, 1 );
      len = mq_receive( mq, buf, sizeof( buf ), &prio );
      T_eq_sz( len, MSG_SIZE );
      T_eq_u32( prio, MQ_PRIO_MAX );
      T_eq_mem( buf, message, MSG_SIZE );
      break;
    }

    case CMqueueReqTimedsend_Post_Queue_Nop: {
      /*
       * The message queue shall not be changed by the mq_timedsend() call.
       */
      T_eq_long( attr.mq_curmsgs, ctx->curmsgs );

      if ( ctx->curmsgs > 0 ) {
        len = mq_receive( mq, buf, sizeof( buf ), &prio );
        T_eq_sz( len, 1 );
        T_eq_u32( prio, 0 );
        T_eq_mem( buf, "x", 1 );
      }
      break;
    }

    case CMqueueReqTimedsend_Post_Queue_NA:
      break;
  }

  T_eq_int( mq_close( mq ), 0 );
}

static void CMqueueReqTimedsend_Setup( CMqueueReqTimedsend_Context *ctx )
{
  rtems_time_of_day now = { 1988, 1, 1, 0, 0, 0, 0 };

  T_rsc_success( rtems_clock_set( &now ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void CMqueueReqTimedsend_Setup_Wrap( void *arg )
{
  CMqueueReqTimedsend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqTimedsend_Setup( ctx );
}

static void CMqueueReqTimedsend_Teardown( CMqueueReqTimedsend_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void CMqueueReqTimedsend_Teardown_Wrap( void *arg )
{
  CMqueueReqTimedsend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqTimedsend_Teardown( ctx );
}

static void CMqueueReqTimedsend_Prepare( CMqueueReqTimedsend_Context *ctx )
{
  ctx->retval = -1;
  ctx->eno = 0;
  ctx->curmsgs = 0;
  ctx->msg_ptr = message;
  ctx->abstime = &ctx->abstime_obj;
  ctx->abstime_obj.tv_sec = 1621322302;
  ctx->abstime_obj.tv_nsec = 0;
}

static void CMqueueReqTimedsend_Action( CMqueueReqTimedsend_Context *ctx )
{
  ResumeTask( ctx->worker_id );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );
  FinalClockTick();
}

static void CMqueueReqTimedsend_Cleanup( CMqueueReqTimedsend_Context *ctx )
{
  (void) mq_close( ctx->mq );
  (void) mq_unlink( QUEUE_NAME );
}

static const CMqueueReqTimedsend_Entry
CMqueueReqTimedsend_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_EINVAL,
    CMqueueReqTimedsend_Post_Timer_Inactive, CMqueueReqTimedsend_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_EBADF,
    CMqueueReqTimedsend_Post_Timer_Inactive, CMqueueReqTimedsend_Post_Queue_Nop },
  { 0, 0, 1, 0, 0, 0, 1, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_EBADF,
    CMqueueReqTimedsend_Post_Timer_Inactive, CMqueueReqTimedsend_Post_Queue_Nop },
  { 0, 0, 1, 0, 0, 0, 1, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_EINVAL,
    CMqueueReqTimedsend_Post_Timer_Inactive, CMqueueReqTimedsend_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_EMSGSIZE,
    CMqueueReqTimedsend_Post_Timer_Inactive, CMqueueReqTimedsend_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedsend_Post_Status_Zero,
    CMqueueReqTimedsend_Post_Errno_Unchanged,
    CMqueueReqTimedsend_Post_Timer_Inactive,
    CMqueueReqTimedsend_Post_Queue_Added },
  { 0, 0, 0, 0, 0, 0, 1, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_EAGAIN,
    CMqueueReqTimedsend_Post_Timer_Inactive, CMqueueReqTimedsend_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_EINVAL,
    CMqueueReqTimedsend_Post_Timer_Inactive, CMqueueReqTimedsend_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_ETIMEDOUT,
    CMqueueReqTimedsend_Post_Timer_Realtime, CMqueueReqTimedsend_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CMqueueReqTimedsend_Post_Status_MinusOne,
    CMqueueReqTimedsend_Post_Errno_ETIMEDOUT,
    CMqueueReqTimedsend_Post_Timer_Inactive, CMqueueReqTimedsend_Post_Queue_Nop }
};

static const uint8_t
CMqueueReqTimedsend_Map[] = {
  8, 9, 7, 7, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 5, 5, 5, 5,
  0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0,
  4, 4, 4, 4, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 4, 4, 4, 4,
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3,
  2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2,
  2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2,
  3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3,
  3, 3
};

static size_t CMqueueReqTimedsend_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqTimedsend_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqTimedsend_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqTimedsend_Fixture = {
  .setup = CMqueueReqTimedsend_Setup_Wrap,
  .stop = NULL,
  .teardown = CMqueueReqTimedsend_Teardown_Wrap,
  .scope = CMqueueReqTimedsend_Scope,
  .initial_context = &CMqueueReqTimedsend_Instance
};

static inline CMqueueReqTimedsend_Entry CMqueueReqTimedsend_PopEntry(
  CMqueueReqTimedsend_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqTimedsend_Entries[
    CMqueueReqTimedsend_Map[ index ]
  ];
}

static void CMqueueReqTimedsend_SetPreConditionStates(
  CMqueueReqTimedsend_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Full_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqTimedsend_Pre_Full_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];

  if ( ctx->Map.entry.Pre_Abstime_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqTimedsend_Pre_Abstime_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }
}

static void CMqueueReqTimedsend_TestVariant( CMqueueReqTimedsend_Context *ctx )
{
  CMqueueReqTimedsend_Pre_Flags_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqTimedsend_Pre_Full_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqTimedsend_Pre_Mqdes_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqTimedsend_Pre_MsgLen_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqTimedsend_Pre_MsgPrio_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqTimedsend_Pre_Abstime_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqTimedsend_Action( ctx );
  CMqueueReqTimedsend_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqTimedsend_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqTimedsend_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  CMqueueReqTimedsend_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
}

/**
 * @fn void T_case_body_CMqueueReqTimedsend( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqTimedsend, &CMqueueReqTimedsend_Fixture )
{
  CMqueueReqTimedsend_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqTimedsend_Pre_Flags_Blocking;
    ctx->Map.pci[ 0 ] < CMqueueReqTimedsend_Pre_Flags_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqTimedsend_Pre_Full_Yes;
      ctx->Map.pci[ 1 ] < CMqueueReqTimedsend_Pre_Full_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqTimedsend_Pre_Mqdes_Valid;
        ctx->Map.pci[ 2 ] < CMqueueReqTimedsend_Pre_Mqdes_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqTimedsend_Pre_MsgLen_Fits;
          ctx->Map.pci[ 3 ] < CMqueueReqTimedsend_Pre_MsgLen_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqTimedsend_Pre_MsgPrio_Valid;
            ctx->Map.pci[ 4 ] < CMqueueReqTimedsend_Pre_MsgPrio_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqTimedsend_Pre_Abstime_Future;
              ctx->Map.pci[ 5 ] < CMqueueReqTimedsend_Pre_Abstime_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = CMqueueReqTimedsend_PopEntry( ctx );
              CMqueueReqTimedsend_SetPreConditionStates( ctx );
              CMqueueReqTimedsend_Prepare( ctx );
              CMqueueReqTimedsend_TestVariant( ctx );
              CMqueueReqTimedsend_Cleanup( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
