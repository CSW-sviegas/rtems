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
#include <mqueue.h>
#include <sys/stat.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqGetattr spec:/c/mqueue/req/getattr
 *
 * @{
 */

typedef enum {
  CMqueueReqGetattr_Pre_NonBlock_Yes,
  CMqueueReqGetattr_Pre_NonBlock_No,
  CMqueueReqGetattr_Pre_NonBlock_NA
} CMqueueReqGetattr_Pre_NonBlock;

typedef enum {
  CMqueueReqGetattr_Pre_Messages_None,
  CMqueueReqGetattr_Pre_Messages_One,
  CMqueueReqGetattr_Pre_Messages_NA
} CMqueueReqGetattr_Pre_Messages;

typedef enum {
  CMqueueReqGetattr_Pre_Mqdes_Valid,
  CMqueueReqGetattr_Pre_Mqdes_Closed,
  CMqueueReqGetattr_Pre_Mqdes_Invalid,
  CMqueueReqGetattr_Pre_Mqdes_NA
} CMqueueReqGetattr_Pre_Mqdes;

typedef enum {
  CMqueueReqGetattr_Pre_MqStat_Valid,
  CMqueueReqGetattr_Pre_MqStat_Null,
  CMqueueReqGetattr_Pre_MqStat_NA
} CMqueueReqGetattr_Pre_MqStat;

typedef enum {
  CMqueueReqGetattr_Post_Status_Zero,
  CMqueueReqGetattr_Post_Status_MinusOne,
  CMqueueReqGetattr_Post_Status_NA
} CMqueueReqGetattr_Post_Status;

typedef enum {
  CMqueueReqGetattr_Post_Errno_Unchanged,
  CMqueueReqGetattr_Post_Errno_EBADF,
  CMqueueReqGetattr_Post_Errno_EINVAL,
  CMqueueReqGetattr_Post_Errno_NA
} CMqueueReqGetattr_Post_Errno;

typedef enum {
  CMqueueReqGetattr_Post_Flags_Set,
  CMqueueReqGetattr_Post_Flags_Nop,
  CMqueueReqGetattr_Post_Flags_NA
} CMqueueReqGetattr_Post_Flags;

typedef enum {
  CMqueueReqGetattr_Post_MaxMsg_Set,
  CMqueueReqGetattr_Post_MaxMsg_Nop,
  CMqueueReqGetattr_Post_MaxMsg_NA
} CMqueueReqGetattr_Post_MaxMsg;

typedef enum {
  CMqueueReqGetattr_Post_MsgSize_Set,
  CMqueueReqGetattr_Post_MsgSize_Nop,
  CMqueueReqGetattr_Post_MsgSize_NA
} CMqueueReqGetattr_Post_MsgSize;

typedef enum {
  CMqueueReqGetattr_Post_CurMsgs_Set,
  CMqueueReqGetattr_Post_CurMsgs_Nop,
  CMqueueReqGetattr_Post_CurMsgs_NA
} CMqueueReqGetattr_Post_CurMsgs;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_NonBlock_NA : 1;
  uint32_t Pre_Messages_NA : 1;
  uint32_t Pre_Mqdes_NA : 1;
  uint32_t Pre_MqStat_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 2;
  uint32_t Post_Flags : 2;
  uint32_t Post_MaxMsg : 2;
  uint32_t Post_MsgSize : 2;
  uint32_t Post_CurMsgs : 2;
} CMqueueReqGetattr_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/getattr test case.
 */
