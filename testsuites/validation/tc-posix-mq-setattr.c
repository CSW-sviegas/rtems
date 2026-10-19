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
 * @defgroup CMqueueReqSetattr spec:/c/mqueue/req/setattr
 *
 * @{
 */

typedef enum {
  CMqueueReqSetattr_Pre_Mqdes_Valid,
  CMqueueReqSetattr_Pre_Mqdes_Closed,
  CMqueueReqSetattr_Pre_Mqdes_Invalid,
  CMqueueReqSetattr_Pre_Mqdes_NA
} CMqueueReqSetattr_Pre_Mqdes;

typedef enum {
  CMqueueReqSetattr_Pre_MqStat_Valid,
  CMqueueReqSetattr_Pre_MqStat_Null,
  CMqueueReqSetattr_Pre_MqStat_NA
} CMqueueReqSetattr_Pre_MqStat;

typedef enum {
  CMqueueReqSetattr_Pre_OMqStat_Valid,
  CMqueueReqSetattr_Pre_OMqStat_Null,
  CMqueueReqSetattr_Pre_OMqStat_NA
} CMqueueReqSetattr_Pre_OMqStat;

typedef enum {
  CMqueueReqSetattr_Post_Status_Zero,
  CMqueueReqSetattr_Post_Status_MinusOne,
  CMqueueReqSetattr_Post_Status_NA
} CMqueueReqSetattr_Post_Status;

typedef enum {
  CMqueueReqSetattr_Post_Errno_Unchanged,
  CMqueueReqSetattr_Post_Errno_EBADF,
  CMqueueReqSetattr_Post_Errno_EINVAL,
  CMqueueReqSetattr_Post_Errno_NA
} CMqueueReqSetattr_Post_Errno;

typedef enum {
  CMqueueReqSetattr_Post_PrevFlags_Set,
  CMqueueReqSetattr_Post_PrevFlags_Nop,
  CMqueueReqSetattr_Post_PrevFlags_NA
} CMqueueReqSetattr_Post_PrevFlags;

typedef enum {
  CMqueueReqSetattr_Post_PrevMaxMsg_Set,
  CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
  CMqueueReqSetattr_Post_PrevMaxMsg_NA
} CMqueueReqSetattr_Post_PrevMaxMsg;

typedef enum {
  CMqueueReqSetattr_Post_PrevMsgSize_Set,
  CMqueueReqSetattr_Post_PrevMsgSize_Nop,
  CMqueueReqSetattr_Post_PrevMsgSize_NA
} CMqueueReqSetattr_Post_PrevMsgSize;

typedef enum {
  CMqueueReqSetattr_Post_PrevCurMsgs_Set,
  CMqueueReqSetattr_Post_PrevCurMsgs_Nop,
  CMqueueReqSetattr_Post_PrevCurMsgs_NA
} CMqueueReqSetattr_Post_PrevCurMsgs;

typedef enum {
  CMqueueReqSetattr_Post_Flags_Set,
  CMqueueReqSetattr_Post_Flags_Nop,
  CMqueueReqSetattr_Post_Flags_NA
} CMqueueReqSetattr_Post_Flags;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Mqdes_NA : 1;
  uint32_t Pre_MqStat_NA : 1;
  uint32_t Pre_OMqStat_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 2;
  uint32_t Post_PrevFlags : 2;
  uint32_t Post_PrevMaxMsg : 2;
  uint32_t Post_PrevMsgSize : 2;
  uint32_t Post_PrevCurMsgs : 2;
  uint32_t Post_Flags : 2;
} CMqueueReqSetattr_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/setattr test case.
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
   * @brief This member provides the object referenced by the `mqstat`
   *   parameter.
   */
  struct mq_attr mqstat_obj;

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
  int retval;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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

static CMqueueReqSetattr_Context
  CMqueueReqSetattr_Instance;

static const char * const CMqueueReqSetattr_PreDesc_Mqdes[] = {
  "Valid",
  "Closed",
  "Invalid",
  "NA"
};

