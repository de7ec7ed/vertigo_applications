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

.align 4
.globl _tt_get_ttbr0
_tt_get_ttbr0:
	mrc p15, 0, r0, c2, c0, 0 // Translation Table Base Register 0
	mov pc, lr

.globl _tt_get_ttbr1
_tt_get_ttbr1:
	mrc p15, 0, r0, c2, c0, 1 // Translation Table Base Register 1
	mov pc, lr

.globl _tt_get_ttbrc
_tt_get_ttbrc:
	mrc p15, 0, r0, c2, c0, 2 // Translatio Table Control Register
	mov pc, lr

.globl _gen_privileged_read_translation
_gen_privileged_read_translation:
	mcr p15, 0, r0, c7, c8, 0 // Privileged Read VA to PA translation
	mov pc, lr

.globl _gen_get_par
_gen_get_par:
	mrc p15, 0, r0, c7, c4, 0 // Physical Address Register (Translation result)
	mov pc, lr

.globl _gen_set_par
_gen_set_par:
    mcr p15, 0, r0, c7, c4, 0 // Physical Address Register (Translation result)
    mov pc, lr

.globl _tlb_invalidate_entire_unified_tlb
_tlb_invalidate_entire_unified_tlb:
	mcr p15, 0, r0, c8, c7, 0 // Invalidate unified TLB
	mov pc, lr

mask_1k:  .word 0x3FF
mask_32k: .word 0x7FFF

// TODO: this function will need to be renamed and the
// prototype removed from main.h as soon as it has been
// migrated to the armlib.
.globl _cac_flush_entire_data_cache
_cac_flush_entire_data_cache:
	push {r0 - r12}
	dmb                                // ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1          // read clidr
	ands	r3, r0, #0x7000000         // extract loc from clidr
	mov	r3, r3, lsr #23                // left align loc bit field
	beq	finished                       // if loc is 0, then no need to clean
	mov	r10, #0                        // start clean at cache level 0
	loop1:
	add	r2, r10, r10, lsr #1           // work out 3x current cache level
	mov	r1, r0, lsr r2                 // extract cache type bits from clidr
	and	r1, r1, #7                     // mask of the bits for current cache only
	cmp	r1, #2                         // see what cache we have at this level
	blt	skip                           // skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0         // select current cache level in cssr
	isb                                // isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0          // read the new csidr
	and	r2, r1, #7                     // extract the length of the cache lines
	add	r2, r2, #4                     // add 4 (line length offset)
	ldr	r4, mask_1k
	ands	r4, r4, r1, lsr #3         // find maximum number on the way size
	clz	r5, r4                         // find bit position of way size increment
	ldr	r7, mask_32k
	ands	r7, r7, r1, lsr #13        // extract max number of the index size
	loop2:
	mov	r9, r4                         // create working copy of max way size
	loop3:
	orr	r11, r10, r9, lsl r5           // factor way and cache number into r11
	orr	r11, r11, r7, lsl r2           // factor index number into r11
	mcr	p15, 0, r11, c7, c14, 2        // clean & invalidate by set/way
	subs	r9, r9, #1                 // decrement the way
	bge	loop3
	subs	r7, r7, #1                 // decrement the index
	bge	loop2
	skip:
	add	r10, r10, #2                   // increment cache number
	cmp	r3, r10
	bgt	loop1
	finished:
	mov	r10, #0                        // swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0         // select current cache level in cssr
	dsb
	isb
	pop {r0 - r12}
	mov	pc, lr
