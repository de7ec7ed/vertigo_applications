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
ldr_call_identifier: .word 0x11111111
add_module: .word 0  //LDR_ADD_MODULE

.align 4
.globl ldr_call_add_module
ldr_call_add_module:
	push {r1 - r6}
	push {r0 - r3}
	ldr r0, callsign
	ldr r1, ldr_call_identifier
	ldr r2, add_module
	pop {r3 - r6}
	mcr	p0, 0, r0, c0, c0, 0
	pop {r1 - r6}
	mov pc, lr

