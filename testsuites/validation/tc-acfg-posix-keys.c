/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValPosixKeys
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
#include <pthread.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup AcfgValPosixKeys spec:/acfg/val/posix-keys
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @brief Tests the maximum number of POSIX keys and key value pairs which the
 *   test suite configures.
 *
 * The test suite defines CONFIGURE_MAXIMUM_POSIX_KEYS and
 * CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS to eight.
 *
 * This test case performs the following actions:
 *
 * - Create as many keys as the test suite configures.
 *
 *   - Check that each key was created. This shows that the configured number
 *     of keys can be active at the same time.
 *
 * - Try to create one more key.
 *
 *   - Check that the create failed with EAGAIN, since all configured keys are
 *     active.
 *
 * - Associate a value with each key in the calling thread. The test suite
 *   configures as many key value pairs as keys, so that the associations use
 *   all configured key value pairs.
 *
 *   - Check that each value was associated. This shows that the configured
 *     number of key value pairs can be active at the same time.
 *
 * - Try to associate a value with a key in another thread.
 *
 *   - Check that the association failed with ENOMEM, since all configured key
 *     value pairs are active.
 *
 * @{
 */

#define MAX_KEYS 8

static pthread_key_t keys[ MAX_KEYS ];

static int key_status[ MAX_KEYS ];

static int value;

static rtems_id runner_id;

static rtems_id worker_id;

static int worker_status;

static void Worker( rtems_task_argument arg )
{
  (void) arg;
  worker_status = pthread_setspecific( keys[ 0 ], &value );
  SendEvents( runner_id, RTEMS_EVENT_0 );

  while ( true ) {
    (void) ReceiveAnyEvents();
  }
}

static void AcfgValPosixKeys_Setup( void )
{
  size_t i;

  for ( i = 0; i < MAX_KEYS; ++i ) {
    key_status[ i ] = -1;
  }

  worker_id = 0;
}

static void AcfgValPosixKeys_Setup_Wrap( void *arg )
{
  (void) arg;
  AcfgValPosixKeys_Setup();
}

static void AcfgValPosixKeys_Teardown( void )
{
  size_t i;

  if ( worker_id != 0 ) {
    DeleteTask( worker_id );
  }

  for ( i = 0; i < MAX_KEYS; ++i ) {
    if ( key_status[ i ] == 0 ) {
      T_eq_int( pthread_key_delete( keys[ i ] ), 0 );
    }
  }
}

static void AcfgValPosixKeys_Teardown_Wrap( void *arg )
{
  (void) arg;
  AcfgValPosixKeys_Teardown();
}

static T_fixture AcfgValPosixKeys_Fixture = {
  .setup = AcfgValPosixKeys_Setup_Wrap,
  .stop = NULL,
  .teardown = AcfgValPosixKeys_Teardown_Wrap,
  .scope = NULL,
  .initial_context = NULL
};

/**
 * @brief Create as many keys as the test suite configures.
 */
static void AcfgValPosixKeys_Action_0( void )
{
  size_t i;

  for ( i = 0; i < MAX_KEYS; ++i ) {
    key_status[ i ] = pthread_key_create( &keys[ i ], NULL );
  }

  /*
   * Check that each key was created. This shows that the configured number of
   * keys can be active at the same time.
   */
  for ( i = 0; i < MAX_KEYS; ++i ) {
    T_eq_int( key_status[ i ], 0 );
  }
}

/**
 * @brief Try to create one more key.
 */
static void AcfgValPosixKeys_Action_1( void )
{
  pthread_key_t other;
  int           eno;

  eno = pthread_key_create( &other, NULL );

  /*
   * Check that the create failed with EAGAIN, since all configured keys are
   * active.
   */
  T_eq_int( eno, EAGAIN );
}

/**
 * @brief Associate a value with each key in the calling thread. The test suite
 *   configures as many key value pairs as keys, so that the associations use
 *   all configured key value pairs.
 */
static void AcfgValPosixKeys_Action_2( void )
{
  int    eno[ MAX_KEYS ];
  size_t i;

  for ( i = 0; i < MAX_KEYS; ++i ) {
    eno[ i ] = pthread_setspecific( keys[ i ], &value );
  }

  /*
   * Check that each value was associated. This shows that the configured
   * number of key value pairs can be active at the same time.
   */
  for ( i = 0; i < MAX_KEYS; ++i ) {
    T_eq_int( eno[ i ], 0 );
  }
}

/**
 * @brief Try to associate a value with a key in another thread.
 */
static void AcfgValPosixKeys_Action_3( void )
{
  runner_id = rtems_task_self();
  worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( worker_id, Worker, 0 );
  (void) ReceiveAnyEvents();

  /*
   * Check that the association failed with ENOMEM, since all configured key
   * value pairs are active.
   */
  T_eq_int( worker_status, ENOMEM );
}

/**
 * @fn void T_case_body_AcfgValPosixKeys( void )
 */
T_TEST_CASE_FIXTURE( AcfgValPosixKeys, &AcfgValPosixKeys_Fixture )
{
  AcfgValPosixKeys_Action_0();
  AcfgValPosixKeys_Action_1();
  AcfgValPosixKeys_Action_2();
  AcfgValPosixKeys_Action_3();
}

/** @} */
