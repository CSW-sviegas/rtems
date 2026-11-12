/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRecordValRecord
 */

/*
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

#include <rtems.h>
#include <string.h>
#include <rtems/record.h>
#include <rtems/recorddump.h>
#include <rtems/sysinit.h>
#include <rtems/version.h>
#include <sys/endian.h>
#include <sys/time.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/timecounter.h>
#include <rtems/score/watchdogimpl.h>

#include "ts-record.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsRecordValRecord spec:/rtems/record/val/record
 *
 * @ingroup TestsuitesValidationRecord0
 *
 * @brief Tests the event recording.
 *
 * This test case performs the following actions:
 *
 * - Call rtems_record_produce() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_produce() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_produce() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_produce_2() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_produce_2() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_produce_2() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_produce_n() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_produce_n() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_produce_n() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_caller() twice in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_caller_3() twice in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_3() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_3() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_3() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_caller_4() twice in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_4() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_4() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_4() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_caller_arg() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_2() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_2() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_2() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_2() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_3() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_3() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_3() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_3() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_4() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_4() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_4() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_4() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_5() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_5() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_5() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_5() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_6() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_6() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_6() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_6() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_7() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_7() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_7() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_7() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_8() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_8() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_8() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_8() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_9() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_9() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_9() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_9() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_caller_arg_10() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_caller_arg_10() produced the
 *     expected items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_caller_arg_10() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_caller_arg_10() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_line() twice in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_line() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line() produced distinct
 *     event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line() have the time stamp
 *     read before the first item.
 *
 * - Call rtems_record_line_2() twice in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_2() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_2() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_2() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_3() twice in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_3() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_3() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_3() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg() twice in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_2() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_2() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_2() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_2() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_3() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_3() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_3() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_3() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_4() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_4() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_4() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_4() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_5() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_5() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_5() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_5() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_6() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_6() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_6() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_6() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_7() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_7() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_7() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_7() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_8() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_8() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_8() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_8() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_9() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_9() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_9() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_9() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_line_arg_10() twice in a helper function with the
 *   maskable interrupts disabled.
 *
 *   - Check that the call of rtems_record_line_arg_10() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the two invocations of rtems_record_line_arg_10() produced
 *     distinct event data for the line item.
 *
 *   - Check that the items produced by rtems_record_line_arg_10() have the
 *     time stamp read before the first item.
 *
 * - Call rtems_record_entry() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_1() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_1() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_1() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_2() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_2() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_2() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_3() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_3() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_3() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_4() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_4() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_4() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_5() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_5() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_5() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_6() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_6() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_6() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_7() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_7() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_7() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_8() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_8() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_8() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_9() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_9() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_9() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_entry_10() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_entry_10() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_entry_10() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit() have the time stamp
 *     read before the first item.
 *
 * - Call rtems_record_exit_1() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_1() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_1() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_2() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_2() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_2() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_3() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_3() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_3() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_4() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_4() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_4() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_5() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_5() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_5() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_6() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_6() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_6() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_7() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_7() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_7() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_8() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_8() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_8() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_9() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_9() produced the expected items
 *     in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_9() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_exit_10() once in a helper function with the maskable
 *   interrupts disabled.
 *
 *   - Check that the call of rtems_record_exit_10() produced the expected
 *     items in the record buffer of the processor.
 *
 *   - Check that the items produced by rtems_record_exit_10() have the time
 *     stamp read before the first item.
 *
 * - Call rtems_record_interrupt_disable() and rtems_record_interrupt_enable()
 *   in a helper function with the maskable interrupts enabled.
 *
 *   - Check that rtems_record_interrupt_disable() disabled the maskable
 *     interrupts and returned the previous interrupt level.
 *
 *   - Check that rtems_record_interrupt_disable() produced the expected item
 *     while the maskable interrupts were disabled.
 *
 *   - Check that rtems_record_interrupt_enable() restored the interrupt level.
 *
 *   - Check that rtems_record_interrupt_enable() produced the expected item
 *     directly after the item of rtems_record_interrupt_disable().
 *
 * - Prepare a record context with rtems_record_prepare() while the maskable
 *   interrupts are enabled. Add two items with rtems_record_add(). Fetch the
 *   items of the processor. Commit the record context with
 *   rtems_record_commit(). Fetch the items of the processor again.
 *
 *   - Check that rtems_record_prepare() disabled the maskable interrupts.
 *     Check that it initialized the record context with the record buffer of
 *     the current processor, the previous interrupt level, and the time stamp
 *     of the counter. This shows that rtems_record_context provided the
 *     storage of the record context.
 *
 *   - Check that the items which rtems_record_add() appended were invisible to
 *     rtems_record_fetch() before the commit.
 *
 *   - Check that rtems_record_commit() made the items visible to
 *     rtems_record_fetch() and restored the interrupt level.
 *
 *   - Check that rtems_record_add() appended the items with the time stamp of
 *     the record context, the event, and the event data.
 *
 * - Disable the maskable interrupts. Prepare a record context with
 *   rtems_record_prepare_critical() for the processor with the highest index.
 *   Add an item with rtems_record_add(). Fetch the items of the processor.
 *   Commit the record context with rtems_record_commit_critical(). Fetch the
 *   items of the processor again. Enable the maskable interrupts.
 *
 *   - Check that rtems_record_prepare_critical() initialized the record
 *     context with the record buffer of the processor and the time stamp of
 *     the counter.
 *
 *   - Check that the item which rtems_record_add() appended was invisible to
 *     rtems_record_fetch() before the commit.
 *
 *   - Check that rtems_record_commit_critical() made the item visible to
 *     rtems_record_fetch() and left the maskable interrupts disabled.
 *
 * - Get the record buffer of each configured processor. Disable the maskable
 *   interrupts. Produce three items more than the capacity of the record
 *   buffer of the current processor. Copy the record buffer. Enable the
 *   maskable interrupts.
 *
 *   - Check that each configured processor has a record buffer of its own with
 *     a capacity of CONFIGURE_RECORD_PER_PROCESSOR_ITEMS items.
 *
 *   - Check that the record buffer contains the last items in the order of
 *     their production. The first three items replaced the oldest items of the
 *     full record buffer.
 *
 * - Produce an item in the record buffer of the processor with index zero.
 *   Fill a record fetch control with a pattern and initialize it with
 *   rtems_record_fetch_initialize(). Fetch the items.
 *
 *   - Check that the call of rtems_record_fetch() fetched the item of the
 *     processor with index zero into the array of the record fetch control.
 *     This shows that rtems_record_fetch_control provided the storage of the
 *     record fetch control.
 *
 * - Get the item count for a fetch with
 *   rtems_record_get_item_count_for_fetch(). Fill the record buffer of the
 *   processor with index zero beyond its capacity. Fetch the items with this
 *   item count. Fill the record buffer again. Fetch the items with the item
 *   count minus one.
 *
 *   - Check that one fetch with the item count fetched all items of the record
 *     buffer. Check that one fetch with the item count minus one left items in
 *     the record buffer.
 *
 * - Produce more items in the record buffer of the current processor than one
 *   fetch of the dump holds. Call rtems_record_dump() with a handler which
 *   collects the output.
 *
 *   - Check that rtems_record_dump() called the handler with the argument.
 *
 *   - Check that the output starts with the stream header.
 *
 *   - Check the content of the stream header. Check that the
 *     RTEMS_RECORD_VERSION, the RTEMS_RECORD_PROCESSOR_MAXIMUM, the
 *     RTEMS_RECORD_PER_CPU_COUNT, and the RTEMS_RECORD_FREQUENCY items have a
 *     time stamp of zero and the specified event data. Check that each string
 *     takes exactly the items which its characters need and that zero bytes
 *     follow its last character only.
 *
 *   - Check that the thread names follow the stream header. The name of the
 *     runner has more than 31 characters, so the output holds its first 31
 *     characters in more than one item. Check that each name takes exactly the
 *     items which its characters need and that zero bytes follow its last
 *     character only.
 *
 *   - Check that the fetched items of all processors follow the thread names.
 *
 * - Produce more items in the record buffer of the current processor than one
 *   fetch of the dump holds. Call rtems_record_dump_base64() with a handler
 *   which collects the output.
 *
 *   - Check that rtems_record_dump_base64() output a newline character after
 *     each 76 characters.
 *
 *   - Check that rtems_record_dump_base64() called the handler with the
 *     argument and output the base64 encoding of the data of
 *     rtems_record_dump().
 *
 * - Get the text of each enumerator of rtems_record_event with
 *   rtems_record_event_text().
 *
 *   - Check that the text is the name of the enumerator without the
 *     `RTEMS_RECORD_` prefix.
 *
 * - Evaluate the constants of the event recording.
 *
 *   - Check the value of RTEMS_RECORD_EVENT_BITS.
 *
 *   - Check the value of RTEMS_RECORD_TIME_BITS.
 *
 *   - Check the value of RTEMS_RECORD_THE_VERSION.
 *
 *   - Check the value of RTEMS_RECORD_MAGIC.
 *
 *   - Check the value of RTEMS_RECORD_FORMAT_BE_32.
 *
 *   - Check the value of RTEMS_RECORD_FORMAT_BE_64.
 *
 *   - Check the value of RTEMS_RECORD_FORMAT_LE_32.
 *
 *   - Check the value of RTEMS_RECORD_FORMAT_LE_64.
 *
 *   - Check the value of RTEMS_RECORD_LAST.
 *
 *   - Check the value of RTEMS_RECORD_NO_TIME_LAST.
 *
 *   - Check the value of RTEMS_RECORD_EMPTY.
 *
 *   - Check the value of RTEMS_RECORD_CONNECT_ENTRY.
 *
 *   - Check the value of RTEMS_RECORD_USER_511.
 *
 * - Evaluate the macros of the event recording.
 *
 *   - Check that RTEMS_RECORD_TIME_EVENT() evaluated to the time event with
 *     the time stamp in the upper bits and the event in the lower bits.
 *
 *   - Check that RTEMS_RECORD_GET_TIME() evaluates to the time stamp of the
 *     time event.
 *
 *   - Check that RTEMS_RECORD_GET_EVENT() evaluates to the event of the time
 *     event.
 *
 *   - Check that RTEMS_RECORD_USER() evaluates to the user event of the index.
 *
 * - Evaluate the types of the event recording.
 *
 *   - Check that rtems_record_data is an unsigned integer type which can hold
 *     a value of the type `uintptr_t`.
 *
 *   - Check the members and the layout of rtems_record_item.
 *
 *   - Check the members and the layout of rtems_record_item_32.
 *
 *   - Check the members and the layout of rtems_record_item_64.
 *
 *   - Check that rtems_record_dump_chunk is a pointer to a function with the
 *     specified parameters which returns no value.
 *
 *   - Check that the enumerators of rtems_record_fetch_status have distinct
 *     values.
 *
 * - Get the items which the system produced during the initialization of the
 *   device drivers.
 *
 *   - Check that the system produced the uptime items with an uptime between
 *     the uptimes before and after the production.
 *
 * - Insert the uptime watchdogs again. Wait for two times the specified
 *   interval of the periodic uptime items plus two clock ticks. Remove the
 *   uptime watchdogs. Fetch the items of each processor.
 *
 *   - Check that each processor produced at least two uptime item pairs. Check
 *     that the uptimes of consecutive pairs differ by the specified interval.
 *
 * - Create and delete a task. Install an interrupt entry for a testable
 *   interrupt vector, raise the vector, and remove the entry. Count the
 *   RTEMS_RECORD_THREAD_CREATE and the RTEMS_RECORD_INTERRUPT_ENTRY items in
 *   the record buffers of all processors.
 *
 *   - Check that the system produced no RTEMS_RECORD_THREAD_CREATE item.
 *
 *   - Check that the system produced no RTEMS_RECORD_INTERRUPT_ENTRY item.
 *
 * @{
 */

#define FETCH_STORAGE_COUNT ( TEST_RECORD_PER_PROCESSOR_ITEMS + 2 )

typedef struct {
  rtems_counter_ticks before;
  rtems_counter_ticks after;
  size_t              first;
  size_t              count;
} Segment;

typedef struct {
  const uint8_t *data;
  size_t         size;
  size_t         pos;
} Stream;

/**
 * @brief Test context for spec:/rtems/record/val/record test case.
 */
typedef struct {
  /**
   * @brief This member contains the count of the RTEMS_RECORD_THREAD_CREATE
   *   items in the record buffers.
   */
  size_t thread_create_items;

  /**
   * @brief This member contains the count of the RTEMS_RECORD_INTERRUPT_ENTRY
   *   items in the record buffers.
   */
  size_t interrupt_entry_items;

  /**
   * @brief This member contains the interrupt level of the action.
   */
  rtems_interrupt_level level;

  /**
   * @brief This member references the record control of the producer.
   */
  Record_Control *control;

  /**
   * @brief This member contains the head index of the next snapshot.
   */
  unsigned int head;

  /**
   * @brief This member contains the counter value of the next snapshot.
   */
  rtems_counter_ticks last;

  /**
   * @brief This member contains the item segments of the snapshots.
   */
  Segment segments[ 2 ];

  /**
   * @brief This member contains the count of item segments.
   */
  size_t segment_count;

  /**
   * @brief This member contains the items of the snapshots.
   */
  rtems_record_item items[ 64 ];

  /**
   * @brief This member contains the count of items of the snapshots.
   */
  size_t item_count;

  /**
   * @brief This member is true, if the maskable interrupts were enabled before
   *   the call of rtems_record_interrupt_disable().
   */
  bool enabled_before;

  /**
   * @brief This member is true, if the maskable interrupts were enabled after
   *   the call of rtems_record_interrupt_disable().
   */
  bool enabled_after_disable;

  /**
   * @brief This member is true, if the maskable interrupts were enabled after
   *   the call of rtems_record_interrupt_enable().
   */
  bool enabled_after_enable;

  /**
   * @brief This member contains the interrupt level returned by
   *   rtems_record_interrupt_disable().
   */
  uint32_t record_level;

  /**
   * @brief This member contains the capacity of each record buffer.
   */
  unsigned int capacity[ CPU_MAXIMUM_PROCESSORS ];

  /**
   * @brief This member contains the record fetch control.
   */
  rtems_record_fetch_control fetch;

  /**
   * @brief This member provides the storage of the record fetch control.
   */
  rtems_record_item fetch_storage[ FETCH_STORAGE_COUNT ];

  /**
   * @brief This member contains the status of the last fetch.
   */
  rtems_record_fetch_status fetch_status;

  /**
   * @brief This member contains the expiration time of the uptime watchdog of
   *   each processor.
   */
  uint64_t expire[ CPU_MAXIMUM_PROCESSORS ];

  /**
   * @brief This member contains the index of the processor which produced the
   *   items of the dump.
   */
  uint32_t dump_cpu_index;

  /**
   * @brief This member contains the output of the dump.
   */
  uint8_t dump[ 8192 ];

  /**
   * @brief This member contains the size of the output of the dump.
   */
  size_t dump_size;

  /**
   * @brief This member contains the count of dump handler calls.
   */
  size_t dump_calls;

  /**
   * @brief This member is true, if each dump handler call had the context as
   *   argument.
   */
  bool dump_arg_ok;

  /**
   * @brief This member contains the output of the base64 dump.
   */
  char base64[ 16384 ];

  /**
   * @brief This member contains the size of the output of the base64 dump.
   */
  size_t base64_size;

  /**
   * @brief This member is true, if each base64 dump handler call had the
   *   context as argument.
   */
  bool base64_arg_ok;

  /**
   * @brief This member contains the stream of the dump output.
   */
  Stream stream;
} RtemsRecordValRecord_Context;

