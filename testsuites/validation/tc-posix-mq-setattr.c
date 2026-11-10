/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqSetattr
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
 * @defgroup CMqueueReqSetattr spec:/c/mqueue/req/setattr
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CMqueueReqSetattr_Pre_MqStat_Obj,
  CMqueueReqSetattr_Pre_MqStat_Null,
  CMqueueReqSetattr_Pre_MqStat_NA
} CMqueueReqSetattr_Pre_MqStat;

typedef enum {
  CMqueueReqSetattr_Pre_NewFlags_Zero,
  CMqueueReqSetattr_Pre_NewFlags_NonBlock,
  CMqueueReqSetattr_Pre_NewFlags_NA
} CMqueueReqSetattr_Pre_NewFlags;

typedef enum {
  CMqueueReqSetattr_Pre_OMqStat_Obj,
  CMqueueReqSetattr_Pre_OMqStat_Null,
  CMqueueReqSetattr_Pre_OMqStat_NA
} CMqueueReqSetattr_Pre_OMqStat;

typedef enum {
  CMqueueReqSetattr_Pre_StoredFlags_Block,
  CMqueueReqSetattr_Pre_StoredFlags_NonBlock,
  CMqueueReqSetattr_Pre_StoredFlags_NA
} CMqueueReqSetattr_Pre_StoredFlags;

typedef enum {
  CMqueueReqSetattr_Pre_StoredMaxMsg_One,
  CMqueueReqSetattr_Pre_StoredMaxMsg_Many,
  CMqueueReqSetattr_Pre_StoredMaxMsg_NA
} CMqueueReqSetattr_Pre_StoredMaxMsg;

typedef enum {
  CMqueueReqSetattr_Pre_StoredMsgSize_One,
  CMqueueReqSetattr_Pre_StoredMsgSize_Many,
  CMqueueReqSetattr_Pre_StoredMsgSize_NA
} CMqueueReqSetattr_Pre_StoredMsgSize;

typedef enum {
  CMqueueReqSetattr_Pre_MqDes_Valid,
  CMqueueReqSetattr_Pre_MqDes_Invalid,
  CMqueueReqSetattr_Pre_MqDes_NA
} CMqueueReqSetattr_Pre_MqDes;

typedef enum {
  CMqueueReqSetattr_Pre_StoredCurMsgs_Zero,
  CMqueueReqSetattr_Pre_StoredCurMsgs_One,
  CMqueueReqSetattr_Pre_StoredCurMsgs_NA
} CMqueueReqSetattr_Pre_StoredCurMsgs;

typedef enum {
  CMqueueReqSetattr_Pre_Open_Yes,
  CMqueueReqSetattr_Pre_Open_No,
  CMqueueReqSetattr_Pre_Open_NA
} CMqueueReqSetattr_Pre_Open;

typedef enum {
  CMqueueReqSetattr_Post_Status_Zero,
  CMqueueReqSetattr_Post_Status_MinusOne,
  CMqueueReqSetattr_Post_Status_NA
} CMqueueReqSetattr_Post_Status;

typedef enum {
  CMqueueReqSetattr_Post_Errno_Nop,
  CMqueueReqSetattr_Post_Errno_Badf,
  CMqueueReqSetattr_Post_Errno_Inval,
  CMqueueReqSetattr_Post_Errno_NA
} CMqueueReqSetattr_Post_Errno;

typedef enum {
  CMqueueReqSetattr_Post_PrevFlags_Block,
  CMqueueReqSetattr_Post_PrevFlags_NonBlock,
  CMqueueReqSetattr_Post_PrevFlags_Nop,
  CMqueueReqSetattr_Post_PrevFlags_NA
} CMqueueReqSetattr_Post_PrevFlags;

typedef enum {
  CMqueueReqSetattr_Post_PrevMaxMsg_One,
  CMqueueReqSetattr_Post_PrevMaxMsg_Many,
  CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
  CMqueueReqSetattr_Post_PrevMaxMsg_NA
} CMqueueReqSetattr_Post_PrevMaxMsg;

