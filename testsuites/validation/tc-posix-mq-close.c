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

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqClose spec:/c/mqueue/req/close
 *
 * @{
 */

typedef enum {
  CMqueueReqClose_Pre_Mqdes_Valid,
  CMqueueReqClose_Pre_Mqdes_Closed,
  CMqueueReqClose_Pre_Mqdes_Invalid,
  CMqueueReqClose_Pre_Mqdes_NA
} CMqueueReqClose_Pre_Mqdes;

typedef enum {
  CMqueueReqClose_Pre_Opens_One,
  CMqueueReqClose_Pre_Opens_Several,
  CMqueueReqClose_Pre_Opens_NA
} CMqueueReqClose_Pre_Opens;

typedef enum {
  CMqueueReqClose_Post_Status_Zero,
  CMqueueReqClose_Post_Status_MinusOne,
  CMqueueReqClose_Post_Status_NA
} CMqueueReqClose_Post_Status;

typedef enum {
  CMqueueReqClose_Post_Errno_Unchanged,
  CMqueueReqClose_Post_Errno_EBADF,
  CMqueueReqClose_Post_Errno_NA
} CMqueueReqClose_Post_Errno;

typedef enum {
  CMqueueReqClose_Post_Access_Granted,
  CMqueueReqClose_Post_Access_Denied,
  CMqueueReqClose_Post_Access_NA
} CMqueueReqClose_Post_Access;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Mqdes_NA : 1;
  uint16_t Pre_Opens_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Access : 2;
} CMqueueReqClose_Entry;

#define MQ_NAME "/mqclose"

#define MQ_FAILED ( ( mqd_t ) - 1 )

/**
 * @brief Test context for spec:/c/mqueue/req/close test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `mqdes` parameter value.
   */
  mqd_t mqdes;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the descriptor returned by the first mq_open()
   *   call.
   */
  mqd_t mq;

  /**
   * @brief This member contains the descriptor returned by the second
   *   mq_open() call.
   */
  mqd_t mq_other;

  /**
   * @brief This member provides the object used to query the attributes.
   */
  struct mq_attr attr;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 2 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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

static CMqueueReqClose_Context
  CMqueueReqClose_Instance;

static const char * const CMqueueReqClose_PreDesc_Mqdes[] = {
  "Valid",
  "Closed",
  "Invalid",
  "NA"
};

static const char * const CMqueueReqClose_PreDesc_Opens[] = {
  "One",
  "Several",
  "NA"
};

static const char * const * const CMqueueReqClose_PreDesc[] = {
  CMqueueReqClose_PreDesc_Mqdes,
  CMqueueReqClose_PreDesc_Opens,
  NULL
};