typedef struct {
  /**
   * @brief This member contains the descriptor of the message queue created
   *   for the test variant.
   */
  mqd_t mq;

  /**
   * @brief This member contains the flags used to open the message queue.
   */
  int oflag;

  /**
   * @brief This member contains the expected number of messages in the message
   *   queue.
   */
  long curmsgs;

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
  int retval;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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

static CMqueueReqGetattr_Context
  CMqueueReqGetattr_Instance;

static const char * const CMqueueReqGetattr_PreDesc_NonBlock[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const CMqueueReqGetattr_PreDesc_Messages[] = {
  "None",
  "One",
  "NA"
};

static const char * const CMqueueReqGetattr_PreDesc_Mqdes[] = {
  "Valid",
  "Closed",
  "Invalid",
  "NA"
};

static const char * const CMqueueReqGetattr_PreDesc_MqStat[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CMqueueReqGetattr_PreDesc[] = {
  CMqueueReqGetattr_PreDesc_NonBlock,
  CMqueueReqGetattr_PreDesc_Messages,
  CMqueueReqGetattr_PreDesc_Mqdes,
  CMqueueReqGetattr_PreDesc_MqStat,
  NULL
};

#define QUEUE_NAME "/mqgetattr"

#define MAX_MSG 2

#define MSG_SIZE 4

static const struct mq_attr queue_attr = { 0, MAX_MSG, MSG_SIZE, 0 };

static void CMqueueReqGetattr_Pre_NonBlock_Prepare(
  CMqueueReqGetattr_Context     *ctx,
  CMqueueReqGetattr_Pre_NonBlock state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_NonBlock_Yes: {
      /*
       * While the message queue was opened with the O_NONBLOCK flag.
       */
      ctx->oflag = O_CREAT | O_RDWR | O_NONBLOCK;
      ctx->mq = mq_open( QUEUE_NAME, ctx->oflag, S_IRWXU, &queue_attr );
      T_ne_u32( ctx->mq, (mqd_t) -1 );
      break;
    }

    case CMqueueReqGetattr_Pre_NonBlock_No: {
      /*
       * While the message queue was opened without the O_NONBLOCK flag.
       */
      ctx->oflag = O_CREAT | O_RDWR;
      ctx->mq = mq_open( QUEUE_NAME, ctx->oflag, S_IRWXU, &queue_attr );
      T_ne_u32( ctx->mq, (mqd_t) -1 );
      break;
    }

    case CMqueueReqGetattr_Pre_NonBlock_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_Messages_Prepare(
  CMqueueReqGetattr_Context     *ctx,
  CMqueueReqGetattr_Pre_Messages state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_Messages_None: {
      /*
       * While the message queue contains no message.
       */
      ctx->curmsgs = 0;
      break;
    }

    case CMqueueReqGetattr_Pre_Messages_One: {
      /*
       * While the message queue contains exactly one message.
       */
      T_eq_int( mq_send( ctx->mq, "a", 1, 0 ), 0 );
      ctx->curmsgs = 1;
      break;
    }

    case CMqueueReqGetattr_Pre_Messages_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_Mqdes_Prepare(
  CMqueueReqGetattr_Context  *ctx,
  CMqueueReqGetattr_Pre_Mqdes state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_Mqdes_Valid: {
      /*
       * While the `mqdes` parameter specifies a message queue descriptor of an
       * open message queue.
       */
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqGetattr_Pre_Mqdes_Closed: {
      /*
       * While the `mqdes` parameter specifies a message queue descriptor of a
       * message queue which was closed and not unlinked.
       */
      T_eq_int( mq_close( ctx->mq ), 0 );
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqGetattr_Pre_Mqdes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = INVALID_ID;
      break;
    }

    case CMqueueReqGetattr_Pre_Mqdes_NA:
      break;
  }
}

static void CMqueueReqGetattr_Pre_MqStat_Prepare(
  CMqueueReqGetattr_Context   *ctx,
  CMqueueReqGetattr_Pre_MqStat state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Pre_MqStat_Valid: {
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

static void CMqueueReqGetattr_Post_Status_Check(
  CMqueueReqGetattr_Context    *ctx,
  CMqueueReqGetattr_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Post_Status_Zero: {
      /*
       * The return value of mq_getattr() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CMqueueReqGetattr_Post_Status_MinusOne: {
      /*
       * The return value of mq_getattr() shall be equal to minus one.
       */
      T_eq_int( ctx->retval, -1 );
      break;
    }

    case CMqueueReqGetattr_Post_Status_NA:
      break;
  }
}

static void CMqueueReqGetattr_Post_Errno_Check(
  CMqueueReqGetattr_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqGetattr_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_getattr() call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CMqueueReqGetattr_Post_Errno_EBADF: {
      /*
       * The errno shall be set to EBADF.
       */
      T_eq_int( errno, EBADF );
      break;
    }

    case CMqueueReqGetattr_Post_Errno_EINVAL: {
      /*
       * The errno shall be set to EINVAL.
       */
      T_eq_int( errno, EINVAL );
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
    case CMqueueReqGetattr_Post_Flags_Set: {
      /*
       * The mq_flags member of the object referenced by the `mqstat` parameter
       * shall be set to the flags of the message queue descriptor specified by
       * the `mqdes` parameter.
       */
      T_eq_long( ctx->mqstat_obj.mq_flags, ctx->oflag );
      break;
    }

    case CMqueueReqGetattr_Post_Flags_Nop: {
      /*
       * The mq_flags member of the object referenced by the `mqstat` parameter
       * shall not be changed by the mq_getattr() call.
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
    case CMqueueReqGetattr_Post_MaxMsg_Set: {
      /*
       * The mq_maxmsg member of the object referenced by the `mqstat`
       * parameter shall be set to the maximum number of messages of the
       * message queue specified by the `mqdes` parameter.
       */
      T_eq_long( ctx->mqstat_obj.mq_maxmsg, MAX_MSG );
      break;
    }

    case CMqueueReqGetattr_Post_MaxMsg_Nop: {
      /*
       * The mq_maxmsg member of the object referenced by the `mqstat`
       * parameter shall not be changed by the mq_getattr() call.
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
    case CMqueueReqGetattr_Post_MsgSize_Set: {
      /*
       * The mq_msgsize member of the object referenced by the `mqstat`
       * parameter shall be set to the maximum message size of the message
       * queue specified by the `mqdes` parameter.
       */
      T_eq_long( ctx->mqstat_obj.mq_msgsize, MSG_SIZE );
      break;
    }

    case CMqueueReqGetattr_Post_MsgSize_Nop: {
      /*
       * The mq_msgsize member of the object referenced by the `mqstat`
       * parameter shall not be changed by the mq_getattr() call.
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
    case CMqueueReqGetattr_Post_CurMsgs_Set: {
      /*
       * The mq_curmsgs member of the object referenced by the `mqstat`
       * parameter shall be set to the number of messages currently in the
       * message queue specified by the `mqdes` parameter.
       */
      T_eq_long( ctx->mqstat_obj.mq_curmsgs, ctx->curmsgs );
      break;
    }

    case CMqueueReqGetattr_Post_CurMsgs_Nop: {
      /*
       * The mq_curmsgs member of the object referenced by the `mqstat`
       * parameter shall not be changed by the mq_getattr() call.
       */
      T_eq_long( ctx->mqstat_obj.mq_curmsgs, -1 );
      break;
    }

    case CMqueueReqGetattr_Post_CurMsgs_NA:
      break;
  }
}

static void CMqueueReqGetattr_Prepare( CMqueueReqGetattr_Context *ctx )
{
  errno = -1;
  ctx->retval = -1;
  ctx->mqstat_obj.mq_flags = -1;
  ctx->mqstat_obj.mq_maxmsg = -1;
  ctx->mqstat_obj.mq_msgsize = -1;
  ctx->mqstat_obj.mq_curmsgs = -1;
}

static void CMqueueReqGetattr_Action( CMqueueReqGetattr_Context *ctx )
{
  ctx->retval = mq_getattr( ctx->mqdes, ctx->mqstat );
}

static void CMqueueReqGetattr_Cleanup( CMqueueReqGetattr_Context *ctx )
{
  (void) mq_close( ctx->mq );
  (void) mq_unlink( QUEUE_NAME );
}

static const CMqueueReqGetattr_Entry
CMqueueReqGetattr_Entries[] = {
  { 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_MinusOne,
    CMqueueReqGetattr_Post_Errno_EINVAL, CMqueueReqGetattr_Post_Flags_NA,
    CMqueueReqGetattr_Post_MaxMsg_NA, CMqueueReqGetattr_Post_MsgSize_NA,
    CMqueueReqGetattr_Post_CurMsgs_NA },
  { 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_MinusOne,
    CMqueueReqGetattr_Post_Errno_EBADF, CMqueueReqGetattr_Post_Flags_Nop,
    CMqueueReqGetattr_Post_MaxMsg_Nop, CMqueueReqGetattr_Post_MsgSize_Nop,
    CMqueueReqGetattr_Post_CurMsgs_Nop },
  { 0, 0, 0, 0, 0, CMqueueReqGetattr_Post_Status_Zero,
    CMqueueReqGetattr_Post_Errno_Unchanged, CMqueueReqGetattr_Post_Flags_Set,
    CMqueueReqGetattr_Post_MaxMsg_Set, CMqueueReqGetattr_Post_MsgSize_Set,
    CMqueueReqGetattr_Post_CurMsgs_Set }
};

static const uint8_t
CMqueueReqGetattr_Map[] = {
  2, 0, 1, 0, 1, 0, 2, 0, 1, 0, 1, 0, 2, 0, 1, 0, 1, 0, 2, 0, 1, 0, 1, 0
};

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
  return CMqueueReqGetattr_Entries[
    CMqueueReqGetattr_Map[ index ]
  ];
}

static void CMqueueReqGetattr_TestVariant( CMqueueReqGetattr_Context *ctx )
{
  CMqueueReqGetattr_Pre_NonBlock_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqGetattr_Pre_Messages_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqGetattr_Pre_Mqdes_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqGetattr_Pre_MqStat_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqGetattr_Action( ctx );
  CMqueueReqGetattr_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqGetattr_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CMqueueReqGetattr_Post_Flags_Check( ctx, ctx->Map.entry.Post_Flags );
  CMqueueReqGetattr_Post_MaxMsg_Check( ctx, ctx->Map.entry.Post_MaxMsg );
  CMqueueReqGetattr_Post_MsgSize_Check( ctx, ctx->Map.entry.Post_MsgSize );
  CMqueueReqGetattr_Post_CurMsgs_Check( ctx, ctx->Map.entry.Post_CurMsgs );
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
    ctx->Map.pcs[ 0 ] = CMqueueReqGetattr_Pre_NonBlock_Yes;
    ctx->Map.pcs[ 0 ] < CMqueueReqGetattr_Pre_NonBlock_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CMqueueReqGetattr_Pre_Messages_None;
      ctx->Map.pcs[ 1 ] < CMqueueReqGetattr_Pre_Messages_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CMqueueReqGetattr_Pre_Mqdes_Valid;
        ctx->Map.pcs[ 2 ] < CMqueueReqGetattr_Pre_Mqdes_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CMqueueReqGetattr_Pre_MqStat_Valid;
          ctx->Map.pcs[ 3 ] < CMqueueReqGetattr_Pre_MqStat_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = CMqueueReqGetattr_PopEntry( ctx );
          CMqueueReqGetattr_Prepare( ctx );
          CMqueueReqGetattr_TestVariant( ctx );
          CMqueueReqGetattr_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
