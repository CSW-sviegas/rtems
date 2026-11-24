/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplPOSIXMessageQueue
 *
 * @brief This header file provides the implementation interfaces of the
 *   POSIX message queues.
 *
 * This include file contains the static inline implementation of the private
 * inlined routines for POSIX Message Queue.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_POSIX_MQUEUE_INL
#define _RTEMS_POSIX_MQUEUE_INL

#include <rtems/posix/mqueue.h>
#include <rtems/posix/posixapi.h>
#include <rtems/score/coremsgimpl.h>
#include <rtems/score/threadqimpl.h>

#include <rtems/seterr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Delete a POSIX Message Queue
 *
 * This routine supports the mq_unlink and mq_close routines by
 * doing most of the work involved with removing a message queue.
 */
void _POSIX_Message_queue_Delete(
  POSIX_Message_queue_Control *the_mq,
  Thread_queue_Context        *queue_context
);

/*@
 *  @brief POSIX Message Queue Receive Support
 *
 *  This routine supports the various flavors of receiving a message.
 *
 *  @note The structure of the routines is identical to that of POSIX
 *        Message_queues to leave the option of having unnamed message
 *        queues at a future date.  They are currently not part of the
 *        POSIX standard but unnamed message_queues are.  This is also
 *        the reason for the apparently unnecessary tracking of
 *        the process_shared attribute.  [In addition to the fact that
 *        it would be trivial to add pshared to the mq_attr structure
 *        and have process private message queues.]
 *
 * @note This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open time.
 */
ssize_t _POSIX_Message_queue_Receive_support(
  mqd_t                        mqdes,
  char                        *msg_ptr,
  size_t                       msg_len,
  unsigned int                *msg_prio,
  const struct timespec       *abstime,
  Thread_queue_Enqueue_callout enqueue_callout
);

/**
 *  @brief POSIX Message Queue Send Support
 *
 *  This routine posts a message to a specified message queue.
 */
int _POSIX_Message_queue_Send_support(
  mqd_t                        mqdes,
  const char                  *msg_ptr,
  size_t                       msg_len,
  unsigned int                 msg_prio,
  const struct timespec       *abstime,
  Thread_queue_Enqueue_callout enqueue_callout
);

static inline POSIX_Message_queue_Control *
_POSIX_Message_queue_Allocate_unprotected( void )
{
  return (POSIX_Message_queue_Control *) _Objects_Allocate_unprotected(
    &_POSIX_Message_queue_Information
  );
}

/**
 *  @brief POSIX Message Queue Free
 *
 *  This routine frees a message queue control block to the
 *  inactive chain of free message queue control blocks.
 */
static inline void _POSIX_Message_queue_Free(
  POSIX_Message_queue_Control *the_mq
)
{
  _Objects_Free( &_POSIX_Message_queue_Information, &the_mq->Object );
}

static inline POSIX_Message_queue_Control *_POSIX_Message_queue_Get(
  Objects_Id            id,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Context_initialize( queue_context );
  return (POSIX_Message_queue_Control *) _Objects_Get(
    id,
    &queue_context->Lock_context.Lock_context,
    &_POSIX_Message_queue_Information
  );
}

/**
 * @brief Gets the message queue of the descriptor and acquires its lock.
 *
 * @param mqdes is the message queue descriptor.
 *
 * @param[out] queue_context is the thread queue context.  The lock of the
 *   message queue is acquired through this context if the function returns a
 *   message queue.
 *
 * @retval NULL The descriptor is invalid or the message queue is not open.
 *   The lock is not acquired and interrupts are enabled.
 *
 * @return Returns the message queue of the descriptor.
 */
static inline POSIX_Message_queue_Control *_POSIX_Message_queue_Get_open(
  mqd_t                 mqdes,
  Thread_queue_Context *queue_context
)
{
  POSIX_Message_queue_Control *the_mq;

  the_mq = _POSIX_Message_queue_Get( mqdes, queue_context );

  if ( the_mq == NULL ) {
    return NULL;
  }

  _CORE_message_queue_Acquire_critical( &the_mq->Message_queue, queue_context );

  if ( the_mq->open_count == 0 ) {
    _CORE_message_queue_Release( &the_mq->Message_queue, queue_context );
    return NULL;
  }

  return the_mq;
}

/**
 * @brief Converts the POSIX message priority to the submit type.
 *
 * The message queue handler inserts a message with a lower submit type in
 * front of a message with a higher submit type.
 *
 * @param priority is the POSIX message priority.  It is less than
 *   MQ_PRIO_MAX.
 *
 * @return Returns the submit type of the priority.
 */
static inline CORE_message_queue_Submit_types
_POSIX_Message_queue_Priority_to_core( unsigned int priority )
{
  return -(CORE_message_queue_Submit_types) priority;
}

/**
 * @brief Converts the submit type to the POSIX message priority.
 *
 * @param priority is the submit type of a message which
 *   _POSIX_Message_queue_Priority_to_core() produced.
 *
 * @return Returns the POSIX message priority of the submit type.
 */
static inline unsigned int _POSIX_Message_queue_Priority_from_core(
  CORE_message_queue_Submit_types priority
)
{
  return (unsigned int) -priority;
}

/**
 *  @brief POSIX Message Queue Remove from Namespace
 */
static inline void _POSIX_Message_queue_Namespace_remove(
  POSIX_Message_queue_Control *the_mq
)
{
  _Objects_Namespace_remove_string(
    &_POSIX_Message_queue_Information,
    &the_mq->Object
  );
}

static inline POSIX_Message_queue_Control *_POSIX_Message_queue_Get_by_name(
  const char                *name,
  size_t                    *name_length_p,
  Objects_Get_by_name_error *error
)
{
  return (POSIX_Message_queue_Control *) _Objects_Get_by_name(
    &_POSIX_Message_queue_Information,
    name,
    name_length_p,
    error
  );
}

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
