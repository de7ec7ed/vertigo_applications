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

// System Includes
#include <stdlib.h>
#include <stdio.h>

// Global Includes
#include <defines.h>
#include <types.h>

// Local Includes
#include <check.h>
#include <main.h>

#define FLAT_BINARY_FILE_MAX_SIZE (ONE_MEGABYTE * 4)

void usage(int argc, char *argv[]) {

	printf("%s [string]\n", argv[0]);

	return;
}

result_t remove_module(u8_t *string, size_t argc, u8_t *argv[]) {

	size_t result;

	result = ldr_call_remove_module(string, argc, argv);

	return result;
}

int main(int argc, char *argv[]) {

	printf("\t------------------------------\n");
	printf("\t-    v*v  0xde7ec7ed  v*v    -\n");
	printf("\t-    *^*  iOS  rmmod  *^*    -\n");
	printf("\t------------------------------\n");

	if(argc < 2) {
		usage(argc, argv);
		return FAILURE;
	}

	CHECK_SUCCESS(remove_module((u8_t *)(argv[1]), (argc - 1), (u8_t **)&argv[1]), "[-] unable to remove the module\n");

	printf("[+] module successfully removed\n");

	return SUCCESS;
}
