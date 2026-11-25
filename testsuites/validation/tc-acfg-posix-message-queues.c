/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValPosixMessageQueues
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
#include <limits.h>
#include <mqueue.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup AcfgValPosixMessageQueues spec:/acfg/val/posix-message-queues
 *
 * @ingroup TestsuitesValidationMqueue
 *
 * @brief Tests the maximum number of POSIX message queues and the message
 *   buffer memory which the test suite configures.
 *
 * The test suite defines CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES to two and
 * CONFIGURE_MESSAGE_BUFFER_MEMORY to the sum of two
 * CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE values for message queues with at most
 * ten pending messages of 16 bytes.
 *
 * This test case performs the following actions:
 *
 * - Open as many message queues as the test suite configures. Each message
 *   queue has a name with the maximum length, the maximum number of pending
 *   messages and the maximum message size for which the test suite defines the
 *   message buffer memory, so that the message queues use all of the
 *   configured memory.
 *
 *   - Check that each message queue was opened. This shows that the configured
 *     number of message queues can be active at the same time and that the
 *     message buffer memory provides the message buffers of each message
 *     queue. The test suite defines this memory with
 *     CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE.
 *
 * - Try to open one more message queue.
 *
 *   - Check that the open failed with ENFILE, since all configured message
 *     queues are active.
 *
 * @{
 */

#define MAX_QUEUES 2

#define MAX_MSG 10

#define MSG_SIZE 16

#define OTHER_NAME "/acfg-other"

static char names[ MAX_QUEUES ][ _POSIX_PATH_MAX + 1 ];

static mqd_t queues[ MAX_QUEUES ];

static void AcfgValPosixMessageQueues_Setup( void )
{
  size_t i;

  for ( i = 0; i < MAX_QUEUES; ++i ) {
    memset( names[ i ], 'a' + (int) i, sizeof( names[ i ] ) - 1 );
    names[ i ][ 0 ] = '/';
    names[ i ][ sizeof( names[ i ] ) - 1 ] = '\0';
  }
}

static void AcfgValPosixMessageQueues_Setup_Wrap( void *arg )
{
  (void) arg;
  AcfgValPosixMessageQueues_Setup();
}

static void AcfgValPosixMessageQueues_Teardown( void )
{
  size_t i;

  for ( i = 0; i < MAX_QUEUES; ++i ) {
    if ( queues[ i ] != (mqd_t) -1 ) {
      T_eq_int( mq_close( queues[ i ] ), 0 );
      T_eq_int( mq_unlink( names[ i ] ), 0 );
    }
  }
}

static void AcfgValPosixMessageQueues_Teardown_Wrap( void *arg )
{
  (void) arg;
  AcfgValPosixMessageQueues_Teardown();
}

static T_fixture AcfgValPosixMessageQueues_Fixture = {
  .setup = AcfgValPosixMessageQueues_Setup_Wrap,
  .stop = NULL,
  .teardown = AcfgValPosixMessageQueues_Teardown_Wrap,
  .scope = NULL,
  .initial_context = NULL
};

/**
 * @brief Open as many message queues as the test suite configures. Each
 *   message queue has a name with the maximum length, the maximum number of
 *   pending messages and the maximum message size for which the test suite
 *   defines the message buffer memory, so that the message queues use all of
 *   the configured memory.
 */
static void AcfgValPosixMessageQueues_Action_0( void )
{
  struct mq_attr attr;
  size_t         i;

  memset( &attr, 0, sizeof( attr ) );
  attr.mq_maxmsg = MAX_MSG;
  attr.mq_msgsize = MSG_SIZE;

  for ( i = 0; i < MAX_QUEUES; ++i ) {
    queues[ i ] = mq_open( names[ i ], O_CREAT | O_EXCL | O_RDWR, 0, &attr );
  }

  /*
   * Check that each message queue was opened. This shows that the configured
   * number of message queues can be active at the same time and that the
   * message buffer memory provides the message buffers of each message queue.
   * The test suite defines this memory with
   * CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE.
   */
  for ( i = 0; i < MAX_QUEUES; ++i ) {
    T_ne_u32( queues[ i ], (mqd_t) -1 );
  }
}

/**
 * @brief Try to open one more message queue.
 */
static void AcfgValPosixMessageQueues_Action_1( void )
{
  struct mq_attr attr;
  mqd_t          mqdes;
  int            eno;

  memset( &attr, 0, sizeof( attr ) );
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = 1;

  errno = 0;
  mqdes = mq_open( OTHER_NAME, O_CREAT | O_EXCL | O_RDWR, 0, &attr );
  eno = errno;

  /*
   * Check that the open failed with ENFILE, since all configured message
   * queues are active.
   */
  T_eq_u32( mqdes, (mqd_t) -1 );
  T_eq_int( eno, ENFILE );
}

/**
 * @fn void T_case_body_AcfgValPosixMessageQueues( void )
 */
T_TEST_CASE_FIXTURE(
  AcfgValPosixMessageQueues,
  &AcfgValPosixMessageQueues_Fixture
)
{
  AcfgValPosixMessageQueues_Action_0();
  AcfgValPosixMessageQueues_Action_1();
}

/** @} */
