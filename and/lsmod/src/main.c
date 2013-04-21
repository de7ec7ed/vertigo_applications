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
#include <string.h>

// Global Includes
#include <defines.h>
#include <types.h>
#include <hdrlib/mod.h>

// Local Includes
#include <check.h>
#include <main.h>

#define FLAT_BINARY_FILE_MAX_SIZE (ONE_MEGABYTE * 4)

void usage(int argc, char *argv[]) {

	printf("%s\n", argv[0]);

	return;
}

result_t copy_module_header(size_t index, mod_header_t *hdr, size_t size) {

	size_t result;

	result = ldr_call_copy_module_header(index, hdr, size);

	return result;
}

int main(int argc, char *argv[]) {

	mod_header_t *hdr;
	size_t size = 256;
	size_t index = 0;

	printf("\t------------------------------\n");
	printf("\t-    v*v    de7ec7ed   v*v    -\n");
	printf("\t-    *^* Android lsmod *^*    -\n");
	printf("\t------------------------------\n");

	hdr = malloc(size);

	if(argc != 1) {
		usage(argc, argv);
		return FAILURE;
	}

	printf("name\t\timport\texport\tinit\tfini\n");

	while(copy_module_header(index, hdr, size) != FAILURE) {
		printf("%-10s\t0x%04x\t0x%04x\t0x%04x\t0x%04x\n", hdr->string, (unsigned int)(hdr->import), (unsigned int)(hdr->export), (unsigned int)(hdr->init), (unsigned int)(hdr->fini));
		memset(hdr, 0, size);
		index++;
	}

	printf("[+] module successfully listed\n");

	free(hdr);

	return SUCCESS;
}