typedef enum {
  CMqueueReqSetattr_Post_PrevMsgSize_One,
  CMqueueReqSetattr_Post_PrevMsgSize_Many,
  CMqueueReqSetattr_Post_PrevMsgSize_Nop,
  CMqueueReqSetattr_Post_PrevMsgSize_NA
} CMqueueReqSetattr_Post_PrevMsgSize;

typedef enum {
  CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
  CMqueueReqSetattr_Post_PrevCurMsgs_One,
  CMqueueReqSetattr_Post_PrevCurMsgs_Nop,
  CMqueueReqSetattr_Post_PrevCurMsgs_NA
} CMqueueReqSetattr_Post_PrevCurMsgs;

typedef enum {
  CMqueueReqSetattr_Post_Flags_Zero,
  CMqueueReqSetattr_Post_Flags_NonBlock,
  CMqueueReqSetattr_Post_Flags_Nop,
  CMqueueReqSetattr_Post_Flags_NA
} CMqueueReqSetattr_Post_Flags;

typedef enum {
  CMqueueReqSetattr_Post_Other_Nop,
  CMqueueReqSetattr_Post_Other_NA
} CMqueueReqSetattr_Post_Other;

typedef enum {
  CMqueueReqSetattr_Post_MqStatObj_Nop,
  CMqueueReqSetattr_Post_MqStatObj_NA
} CMqueueReqSetattr_Post_MqStatObj;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_MqStat_NA : 1;
  uint32_t Pre_NewFlags_NA : 1;
  uint32_t Pre_OMqStat_NA : 1;
  uint32_t Pre_StoredFlags_NA : 1;
  uint32_t Pre_StoredMaxMsg_NA : 1;
  uint32_t Pre_StoredMsgSize_NA : 1;
  uint32_t Pre_MqDes_NA : 1;
  uint32_t Pre_StoredCurMsgs_NA : 1;
  uint32_t Pre_Open_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 2;
  uint32_t Post_PrevFlags : 2;
  uint32_t Post_PrevMaxMsg : 2;
  uint32_t Post_PrevMsgSize : 2;
  uint32_t Post_PrevCurMsgs : 2;
  uint32_t Post_Flags : 2;
  uint32_t Post_Other : 1;
  uint32_t Post_MqStatObj : 1;
} CMqueueReqSetattr_Entry;

typedef struct {
  int      oflag;
  uint32_t open_count;
  bool     linked;
  uint32_t maximum_pending_messages;
  uint32_t number_of_pending_messages;
  size_t   maximum_message_size;
} QueueState;

