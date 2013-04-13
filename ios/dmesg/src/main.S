/* This file is part of VERTIGO.
 *
 * (C) Copyright 2013, Siege Technologies <http://www.siegetechnologies.com>
 * (C) Copyright 2013, Kirk Swidowski <http://www.swidowski.com>
 *
 * VERTIGO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VERTIGO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VERTIGO. If not, see <http://www.gnu.org/licenses/>.
 *
 * Written by Kirk Swidowski <kirk@swidowski.com>
 */

#include <defines.h>

callsign: .word CALLSIGN
log_call_identifier: .word 0x00000000

log_init_function: .word 0

.align 4
.globl _log_call_init
_log_call_init:
	push {r1 - r2}
	ldr r0, callsign
	ldr r1, log_call_identifier
	ldr r2, log_init_function
	mcr	p0, 0, r0, c0, c0, 0
	pop {r1 - r2}
	mov pc, lr

log_fini_function: .word 3

.align 4
.globl _log_call_fini
_log_call_fini:
	push {r1 - r2}
	ldr r0, callsign
	ldr r1, log_call_identifier
	ldr r2, log_fini_function
	mcr	p0, 0, r0, c0, c0, 0
	pop {r1 - r2}
	mov pc, lr

log_get_value_function: .word 2

.align 4
.globl _log_call_get_value
_log_call_get_value:
	push {r1 - r2}
	ldr r0, callsign
	ldr r1, log_call_identifier
	ldr r2, log_get_value_function
	mcr	p0, 0, r0, c0, c0, 0
	mov r0, r1
	pop {r1 - r2}
	mov pc, lr

log_get_size_function: .word 1

.align 4
.globl _log_call_get_size
_log_call_get_size:
	push {r1 - r2}
	ldr r0, callsign
	ldr r1, log_call_identifier
	ldr r2, log_get_size_function
	mcr	p0, 0, r0, c0, c0, 0
	mov r0, r1
	pop {r1 - r2}
	mov pc, lr
