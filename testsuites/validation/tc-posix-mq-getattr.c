/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqGetattr
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
#include <mqueue.h>
#include <string.h>
#include <rtems/posix/mqueueimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqGetattr spec:/c/mqueue/req/getattr
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CMqueueReqGetattr_Pre_MqStat_Obj,
  CMqueueReqGetattr_Pre_MqStat_Null,
  CMqueueReqGetattr_Pre_MqStat_NA
} CMqueueReqGetattr_Pre_MqStat;

typedef enum {
  CMqueueReqGetattr_Pre_StoredFlags_Block,
  CMqueueReqGetattr_Pre_StoredFlags_NonBlock,
  CMqueueReqGetattr_Pre_StoredFlags_NA
} CMqueueReqGetattr_Pre_StoredFlags;

typedef enum {
  CMqueueReqGetattr_Pre_StoredMaxMsg_One,
  CMqueueReqGetattr_Pre_StoredMaxMsg_Many,
  CMqueueReqGetattr_Pre_StoredMaxMsg_NA
} CMqueueReqGetattr_Pre_StoredMaxMsg;

typedef enum {
  CMqueueReqGetattr_Pre_StoredMsgSize_One,
  CMqueueReqGetattr_Pre_StoredMsgSize_Many,
  CMqueueReqGetattr_Pre_StoredMsgSize_NA
} CMqueueReqGetattr_Pre_StoredMsgSize;

typedef enum {
  CMqueueReqGetattr_Pre_MqDes_Valid,
  CMqueueReqGetattr_Pre_MqDes_Invalid,
  CMqueueReqGetattr_Pre_MqDes_NA
} CMqueueReqGetattr_Pre_MqDes;

typedef enum {
  CMqueueReqGetattr_Pre_StoredCurMsgs_Zero,
  CMqueueReqGetattr_Pre_StoredCurMsgs_One,
  CMqueueReqGetattr_Pre_StoredCurMsgs_NA
} CMqueueReqGetattr_Pre_StoredCurMsgs;

typedef enum {
  CMqueueReqGetattr_Pre_Open_Yes,
  CMqueueReqGetattr_Pre_Open_No,
  CMqueueReqGetattr_Pre_Open_NA
} CMqueueReqGetattr_Pre_Open;

typedef enum {
  CMqueueReqGetattr_Post_Status_Zero,
  CMqueueReqGetattr_Post_Status_MinusOne,
  CMqueueReqGetattr_Post_Status_NA
} CMqueueReqGetattr_Post_Status;

typedef enum {
  CMqueueReqGetattr_Post_Errno_Nop,
  CMqueueReqGetattr_Post_Errno_Badf,
  CMqueueReqGetattr_Post_Errno_Inval,
  CMqueueReqGetattr_Post_Errno_NA
} CMqueueReqGetattr_Post_Errno;

typedef enum {
  CMqueueReqGetattr_Post_Flags_Block,
  CMqueueReqGetattr_Post_Flags_NonBlock,
  CMqueueReqGetattr_Post_Flags_Nop,
  CMqueueReqGetattr_Post_Flags_NA
} CMqueueReqGetattr_Post_Flags;

typedef enum {
  CMqueueReqGetattr_Post_MaxMsg_One,
  CMqueueReqGetattr_Post_MaxMsg_Many,
  CMqueueReqGetattr_Post_MaxMsg_Nop,
  CMqueueReqGetattr_Post_MaxMsg_NA
} CMqueueReqGetattr_Post_MaxMsg;

typedef enum {
  CMqueueReqGetattr_Post_MsgSize_One,
  CMqueueReqGetattr_Post_MsgSize_Many,
  CMqueueReqGetattr_Post_MsgSize_Nop,
  CMqueueReqGetattr_Post_MsgSize_NA
} CMqueueReqGetattr_Post_MsgSize;

typedef enum {
  CMqueueReqGetattr_Post_CurMsgs_Zero,
  CMqueueReqGetattr_Post_CurMsgs_One,
  CMqueueReqGetattr_Post_CurMsgs_Nop,
  CMqueueReqGetattr_Post_CurMsgs_NA
} CMqueueReqGetattr_Post_CurMsgs;

