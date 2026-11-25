/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqTimedsendTimeout
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
#include <mqueue.h>
#include <string.h>
#include <rtems/posix/mqueueimpl.h>

#include "tr-tq-timeout.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqTimedsendTimeout spec:/c/mqueue/req/timedsend-timeout
 *
 * @ingroup TestsuitesValidationMqueue
 *
 * @{
 */

typedef enum {
  CMqueueReqTimedsendTimeout_Pre_Access_ReadWrite,
  CMqueueReqTimedsendTimeout_Pre_Access_WriteOnly,
  CMqueueReqTimedsendTimeout_Pre_Access_NA
} CMqueueReqTimedsendTimeout_Pre_Access;

typedef enum {
  CMqueueReqTimedsendTimeout_Post_Action_Timeout,
  CMqueueReqTimedsendTimeout_Post_Action_NA
} CMqueueReqTimedsendTimeout_Post_Action;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Access_NA : 1;
  uint8_t Post_Action : 1;
} CMqueueReqTimedsendTimeout_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/timedsend-timeout test case.
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

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    CMqueueReqTimedsendTimeout_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqTimedsendTimeout_Context;

static CMqueueReqTimedsendTimeout_Context CMqueueReqTimedsendTimeout_Instance;

static const char *const CMqueueReqTimedsendTimeout_PreDesc_Access[] =
  { "ReadWrite", "WriteOnly", "NA" };

static const char *const *const CMqueueReqTimedsendTimeout_PreDesc[] = {
  CMqueueReqTimedsendTimeout_PreDesc_Access,
  NULL
};

typedef CMqueueReqTimedsendTimeout_Context Context;

#define NAME "/mq-timedsend-timeout"

#define MAX_MSG 2

#define MSG_SIZE 4

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

static void Send( Context *ctx )
{
  char buf[ MSG_SIZE ];
  int  oflag;

  /* Send through a descriptor open for writing which does not block */
  oflag = ctx->queue->oflag;
  ctx->queue->oflag = O_RDWR | O_NONBLOCK;
  memset( buf, 0, sizeof( buf ) );
  T_eq_int( mq_send( ctx->queue_id, buf, sizeof( buf ), 0 ), 0 );
  ctx->queue->oflag = oflag;
}

static void Drain( Context *ctx )
{
  char buf[ MSG_SIZE ];
  int  oflag;

  /* Receive through a descriptor open for reading which does not block */
  oflag = ctx->queue->oflag;
  ctx->queue->oflag = O_RDWR | O_NONBLOCK;

  while ( mq_receive( ctx->queue_id, buf, sizeof( buf ), NULL ) >= 0 ) {
    /* Remove each message */
  }

  ctx->queue->oflag = oflag;
}

static void EnqueuePrepare( TQContext *tq_ctx )
{
  Send( ToContext( tq_ctx ) );
  Send( ToContext( tq_ctx ) );
}

static Status_Control Enqueue( TQContext *tq_ctx, TQWait wait )
{
  Context        *ctx;
  char            buf[ MSG_SIZE ];
  struct timespec abstime = { .tv_sec = INT64_MAX, .tv_nsec = 0 };
  int             rv;

  ctx = ToContext( tq_ctx );
  (void) wait;
  memset( buf, 0, sizeof( buf ) );
  rv = mq_timedsend( ctx->queue_id, buf, sizeof( buf ), 0, &abstime ) < 0 ? -1
                                                                          : 0;

  return STATUS_BUILD( 0, rv != 0 ? errno : 0 );
}

static void EnqueueDone( TQContext *tq_ctx )
{
  Context *ctx;

  ctx = ToContext( tq_ctx );
  Drain( ctx );
}

static void CMqueueReqTimedsendTimeout_Pre_Access_Prepare(
  CMqueueReqTimedsendTimeout_Context   *ctx,
  CMqueueReqTimedsendTimeout_Pre_Access state
)
{
  switch ( state ) {
    case CMqueueReqTimedsendTimeout_Pre_Access_ReadWrite: {
      /*
       * While the message queue is open for reading and writing.
       */
      ctx->oflag = O_RDWR;
      break;
    }

    case CMqueueReqTimedsendTimeout_Pre_Access_WriteOnly: {
      /*
       * While the message queue is open for writing only.
       */
      ctx->oflag = O_WRONLY;
      break;
    }

    case CMqueueReqTimedsendTimeout_Pre_Access_NA:
      break;
  }
}

