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

#ifndef __MAIN_H__
#define __MAIN_H__

#define MAIN__DEFAULT_BUFFER_SIZE 512
#define MAIN__DEFAULT_BUFFER_THRESHOLD 128

//#define MAIN__FILE_TO_LOAD "/data/data/siege.technologies.project/hypervisor.bin"

#define MAIN__FILE_TO_LOAD "trebuchet.bin"

static int __init main_init(void);

static void __exit main_exit(void);

#endif //__MAIN_H__
