/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CMqueueReqOpen
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
#include <stdio.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CMqueueReqOpen spec:/c/mqueue/req/open
 *
 * @{
 */

typedef enum {
  CMqueueReqOpen_Pre_Name_Valid,
  CMqueueReqOpen_Pre_Name_Null,
  CMqueueReqOpen_Pre_Name_TooLong,
  CMqueueReqOpen_Pre_Name_NA
} CMqueueReqOpen_Pre_Name;

typedef enum {
  CMqueueReqOpen_Pre_Exists_Yes,
  CMqueueReqOpen_Pre_Exists_No,
  CMqueueReqOpen_Pre_Exists_NA
} CMqueueReqOpen_Pre_Exists;

typedef enum {
  CMqueueReqOpen_Pre_Creat_Set,
  CMqueueReqOpen_Pre_Creat_Unset,
  CMqueueReqOpen_Pre_Creat_NA
} CMqueueReqOpen_Pre_Creat;

typedef enum {
  CMqueueReqOpen_Pre_Excl_Set,
  CMqueueReqOpen_Pre_Excl_Unset,
  CMqueueReqOpen_Pre_Excl_NA
} CMqueueReqOpen_Pre_Excl;

typedef enum {
  CMqueueReqOpen_Pre_Attr_Null,
  CMqueueReqOpen_Pre_Attr_Valid,
  CMqueueReqOpen_Pre_Attr_InvMaxMsg,
  CMqueueReqOpen_Pre_Attr_InvMsgSize,
  CMqueueReqOpen_Pre_Attr_Huge,
  CMqueueReqOpen_Pre_Attr_NA
} CMqueueReqOpen_Pre_Attr;

typedef enum {
  CMqueueReqOpen_Pre_Free_Yes,
  CMqueueReqOpen_Pre_Free_No,
  CMqueueReqOpen_Pre_Free_NA
} CMqueueReqOpen_Pre_Free;

typedef enum {
  CMqueueReqOpen_Pre_Memory_Available,
  CMqueueReqOpen_Pre_Memory_None,
  CMqueueReqOpen_Pre_Memory_NA
} CMqueueReqOpen_Pre_Memory;

typedef enum {
  CMqueueReqOpen_Post_Status_Mqdes,
  CMqueueReqOpen_Post_Status_MinusOne,
  CMqueueReqOpen_Post_Status_NA
} CMqueueReqOpen_Post_Status;

typedef enum {
  CMqueueReqOpen_Post_Errno_Unchanged,
  CMqueueReqOpen_Post_Errno_EINVAL,
  CMqueueReqOpen_Post_Errno_ENAMETOOLONG,
  CMqueueReqOpen_Post_Errno_ENOENT,
  CMqueueReqOpen_Post_Errno_EEXIST,
  CMqueueReqOpen_Post_Errno_ENFILE,
  CMqueueReqOpen_Post_Errno_ENOMEM,
  CMqueueReqOpen_Post_Errno_ENOSPC,
  CMqueueReqOpen_Post_Errno_NA
} CMqueueReqOpen_Post_Errno;

typedef enum {
  CMqueueReqOpen_Post_Attr_Default,
  CMqueueReqOpen_Post_Attr_Given,
  CMqueueReqOpen_Post_Attr_Existing,
  CMqueueReqOpen_Post_Attr_NA
} CMqueueReqOpen_Post_Attr;

typedef enum {
  CMqueueReqOpen_Post_Linked_Yes,
  CMqueueReqOpen_Post_Linked_No,
  CMqueueReqOpen_Post_Linked_NA
} CMqueueReqOpen_Post_Linked;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Name_NA : 1;
  uint32_t Pre_Exists_NA : 1;
  uint32_t Pre_Creat_NA : 1;
  uint32_t Pre_Excl_NA : 1;
  uint32_t Pre_Attr_NA : 1;
  uint32_t Pre_Free_NA : 1;
  uint32_t Pre_Memory_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 4;
  uint32_t Post_Attr : 2;
  uint32_t Post_Linked : 2;
} CMqueueReqOpen_Entry;

