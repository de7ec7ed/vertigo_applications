/* This file is part of VERTIGO.
 *
 * (C) Copyright 2013, Siege Technologies <http://www.siegetechnologies.com>
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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <types.h>
#include <armlib/vmsa/tt.h>
#include <armlib/vmsa/gen.h>
#include <hdrlib/mod.h>

void usage(int argc, char *argv[]);
result_t copy_module_header(size_t index, mod_header_t *hdr, size_t size);
int main(int argc, char *argv[]);

// Missing system ldr prototype
extern result_t ldr_call_copy_module_header(size_t index, mod_header_t *hdr, size_t size);

#endif //__MAIN_H__
