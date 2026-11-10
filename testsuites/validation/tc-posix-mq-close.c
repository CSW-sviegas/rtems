/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqClose
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
#include <rtems/posix/mqueueimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqClose spec:/c/mqueue/req/close
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CMqueueReqClose_Pre_MqDes_Valid,
  CMqueueReqClose_Pre_MqDes_Invalid,
  CMqueueReqClose_Pre_MqDes_NA
} CMqueueReqClose_Pre_MqDes;

typedef enum {
  CMqueueReqClose_Pre_Open_No,
  CMqueueReqClose_Pre_Open_Once,
  CMqueueReqClose_Pre_Open_Twice,
  CMqueueReqClose_Pre_Open_NA
} CMqueueReqClose_Pre_Open;

typedef enum {
  CMqueueReqClose_Pre_Name_Yes,
  CMqueueReqClose_Pre_Name_No,
  CMqueueReqClose_Pre_Name_NA
} CMqueueReqClose_Pre_Name;

typedef enum {
  CMqueueReqClose_Post_Status_Zero,
  CMqueueReqClose_Post_Status_MinusOne,
  CMqueueReqClose_Post_Status_NA
} CMqueueReqClose_Post_Status;

typedef enum {
  CMqueueReqClose_Post_Errno_Nop,
  CMqueueReqClose_Post_Errno_Badf,
  CMqueueReqClose_Post_Errno_NA
} CMqueueReqClose_Post_Errno;

typedef enum {
  CMqueueReqClose_Post_Open_No,
  CMqueueReqClose_Post_Open_Once,
  CMqueueReqClose_Post_Open_Nop,
  CMqueueReqClose_Post_Open_NA
} CMqueueReqClose_Post_Open;

typedef enum {
  CMqueueReqClose_Post_Queue_Deleted,
  CMqueueReqClose_Post_Queue_Nop,
  CMqueueReqClose_Post_Queue_NA
} CMqueueReqClose_Post_Queue;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_MqDes_NA : 1;
  uint16_t Pre_Open_NA : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Open : 2;
  uint16_t Post_Queue : 2;
} CMqueueReqClose_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/close test case.
 */
typedef struct {
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
   * @brief This member contains the number of opens before the mq_close()
   *   call.
   */
  uint32_t open_count;

  /**
   * @brief This member contains the number of inactive message queue objects
   *   before the mq_close() call.
   */
  size_t inactive_count;

  /**
   * @brief This member specifies the `mqdes` parameter value.
   */
  mqd_t mqdes;

  /**
   * @brief This member contains the return value of the mq_close() call.
   */
  int status;

  /**
   * @brief This member contains the value of errno after the mq_close() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 3 ];

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
    CMqueueReqClose_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqClose_Context;

static CMqueueReqClose_Context CMqueueReqClose_Instance;

static const char *const CMqueueReqClose_PreDesc_MqDes[] =
  { "Valid", "Invalid", "NA" };

static const char *const CMqueueReqClose_PreDesc_Open[] =
  { "No", "Once", "Twice", "NA" };

static const char *const CMqueueReqClose_PreDesc_Name[] =
  { "Yes", "No", "NA" };

static const char *const *const CMqueueReqClose_PreDesc[] = {
  CMqueueReqClose_PreDesc_MqDes,
  CMqueueReqClose_PreDesc_Open,
  CMqueueReqClose_PreDesc_Name,
  NULL
};

typedef CMqueueReqClose_Context Context;

#define NAME "/mq-close"

#define ERRNO_POISON -1

static POSIX_Message_queue_Control *GetQueue( mqd_t mqdes )
{
  return (POSIX_Message_queue_Control *) _Objects_Get_no_protection(
    (Objects_Id) mqdes,
    &_POSIX_Message_queue_Information
  );
}

static size_t GetInactiveCount( void )
{
  const Chain_Control *chain;
  const Chain_Node    *node;
  size_t               count;

  chain = &_POSIX_Message_queue_Information.Inactive;
  node = _Chain_Immutable_first( chain );
  count = 0;

  while ( node != _Chain_Immutable_tail( chain ) ) {
    ++count;
    node = _Chain_Immutable_next( node );
  }

  return count;
}

static void CMqueueReqClose_Pre_MqDes_Prepare(
  CMqueueReqClose_Context  *ctx,
  CMqueueReqClose_Pre_MqDes state
)
{
  switch ( state ) {
    case CMqueueReqClose_Pre_MqDes_Valid: {
      /*
       * While the `mqdes` parameter is the message queue descriptor of a
       * message queue.
       */
      ctx->mqdes = ctx->queue_id;
      break;
    }

    case CMqueueReqClose_Pre_MqDes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = (mqd_t) INVALID_ID;
      break;
    }

    case CMqueueReqClose_Pre_MqDes_NA:
      break;
  }
}