/**
 * @brief Test context for spec:/c/mqueue/req/open test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `name` parameter value.
   */
  const char *name;

  /**
   * @brief This member specifies the `oflag` parameter value.
   */
  int oflag;

  /**
   * @brief This member specifies the mode argument value.
   */
  mode_t mode;

  /**
   * @brief This member specifies the message queue attributes argument value.
   */
  const struct mq_attr *attr;

  /**
   * @brief This member contains the return value.
   */
  mqd_t retval;

  /**
   * @brief This member contains the message queue attributes.
   */
  struct mq_attr attr_obj;

  /**
   * @brief This member contains the message queue descriptor of the existing
   *   message queue.
   */
  mqd_t mqd_obj;

  /**
   * @brief This member contains a name which is too long.
   */
  char long_name[ _POSIX_PATH_MAX + 2 ];

  /**
   * @brief This member contains the seized message queue objects.
   */
  void *seized_objects;

  /**
   * @brief This member contains the count of seized message queue objects.
   */
  size_t seized_count;

  /**
   * @brief This member contains the greedy workspace allocation.
   */
  void *greedy;

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
    CMqueueReqOpen_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CMqueueReqOpen_Context;

static CMqueueReqOpen_Context
  CMqueueReqOpen_Instance;

static const char * const CMqueueReqOpen_PreDesc_Name[] = {
  "Valid",
  "Null",
  "TooLong",
  "NA"
};

