/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqUnlink
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
#include <sys/stat.h>

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqUnlink spec:/c/mqueue/req/unlink
 *
 * @{
 */

typedef enum {
  CMqueueReqUnlink_Pre_Name_Valid,
  CMqueueReqUnlink_Pre_Name_Unknown,
  CMqueueReqUnlink_Pre_Name_TooLong,
  CMqueueReqUnlink_Pre_Name_Null,
  CMqueueReqUnlink_Pre_Name_NA
} CMqueueReqUnlink_Pre_Name;

typedef enum {
  CMqueueReqUnlink_Pre_Open_Yes,
  CMqueueReqUnlink_Pre_Open_No,
  CMqueueReqUnlink_Pre_Open_NA
} CMqueueReqUnlink_Pre_Open;

typedef enum {
  CMqueueReqUnlink_Post_Status_Zero,
  CMqueueReqUnlink_Post_Status_MinusOne,
  CMqueueReqUnlink_Post_Status_NA
} CMqueueReqUnlink_Post_Status;

typedef enum {
  CMqueueReqUnlink_Post_Errno_Unchanged,
  CMqueueReqUnlink_Post_Errno_EINVAL,
  CMqueueReqUnlink_Post_Errno_ENAMETOOLONG,
  CMqueueReqUnlink_Post_Errno_ENOENT,
  CMqueueReqUnlink_Post_Errno_NA
} CMqueueReqUnlink_Post_Errno;

typedef enum {
  CMqueueReqUnlink_Post_Name_Removed,
  CMqueueReqUnlink_Post_Name_Nop,
  CMqueueReqUnlink_Post_Name_NA
} CMqueueReqUnlink_Post_Name;

typedef enum {
  CMqueueReqUnlink_Post_Mqdes_Valid,
  CMqueueReqUnlink_Post_Mqdes_NA
} CMqueueReqUnlink_Post_Mqdes;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Open_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 3;
  uint16_t Post_Name : 2;
  uint16_t Post_Mqdes : 1;
} CMqueueReqUnlink_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/unlink test case.
 */
typedef struct {
  /**
   * @brief This member contains the descriptor of the message queue created
   *   for the test variant.
   */
  mqd_t mq;

  /**
   * @brief This member provides a string longer than the maximum message queue
   *   name length.
   */
  char long_name[ _POSIX_PATH_MAX + 2 ];

  /**
   * @brief This member specifies the `name` parameter value.
   */
  const char *name;

  /**
   * @brief This member contains the return value of the mq_unlink() call.
   */
  int retval;

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
    CMqueueReqUnlink_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqUnlink_Context;

static CMqueueReqUnlink_Context
  CMqueueReqUnlink_Instance;

static const char * const CMqueueReqUnlink_PreDesc_Name[] = {
  "Valid",
  "Unknown",
  "TooLong",
  "Null",
  "NA"
};

static const char * const CMqueueReqUnlink_PreDesc_Open[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const CMqueueReqUnlink_PreDesc[] = {
  CMqueueReqUnlink_PreDesc_Name,
  CMqueueReqUnlink_PreDesc_Open,
  NULL
};

#define QUEUE_NAME "mq"

static const struct mq_attr queue_attr = { 0, 1, 1, 0 };

static void CMqueueReqUnlink_Pre_Name_Prepare(
  CMqueueReqUnlink_Context *ctx,
  CMqueueReqUnlink_Pre_Name state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Pre_Name_Valid: {
      /*
       * While the `name` parameter is the name of an existing message queue.
       */
      ctx->name = QUEUE_NAME;
      break;
    }

    case CMqueueReqUnlink_Pre_Name_Unknown: {
      /*
       * While the `name` parameter is not the name of a message queue.
       */
      ctx->name = "unknown";
      break;
    }

    case CMqueueReqUnlink_Pre_Name_TooLong: {
      /*
       * While the length of the string referenced by the `name` parameter is
       * greater than the maximum length of a message queue name.
       */
      memset( ctx->long_name, 'a', _POSIX_PATH_MAX + 1 );
      ctx->long_name[ _POSIX_PATH_MAX + 1 ] = '\0';
      ctx->name = ctx->long_name;
      break;
    }

    case CMqueueReqUnlink_Pre_Name_Null: {
      /*
       * While the `name` parameter is NULL.
       */
      ctx->name = NULL;
      break;
    }

    case CMqueueReqUnlink_Pre_Name_NA:
      break;
  }
}

static void CMqueueReqUnlink_Pre_Open_Prepare(
  CMqueueReqUnlink_Context *ctx,
  CMqueueReqUnlink_Pre_Open state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Pre_Open_Yes: {
      /*
       * While the message queue is open.
       */
      /* Already prepared */
      break;
    }

    case CMqueueReqUnlink_Pre_Open_No: {
      /*
       * While the message queue is not open.
       */
      T_eq_int( mq_close( ctx->mq ), 0 );
      break;
    }

    case CMqueueReqUnlink_Pre_Open_NA:
      break;
  }
}