static void CMqueueReqClose_Pre_Open_Prepare(
  CMqueueReqClose_Context *ctx,
  CMqueueReqClose_Pre_Open state
)
{
  switch ( state ) {
    case CMqueueReqClose_Pre_Open_No: {
      /*
       * While the message queue is not open.
       */
      T_eq_int( mq_close( ctx->queue_id ), 0 );
      break;
    }

    case CMqueueReqClose_Pre_Open_Once: {
      /*
       * While the message queue is open once.
       */
      /* The message queue was opened once by the preparation */
      break;
    }

    case CMqueueReqClose_Pre_Open_Twice: {
      /*
       * While the message queue is open twice.
       */
      T_eq_int( mq_open( NAME, O_RDWR ), ctx->queue_id );
      break;
    }

    case CMqueueReqClose_Pre_Open_NA:
      break;
  }
}

static void CMqueueReqClose_Pre_Name_Prepare( CMqueueReqClose_Pre_Name state )
{
  switch ( state ) {
    case CMqueueReqClose_Pre_Name_Yes: {
      /*
       * While the message queue has a name.
       */
      /* The message queue was created with a name */
      break;
    }

    case CMqueueReqClose_Pre_Name_No: {
      /*
       * While the message queue has no name.
       */
      T_eq_int( mq_unlink( NAME ), 0 );
      break;
    }

    case CMqueueReqClose_Pre_Name_NA:
      break;
  }
}

