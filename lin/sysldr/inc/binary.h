/* This file is part of VERTIGO.
 *
 * (C) Copyright 2013, Siege Technologies <siegetechnologies.com>
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

#ifndef __BINARY_H__
#define __BINARY_H__

#include <defines.h>
#include <types.h>

#define BINARY_DEFAULT_BUFFER_SIZE ONE_MEGABYTE

result_t binary_load(const u8_t *path_and_name, void **buffer, size_t *size);
result_t binary_neuter_xn(void *fb, size_t size);
result_t binary_patch(void *buffer);
result_t binary_execute(void *buffer);
result_t binary_print(void *buffer, size_t size);
result_t binary_unload(void *buffer);

#endif //__BINARY_H__