static void CMqueueReqClose_Pre_Mqdes_Prepare(
  CMqueueReqClose_Context  *ctx,
  CMqueueReqClose_Pre_Mqdes state
)
{
  switch ( state ) {
    case CMqueueReqClose_Pre_Mqdes_Valid: {
      /*
       * While the `mqdes` parameter is a message queue descriptor which is
       * open.
       */
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqClose_Pre_Mqdes_Closed: {
      /*
       * While the `mqdes` parameter is a message queue descriptor which was
       * already closed.
       */
      T_eq_int( mq_close( ctx->mq ), 0 );
      ctx->mqdes = ctx->mq;
      break;
    }

    case CMqueueReqClose_Pre_Mqdes_Invalid: {
      /*
       * While the `mqdes` parameter is not a message queue descriptor.
       */
      ctx->mqdes = 0;
      break;
    }

    case CMqueueReqClose_Pre_Mqdes_NA:
      break;
  }
}

static void CMqueueReqClose_Pre_Opens_Prepare(
  CMqueueReqClose_Context  *ctx,
  CMqueueReqClose_Pre_Opens state
)
{
  switch ( state ) {
    case CMqueueReqClose_Pre_Opens_One: {
      /*
       * While the message queue is open once.
       */
      /* Already prepared */
      break;
    }

    case CMqueueReqClose_Pre_Opens_Several: {
      /*
       * While the message queue is open more than once.
       */
      ctx->mq_other = mq_open( MQ_NAME, O_RDWR );
      T_ne_u32( ctx->mq_other, MQ_FAILED );
      break;
    }

    case CMqueueReqClose_Pre_Opens_NA:
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
       * The return value of mq_close() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CMqueueReqClose_Post_Status_MinusOne: {
      /*
       * The return value of mq_close() shall be equal to minus one.
       */
      T_eq_int( ctx->retval, -1 );
      break;
    }

    case CMqueueReqClose_Post_Status_NA:
      break;
  }
}

static void CMqueueReqClose_Post_Errno_Check(
  CMqueueReqClose_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqClose_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_close() call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CMqueueReqClose_Post_Errno_EBADF: {
      /*
       * The errno shall be set to EBADF.
       */
      T_eq_int( errno, EBADF );
      break;
    }

    case CMqueueReqClose_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqClose_Post_Access_Check(
  CMqueueReqClose_Context    *ctx,
  CMqueueReqClose_Post_Access state
)
{
  switch ( state ) {
    case CMqueueReqClose_Post_Access_Granted: {
      /*
       * The message queue descriptor specified by `mqdes` shall still grant
       * access to the message queue after the mq_close() call.
       */
      T_eq_int( mq_getattr( ctx->mqdes, &ctx->attr ), 0 );
      break;
    }

    case CMqueueReqClose_Post_Access_Denied: {
      /*
       * The message queue descriptor specified by `mqdes` shall not grant
       * access to the message queue after the mq_close() call.
       */
      T_eq_int( mq_getattr( ctx->mqdes, &ctx->attr ), -1 );
      T_eq_int( errno, EBADF );
      break;
    }

    case CMqueueReqClose_Post_Access_NA:
      break;
  }
}

static void CMqueueReqClose_Prepare( CMqueueReqClose_Context *ctx )
{
  errno = -1;
  ctx->retval = -1;
  ctx->mq = mq_open( MQ_NAME, O_CREAT | O_RDWR, S_IRWXU, NULL );
  T_ne_u32( ctx->mq, MQ_FAILED );
}

static void CMqueueReqClose_Action( CMqueueReqClose_Context *ctx )
{
  ctx->retval = mq_close( ctx->mqdes );
}

static void CMqueueReqClose_Cleanup( CMqueueReqClose_Context *ctx )
{
  /* Remove the name and drop all remaining opens of the message queue */
  (void) mq_unlink( MQ_NAME );

  while ( mq_close( ctx->mq ) == 0 ) {
    /* Do nothing */
  }
}

static const CMqueueReqClose_Entry
CMqueueReqClose_Entries[] = {
  { 0, 0, 1, CMqueueReqClose_Post_Status_MinusOne,
    CMqueueReqClose_Post_Errno_EBADF, CMqueueReqClose_Post_Access_Denied },
  { 0, 0, 1, CMqueueReqClose_Post_Status_MinusOne,
    CMqueueReqClose_Post_Errno_EBADF, CMqueueReqClose_Post_Access_NA },
  { 0, 0, 0, CMqueueReqClose_Post_Status_Zero,
    CMqueueReqClose_Post_Errno_Unchanged, CMqueueReqClose_Post_Access_Denied },
  { 0, 0, 0, CMqueueReqClose_Post_Status_Zero,
    CMqueueReqClose_Post_Errno_Unchanged, CMqueueReqClose_Post_Access_Granted }
};

static const uint8_t
CMqueueReqClose_Map[] = {
  2, 3, 0, 0, 1, 1
};

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
  return CMqueueReqClose_Entries[
    CMqueueReqClose_Map[ index ]
  ];
}

static void CMqueueReqClose_SetPreConditionStates(
  CMqueueReqClose_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Opens_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqClose_Pre_Opens_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CMqueueReqClose_TestVariant( CMqueueReqClose_Context *ctx )
{
  CMqueueReqClose_Pre_Mqdes_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqClose_Pre_Opens_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqClose_Action( ctx );
  CMqueueReqClose_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqClose_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CMqueueReqClose_Post_Access_Check( ctx, ctx->Map.entry.Post_Access );
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
    ctx->Map.pci[ 0 ] = CMqueueReqClose_Pre_Mqdes_Valid;
    ctx->Map.pci[ 0 ] < CMqueueReqClose_Pre_Mqdes_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqClose_Pre_Opens_One;
      ctx->Map.pci[ 1 ] < CMqueueReqClose_Pre_Opens_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CMqueueReqClose_PopEntry( ctx );
      CMqueueReqClose_SetPreConditionStates( ctx );
      CMqueueReqClose_Prepare( ctx );
      CMqueueReqClose_TestVariant( ctx );
      CMqueueReqClose_Cleanup( ctx );
    }
  }
}

/** @} */