typedef enum {
  CMqueueReqGetattr_Post_Queue_Nop,
  CMqueueReqGetattr_Post_Queue_NA
} CMqueueReqGetattr_Post_Queue;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_MqStat_NA : 1;
  uint32_t Pre_StoredFlags_NA : 1;
  uint32_t Pre_StoredMaxMsg_NA : 1;
  uint32_t Pre_StoredMsgSize_NA : 1;
  uint32_t Pre_MqDes_NA : 1;
  uint32_t Pre_StoredCurMsgs_NA : 1;
  uint32_t Pre_Open_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 2;
  uint32_t Post_Flags : 2;
  uint32_t Post_MaxMsg : 2;
  uint32_t Post_MsgSize : 2;
  uint32_t Post_CurMsgs : 2;
  uint32_t Post_Queue : 1;
} CMqueueReqGetattr_Entry;

typedef struct {
  int      oflag;
  uint32_t open_count;
  bool     linked;
  uint32_t maximum_pending_messages;
  uint32_t number_of_pending_messages;
  size_t   maximum_message_size;
} QueueState;

/**
 * @brief Test context for spec:/c/mqueue/req/getattr test case.
 */
typedef struct {
  /**
   * @brief This member contains the flags to create the message queue.
   */
  int oflag;

  /**
   * @brief This member contains the message queue attributes to create the
   *   message queue.
   */
  struct mq_attr attr;

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
   * @brief This member contains the state of the message queue before the
   *   mq_getattr() call.
   */
  QueueState queue_before;

  /**
   * @brief This member provides the object referenced by the `mqstat`
   *   parameter.
   */
  struct mq_attr mqstat_obj;

  /**
   * @brief This member specifies the `mqdes` parameter value.
   */
  mqd_t mqdes;

  /**
   * @brief This member specifies the `mqstat` parameter value.
   */
  struct mq_attr *mqstat;

  /**
   * @brief This member contains the return value of the mq_getattr() call.
   */
  int status;

  /**
   * @brief This member contains the value of errno after the mq_getattr()
   *   call.
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
    CMqueueReqGetattr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqGetattr_Context;

static CMqueueReqGetattr_Context CMqueueReqGetattr_Instance;

static const char *const CMqueueReqGetattr_PreDesc_MqStat[] =
  { "Obj", "Null", "NA" };

static const char *const CMqueueReqGetattr_PreDesc_StoredFlags[] =
  { "Block", "NonBlock", "NA" };

static const char *const CMqueueReqGetattr_PreDesc_StoredMaxMsg[] =
  { "One", "Many", "NA" };

static const char *const CMqueueReqGetattr_PreDesc_StoredMsgSize[] =
  { "One", "Many", "NA" };

static const char *const CMqueueReqGetattr_PreDesc_MqDes[] =
  { "Valid", "Invalid", "NA" };

static const char *const CMqueueReqGetattr_PreDesc_StoredCurMsgs[] =
  { "Zero", "One", "NA" };

static const char *const CMqueueReqGetattr_PreDesc_Open[] =
  { "Yes", "No", "NA" };

static const char *const *const CMqueueReqGetattr_PreDesc[] = {
  CMqueueReqGetattr_PreDesc_MqStat,
  CMqueueReqGetattr_PreDesc_StoredFlags,
  CMqueueReqGetattr_PreDesc_StoredMaxMsg,
  CMqueueReqGetattr_PreDesc_StoredMsgSize,
  CMqueueReqGetattr_PreDesc_MqDes,
  CMqueueReqGetattr_PreDesc_StoredCurMsgs,
  CMqueueReqGetattr_PreDesc_Open,
  NULL
};

#define NAME "/mq-getattr"

#define ERRNO_POISON -1

#define FLAGS_BLOCK ( O_CREAT | O_EXCL | O_RDWR )

#define FLAGS_NON_BLOCK ( O_CREAT | O_EXCL | O_RDWR | O_NONBLOCK )

#define MAX_MSG 3

#define MSG_SIZE 8

static POSIX_Message_queue_Control *GetQueue( mqd_t mqdes )
{
  return (POSIX_Message_queue_Control *) _Objects_Get_no_protection(
    (Objects_Id) mqdes,
    &_POSIX_Message_queue_Information
  );
}

static void GetQueueState(
  const POSIX_Message_queue_Control *queue,
  QueueState                        *state
)
{
  memset( state, 0, sizeof( *state ) );
  state->oflag = queue->oflag;
  state->open_count = queue->open_count;
  state->linked = queue->linked;
  state->maximum_pending_messages = queue->Message_queue
                                      .maximum_pending_messages;
  state->number_of_pending_messages = queue->Message_queue
                                        .number_of_pending_messages;
  state->maximum_message_size = queue->Message_queue.maximum_message_size;
}

static void CMqueueReqGetattr_Pre_MqStat_Prepare(
  CMqueueReqGetattr_Context   *ctx,
  CMqueueReqGetattr_Pre_MqStat state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_MqStat_Obj: {
      /*
       * While the `mqstat` parameter references an object of type struct
       * mq_attr.
       */
      ctx->mqstat = &ctx->mqstat_obj;
      break;
    }

    case CMqueueReqGetattr_Pre_MqStat_Null: {
      /*
       * While the `mqstat` parameter is NULL.
       */
      ctx->mqstat = NULL;
      break;
    }

    case CMqueueReqGetattr_Pre_MqStat_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_StoredFlags_Prepare(
  CMqueueReqGetattr_Context        *ctx,
  CMqueueReqGetattr_Pre_StoredFlags state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_StoredFlags_Block: {
      /*
       * While the flags of the message queue do not contain O_NONBLOCK.
       */
      ctx->oflag = FLAGS_BLOCK;
      break;
    }

    case CMqueueReqGetattr_Pre_StoredFlags_NonBlock: {
      /*
       * While the flags of the message queue contain O_NONBLOCK.
       */
      ctx->oflag = FLAGS_NON_BLOCK;
      break;
    }

    case CMqueueReqGetattr_Pre_StoredFlags_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_StoredMaxMsg_Prepare(
  CMqueueReqGetattr_Context         *ctx,
  CMqueueReqGetattr_Pre_StoredMaxMsg state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_StoredMaxMsg_One: {
      /*
       * While the maximum number of messages of the message queue is one.
       */
      ctx->attr.mq_maxmsg = 1;
      break;
    }

    case CMqueueReqGetattr_Pre_StoredMaxMsg_Many: {
      /*
       * While the maximum number of messages of the message queue is greater
       * than one.
       */
      ctx->attr.mq_maxmsg = MAX_MSG;
      break;
    }

    case CMqueueReqGetattr_Pre_StoredMaxMsg_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_StoredMsgSize_Prepare(
  CMqueueReqGetattr_Context          *ctx,
  CMqueueReqGetattr_Pre_StoredMsgSize state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_StoredMsgSize_One: {
      /*
       * While the maximum message size of the message queue is one.
       */
      ctx->attr.mq_msgsize = 1;
      break;
    }

    case CMqueueReqGetattr_Pre_StoredMsgSize_Many: {
      /*
       * While the maximum message size of the message queue is greater than
       * one.
       */
      ctx->attr.mq_msgsize = MSG_SIZE;
      break;
    }

    case CMqueueReqGetattr_Pre_StoredMsgSize_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_MqDes_Prepare(
  CMqueueReqGetattr_Context  *ctx,
  CMqueueReqGetattr_Pre_MqDes state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_MqDes_Valid: {
      /*
       * While the `mqdes` parameter is the message queue descriptor of a
       * message queue.
       */
      ctx->queue_id = mq_open( NAME, ctx->oflag, 0, &ctx->attr );
      T_ne_u32( ctx->queue_id, (mqd_t) -1 );
      ctx->queue = GetQueue( ctx->queue_id );
      T_assert_not_null( ctx->queue );
      ctx->mqdes = ctx->queue_id;
      break;
    }

    case CMqueueReqGetattr_Pre_MqDes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = (mqd_t) INVALID_ID;
      break;
    }

    case CMqueueReqGetattr_Pre_MqDes_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_StoredCurMsgs_Prepare(
  CMqueueReqGetattr_Context          *ctx,
  CMqueueReqGetattr_Pre_StoredCurMsgs state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_StoredCurMsgs_Zero: {
      /*
       * While the message queue contains no message.
       */
      /* The message queue was created without messages */
      break;
    }

    case CMqueueReqGetattr_Pre_StoredCurMsgs_One: {
      /*
       * While the message queue contains exactly one message.
       */
      T_eq_int( mq_send( ctx->queue_id, "M", 1, 0 ), 0 );
      break;
    }

    case CMqueueReqGetattr_Pre_StoredCurMsgs_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_Open_Prepare(
  CMqueueReqGetattr_Context *ctx,
  CMqueueReqGetattr_Pre_Open state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_Open_Yes: {
      /*
       * While the message queue is open.
       */
      /* The message queue was opened by its creation */
      break;
    }

    case CMqueueReqGetattr_Pre_Open_No: {
      /*
       * While the message queue is not open.
       */
      T_eq_int( mq_close( ctx->queue_id ), 0 );
      break;
    }

    case CMqueueReqGetattr_Pre_Open_NA:
      break;
  }
}

static void CMqueueReqGetattr_Post_Status_Check(
  CMqueueReqGetattr_Context    *ctx,
  CMqueueReqGetattr_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Post_Status_Zero: {
      /*
       * The return value of mq_getattr() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CMqueueReqGetattr_Post_Status_MinusOne: {
      /*
       * The return value of mq_getattr() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CMqueueReqGetattr_Post_Status_NA:
      break;
  }
}

static void CMqueueReqGetattr_Post_Errno_Check(
  CMqueueReqGetattr_Context   *ctx,
  CMqueueReqGetattr_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified by the mq_getattr() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CMqueueReqGetattr_Post_Errno_Badf: {
      /*
       * The value of errno shall be EBADF.
       */
      T_eq_int( ctx->errno_value, EBADF );
      break;
    }

    case CMqueueReqGetattr_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CMqueueReqGetattr_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqGetattr_Post_Flags_Check(
  CMqueueReqGetattr_Context   *ctx,
  CMqueueReqGetattr_Post_Flags state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Post_Flags_Block: {
      /*
       * The mq_flags member of the object referenced by the `mqstat` parameter
       * shall be the flags of the message queue, which do not contain
       * O_NONBLOCK.
       */
      T_eq_long( ctx->mqstat_obj.mq_flags, FLAGS_BLOCK );
      break;
    }

    case CMqueueReqGetattr_Post_Flags_NonBlock: {
      /*
       * The mq_flags member of the object referenced by the `mqstat` parameter
       * shall be the flags of the message queue, which contain O_NONBLOCK.
       */
      T_eq_long( ctx->mqstat_obj.mq_flags, FLAGS_NON_BLOCK );
      break;
    }

    case CMqueueReqGetattr_Post_Flags_Nop: {
      /*
       * The mq_flags member of the object referenced by the `mqstat` parameter
       * shall not be modified.
       */
      T_eq_long( ctx->mqstat_obj.mq_flags, -1 );
      break;
    }

    case CMqueueReqGetattr_Post_Flags_NA:
      break;
  }
}

static void CMqueueReqGetattr_Post_MaxMsg_Check(
  CMqueueReqGetattr_Context    *ctx,
  CMqueueReqGetattr_Post_MaxMsg state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Post_MaxMsg_One: {
      /*
       * The mq_maxmsg member of the object referenced by the `mqstat`
       * parameter shall be one.
       */
      T_eq_long( ctx->mqstat_obj.mq_maxmsg, 1 );
      break;
    }

    case CMqueueReqGetattr_Post_MaxMsg_Many: {
      /*
       * The mq_maxmsg member of the object referenced by the `mqstat`
       * parameter shall be the maximum number of messages of the message
       * queue.
       */
      T_eq_long( ctx->mqstat_obj.mq_maxmsg, MAX_MSG );
      break;
    }

    case CMqueueReqGetattr_Post_MaxMsg_Nop: {
      /*
       * The mq_maxmsg member of the object referenced by the `mqstat`
       * parameter shall not be modified.
       */
      T_eq_long( ctx->mqstat_obj.mq_maxmsg, -1 );
      break;
    }

    case CMqueueReqGetattr_Post_MaxMsg_NA:
      break;
  }
}

static void CMqueueReqGetattr_Post_MsgSize_Check(
  CMqueueReqGetattr_Context     *ctx,
  CMqueueReqGetattr_Post_MsgSize state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Post_MsgSize_One: {
      /*
       * The mq_msgsize member of the object referenced by the `mqstat`
       * parameter shall be one.
       */
      T_eq_long( ctx->mqstat_obj.mq_msgsize, 1 );
      break;
    }

    case CMqueueReqGetattr_Post_MsgSize_Many: {
      /*
       * The mq_msgsize member of the object referenced by the `mqstat`
       * parameter shall be the maximum message size of the message queue.
       */
      T_eq_long( ctx->mqstat_obj.mq_msgsize, MSG_SIZE );
      break;
    }

    case CMqueueReqGetattr_Post_MsgSize_Nop: {
      /*
       * The mq_msgsize member of the object referenced by the `mqstat`
       * parameter shall not be modified.
       */
      T_eq_long( ctx->mqstat_obj.mq_msgsize, -1 );
      break;
    }

    case CMqueueReqGetattr_Post_MsgSize_NA:
      break;
  }
}

static void CMqueueReqGetattr_Post_CurMsgs_Check(
  CMqueueReqGetattr_Context     *ctx,
  CMqueueReqGetattr_Post_CurMsgs state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Post_CurMsgs_Zero: {
      /*
       * The mq_curmsgs member of the object referenced by the `mqstat`
       * parameter shall be zero.
       */
      T_eq_long( ctx->mqstat_obj.mq_curmsgs, 0 );
      break;
    }

    case CMqueueReqGetattr_Post_CurMsgs_One: {
      /*
       * The mq_curmsgs member of the object referenced by the `mqstat`
       * parameter shall be one.
       */
      T_eq_long( ctx->mqstat_obj.mq_curmsgs, 1 );
      break;
    }

    case CMqueueReqGetattr_Post_CurMsgs_Nop: {
      /*
       * The mq_curmsgs member of the object referenced by the `mqstat`
       * parameter shall not be modified.
       */
      T_eq_long( ctx->mqstat_obj.mq_curmsgs, -1 );
      break;
    }

    case CMqueueReqGetattr_Post_CurMsgs_NA:
      break;
  }
}