static RtemsRecordValRecord_Context RtemsRecordValRecord_Instance;

typedef RtemsRecordValRecord_Context Context;

#define DATA( i ) ( (rtems_record_data) ( 0x1000 + ( i ) ) )

#define LONG_THREAD_NAME "RecordLongNameWhichTheDumpTruncates"

#define TRUNCATED_THREAD_NAME "RecordLongNameWhichTheDumpTrunc"

#define DATA_RA ( (rtems_record_data) 1 )

#define DATA_LINE ( (rtems_record_data) 2 )

#define HELPER_SIZE 1024

#define DUMP_ITEM_COUNT 200

#define TIME_MASK ( ( UINT32_C( 1 ) << RTEMS_RECORD_TIME_BITS ) - 1 )

#define IS_TYPE( expr, type ) \
  __builtin_types_compatible_p( __typeof__( expr ), type )

static const rtems_record_item produce_n_items[] = {
  { RTEMS_RECORD_USER( 1 ), DATA( 0 ) },
  { RTEMS_RECORD_USER( 2 ), DATA( 1 ) },
  { RTEMS_RECORD_USER( 3 ), DATA( 2 ) }
};

static const rtems_record_item interrupt_disable_items[] = {
  { RTEMS_RECORD_ISR_DISABLE, DATA_LINE }
};

static const rtems_record_item interrupt_enable_items[] = {
  { RTEMS_RECORD_ISR_ENABLE, DATA_LINE }
};

static const char *const system_event_names[] = {
  "EMPTY",
  "VERSION",
  "ARCH",
  "BSP",
  "FREQUENCY",
  "MULTILIB",
  "PER_CPU_COUNT",
  "PER_CPU_DISCARD",
  "PER_CPU_OVERFLOW",
  "PROCESSOR",
  "PROCESSOR_MAXIMUM",
  "THREAD_ID",
  "THREAD_NAME",
  "TIME_ADJUSTMENT",
  "TOOLS",
  "UNRELIABLE_TIME",
  "VERSION_CONTROL_KEY",
  "ACCEPT_ENTRY",
  "ACCEPT_EXIT",
  "ADDRESS",
  "ALIGNED_ALLOC_ENTRY",
  "ALIGNED_ALLOC_EXIT",
  "ARG_0",
  "ARG_1",
  "ARG_2",
  "ARG_3",
  "ARG_4",
  "ARG_5",
  "ARG_6",
  "ARG_7",
  "ARG_8",
  "ARG_9",
  "BIND_ENTRY",
  "BIND_EXIT",
  "BUFFER",
  "CALLER",
  "CALLOC_ENTRY",
  "CALLOC_EXIT",
  "CHOWN_ENTRY",
  "CHOWN_EXIT",
  "CLOSE_ENTRY",
  "CLOSE_EXIT",
  "CONNECT_ENTRY",
  "CONNECT_EXIT",
  "ETHER_INPUT",
  "ETHER_OUTPUT",
  "ERRNO",
  "FATAL_CODE",
  "FATAL_SOURCE",
  "FCHMOD_ENTRY",
  "FCHMOD_EXIT",
  "FCNTL_ENTRY",
  "FCNTL_EXIT",
  "FDATASYNC_ENTRY",
  "FDATASYNC_EXIT",
  "FREE_ENTRY",
  "FREE_EXIT",
  "FSTAT_ENTRY",
  "FSTAT_EXIT",
  "FSYNC_ENTRY",
  "FSYNC_EXIT",
  "FTRUNCATE_ENTRY",
  "FTRUNCATE_EXIT",
  "FUNCTION_ENTRY",
  "FUNCTION_EXIT",
  "GETSOCKOPT_ENTRY",
  "GETSOCKOPT_EXIT",
  "HEAP_ALLOC",
  "HEAP_FREE",
  "HEAP_SIZE",
  "HEAP_USAGE",
  "INTERRUPT_ENTRY",
  "INTERRUPT_EXIT",
  "INTERRUPT_INSTALL",
  "INTERRUPT_REMOVE",
  "INTERRUPT_SERVER_ENTRY",
  "INTERRUPT_SERVER_EXIT",
  "INTERRUPT_SERVER_INSTALL",
  "INTERRUPT_SERVER_MOVE",
  "INTERRUPT_SERVER_REMOVE",
  "INTERRUPT_SERVER_TRIGGER",
  "IOCTL_ENTRY",
  "IOCTL_EXIT",
  "IP6_INPUT",
  "IP6_OUTPUT",
  "IP_INPUT",
  "IP_OUTPUT",
  "ISR_DISABLE",
  "ISR_ENABLE",
  "ISR_LOCK_ACQUIRE_ENTRY",
  "ISR_LOCK_ACQUIRE_EXIT",
  "ISR_LOCK_ADDRESS",
  "ISR_LOCK_DESTROY",
  "ISR_LOCK_INITIALIZE",
  "ISR_LOCK_NAME",
  "ISR_LOCK_RELEASE",
  "KEVENT_ENTRY",
  "KEVENT_EXIT",
  "KQUEUE_ENTRY",
  "KQUEUE_EXIT",
  "LENGTH",
  "LINE",
  "LINK_ENTRY",
  "LINK_EXIT",
  "LISTEN_ENTRY",
  "LISTEN_EXIT",
  "LSEEK_ENTRY",
  "LSEEK_EXIT",
  "MALLOC_ENTRY",
  "MALLOC_EXIT",
  "MEMORY",
  "MKNOD_ENTRY",
  "MKNOD_EXIT",
  "MMAP_ENTRY",
  "MMAP_EXIT",
  "MOUNT_ENTRY",
  "MOUNT_EXIT",
  "OPEN_ENTRY",
  "OPEN_EXIT",
  "PAGE_ALLOC",
  "PAGE_FREE",
  "POLL_ENTRY",
  "POLL_EXIT",
  "POSIX_MEMALIGN_ENTRY",
  "POSIX_MEMALIGN_EXIT",
  "READ_ENTRY",
  "READ_EXIT",
  "READLINK_ENTRY",
  "READLINK_EXIT",
  "READV_ENTRY",
  "READV_EXIT",
  "REALLOC_ENTRY",
  "REALLOC_EXIT",
  "RECV_ENTRY",
  "RECV_EXIT",
  "RECVFROM_ENTRY",
  "RECVFROM_EXIT",
  "RECVMSG_ENTRY",
  "RECVMSG_EXIT",
  "REGISTERS",
  "RENAME_ENTRY",
  "RENAME_EXIT",
  "RETURN_0",
  "RETURN_1",
  "RETURN_2",
  "RETURN_3",
  "RETURN_4",
  "RETURN_5",
  "RETURN_6",
  "RETURN_7",
  "RETURN_8",
  "RETURN_9",
  "RTEMS_BARRIER_CREATE",
  "RTEMS_BARRIER_DELETE",
  "RTEMS_BARRIER_RELEASE",
  "RTEMS_BARRIER_WAIT",
  "RTEMS_CALLOC_ENTRY",
  "RTEMS_CALLOC_EXIT",
  "RTEMS_EVENT_RECEIVE",
  "RTEMS_EVENT_SEND",
  "RTEMS_EVENT_SYSTEM_RECEIVE",
  "RTEMS_EVENT_SYSTEM_SEND",
  "RTEMS_MALLOC_ENTRY",
  "RTEMS_MALLOC_EXIT",
  "RTEMS_MESSAGE_QUEUE_BROADCAST",
  "RTEMS_MESSAGE_QUEUE_CREATE",
  "RTEMS_MESSAGE_QUEUE_DELETE",
  "RTEMS_MESSAGE_QUEUE_FLUSH",
  "RTEMS_MESSAGE_QUEUE_RECEIVE",
  "RTEMS_MESSAGE_QUEUE_SEND",
  "RTEMS_MESSAGE_QUEUE_URGENT",
  "RTEMS_PARTITION_CREATE",
  "RTEMS_PARTITION_DELETE",
  "RTEMS_PARTITION_GET_BUFFER",
  "RTEMS_PARTITION_RETURN_BUFFER",
  "RTEMS_RATE_MONOTONIC_CANCEL",
  "RTEMS_RATE_MONOTONIC_CREATE",
  "RTEMS_RATE_MONOTONIC_DELETE",
  "RTEMS_RATE_MONOTONIC_PERIOD",
  "RTEMS_SEMAPHORE_CREATE",
  "RTEMS_SEMAPHORE_DELETE",
  "RTEMS_SEMAPHORE_FLUSH",
  "RTEMS_SEMAPHORE_OBTAIN",
  "RTEMS_SEMAPHORE_RELEASE",
  "RTEMS_TIMER_CANCEL",
  "RTEMS_TIMER_CREATE",
  "RTEMS_TIMER_DELETE",
  "RTEMS_TIMER_FIRE_AFTER",
  "RTEMS_TIMER_FIRE_WHEN",
  "RTEMS_TIMER_RESET",
  "RTEMS_TIMER_SERVER_FIRE_AFTER",
  "RTEMS_TIMER_SERVER_FIRE_WHEN",
  "SBWAIT_ENTRY",
  "SBWAIT_EXIT",
  "SBWAKEUP_ENTRY",
  "SBWAKEUP_EXIT",
  "SCHEDULER_ADD_PROCESSOR",
  "SCHEDULER_ASK_FOR_HELP",
  "SCHEDULER_BLOCK",
  "SCHEDULER_CANCEL_JOB",
  "SCHEDULER_ID",
  "SCHEDULER_MAP_PRIORITY",
  "SCHEDULER_NAME",
  "SCHEDULER_PIN",
  "SCHEDULER_RECONSIDER_HELP_REQUEST",
  "SCHEDULER_RELEASE_JOB",
  "SCHEDULER_REMOVE_PROCESSOR",
  "SCHEDULER_SCHEDULE",
  "SCHEDULER_SET_AFFINITY",
  "SCHEDULER_TICK",
  "SCHEDULER_UNBLOCK",
  "SCHEDULER_UNMAP_PRIORITY",
  "SCHEDULER_UNPIN",
  "SCHEDULER_UPDATE_PRIORITY",
  "SCHEDULER_WITHDRAW_NODE",
  "SCHEDULER_YIELD",
  "SELECT_ENTRY",
  "SELECT_EXIT",
  "SEND_ENTRY",
  "SEND_EXIT",
  "SENDMSG_ENTRY",
  "SENDMSG_EXIT",
  "SENDTO_ENTRY",
  "SENDTO_EXIT",
  "SETSOCKOPT_ENTRY",
  "SETSOCKOPT_EXIT",
  "SHUTDOWN_ENTRY",
  "SHUTDOWN_EXIT",
  "SOABORT_ENTRY",
  "SOABORT_EXIT",
  "SOACCEPT_ENTRY",
  "SOACCEPT_EXIT",
  "SOALLOC_ENTRY",
  "SOALLOC_EXIT",
  "SOBINDAT_ENTRY",
  "SOBINDAT_EXIT",
  "SOBIND_ENTRY",
  "SOBIND_EXIT",
  "SOCKET_ENTRY",
  "SOCKET_EXIT",
  "SOCLOSE_ENTRY",
  "SOCLOSE_EXIT",
  "SOCONNECT2_ENTRY",
  "SOCONNECT2_EXIT",
  "SOCONNECTAT_ENTRY",
  "SOCONNECTAT_EXIT",
  "SOCREATE_ENTRY",
  "SOCREATE_EXIT",
  "SODEALLOC_ENTRY",
  "SODEALLOC_EXIT",
  "SODISCONNECT_ENTRY",
  "SODISCONNECT_EXIT",
  "SOFREE_ENTRY",
  "SOFREE_EXIT",
  "SOLISTEN_ENTRY",
  "SOLISTEN_EXIT",
  "SONEWCONN_ENTRY",
  "SONEWCONN_EXIT",
  "SORECEIVE_ENTRY",
  "SORECEIVE_EXIT",
  "SORFLUSH_ENTRY",
  "SORFLUSH_EXIT",
  "SOSEND_ENTRY",
  "SOSEND_EXIT",
  "SOSHUTDOWN_ENTRY",
  "SOSHUTDOWN_EXIT",
  "STATVFS_ENTRY",
  "STATVFS_EXIT",
  "SYMLINK_ENTRY",
  "SYMLINK_EXIT",
  "TCP_CLOSE",
  "TCP_INPUT",
  "TCP_OUTPUT",
  "THREAD_BEGIN",
  "THREAD_CONTINUE_ENTRY",
  "THREAD_CONTINUE_EXIT",
  "THREAD_CREATE",
  "THREAD_DELETE",
  "THREAD_DISPATCH_DISABLE",
  "THREAD_DISPATCH_ENABLE",
  "THREAD_EXIT",
  "THREAD_EXITTED",
  "THREAD_PRIO_CURRENT_HIGH",
  "THREAD_PRIO_CURRENT_LOW",
  "THREAD_PRIO_REAL_HIGH",
  "THREAD_PRIO_REAL_LOW",
  "THREAD_QUEUE_ADDRESS",
  "THREAD_QUEUE_DESTROY",
  "THREAD_QUEUE_ENQUEUE",
  "THREAD_QUEUE_ENQUEUE_STICKY",
  "THREAD_QUEUE_EXTRACT",
  "THREAD_QUEUE_ID",
  "THREAD_QUEUE_INITIALIZE",
  "THREAD_QUEUE_NAME",
  "THREAD_QUEUE_SURRENDER",
  "THREAD_QUEUE_SURRENDER_STICKY",
  "THREAD_RESOURCE_OBTAIN",
  "THREAD_RESOURCE_RELEASE",
  "THREAD_RESTART",
  "THREAD_STACK_CURRENT",
  "THREAD_STACK_SIZE",
  "THREAD_STACK_USAGE",
  "THREAD_START",
  "THREAD_STATE_CLEAR",
  "THREAD_STATE_SET",
  "THREAD_SWITCH_IN",
  "THREAD_SWITCH_OUT",
  "THREAD_TERMINATE",
  "THREAD_TIMER_INSERT_MONOTONIC",
  "THREAD_TIMER_INSERT_REALTIME",
  "THREAD_TIMER_INSERT_TICKS",
  "THREAD_TIMER_REMOVE",
  "UDP_INPUT",
  "UDP_OUTPUT",
  "UMA_ALLOC_PTR",
  "UMA_ALLOC_ZONE",
  "UMA_FREE_PTR",
  "UMA_FREE_ZONE",
  "UNLINK_ENTRY",
  "UNLINK_EXIT",
  "UNMOUNT_ENTRY",
  "UNMOUNT_EXIT",
  "UPTIME_HIGH",
  "UPTIME_LOW",
  "WATCHDOG_ADDRESS",
  "WATCHDOG_CPU",
  "WATCHDOG_INITIALIZE",
  "WATCHDOG_INSERT",
  "WATCHDOG_PREINITIALIZE",
  "WATCHDOG_REMOVE",
  "WATCHDOG_ROUTINE",
  "WATCHDOG_STATE",
  "WORKSPACE_ALLOC_ENTRY",
  "WORKSPACE_ALLOC_EXIT",
  "WORKSPACE_FREE_ENTY",
  "WORKSPACE_FREE_EXIT",
  "WORKSPACE_SIZE",
  "WORKSPACE_USAGE",
  "WRITE_ENTRY",
  "WRITE_EXIT",
  "WRITEV_ENTRY",
  "WRITEV_EXIT",
  "SYSTEM_342",
  "SYSTEM_343",
  "SYSTEM_344",
  "SYSTEM_345",
  "SYSTEM_346",
  "SYSTEM_347",
  "SYSTEM_348",
  "SYSTEM_349",
  "SYSTEM_350",
  "SYSTEM_351",
  "SYSTEM_352",
  "SYSTEM_353",
  "SYSTEM_354",
  "SYSTEM_355",
  "SYSTEM_356",
  "SYSTEM_357",
  "SYSTEM_358",
  "SYSTEM_359",
  "SYSTEM_360",
  "SYSTEM_361",
  "SYSTEM_362",
  "SYSTEM_363",
  "SYSTEM_364",
  "SYSTEM_365",
  "SYSTEM_366",
  "SYSTEM_367",
  "SYSTEM_368",
  "SYSTEM_369",
  "SYSTEM_370",
  "SYSTEM_371",
  "SYSTEM_372",
  "SYSTEM_373",
  "SYSTEM_374",
  "SYSTEM_375",
  "SYSTEM_376",
  "SYSTEM_377",
  "SYSTEM_378",
  "SYSTEM_379",
  "SYSTEM_380",
  "SYSTEM_381",
  "SYSTEM_382",
  "SYSTEM_383",
  "SYSTEM_384",
  "SYSTEM_385",
  "SYSTEM_386",
  "SYSTEM_387",
  "SYSTEM_388",
  "SYSTEM_389",
  "SYSTEM_390",
  "SYSTEM_391",
  "SYSTEM_392",
  "SYSTEM_393",
  "SYSTEM_394",
  "SYSTEM_395",
  "SYSTEM_396",
  "SYSTEM_397",
  "SYSTEM_398",
  "SYSTEM_399",
  "SYSTEM_400",
  "SYSTEM_401",
  "SYSTEM_402",
  "SYSTEM_403",
  "SYSTEM_404",
  "SYSTEM_405",
  "SYSTEM_406",
  "SYSTEM_407",
  "SYSTEM_408",
  "SYSTEM_409",
  "SYSTEM_410",
  "SYSTEM_411",
  "SYSTEM_412",
  "SYSTEM_413",
  "SYSTEM_414",
  "SYSTEM_415",
  "SYSTEM_416",
  "SYSTEM_417",
  "SYSTEM_418",
  "SYSTEM_419",
  "SYSTEM_420",
  "SYSTEM_421",
  "SYSTEM_422",
  "SYSTEM_423",
  "SYSTEM_424",
  "SYSTEM_425",
  "SYSTEM_426",
  "SYSTEM_427",
  "SYSTEM_428",
  "SYSTEM_429",
  "SYSTEM_430",
  "SYSTEM_431",
  "SYSTEM_432",
  "SYSTEM_433",
  "SYSTEM_434",
  "SYSTEM_435",
  "SYSTEM_436",
  "SYSTEM_437",
  "SYSTEM_438",
  "SYSTEM_439",
  "SYSTEM_440",
  "SYSTEM_441",
  "SYSTEM_442",
  "SYSTEM_443",
  "SYSTEM_444",
  "SYSTEM_445",
  "SYSTEM_446",
  "SYSTEM_447",
  "SYSTEM_448",
  "SYSTEM_449",
  "SYSTEM_450",
  "SYSTEM_451",
  "SYSTEM_452",
  "SYSTEM_453",
  "SYSTEM_454",
  "SYSTEM_455",
  "SYSTEM_456",
  "SYSTEM_457",
  "SYSTEM_458",
  "SYSTEM_459",
  "SYSTEM_460",
  "SYSTEM_461",
  "SYSTEM_462",
  "SYSTEM_463",
  "SYSTEM_464",
  "SYSTEM_465",
  "SYSTEM_466",
  "SYSTEM_467",
  "SYSTEM_468",
  "SYSTEM_469",
  "SYSTEM_470",
  "SYSTEM_471",
  "SYSTEM_472",
  "SYSTEM_473",
  "SYSTEM_474",
  "SYSTEM_475",
  "SYSTEM_476",
  "SYSTEM_477",
  "SYSTEM_478",
  "SYSTEM_479",
  "SYSTEM_480",
  "SYSTEM_481",
  "SYSTEM_482",
  "SYSTEM_483",
  "SYSTEM_484",
  "SYSTEM_485",
  "SYSTEM_486",
  "SYSTEM_487",
  "SYSTEM_488",
  "SYSTEM_489",
  "SYSTEM_490",
  "SYSTEM_491",
  "SYSTEM_492",
  "SYSTEM_493",
  "SYSTEM_494",
  "SYSTEM_495",
  "SYSTEM_496",
  "SYSTEM_497",
  "SYSTEM_498",
  "SYSTEM_499",
  "SYSTEM_500",
  "SYSTEM_501",
  "SYSTEM_502",
  "SYSTEM_503",
  "SYSTEM_504",
  "SYSTEM_505",
  "SYSTEM_506",
  "SYSTEM_507",
  "SYSTEM_508",
  "SYSTEM_509",
  "SYSTEM_510",
  "SYSTEM_511",
};