static void CMqueueReqTimedsendTimeout_Post_Action_Check(
  CMqueueReqTimedsendTimeout_Context    *ctx,
  CMqueueReqTimedsendTimeout_Post_Action state
)
{
  switch ( state ) {
    case CMqueueReqTimedsendTimeout_Post_Action_Timeout: {
      /*
       * The timeout actions shall be done as specified by
       * spec:/score/tq/req/timeout.
       */
      ctx->tq_ctx.wait = TQ_WAIT_TIMED;
      ScoreTqReqTimeout_Run( &ctx->tq_ctx );
      break;
    }

    case CMqueueReqTimedsendTimeout_Post_Action_NA:
      break;
  }
}

static void CMqueueReqTimedsendTimeout_Setup(
  CMqueueReqTimedsendTimeout_Context *ctx
)
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.enqueue_prepare = EnqueuePrepare;
  ctx->tq_ctx.enqueue_done = EnqueueDone;
  ctx->tq_ctx.enqueue = Enqueue;
  ctx->tq_ctx.surrender = TQDoNothingSuccessfully;
  ctx->tq_ctx.convert_status = TQConvertStatusPOSIX;
  TQInitialize( &ctx->tq_ctx );
}

static void CMqueueReqTimedsendTimeout_Setup_Wrap( void *arg )
{
  CMqueueReqTimedsendTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqTimedsendTimeout_Setup( ctx );
}

static void CMqueueReqTimedsendTimeout_Teardown(
  CMqueueReqTimedsendTimeout_Context *ctx
)
{
  TQDestroy( &ctx->tq_ctx );
}

static void CMqueueReqTimedsendTimeout_Teardown_Wrap( void *arg )
{
  CMqueueReqTimedsendTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CMqueueReqTimedsendTimeout_Teardown( ctx );
}

static void CMqueueReqTimedsendTimeout_Action(
  CMqueueReqTimedsendTimeout_Context *ctx
)
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

static void CMqueueReqTimedsendTimeout_Cleanup(
  CMqueueReqTimedsendTimeout_Context *ctx
)
{
  T_eq_int( mq_close( ctx->queue_id ), 0 );
  T_eq_int( mq_unlink( NAME ), 0 );
}

/* clang-format off */

static const CMqueueReqTimedsendTimeout_Entry
CMqueueReqTimedsendTimeout_Entries[] = {
  { 0, 0, CMqueueReqTimedsendTimeout_Post_Action_Timeout }
};

static const uint8_t
CMqueueReqTimedsendTimeout_Map[] = {
  0, 0
};

/* clang-format on */

static size_t CMqueueReqTimedsendTimeout_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CMqueueReqTimedsendTimeout_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CMqueueReqTimedsendTimeout_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CMqueueReqTimedsendTimeout_Fixture = {
  .setup = CMqueueReqTimedsendTimeout_Setup_Wrap,
  .stop = NULL,
  .teardown = CMqueueReqTimedsendTimeout_Teardown_Wrap,
  .scope = CMqueueReqTimedsendTimeout_Scope,
  .initial_context = &CMqueueReqTimedsendTimeout_Instance
};

static inline CMqueueReqTimedsendTimeout_Entry
CMqueueReqTimedsendTimeout_PopEntry( CMqueueReqTimedsendTimeout_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqTimedsendTimeout_Entries
    [ CMqueueReqTimedsendTimeout_Map[ index ] ];
}

static void CMqueueReqTimedsendTimeout_TestVariant(
  CMqueueReqTimedsendTimeout_Context *ctx
)
{
  CMqueueReqTimedsendTimeout_Pre_Access_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqTimedsendTimeout_Action( ctx );
  CMqueueReqTimedsendTimeout_Post_Action_Check(
    ctx,
    ctx->Map.entry.Post_Action
  );
}

/**
 * @fn void T_case_body_CMqueueReqTimedsendTimeout( void )
 */
T_TEST_CASE_FIXTURE(
  CMqueueReqTimedsendTimeout,
  &CMqueueReqTimedsendTimeout_Fixture
)
{
  CMqueueReqTimedsendTimeout_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CMqueueReqTimedsendTimeout_Pre_Access_ReadWrite;
    ctx->Map.pcs[ 0 ] < CMqueueReqTimedsendTimeout_Pre_Access_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CMqueueReqTimedsendTimeout_PopEntry( ctx );
    CMqueueReqTimedsendTimeout_TestVariant( ctx );
    CMqueueReqTimedsendTimeout_Cleanup( ctx );
  }
}

/** @} */