static void CMqueueReqUnlink_Post_Status_Check(
  CMqueueReqUnlink_Context    *ctx,
  CMqueueReqUnlink_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Post_Status_Zero: {
      /*
       * The return value of mq_unlink() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CMqueueReqUnlink_Post_Status_MinusOne: {
      /*
       * The return value of mq_unlink() shall be equal to minus one.
       */
      T_eq_int( ctx->retval, -1 );
      break;
    }

    case CMqueueReqUnlink_Post_Status_NA:
      break;
  }
}

static void CMqueueReqUnlink_Post_Errno_Check(
  CMqueueReqUnlink_Post_Errno state
)
{
  switch ( state ) {
    case CMqueueReqUnlink_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_unlink() call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CMqueueReqUnlink_Post_Errno_EINVAL: {
      /*
       * The errno shall be set to EINVAL.
       */
      T_eq_int( errno, EINVAL );
      break;
    }

    case CMqueueReqUnlink_Post_Errno_ENAMETOOLONG: {
      /*
       * The errno shall be set to ENAMETOOLONG.
       */
      T_eq_int( errno, ENAMETOOLONG );
      break;
    }

    case CMqueueReqUnlink_Post_Errno_ENOENT: {
      /*
       * The errno shall be set to ENOENT.
       */
      T_eq_int( errno, ENOENT );
      break;
    }

    case CMqueueReqUnlink_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqUnlink_Post_Name_Check(
  CMqueueReqUnlink_Post_Name state
)
{
  mqd_t mq;

  mq = mq_open( QUEUE_NAME, O_RDWR );

  switch ( state ) {
    case CMqueueReqUnlink_Post_Name_Removed: {
      /*
       * The name specified by the `name` parameter shall not identify a
       * message queue after the return of the mq_unlink() call.
       */
      T_eq_u32( mq, (mqd_t) -1 );
      T_eq_int( errno, ENOENT );
      break;
    }

    case CMqueueReqUnlink_Post_Name_Nop: {
      /*
       * The message queue shall be identified by its name after the return of
       * the mq_unlink() call.
       */
      T_ne_u32( mq, (mqd_t) -1 );
      T_eq_int( mq_close( mq ), 0 );
      break;
    }

    case CMqueueReqUnlink_Post_Name_NA:
      break;
  }
}

static void CMqueueReqUnlink_Post_Mqdes_Check(
  CMqueueReqUnlink_Context   *ctx,
  CMqueueReqUnlink_Post_Mqdes state
)
{
  struct mq_attr attr;

  switch ( state ) {
    case CMqueueReqUnlink_Post_Mqdes_Valid: {
      /*
       * The message queue descriptor of the message queue opened before the
       * mq_unlink() call shall still grant access to the message queue after
       * the return of the mq_unlink() call.
       */
      T_eq_int( mq_getattr( ctx->mq, &attr ), 0 );
      break;
    }

    case CMqueueReqUnlink_Post_Mqdes_NA:
      break;
  }
}

static void CMqueueReqUnlink_Prepare( CMqueueReqUnlink_Context *ctx )
{
  errno = -1;
  ctx->retval = -1;
  ctx->mq = mq_open( QUEUE_NAME, O_CREAT | O_RDWR, S_IRWXU, &queue_attr );
  T_ne_u32( ctx->mq, (mqd_t) -1 );
}

static void CMqueueReqUnlink_Action( CMqueueReqUnlink_Context *ctx )
{
  ctx->retval = mq_unlink( ctx->name );
}

static void CMqueueReqUnlink_Cleanup( CMqueueReqUnlink_Context *ctx )
{
  (void) mq_close( ctx->mq );
  (void) mq_unlink( QUEUE_NAME );
}

static const CMqueueReqUnlink_Entry
CMqueueReqUnlink_Entries[] = {
  { 0, 0, 1, CMqueueReqUnlink_Post_Status_MinusOne,
    CMqueueReqUnlink_Post_Errno_ENOENT, CMqueueReqUnlink_Post_Name_Nop,
    CMqueueReqUnlink_Post_Mqdes_Valid },
  { 0, 0, 1, CMqueueReqUnlink_Post_Status_MinusOne,
    CMqueueReqUnlink_Post_Errno_ENAMETOOLONG, CMqueueReqUnlink_Post_Name_Nop,
    CMqueueReqUnlink_Post_Mqdes_Valid },
  { 0, 0, 1, CMqueueReqUnlink_Post_Status_MinusOne,
    CMqueueReqUnlink_Post_Errno_EINVAL, CMqueueReqUnlink_Post_Name_Nop,
    CMqueueReqUnlink_Post_Mqdes_Valid },
  { 0, 0, 0, CMqueueReqUnlink_Post_Status_Zero,
    CMqueueReqUnlink_Post_Errno_Unchanged, CMqueueReqUnlink_Post_Name_Removed,
    CMqueueReqUnlink_Post_Mqdes_Valid },
  { 0, 0, 0, CMqueueReqUnlink_Post_Status_Zero,
    CMqueueReqUnlink_Post_Errno_Unchanged, CMqueueReqUnlink_Post_Name_Removed,
    CMqueueReqUnlink_Post_Mqdes_NA }
};

static const uint8_t
CMqueueReqUnlink_Map[] = {
  3, 4, 0, 0, 1, 1, 2, 2
};

static size_t CMqueueReqUnlink_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqUnlink_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqUnlink_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqUnlink_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CMqueueReqUnlink_Scope,
  .initial_context = &CMqueueReqUnlink_Instance
};

static inline CMqueueReqUnlink_Entry CMqueueReqUnlink_PopEntry(
  CMqueueReqUnlink_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqUnlink_Entries[
    CMqueueReqUnlink_Map[ index ]
  ];
}

static void CMqueueReqUnlink_SetPreConditionStates(
  CMqueueReqUnlink_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Open_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqUnlink_Pre_Open_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CMqueueReqUnlink_TestVariant( CMqueueReqUnlink_Context *ctx )
{
  CMqueueReqUnlink_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqUnlink_Pre_Open_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqUnlink_Action( ctx );
  CMqueueReqUnlink_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqUnlink_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CMqueueReqUnlink_Post_Name_Check( ctx->Map.entry.Post_Name );
  CMqueueReqUnlink_Post_Mqdes_Check( ctx, ctx->Map.entry.Post_Mqdes );
}

/**
 * @fn void T_case_body_CMqueueReqUnlink( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqUnlink, &CMqueueReqUnlink_Fixture )
{
  CMqueueReqUnlink_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqUnlink_Pre_Name_Valid;
    ctx->Map.pci[ 0 ] < CMqueueReqUnlink_Pre_Name_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqUnlink_Pre_Open_Yes;
      ctx->Map.pci[ 1 ] < CMqueueReqUnlink_Pre_Open_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CMqueueReqUnlink_PopEntry( ctx );
      CMqueueReqUnlink_SetPreConditionStates( ctx );
      CMqueueReqUnlink_Prepare( ctx );
      CMqueueReqUnlink_TestVariant( ctx );
      CMqueueReqUnlink_Cleanup( ctx );
    }
  }
}

/** @} */
