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

// Local Includes
#include <check.h>
#include <main.h>


#define FLAT_BINARY_FILE_MAX_SIZE (ONE_MEGABYTE * 4)

void usage(int argc, char *argv[]) {

	printf("%s [file]\n", argv[0]);

	return;
}

int main(int argc, char *argv[]) {

	size_t size;
	size_t value;
	size_t i;
	u8_t *buffer;

	printf("\t------------------------------\n");
	printf("\t-  v*v     de7ec7ed     v*v  -\n");
	printf("\t-  *^*  android dmesg   *^*  -\n");
	printf("\t------------------------------\n");

	log_call_init();
	size = log_call_get_size();

	buffer = malloc(size);

	if(buffer == NULL) {
		return FAILURE;
	}

	memset(buffer, 0, size);

	for(i = 0; i < size; i += sizeof(value)) {
		value = log_call_get_value();
		if((size - i) < sizeof(value)) {
			memcpy(&(buffer[i]), &value, (size - i));
		}
		else {
			memcpy(&(buffer[i]), &value, sizeof(value));
		}
	}

	log_call_fini();

	printf("%s", buffer);

	printf("[+] messages successfully displayed\n");

	return SUCCESS;
}