static void UserEventName( char *buf, int index )
{
  char digits[ 4 ];
  int  n;

  memcpy( buf, "USER_", 5 );
  buf += 5;
  n = 0;

  do {
    digits[ n ] = (char) ( '0' + index % 10 );
    ++n;
    index /= 10;
  } while ( index > 0 );

  while ( n > 0 ) {
    --n;
    *buf = digits[ n ];
    ++buf;
  }

  *buf = '\0';
}

static struct {
  sbintime_t        before;
  sbintime_t        after;
  Record_Control   *control;
  unsigned int      head;
  size_t            count;
  rtems_record_item items[ 4 ];
} uptime_init;

static void UptimeInitBefore( void )
{
  uptime_init.control = _Per_CPU_Get()->record;
  uptime_init.head = _Record_Head( uptime_init.control );
  uptime_init.before = _Timecounter_Sbinuptime();
}

RTEMS_SYSINIT_ITEM(
  UptimeInitBefore,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_6
);

static void UptimeInitAfter( void )
{
  Record_Control *control;
  unsigned int    head;

  uptime_init.after = _Timecounter_Sbinuptime();
  control = uptime_init.control;
  head = _Record_Head( control );

  while (
    uptime_init.head != head &&
    uptime_init.count < RTEMS_ARRAY_SIZE( uptime_init.items )
  ) {
    uptime_init.items[ uptime_init.count ] =
      control->Items[ _Record_Index( control, uptime_init.head ) ];
    ++uptime_init.count;
    ++uptime_init.head;
  }
}

RTEMS_SYSINIT_ITEM(
  UptimeInitAfter,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_4
);

static Record_Control *GetControl( uint32_t cpu_index )
{
  return _Per_CPU_Get_by_index( cpu_index )->record;
}

static void RemoveUptimeWatchdogs( Context *ctx )
{
  uint32_t cpu_index;

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    rtems_interrupt_level level;
    Watchdog_Control     *watchdog;

    watchdog = &GetControl( cpu_index )->Watchdog;
    rtems_interrupt_local_disable( level );
    _Watchdog_Per_CPU_remove_ticks( watchdog );
    ctx->expire[ cpu_index ] = watchdog->expire;
    rtems_interrupt_local_enable( level );
  }
}

static void InsertUptimeWatchdogs( Context *ctx )
{
  uint32_t cpu_index;

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    rtems_interrupt_level level;
    Per_CPU_Control      *cpu;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    rtems_interrupt_local_disable( level );
    (void) _Watchdog_Per_CPU_insert(
      &cpu->record->Watchdog,
      cpu,
      &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
      ctx->expire[ cpu_index ]
    );
    rtems_interrupt_local_enable( level );
  }
}

static void Begin( Context *ctx )
{
  rtems_interrupt_local_disable( ctx->level );
  ctx->control = _Per_CPU_Get()->record;
  ctx->head = _Record_Head( ctx->control );
  ctx->item_count = 0;
  ctx->segment_count = 0;
  ctx->last = rtems_counter_read();
}

static void End( Context *ctx )
{
  rtems_interrupt_local_enable( ctx->level );
}

static void SnapshotCount(
  Context            *ctx,
  rtems_counter_ticks now,
  size_t              count
)
{
  Segment *segment;

  if ( ctx->segment_count >= RTEMS_ARRAY_SIZE( ctx->segments ) ) {
    return;
  }

  segment = &ctx->segments[ ctx->segment_count ];
  ++ctx->segment_count;
  segment->before = ctx->last;
  segment->after = now;
  segment->first = ctx->item_count;
  segment->count = count;

  while ( count > 0 && ctx->item_count < RTEMS_ARRAY_SIZE( ctx->items ) ) {
    ctx->items[ ctx->item_count ] =
      ctx->control->Items[ _Record_Index( ctx->control, ctx->head ) ];
    ++ctx->item_count;
    ++ctx->head;
    --count;
  }

  ctx->last = rtems_counter_read();
}

static RTEMS_NO_INLINE void Snapshot( Context *ctx )
{
  rtems_counter_ticks now;

  now = rtems_counter_read();
  SnapshotCount( ctx, now, _Record_Head( ctx->control ) - ctx->head );
}

static void CheckTimeStamp(
  uint32_t            time_stamp,
  rtems_counter_ticks before,
  rtems_counter_ticks after
)
{
  uint32_t span;

  span = (uint32_t) ( after - before );
  T_lt_u32( span, UINT32_C( 1 ) << RTEMS_RECORD_TIME_BITS );
  T_le_u32( ( time_stamp - (uint32_t) before ) & TIME_MASK, span );
}

static void CheckTime( const Context *ctx, size_t index )
{
  const Segment *segment;
  uint32_t       time_stamp;
  size_t         i;

  T_lt_sz( index, ctx->segment_count );

  if ( index >= ctx->segment_count ) {
    return;
  }

  segment = &ctx->segments[ index ];
  T_gt_sz( segment->count, 0 );

  if ( segment->count == 0 ) {
    return;
  }

  time_stamp = RTEMS_RECORD_GET_TIME( ctx->items[ segment->first ].event );
  CheckTimeStamp( time_stamp, segment->before, segment->after );

  for ( i = 1; i < segment->count; ++i ) {
    T_eq_u32(
      RTEMS_RECORD_GET_TIME( ctx->items[ segment->first + i ].event ),
      time_stamp
    );
  }
}

static void CheckSegment(
  const Context           *ctx,
  size_t                   index,
  const rtems_record_item *expected,
  size_t                   count,
  const void              *ra,
  const void              *helper
)
{
  const Segment *segment;
  size_t         i;

  T_lt_sz( index, ctx->segment_count );

  if ( index >= ctx->segment_count ) {
    return;
  }

  segment = &ctx->segments[ index ];
  T_eq_sz( segment->count, count );

  if ( segment->count != count ) {
    return;
  }

  for ( i = 0; i < count; ++i ) {
    const rtems_record_item *item;

    item = &ctx->items[ segment->first + i ];
    T_eq_u32( RTEMS_RECORD_GET_EVENT( item->event ), expected[ i ].event );

    if ( expected[ i ].data == DATA_RA ) {
      T_eq_ulong( item->data, (rtems_record_data) ra );
    } else if ( expected[ i ].data == DATA_LINE ) {
      T_gt_uptr( (uintptr_t) item->data, (uintptr_t) helper );
      T_lt_uptr( (uintptr_t) item->data - (uintptr_t) helper, HELPER_SIZE );
    } else {
      T_eq_ulong( item->data, expected[ i ].data );
    }
  }
}

static void CheckLineDiffers( const Context *ctx, size_t index )
{
  T_eq_sz( ctx->segment_count, 2 );

  if ( ctx->segment_count != 2 ) {
    return;
  }

  T_ne_ulong(
    ctx->items[ ctx->segments[ 0 ].first + index ].data,
    ctx->items[ ctx->segments[ 1 ].first + index ].data
  );
}