static void CMqueueReqClose_Post_Status_Check(
  CMqueueReqClose_Context    *ctx,
  CMqueueReqClose_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqClose_Post_Status_Zero: {
      /*
       * The return value of mq_close() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CMqueueReqClose_Post_Status_MinusOne: {
      /*
       * The return value of mq_close() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CMqueueReqClose_Post_Status_NA:
      break;
  }
}

static void CMqueueReqClose_Post_Errno_Check(
  CMqueueReqClose_Context   *ctx,
  CMqueueReqClose_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqClose_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified by the mq_close() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CMqueueReqClose_Post_Errno_Badf: {
      /*
       * The value of errno shall be EBADF.
       */
      T_eq_int( ctx->errno_value, EBADF );
      break;
    }

    case CMqueueReqClose_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqClose_Post_Open_Check(
  CMqueueReqClose_Context  *ctx,
  CMqueueReqClose_Post_Open state
)
{
  switch ( state ) {
    case CMqueueReqClose_Post_Open_No: {
      /*
       * The message queue shall not be open.
       */
      T_eq_u32( ctx->queue->open_count, 0 );
      break;
    }

    case CMqueueReqClose_Post_Open_Once: {
      /*
       * The message queue shall be open once.
       */
      T_eq_u32( ctx->queue->open_count, 1 );
      break;
    }

    case CMqueueReqClose_Post_Open_Nop: {
      /*
       * The number of opens of the message queue shall not be modified by the
       * mq_close() call.
       */
      T_eq_u32( ctx->queue->open_count, ctx->open_count );
      break;
    }

    case CMqueueReqClose_Post_Open_NA:
      break;
  }
}

static void CMqueueReqClose_Post_Queue_Check(
  CMqueueReqClose_Context   *ctx,
  CMqueueReqClose_Post_Queue state
)
{
  switch ( state ) {
    case CMqueueReqClose_Post_Queue_Deleted: {
      /*
       * The message queue shall be deleted.
       */
      T_eq_sz( GetInactiveCount(), ctx->inactive_count + 1 );
      break;
    }

    case CMqueueReqClose_Post_Queue_Nop: {
      /*
       * No message queue shall be deleted by the mq_close() call.
       */
      T_eq_sz( GetInactiveCount(), ctx->inactive_count );
      break;
    }

    case CMqueueReqClose_Post_Queue_NA:
      break;
  }
}

static void CMqueueReqClose_Prepare( CMqueueReqClose_Context *ctx )
{
  ctx->queue_id = mq_open( NAME, O_CREAT | O_EXCL | O_RDWR, 0, NULL );
  T_ne_u32( ctx->queue_id, (mqd_t) -1 );
  ctx->queue = GetQueue( ctx->queue_id );
  T_assert_not_null( ctx->queue );
}

static void CMqueueReqClose_Action( CMqueueReqClose_Context *ctx )
{
  ctx->open_count = ctx->queue->open_count;
  ctx->inactive_count = GetInactiveCount();
  errno = ERRNO_POISON;
  ctx->status = mq_close( ctx->mqdes );
  ctx->errno_value = errno;
}

static void CMqueueReqClose_Cleanup( CMqueueReqClose_Context *ctx )
{
  int i;

  if ( ctx->queue->linked ) {
    T_eq_int( mq_unlink( NAME ), 0 );
  }

  for ( i = 0; i < 2 && ctx->queue->open_count > 0; ++i ) {
    T_eq_int( mq_close( ctx->queue_id ), 0 );
  }
}

/* clang-format off */

static const CMqueueReqClose_Entry
CMqueueReqClose_Entries[] = {
  { 1, 0, 0, 0, CMqueueReqClose_Post_Status_NA, CMqueueReqClose_Post_Errno_NA,
    CMqueueReqClose_Post_Open_NA, CMqueueReqClose_Post_Queue_NA },
  { 0, 0, 0, 0, CMqueueReqClose_Post_Status_Zero,
    CMqueueReqClose_Post_Errno_Nop, CMqueueReqClose_Post_Open_Once,
    CMqueueReqClose_Post_Queue_Nop },
  { 0, 0, 0, 0, CMqueueReqClose_Post_Status_MinusOne,
    CMqueueReqClose_Post_Errno_Badf, CMqueueReqClose_Post_Open_Nop,
    CMqueueReqClose_Post_Queue_Nop },
  { 0, 0, 0, 0, CMqueueReqClose_Post_Status_Zero,
    CMqueueReqClose_Post_Errno_Nop, CMqueueReqClose_Post_Open_No,
    CMqueueReqClose_Post_Queue_Nop },
  { 0, 0, 0, 0, CMqueueReqClose_Post_Status_Zero,
    CMqueueReqClose_Post_Errno_Nop, CMqueueReqClose_Post_Open_NA,
    CMqueueReqClose_Post_Queue_Deleted },
  { 0, 0, 1, 1, CMqueueReqClose_Post_Status_MinusOne,
    CMqueueReqClose_Post_Errno_Badf, CMqueueReqClose_Post_Open_NA,
    CMqueueReqClose_Post_Queue_Nop }
};

static const uint8_t
CMqueueReqClose_Map[] = {
  2, 0, 3, 4, 1, 1, 5, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CMqueueReqClose_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqClose_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqClose_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqClose_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CMqueueReqClose_Scope,
  .initial_context = &CMqueueReqClose_Instance
};

static inline CMqueueReqClose_Entry CMqueueReqClose_PopEntry(
  CMqueueReqClose_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqClose_Entries[ CMqueueReqClose_Map[ index ] ];
}

static void CMqueueReqClose_SetPreConditionStates(
  CMqueueReqClose_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqClose_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Name_NA ) {
    ctx->Map.pcs[ 2 ] = CMqueueReqClose_Pre_Name_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CMqueueReqClose_TestVariant( CMqueueReqClose_Context *ctx )
{
  CMqueueReqClose_Pre_MqDes_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqClose_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqClose_Pre_Name_Prepare( ctx->Map.pcs[ 2 ] );
  CMqueueReqClose_Action( ctx );
  CMqueueReqClose_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqClose_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CMqueueReqClose_Post_Open_Check( ctx, ctx->Map.entry.Post_Open );
  CMqueueReqClose_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
}

/**
 * @fn void T_case_body_CMqueueReqClose( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqClose, &CMqueueReqClose_Fixture )
{
  CMqueueReqClose_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqClose_Pre_MqDes_Valid;
    ctx->Map.pci[ 0 ] < CMqueueReqClose_Pre_MqDes_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqClose_Pre_Open_No;
      ctx->Map.pci[ 1 ] < CMqueueReqClose_Pre_Open_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqClose_Pre_Name_Yes;
        ctx->Map.pci[ 2 ] < CMqueueReqClose_Pre_Name_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CMqueueReqClose_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CMqueueReqClose_SetPreConditionStates( ctx );
        CMqueueReqClose_Prepare( ctx );
        CMqueueReqClose_TestVariant( ctx );
        CMqueueReqClose_Cleanup( ctx );
      }
    }
  }
}

/** @} */