static const char * const CMqueueReqSetattr_PreDesc_MqStat[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CMqueueReqSetattr_PreDesc_OMqStat[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CMqueueReqSetattr_PreDesc[] = {
  CMqueueReqSetattr_PreDesc_Mqdes,
  CMqueueReqSetattr_PreDesc_MqStat,
  CMqueueReqSetattr_PreDesc_OMqStat,
  NULL
};

#define QUEUE_NAME "mq"

#define MAX_MSG 2

#define MSG_SIZE 4

static const struct mq_attr queue_attr = { 0, MAX_MSG, MSG_SIZE, 0 };

static void CMqueueReqSetattr_Pre_Mqdes_Prepare(
  CMqueueReqSetattr_Context  *ctx,
  CMqueueReqSetattr_Pre_Mqdes state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_Mqdes_Valid: {
      /*
       * While the `mqdes` parameter specifies a message queue descriptor of an
       * open message queue.
       */
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqSetattr_Pre_Mqdes_Closed: {
      /*
       * While the `mqdes` parameter specifies a message queue descriptor of a
       * message queue which was closed and not unlinked.
       */
      T_eq_int( mq_close( ctx->mq ), 0 );
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqSetattr_Pre_Mqdes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = INVALID_ID;
      break;
    }

    case CMqueueReqSetattr_Pre_Mqdes_NA:
      break;
  }
}

static void CMqueueReqSetattr_Pre_MqStat_Prepare(
  CMqueueReqSetattr_Context   *ctx,
  CMqueueReqSetattr_Pre_MqStat state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_MqStat_Valid: {
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

static void CMqueueReqSetattr_Pre_OMqStat_Prepare(
  CMqueueReqSetattr_Context    *ctx,
  CMqueueReqSetattr_Pre_OMqStat state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Pre_OMqStat_Valid: {
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

static void CMqueueReqSetattr_Post_Status_Check(
  CMqueueReqSetattr_Context    *ctx,
  CMqueueReqSetattr_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_Status_Zero: {
      /*
       * The return value of mq_setattr() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CMqueueReqSetattr_Post_Status_MinusOne: {
      /*
       * The return value of mq_setattr() shall be equal to minus one.
       */
      T_eq_int( ctx->retval, -1 );
      break;
    }

    case CMqueueReqSetattr_Post_Status_NA:
      break;
  }
}

static void CMqueueReqSetattr_Post_Errno_Check(
  CMqueueReqSetattr_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqSetattr_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_setattr() call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CMqueueReqSetattr_Post_Errno_EBADF: {
      /*
       * The errno shall be set to EBADF.
       */
      T_eq_int( errno, EBADF );
      break;
    }

    case CMqueueReqSetattr_Post_Errno_EINVAL: {
      /*
       * The errno shall be set to EINVAL.
       */
      T_eq_int( errno, EINVAL );
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
    case CMqueueReqSetattr_Post_PrevFlags_Set: {
      /*
       * The mq_flags member of the object referenced by the `omqstat`
       * parameter shall be set to the flags of the message queue descriptor
       * specified by the `mqdes` parameter before the call.
       */
      T_eq_long( ctx->omqstat_obj.mq_flags, ctx->oflag );
      break;
    }

    case CMqueueReqSetattr_Post_PrevFlags_Nop: {
      /*
       * The mq_flags member of the object referenced by the `omqstat`
       * parameter shall not be changed by the mq_setattr() call.
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
    case CMqueueReqSetattr_Post_PrevMaxMsg_Set: {
      /*
       * The mq_maxmsg member of the object referenced by the `omqstat`
       * parameter shall be set to the maximum number of messages of the
       * message queue specified by the `mqdes` parameter.
       */
      T_eq_long( ctx->omqstat_obj.mq_maxmsg, MAX_MSG );
      break;
    }

    case CMqueueReqSetattr_Post_PrevMaxMsg_Nop: {
      /*
       * The mq_maxmsg member of the object referenced by the `omqstat`
       * parameter shall not be changed by the mq_setattr() call.
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
    case CMqueueReqSetattr_Post_PrevMsgSize_Set: {
      /*
       * The mq_msgsize member of the object referenced by the `omqstat`
       * parameter shall be set to the maximum message size of the message
       * queue specified by the `mqdes` parameter.
       */
      T_eq_long( ctx->omqstat_obj.mq_msgsize, MSG_SIZE );
      break;
    }

    case CMqueueReqSetattr_Post_PrevMsgSize_Nop: {
      /*
       * The mq_msgsize member of the object referenced by the `omqstat`
       * parameter shall not be changed by the mq_setattr() call.
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
    case CMqueueReqSetattr_Post_PrevCurMsgs_Set: {
      /*
       * The mq_curmsgs member of the object referenced by the `omqstat`
       * parameter shall be set to the number of messages in the message queue
       * specified by the `mqdes` parameter before the call.
       */
      T_eq_long( ctx->omqstat_obj.mq_curmsgs, 1 );
      break;
    }

    case CMqueueReqSetattr_Post_PrevCurMsgs_Nop: {
      /*
       * The mq_curmsgs member of the object referenced by the `omqstat`
       * parameter shall not be changed by the mq_setattr() call.
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
  struct mq_attr attr;
  mqd_t          mq;

  mq = mq_open( QUEUE_NAME, O_RDWR );
  T_ne_u32( mq, (mqd_t) -1 );
  T_eq_int( mq_getattr( mq, &attr ), 0 );

  switch ( state ) {
    case CMqueueReqSetattr_Post_Flags_Set: {
      /*
       * The flags of the message queue descriptor specified by the `mqdes`
       * parameter shall be set to the value of the mq_flags member of the
       * object referenced by the `mqstat` parameter.
       */
      T_eq_long( attr.mq_flags, O_NONBLOCK );
      break;
    }

    case CMqueueReqSetattr_Post_Flags_Nop: {
      /*
       * The flags of the message queue descriptor specified by the `mqdes`
       * parameter shall not be changed by the mq_setattr() call.
       */
      T_eq_long( attr.mq_flags, ctx->oflag );
      break;
    }

    case CMqueueReqSetattr_Post_Flags_NA:
      break;
  }

  T_eq_int( mq_close( mq ), 0 );
}

static void CMqueueReqSetattr_Prepare( CMqueueReqSetattr_Context *ctx )
{
  errno = -1;
  ctx->retval = -1;
  ctx->oflag = O_CREAT | O_RDWR;
  ctx->mq = mq_open( QUEUE_NAME, ctx->oflag, S_IRWXU, &queue_attr );
  T_ne_u32( ctx->mq, (mqd_t) -1 );
  T_eq_int( mq_send( ctx->mq, "a", 1, 0 ), 0 );
  ctx->mqstat_obj.mq_flags = O_NONBLOCK;
  ctx->mqstat_obj.mq_maxmsg = 0;
  ctx->mqstat_obj.mq_msgsize = 0;
  ctx->mqstat_obj.mq_curmsgs = 0;
  ctx->omqstat_obj.mq_flags = -1;
  ctx->omqstat_obj.mq_maxmsg = -1;
  ctx->omqstat_obj.mq_msgsize = -1;
  ctx->omqstat_obj.mq_curmsgs = -1;
}

static void CMqueueReqSetattr_Action( CMqueueReqSetattr_Context *ctx )
{
  ctx->retval = mq_setattr( ctx->mqdes, ctx->mqstat, ctx->omqstat );
}

static void CMqueueReqSetattr_Cleanup( CMqueueReqSetattr_Context *ctx )
{
  (void) mq_close( ctx->mq );
  (void) mq_unlink( QUEUE_NAME );
}

static const CMqueueReqSetattr_Entry
CMqueueReqSetattr_Entries[] = {
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_EINVAL, CMqueueReqSetattr_Post_PrevFlags_Nop,
    CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
    CMqueueReqSetattr_Post_PrevMsgSize_Nop,
    CMqueueReqSetattr_Post_PrevCurMsgs_Nop, CMqueueReqSetattr_Post_Flags_Nop },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_EINVAL, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_Nop },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Unchanged,
    CMqueueReqSetattr_Post_PrevFlags_Set,
    CMqueueReqSetattr_Post_PrevMaxMsg_Set,
    CMqueueReqSetattr_Post_PrevMsgSize_Set,
    CMqueueReqSetattr_Post_PrevCurMsgs_Set, CMqueueReqSetattr_Post_Flags_Set },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_Zero,
    CMqueueReqSetattr_Post_Errno_Unchanged,
    CMqueueReqSetattr_Post_PrevFlags_NA, CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_Set },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_EBADF, CMqueueReqSetattr_Post_PrevFlags_Nop,
    CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
    CMqueueReqSetattr_Post_PrevMsgSize_Nop,
    CMqueueReqSetattr_Post_PrevCurMsgs_Nop, CMqueueReqSetattr_Post_Flags_Nop },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_EBADF, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_Nop },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_EBADF, CMqueueReqSetattr_Post_PrevFlags_Nop,
    CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
    CMqueueReqSetattr_Post_PrevMsgSize_Nop,
    CMqueueReqSetattr_Post_PrevCurMsgs_Nop, CMqueueReqSetattr_Post_Flags_NA },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_EBADF, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_NA },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_EINVAL, CMqueueReqSetattr_Post_PrevFlags_Nop,
    CMqueueReqSetattr_Post_PrevMaxMsg_Nop,
    CMqueueReqSetattr_Post_PrevMsgSize_Nop,
    CMqueueReqSetattr_Post_PrevCurMsgs_Nop, CMqueueReqSetattr_Post_Flags_NA },
  { 0, 0, 0, 0, CMqueueReqSetattr_Post_Status_MinusOne,
    CMqueueReqSetattr_Post_Errno_EINVAL, CMqueueReqSetattr_Post_PrevFlags_NA,
    CMqueueReqSetattr_Post_PrevMaxMsg_NA,
    CMqueueReqSetattr_Post_PrevMsgSize_NA,
    CMqueueReqSetattr_Post_PrevCurMsgs_NA, CMqueueReqSetattr_Post_Flags_NA }
};

