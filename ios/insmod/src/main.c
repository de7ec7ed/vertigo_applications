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

	printf("%s [file]\n", argv[0]);

	return;
}

result_t add_module(char *file, size_t argc, u8_t *argv[]) {

	FILE *fp;
	void *tmp;
	size_t size;
	size_t result;

	printf("[%%] file: %s\n", file);

	tmp = malloc(FLAT_BINARY_FILE_MAX_SIZE);

	CHECK_NOT_NULL(tmp, "[-] unable to malloc space for the tmp module buffer\n");

	fp = fopen(file, "r");

	CHECK_NOT_NULL(fp, "[-] unable to open file\n");

	size = fread(tmp, sizeof(u8_t), FLAT_BINARY_FILE_MAX_SIZE, fp);

	if(size <= 0) {
		printf("[-] unable to read file\n");
		return FAILURE;
	}

	result = ldr_call_add_module(tmp, size, argc, argv);

	free(tmp);

	return result;
}

int main(int argc, char *argv[]) {

	printf("\t------------------------------\n");
	printf("\t-    v*v  0xde7ec7ed  v*v    -\n");
	printf("\t-    *^*  iOS insmod  *^*    -\n");
	printf("\t------------------------------\n");

	if(argc < 2) {
		usage(argc, argv);
		return FAILURE;
	}

	CHECK_SUCCESS(add_module(argv[1], (argc - 1), (u8_t **)&(argv[1])), "[-] unable to add the module\n");

	printf("[+] module successfully added\n");

	return SUCCESS;
}
