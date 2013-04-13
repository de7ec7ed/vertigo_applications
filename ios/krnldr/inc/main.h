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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <types.h>
#include <armv7lib/vmsa/tt.h>
#include <armv7lib/vmsa/gen.h>

// SVC Mode Functions
result_t neuter_xn(void *fb, size_t size);
int system_call(void *p, void *params, size_t *ret_val);

// USR Mode Functions
size_t cross_usr_svc_mode_boundary(void *fb, size_t size);
result_t print_flat_binary(void *fb, size_t size, size_t limit);
result_t load_flat_binary(char *file, void **fb, size_t *size);
result_t patch_flat_binary_usr_mode(void *fb, size_t size);
result_t patch_flat_binary_svc_mode(void *fb, size_t size);
void usage(int argc, char *argv[]);
int main(int argc, char *argv[]);

// Missing system ldr prototype
extern result_t ldr_call_add_module(void *pointer, size_t size);


// Missing ioctl prototype
extern int ioctl(int fd, int command, ...);

void flush_entire_data_cache();

#endif //__MAIN_H__