static void CMqueueReqGetattr_Post_Queue_Check(
  CMqueueReqGetattr_Context   *ctx,
  CMqueueReqGetattr_Post_Queue state
)
{
  QueueState queue_state;

  switch ( state ) {
    case CMqueueReqGetattr_Post_Queue_Nop: {
      /*
       * The message queue shall not be modified by the mq_getattr() call.
       */
      GetQueueState( ctx->queue, &queue_state );
      T_eq_mem( &queue_state, &ctx->queue_before, sizeof( queue_state ) );
      break;
    }

    case CMqueueReqGetattr_Post_Queue_NA:
      break;
  }
}

static void CMqueueReqGetattr_Prepare( CMqueueReqGetattr_Context *ctx )
{
  ctx->queue = NULL;
  memset( &ctx->attr, 0, sizeof( ctx->attr ) );
}

static void CMqueueReqGetattr_Action( CMqueueReqGetattr_Context *ctx )
{
  memset( &ctx->mqstat_obj, 0xff, sizeof( ctx->mqstat_obj ) );

  if ( ctx->queue != NULL ) {
    GetQueueState( ctx->queue, &ctx->queue_before );
  }

  errno = ERRNO_POISON;
  ctx->status = mq_getattr( ctx->mqdes, ctx->mqstat );
  ctx->errno_value = errno;
}