/**
 * @brief Test context for spec:/c/mqueue/req/setattr test case.
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
   *   mq_setattr() call.
   */
  QueueState queue_before;

  /**
   * @brief This member provides the object referenced by the `mqstat`
   *   parameter.
   */
  struct mq_attr mqstat_obj;

  /**
   * @brief This member contains a copy of the object referenced by the
   *   `mqstat` parameter before the call.
   */
  struct mq_attr mqstat_before;

  /**
   * @brief This member provides the object referenced by the `omqstat`
   *   parameter.
   */
  struct mq_attr omqstat_obj;

  /**
   * @brief This member specifies the `mqdes` parameter value.
   */
  mqd_t mqdes;

  /**
   * @brief This member specifies the `mqstat` parameter value.
   */
  const struct mq_attr *mqstat;

  /**
   * @brief This member specifies the `omqstat` parameter value.
   */
  struct mq_attr *omqstat;

  /**
   * @brief This member contains the return value of the mq_setattr() call.
   */
  int status;

  /**
   * @brief This member contains the value of errno after the mq_setattr()
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
    CMqueueReqSetattr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqSetattr_Context;

static CMqueueReqSetattr_Context CMqueueReqSetattr_Instance;

static const char *const CMqueueReqSetattr_PreDesc_MqStat[] =
  { "Obj", "Null", "NA" };

static const char *const CMqueueReqSetattr_PreDesc_NewFlags[] =
  { "Zero", "NonBlock", "NA" };

static const char *const CMqueueReqSetattr_PreDesc_OMqStat[] =
  { "Obj", "Null", "NA" };

static const char *const CMqueueReqSetattr_PreDesc_StoredFlags[] =
  { "Block", "NonBlock", "NA" };

static const char *const CMqueueReqSetattr_PreDesc_StoredMaxMsg[] =
  { "One", "Many", "NA" };

static const char *const CMqueueReqSetattr_PreDesc_StoredMsgSize[] =
  { "One", "Many", "NA" };

static const char *const CMqueueReqSetattr_PreDesc_MqDes[] =
  { "Valid", "Invalid", "NA" };

static const char *const CMqueueReqSetattr_PreDesc_StoredCurMsgs[] =
  { "Zero", "One", "NA" };

static const char *const CMqueueReqSetattr_PreDesc_Open[] =
  { "Yes", "No", "NA" };

static const char *const *const CMqueueReqSetattr_PreDesc[] = {
  CMqueueReqSetattr_PreDesc_MqStat,
  CMqueueReqSetattr_PreDesc_NewFlags,
  CMqueueReqSetattr_PreDesc_OMqStat,
  CMqueueReqSetattr_PreDesc_StoredFlags,
  CMqueueReqSetattr_PreDesc_StoredMaxMsg,
  CMqueueReqSetattr_PreDesc_StoredMsgSize,
  CMqueueReqSetattr_PreDesc_MqDes,
  CMqueueReqSetattr_PreDesc_StoredCurMsgs,
  CMqueueReqSetattr_PreDesc_Open,
  NULL
};

#define NAME "/mq-setattr"

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

static void CMqueueReqSetattr_Pre_MqStat_Prepare(
  CMqueueReqSetattr_Context   *ctx,
  CMqueueReqSetattr_Pre_MqStat state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_MqStat_Obj: {
      /*
       * While the `mqstat` parameter references an object of type struct
       * mq_attr.
       */
      ctx->mqstat = &ctx->mqstat_obj;
      break;
    }

    case CMqueueReqSetattr_Pre_MqStat_Null: {
      /*
       * While the `mqstat` parameter is NULL.
       */
      ctx->mqstat = NULL;
      break;
    }

    case CMqueueReqSetattr_Pre_MqStat_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_NewFlags_Prepare(
  CMqueueReqSetattr_Context     *ctx,
  CMqueueReqSetattr_Pre_NewFlags state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_NewFlags_Zero: {
      /*
       * While the mq_flags member of the object referenced by the `mqstat`
       * parameter is zero.
       */
      ctx->mqstat_obj.mq_flags = 0;
      break;
    }

    case CMqueueReqSetattr_Pre_NewFlags_NonBlock: {
      /*
       * While the mq_flags member of the object referenced by the `mqstat`
       * parameter is O_NONBLOCK.
       */
      ctx->mqstat_obj.mq_flags = O_NONBLOCK;
      break;
    }

    case CMqueueReqSetattr_Pre_NewFlags_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_OMqStat_Prepare(
  CMqueueReqSetattr_Context    *ctx,
  CMqueueReqSetattr_Pre_OMqStat state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_OMqStat_Obj: {
      /*
       * While the `omqstat` parameter references an object of type struct
       * mq_attr.
       */
      ctx->omqstat = &ctx->omqstat_obj;
      break;
    }

    case CMqueueReqSetattr_Pre_OMqStat_Null: {
      /*
       * While the `omqstat` parameter is NULL.
       */
      ctx->omqstat = NULL;
      break;
    }

    case CMqueueReqSetattr_Pre_OMqStat_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_StoredFlags_Prepare(
  CMqueueReqSetattr_Context        *ctx,
  CMqueueReqSetattr_Pre_StoredFlags state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_StoredFlags_Block: {
      /*
       * While the flags of the message queue do not contain O_NONBLOCK.
       */
      ctx->oflag = FLAGS_BLOCK;
      break;
    }

    case CMqueueReqSetattr_Pre_StoredFlags_NonBlock: {
      /*
       * While the flags of the message queue contain O_NONBLOCK.
       */
      ctx->oflag = FLAGS_NON_BLOCK;
      break;
    }

    case CMqueueReqSetattr_Pre_StoredFlags_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_StoredMaxMsg_Prepare(
  CMqueueReqSetattr_Context         *ctx,
  CMqueueReqSetattr_Pre_StoredMaxMsg state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_StoredMaxMsg_One: {
      /*
       * While the maximum number of messages of the message queue is one.
       */
      ctx->attr.mq_maxmsg = 1;
      break;
    }

    case CMqueueReqSetattr_Pre_StoredMaxMsg_Many: {
      /*
       * While the maximum number of messages of the message queue is greater
       * than one.
       */
      ctx->attr.mq_maxmsg = MAX_MSG;
      break;
    }

    case CMqueueReqSetattr_Pre_StoredMaxMsg_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_StoredMsgSize_Prepare(
  CMqueueReqSetattr_Context          *ctx,
  CMqueueReqSetattr_Pre_StoredMsgSize state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_StoredMsgSize_One: {
      /*
       * While the maximum message size of the message queue is one.
       */
      ctx->attr.mq_msgsize = 1;
      break;
    }

    case CMqueueReqSetattr_Pre_StoredMsgSize_Many: {
      /*
       * While the maximum message size of the message queue is greater than
       * one.
       */
      ctx->attr.mq_msgsize = MSG_SIZE;
      break;
    }

    case CMqueueReqSetattr_Pre_StoredMsgSize_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_MqDes_Prepare(
  CMqueueReqSetattr_Context  *ctx,
  CMqueueReqSetattr_Pre_MqDes state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_MqDes_Valid: {
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

    case CMqueueReqSetattr_Pre_MqDes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = (mqd_t) INVALID_ID;
      break;
    }

    case CMqueueReqSetattr_Pre_MqDes_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_StoredCurMsgs_Prepare(
  CMqueueReqSetattr_Context          *ctx,
  CMqueueReqSetattr_Pre_StoredCurMsgs state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_StoredCurMsgs_Zero: {
      /*
       * While the message queue contains no message.
       */
      /* The message queue was created without messages */
      break;
    }

    case CMqueueReqSetattr_Pre_StoredCurMsgs_One: {
      /*
       * While the message queue contains exactly one message.
       */
      T_eq_int( mq_send( ctx->queue_id, "M", 1, 0 ), 0 );
      break;
    }

    case CMqueueReqSetattr_Pre_StoredCurMsgs_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_Open_Prepare(
  CMqueueReqSetattr_Context *ctx,
  CMqueueReqSetattr_Pre_Open state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_Open_Yes: {
      /*
       * While the message queue is open.
       */
      /* The message queue was opened by its creation */
      break;
    }

    case CMqueueReqSetattr_Pre_Open_No: {
      /*
       * While the message queue is not open.
       */
      T_eq_int( mq_close( ctx->queue_id ), 0 );
      break;
    }

    case CMqueueReqSetattr_Pre_Open_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_Status_Check(
  CMqueueReqSetattr_Context    *ctx,
  CMqueueReqSetattr_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_Status_Zero: {
      /*
       * The return value of mq_setattr() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CMqueueReqSetattr_Post_Status_MinusOne: {
      /*
       * The return value of mq_setattr() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CMqueueReqSetattr_Post_Status_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_Errno_Check(
  CMqueueReqSetattr_Context   *ctx,
  CMqueueReqSetattr_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified by the mq_setattr() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CMqueueReqSetattr_Post_Errno_Badf: {
      /*
       * The value of errno shall be EBADF.
       */
      T_eq_int( ctx->errno_value, EBADF );
      break;
    }

    case CMqueueReqSetattr_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CMqueueReqSetattr_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_PrevFlags_Check(
  CMqueueReqSetattr_Context       *ctx,
  CMqueueReqSetattr_Post_PrevFlags state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_PrevFlags_Block: {
      /*
       * The mq_flags member of the object referenced by the `omqstat`
       * parameter shall be the flags of the message queue before the call,
       * which do not contain O_NONBLOCK.
       */
      T_eq_long( ctx->omqstat_obj.mq_flags, FLAGS_BLOCK );
      break;
    }

    case CMqueueReqSetattr_Post_PrevFlags_NonBlock: {
      /*
       * The mq_flags member of the object referenced by the `omqstat`
       * parameter shall be the flags of the message queue before the call,
       * which contain O_NONBLOCK.
       */
      T_eq_long( ctx->omqstat_obj.mq_flags, FLAGS_NON_BLOCK );
      break;
    }

    case CMqueueReqSetattr_Post_PrevFlags_Nop: {
      /*
       * The mq_flags member of the object referenced by the `omqstat`
       * parameter shall not be modified.
       */
      T_eq_long( ctx->omqstat_obj.mq_flags, -1 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevFlags_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_PrevMaxMsg_Check(
  CMqueueReqSetattr_Context        *ctx,
  CMqueueReqSetattr_Post_PrevMaxMsg state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_PrevMaxMsg_One: {
      /*
       * The mq_maxmsg member of the object referenced by the `omqstat`
       * parameter shall be one.
       */
      T_eq_long( ctx->omqstat_obj.mq_maxmsg, 1 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevMaxMsg_Many: {
      /*
       * The mq_maxmsg member of the object referenced by the `omqstat`
       * parameter shall be the maximum number of messages of the message
       * queue.
       */
      T_eq_long( ctx->omqstat_obj.mq_maxmsg, MAX_MSG );
      break;
    }

    case CMqueueReqSetattr_Post_PrevMaxMsg_Nop: {
      /*
       * The mq_maxmsg member of the object referenced by the `omqstat`
       * parameter shall not be modified.
       */
      T_eq_long( ctx->omqstat_obj.mq_maxmsg, -1 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevMaxMsg_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_PrevMsgSize_Check(
  CMqueueReqSetattr_Context         *ctx,
  CMqueueReqSetattr_Post_PrevMsgSize state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_PrevMsgSize_One: {
      /*
       * The mq_msgsize member of the object referenced by the `omqstat`
       * parameter shall be one.
       */
      T_eq_long( ctx->omqstat_obj.mq_msgsize, 1 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevMsgSize_Many: {
      /*
       * The mq_msgsize member of the object referenced by the `omqstat`
       * parameter shall be the maximum message size of the message queue.
       */
      T_eq_long( ctx->omqstat_obj.mq_msgsize, MSG_SIZE );
      break;
    }

    case CMqueueReqSetattr_Post_PrevMsgSize_Nop: {
      /*
       * The mq_msgsize member of the object referenced by the `omqstat`
       * parameter shall not be modified.
       */
      T_eq_long( ctx->omqstat_obj.mq_msgsize, -1 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevMsgSize_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_PrevCurMsgs_Check(
  CMqueueReqSetattr_Context         *ctx,
  CMqueueReqSetattr_Post_PrevCurMsgs state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_PrevCurMsgs_Zero: {
      /*
       * The mq_curmsgs member of the object referenced by the `omqstat`
       * parameter shall be zero.
       */
      T_eq_long( ctx->omqstat_obj.mq_curmsgs, 0 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevCurMsgs_One: {
      /*
       * The mq_curmsgs member of the object referenced by the `omqstat`
       * parameter shall be one.
       */
      T_eq_long( ctx->omqstat_obj.mq_curmsgs, 1 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevCurMsgs_Nop: {
      /*
       * The mq_curmsgs member of the object referenced by the `omqstat`
       * parameter shall not be modified.
       */
      T_eq_long( ctx->omqstat_obj.mq_curmsgs, -1 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevCurMsgs_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_Flags_Check(
  CMqueueReqSetattr_Context   *ctx,
  CMqueueReqSetattr_Post_Flags state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_Flags_Zero: {
      /*
       * The flags of the message queue shall be zero.
       */
      T_eq_int( ctx->queue->oflag, 0 );
      break;
    }

    case CMqueueReqSetattr_Post_Flags_NonBlock: {
      /*
       * The flags of the message queue shall be O_NONBLOCK.
       */
      T_eq_int( ctx->queue->oflag, O_NONBLOCK );
      break;
    }

    case CMqueueReqSetattr_Post_Flags_Nop: {
      /*
       * The flags of the message queue shall not be modified by the
       * mq_setattr() call.
       */
      T_eq_int( ctx->queue->oflag, ctx->queue_before.oflag );
      break;
    }

    case CMqueueReqSetattr_Post_Flags_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_Other_Check(
  CMqueueReqSetattr_Context   *ctx,
  CMqueueReqSetattr_Post_Other state
)
{
  QueueState queue_state;

  switch ( state ) {
    case CMqueueReqSetattr_Post_Other_Nop: {
      /*
       * The message queue except its flags shall not be modified by the
       * mq_setattr() call.
       */
      GetQueueState( ctx->queue, &queue_state );
      queue_state.oflag = ctx->queue_before.oflag;
      T_eq_mem( &queue_state, &ctx->queue_before, sizeof( queue_state ) );
      break;
    }

    case CMqueueReqSetattr_Post_Other_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_MqStatObj_Check(
  CMqueueReqSetattr_Context       *ctx,
  CMqueueReqSetattr_Post_MqStatObj state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_MqStatObj_Nop: {
      /*
       * The object referenced by the `mqstat` parameter shall not be modified
       * by the mq_setattr() call.
       */
      T_eq_mem(
        &ctx->mqstat_obj,
        &ctx->mqstat_before,
        sizeof( ctx->mqstat_obj )
      );
      break;
    }

    case CMqueueReqSetattr_Post_MqStatObj_NA:
      break;
  }
}

static void CMqueueReqSetattr_Prepare( CMqueueReqSetattr_Context *ctx )
{
  ctx->queue = NULL;
  memset( &ctx->attr, 0, sizeof( ctx->attr ) );
  memset( &ctx->mqstat_obj, 0x55, sizeof( ctx->mqstat_obj ) );
}

static void CMqueueReqSetattr_Action( CMqueueReqSetattr_Context *ctx )
{
  memset( &ctx->omqstat_obj, 0xff, sizeof( ctx->omqstat_obj ) );
  memcpy(
    &ctx->mqstat_before,
    &ctx->mqstat_obj,
    sizeof( ctx->mqstat_before )
  );

  if ( ctx->queue != NULL ) {
    GetQueueState( ctx->queue, &ctx->queue_before );
  }

  errno = ERRNO_POISON;
  ctx->status = mq_setattr( ctx->mqdes, ctx->mqstat, ctx->omqstat );
  ctx->errno_value = errno;
}

static void CMqueueReqSetattr_Cleanup( CMqueueReqSetattr_Context *ctx )
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

static const CMqueueReqSetattr_Entry
CMqueueReqSetattr_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_NA,
    CMqueueReqSetattr_Post_Errno_NA, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_NA,
    CMqueueReqSetattr_Post_Other_NA, CMqueueReqSetattr_Post_MqStatObj_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_Badf, CMqueueReqSetattr_Post_PrevFlags_Nop,
    CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
    CMqueueReqSetattr_Post_PrevMsgSize_Nop,
    CMqueueReqSetattr_Post_PrevCurMsgs_Nop, CMqueueReqSetattr_Post_Flags_Nop,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_Badf, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_Nop,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_Inval, CMqueueReqSetattr_Post_PrevFlags_Nop,
    CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
    CMqueueReqSetattr_Post_PrevMsgSize_Nop,
    CMqueueReqSetattr_Post_PrevCurMsgs_Nop, CMqueueReqSetattr_Post_Flags_Nop,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_NA },
  { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_Inval, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_Nop,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_Badf, CMqueueReqSetattr_Post_PrevFlags_Nop,
    CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
    CMqueueReqSetattr_Post_PrevMsgSize_Nop,
    CMqueueReqSetattr_Post_PrevCurMsgs_Nop, CMqueueReqSetattr_Post_Flags_NA,
    CMqueueReqSetattr_Post_Other_NA, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_Badf, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_NA,
    CMqueueReqSetattr_Post_Other_NA, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_One, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_One, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_One, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_One, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_One, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_One, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_One, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_One, CMqueueReqSetattr_Post_Flags_Zero,
    CMqueueReqSetattr_Post_Other_Nop, CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_One,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_One,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_One,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop, CMqueueReqSetattr_Post_PrevFlags_Block,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_One,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_One,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_One,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_One,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_One,
    CMqueueReqSetattr_Post_PrevCurMsgs_One,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_Zero,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Nop,
    CMqueueReqSetattr_Post_PrevFlags_NonBlock,
    CMqueueReqSetattr_Post_PrevMaxMsg_Many,
    CMqueueReqSetattr_Post_PrevMsgSize_Many,
    CMqueueReqSetattr_Post_PrevCurMsgs_One,
    CMqueueReqSetattr_Post_Flags_NonBlock, CMqueueReqSetattr_Post_Other_Nop,
    CMqueueReqSetattr_Post_MqStatObj_Nop },
  { 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_Inval, CMqueueReqSetattr_Post_PrevFlags_Nop,
    CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
    CMqueueReqSetattr_Post_PrevMsgSize_Nop,
    CMqueueReqSetattr_Post_PrevCurMsgs_Nop, CMqueueReqSetattr_Post_Flags_NA,
    CMqueueReqSetattr_Post_Other_NA, CMqueueReqSetattr_Post_MqStatObj_NA },
  { 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_Inval, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_NA,
    CMqueueReqSetattr_Post_Other_NA, CMqueueReqSetattr_Post_MqStatObj_NA }
};

static const uint8_t
CMqueueReqSetattr_Map[] = {
  9, 1, 10, 1, 7, 0, 0, 0, 11, 1, 12, 1, 0, 0, 0, 0, 13, 1, 14, 1, 0, 0, 0, 0,
  15, 1, 16, 1, 0, 0, 0, 0, 17, 1, 18, 1, 0, 0, 0, 0, 19, 1, 20, 1, 0, 0, 0, 0,
  21, 1, 22, 1, 0, 0, 0, 0, 23, 1, 24, 1, 0, 0, 0, 0, 5, 2, 5, 2, 8, 0, 0, 0,
  5, 2, 5, 2, 0, 0, 0, 0, 5, 2, 5, 2, 0, 0, 0, 0, 5, 2, 5, 2, 0, 0, 0, 0, 5, 2,
  5, 2, 0, 0, 0, 0, 5, 2, 5, 2, 0, 0, 0, 0, 5, 2, 5, 2, 0, 0, 0, 0, 5, 2, 5, 2,
  0, 0, 0, 0, 25, 1, 26, 1, 7, 0, 0, 0, 27, 1, 28, 1, 0, 0, 0, 0, 29, 1, 30, 1,
  0, 0, 0, 0, 31, 1, 32, 1, 0, 0, 0, 0, 33, 1, 34, 1, 0, 0, 0, 0, 35, 1, 36, 1,
  0, 0, 0, 0, 37, 1, 38, 1, 0, 0, 0, 0, 39, 1, 40, 1, 0, 0, 0, 0, 6, 2, 6, 2,
  8, 0, 0, 0, 6, 2, 6, 2, 0, 0, 0, 0, 6, 2, 6, 2, 0, 0, 0, 0, 6, 2, 6, 2, 0, 0,
  0, 0, 6, 2, 6, 2, 0, 0, 0, 0, 6, 2, 6, 2, 0, 0, 0, 0, 6, 2, 6, 2, 0, 0, 0, 0,
  6, 2, 6, 2, 0, 0, 0, 0, 3, 3, 3, 3, 41, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 3,
  3, 3, 3, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 3, 3, 3,
  3, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 4, 4, 4, 4,
  42, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0,
  0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0,
  0, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CMqueueReqSetattr_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqSetattr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqSetattr_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqSetattr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CMqueueReqSetattr_Scope,
  .initial_context = &CMqueueReqSetattr_Instance
};

static inline CMqueueReqSetattr_Entry CMqueueReqSetattr_PopEntry(
  CMqueueReqSetattr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqSetattr_Entries[ CMqueueReqSetattr_Map[ index ] ];
}

static void CMqueueReqSetattr_SetPreConditionStates(
  CMqueueReqSetattr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_NewFlags_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqSetattr_Pre_NewFlags_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_StoredFlags_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqSetattr_Pre_StoredFlags_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_StoredMaxMsg_NA ) {
    ctx->Map.pcs[ 4 ] = CMqueueReqSetattr_Pre_StoredMaxMsg_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_StoredMsgSize_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqSetattr_Pre_StoredMsgSize_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];

  if ( ctx->Map.entry.Pre_StoredCurMsgs_NA ) {
    ctx->Map.pcs[ 7 ] = CMqueueReqSetattr_Pre_StoredCurMsgs_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 8 ] = CMqueueReqSetattr_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }
}

static void CMqueueReqSetattr_TestVariant( CMqueueReqSetattr_Context *ctx )
{
  CMqueueReqSetattr_Pre_MqStat_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqSetattr_Pre_NewFlags_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqSetattr_Pre_OMqStat_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqSetattr_Pre_StoredFlags_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqSetattr_Pre_StoredMaxMsg_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqSetattr_Pre_StoredMsgSize_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqSetattr_Pre_MqDes_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CMqueueReqSetattr_Pre_StoredCurMsgs_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CMqueueReqSetattr_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  CMqueueReqSetattr_Action( ctx );
  CMqueueReqSetattr_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqSetattr_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqSetattr_Post_PrevFlags_Check( ctx, ctx->Map.entry.Post_PrevFlags );
  CMqueueReqSetattr_Post_PrevMaxMsg_Check(
    ctx,
    ctx->Map.entry.Post_PrevMaxMsg
  );
  CMqueueReqSetattr_Post_PrevMsgSize_Check(
    ctx,
    ctx->Map.entry.Post_PrevMsgSize
  );
  CMqueueReqSetattr_Post_PrevCurMsgs_Check(
    ctx,
    ctx->Map.entry.Post_PrevCurMsgs
  );
  CMqueueReqSetattr_Post_Flags_Check( ctx, ctx->Map.entry.Post_Flags );
  CMqueueReqSetattr_Post_Other_Check( ctx, ctx->Map.entry.Post_Other );
  CMqueueReqSetattr_Post_MqStatObj_Check( ctx, ctx->Map.entry.Post_MqStatObj );
}

/**
 * @fn void T_case_body_CMqueueReqSetattr( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqSetattr, &CMqueueReqSetattr_Fixture )
{
  CMqueueReqSetattr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqSetattr_Pre_MqStat_Obj;
    ctx->Map.pci[ 0 ] < CMqueueReqSetattr_Pre_MqStat_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqSetattr_Pre_NewFlags_Zero;
      ctx->Map.pci[ 1 ] < CMqueueReqSetattr_Pre_NewFlags_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqSetattr_Pre_OMqStat_Obj;
        ctx->Map.pci[ 2 ] < CMqueueReqSetattr_Pre_OMqStat_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqSetattr_Pre_StoredFlags_Block;
          ctx->Map.pci[ 3 ] < CMqueueReqSetattr_Pre_StoredFlags_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqSetattr_Pre_StoredMaxMsg_One;
            ctx->Map.pci[ 4 ] < CMqueueReqSetattr_Pre_StoredMaxMsg_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqSetattr_Pre_StoredMsgSize_One;
              ctx->Map.pci[ 5 ] < CMqueueReqSetattr_Pre_StoredMsgSize_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CMqueueReqSetattr_Pre_MqDes_Valid;
                ctx->Map.pci[ 6 ] < CMqueueReqSetattr_Pre_MqDes_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CMqueueReqSetattr_Pre_StoredCurMsgs_Zero;
                  ctx->Map.pci[ 7 ] < CMqueueReqSetattr_Pre_StoredCurMsgs_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = CMqueueReqSetattr_Pre_Open_Yes;
                    ctx->Map.pci[ 8 ] < CMqueueReqSetattr_Pre_Open_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    ctx->Map.entry = CMqueueReqSetattr_PopEntry( ctx );

                    if ( ctx->Map.entry.Skip ) {
                      continue;
                    }

                    CMqueueReqSetattr_SetPreConditionStates( ctx );
                    CMqueueReqSetattr_Prepare( ctx );
                    CMqueueReqSetattr_TestVariant( ctx );
                    CMqueueReqSetattr_Cleanup( ctx );
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
