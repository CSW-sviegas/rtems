/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012, 2017 embedded brains GmbH & Co. KG
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

#include <bspopts.h>

#include <rtems/score/percpu.h>
#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#define VECTOR_TABLE_ENTRY_SIZE 32

.macro EXCEPTION_PROLOGUE_FATAL exception_number, handler
	.balign VECTOR_TABLE_ENTRY_SIZE
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, PPC_EXC_GPR3_PROLOGUE_OFFSET(r1)
	addi	r3, r1, EXC_GENERIC_SIZE
	PPC_REG_STORE	r3, PPC_EXC_GPR1_PROLOGUE_OFFSET(r1)
	li	r3, \exception_number
	b	\handler
.endm

.macro EXCEPTION_PROLOGUE_INTERRUPT exception_number
	.balign VECTOR_TABLE_ENTRY_SIZE
	PPC_REG_STORE_UPDATE	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	PPC_REG_STORE	r3, PPC_EXC_GPR3_PROLOGUE_OFFSET(r1)
	li	r3, \exception_number
	b	ppc_exc_interrupt
.endm

	.globl _start
	.globl bsp_exc_vector_base

	.section ".bsp_start_text", "ax"

	/* Primitive NULL pointer protection */
.rept 1024
	sc
.endr

_start:
	/* Enable time base */
	li	r0, 0x4000
	mtspr	HID0, r0

	/* Initialize start stack */
	LA	r1, _ISR_Stack_area_end
	subi	r1, r1, 16
	li	r0, 0
	stw	r0, 0(r1)

	SET_SELF_CPU_CONTROL	r3, r4

	/* Copy fast text */
	LWI	r3, bsp_section_fast_text_begin
	LWI	r4, bsp_section_fast_text_load_begin
	LWI	r5, bsp_section_fast_text_size
	bl	copy

	/* Copy read-only data */
	LWI	r3, bsp_section_rodata_begin
	LWI	r4, bsp_section_rodata_load_begin
	LWI	r5, bsp_section_rodata_size
	bl	copy

	/* Copy fast data */
	LWI	r3, bsp_section_fast_data_begin
	LWI	r4, bsp_section_fast_data_load_begin
	LWI	r5, bsp_section_fast_data_size
	bl	copy

	/* Copy data */
	LWI	r3, bsp_section_data_begin
	LWI	r4, bsp_section_data_load_begin
	LWI	r5, bsp_section_data_size
	bl	copy

	/* Clear SBSS */
	LWI	r3, bsp_section_sbss_begin
	LWI	r4, bsp_section_sbss_size
	bl	bsp_start_zero

	/* Clear BSS */
	LWI	r3, bsp_section_bss_begin
	LWI	r4, bsp_section_bss_size
	bl	bsp_start_zero

	/* Set up EABI and SYSV environment */
	bl	__eabi

	/* Clear command line */
	li	r3, 0

	bl	boot_card

twiddle:
	b	twiddle

copy:
	cmpw	r3, r4
	beqlr
	b	memcpy

	/* Exception vector prologues area */
	.section ".bsp_start_text", "ax"
	.balign VECTOR_TABLE_ENTRY_SIZE
bsp_exc_vector_base:
	/* Critical input */
	EXCEPTION_PROLOGUE_FATAL	0, ppc_exc_fatal_critical
	/* Machine check */
	EXCEPTION_PROLOGUE_FATAL	1, ppc_exc_fatal_machine_check
	/* Data storage */
	EXCEPTION_PROLOGUE_FATAL	2, ppc_exc_fatal_normal
	/* Instruction storage */
	EXCEPTION_PROLOGUE_FATAL	3, ppc_exc_fatal_normal
	/* External input */
	EXCEPTION_PROLOGUE_INTERRUPT	4
	/* Alignment */
	EXCEPTION_PROLOGUE_FATAL	5, ppc_exc_fatal_normal
	/* Program */
	EXCEPTION_PROLOGUE_FATAL	6, ppc_exc_fatal_normal
	/* Floating-point unavailable */
	EXCEPTION_PROLOGUE_FATAL	7, ppc_exc_fatal_normal
	/* System call */
	EXCEPTION_PROLOGUE_FATAL	8, ppc_exc_fatal_normal
	/* APU unavailable */
	EXCEPTION_PROLOGUE_FATAL	9, ppc_exc_fatal_normal
	/* Decrementer */
	EXCEPTION_PROLOGUE_INTERRUPT	10
	/* Fixed-interval timer interrupt */
	EXCEPTION_PROLOGUE_FATAL	11, ppc_exc_fatal_normal
	/* Watchdog timer interrupt */
	EXCEPTION_PROLOGUE_FATAL	12, ppc_exc_fatal_critical
	/* Data TLB error */
	EXCEPTION_PROLOGUE_FATAL	13, ppc_exc_fatal_normal
	/* Instruction TLB error */
	EXCEPTION_PROLOGUE_FATAL	14, ppc_exc_fatal_normal
	/* Debug */
	EXCEPTION_PROLOGUE_FATAL	15, ppc_exc_fatal_debug
	/* SPE APU unavailable or AltiVec unavailable */
	EXCEPTION_PROLOGUE_FATAL	32, ppc_exc_fatal_normal
	/* SPE floating-point data exception or AltiVec assist */
	EXCEPTION_PROLOGUE_FATAL	33, ppc_exc_fatal_normal
	/* SPE floating-point round exception */
	EXCEPTION_PROLOGUE_FATAL	34, ppc_exc_fatal_normal
	/* Performance monitor */
	EXCEPTION_PROLOGUE_FATAL	35, ppc_exc_fatal_normal