static RTEMS_NO_INLINE void *InterruptDisableEnable( Context *ctx )
{
  uint32_t            level;
  rtems_counter_ticks now;

  ctx->item_count = 0;
  ctx->segment_count = 0;
  ctx->enabled_before = _ISR_Get_level() == 0;
  ctx->last = rtems_counter_read();
  level = rtems_record_interrupt_disable();
  now = rtems_counter_read();
  ctx->enabled_after_disable = _ISR_Get_level() == 0;
  ctx->record_level = level;
  ctx->control = _Per_CPU_Get()->record;
  ctx->head = _Record_Head( ctx->control ) - 1;
  SnapshotCount( ctx, now, 1 );
  rtems_record_interrupt_enable( level );
  now = rtems_counter_read();
  ctx->enabled_after_enable = _ISR_Get_level() == 0;
  SnapshotCount( ctx, now, 1 );
  return __builtin_return_address( 0 );
}

static size_t Fetch( Context *ctx, uint32_t cpu_index, size_t count )
{
  rtems_record_fetch_initialize( &ctx->fetch, ctx->fetch_storage, count );
#if defined( RTEMS_SMP )
  ctx->fetch.internal.cpu_index = cpu_index;
#else
  (void) cpu_index;
#endif
  ctx->fetch_status = rtems_record_fetch( &ctx->fetch );

  if ( ctx->fetch.fetched_items == NULL || ctx->fetch.fetched_count == 0 ) {
    return 0;
  }

  return ctx->fetch.fetched_count - 1;
}

static void Fill( uint32_t cpu_index )
{
  rtems_interrupt_level  level;
  const Per_CPU_Control *cpu;
  unsigned int           i;

  cpu = _Per_CPU_Get_by_index( cpu_index );
  rtems_interrupt_local_disable( level );

  for ( i = 0; i < cpu->record->mask + 4; ++i ) {
    rtems_record_context context;

    rtems_record_prepare_critical( &context, cpu );
    rtems_record_add( &context, RTEMS_RECORD_USER( 10 ), DATA( i ) );
    rtems_record_commit_critical( &context );
  }

  rtems_interrupt_local_enable( level );
}

static void ProduceDumpItems( Context *ctx )
{
  rtems_interrupt_level level;
  uint32_t              cpu_index;
  size_t                i;

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    Record_Control *control;

    control = GetControl( cpu_index );
    control->tail = _Record_Head( control );
  }

  rtems_interrupt_local_disable( level );
  ctx->dump_cpu_index = rtems_scheduler_get_processor();

  for ( i = 0; i < DUMP_ITEM_COUNT; ++i ) {
    rtems_record_produce( RTEMS_RECORD_USER( 1 ), DATA( i ) );
  }

  rtems_interrupt_local_enable( level );
}

static void DumpChunk( void *arg, const void *data, size_t length )
{
  Context *ctx;
  size_t   n;

  ctx = arg;
  ++ctx->dump_calls;

  if ( ctx != T_fixture_context() ) {
    ctx->dump_arg_ok = false;
  }

  n = sizeof( ctx->dump ) - ctx->dump_size;

  if ( length > n ) {
    length = n;
  }

  memcpy( &ctx->dump[ ctx->dump_size ], data, length );
  ctx->dump_size += length;
}

static void Base64PutChar( int c, void *arg )
{
  Context *ctx;

  ctx = arg;

  if ( ctx != T_fixture_context() ) {
    ctx->base64_arg_ok = false;
  }

  if ( ctx->base64_size < sizeof( ctx->base64 ) ) {
    ctx->base64[ ctx->base64_size ] = (char) c;
    ++ctx->base64_size;
  }
}

static int Base64Value( char c )
{
  if ( c >= 'A' && c <= 'Z' ) {
    return c - 'A';
  }

  if ( c >= 'a' && c <= 'z' ) {
    return c - 'a' + 26;
  }

  if ( c >= '0' && c <= '9' ) {
    return c - '0' + 52;
  }

  if ( c == '+' ) {
    return 62;
  }

  if ( c == '/' ) {
    return 63;
  }

  return -1;
}

static size_t Base64Decode(
  const char *in,
  size_t      in_size,
  uint8_t    *out,
  size_t      out_size
)
{
  uint32_t value;
  int      bits;
  size_t   i;
  size_t   n;

  value = 0;
  bits = 0;
  n = 0;

  for ( i = 0; i < in_size; ++i ) {
    int v;

    if ( in[ i ] == '\n' ) {
      continue;
    }

    if ( in[ i ] == '=' ) {
      break;
    }

    v = Base64Value( in[ i ] );

    if ( v < 0 ) {
      return SIZE_MAX;
    }

    value = ( value << 6 ) | (uint32_t) v;
    bits += 6;

    if ( bits >= 8 ) {
      bits -= 8;

      if ( n >= out_size ) {
        return SIZE_MAX;
      }

      out[ n ] = (uint8_t) ( value >> bits );
      ++n;
    }
  }

  return n;
}

static void StreamInit( Stream *stream, const uint8_t *data, size_t size )
{
  stream->data = data;
  stream->size = size;
  stream->pos = 0;
}

static bool StreamPeek( const Stream *stream, rtems_record_item *item )
{
  if ( stream->pos + sizeof( *item ) > stream->size ) {
    return false;
  }

  memcpy( item, &stream->data[ stream->pos ], sizeof( *item ) );
  return true;
}

static bool StreamNext( Stream *stream, rtems_record_item *item )
{
  if ( !StreamPeek( stream, item ) ) {
    return false;
  }

  stream->pos += sizeof( *item );
  return true;
}

static void CheckNext( Stream *stream, uint32_t event, rtems_record_data data )
{
  rtems_record_item item;

  memset( &item, 0, sizeof( item ) );
  T_true( StreamNext( stream, &item ) );
  T_eq_u32( item.event, event );
  T_eq_ulong( item.data, data );
}

static void CheckFormatAndMagic( Stream *stream )
{
  uint32_t format;
  uint32_t magic;

  T_ge_sz( stream->size, 2 * sizeof( uint32_t ) );

  if ( stream->size < 2 * sizeof( uint32_t ) ) {
    return;
  }

  memcpy( &format, &stream->data[ 0 ], sizeof( format ) );
  memcpy( &magic, &stream->data[ sizeof( format ) ], sizeof( magic ) );
  stream->pos = 2 * sizeof( uint32_t );

#if BYTE_ORDER == LITTLE_ENDIAN
#if __INTPTR_WIDTH__ == 32
  T_eq_u32( format, RTEMS_RECORD_FORMAT_LE_32 );
#else
  T_eq_u32( format, RTEMS_RECORD_FORMAT_LE_64 );
#endif
#else
#if __INTPTR_WIDTH__ == 32
  T_eq_u32( format, RTEMS_RECORD_FORMAT_BE_32 );
#else
  T_eq_u32( format, RTEMS_RECORD_FORMAT_BE_64 );
#endif
#endif
  T_eq_u32( magic, RTEMS_RECORD_MAGIC );
}

static size_t CheckString(
  Stream            *stream,
  rtems_record_event event,
  const char        *str
)
{
  char              buf[ 256 ];
  size_t            n;
  size_t            count;
  bool              zero;
  bool              order_ok;
  rtems_record_item item;

  n = 0;
  count = 0;
  zero = false;
  order_ok = true;

  while ( StreamPeek( stream, &item ) && item.event == event ) {
    size_t i;

    for ( i = 0; i < sizeof( item.data ); ++i ) {
      char c;

      c = (char) ( item.data >> ( 8 * i ) );

      if ( c == '\0' ) {
        zero = true;
      } else {
        if ( zero ) {
          order_ok = false;
        }

        if ( n < sizeof( buf ) - 1 ) {
          buf[ n ] = c;
          ++n;
        }
      }
    }

    stream->pos += sizeof( item );
    ++count;
  }

  buf[ n ] = '\0';
  T_eq_str( buf, str );
  T_true( order_ok );
  T_eq_sz(
    count,
    ( strlen( str ) + sizeof( item.data ) - 1 ) / sizeof( item.data )
  );
  return count;
}

static void CheckHeaderItems( Stream *stream )
{
  CheckNext( stream, RTEMS_RECORD_VERSION, RTEMS_RECORD_THE_VERSION );
  CheckNext(
    stream,
    RTEMS_RECORD_PROCESSOR_MAXIMUM,
    rtems_scheduler_get_processor_maximum() - 1
  );
  CheckNext(
    stream,
    RTEMS_RECORD_PER_CPU_COUNT,
    TEST_RECORD_PER_PROCESSOR_ITEMS
  );
  CheckNext( stream, RTEMS_RECORD_FREQUENCY, rtems_counter_frequency() );
  CheckString( stream, RTEMS_RECORD_ARCH, CPU_NAME );
  CheckString( stream, RTEMS_RECORD_MULTILIB, CPU_MODEL_NAME );
  CheckString( stream, RTEMS_RECORD_BSP, rtems_board_support_package() );
  CheckString(
    stream,
    RTEMS_RECORD_VERSION_CONTROL_KEY,
    rtems_version_control_key()
  );
  CheckString( stream, RTEMS_RECORD_TOOLS, __VERSION__ );
}

static bool ThreadNameVisitor( rtems_tcb *tcb, void *arg )
{
  Stream *stream;
  char    name[ 2 * THREAD_DEFAULT_MAXIMUM_NAME_SIZE ];
  size_t  n;
  size_t  count;

  stream = arg;
  CheckNext( stream, RTEMS_RECORD_THREAD_ID, tcb->Object.id );
  n = _Thread_Get_name( tcb, name, sizeof( name ) );

  if ( n >= sizeof( name ) ) {
    n = sizeof( name ) - 1;
  }

  name[ n ] = '\0';
  count = CheckString( stream, RTEMS_RECORD_THREAD_NAME, name );

  if ( tcb == _Thread_Get_executing() ) {
    T_eq_str( name, TRUNCATED_THREAD_NAME );
    T_gt_sz( count, 1 );
  }

  return false;
}

static void CheckThreadNames( Stream *stream )
{
  _Thread_Iterate( ThreadNameVisitor, stream );
}

static void CheckDumpItems( const Context *ctx, Stream *stream )
{
  uint32_t          cpu_index;
  rtems_record_item item;

  memset( &item, 0, sizeof( item ) );

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    CheckNext( stream, RTEMS_RECORD_PROCESSOR, cpu_index );

    if ( cpu_index == ctx->dump_cpu_index ) {
      size_t i;

      i = 0;

      while ( i < DUMP_ITEM_COUNT ) {
        bool next;

        next = StreamNext( stream, &item );
        T_true( next );

        if ( !next ) {
          break;
        }

        if ( item.event == RTEMS_RECORD_PROCESSOR ) {
          T_eq_ulong( item.data, cpu_index );
        } else {
          T_eq_u32(
            RTEMS_RECORD_GET_EVENT( item.event ),
            RTEMS_RECORD_USER( 1 )
          );
          T_eq_ulong( item.data, DATA( i ) );
          ++i;
        }
      }
    }
  }

  T_eq_sz( stream->pos, stream->size );
}

static void CheckUptimePair(
  const rtems_record_item *items,
  sbintime_t               before,
  sbintime_t               after
)
{
  sbintime_t uptime;

  T_eq_u32(
    RTEMS_RECORD_GET_EVENT( items[ 0 ].event ),
    RTEMS_RECORD_UPTIME_LOW
  );
  T_eq_u32(
    RTEMS_RECORD_GET_EVENT( items[ 1 ].event ),
    RTEMS_RECORD_UPTIME_HIGH
  );
  uptime = (sbintime_t) ( ( (uint64_t) items[ 1 ].data << 32 ) |
                          (uint32_t) items[ 0 ].data );
  T_ge_ull( (unsigned long long) uptime, (unsigned long long) before );
  T_le_ull( (unsigned long long) uptime, (unsigned long long) after );
}

/*
 * Get the largest count of clock ticks which lasts at most two to the power of
 * RTEMS_RECORD_TIME_BITS counter ticks, and at least one clock tick.
 */
static void ClearRecordBuffers( void )
{
  uint32_t cpu_index;

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    Record_Control *control;

    control = GetControl( cpu_index );
    control->tail = _Record_Head( control );
  }
}

static size_t CountItems( rtems_record_event event )
{
  uint32_t cpu_index;
  size_t   count;

  count = 0;

  for (
    cpu_index = 0; cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    Record_Control *control;
    unsigned int    head;
    unsigned int    tail;

    control = GetControl( cpu_index );
    head = _Record_Head( control );
    tail = control->tail;

    if ( head - tail > control->mask + 1 ) {
      tail = head - ( control->mask + 1 );
    }

    while ( tail != head ) {
      const rtems_record_item *item;

      item = &control->Items[ _Record_Index( control, tail ) ];

      if ( RTEMS_RECORD_GET_EVENT( item->event ) == event ) {
        ++count;
      }

      ++tail;
    }
  }

  return count;
}

static void ClearInterrupt( void *arg )
{
  const rtems_vector_number *vector;

  vector = arg;
  (void) ClearSoftwareInterrupt( *vector );
}

static rtems_interval UptimeInterval( void )
{
  uint64_t       limit;
  uint64_t       frequency;
  rtems_interval interval;

  /*
   * The interval is the largest count of clock ticks which lasts at most
   * the limit of counter ticks, and at least one clock tick.
   */
  limit = UINT64_C( 1 ) << RTEMS_RECORD_TIME_BITS;
  limit *= rtems_clock_get_ticks_per_second();
  frequency = rtems_counter_frequency();
  interval = 1;

  while ( ( (uint64_t) interval + 1 ) * frequency <= limit ) {
    ++interval;
  }

  return interval;
}

static rtems_interval ClockTicks( sbintime_t duration )
{
  sbintime_t tick;

  tick = SBT_1S / rtems_clock_get_ticks_per_second();

  return (rtems_interval) ( ( duration + tick / 2 ) / tick );
}