static void CMqueueReqGetattr_Cleanup( CMqueueReqGetattr_Context *ctx )
{
  if ( ctx->queue != NULL ) {
    if ( ctx->queue->open_count > 0 ) {
      T_eq_int( mq_close( ctx->queue_id ), 0 );
    }

    T_eq_int( mq_unlink( NAME ), 0 );
    ctx->queue = NULL;
  }
}

/* clang-format off */

static const CMqueueReqGetattr_Entry
CMqueueReqGetattr_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_NA,
    CMqueueReqGetattr_Post_Errno_NA, CMqueueReqGetattr_Post_Flags_NA,
    CMqueueReqGetattr_Post_MaxMsg_NA, CMqueueReqGetattr_Post_MsgSize_NA,
    CMqueueReqGetattr_Post_CurMsgs_NA, CMqueueReqGetattr_Post_Queue_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_MinusOne,
    CMqueueReqGetattr_Post_Errno_Inval, CMqueueReqGetattr_Post_Flags_NA,
    CMqueueReqGetattr_Post_MaxMsg_NA, CMqueueReqGetattr_Post_MsgSize_NA,
    CMqueueReqGetattr_Post_CurMsgs_NA, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_MinusOne,
    CMqueueReqGetattr_Post_Errno_Badf, CMqueueReqGetattr_Post_Flags_Nop,
    CMqueueReqGetattr_Post_MaxMsg_Nop, CMqueueReqGetattr_Post_MsgSize_Nop,
    CMqueueReqGetattr_Post_CurMsgs_Nop, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_Block,
    CMqueueReqGetattr_Post_MaxMsg_One, CMqueueReqGetattr_Post_MsgSize_One,
    CMqueueReqGetattr_Post_CurMsgs_Zero, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_Block,
    CMqueueReqGetattr_Post_MaxMsg_One, CMqueueReqGetattr_Post_MsgSize_One,
    CMqueueReqGetattr_Post_CurMsgs_One, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 1, 1, 1, 0, 1, 1, CMqueueReqGetattr_Post_Status_MinusOne,
    CMqueueReqGetattr_Post_Errno_Badf, CMqueueReqGetattr_Post_Flags_Nop,
    CMqueueReqGetattr_Post_MaxMsg_Nop, CMqueueReqGetattr_Post_MsgSize_Nop,
    CMqueueReqGetattr_Post_CurMsgs_Nop, CMqueueReqGetattr_Post_Queue_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_Block,
    CMqueueReqGetattr_Post_MaxMsg_One, CMqueueReqGetattr_Post_MsgSize_Many,
    CMqueueReqGetattr_Post_CurMsgs_Zero, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_Block,
    CMqueueReqGetattr_Post_MaxMsg_One, CMqueueReqGetattr_Post_MsgSize_Many,
    CMqueueReqGetattr_Post_CurMsgs_One, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_Block,
    CMqueueReqGetattr_Post_MaxMsg_Many, CMqueueReqGetattr_Post_MsgSize_One,
    CMqueueReqGetattr_Post_CurMsgs_Zero, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_Block,
    CMqueueReqGetattr_Post_MaxMsg_Many, CMqueueReqGetattr_Post_MsgSize_One,
    CMqueueReqGetattr_Post_CurMsgs_One, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_Block,
    CMqueueReqGetattr_Post_MaxMsg_Many, CMqueueReqGetattr_Post_MsgSize_Many,
    CMqueueReqGetattr_Post_CurMsgs_Zero, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_Block,
    CMqueueReqGetattr_Post_MaxMsg_Many, CMqueueReqGetattr_Post_MsgSize_Many,
    CMqueueReqGetattr_Post_CurMsgs_One, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_NonBlock,
    CMqueueReqGetattr_Post_MaxMsg_One, CMqueueReqGetattr_Post_MsgSize_One,
    CMqueueReqGetattr_Post_CurMsgs_Zero, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_NonBlock,
    CMqueueReqGetattr_Post_MaxMsg_One, CMqueueReqGetattr_Post_MsgSize_One,
    CMqueueReqGetattr_Post_CurMsgs_One, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_NonBlock,
    CMqueueReqGetattr_Post_MaxMsg_One, CMqueueReqGetattr_Post_MsgSize_Many,
    CMqueueReqGetattr_Post_CurMsgs_Zero, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_NonBlock,
    CMqueueReqGetattr_Post_MaxMsg_One, CMqueueReqGetattr_Post_MsgSize_Many,
    CMqueueReqGetattr_Post_CurMsgs_One, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_NonBlock,
    CMqueueReqGetattr_Post_MaxMsg_Many, CMqueueReqGetattr_Post_MsgSize_One,
    CMqueueReqGetattr_Post_CurMsgs_Zero, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_NonBlock,
    CMqueueReqGetattr_Post_MaxMsg_Many, CMqueueReqGetattr_Post_MsgSize_One,
    CMqueueReqGetattr_Post_CurMsgs_One, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_NonBlock,
    CMqueueReqGetattr_Post_MaxMsg_Many, CMqueueReqGetattr_Post_MsgSize_Many,
    CMqueueReqGetattr_Post_CurMsgs_Zero, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Nop, CMqueueReqGetattr_Post_Flags_NonBlock,
    CMqueueReqGetattr_Post_MaxMsg_Many, CMqueueReqGetattr_Post_MsgSize_Many,
    CMqueueReqGetattr_Post_CurMsgs_One, CMqueueReqGetattr_Post_Queue_Nop },
  { 0, 0, 1, 1, 1, 0, 1, 1, CMqueueReqGetattr_Post_Status_MinusOne,
    CMqueueReqGetattr_Post_Errno_Inval, CMqueueReqGetattr_Post_Flags_NA,
    CMqueueReqGetattr_Post_MaxMsg_NA, CMqueueReqGetattr_Post_MsgSize_NA,
    CMqueueReqGetattr_Post_CurMsgs_NA, CMqueueReqGetattr_Post_Queue_NA }
};