static const char * const CMqueueReqOpen_PreDesc_Exists[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const CMqueueReqOpen_PreDesc_Creat[] = {
  "Set",
  "Unset",
  "NA"
};

static const char * const CMqueueReqOpen_PreDesc_Excl[] = {
  "Set",
  "Unset",
  "NA"
};

static const char * const CMqueueReqOpen_PreDesc_Attr[] = {
  "Null",
  "Valid",
  "InvMaxMsg",
  "InvMsgSize",
  "Huge",
  "NA"
};

static const char * const CMqueueReqOpen_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const CMqueueReqOpen_PreDesc_Memory[] = {
  "Available",
  "None",
  "NA"
};

static const char * const * const CMqueueReqOpen_PreDesc[] = {
  CMqueueReqOpen_PreDesc_Name,
  CMqueueReqOpen_PreDesc_Exists,
  CMqueueReqOpen_PreDesc_Creat,
  CMqueueReqOpen_PreDesc_Excl,
  CMqueueReqOpen_PreDesc_Attr,
  CMqueueReqOpen_PreDesc_Free,
  CMqueueReqOpen_PreDesc_Memory,
  NULL
};

typedef CMqueueReqOpen_Context Context;

#define NAME "/mq"

#define MQ_OPEN_FAILED ( ( mqd_t ) - 1 )

#define DEFAULT_MAXMSG 10

#define DEFAULT_MSGSIZE 16

#define EXISTING_MAXMSG 3

#define EXISTING_MSGSIZE 8

#define SEIZED_NAME_SIZE 16

static const struct mq_attr existing_attr = {
  .mq_maxmsg = EXISTING_MAXMSG,
  .mq_msgsize = EXISTING_MSGSIZE
};

static const struct mq_attr seized_attr = { .mq_maxmsg = 1, .mq_msgsize = 1 };

static void SeizedName( char *name, size_t i )
{
  (void) snprintf( name, SEIZED_NAME_SIZE, "/seized%zu", i );
}

static rtems_status_code Create( void *arg, uint32_t *id )
{
  Context *ctx;
  char     name[ SEIZED_NAME_SIZE ];
  mqd_t    mqd;

  (void) arg;
  ctx = T_fixture_context();
  SeizedName( name, ctx->seized_count );
  mqd = mq_open( name, O_RDWR | O_CREAT | O_EXCL, 0600, &seized_attr );

  if ( mqd == MQ_OPEN_FAILED ) {
    T_quiet_psx_error( (int) mqd, ENFILE );
    return RTEMS_TOO_MANY;
  }

  ++ctx->seized_count;
  *id = mqd;
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code Delete( uint32_t id )
{
  int rv;

  rv = mq_close( id );
  T_quiet_psx_success( rv );
  return RTEMS_SUCCESSFUL;
}

static void CMqueueReqOpen_Pre_Name_Prepare(
  CMqueueReqOpen_Context *ctx,
  CMqueueReqOpen_Pre_Name state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Name_Valid: {
      /*
       * While the `name` parameter references a valid string which is not
       * longer than the maximum name length.
       */
      ctx->name = NAME;
      break;
    }

    case CMqueueReqOpen_Pre_Name_Null: {
      /*
       * While the `name` parameter is NULL.
       */
      ctx->name = NULL;
      break;
    }

    case CMqueueReqOpen_Pre_Name_TooLong: {
      /*
       * While the `name` parameter references a string which is longer than
       * the maximum name length.
       */
      memset( ctx->long_name, 'a', sizeof( ctx->long_name ) - 1 );
      ctx->long_name[ 0 ] = '/';
      ctx->long_name[ sizeof( ctx->long_name ) - 1 ] = '\0';
      ctx->name = ctx->long_name;
      break;
    }

    case CMqueueReqOpen_Pre_Name_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Exists_Prepare(
  CMqueueReqOpen_Context   *ctx,
  CMqueueReqOpen_Pre_Exists state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Exists_Yes: {
      /*
       * While a message queue with the name specified by `name` exists.
       */
      ctx->mqd_obj = mq_open(
        NAME,
        O_RDWR | O_CREAT | O_EXCL,
        0600,
        &existing_attr
      );
      T_ne_u32( ctx->mqd_obj, MQ_OPEN_FAILED );
      break;
    }

    case CMqueueReqOpen_Pre_Exists_No: {
      /*
       * While no message queue with the name specified by `name` exists.
       */
      /* Nothing to do */
      break;
    }

    case CMqueueReqOpen_Pre_Exists_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Creat_Prepare(
  CMqueueReqOpen_Context  *ctx,
  CMqueueReqOpen_Pre_Creat state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Creat_Set: {
      /*
       * While the O_CREAT flag is set in the `oflag` parameter.
       */
      ctx->oflag |= O_CREAT;
      break;
    }

    case CMqueueReqOpen_Pre_Creat_Unset: {
      /*
       * While the O_CREAT flag is not set in the `oflag` parameter.
       */
      ctx->oflag &= ~O_CREAT;
      break;
    }

    case CMqueueReqOpen_Pre_Creat_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Excl_Prepare(
  CMqueueReqOpen_Context *ctx,
  CMqueueReqOpen_Pre_Excl state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Excl_Set: {
      /*
       * While the O_EXCL flag is set in the `oflag` parameter.
       */
      ctx->oflag |= O_EXCL;
      break;
    }

    case CMqueueReqOpen_Pre_Excl_Unset: {
      /*
       * While the O_EXCL flag is not set in the `oflag` parameter.
       */
      ctx->oflag &= ~O_EXCL;
      break;
    }

    case CMqueueReqOpen_Pre_Excl_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Attr_Prepare(
  CMqueueReqOpen_Context *ctx,
  CMqueueReqOpen_Pre_Attr state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Attr_Null: {
      /*
       * While the optional message queue attributes argument is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CMqueueReqOpen_Pre_Attr_Valid: {
      /*
       * While the optional message queue attributes argument references valid
       * message queue attributes.
       */
      ctx->attr_obj.mq_maxmsg = 5;
      ctx->attr_obj.mq_msgsize = 12;
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CMqueueReqOpen_Pre_Attr_InvMaxMsg: {
      /*
       * While the optional message queue attributes argument references
       * message queue attributes with a maximum number of messages which is
       * not positive.
       */
      ctx->attr_obj.mq_maxmsg = 0;
      ctx->attr_obj.mq_msgsize = 12;
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CMqueueReqOpen_Pre_Attr_InvMsgSize: {
      /*
       * While the optional message queue attributes argument references
       * message queue attributes with a message size which is not positive.
       */
      ctx->attr_obj.mq_maxmsg = 5;
      ctx->attr_obj.mq_msgsize = 0;
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CMqueueReqOpen_Pre_Attr_Huge: {
      /*
       * While the optional message queue attributes argument references
       * message queue attributes which require more message buffer storage
       * than the system can provide.
       */
      ctx->attr_obj.mq_maxmsg = INT_MAX;
      ctx->attr_obj.mq_msgsize = 12;
      ctx->attr = &ctx->attr_obj;
      break;
    }

    case CMqueueReqOpen_Pre_Attr_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Free_Prepare(
  CMqueueReqOpen_Context *ctx,
  CMqueueReqOpen_Pre_Free state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive message queue object
       * available.
       */
      /* Ensured by the test suite configuration */
      break;
    }

    case CMqueueReqOpen_Pre_Free_No: {
      /*
       * While the system has no inactive message queue object available.
       */
      ctx->seized_objects = T_seize_objects( Create, NULL );
      break;
    }

    case CMqueueReqOpen_Pre_Free_NA:
      break;
  }
}

static void CMqueueReqOpen_Pre_Memory_Prepare(
  CMqueueReqOpen_Context   *ctx,
  CMqueueReqOpen_Pre_Memory state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Pre_Memory_Available: {
      /*
       * While the system has enough memory to store a copy of the name.
       */
      /* Nothing to do */
      break;
    }

    case CMqueueReqOpen_Pre_Memory_None: {
      /*
       * While the system has no memory available to store a copy of the name.
       */
      ctx->greedy = rtems_workspace_greedy_allocate( NULL, 0 );
      break;
    }

    case CMqueueReqOpen_Pre_Memory_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Status_Check(
  CMqueueReqOpen_Context    *ctx,
  CMqueueReqOpen_Post_Status state
)
{
  switch ( state ) {
    case CMqueueReqOpen_Post_Status_Mqdes: {
      /*
       * The return value of mq_open() shall be a message queue descriptor.
       */
      T_ne_u32( ctx->retval, MQ_OPEN_FAILED );
      break;
    }

    case CMqueueReqOpen_Post_Status_MinusOne: {
      /*
       * The return value of mq_open() shall be equal to minus one.
       */
      T_eq_u32( ctx->retval, MQ_OPEN_FAILED );
      break;
    }

    case CMqueueReqOpen_Post_Status_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Errno_Check( CMqueueReqOpen_Post_Errno state )
{
  switch ( state ) {
    case CMqueueReqOpen_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the mq_open() call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CMqueueReqOpen_Post_Errno_EINVAL: {
      /*
       * The errno shall be set to EINVAL.
       */
      T_eq_int( errno, EINVAL );
      break;
    }

    case CMqueueReqOpen_Post_Errno_ENAMETOOLONG: {
      /*
       * The errno shall be set to ENAMETOOLONG.
       */
      T_eq_int( errno, ENAMETOOLONG );
      break;
    }

    case CMqueueReqOpen_Post_Errno_ENOENT: {
      /*
       * The errno shall be set to ENOENT.
       */
      T_eq_int( errno, ENOENT );
      break;
    }

    case CMqueueReqOpen_Post_Errno_EEXIST: {
      /*
       * The errno shall be set to EEXIST.
       */
      T_eq_int( errno, EEXIST );
      break;
    }

    case CMqueueReqOpen_Post_Errno_ENFILE: {
      /*
       * The errno shall be set to ENFILE.
       */
      T_eq_int( errno, ENFILE );
      break;
    }

    case CMqueueReqOpen_Post_Errno_ENOMEM: {
      /*
       * The errno shall be set to ENOMEM.
       */
      T_eq_int( errno, ENOMEM );
      break;
    }

    case CMqueueReqOpen_Post_Errno_ENOSPC: {
      /*
       * The errno shall be set to ENOSPC.
       */
      T_eq_int( errno, ENOSPC );
      break;
    }

    case CMqueueReqOpen_Post_Errno_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Attr_Check(
  CMqueueReqOpen_Context  *ctx,
  CMqueueReqOpen_Post_Attr state
)
{
  struct mq_attr attr;
  int            rv;

  switch ( state ) {
    case CMqueueReqOpen_Post_Attr_Default: {
      /*
       * The message queue attributes of the message queue specified by the
       * returned message queue descriptor shall be the default message queue
       * attributes of mq_open().
       */
      rv = mq_getattr( ctx->retval, &attr );
      T_psx_success( rv );
      T_eq_long( attr.mq_maxmsg, DEFAULT_MAXMSG );
      T_eq_long( attr.mq_msgsize, DEFAULT_MSGSIZE );
      break;
    }

    case CMqueueReqOpen_Post_Attr_Given: {
      /*
       * The message queue attributes of the message queue specified by the
       * returned message queue descriptor shall be the message queue
       * attributes specified by the optional argument of mq_open().
       */
      rv = mq_getattr( ctx->retval, &attr );
      T_psx_success( rv );
      T_eq_long( attr.mq_maxmsg, ctx->attr_obj.mq_maxmsg );
      T_eq_long( attr.mq_msgsize, ctx->attr_obj.mq_msgsize );
      break;
    }

    case CMqueueReqOpen_Post_Attr_Existing: {
      /*
       * The message queue attributes of the message queue specified by the
       * returned message queue descriptor shall be the message queue
       * attributes of the existing message queue.
       */
      rv = mq_getattr( ctx->retval, &attr );
      T_psx_success( rv );
      T_eq_long( attr.mq_maxmsg, EXISTING_MAXMSG );
      T_eq_long( attr.mq_msgsize, EXISTING_MSGSIZE );
      break;
    }

    case CMqueueReqOpen_Post_Attr_NA:
      break;
  }
}

static void CMqueueReqOpen_Post_Linked_Check(
  CMqueueReqOpen_Post_Linked state
)
{
  int rv;

  switch ( state ) {
    case CMqueueReqOpen_Post_Linked_Yes: {
      /*
       * A message queue with the name specified by `name` shall exist after
       * the mq_open() call.
       */
      rv = mq_unlink( NAME );
      T_psx_success( rv );
      break;
    }

    case CMqueueReqOpen_Post_Linked_No: {
      /*
       * No message queue with the name specified by `name` shall exist after
       * the mq_open() call.
       */
      rv = mq_unlink( NAME );
      T_psx_error( rv, ENOENT );
      break;
    }

    case CMqueueReqOpen_Post_Linked_NA:
      break;
  }
}

static void CMqueueReqOpen_Prepare( CMqueueReqOpen_Context *ctx )
{
  errno = -1;
  ctx->oflag = O_RDWR;
  ctx->mode = 0600;
  ctx->attr = NULL;
  ctx->retval = MQ_OPEN_FAILED;
  ctx->mqd_obj = MQ_OPEN_FAILED;
  ctx->seized_objects = NULL;
  ctx->seized_count = 0;
  ctx->greedy = NULL;
  memset( &ctx->attr_obj, 0, sizeof( ctx->attr_obj ) );
}

static void CMqueueReqOpen_Action( CMqueueReqOpen_Context *ctx )
{
  ctx->retval = mq_open( ctx->name, ctx->oflag, ctx->mode, ctx->attr );
}

static void CMqueueReqOpen_Cleanup( CMqueueReqOpen_Context *ctx )
{
  size_t i;

  if ( ctx->greedy != NULL ) {
    rtems_workspace_greedy_free( ctx->greedy );
    ctx->greedy = NULL;
  }

  if ( ctx->retval != MQ_OPEN_FAILED ) {
    int rv;

    rv = mq_close( ctx->retval );
    T_psx_success( rv );
  }

  if ( ctx->mqd_obj != MQ_OPEN_FAILED ) {
    int rv;

    rv = mq_close( ctx->mqd_obj );
    T_psx_success( rv );
  }

  (void) mq_unlink( NAME );

  for ( i = 0; i < ctx->seized_count; ++i ) {
    char seized_name[ SEIZED_NAME_SIZE ];

    SeizedName( seized_name, i );
    (void) mq_unlink( seized_name );
  }

  T_surrender_objects( &ctx->seized_objects, Delete );
}

static const CMqueueReqOpen_Entry
CMqueueReqOpen_Entries[] = {
  { 0, 0, 1, 1, 1, 1, 1, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_EINVAL, CMqueueReqOpen_Post_Attr_NA,
    CMqueueReqOpen_Post_Linked_NA },
  { 0, 0, 1, 1, 1, 1, 1, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_ENAMETOOLONG, CMqueueReqOpen_Post_Attr_NA,
    CMqueueReqOpen_Post_Linked_NA },
  { 0, 0, 0, 0, 1, 1, 1, 1, CMqueueReqOpen_Post_Status_Mqdes,
    CMqueueReqOpen_Post_Errno_Unchanged, CMqueueReqOpen_Post_Attr_Existing,
    CMqueueReqOpen_Post_Linked_Yes },
  { 0, 0, 0, 0, 1, 1, 1, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_ENOENT, CMqueueReqOpen_Post_Attr_NA,
    CMqueueReqOpen_Post_Linked_No },
  { 0, 0, 0, 0, 0, 1, 1, 1, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_EEXIST, CMqueueReqOpen_Post_Attr_NA,
    CMqueueReqOpen_Post_Linked_Yes },
  { 0, 0, 0, 0, 0, 1, 1, 1, CMqueueReqOpen_Post_Status_Mqdes,
    CMqueueReqOpen_Post_Errno_Unchanged, CMqueueReqOpen_Post_Attr_Existing,
    CMqueueReqOpen_Post_Linked_Yes },
  { 0, 0, 0, 0, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_EINVAL, CMqueueReqOpen_Post_Attr_NA,
    CMqueueReqOpen_Post_Linked_No },
  { 0, 0, 0, 0, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_ENFILE, CMqueueReqOpen_Post_Attr_NA,
    CMqueueReqOpen_Post_Linked_No },
  { 0, 0, 0, 0, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_ENOMEM, CMqueueReqOpen_Post_Attr_NA,
    CMqueueReqOpen_Post_Linked_No },
  { 0, 0, 0, 0, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_Mqdes,
    CMqueueReqOpen_Post_Errno_Unchanged, CMqueueReqOpen_Post_Attr_Default,
    CMqueueReqOpen_Post_Linked_Yes },
  { 0, 0, 0, 0, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_Mqdes,
    CMqueueReqOpen_Post_Errno_Unchanged, CMqueueReqOpen_Post_Attr_Given,
    CMqueueReqOpen_Post_Linked_Yes },
  { 0, 0, 0, 0, 1, 0, 0, 0, CMqueueReqOpen_Post_Status_MinusOne,
    CMqueueReqOpen_Post_Errno_ENOSPC, CMqueueReqOpen_Post_Attr_NA,
    CMqueueReqOpen_Post_Linked_No }
};

static const uint8_t
CMqueueReqOpen_Map[] = {
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 9, 8, 7, 7, 10, 8, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 11, 8, 7, 7, 9, 8, 7,
  7, 10, 8, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 11, 8, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static size_t CMqueueReqOpen_Scope( void *arg, char *buf, size_t n )
{
  CMqueueReqOpen_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CMqueueReqOpen_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CMqueueReqOpen_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CMqueueReqOpen_Scope,
  .initial_context = &CMqueueReqOpen_Instance
};

static inline CMqueueReqOpen_Entry CMqueueReqOpen_PopEntry(
  CMqueueReqOpen_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CMqueueReqOpen_Entries[
    CMqueueReqOpen_Map[ index ]
  ];
}

static void CMqueueReqOpen_SetPreConditionStates( CMqueueReqOpen_Context *ctx )
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Exists_NA ) {
    ctx->Map.pcs[ 1 ] = CMqueueReqOpen_Pre_Exists_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Creat_NA ) {
    ctx->Map.pcs[ 2 ] = CMqueueReqOpen_Pre_Creat_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Excl_NA ) {
    ctx->Map.pcs[ 3 ] = CMqueueReqOpen_Pre_Excl_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Attr_NA ) {
    ctx->Map.pcs[ 4 ] = CMqueueReqOpen_Pre_Attr_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Free_NA ) {
    ctx->Map.pcs[ 5 ] = CMqueueReqOpen_Pre_Free_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Memory_NA ) {
    ctx->Map.pcs[ 6 ] = CMqueueReqOpen_Pre_Memory_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }
}

static void CMqueueReqOpen_TestVariant( CMqueueReqOpen_Context *ctx )
{
  CMqueueReqOpen_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CMqueueReqOpen_Pre_Exists_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CMqueueReqOpen_Pre_Creat_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CMqueueReqOpen_Pre_Excl_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CMqueueReqOpen_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CMqueueReqOpen_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CMqueueReqOpen_Pre_Memory_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CMqueueReqOpen_Action( ctx );
  CMqueueReqOpen_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CMqueueReqOpen_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CMqueueReqOpen_Post_Attr_Check( ctx, ctx->Map.entry.Post_Attr );
  CMqueueReqOpen_Post_Linked_Check( ctx->Map.entry.Post_Linked );
}

/**
 * @fn void T_case_body_CMqueueReqOpen( void )
 */
T_TEST_CASE_FIXTURE( CMqueueReqOpen, &CMqueueReqOpen_Fixture )
{
  CMqueueReqOpen_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CMqueueReqOpen_Pre_Name_Valid;
    ctx->Map.pci[ 0 ] < CMqueueReqOpen_Pre_Name_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CMqueueReqOpen_Pre_Exists_Yes;
      ctx->Map.pci[ 1 ] < CMqueueReqOpen_Pre_Exists_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CMqueueReqOpen_Pre_Creat_Set;
        ctx->Map.pci[ 2 ] < CMqueueReqOpen_Pre_Creat_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CMqueueReqOpen_Pre_Excl_Set;
          ctx->Map.pci[ 3 ] < CMqueueReqOpen_Pre_Excl_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CMqueueReqOpen_Pre_Attr_Null;
            ctx->Map.pci[ 4 ] < CMqueueReqOpen_Pre_Attr_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CMqueueReqOpen_Pre_Free_Yes;
              ctx->Map.pci[ 5 ] < CMqueueReqOpen_Pre_Free_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CMqueueReqOpen_Pre_Memory_Available;
                ctx->Map.pci[ 6 ] < CMqueueReqOpen_Pre_Memory_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                ctx->Map.entry = CMqueueReqOpen_PopEntry( ctx );
                CMqueueReqOpen_SetPreConditionStates( ctx );
                CMqueueReqOpen_Prepare( ctx );
                CMqueueReqOpen_TestVariant( ctx );
                CMqueueReqOpen_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
