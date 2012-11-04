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