static const uint8_t
CMqueueReqGetattr_Map[] = {
  3, 2, 4, 2, 5, 0, 0, 0, 6, 2, 7, 2, 0, 0, 0, 0, 8, 2, 9, 2, 0, 0, 0, 0, 10,
  2, 11, 2, 0, 0, 0, 0, 12, 2, 13, 2, 0, 0, 0, 0, 14, 2, 15, 2, 0, 0, 0, 0, 16,
  2, 17, 2, 0, 0, 0, 0, 18, 2, 19, 2, 0, 0, 0, 0, 1, 1, 1, 1, 20, 0, 0, 0, 1,
  1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1,
  1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0,
  0, 0, 0
};

/* clang-format on */

static size_t CMqueueReqGetattr_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqGetattr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqGetattr_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqGetattr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CMqueueReqGetattr_Scope,
  .initial_context = &CMqueueReqGetattr_Instance
};

static inline CMqueueReqGetattr_Entry CMqueueReqGetattr_PopEntry(
  CMqueueReqGetattr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqGetattr_Entries[ CMqueueReqGetattr_Map[ index ] ];
}

static void CMqueueReqGetattr_SetPreConditionStates(
  CMqueueReqGetattr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_StoredFlags_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqGetattr_Pre_StoredFlags_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_StoredMaxMsg_NA ) {
    ctx->Map.pcs[ 2 ] = CMqueueReqGetattr_Pre_StoredMaxMsg_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_StoredMsgSize_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqGetattr_Pre_StoredMsgSize_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];

  if ( ctx->Map.entry.Pre_StoredCurMsgs_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqGetattr_Pre_StoredCurMsgs_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 6 ] = CMqueueReqGetattr_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }
}