static const uint8_t
CMqueueReqSetattr_Map[] = {
  2, 3, 0, 1, 4, 5, 0, 1, 6, 7, 8, 9
};

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
  return CMqueueReqSetattr_Entries[
    CMqueueReqSetattr_Map[ index ]
  ];
}

static void CMqueueReqSetattr_TestVariant( CMqueueReqSetattr_Context *ctx )
{
  CMqueueReqSetattr_Pre_Mqdes_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqSetattr_Pre_MqStat_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqSetattr_Pre_OMqStat_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqSetattr_Action( ctx );
  CMqueueReqSetattr_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqSetattr_Post_Errno_Check( ctx->Map.entry.Post_Errno );
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
    ctx->Map.pcs[ 0 ] = CMqueueReqSetattr_Pre_Mqdes_Valid;
    ctx->Map.pcs[ 0 ] < CMqueueReqSetattr_Pre_Mqdes_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CMqueueReqSetattr_Pre_MqStat_Valid;
      ctx->Map.pcs[ 1 ] < CMqueueReqSetattr_Pre_MqStat_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CMqueueReqSetattr_Pre_OMqStat_Valid;
        ctx->Map.pcs[ 2 ] < CMqueueReqSetattr_Pre_OMqStat_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CMqueueReqSetattr_PopEntry( ctx );
        CMqueueReqSetattr_Prepare( ctx );
        CMqueueReqSetattr_TestVariant( ctx );
        CMqueueReqSetattr_Cleanup( ctx );
      }
    }
  }
}

/** @} */
