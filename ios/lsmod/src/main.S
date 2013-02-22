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

copy_module_header: .word 2//LDR_COPY_MODULE_HEADER

.globl _ldr_call_copy_module_header
_ldr_call_copy_module_header:
	push {r1 - r4}
	push {r0 - r2}
	ldr r0, callsign
	ldr r1, copy_module_header
	pop {r2 - r4}
	mcr	p0, 0, r0, c0, c0, 0
	pop {r1 - r4}
	mov pc, lr