static void CheckPeriodicUptime(
  Context   *ctx,
  uint32_t   cpu_index,
  sbintime_t before,
  sbintime_t after
)
{
  size_t     count;
  size_t     i;
  size_t     pairs;
  sbintime_t previous;

  count = Fetch( ctx, cpu_index, FETCH_STORAGE_COUNT );
  T_eq_sz( count % 2, 0 );
  pairs = 0;
  previous = 0;

  for ( i = 0; i + 1 < count; i += 2 ) {
    const rtems_record_item *items;
    sbintime_t               uptime;

    items = &ctx->fetch.fetched_items[ 1 + i ];
    CheckUptimePair( items, before, after );
    uptime = (sbintime_t) ( ( (uint64_t) items[ 1 ].data << 32 ) |
                            (uint32_t) items[ 0 ].data );

    if ( pairs > 0 ) {
      T_eq_u32( ClockTicks( uptime - previous ), UptimeInterval() );
    }

    previous = uptime;
    ++pairs;
  }

  T_ge_sz( pairs, 2 );
}

static RTEMS_NO_INLINE void *Produce( Context *ctx )
{
  rtems_record_produce( RTEMS_RECORD_USER( 1 ), DATA( 0 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Produce2( Context *ctx )
{
  rtems_record_produce_2(
    RTEMS_RECORD_USER( 1 ),
    DATA( 0 ),
    RTEMS_RECORD_USER( 2 ),
    DATA( 1 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *ProduceN( Context *ctx )
{
  rtems_record_produce_n(
    produce_n_items,
    RTEMS_ARRAY_SIZE( produce_n_items )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Caller( Context *ctx )
{
  rtems_record_caller();
  Snapshot( ctx );
  rtems_record_caller();
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Caller3( Context *ctx )
{
  rtems_record_caller_3( RTEMS_RECORD_USER( 1 ), DATA( 0 ) );
  Snapshot( ctx );
  rtems_record_caller_3( RTEMS_RECORD_USER( 1 ), DATA( 0 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Caller4( Context *ctx )
{
  rtems_record_caller_4(
    RTEMS_RECORD_USER( 1 ),
    DATA( 0 ),
    RTEMS_RECORD_USER( 2 ),
    DATA( 1 )
  );
  Snapshot( ctx );
  rtems_record_caller_4(
    RTEMS_RECORD_USER( 1 ),
    DATA( 0 ),
    RTEMS_RECORD_USER( 2 ),
    DATA( 1 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg( Context *ctx )
{
  rtems_record_caller_arg( DATA( 0 ) );
  Snapshot( ctx );
  rtems_record_caller_arg( DATA( 0 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg2( Context *ctx )
{
  rtems_record_caller_arg_2( DATA( 0 ), DATA( 1 ) );
  Snapshot( ctx );
  rtems_record_caller_arg_2( DATA( 0 ), DATA( 1 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg3( Context *ctx )
{
  rtems_record_caller_arg_3( DATA( 0 ), DATA( 1 ), DATA( 2 ) );
  Snapshot( ctx );
  rtems_record_caller_arg_3( DATA( 0 ), DATA( 1 ), DATA( 2 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg4( Context *ctx )
{
  rtems_record_caller_arg_4( DATA( 0 ), DATA( 1 ), DATA( 2 ), DATA( 3 ) );
  Snapshot( ctx );
  rtems_record_caller_arg_4( DATA( 0 ), DATA( 1 ), DATA( 2 ), DATA( 3 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg5( Context *ctx )
{
  rtems_record_caller_arg_5(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 )
  );
  Snapshot( ctx );
  rtems_record_caller_arg_5(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg6( Context *ctx )
{
  rtems_record_caller_arg_6(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 )
  );
  Snapshot( ctx );
  rtems_record_caller_arg_6(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg7( Context *ctx )
{
  rtems_record_caller_arg_7(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 )
  );
  Snapshot( ctx );
  rtems_record_caller_arg_7(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg8( Context *ctx )
{
  rtems_record_caller_arg_8(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 )
  );
  Snapshot( ctx );
  rtems_record_caller_arg_8(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg9( Context *ctx )
{
  rtems_record_caller_arg_9(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 )
  );
  Snapshot( ctx );
  rtems_record_caller_arg_9(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *CallerArg10( Context *ctx )
{
  rtems_record_caller_arg_10(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 ),
    DATA( 9 )
  );
  Snapshot( ctx );
  rtems_record_caller_arg_10(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 ),
    DATA( 9 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Line( Context *ctx )
{
  rtems_record_line();
  Snapshot( ctx );
  rtems_record_line();
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Line2( Context *ctx )
{
  rtems_record_line_2( RTEMS_RECORD_USER( 1 ), DATA( 0 ) );
  Snapshot( ctx );
  rtems_record_line_2( RTEMS_RECORD_USER( 1 ), DATA( 0 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Line3( Context *ctx )
{
  rtems_record_line_3(
    RTEMS_RECORD_USER( 1 ),
    DATA( 0 ),
    RTEMS_RECORD_USER( 2 ),
    DATA( 1 )
  );
  Snapshot( ctx );
  rtems_record_line_3(
    RTEMS_RECORD_USER( 1 ),
    DATA( 0 ),
    RTEMS_RECORD_USER( 2 ),
    DATA( 1 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg( Context *ctx )
{
  rtems_record_line_arg( DATA( 0 ) );
  Snapshot( ctx );
  rtems_record_line_arg( DATA( 0 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg2( Context *ctx )
{
  rtems_record_line_arg_2( DATA( 0 ), DATA( 1 ) );
  Snapshot( ctx );
  rtems_record_line_arg_2( DATA( 0 ), DATA( 1 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg3( Context *ctx )
{
  rtems_record_line_arg_3( DATA( 0 ), DATA( 1 ), DATA( 2 ) );
  Snapshot( ctx );
  rtems_record_line_arg_3( DATA( 0 ), DATA( 1 ), DATA( 2 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg4( Context *ctx )
{
  rtems_record_line_arg_4( DATA( 0 ), DATA( 1 ), DATA( 2 ), DATA( 3 ) );
  Snapshot( ctx );
  rtems_record_line_arg_4( DATA( 0 ), DATA( 1 ), DATA( 2 ), DATA( 3 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg5( Context *ctx )
{
  rtems_record_line_arg_5(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 )
  );
  Snapshot( ctx );
  rtems_record_line_arg_5(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg6( Context *ctx )
{
  rtems_record_line_arg_6(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 )
  );
  Snapshot( ctx );
  rtems_record_line_arg_6(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg7( Context *ctx )
{
  rtems_record_line_arg_7(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 )
  );
  Snapshot( ctx );
  rtems_record_line_arg_7(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg8( Context *ctx )
{
  rtems_record_line_arg_8(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 )
  );
  Snapshot( ctx );
  rtems_record_line_arg_8(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg9( Context *ctx )
{
  rtems_record_line_arg_9(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 )
  );
  Snapshot( ctx );
  rtems_record_line_arg_9(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *LineArg10( Context *ctx )
{
  rtems_record_line_arg_10(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 ),
    DATA( 9 )
  );
  Snapshot( ctx );
  rtems_record_line_arg_10(
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 ),
    DATA( 9 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry( Context *ctx )
{
  rtems_record_entry( RTEMS_RECORD_USER( 3 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry1( Context *ctx )
{
  rtems_record_entry_1( RTEMS_RECORD_USER( 3 ), DATA( 0 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry2( Context *ctx )
{
  rtems_record_entry_2( RTEMS_RECORD_USER( 3 ), DATA( 0 ), DATA( 1 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry3( Context *ctx )
{
  rtems_record_entry_3(
    RTEMS_RECORD_USER( 3 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry4( Context *ctx )
{
  rtems_record_entry_4(
    RTEMS_RECORD_USER( 3 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry5( Context *ctx )
{
  rtems_record_entry_5(
    RTEMS_RECORD_USER( 3 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry6( Context *ctx )
{
  rtems_record_entry_6(
    RTEMS_RECORD_USER( 3 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry7( Context *ctx )
{
  rtems_record_entry_7(
    RTEMS_RECORD_USER( 3 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry8( Context *ctx )
{
  rtems_record_entry_8(
    RTEMS_RECORD_USER( 3 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry9( Context *ctx )
{
  rtems_record_entry_9(
    RTEMS_RECORD_USER( 3 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Entry10( Context *ctx )
{
  rtems_record_entry_10(
    RTEMS_RECORD_USER( 3 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 ),
    DATA( 9 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit( Context *ctx )
{
  rtems_record_exit( RTEMS_RECORD_USER( 4 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit1( Context *ctx )
{
  rtems_record_exit_1( RTEMS_RECORD_USER( 4 ), DATA( 0 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit2( Context *ctx )
{
  rtems_record_exit_2( RTEMS_RECORD_USER( 4 ), DATA( 0 ), DATA( 1 ) );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit3( Context *ctx )
{
  rtems_record_exit_3(
    RTEMS_RECORD_USER( 4 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit4( Context *ctx )
{
  rtems_record_exit_4(
    RTEMS_RECORD_USER( 4 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit5( Context *ctx )
{
  rtems_record_exit_5(
    RTEMS_RECORD_USER( 4 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit6( Context *ctx )
{
  rtems_record_exit_6(
    RTEMS_RECORD_USER( 4 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit7( Context *ctx )
{
  rtems_record_exit_7(
    RTEMS_RECORD_USER( 4 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit8( Context *ctx )
{
  rtems_record_exit_8(
    RTEMS_RECORD_USER( 4 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit9( Context *ctx )
{
  rtems_record_exit_9(
    RTEMS_RECORD_USER( 4 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static RTEMS_NO_INLINE void *Exit10( Context *ctx )
{
  rtems_record_exit_10(
    RTEMS_RECORD_USER( 4 ),
    DATA( 0 ),
    DATA( 1 ),
    DATA( 2 ),
    DATA( 3 ),
    DATA( 4 ),
    DATA( 5 ),
    DATA( 6 ),
    DATA( 7 ),
    DATA( 8 ),
    DATA( 9 )
  );
  Snapshot( ctx );
  return __builtin_return_address( 0 );
}

static void RtemsRecordValRecord_Setup( RtemsRecordValRecord_Context *ctx )
{
  RemoveUptimeWatchdogs( ctx );
  (void) _Thread_Set_name( _Thread_Get_executing(), LONG_THREAD_NAME );
}

static void RtemsRecordValRecord_Setup_Wrap( void *arg )
{
  RtemsRecordValRecord_Context *ctx;

  ctx = arg;
  RtemsRecordValRecord_Setup( ctx );
}

static void RtemsRecordValRecord_Teardown( RtemsRecordValRecord_Context *ctx )
{
  (void) _Thread_Set_name( _Thread_Get_executing(), "" );
  InsertUptimeWatchdogs( ctx );
}

static void RtemsRecordValRecord_Teardown_Wrap( void *arg )
{
  RtemsRecordValRecord_Context *ctx;

  ctx = arg;
  RtemsRecordValRecord_Teardown( ctx );
}

static T_fixture RtemsRecordValRecord_Fixture = {
  .setup = RtemsRecordValRecord_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRecordValRecord_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsRecordValRecord_Instance
};

/**
 * @brief Call rtems_record_produce() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_0( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 1 ), DATA( 0 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Produce( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_produce() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Produce );

  /*
   * Check that the items produced by rtems_record_produce() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_produce_2() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_1( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 1 ), DATA( 0 ) },
    { RTEMS_RECORD_USER( 2 ), DATA( 1 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Produce2( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_produce_2() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Produce2 );

  /*
   * Check that the items produced by rtems_record_produce_2() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_produce_n() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_2( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 1 ), DATA( 0 ) },
    { RTEMS_RECORD_USER( 2 ), DATA( 1 ) },
    { RTEMS_RECORD_USER( 3 ), DATA( 2 ) },
  };
  void *ra;

  Begin( ctx );
  ra = ProduceN( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_produce_n() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, ProduceN );

  /*
   * Check that the items produced by rtems_record_produce_n() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_caller() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_3( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
  };
  void *ra;

  Begin( ctx );
  ra = Caller( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Caller );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, Caller );

  /*
   * Check that the two invocations of rtems_record_caller() produced distinct
   * event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_3() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_4( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_USER( 1 ), DATA( 0 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Caller3( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_3() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Caller3 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, Caller3 );

  /*
   * Check that the two invocations of rtems_record_caller_3() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_3() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_4() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_5( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_USER( 1 ), DATA( 0 ) },
    { RTEMS_RECORD_USER( 2 ), DATA( 1 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Caller4( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_4() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Caller4 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, Caller4 );

  /*
   * Check that the two invocations of rtems_record_caller_4() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_4() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_6( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_2() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_7( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg2( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_2() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg2
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg2
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_2() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_2() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_3() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_8( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg3( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_3() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg3
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg3
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_3() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_3() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_4() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_9( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg4( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_4() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg4
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg4
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_4() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_4() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_5() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_10( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg5( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_5() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg5
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg5
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_5() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_5() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_6() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_11( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg6( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_6() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg6
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg6
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_6() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_6() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_7() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_12( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg7( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_7() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg7
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg7
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_7() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_7() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_8() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_13( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg8( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_8() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg8
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg8
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_8() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_8() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_9() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_14( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
    { RTEMS_RECORD_ARG_8, DATA( 8 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg9( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_9() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg9
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg9
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_9() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_9() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_caller_arg_10() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_15( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_CALLER, DATA_RA },
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
    { RTEMS_RECORD_ARG_8, DATA( 8 ) },
    { RTEMS_RECORD_ARG_9, DATA( 9 ) },
  };
  void *ra;

  Begin( ctx );
  ra = CallerArg10( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_caller_arg_10() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg10
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    CallerArg10
  );

  /*
   * Check that the two invocations of rtems_record_caller_arg_10() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 1 );

  /*
   * Check that the items produced by rtems_record_caller_arg_10() have the
   * time stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line() twice in a helper function with the maskable
 *   interrupts disabled.
 */
static void RtemsRecordValRecord_Action_16( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
  };
  void *ra;

  Begin( ctx );
  ra = Line( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line() produced the expected items in
   * the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Line );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, Line );

  /*
   * Check that the two invocations of rtems_record_line() produced distinct
   * event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_2() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_17( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_USER( 1 ), DATA( 0 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Line2( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_2() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Line2 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, Line2 );

  /*
   * Check that the two invocations of rtems_record_line_2() produced distinct
   * event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_2() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_3() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_18( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_USER( 1 ), DATA( 0 ) },
    { RTEMS_RECORD_USER( 2 ), DATA( 1 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Line3( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_3() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Line3 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, Line3 );

  /*
   * Check that the two invocations of rtems_record_line_3() produced distinct
   * event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_3() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_19( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg );

  /*
   * Check that the two invocations of rtems_record_line_arg() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_2() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_20( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg2( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_2() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg2 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg2 );

  /*
   * Check that the two invocations of rtems_record_line_arg_2() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_2() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_3() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_21( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg3( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_3() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg3 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg3 );

  /*
   * Check that the two invocations of rtems_record_line_arg_3() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_3() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_4() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_22( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg4( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_4() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg4 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg4 );

  /*
   * Check that the two invocations of rtems_record_line_arg_4() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_4() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_5() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_23( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg5( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_5() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg5 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg5 );

  /*
   * Check that the two invocations of rtems_record_line_arg_5() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_5() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_6() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_24( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg6( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_6() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg6 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg6 );

  /*
   * Check that the two invocations of rtems_record_line_arg_6() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_6() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_7() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_25( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg7( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_7() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg7 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg7 );

  /*
   * Check that the two invocations of rtems_record_line_arg_7() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_7() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_8() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_26( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg8( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_8() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg8 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg8 );

  /*
   * Check that the two invocations of rtems_record_line_arg_8() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_8() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_9() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_27( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
    { RTEMS_RECORD_ARG_8, DATA( 8 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg9( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_9() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg9 );
  CheckSegment( ctx, 1, expected, RTEMS_ARRAY_SIZE( expected ), ra, LineArg9 );

  /*
   * Check that the two invocations of rtems_record_line_arg_9() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_9() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_line_arg_10() twice in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_28( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_LINE, DATA_LINE },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
    { RTEMS_RECORD_ARG_8, DATA( 8 ) },
    { RTEMS_RECORD_ARG_9, DATA( 9 ) },
  };
  void *ra;

  Begin( ctx );
  ra = LineArg10( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_line_arg_10() produced the expected
   * items in the record buffer of the processor.
   */
  CheckSegment(
    ctx,
    0,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    LineArg10
  );
  CheckSegment(
    ctx,
    1,
    expected,
    RTEMS_ARRAY_SIZE( expected ),
    ra,
    LineArg10
  );

  /*
   * Check that the two invocations of rtems_record_line_arg_10() produced
   * distinct event data for the line item.
   */
  CheckLineDiffers( ctx, 0 );

  /*
   * Check that the items produced by rtems_record_line_arg_10() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
  CheckTime( ctx, 1 );
}

/**
 * @brief Call rtems_record_entry() once in a helper function with the maskable
 *   interrupts disabled.
 */
static void RtemsRecordValRecord_Action_29( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
  };
  void *ra;

  Begin( ctx );
  ra = Entry( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry() produced the expected items in
   * the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry );

  /*
   * Check that the items produced by rtems_record_entry() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_1() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_30( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry1( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_1() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry1 );

  /*
   * Check that the items produced by rtems_record_entry_1() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_2() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_31( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry2( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_2() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry2 );

  /*
   * Check that the items produced by rtems_record_entry_2() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_3() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_32( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry3( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_3() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry3 );

  /*
   * Check that the items produced by rtems_record_entry_3() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_4() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_33( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry4( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_4() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry4 );

  /*
   * Check that the items produced by rtems_record_entry_4() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_5() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_34( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry5( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_5() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry5 );

  /*
   * Check that the items produced by rtems_record_entry_5() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_6() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_35( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry6( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_6() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry6 );

  /*
   * Check that the items produced by rtems_record_entry_6() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_7() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_36( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry7( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_7() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry7 );

  /*
   * Check that the items produced by rtems_record_entry_7() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_8() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_37( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry8( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_8() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry8 );

  /*
   * Check that the items produced by rtems_record_entry_8() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_9() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_38( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
    { RTEMS_RECORD_ARG_8, DATA( 8 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry9( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_9() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry9 );

  /*
   * Check that the items produced by rtems_record_entry_9() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_entry_10() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_39( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 3 ), DATA_RA },
    { RTEMS_RECORD_ARG_0, DATA( 0 ) },
    { RTEMS_RECORD_ARG_1, DATA( 1 ) },
    { RTEMS_RECORD_ARG_2, DATA( 2 ) },
    { RTEMS_RECORD_ARG_3, DATA( 3 ) },
    { RTEMS_RECORD_ARG_4, DATA( 4 ) },
    { RTEMS_RECORD_ARG_5, DATA( 5 ) },
    { RTEMS_RECORD_ARG_6, DATA( 6 ) },
    { RTEMS_RECORD_ARG_7, DATA( 7 ) },
    { RTEMS_RECORD_ARG_8, DATA( 8 ) },
    { RTEMS_RECORD_ARG_9, DATA( 9 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Entry10( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_entry_10() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Entry10 );

  /*
   * Check that the items produced by rtems_record_entry_10() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit() once in a helper function with the maskable
 *   interrupts disabled.
 */
static void RtemsRecordValRecord_Action_40( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
  };
  void *ra;

  Begin( ctx );
  ra = Exit( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit() produced the expected items in
   * the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit );

  /*
   * Check that the items produced by rtems_record_exit() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_1() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_41( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit1( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_1() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit1 );

  /*
   * Check that the items produced by rtems_record_exit_1() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_2() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_42( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit2( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_2() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit2 );

  /*
   * Check that the items produced by rtems_record_exit_2() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_3() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_43( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
    { RTEMS_RECORD_RETURN_2, DATA( 2 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit3( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_3() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit3 );

  /*
   * Check that the items produced by rtems_record_exit_3() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_4() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_44( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
    { RTEMS_RECORD_RETURN_2, DATA( 2 ) },
    { RTEMS_RECORD_RETURN_3, DATA( 3 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit4( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_4() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit4 );

  /*
   * Check that the items produced by rtems_record_exit_4() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_5() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_45( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
    { RTEMS_RECORD_RETURN_2, DATA( 2 ) },
    { RTEMS_RECORD_RETURN_3, DATA( 3 ) },
    { RTEMS_RECORD_RETURN_4, DATA( 4 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit5( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_5() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit5 );

  /*
   * Check that the items produced by rtems_record_exit_5() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_6() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_46( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
    { RTEMS_RECORD_RETURN_2, DATA( 2 ) },
    { RTEMS_RECORD_RETURN_3, DATA( 3 ) },
    { RTEMS_RECORD_RETURN_4, DATA( 4 ) },
    { RTEMS_RECORD_RETURN_5, DATA( 5 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit6( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_6() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit6 );

  /*
   * Check that the items produced by rtems_record_exit_6() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_7() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_47( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
    { RTEMS_RECORD_RETURN_2, DATA( 2 ) },
    { RTEMS_RECORD_RETURN_3, DATA( 3 ) },
    { RTEMS_RECORD_RETURN_4, DATA( 4 ) },
    { RTEMS_RECORD_RETURN_5, DATA( 5 ) },
    { RTEMS_RECORD_RETURN_6, DATA( 6 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit7( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_7() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit7 );

  /*
   * Check that the items produced by rtems_record_exit_7() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_8() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_48( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
    { RTEMS_RECORD_RETURN_2, DATA( 2 ) },
    { RTEMS_RECORD_RETURN_3, DATA( 3 ) },
    { RTEMS_RECORD_RETURN_4, DATA( 4 ) },
    { RTEMS_RECORD_RETURN_5, DATA( 5 ) },
    { RTEMS_RECORD_RETURN_6, DATA( 6 ) },
    { RTEMS_RECORD_RETURN_7, DATA( 7 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit8( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_8() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit8 );

  /*
   * Check that the items produced by rtems_record_exit_8() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_9() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_49( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
    { RTEMS_RECORD_RETURN_2, DATA( 2 ) },
    { RTEMS_RECORD_RETURN_3, DATA( 3 ) },
    { RTEMS_RECORD_RETURN_4, DATA( 4 ) },
    { RTEMS_RECORD_RETURN_5, DATA( 5 ) },
    { RTEMS_RECORD_RETURN_6, DATA( 6 ) },
    { RTEMS_RECORD_RETURN_7, DATA( 7 ) },
    { RTEMS_RECORD_RETURN_8, DATA( 8 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit9( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_9() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit9 );

  /*
   * Check that the items produced by rtems_record_exit_9() have the time stamp
   * read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_exit_10() once in a helper function with the
 *   maskable interrupts disabled.
 */
static void RtemsRecordValRecord_Action_50( RtemsRecordValRecord_Context *ctx )
{
  static const rtems_record_item expected[] = {
    { RTEMS_RECORD_USER( 4 ), DATA_RA },
    { RTEMS_RECORD_RETURN_0, DATA( 0 ) },
    { RTEMS_RECORD_RETURN_1, DATA( 1 ) },
    { RTEMS_RECORD_RETURN_2, DATA( 2 ) },
    { RTEMS_RECORD_RETURN_3, DATA( 3 ) },
    { RTEMS_RECORD_RETURN_4, DATA( 4 ) },
    { RTEMS_RECORD_RETURN_5, DATA( 5 ) },
    { RTEMS_RECORD_RETURN_6, DATA( 6 ) },
    { RTEMS_RECORD_RETURN_7, DATA( 7 ) },
    { RTEMS_RECORD_RETURN_8, DATA( 8 ) },
    { RTEMS_RECORD_RETURN_9, DATA( 9 ) },
  };
  void *ra;

  Begin( ctx );
  ra = Exit10( ctx );
  End( ctx );

  /*
   * Check that the call of rtems_record_exit_10() produced the expected items
   * in the record buffer of the processor.
   */
  CheckSegment( ctx, 0, expected, RTEMS_ARRAY_SIZE( expected ), ra, Exit10 );

  /*
   * Check that the items produced by rtems_record_exit_10() have the time
   * stamp read before the first item.
   */
  CheckTime( ctx, 0 );
}

/**
 * @brief Call rtems_record_interrupt_disable() and
 *   rtems_record_interrupt_enable() in a helper function with the maskable
 *   interrupts enabled.
 */
static void RtemsRecordValRecord_Action_51( RtemsRecordValRecord_Context *ctx )
{
  void *ra;

  ra = InterruptDisableEnable( ctx );

  /*
   * Check that rtems_record_interrupt_disable() disabled the maskable
   * interrupts and returned the previous interrupt level.
   */
  T_true( ctx->enabled_before );
  T_false( ctx->enabled_after_disable );
  T_true( _ISR_Is_enabled( ctx->record_level ) );

  /*
   * Check that rtems_record_interrupt_disable() produced the expected item
   * while the maskable interrupts were disabled.
   */
  CheckSegment(
    ctx,
    0,
    interrupt_disable_items,
    RTEMS_ARRAY_SIZE( interrupt_disable_items ),
    ra,
    InterruptDisableEnable
  );
  CheckTime( ctx, 0 );

  /*
   * Check that rtems_record_interrupt_enable() restored the interrupt level.
   */
  T_true( ctx->enabled_after_enable );

  /*
   * Check that rtems_record_interrupt_enable() produced the expected item
   * directly after the item of rtems_record_interrupt_disable().
   */
  CheckSegment(
    ctx,
    1,
    interrupt_enable_items,
    RTEMS_ARRAY_SIZE( interrupt_enable_items ),
    ra,
    InterruptDisableEnable
  );
  CheckTime( ctx, 1 );
}

/**
 * @brief Prepare a record context with rtems_record_prepare() while the
 *   maskable interrupts are enabled. Add two items with rtems_record_add().
 *   Fetch the items of the processor. Commit the record context with
 *   rtems_record_commit(). Fetch the items of the processor again.
 */
static void RtemsRecordValRecord_Action_52( RtemsRecordValRecord_Context *ctx )
{
  rtems_record_context context;
  rtems_counter_ticks  before;
  rtems_counter_ticks  after;
  Record_Control      *control;
  uint32_t             cpu_index;
  bool                 enabled_before;
  bool                 enabled_after_prepare;
  bool                 enabled_after_commit;
  size_t               hidden_count;

  enabled_before = _ISR_Get_level() == 0;
  before = rtems_counter_read();
  rtems_record_prepare( &context );
  after = rtems_counter_read();
  enabled_after_prepare = _ISR_Get_level() == 0;
  cpu_index = rtems_scheduler_get_processor();
  control = _Per_CPU_Get()->record;
  control->tail = _Record_Head( control );
  rtems_record_add( &context, RTEMS_RECORD_USER( 5 ), DATA( 0 ) );
  rtems_record_add( &context, RTEMS_RECORD_USER( 6 ), DATA( 1 ) );
  hidden_count = Fetch( ctx, cpu_index, FETCH_STORAGE_COUNT );
  rtems_record_commit( &context );
  enabled_after_commit = _ISR_Get_level() == 0;
  (void) Fetch( ctx, cpu_index, FETCH_STORAGE_COUNT );

  /*
   * Check that rtems_record_prepare() disabled the maskable interrupts. Check
   * that it initialized the record context with the record buffer of the
   * current processor, the previous interrupt level, and the time stamp of the
   * counter. This shows that rtems_record_context provided the storage of the
   * record context.
   */
  T_true( enabled_before );
  T_false( enabled_after_prepare );
  T_eq_ptr( context.control, control );
  T_true( _ISR_Is_enabled( context.level ) );
  T_eq_u32( RTEMS_RECORD_GET_EVENT( context.now ), 0 );
  CheckTimeStamp( RTEMS_RECORD_GET_TIME( context.now ), before, after );

  /*
   * Check that the items which rtems_record_add() appended were invisible to
   * rtems_record_fetch() before the commit.
   */
  T_eq_sz( hidden_count, 0 );

  /*
   * Check that rtems_record_commit() made the items visible to
   * rtems_record_fetch() and restored the interrupt level.
   */
  T_true( enabled_after_commit );
  T_ge_sz( ctx->fetch.fetched_count, 3 );

  /*
   * Check that rtems_record_add() appended the items with the time stamp of
   * the record context, the event, and the event data.
   */
  if ( ctx->fetch.fetched_count >= 3 ) {
    const rtems_record_item *items;

    items = &ctx->fetch.fetched_items[ 1 ];
    T_eq_u32( items[ 0 ].event, context.now | RTEMS_RECORD_USER( 5 ) );
    T_eq_ulong( items[ 0 ].data, DATA( 0 ) );
    T_eq_u32( items[ 1 ].event, context.now | RTEMS_RECORD_USER( 6 ) );
    T_eq_ulong( items[ 1 ].data, DATA( 1 ) );
  }
}

/**
 * @brief Disable the maskable interrupts. Prepare a record context with
 *   rtems_record_prepare_critical() for the processor with the highest index.
 *   Add an item with rtems_record_add(). Fetch the items of the processor.
 *   Commit the record context with rtems_record_commit_critical(). Fetch the
 *   items of the processor again. Enable the maskable interrupts.
 */
static void RtemsRecordValRecord_Action_53( RtemsRecordValRecord_Context *ctx )
{
  rtems_record_context   context;
  rtems_interrupt_level  level;
  rtems_counter_ticks    before;
  rtems_counter_ticks    after;
  const Per_CPU_Control *cpu;
  Record_Control        *control;
  uint32_t               cpu_index;
  bool                   enabled_after_commit;
  size_t                 hidden_count;
  size_t                 visible_count;
  rtems_record_item      item;

  cpu_index = rtems_scheduler_get_processor_maximum() - 1;
  cpu = _Per_CPU_Get_by_index( cpu_index );
  control = cpu->record;
  memset( &item, 0, sizeof( item ) );

  rtems_interrupt_local_disable( level );
  control->tail = _Record_Head( control );
  before = rtems_counter_read();
  rtems_record_prepare_critical( &context, cpu );
  after = rtems_counter_read();
  rtems_record_add( &context, RTEMS_RECORD_USER( 7 ), DATA( 0 ) );
  hidden_count = Fetch( ctx, cpu_index, FETCH_STORAGE_COUNT );
  rtems_record_commit_critical( &context );
  enabled_after_commit = _ISR_Get_level() == 0;
  visible_count = Fetch( ctx, cpu_index, FETCH_STORAGE_COUNT );

  if ( visible_count > 0 ) {
    item = ctx->fetch.fetched_items[ 1 ];
  }

  rtems_interrupt_local_enable( level );

  /*
   * Check that rtems_record_prepare_critical() initialized the record context
   * with the record buffer of the processor and the time stamp of the counter.
   */
  T_eq_ptr( context.control, control );
  T_eq_u32( RTEMS_RECORD_GET_EVENT( context.now ), 0 );
  CheckTimeStamp( RTEMS_RECORD_GET_TIME( context.now ), before, after );

  /*
   * Check that the item which rtems_record_add() appended was invisible to
   * rtems_record_fetch() before the commit.
   */
  T_eq_sz( hidden_count, 0 );

  /*
   * Check that rtems_record_commit_critical() made the item visible to
   * rtems_record_fetch() and left the maskable interrupts disabled.
   */
  T_false( enabled_after_commit );
  T_eq_sz( visible_count, 1 );
  T_eq_u32( item.event, context.now | RTEMS_RECORD_USER( 7 ) );
  T_eq_ulong( item.data, DATA( 0 ) );
}

/**
 * @brief Get the record buffer of each configured processor. Disable the
 *   maskable interrupts. Produce three items more than the capacity of the
 *   record buffer of the current processor. Copy the record buffer. Enable the
 *   maskable interrupts.
 */
static void RtemsRecordValRecord_Action_54( RtemsRecordValRecord_Context *ctx )
{
  rtems_interrupt_level  level;
  const Per_CPU_Control *cpu;
  Record_Control        *control;
  unsigned int           capacity;
  unsigned int           head;
  unsigned int           i;
  uint32_t               cpu_index;
  uint32_t               cpu_max;
  bool                   distinct;

  cpu_max = rtems_scheduler_get_processor_maximum();
  distinct = true;

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    uint32_t other;

    ctx->capacity[ cpu_index ] = GetControl( cpu_index )->mask + 1;

    for ( other = 0; other < cpu_index; ++other ) {
      if ( GetControl( other ) == GetControl( cpu_index ) ) {
        distinct = false;
      }
    }
  }

  rtems_interrupt_local_disable( level );
  cpu = _Per_CPU_Get();
  control = cpu->record;
  capacity = control->mask + 1;

  for ( i = 0; i < capacity + 3; ++i ) {
    rtems_record_context context;

    rtems_record_prepare_critical( &context, cpu );
    rtems_record_add( &context, RTEMS_RECORD_USER( 8 ), DATA( i ) );
    rtems_record_commit_critical( &context );
  }

  head = _Record_Head( control );

  for ( i = 0; i < capacity && i < RTEMS_ARRAY_SIZE( ctx->items ); ++i ) {
    ctx->items[ i ] = control->Items[ _Record_Index( control, head + i ) ];
  }

  control->tail = head;
  rtems_interrupt_local_enable( level );

  /*
   * Check that each configured processor has a record buffer of its own with a
   * capacity of CONFIGURE_RECORD_PER_PROCESSOR_ITEMS items.
   */
  T_true( distinct );

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    T_eq_uint( ctx->capacity[ cpu_index ], TEST_RECORD_PER_PROCESSOR_ITEMS );
  }

  /*
   * Check that the record buffer contains the last items in the order of their
   * production. The first three items replaced the oldest items of the full
   * record buffer.
   */
  T_eq_uint( capacity, TEST_RECORD_PER_PROCESSOR_ITEMS );

  for ( i = 0; i < capacity && i < RTEMS_ARRAY_SIZE( ctx->items ); ++i ) {
    T_eq_u32(
      RTEMS_RECORD_GET_EVENT( ctx->items[ i ].event ),
      RTEMS_RECORD_USER( 8 )
    );
    T_eq_ulong( ctx->items[ i ].data, DATA( i + 3 ) );
  }
}

/**
 * @brief Produce an item in the record buffer of the processor with index
 *   zero. Fill a record fetch control with a pattern and initialize it with
 *   rtems_record_fetch_initialize(). Fetch the items.
 */
static void RtemsRecordValRecord_Action_55( RtemsRecordValRecord_Context *ctx )
{
  rtems_interrupt_level     level;
  const Per_CPU_Control    *cpu;
  rtems_record_fetch_status status;
  rtems_record_context      context;

  cpu = _Per_CPU_Get_by_index( 0 );
  rtems_interrupt_local_disable( level );
  cpu->record->tail = _Record_Head( cpu->record );
  rtems_record_prepare_critical( &context, cpu );
  rtems_record_add( &context, RTEMS_RECORD_USER( 9 ), DATA( 0 ) );
  rtems_record_commit_critical( &context );
  rtems_interrupt_local_enable( level );

  memset( &ctx->fetch, 0xa5, sizeof( ctx->fetch ) );
  memset( ctx->fetch_storage, 0, sizeof( ctx->fetch_storage ) );
  rtems_record_fetch_initialize( &ctx->fetch, ctx->fetch_storage, 4 );
  status = rtems_record_fetch( &ctx->fetch );

  /*
   * Check that the call of rtems_record_fetch() fetched the item of the
   * processor with index zero into the array of the record fetch control. This
   * shows that rtems_record_fetch_control provided the storage of the record
   * fetch control.
   */
  T_ne_int( status, RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT );
  T_eq_sz( ctx->fetch.fetched_count, 2 );
  T_ge_uptr(
    (uintptr_t) ctx->fetch.fetched_items,
    (uintptr_t) &ctx->fetch_storage[ 0 ]
  );
  T_le_uptr(
    (uintptr_t) ( ctx->fetch.fetched_items + ctx->fetch.fetched_count ),
    (uintptr_t) &ctx->fetch_storage[ 4 ]
  );

  if ( ctx->fetch.fetched_count == 2 ) {
    T_eq_u32( ctx->fetch.fetched_items[ 0 ].event, RTEMS_RECORD_PROCESSOR );
    T_eq_ulong( ctx->fetch.fetched_items[ 0 ].data, 0 );
    T_eq_u32(
      RTEMS_RECORD_GET_EVENT( ctx->fetch.fetched_items[ 1 ].event ),
      RTEMS_RECORD_USER( 9 )
    );
    T_eq_ulong( ctx->fetch.fetched_items[ 1 ].data, DATA( 0 ) );
  }
}

/**
 * @brief Get the item count for a fetch with
 *   rtems_record_get_item_count_for_fetch(). Fill the record buffer of the
 *   processor with index zero beyond its capacity. Fetch the items with this
 *   item count. Fill the record buffer again. Fetch the items with the item
 *   count minus one.
 */
static void RtemsRecordValRecord_Action_56( RtemsRecordValRecord_Context *ctx )
{
  size_t          count;
  Record_Control *control;
  unsigned int    head_all;
  unsigned int    tail_all;
  unsigned int    head_less;
  unsigned int    tail_less;

  count = rtems_record_get_item_count_for_fetch();
  control = GetControl( 0 );

  Fill( 0 );
  (void) Fetch( ctx, 0, count );
  head_all = _Record_Head( control );
  tail_all = control->tail;

  Fill( 0 );
  (void) Fetch( ctx, 0, count - 1 );
  head_less = _Record_Head( control );
  tail_less = control->tail;
  control->tail = head_less;

  /*
   * Check that one fetch with the item count fetched all items of the record
   * buffer. Check that one fetch with the item count minus one left items in
   * the record buffer.
   */
  T_le_sz( count, FETCH_STORAGE_COUNT );
  T_eq_uint( tail_all, head_all );
  T_ne_uint( tail_less, head_less );
}

/**
 * @brief Produce more items in the record buffer of the current processor than
 *   one fetch of the dump holds. Call rtems_record_dump() with a handler which
 *   collects the output.
 */
static void RtemsRecordValRecord_Action_57( RtemsRecordValRecord_Context *ctx )
{
  ProduceDumpItems( ctx );
  ctx->dump_size = 0;
  ctx->dump_calls = 0;
  ctx->dump_arg_ok = true;
  rtems_record_dump( DumpChunk, ctx );
  StreamInit( &ctx->stream, ctx->dump, ctx->dump_size );

  /*
   * Check that rtems_record_dump() called the handler with the argument.
   */
  T_gt_sz( ctx->dump_calls, 0 );
  T_true( ctx->dump_arg_ok );

  /*
   * Check that the output starts with the stream header.
   */
  CheckFormatAndMagic( &ctx->stream );

  /*
   * Check the content of the stream header. Check that the
   * RTEMS_RECORD_VERSION, the RTEMS_RECORD_PROCESSOR_MAXIMUM, the
   * RTEMS_RECORD_PER_CPU_COUNT, and the RTEMS_RECORD_FREQUENCY items have a
   * time stamp of zero and the specified event data. Check that each string
   * takes exactly the items which its characters need and that zero bytes
   * follow its last character only.
   */
  CheckHeaderItems( &ctx->stream );

  /*
   * Check that the thread names follow the stream header. The name of the
   * runner has more than 31 characters, so the output holds its first 31
   * characters in more than one item. Check that each name takes exactly the
   * items which its characters need and that zero bytes follow its last
   * character only.
   */
  CheckThreadNames( &ctx->stream );

  /*
   * Check that the fetched items of all processors follow the thread names.
   */
  CheckDumpItems( ctx, &ctx->stream );
}

/**
 * @brief Produce more items in the record buffer of the current processor than
 *   one fetch of the dump holds. Call rtems_record_dump_base64() with a
 *   handler which collects the output.
 */
static void RtemsRecordValRecord_Action_58( RtemsRecordValRecord_Context *ctx )
{
  size_t i;
  size_t column;
  bool   lines_ok;

  ProduceDumpItems( ctx );
  ctx->base64_size = 0;
  ctx->base64_arg_ok = true;
  rtems_record_dump_base64( Base64PutChar, ctx );
  lines_ok = true;
  column = 0;

  for ( i = 0; i < ctx->base64_size; ++i ) {
    if ( ctx->base64[ i ] == '\n' ) {
      if ( column != 76 ) {
        lines_ok = false;
      }

      column = 0;
    } else {
      ++column;

      if ( column > 76 ) {
        lines_ok = false;
      }
    }
  }

  ctx->dump_size = Base64Decode(
    ctx->base64,
    ctx->base64_size,
    ctx->dump,
    sizeof( ctx->dump )
  );
  StreamInit( &ctx->stream, ctx->dump, ctx->dump_size );

  /*
   * Check that rtems_record_dump_base64() output a newline character after
   * each 76 characters.
   */
  T_gt_sz( ctx->base64_size, 76 );
  T_true( lines_ok );
  T_lt_sz( column, 76 );

  /*
   * Check that rtems_record_dump_base64() called the handler with the argument
   * and output the base64 encoding of the data of rtems_record_dump().
   */
  T_true( ctx->base64_arg_ok );
  T_ne_sz( ctx->dump_size, SIZE_MAX );
  CheckFormatAndMagic( &ctx->stream );
  CheckHeaderItems( &ctx->stream );
  CheckThreadNames( &ctx->stream );
  CheckDumpItems( ctx, &ctx->stream );
}

/**
 * @brief Get the text of each enumerator of rtems_record_event with
 *   rtems_record_event_text().
 */
static void RtemsRecordValRecord_Action_59( void )
{
  int i;

  /*
   * Check that the text is the name of the enumerator without the
   * `RTEMS_RECORD_` prefix.
   */
  for ( i = 0; i <= RTEMS_RECORD_LAST; ++i ) {
    const char *text;

    text = rtems_record_event_text( (rtems_record_event) i );
    T_not_null( text );

    if ( text == NULL ) {
      continue;
    }

    if ( i < RTEMS_RECORD_USER_0 ) {
      T_eq_str( text, system_event_names[ i ] );
    } else {
      char user[ 16 ];

      UserEventName( user, i - RTEMS_RECORD_USER_0 );
      T_eq_str( text, user );
    }
  }
}

/**
 * @brief Evaluate the constants of the event recording.
 */
static void RtemsRecordValRecord_Action_60( void )
{
  /* No action */

  /*
   * Check the value of RTEMS_RECORD_EVENT_BITS.
   */
  T_eq_int( RTEMS_RECORD_EVENT_BITS, 10 );

  /*
   * Check the value of RTEMS_RECORD_TIME_BITS.
   */
  T_eq_int( RTEMS_RECORD_TIME_BITS, 22 );

  /*
   * Check the value of RTEMS_RECORD_THE_VERSION.
   */
  T_eq_int( RTEMS_RECORD_THE_VERSION, 10 );

  /*
   * Check the value of RTEMS_RECORD_MAGIC.
   */
  T_eq_u32( RTEMS_RECORD_MAGIC, 0x82e14ec1 );

  /*
   * Check the value of RTEMS_RECORD_FORMAT_BE_32.
   */
  T_eq_u32( RTEMS_RECORD_FORMAT_BE_32, 0x33333333 );

  /*
   * Check the value of RTEMS_RECORD_FORMAT_BE_64.
   */
  T_eq_u32( RTEMS_RECORD_FORMAT_BE_64, 0x44444444 );

  /*
   * Check the value of RTEMS_RECORD_FORMAT_LE_32.
   */
  T_eq_u32( RTEMS_RECORD_FORMAT_LE_32, 0x11111111 );

  /*
   * Check the value of RTEMS_RECORD_FORMAT_LE_64.
   */
  T_eq_u32( RTEMS_RECORD_FORMAT_LE_64, 0x22222222 );

  /*
   * Check the value of RTEMS_RECORD_LAST.
   */
  T_eq_int( RTEMS_RECORD_LAST, RTEMS_RECORD_USER_511 );

  /*
   * Check the value of RTEMS_RECORD_NO_TIME_LAST.
   */
  T_eq_int( RTEMS_RECORD_NO_TIME_LAST, RTEMS_RECORD_VERSION_CONTROL_KEY );

  /*
   * Check the value of RTEMS_RECORD_EMPTY.
   */
  T_eq_int( RTEMS_RECORD_EMPTY, 0 );

  /*
   * Check the value of RTEMS_RECORD_CONNECT_ENTRY.
   */
  T_eq_int( RTEMS_RECORD_CONNECT_ENTRY, 42 );

  /*
   * Check the value of RTEMS_RECORD_USER_511.
   */
  T_eq_int( RTEMS_RECORD_USER_511, 1023 );
}

/**
 * @brief Evaluate the macros of the event recording.
 */
static void RtemsRecordValRecord_Action_61( void )
{
  uint32_t time_event;

  time_event = RTEMS_RECORD_TIME_EVENT(
    UINT32_C( 0x2abcde ),
    RTEMS_RECORD_USER( 5 )
  );

  /*
   * Check that RTEMS_RECORD_TIME_EVENT() evaluated to the time event with the
   * time stamp in the upper bits and the event in the lower bits.
   */
  T_eq_u32( time_event, 0xaaf37a05 );

  /*
   * Check that RTEMS_RECORD_GET_TIME() evaluates to the time stamp of the time
   * event.
   */
  T_eq_u32( RTEMS_RECORD_GET_TIME( UINT32_C( 0xaaf37a05 ) ), 0x2abcde );

  /*
   * Check that RTEMS_RECORD_GET_EVENT() evaluates to the event of the time
   * event.
   */
  T_eq_u32( RTEMS_RECORD_GET_EVENT( UINT32_C( 0xaaf37a05 ) ), 0x205 );

  /*
   * Check that RTEMS_RECORD_USER() evaluates to the user event of the index.
   */
  T_eq_int( RTEMS_RECORD_USER( 0 ), RTEMS_RECORD_USER_0 );
  T_eq_int( RTEMS_RECORD_USER( 5 ), 517 );
  T_eq_int( RTEMS_RECORD_USER( 511 ), RTEMS_RECORD_USER_511 );
}

/**
 * @brief Evaluate the types of the event recording.
 */
static void RtemsRecordValRecord_Action_62( void )
{
  rtems_record_fetch_status status[ 3 ];

  status[ 0 ] = RTEMS_RECORD_FETCH_DONE;
  status[ 1 ] = RTEMS_RECORD_FETCH_CONTINUE;
  status[ 2 ] = RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT;

  /*
   * Check that rtems_record_data is an unsigned integer type which can hold a
   * value of the type `uintptr_t`.
   */
  T_ge_sz( sizeof( rtems_record_data ), sizeof( uintptr_t ) );
  T_gt_ulong( (rtems_record_data) -1, 0 );
  T_eq_uptr( (uintptr_t) (rtems_record_data) UINTPTR_MAX, UINTPTR_MAX );

  /*
   * Check the members and the layout of rtems_record_item.
   */
  T_true( IS_TYPE( ( (rtems_record_item *) 0 )->event, uint32_t ) );
  T_true( IS_TYPE( ( (rtems_record_item *) 0 )->data, rtems_record_data ) );
  T_eq_sz( offsetof( rtems_record_item, event ), 0 );
  T_eq_sz( offsetof( rtems_record_item, data ), 4 );
  T_eq_sz( sizeof( rtems_record_item ), 4 + sizeof( rtems_record_data ) );

  /*
   * Check the members and the layout of rtems_record_item_32.
   */
  T_true( IS_TYPE( ( (rtems_record_item_32 *) 0 )->event, uint32_t ) );
  T_true( IS_TYPE( ( (rtems_record_item_32 *) 0 )->data, uint32_t ) );
  T_eq_sz( offsetof( rtems_record_item_32, event ), 0 );
  T_eq_sz( offsetof( rtems_record_item_32, data ), 4 );
  T_eq_sz( sizeof( rtems_record_item_32 ), 8 );

  /*
   * Check the members and the layout of rtems_record_item_64.
   */
  T_true( IS_TYPE( ( (rtems_record_item_64 *) 0 )->event, uint32_t ) );
  T_true( IS_TYPE( ( (rtems_record_item_64 *) 0 )->data, uint64_t ) );
  T_eq_sz( offsetof( rtems_record_item_64, event ), 0 );
  T_eq_sz( offsetof( rtems_record_item_64, data ), 4 );
  T_eq_sz( sizeof( rtems_record_item_64 ), 12 );

  /*
   * Check that rtems_record_dump_chunk is a pointer to a function with the
   * specified parameters which returns no value.
   */
  T_true( IS_TYPE(
    (rtems_record_dump_chunk) NULL,
    void ( * )( void *, const void *, size_t )
  ) );

  /*
   * Check that the enumerators of rtems_record_fetch_status have distinct
   * values.
   */
  T_ne_int( status[ 0 ], status[ 1 ] );
  T_ne_int( status[ 0 ], status[ 2 ] );
  T_ne_int( status[ 1 ], status[ 2 ] );
}

/**
 * @brief Get the items which the system produced during the initialization of
 *   the device drivers.
 */
static void RtemsRecordValRecord_Action_63( void )
{
  /* The system initialization collected the items */

  /*
   * Check that the system produced the uptime items with an uptime between the
   * uptimes before and after the production.
   */
  T_eq_sz( uptime_init.count, 2 );

  if ( uptime_init.count == 2 ) {
    CheckUptimePair(
      &uptime_init.items[ 0 ],
      uptime_init.before,
      uptime_init.after
    );
  }
}

/**
 * @brief Insert the uptime watchdogs again. Wait for two times the specified
 *   interval of the periodic uptime items plus two clock ticks. Remove the
 *   uptime watchdogs. Fetch the items of each processor.
 */
static void RtemsRecordValRecord_Action_64( RtemsRecordValRecord_Context *ctx )
{
  sbintime_t     before;
  sbintime_t     after;
  rtems_interval interval;
  uint32_t       cpu_index;
  uint32_t       cpu_max;

  cpu_max = rtems_scheduler_get_processor_maximum();

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    Record_Control *control;

    control = GetControl( cpu_index );
    control->tail = _Record_Head( control );
  }

  interval = UptimeInterval();

  before = _Timecounter_Sbinuptime();
  InsertUptimeWatchdogs( ctx );
  (void) rtems_task_wake_after( 2 * interval + 2 );
  RemoveUptimeWatchdogs( ctx );
  after = _Timecounter_Sbinuptime();

  /*
   * Check that each processor produced at least two uptime item pairs. Check
   * that the uptimes of consecutive pairs differ by the specified interval.
   */
  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    CheckPeriodicUptime( ctx, cpu_index, before, after );
  }
}

/**
 * @brief Create and delete a task. Install an interrupt entry for a testable
 *   interrupt vector, raise the vector, and remove the entry. Count the
 *   RTEMS_RECORD_THREAD_CREATE and the RTEMS_RECORD_INTERRUPT_ENTRY items in
 *   the record buffers of all processors.
 */
static void RtemsRecordValRecord_Action_65( RtemsRecordValRecord_Context *ctx )
{
  rtems_status_code          sc;
  rtems_id                   id;
  rtems_interrupt_attributes required;
  rtems_vector_number        vector;
  rtems_interrupt_entry      entry;

  ClearRecordBuffers();
  sc = rtems_task_create(
    rtems_build_name( 'T', 'A', 'S', 'K' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  T_assert_rsc_success( sc );
  sc = rtems_task_delete( id );
  T_rsc_success( sc );

  memset( &required, 0, sizeof( required ) );
  required.can_raise = true;
  vector = GetTestableInterruptVector( &required );
  T_assert_ne_u32( vector, UINT32_MAX );
  rtems_interrupt_entry_initialize( &entry, ClearInterrupt, &vector, "Test" );
  sc = rtems_interrupt_entry_install( vector, RTEMS_INTERRUPT_SHARED, &entry );
  T_assert_rsc_success( sc );
  (void) rtems_interrupt_vector_enable( vector );
  sc = RaiseSoftwareInterrupt( vector );
  T_rsc_success( sc );
  (void) rtems_interrupt_vector_disable( vector );
  sc = rtems_interrupt_entry_remove( vector, &entry );
  T_rsc_success( sc );

  ctx->thread_create_items = CountItems( RTEMS_RECORD_THREAD_CREATE );
  ctx->interrupt_entry_items = CountItems( RTEMS_RECORD_INTERRUPT_ENTRY );

  /*
   * Check that the system produced no RTEMS_RECORD_THREAD_CREATE item.
   */
  T_eq_sz( ctx->thread_create_items, 0 );

  /*
   * Check that the system produced no RTEMS_RECORD_INTERRUPT_ENTRY item.
   */
  T_eq_sz( ctx->interrupt_entry_items, 0 );
}

/**
 * @fn void T_case_body_RtemsRecordValRecord( void )
 */
T_TEST_CASE_FIXTURE( RtemsRecordValRecord, &RtemsRecordValRecord_Fixture )
{
  RtemsRecordValRecord_Context *ctx;

  ctx = T_fixture_context();

  RtemsRecordValRecord_Action_0( ctx );
  RtemsRecordValRecord_Action_1( ctx );
  RtemsRecordValRecord_Action_2( ctx );
  RtemsRecordValRecord_Action_3( ctx );
  RtemsRecordValRecord_Action_4( ctx );
  RtemsRecordValRecord_Action_5( ctx );
  RtemsRecordValRecord_Action_6( ctx );
  RtemsRecordValRecord_Action_7( ctx );
  RtemsRecordValRecord_Action_8( ctx );
  RtemsRecordValRecord_Action_9( ctx );
  RtemsRecordValRecord_Action_10( ctx );
  RtemsRecordValRecord_Action_11( ctx );
  RtemsRecordValRecord_Action_12( ctx );
  RtemsRecordValRecord_Action_13( ctx );
  RtemsRecordValRecord_Action_14( ctx );
  RtemsRecordValRecord_Action_15( ctx );
  RtemsRecordValRecord_Action_16( ctx );
  RtemsRecordValRecord_Action_17( ctx );
  RtemsRecordValRecord_Action_18( ctx );
  RtemsRecordValRecord_Action_19( ctx );
  RtemsRecordValRecord_Action_20( ctx );
  RtemsRecordValRecord_Action_21( ctx );
  RtemsRecordValRecord_Action_22( ctx );
  RtemsRecordValRecord_Action_23( ctx );
  RtemsRecordValRecord_Action_24( ctx );
  RtemsRecordValRecord_Action_25( ctx );
  RtemsRecordValRecord_Action_26( ctx );
  RtemsRecordValRecord_Action_27( ctx );
  RtemsRecordValRecord_Action_28( ctx );
  RtemsRecordValRecord_Action_29( ctx );
  RtemsRecordValRecord_Action_30( ctx );
  RtemsRecordValRecord_Action_31( ctx );
  RtemsRecordValRecord_Action_32( ctx );
  RtemsRecordValRecord_Action_33( ctx );
  RtemsRecordValRecord_Action_34( ctx );
  RtemsRecordValRecord_Action_35( ctx );
  RtemsRecordValRecord_Action_36( ctx );
  RtemsRecordValRecord_Action_37( ctx );
  RtemsRecordValRecord_Action_38( ctx );
  RtemsRecordValRecord_Action_39( ctx );
  RtemsRecordValRecord_Action_40( ctx );
  RtemsRecordValRecord_Action_41( ctx );
  RtemsRecordValRecord_Action_42( ctx );
  RtemsRecordValRecord_Action_43( ctx );
  RtemsRecordValRecord_Action_44( ctx );
  RtemsRecordValRecord_Action_45( ctx );
  RtemsRecordValRecord_Action_46( ctx );
  RtemsRecordValRecord_Action_47( ctx );
  RtemsRecordValRecord_Action_48( ctx );
  RtemsRecordValRecord_Action_49( ctx );
  RtemsRecordValRecord_Action_50( ctx );
  RtemsRecordValRecord_Action_51( ctx );
  RtemsRecordValRecord_Action_52( ctx );
  RtemsRecordValRecord_Action_53( ctx );
  RtemsRecordValRecord_Action_54( ctx );
  RtemsRecordValRecord_Action_55( ctx );
  RtemsRecordValRecord_Action_56( ctx );
  RtemsRecordValRecord_Action_57( ctx );
  RtemsRecordValRecord_Action_58( ctx );
  RtemsRecordValRecord_Action_59();
  RtemsRecordValRecord_Action_60();
  RtemsRecordValRecord_Action_61();
  RtemsRecordValRecord_Action_62();
  RtemsRecordValRecord_Action_63();
  RtemsRecordValRecord_Action_64( ctx );
  RtemsRecordValRecord_Action_65( ctx );
}

/** @} */