static void CMqueueReqGetattr_TestVariant( CMqueueReqGetattr_Context *ctx )
{
  CMqueueReqGetattr_Pre_MqStat_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqGetattr_Pre_StoredFlags_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqGetattr_Pre_StoredMaxMsg_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqGetattr_Pre_StoredMsgSize_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqGetattr_Pre_MqDes_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqGetattr_Pre_StoredCurMsgs_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqGetattr_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CMqueueReqGetattr_Action( ctx );
  CMqueueReqGetattr_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqGetattr_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqGetattr_Post_Flags_Check( ctx, ctx->Map.entry.Post_Flags );
  CMqueueReqGetattr_Post_MaxMsg_Check( ctx, ctx->Map.entry.Post_MaxMsg );
  CMqueueReqGetattr_Post_MsgSize_Check( ctx, ctx->Map.entry.Post_MsgSize );
  CMqueueReqGetattr_Post_CurMsgs_Check( ctx, ctx->Map.entry.Post_CurMsgs );
  CMqueueReqGetattr_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
}

/**
 * @fn void T_case_body_CMqueueReqGetattr( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqGetattr, &CMqueueReqGetattr_Fixture )
{
  CMqueueReqGetattr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqGetattr_Pre_MqStat_Obj;
    ctx->Map.pci[ 0 ] < CMqueueReqGetattr_Pre_MqStat_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqGetattr_Pre_StoredFlags_Block;
      ctx->Map.pci[ 1 ] < CMqueueReqGetattr_Pre_StoredFlags_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqGetattr_Pre_StoredMaxMsg_One;
        ctx->Map.pci[ 2 ] < CMqueueReqGetattr_Pre_StoredMaxMsg_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqGetattr_Pre_StoredMsgSize_One;
          ctx->Map.pci[ 3 ] < CMqueueReqGetattr_Pre_StoredMsgSize_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqGetattr_Pre_MqDes_Valid;
            ctx->Map.pci[ 4 ] < CMqueueReqGetattr_Pre_MqDes_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqGetattr_Pre_StoredCurMsgs_Zero;
              ctx->Map.pci[ 5 ] < CMqueueReqGetattr_Pre_StoredCurMsgs_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CMqueueReqGetattr_Pre_Open_Yes;
                ctx->Map.pci[ 6 ] < CMqueueReqGetattr_Pre_Open_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                ctx->Map.entry = CMqueueReqGetattr_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                CMqueueReqGetattr_SetPreConditionStates( ctx );
                CMqueueReqGetattr_Prepare( ctx );
                CMqueueReqGetattr_TestVariant( ctx );
                CMqueueReqGetattr_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
